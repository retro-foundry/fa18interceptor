/* CPU bus timing: what the 68000 waits for on an A500.
 *
 * Chip RAM, Slow RAM ("trapdoor" memory, on the chip bus) and the custom
 * registers are shared with DMA. Each colour clock (CCK, two CPU cycles) of a
 * line belongs to at most one bus master; DMA always wins. A CPU word access
 * waits for the next CCK that no DMA channel owns, as in UAE's cycle-exact
 * dma_cycle(). CIA accesses synchronise to the E clock instead.
 *
 * Musashi performs every access of an instruction at the instruction's start
 * and charges the instruction's cycles at its end, so accesses are placed
 * four cycles apart from the instruction start (the 68000 bus cycle). Waits
 * are charged at once, which moves every later access too.
 *
 * DMA slots per line come from refresh, bitplane fetch and Copper activity,
 * recorded when the line starts. A running blit owns its own timeline: each
 * CCK it is not displaced by other DMA it uses for one step of its cycle
 * diagram; with BLTPRI set the CPU gets only the diagram's idle steps. Lines
 * that have not started yet in this frame are predicted from the previous
 * frame. Interpreter and generated code make the same accesses in the same
 * order, so both see identical timing. */
#include "bus.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m68kcpu.h"
#include "m68kops.h"
#include "../os/rom_audit_adapter.h"
#include "recomp_runtime.h"

FA18EmulationMeter fa18_emulation_meter;
int fa18_meter_enabled, fa18_meter_engine;
/* Physical 4 KiB RAM pages; mirrors share the underlying Chip RAM page.
 * A page hit records each overlapping API access, not bytes or DMA slots.
 * Shared-page evidence is conservative; absence is scenario-scoped only. */
static uint64_t meter_pages[256][FA18_ENGINE_COUNT][2];

void fa18_meter_os_opcode(void) {
    if (fa18_meter_enabled) ++fa18_recomp_stats.residual_instructions;
}

void fa18_meter_start(int enabled) {
    fa18_meter_enabled = enabled;
    fa18_meter_engine = FA18_ENGINE_HOST;
    memset(&fa18_emulation_meter, 0, sizeof fa18_emulation_meter);
    memset(meter_pages, 0, sizeof meter_pages);
}

void fa18_meter_access(uint32_t address, unsigned size, int write) {
    if (!fa18_meter_enabled) return;
    if (write) ++fa18_emulation_meter.writes[fa18_meter_engine];
    else ++fa18_emulation_meter.reads[fa18_meter_engine];
    unsigned previous = 256;
    for (unsigned i = 0; i < size; ++i) {
        uint32_t a = (address + i) & 0xffffffu;
        unsigned page = 256;
        if (a < 0x200000u) page = (a & (FA18_CHIP_SIZE - 1)) >> 12;
        else if (a >= FA18_SLOW_BASE && a < FA18_SLOW_BASE + FA18_SLOW_SIZE)
            page = 128 + ((a - FA18_SLOW_BASE) >> 12);
        if (page < 256 && page != previous) ++meter_pages[page][fa18_meter_engine][write != 0];
        previous = page;
    }
}

void fa18_meter_write_json(FILE *out, uint64_t frames) {
    static const char *names[] = {"host", "interpreted", "generated", "residual", "port", "os", "chipset"};
    fprintf(out, "\"_emulation\":{\"schema\":1,\"frames\":%llu,\"instructions\":{"
            "\"interpreted\":%llu,\"generated\":%llu,\"residual\":%llu},\"bus\":{",
            (unsigned long long)frames,
            (unsigned long long)fa18_recomp_stats.interpreted_instructions,
            (unsigned long long)fa18_recomp_stats.generated_instructions,
            (unsigned long long)fa18_recomp_stats.residual_instructions);
    for (unsigned e = 0; e < FA18_ENGINE_COUNT; ++e)
        fprintf(out, "%s\"%s\":{\"reads\":%llu,\"writes\":%llu}", e ? "," : "", names[e],
                (unsigned long long)fa18_emulation_meter.reads[e],
                (unsigned long long)fa18_emulation_meter.writes[e]);
    fprintf(out, "},\"chipset\":{\"blits\":%llu,\"copper_instructions\":%llu,"
            "\"bitplane_words\":%llu,\"cia_events\":%llu,\"interrupt_requests\":%llu},"
            "\"os\":{\"service_steps\":%llu,\"service_entries\":%llu,\"guest_boot_handoff\":true},"
            "\"ports\":{\"calls\":%llu,\"steps\":%llu},\"ram_pages\":{",
            (unsigned long long)fa18_emulation_meter.blits,
            (unsigned long long)fa18_emulation_meter.copper_instructions,
            (unsigned long long)fa18_emulation_meter.bitplane_words,
            (unsigned long long)fa18_emulation_meter.cia_events,
            (unsigned long long)fa18_emulation_meter.interrupts,
            (unsigned long long)fa18_emulation_meter.service_steps,
            (unsigned long long)fa18_emulation_meter.service_entries,
            (unsigned long long)fa18_emulation_meter.port_calls,
            (unsigned long long)fa18_emulation_meter.port_steps);
    int first = 1;
    for (unsigned p = 0; p < 256; ++p) {
        uint64_t total = 0;
        for (unsigned e = 0; e < FA18_ENGINE_COUNT; ++e)
            total += meter_pages[p][e][0] + meter_pages[p][e][1];
        if (!total) continue;
        fprintf(out, "%s\"%06X\":{", first ? "" : ",", p < 128 ? p << 12 : FA18_SLOW_BASE + ((p - 128) << 12));
        first = 0;
        for (unsigned e = 0; e < FA18_ENGINE_COUNT; ++e)
            fprintf(out, "%s\"%s\":[%llu,%llu]", e ? "," : "", names[e],
                    (unsigned long long)meter_pages[p][e][0], (unsigned long long)meter_pages[p][e][1]);
        fputc('}', out);
    }
    fputs("}}", out);
}

int fa18_bus_timing = 1;

static uint8_t line_dma[FA18_PAL_LINES][FA18_LINE_CCKS]; /* 1 = owned by DMA */
static int current_vpos;
static int64_t current_line_start; /* CPU cycles */
static int access_index;           /* bus accesses so far in this instruction */
static uint32_t fetch_next;        /* where sequential program fetch continues */
static int fetches;                /* program fetches of the current instruction */
static int lead;                   /* internal cycles before its first access */
static uint8_t lead_table[0x10000];
static int jumping;                /* the current instruction changes the flow */
static uint32_t jump_pc;           /* ... from here */
static int jump_fetches;           /* its program fetches, not yet charged */
static int eclock_phase;
static int copper_carry; /* Copper fetches left over from the previous line */
static FILE *boundary_trace;
static uint32_t boundary_low, boundary_high;
static uint64_t boundary_trace_bytes, boundary_trace_limit;
static char boundary_trace_path[1024];

extern int fa18_write_log_active;
extern int64_t fa18_next_event;

int fa18_bus_trace_close(void) {
    int result = boundary_trace ? fclose(boundary_trace) : 0;
    boundary_trace = NULL;
    if (result) fprintf(stderr, "boundary trace: cannot finish output\n");
    return result == 0;
}

void fa18_bus_trace_boundary(const char *kind, uint32_t source_pc) {
    FA18Machine *m = fa18_machine;
    uint64_t row_bytes = 0;
    int i, written;
    if (!boundary_trace || source_pc < boundary_low || source_pc >= boundary_high) return;
    /* Read machine/CPU storage directly: tracing must not issue bus accesses
     * or consume cycles. log_mode separates sandboxed ports from live source. */
    written = fprintf(boundary_trace, "%s,%06X,%06X,%lld,%lld,%llu,%d,%llu,%04X,%04X,%04X,%d",
        kind, source_pc, REG_PC, (long long)fa18_machine_now(),
        (long long)fa18_next_event, (unsigned long long)m->frame, m->vpos,
        (unsigned long long)m->blits, m->intreq, m->intena, m->dmacon,
        fa18_write_log_active);
    if (written < 0) goto failed;
    row_bytes += (unsigned)written;
    for (i = 0; i < 16; ++i) {
        written = fprintf(boundary_trace, ",%08X", REG_DA[i]);
        if (written < 0) goto failed;
        row_bytes += (unsigned)written;
    }
    written = fprintf(boundary_trace, ",%04X\n", m68k_get_reg(NULL, M68K_REG_SR));
    if (written < 0) goto failed;
    boundary_trace_bytes += row_bytes + (unsigned)written;
    if (!boundary_trace_limit || boundary_trace_bytes <= boundary_trace_limit) return;
    fprintf(stderr, "boundary trace: exceeded %llu MiB; set FA18_BOUNDARY_TRACE_MAX_MIB=0 for unlimited output\n",
            (unsigned long long)(boundary_trace_limit >> 20));
    fclose(boundary_trace);
    boundary_trace = NULL;
    remove(boundary_trace_path);
    abort();
failed:
    fprintf(stderr, "boundary trace: cannot write observation at %06X\n", source_pc);
    abort();
}

/* The running (or last) blit: one byte per CCK from blit_first, 1 where the
 * blitter or other DMA holds the bus. */
#define BLIT_TIMELINE_MAX (1 << 20)
static uint8_t blit_timeline[BLIT_TIMELINE_MAX];
static int64_t blit_first, blit_count;

static void build_lead_table(void);

void fa18_bus_reset(void) {
    const char *phase = getenv("FA18_ECLOCK_PHASE");
    const char *trace_path = getenv("FA18_BOUNDARY_TRACE");
    const char *trace_range = getenv("FA18_BOUNDARY_RANGE");
    const char *trace_limit = getenv("FA18_BOUNDARY_TRACE_MAX_MIB");
    char *limit_end;
    unsigned long long limit_mib = 1024;
    char trailing;
    if (!fa18_bus_trace_close()) abort();
    if (trace_path || trace_range) {
        if (!trace_path || !*trace_path || !trace_range ||
            sscanf(trace_range, "%x-%x%c", &boundary_low, &boundary_high, &trailing) != 2 ||
            boundary_low >= boundary_high || boundary_high > 0x1000000u) {
            fprintf(stderr, "boundary trace: set FA18_BOUNDARY_TRACE=PATH and FA18_BOUNDARY_RANGE=LO-HI (hex, exclusive HI)\n");
            abort();
        }
        if (trace_limit) {
            limit_mib = strtoull(trace_limit, &limit_end, 10);
            if (!*trace_limit || *limit_end || limit_mib > (0xFFFFFFFFFFFFFFFFull >> 20)) {
                fprintf(stderr, "boundary trace: FA18_BOUNDARY_TRACE_MAX_MIB must be a nonnegative integer\n");
                abort();
            }
        }
        boundary_trace_limit = (uint64_t)limit_mib << 20;
        boundary_trace_bytes = 0;
        if (snprintf(boundary_trace_path, sizeof boundary_trace_path, "%s", trace_path) >=
            (int)sizeof boundary_trace_path) {
            fprintf(stderr, "boundary trace: output path is too long\n");
            abort();
        }
        boundary_trace = fopen(trace_path, "w");
        if (!boundary_trace) {
            fprintf(stderr, "boundary trace: cannot open %s\n", trace_path);
            abort();
        }
        if (fputs("kind,source_pc,pc,cycle,next_event,frame,vpos,blits,intreq,intena,dmacon,log_mode,"
                  "d0,d1,d2,d3,d4,d5,d6,d7,a0,a1,a2,a3,a4,a5,a6,a7,sr\n", boundary_trace) == EOF) {
            fprintf(stderr, "boundary trace: cannot write header to %s\n", trace_path);
            abort();
        }
    }
    memset(line_dma, 0, sizeof line_dma);
    access_index = 0;
    jumping = 0;
    copper_carry = 0;
    fetch_next = 0xFFFFFFFFu;
    blit_first = blit_count = 0;
    eclock_phase = phase ? atoi(phase) : 0;
    build_lead_table();
}

static int dma_owned(int64_t cck) {
    int64_t offset = cck * 2 - current_line_start;
    int64_t lines;
    if (offset < 0) offset = 0;
    lines = offset / FA18_LINE_CYCLES;
    return line_dma[(current_vpos + lines) % FA18_PAL_LINES][(offset % FA18_LINE_CYCLES) / 2];
}

/* ---- per-line DMA map ---------------------------------------------------- */

void fa18_bus_line(FA18Machine *m, int vpos, int64_t line_start) {
    uint8_t *map = line_dma[vpos];
    uint16_t con0 = m->custom[0x100 >> 1];
    uint16_t diwstrt = m->custom[0x08E >> 1], diwstop = m->custom[0x090 >> 1];
    int ddfstrt = m->custom[0x092 >> 1] & 0xFC, ddfstop = m->custom[0x094 >> 1] & 0xFC;
    int vstart = diwstrt >> 8, vstop = (diwstop >> 8) | ((diwstop & 0x8000) ? 0 : 0x100);
    int planes = (con0 >> 12) & 7, hires = (con0 & 0x8000) != 0;
    int i, h;

    current_vpos = vpos;
    current_line_start = line_start;
    memset(map, 0, FA18_LINE_CCKS);
    /* Memory refresh. */
    for (i = 0; i < 4; i++) map[1 + 2 * i] = 1;

    if ((m->dmacon & 0x0300) == 0x0300 && planes && vpos >= vstart && vpos < vstop &&
        ddfstop >= ddfstrt) {
        /* Fetch order per 8 CCKs (lowres) or 4 CCKs (hires), by plane. */
        static const int8_t lowres[8] = {0, 4, 6, 2, 0, 3, 5, 1};
        static const int8_t hiresp[4] = {4, 2, 3, 1};
        if (hires && planes > 4) planes = 4;
        if (!hires && planes > 6) planes = 6;
        if (hires) {
            int units = ((ddfstop - ddfstrt) >> 2) + 2;
            for (i = 0; i < units * 4; i++) {
                h = ddfstrt + i;
                if (h < FA18_LINE_CCKS && hiresp[i & 3] <= planes) map[h] = 1;
            }
        } else {
            int units = ((ddfstop - ddfstrt) >> 3) + 1;
            for (i = 0; i < units * 8; i++) {
                int plane = lowres[i & 7];
                h = ddfstrt + i;
                if (h < FA18_LINE_CCKS && plane && plane <= planes) map[h] = 1;
            }
        }
    }
    /* Copper: two fetches per instruction on even CCKs that are free; what
     * does not fit (or is released past the line's end) runs on the next. */
    {
        int need = copper_carry;
        copper_carry = 0;
        for (i = -1; i < m->copper_segments; i++) {
            int start = 0;
            if (i >= 0) {
                need += 2 * m->copper_segment_count[i];
                start = m->copper_segment_start[i] & ~1;
            }
            for (h = start; h < FA18_LINE_CCKS && need > 0; h += 2) {
                if (map[h]) continue;
                map[h] = 1;
                need--;
            }
        }
        copper_carry = need;
    }
    if (getenv("FA18_MAP_LOG")) {
        char text[FA18_LINE_CCKS + 1];
        for (h = 0; h < FA18_LINE_CCKS; h++) text[h] = map[h] ? 'D' : '.';
        text[FA18_LINE_CCKS] = 0;
        fprintf(stderr, "MAP %3d %s\n", vpos, text);
    }
}

/* ---- blits ---------------------------------------------------------------- */

int64_t fa18_bus_blit(int64_t start, const uint8_t *diagram, int steps_per_word, int64_t words) {
    int64_t cck = (start + 1) >> 1, steps = (int64_t)steps_per_word * words, step = 0, n = 0;
    /* The blitter starts two CCKs after BLTSIZE is written. */
    blit_first = cck;
    blit_timeline[n++] = 0;
    blit_timeline[n++] = 0;
    cck += 2;
    while (step < steps && n < BLIT_TIMELINE_MAX) {
        if (dma_owned(cck)) {
            blit_timeline[n++] = 1;
        } else {
            blit_timeline[n++] = diagram[step % steps_per_word];
            step++;
        }
        cck++;
    }
    blit_count = n;
    return (blit_first + blit_count) * 2;
}

/* ---- CPU accesses --------------------------------------------------------- */

static int64_t access_time(void) {
    return fa18_machine_now() + lead + 4 * access_index;
}

int64_t fa18_bus_now(void) {
    return access_time();
}

void fa18_bus_instruction(void) {
    access_index = 0;
    lead = 0;
}

static int64_t slot_wait(uint32_t a, int64_t t);

/* ---- where an instruction's accesses fall -------------------------------- */

/* Internal cycles before an instruction's first bus access, by opcode (the
 * 68000's microcycle order, e.g. "n np np" for a taken branch). Accesses
 * then follow four cycles apart. 255 marks the conditional branches, whose
 * order depends on whether they are taken. */
enum { LEAD_BCC = 255, LEAD_DBCC = 254 };

static int ea_lead(int ea) {
    int mode = (ea >> 3) & 7, reg = ea & 7;
    if (mode == 4) return 2;                            /* -(An): n nr */
    if (mode == 6 || (mode == 7 && reg == 3)) return 2; /* (d8,An,Xn), (d8,PC,Xn): n np nr */
    return 0;
}

static void build_lead_table(void) {
    int op;
    for (op = 0; op < 0x10000; op++) {
        int ea = op & 0x3F, mode = (ea >> 3) & 7, reg = ea & 7, l = 0;
        switch (op >> 12) {
        case 0x6:
            l = (op & 0xFF00) == 0x6100 ? 2 : LEAD_BCC; /* BSR: n nS ns np np */
            break;
        case 0x5:
            if ((op & 0xF0F8) == 0x50C8) l = LEAD_DBCC;
            else l = ea_lead(ea);
            break;
        case 0x4:
            if ((op & 0xFF80) == 0x4E80) { /* JSR, JMP */
                if (mode == 5 || (mode == 7 && (reg == 0 || reg == 2))) l = 2;
                else if (mode == 6 || (mode == 7 && reg == 3)) l = 6;
            } else if ((op & 0xF1C0) == 0x41C0 || (op & 0xFFC0) == 0x4840) { /* LEA, PEA */
                if (mode == 6 || (mode == 7 && reg == 3)) l = 2;
            } else if ((op & 0xFB80) != 0x4880 && (op & 0xFFF0) != 0x4E70 && (op & 0xFFF0) != 0x4E40 &&
                       (op & 0xFFF0) != 0x4E50 && (op & 0xFFF0) != 0x4E60) {
                l = ea_lead(ea); /* not MOVEM, TRAP, LINK/UNLK, MOVE USP, RTS etc. */
            }
            break;
        case 0x1: case 0x2: case 0x3: /* MOVE: the source field */
        case 0x0: case 0x8: case 0x9: case 0xB: case 0xC: case 0xD:
            l = ea_lead(ea);
            break;
        case 0xE:
            if ((op & 0xC0) == 0xC0) l = ea_lead(ea); /* memory shifts */
            break;
        default: break;
        }
        lead_table[op] = (uint8_t)l;
    }
}

static int condition(int cc) {
    switch (cc & 15) {
    case 0: return 1;
    case 1: return 0;
    case 2: return COND_HI() != 0;
    case 3: return COND_LS() != 0;
    case 4: return COND_CC() != 0;
    case 5: return COND_CS() != 0;
    case 6: return COND_NE() != 0;
    case 7: return COND_EQ() != 0;
    case 8: return COND_VC() != 0;
    case 9: return COND_VS() != 0;
    case 10: return COND_PL() != 0;
    case 11: return COND_MI() != 0;
    case 12: return COND_GE() != 0;
    case 13: return COND_LT() != 0;
    case 14: return COND_GT() != 0;
    default: return COND_LE() != 0;
    }
}

/* A jump's program fetches: the 68000 ends a change of flow by refilling
 * its two-word prefetch queue at the target ("... np np"), after any stack
 * accesses; only words beyond the first two of the instruction are fetched
 * from the jump itself. They are charged once the target is known, at the
 * start of the next instruction, at the times they happened. */
void fa18_bus_finish(uint32_t target) {
    int n, k;
    int64_t end;
    if (!jumping) return;
    jumping = 0;
    if (!fa18_bus_timing) return;
    n = (jump_fetches > 2 ? jump_fetches - 2 : 0) + 2;
    end = fa18_machine_now();
    for (k = 0; k < n; k++) {
        uint32_t a = k < n - 2 ? jump_pc : target;
        int64_t wait = slot_wait(a, end - 4 * (n - k));
        if (wait > 0) {
            USE_CYCLES((int)wait);
            end += wait;
        }
    }
    fetch_next = target + 4;
    if (boundary_trace) fa18_bus_trace_boundary("flow_target", jump_pc);
}

static int is_jump(int op) {
    if ((op & 0xF000) == 0x6000) {
        if ((op & 0xFE00) == 0x6000) return 1; /* BRA, BSR */
        return condition(op >> 8);
    }
    if ((op & 0xF0F8) == 0x50C8) /* DBcc: loops while false and the counter has not run out */
        return !condition(op >> 8) && (REG_D[op & 7] & 0xFFFF) != 0;
    if ((op & 0xFF80) == 0x4E80) return 1; /* JSR, JMP */
    return op == 0x4E75 || op == 0x4E73 || op == 0x4E77; /* RTS, RTE, RTR */
}

/* An instruction at `pc` starts. */
void fa18_bus_begin(uint32_t pc) {
    if (fa18_machine->runtime_guard.enabled &&
        !amiga_runtime_guard_fetch(&fa18_machine->runtime_guard,pc,(uint64_t)fa18_machine_now()))
        fa18_machine_runtime_fault();
    fa18_bus_begin_instruction(pc,fa18_bus_read16(pc));
}

void fa18_bus_begin_instruction(uint32_t pc, uint16_t opcode) {
    int op=opcode, l;
    fa18_rom_audit_instruction(pc);
    fa18_bus_finish(pc);
    access_index = 0;
    fetches = 0;
    l = lead_table[op];
    if (l == LEAD_BCC) {
        /* Taken: n np np. Not taken: nn np (np). */
        l = condition(op >> 8) ? 2 : 4;
    } else if (l == LEAD_DBCC) {
        /* Loops back: n np np; condition true: n n np np. */
        l = condition(op >> 8) ? 4 : 2;
    }
    lead = l;
    jumping = is_jump(op);
    jump_pc = pc;
    jump_fetches = 0;
    if (boundary_trace) fa18_bus_trace_boundary("instruction", pc);
}

static int is_chip_bus(uint32_t a) {
    a &= 0xFFFFFF;
    return a < 0x200000 || (a >= FA18_SLOW_BASE && a < 0xDC0000) || (a >= 0xDFF000 && a < 0xE00000);
}

static int blitter_holds(int64_t cck) {
    int64_t i = cck - blit_first;
    return i >= 0 && i < blit_count && blit_timeline[i];
}

void fa18_blitter_delayed(int cycles);

/* The CPU takes the blitter's cycle `cck`: the rest of the blit moves one
 * cycle later. */
static void blitter_yield(int64_t cck) {
    int64_t i = cck - blit_first;
    if (blit_count >= BLIT_TIMELINE_MAX) return;
    memmove(blit_timeline + i + 1, blit_timeline + i, (size_t)(blit_count - i));
    blit_timeline[i] = 0;
    blit_count++;
    fa18_blitter_delayed(2);
}

/* Cycles a chip-bus access starting at CPU cycle `t` waits for its slot;
 * with `steal`, a cycle the CPU takes from the blitter is taken for good. */
static int64_t slot_wait_steal(uint32_t a, int64_t t, int steal) {
    int64_t cck;
    if (!is_chip_bus(a)) return 0;
    cck = (t + 1) >> 1;
    if (fa18_machine->dmacon & 0x0400) {
        while (dma_owned(cck) || blitter_holds(cck)) cck++;
    } else {
        /* Without BLTPRI the CPU takes a blitter cycle after waiting for the
         * blitter. UAE (dma_cycle, BLIT_NASTY_CPU_STEAL_CYCLE_COUNT) counts
         * every waited cycle and delays the blit; its pipelined blitter is
         * not modelled here, and measured against its traces (bitplane,
         * area and line blits) the closest fit is: count only blitter-held
         * cycles, take the third, no delay. FA18_STEAL=mode,limit selects
         * the variants (mode bit 0: count other DMA, bit 1: delay). */
        static int mode = -1, limit = 3;
        int waited = 1;
        if (mode < 0) {
            const char *e = getenv("FA18_STEAL");
            mode = 0;
            if (e) sscanf(e, "%d,%d", &mode, &limit);
        }
        while (dma_owned(cck) || blitter_holds(cck)) {
            if (!dma_owned(cck) && waited >= limit) {
                if (steal && (mode & 2)) blitter_yield(cck);
                break;
            }
            if ((mode & 1) || !dma_owned(cck)) waited++;
            cck++;
        }
    }
    return cck * 2 - t;
}

static int64_t slot_wait(uint32_t a, int64_t t) { return slot_wait_steal(a, t, 1); }

void fa18_bus_access(uint32_t a) {
    int64_t t, wait;
    if (!fa18_bus_timing) return;
    t = access_time();
    access_index++;
    if ((a & 0xFF0000) == 0xBF0000) {
        /* CIA: wait for the E clock's data phase (UAE cia_wait_pre/post). The
         * table already charges the four cycles of a normal access. */
        int div = (int)((t + eclock_phase) % 10);
        int pre = div == 0 ? 4 : 14 - div;
        USE_CYCLES(pre + 6 - 4);
        return;
    }
    wait = slot_wait(a, t);
    if (wait > 4 && getenv("FA18_WAIT_LOG")) {
        int64_t c = (t + 1) >> 1, offset = t - current_line_start;
        fprintf(stderr, "WAIT %06X t=%lld v=%d h=%lld wait=%lld dma:", a, (long long)t, current_vpos,
                (long long)(offset / 2), (long long)wait);
        for (; c < (t + wait + 1) >> 1; c++) fprintf(stderr, "%c", dma_owned(c) ? 'D' : blitter_holds(c) ? 'B' : '.');
        fprintf(stderr, "\n");
    }
    if (wait > 0) USE_CYCLES((int)wait);
}

/* A program word fetch (the refill after a jump is charged by
 * fa18_bus_begin). */
void fa18_bus_fetch(uint32_t a) {
    if (!fa18_bus_timing) return;
    if (jumping) {
        jump_fetches++;
        return;
    }
    fa18_bus_access(a);
    fetches++;
    fetch_next = a + 2;
}

/* ---- Musashi cycle table ------------------------------------------------- */

/* Corrections to Musashi's 68000 table, measured against cycle-exact UAE
 * (scripts/musashi_timing_audit.py) and matching the 68000 manual's
 * footnotes. The data-dependent cases (MULS, DIVS, DIVU, bit operations on
 * a data register) are patched in m68kops.c. */
void fa18_cpu_timing_init(void) {
    unsigned char *t = m68ki_cycles[0];
    int r, ea;
    for (r = 0; r < 8; r++) {
        for (ea = 0; ea < 16; ea++) { /* Dn or An source */
            int reg = r << 9;
            if (ea < 8) {
                t[0xD080 | reg | ea] = 8; /* ADD.L Dn,Dn */
                t[0x9080 | reg | ea] = 8; /* SUB.L */
                t[0xC080 | reg | ea] = 8; /* AND.L */
                t[0x8080 | reg | ea] = 8; /* OR.L */
            } else {
                t[0xD080 | reg | ea] = 8; /* ADD.L An,Dn */
                t[0x9080 | reg | ea] = 8;
            }
            t[0xD1C0 | reg | ea] = 8; /* ADDA.L */
            t[0x91C0 | reg | ea] = 8; /* SUBA.L */
        }
        t[0xD0FC | r << 9] = 12; /* ADDA.W #imm */
        t[0x90FC | r << 9] = 12; /* SUBA.W #imm */
        t[0x0280 | r] = 16;      /* ANDI.L #imm,Dn */
        for (ea = 0; ea < 8; ea++) {
            t[0x5048 | ea << 9 | r] = 8; /* ADDQ.W #,An */
            t[0x5148 | ea << 9 | r] = 8; /* SUBQ.W #,An */
        }
    }
}
