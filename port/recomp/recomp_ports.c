/* Stage D port dispatch and SHADOW-mode proof (see recomp_ports.h). */
#include "recomp_ports.h"
#include "loop_input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m68kcpu.h"
#include "machine.h"
#include "recomp_runtime.h"

extern int64_t fa18_cycle_origin, fa18_next_event;

/* Write log used by SHADOW mode: every Chip/Slow byte written while active,
 * with its previous value. Active 1 (the sandboxed port): hardware (CIA,
 * mouse counters) is not touched and marks the call as not comparable, and
 * custom-register writes are held. Active 2 (the live generated routine):
 * everything happens as usual; hardware access, or taking an interrupt,
 * marks the call, and custom writes are recorded as well as performed. */
int fa18_write_log_active;
int fa18_write_log_hardware;
typedef struct { uint32_t address; uint8_t old; } LogEntry;
static LogEntry *log_entries;
static size_t log_count, log_capacity;

void fa18_write_log_before(uint32_t address, int size) {
    int n;
    for (n = 0; n < size; n++) {
        uint32_t a = (address + (uint32_t)n) & 0xFFFFFF;
        if (log_count == log_capacity) {
            log_capacity = log_capacity ? log_capacity * 2 : 4096;
            log_entries = realloc(log_entries, log_capacity * sizeof *log_entries);
        }
        log_entries[log_count].address = a;
        log_entries[log_count].old = fa18_bus_read8(a);
        log_count++;
    }
}

/* Custom-register writes made while active, in order. They are compared
 * between the two runs and then performed once, from the reference. */
typedef struct { uint32_t reg; uint16_t value; } CustomWrite;
static CustomWrite *custom_log;
static size_t custom_count, custom_capacity;

void fa18_write_log_custom(uint32_t reg, uint16_t value) {
    if (custom_count == custom_capacity) {
        custom_capacity = custom_capacity ? custom_capacity * 2 : 256;
        custom_log = realloc(custom_log, custom_capacity * sizeof *custom_log);
    }
    custom_log[custom_count].reg = reg;
    custom_log[custom_count].value = value;
    custom_count++;
}

/* Chip bytes the blitter wrote during a live comparison: their final value
 * depends on DMA the sandboxed port did not run, so they are not compared. */
static uint8_t dma_bits[FA18_CHIP_SIZE / 8];
static uint32_t *dma_log;
static size_t dma_count, dma_capacity;

void fa18_note_dma_write(uint32_t address, int size);
void fa18_note_dma_write(uint32_t address, int size) {
    int n;
    for (n = 0; n < size; n++) {
        uint32_t a = (address + (uint32_t)n) & (FA18_CHIP_SIZE - 1);
        if (dma_bits[a >> 3] & (1 << (a & 7))) continue;
        dma_bits[a >> 3] |= (uint8_t)(1 << (a & 7));
        if (dma_count == dma_capacity) {
            dma_capacity = dma_capacity ? dma_capacity * 2 : 4096;
            dma_log = realloc(dma_log, dma_capacity * sizeof *dma_log);
        }
        dma_log[dma_count++] = a;
    }
}

static int dma_written(uint32_t a) {
    a &= 0xFFFFFF;
    if (a >= 0x200000) return 0;
    a &= FA18_CHIP_SIZE - 1;
    return (dma_bits[a >> 3] >> (a & 7)) & 1;
}

static void dma_clear(void) {
    while (dma_count) {
        uint32_t a = dma_log[--dma_count];
        dma_bits[a >> 3] &= (uint8_t)~(1 << (a & 7));
    }
}

static uint8_t *byte_at(uint32_t a) {
    FA18Machine *m = fa18_machine;
    a &= 0xFFFFFF;
    if (a < 0x200000) return &m->chip[a & (FA18_CHIP_SIZE - 1)];
    if (a >= FA18_SLOW_BASE && a < FA18_SLOW_BASE + FA18_SLOW_SIZE) return &m->slow[a - FA18_SLOW_BASE];
    if ((a & 0xFF0000) == 0xF00000) return &m->rtarea[a & 0xFFFF];
    return NULL;
}

static void undo_log(size_t from) {
    while (log_count > from) {
        uint8_t *p;
        log_count--;
        p = byte_at(log_entries[log_count].address);
        if (p) *p = log_entries[log_count].old;
    }
}

/* ---- registry ------------------------------------------------------------ */

typedef struct {
    uint64_t calls, compared, matched, mismatched, hardware, incomplete;
    uint64_t reference_cycles;
    uint64_t busy_input_calls, busy_input_reads;
    int reported;
} PortStats;

static FA18PortMode mode;
static int *port_of_function; /* function id -> port index, or -1 */
static PortStats *stats;
static uint64_t *profile;
static unsigned char *context_before, *context_reference;
typedef struct {
    int port;
    uint32_t return_pc, return_sp;
} SteppedCall;
static SteppedCall *stepped_calls;
static size_t stepped_count, stepped_capacity;

/* A held BLTSIZE write cannot supply the subsequent live BBUSY inputs.
 * For opted-in shadow bridges, record the source inputs first, then replay
 * exactly that PC/read sequence. No source output or write is supplied to C.
 * Timing remains independently checked by live ON replay and bus traces. */
typedef struct { uint32_t pc; uint16_t value; } BusyRead;
static BusyRead *busy_reads;
static size_t busy_count, busy_cursor, busy_capacity;
static int busy_phase, busy_port;
typedef struct {
    uint8_t chip[FA18_CHIP_SIZE], slow[FA18_SLOW_SIZE], rtarea[0x10000];
} ShadowRAM;
static ShadowRAM *shadow_entry_ram, *shadow_live_ram;

uint16_t fa18_shadow_dmaconr(uint16_t value) {
    if (busy_phase == 1) {
        if (busy_count == busy_capacity) {
            size_t capacity = busy_capacity ? busy_capacity * 2 : 256;
            BusyRead *reads = realloc(busy_reads, capacity * sizeof *reads);
            if (!reads) { fputs("shadow: cannot allocate busy inputs\n", stderr); abort(); }
            busy_reads = reads; busy_capacity = capacity;
        }
        busy_reads[busy_count].pc = REG_PPC;
        busy_reads[busy_count++].value = value;
    } else if (busy_phase == 2) {
        if (busy_cursor == busy_count || busy_reads[busy_cursor].pc != REG_PPC) {
            fprintf(stderr, "port %s: unexpected shadow DMACONR read at %06X, input %zu/%zu\n",
                    fa18_ports[busy_port].name, REG_PPC, busy_cursor, busy_count);
            abort();
        }
        return busy_reads[busy_cursor++].value;
    }
    return value;
}

static void save_shadow_ram(ShadowRAM *ram) {
    memcpy(ram->chip, fa18_machine->chip, sizeof ram->chip);
    memcpy(ram->slow, fa18_machine->slow, sizeof ram->slow);
    memcpy(ram->rtarea, fa18_machine->rtarea, sizeof ram->rtarea);
}

static void restore_shadow_ram(const ShadowRAM *ram) {
    memcpy(fa18_machine->chip, ram->chip, sizeof ram->chip);
    memcpy(fa18_machine->slow, ram->slow, sizeof ram->slow);
    memcpy(fa18_machine->rtarea, ram->rtarea, sizeof ram->rtarea);
}

static uint32_t stepped_start(const FA18Port *port) {
    return port->step_start ? port->step_start : port->entry;
}

void fa18_ports_init(FA18PortMode new_mode, const char *only) {
    int i, f;
    mode = new_mode;
    stepped_count = 0;
    busy_phase = 0;
    free(port_of_function);
    free(stats);
    free(profile);
    port_of_function = malloc(sizeof(int) * (size_t)(fa18_recomp_function_count + 1));
    stats = calloc((size_t)fa18_port_count + 1, sizeof *stats);
    profile = calloc((size_t)fa18_recomp_function_count + 1, sizeof *profile);
    for (f = 0; f < fa18_recomp_function_count; f++) port_of_function[f] = -1;
    for (i = 0; i < fa18_port_count; i++) {
        if ((fa18_ports[i].step && (fa18_ports[i].step_end <= fa18_ports[i].entry ||
                                  stepped_start(&fa18_ports[i]) > fa18_ports[i].entry)) ||
            (fa18_ports[i].shadow_busy_reads && !fa18_ports[i].step)) {
            fprintf(stderr, "port %s: invalid stepped source range\n", fa18_ports[i].name);
            abort();
        }
        if (only && *only) {
            char want[16];
            snprintf(want, sizeof want, "%06X", fa18_ports[i].entry);
            if (!strstr(only, want) && !strstr(only, fa18_ports[i].name)) continue;
        }
        for (f = 0; f < fa18_recomp_function_count; f++)
            if (fa18_recomp_functions[f].entry == fa18_ports[i].entry) port_of_function[f] = i;
    }
    free(context_before);
    free(context_reference);
    context_before = malloc(m68k_context_size());
    context_reference = malloc(m68k_context_size());
}

/* The instruction before the routine entry must be the JSR/BSR that called
 * it; branches and fall-through into a routine start are not calls. */
static int entered_by_call(void) {
    uint16_t op = fa18_bus_read16(REG_PPC);
    return (op & 0xFF00) == 0x6100 || (op & 0xFFC0) == 0x4E80;
}

/* A step can dispatch a child after the runtime has serviced a deadline, at
 * which point REG_PPC no longer necessarily names the source call.  Accept
 * the return address saved by that call only when it resumes an active
 * stepped source range and the bytes immediately before it are a BSR/JSR. */
static int entered_from_stepped_call(void) {
    uint32_t ret;
    uint16_t op;
    size_t i;
    if (!stepped_count) return 0;
    ret = fa18_bus_read32(REG_A[7]) & 0xFFFFFFu;
    for (i = stepped_count; i > 0; --i) {
        const FA18Port *parent = &fa18_ports[stepped_calls[i - 1].port];
        if (ret < stepped_start(parent) || ret >= parent->step_end) continue;
        op = fa18_bus_read16(ret - 2);
        if ((op & 0xFF00u) == 0x6100u && (op & 0xFFu)) return 1;
        op = fa18_bus_read16(ret - 4);
        if (op == 0x6100u || (op & 0xFFC0u) == 0x4E80u) return 1;
        op = fa18_bus_read16(ret - 6);
        if ((op & 0xFFC0u) == 0x4E80u) return 1;
    }
    return 0;
}

/* A proof runs a stepped bridge with events held off, exactly like its
 * generated reference. Child calls use their existing dispatch contracts.
 * No stepped-call continuation is retained in a sandbox. */
static int run_port_body(int port) {
    uint32_t ret, sp;
    if (!fa18_ports[port].step) return fa18_ports[port].glue();
    ret = fa18_bus_read32(REG_A[7]) & 0xffffffu;
    sp = REG_A[7] + 4;
    for (;;) {
        int result;
        if (REG_PC == ret && REG_A[7] == sp) return FA18_RET;
        if (REG_PC >= stepped_start(&fa18_ports[port]) && REG_PC < fa18_ports[port].step_end) {
            if (!fa18_ports[port].step()) {
                fprintf(stderr, "port %s: missing instruction boundary at %06X\n",
                        fa18_ports[port].name, REG_PC);
                abort();
            }
        } else {
            result = fa18_recomp_call_dynamic();
            if (result != FA18_RET) return result;
        }
    }
}

static void finish_stepped_calls(void) {
    while (stepped_count &&
           REG_PC == stepped_calls[stepped_count - 1].return_pc &&
           REG_A[7] == stepped_calls[stepped_count - 1].return_sp)
        --stepped_count;
}

int fa18_ports_resume_step(void) {
    size_t i;
    if (mode != FA18_PORTS_ON || fa18_write_log_active) return 0;
    finish_stepped_calls();
    /* An interrupt may enter a nested native call while an older one waits;
     * choose the innermost bridge whose source range owns the resumed PC. */
    for (i = stepped_count; i > 0; --i) {
        const FA18Port *port = &fa18_ports[stepped_calls[i - 1].port];
        if (REG_PC >= stepped_start(port) && REG_PC < port->step_end) {
            if (!port->step()) {
                fprintf(stderr, "port %s: cannot resume at %06X\n", port->name, REG_PC);
                abort();
            }
            finish_stepped_calls();
            return 1;
        }
    }
    return 0;
}

static int run_glue(int port) {
    if (fa18_ports[port].step) {
        SteppedCall *call;
        if (stepped_count == stepped_capacity) {
            size_t capacity = stepped_capacity ? stepped_capacity * 2 : 16;
            SteppedCall *calls = realloc(stepped_calls, capacity * sizeof *calls);
            if (!calls) {
                fprintf(stderr, "port %s: cannot allocate continuation\n", fa18_ports[port].name);
                abort();
            }
            stepped_calls = calls;
            stepped_capacity = capacity;
        }
        call = &stepped_calls[stepped_count++];
        call->port = port;
        call->return_pc = fa18_bus_read32(REG_A[7]) & 0xffffffu;
        call->return_sp = REG_A[7] + 4;
        /* The caller's JSR may already have reached a chipset deadline.
         * Dispatch first so service precedes the first bridge instruction. */
        return FA18_EXIT_DISPATCH;
    }
    int r = fa18_ports[port].glue();
    USE_CYCLES(fa18_ports[port].cycles);
    return r;
}

/* The compared call's return address, taken at its entry. */
static uint32_t report_caller;

static void report_mismatch(int port, const char *what, uint32_t detail, uint32_t ref, uint32_t got) {
    PortStats *s = &stats[port];
    if (s->reported >= 8) return;
    s->reported++;
    fprintf(stderr, "port %s ($%06X) mismatch: %s %06X reference %08X port %08X (call %llu, caller $%06X)\n",
            fa18_ports[port].name, fa18_ports[port].entry, what, detail, ref, got,
            (unsigned long long)s->calls, report_caller);
}

/* ---- liveness ------------------------------------------------------------ */

static int poison;
void fa18_ports_set_poison(int on) { poison = on; }

/* Live registers/flags after the call returning to `ret`; NULL = all live. */
static const FA18CallLiveness *liveness_after(uint32_t ret) {
    int lo = 0, hi = fa18_call_liveness_count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        uint32_t r = fa18_call_liveness[mid].ret;
        if (r == ret) return &fa18_call_liveness[mid];
        if (r < ret) lo = mid + 1;
        else hi = mid - 1;
    }
    return NULL;
}

/* Bits of register i (0-7 D, 8-15 A) the caller can observe. */
static uint32_t register_mask(const FA18CallLiveness *live, int i) {
    uint32_t mask = 0;
    if (!live) return 0xFFFFFFFFu;
    if (i >= 8) return (live->regs >> i & 1) ? 0xFFFFFFFFu : 0;
    if (live->regs >> i & 1) mask |= 0x0000FFFFu;
    if (live->high >> i & 1) mask |= 0xFFFF0000u;
    return mask;
}

/* SR bits of the live condition flags (X=0x10, N=8, Z=4, V=2, C=1). */
static uint32_t sr_flag_mask(const FA18CallLiveness *live) {
    static const uint32_t bit[5] = {0x10, 0x08, 0x04, 0x02, 0x01};
    uint32_t mask = 0;
    int f;
    if (!live) return 0x1F;
    for (f = 0; f < 5; f++)
        if (live->flags >> f & 1) mask |= bit[f];
    return mask;
}

/* Stack below the stack pointer the caller returns with is dead. */
static int dead_stack(uint32_t a, uint32_t sp) {
    sp &= 0xFFFFFF;
    return a < sp && a >= sp - 0x1000;
}

/* Validation of the liveness table: overwrite everything it declares dead.
 * A wrong entry changes the game, which a comparison with the plain
 * generated run then shows. */
static void poison_dead(const FA18CallLiveness *live) {
    int i;
    if (!live) return;
    for (i = 0; i < 16; i++) {
        uint32_t dead = ~register_mask(live, i);
        if (i == 15) dead = 0; /* never A7 */
        REG_DA[i] = (REG_DA[i] & ~dead) | (0xA5C3E1F7u & dead);
    }
    if (!(live->flags & 0x01)) FLAG_X ^= XFLAG_SET;
    if (!(live->flags & 0x02)) FLAG_N ^= NFLAG_SET;
    if (!(live->flags & 0x04)) FLAG_Z = FLAG_Z ? 0 : 1;
    if (!(live->flags & 0x08)) FLAG_V ^= VFLAG_SET;
    if (!(live->flags & 0x10)) FLAG_C ^= CFLAG_SET;
}

/* SANDBOX: the older comparison, reference first with chipset events held
 * off and custom writes performed at its end, then the port on the same
 * state; the game keeps the reference result. It compares calls the live
 * comparison cannot (interrupts or hardware inside the routine) but moves
 * events and blits, so a sandbox run does not keep the plain run's timing:
 * use it to prove routines, not to replay recordings. */
static int run_sandbox(int function, int label, int port) {
    PortStats *s = &stats[port];
    int64_t saved_event = fa18_next_event;
    int cycles_before = GET_CYCLES(), cycles_reference, r, i, mismatch = 0;
    size_t reference_end, port_start, reference_custom_count, k;
    CustomWrite *reference_custom;
    uint32_t caller = fa18_bus_read32(REG_A[7]) & 0xFFFFFF, reference_sp;
    const FA18CallLiveness *live = liveness_after(caller);
    report_caller = caller;
    LogEntry *reference;
    uint8_t *reference_new;

    m68k_get_context(context_before);
    log_count = 0;
    custom_count = 0;
    fa18_write_log_active = 1;
    fa18_write_log_hardware = 0;
    fa18_next_event = INT64_MAX; /* no chipset servicing inside the comparison */
    r = fa18_recomp_functions[function].fn(label);
    fa18_next_event = saved_event;
    if (r != FA18_RET || fa18_write_log_hardware) {
        /* Not comparable: undo and let the generated routine run for real. */
        fa18_write_log_active = 0;
        undo_log(0);
        m68k_set_context(context_before);
        SET_CYCLES(cycles_before);
        if (r != FA18_RET) s->incomplete++;
        else s->hardware++;
        return fa18_recomp_functions[function].fn(label);
    }
    cycles_reference = cycles_before - GET_CYCLES();
    m68k_get_context(context_reference);
    reference_sp = REG_A[7];
    reference_end = log_count;
    reference_custom_count = custom_count;
    reference_custom = malloc(sizeof *reference_custom * (custom_count + 1));
    memcpy(reference_custom, custom_log, sizeof *reference_custom * custom_count);
    custom_count = 0;
    reference = malloc(sizeof *reference * (reference_end + 1));
    reference_new = malloc(reference_end + 1);
    memcpy(reference, log_entries, sizeof *reference * reference_end);
    for (i = 0; i < (int)reference_end; i++) reference_new[i] = *byte_at(reference[i].address);
    undo_log(0);
    m68k_set_context(context_before);
    SET_CYCLES(cycles_before);

    port_start = log_count;
    if (fa18_ports[port].step) fa18_next_event = INT64_MAX;
    r = run_port_body(port);
    fa18_next_event = saved_event;
    fa18_write_log_active = 0;
    s->compared++;
    s->reference_cycles += (uint64_t)cycles_reference;
    if (r != FA18_RET) {
        report_mismatch(port, "glue did not return", 0, 0, (uint32_t)r);
        mismatch = 1;
    } else {
        /* Registers and flags the caller can observe (liveness table). */
        for (i = 0; i < 16; i++) {
            uint32_t want = ((m68ki_cpu_core *)context_reference)->dar[i];
            uint32_t mask = register_mask(live, i);
            if ((REG_DA[i] ^ want) & mask) {
                report_mismatch(port, i < 8 ? "D" : "A", (uint32_t)(i & 7), want, REG_DA[i]);
                mismatch = 1;
            }
        }
        {
            unsigned char *now = malloc(m68k_context_size());
            uint32_t want_sr, got_sr, want_pc, flag_mask = sr_flag_mask(live);
            m68k_get_context(now);
            m68k_set_context(context_reference);
            want_sr = m68k_get_reg(NULL, M68K_REG_SR);
            want_pc = m68k_get_reg(NULL, M68K_REG_PC);
            m68k_set_context(now);
            got_sr = m68k_get_reg(NULL, M68K_REG_SR);
            free(now);
            if ((want_sr ^ got_sr) & (0xFF00 | flag_mask)) {
                report_mismatch(port, "SR", 0, want_sr, got_sr);
                mismatch = 1;
            }
            if (REG_PC != want_pc) {
                report_mismatch(port, "PC", 0, want_pc, REG_PC);
                mismatch = 1;
            }
        }
        /* Hardware: the same custom-register writes in the same order. */
        if (custom_count != reference_custom_count) {
            report_mismatch(port, "custom write count", 0, (uint32_t)reference_custom_count, (uint32_t)custom_count);
            mismatch = 1;
        }
        for (k = 0; !mismatch && k < custom_count; k++) {
            if (custom_log[k].reg != reference_custom[k].reg || custom_log[k].value != reference_custom[k].value) {
                report_mismatch(port, "custom write", reference_custom[k].reg,
                                reference_custom[k].reg << 16 | reference_custom[k].value,
                                custom_log[k].reg << 16 | custom_log[k].value);
                mismatch = 1;
            }
        }
        /* Memory: every byte either run wrote must end with the same value,
         * except the dead stack below the returned-to stack pointer. */
        for (i = 0; i < (int)reference_end; i++) {
            uint8_t got = *byte_at(reference[i].address);
            if (dead_stack(reference[i].address, reference_sp)) continue;
            if (got != reference_new[i]) {
                report_mismatch(port, "byte", reference[i].address, reference_new[i], got);
                mismatch = 1;
                break;
            }
        }
        for (i = (int)port_start; !mismatch && i < (int)log_count; i++) {
            uint32_t a = log_entries[i].address;
            uint8_t want = log_entries[i].old; /* unchanged by the reference unless logged */
            int k;
            for (k = (int)reference_end - 1; k >= 0; k--)
                if (reference[k].address == a) { want = reference_new[k]; break; }
            if (dead_stack(a, reference_sp)) continue;
            if (*byte_at(a) != want) {
                report_mismatch(port, "byte", a, want, *byte_at(a));
                mismatch = 1;
            }
        }
    }
    if (mismatch) s->mismatched++;
    else s->matched++;
    /* Continue on the reference result. */
    undo_log(port_start);
    for (i = 0; i < (int)reference_end; i++) *byte_at(reference[i].address) = reference_new[i];
    m68k_set_context(context_reference);
    SET_CYCLES(cycles_before - cycles_reference);
    if (poison) poison_dead(live);
    for (k = 0; k < reference_custom_count; k++)
        fa18_custom_write(fa18_machine, reference_custom[k].reg, reference_custom[k].value);
    free(reference_custom);
    free(reference);
    free(reference_new);
    (void)caller;
    return FA18_RET;
}


/* SHADOW: normally the port first, sandboxed on the state at the call (no chipset
 * events, hardware blocked, custom writes held, everything undone), then
 * the generated routine live, exactly as a run without ports would do it,
 * with its writes recorded. The two results are compared and the game
 * continues on the live one, so a shadow run keeps the plain run's timing.
 * A call is not compared when the live routine was interrupted, touched
 * hardware, or ended mid-routine at a frame boundary; nor when the port
 * touched hardware. An opted-in busy-input bridge instead runs source first,
 * records its DMACONR inputs, then compares C on entry RAM with those inputs.
 * The source result, comparison masks and exclusion rules remain the same. */
static int run_shadow(int function, int label, int port) {
    PortStats *s = &stats[port];
    int64_t saved_event = fa18_next_event;
    int cycles_before = GET_CYCLES(), r = FA18_RET, i, mismatch = 0, port_hardware = 0;
    int replay_busy = fa18_ports[port].shadow_busy_reads;
    size_t port_count = 0, port_custom_count = 0, k;
    CustomWrite *port_custom = NULL;
    uint32_t caller = fa18_bus_read32(REG_A[7]) & 0xFFFFFF, live_sp;
    const FA18CallLiveness *live = liveness_after(caller);
    report_caller = caller;
    LogEntry *port_writes = NULL;
    uint8_t *port_new = NULL;

    /* The port, sandboxed. */
    m68k_get_context(context_before);
    log_count = 0;
    custom_count = 0;
    if (replay_busy) {
        if (!shadow_entry_ram) shadow_entry_ram = malloc(sizeof *shadow_entry_ram);
        if (!shadow_live_ram) shadow_live_ram = malloc(sizeof *shadow_live_ram);
        if (!shadow_entry_ram || !shadow_live_ram) {
            fputs("shadow: cannot allocate RAM snapshots\n", stderr); abort();
        }
        save_shadow_ram(shadow_entry_ram);
        busy_port = port; busy_count = busy_cursor = 0; busy_phase = 1;
    } else {
        fa18_write_log_active = 1;
        fa18_write_log_hardware = 0;
        fa18_next_event = INT64_MAX;
        r = run_port_body(port);
        fa18_next_event = saved_event;
        fa18_write_log_active = 0;
        port_hardware = fa18_write_log_hardware;
        m68k_get_context(context_reference); /* here: the port's registers */
        port_count = log_count;
        port_writes = malloc(sizeof *port_writes * (port_count + 1));
        port_new = malloc(port_count + 1);
        memcpy(port_writes, log_entries, sizeof *port_writes * port_count);
        for (i = 0; i < (int)port_count; i++) port_new[i] = *byte_at(port_writes[i].address);
        port_custom_count = custom_count;
        port_custom = malloc(sizeof *port_custom * (custom_count + 1));
        memcpy(port_custom, custom_log, sizeof *port_custom * custom_count);
        undo_log(0);
        m68k_set_context(context_before);
        SET_CYCLES(cycles_before);
    }

    /* The generated routine, live. */
    custom_count = 0;
    dma_clear();
    fa18_write_log_active = 2;
    fa18_write_log_hardware = 0;
    {
        uint32_t sp = REG_A[7] + 4;
        int live_r = fa18_recomp_functions[function].fn(label);
        /* Chipset work due mid-routine: service it and carry on, as the
         * dispatcher would, to the routine's own return. */
        if (live_r == FA18_EXIT_INTERP && fa18_machine_event_due()) live_r = fa18_recomp_resume(caller, sp);
        fa18_write_log_active = 0;
        if (live_r != FA18_RET) {
            busy_phase = 0;
            s->incomplete++;
            log_count = 0;
            free(port_writes); free(port_new); free(port_custom);
            return live_r;
        }
    }
    if (fa18_write_log_hardware || port_hardware) {
        busy_phase = 0;
        s->hardware++;
        log_count = 0;
        free(port_writes); free(port_new); free(port_custom);
        return FA18_RET;
    }
    if (replay_busy) {
        size_t live_count = log_count, live_custom_count = custom_count;
        LogEntry *live_writes = malloc(sizeof *live_writes * (live_count + 1));
        CustomWrite *live_custom = malloc(sizeof *live_custom * (live_custom_count + 1));
        unsigned char *live_cpu = malloc(m68k_context_size());
        int live_cycles = GET_CYCLES(), port_touched_hardware;
        int64_t live_event = fa18_next_event;
        if (!live_writes || !live_custom || !live_cpu) {
            fputs("shadow: cannot allocate live result\n", stderr); abort();
        }
        memcpy(live_writes, log_entries, sizeof *live_writes * live_count);
        memcpy(live_custom, custom_log, sizeof *live_custom * live_custom_count);
        m68k_get_context(live_cpu);
        save_shadow_ram(shadow_live_ram);
        restore_shadow_ram(shadow_entry_ram);
        m68k_set_context(context_before); SET_CYCLES(cycles_before);
        log_count = custom_count = 0;
        busy_phase = 2;
        fa18_write_log_active = 1; fa18_write_log_hardware = 0;
        fa18_next_event = INT64_MAX;
        r = run_port_body(port);
        fa18_next_event = live_event;
        fa18_write_log_active = 0; busy_phase = 0;
        port_touched_hardware = fa18_write_log_hardware;
        m68k_get_context(context_reference);
        port_count = log_count; port_custom_count = custom_count;
        port_writes = malloc(sizeof *port_writes * (port_count + 1));
        port_new = malloc(port_count + 1);
        port_custom = malloc(sizeof *port_custom * (port_custom_count + 1));
        if (!port_writes || !port_new || !port_custom) {
            fputs("shadow: cannot allocate port result\n", stderr); abort();
        }
        memcpy(port_writes, log_entries, sizeof *port_writes * port_count);
        for (k = 0; k < port_count; ++k) port_new[k] = *byte_at(port_writes[k].address);
        memcpy(port_custom, custom_log, sizeof *port_custom * port_custom_count);
        restore_shadow_ram(shadow_live_ram);
        m68k_set_context(live_cpu); SET_CYCLES(live_cycles);
        memcpy(log_entries, live_writes, sizeof *live_writes * live_count);
        memcpy(custom_log, live_custom, sizeof *live_custom * live_custom_count);
        log_count = live_count; custom_count = live_custom_count;
        free(live_writes); free(live_custom); free(live_cpu);
        s->busy_input_calls++; s->busy_input_reads += busy_count;
        if (busy_cursor != busy_count) {
            report_mismatch(port, "unconsumed busy inputs", 0, (uint32_t)busy_count, (uint32_t)busy_cursor);
            mismatch = 1;
        }
        if (port_touched_hardware) {
            /* Source was repeatable: a new unsupported native read is a
             * mismatch, rather than a reason to discard this comparison. */
            report_mismatch(port, "extra hardware input", 0, 0, 1);
            mismatch = 1;
        }
    }
    live_sp = REG_A[7];
    s->compared++;
    s->reference_cycles += (uint64_t)(cycles_before - GET_CYCLES());

    if (r != FA18_RET) {
        report_mismatch(port, "glue did not return", 0, 0, (uint32_t)r);
        mismatch = 1;
    } else {
        unsigned char *now = malloc(m68k_context_size());
        uint32_t want_sr, got_sr, want_pc, got_pc, flag_mask = sr_flag_mask(live), got[16];
        /* Registers and flags the caller can observe (liveness table):
         * "reference" is the live generated run, "port" the sandboxed one. */
        m68k_get_context(now);
        for (i = 0; i < 16; i++) got[i] = REG_DA[i];
        want_sr = m68k_get_reg(NULL, M68K_REG_SR);
        want_pc = m68k_get_reg(NULL, M68K_REG_PC);
        m68k_set_context(context_reference);
        got_sr = m68k_get_reg(NULL, M68K_REG_SR);
        got_pc = m68k_get_reg(NULL, M68K_REG_PC);
        for (i = 0; i < 16; i++) {
            uint32_t want = got[i], mask = register_mask(live, i);
            if ((REG_DA[i] ^ want) & mask) {
                report_mismatch(port, i < 8 ? "D" : "A", (uint32_t)(i & 7), want, REG_DA[i]);
                mismatch = 1;
            }
        }
        m68k_set_context(now);
        free(now);
        if ((want_sr ^ got_sr) & (0xFF00 | flag_mask)) {
            report_mismatch(port, "SR", 0, want_sr, got_sr);
            mismatch = 1;
        }
        if (got_pc != want_pc) {
            report_mismatch(port, "PC", 0, want_pc, got_pc);
            mismatch = 1;
        }
        /* Hardware: the same custom-register writes in the same order. */
        if (custom_count != port_custom_count) {
            report_mismatch(port, "custom write count", 0, (uint32_t)custom_count, (uint32_t)port_custom_count);
            mismatch = 1;
        }
        for (k = 0; !mismatch && k < custom_count; k++) {
            if (custom_log[k].reg != port_custom[k].reg || custom_log[k].value != port_custom[k].value) {
                report_mismatch(port, "custom write", custom_log[k].reg, custom_log[k].reg << 16 | custom_log[k].value,
                                port_custom[k].reg << 16 | port_custom[k].value);
                mismatch = 1;
            }
        }
        /* Memory: every byte either run wrote must end with the same value,
         * except the dead stack below the returned-to stack pointer. A byte
         * the port left alone keeps its value from before the call. */
        for (i = 0; !mismatch && i < (int)log_count; i++) {
            uint32_t a = log_entries[i].address;
            uint8_t want = *byte_at(a), have = log_entries[i].old;
            int j, first = 1;
            for (j = 0; j < i; j++)
                if (log_entries[j].address == a) { first = 0; break; }
            if (!first || dead_stack(a, live_sp) || dma_written(a)) continue;
            for (j = (int)port_count - 1; j >= 0; j--)
                if (port_writes[j].address == a) { have = port_new[j]; break; }
            if (have != want) {
                report_mismatch(port, "byte", a, want, have);
                mismatch = 1;
            }
        }
        for (i = 0; !mismatch && i < (int)port_count; i++) {
            uint32_t a = port_writes[i].address;
            int j, later = 0, written = 0;
            for (j = i + 1; j < (int)port_count; j++)
                if (port_writes[j].address == a) { later = 1; break; }
            if (later || dead_stack(a, live_sp) || dma_written(a)) continue;
            for (j = 0; j < (int)log_count; j++)
                if (log_entries[j].address == a) { written = 1; break; }
            if (!written && port_new[i] != *byte_at(a)) {
                report_mismatch(port, "byte", a, *byte_at(a), port_new[i]);
                mismatch = 1;
            }
        }
    }
    if (mismatch) s->mismatched++;
    else s->matched++;
    log_count = 0;
    if (poison) poison_dead(live);
    free(port_writes);
    free(port_new);
    free(port_custom);
    (void)caller;
    return FA18_RET;
}

/* Called by the runtime for every entry into a generated routine. */
/* Observed call edges (return address, routine), for liveness of routines
 * reached through jump tables and indirect calls. */
#define EDGE_SLOTS 65536
static uint64_t edges[EDGE_SLOTS];

static void note_edge(int function) {
    uint32_t ret = fa18_bus_read32(REG_A[7]) & 0xFFFFFF;
    uint64_t key = (uint64_t)ret << 32 | (uint32_t)(function + 1);
    uint32_t h = (uint32_t)((key * 0x9E3779B97F4A7C15ull) >> 48) & (EDGE_SLOTS - 1);
    while (edges[h] && edges[h] != key) h = (h + 1) & (EDGE_SLOTS - 1);
    edges[h] = key;
}

int fa18_recomp_write_edges(const char *path) {
    FILE *out = fopen(path, "w");
    int i, first = 1;
    if (!out) return 0;
    fputs("[", out);
    for (i = 0; i < EDGE_SLOTS; i++) {
        if (!edges[i]) continue;
        fprintf(out, "%s[\"%06X\", \"%06X\"]", first ? "" : ", ", (uint32_t)(edges[i] >> 32),
                fa18_recomp_functions[(uint32_t)edges[i] - 1].entry);
        first = 0;
    }
    fputs("]", out);
    fputc(10, out);
    fclose(out);
    return 1;
}

int fa18_ports_enter(int function, int label, int via_call) {
    int port;
    int tail_call;
    int call_entry = via_call || entered_by_call() || entered_from_stepped_call();
    if (REG_PC == fa18_recomp_functions[function].entry) {
        profile[function]++;
        if (REG_PC == FA18_LOOP_UPDATE_ENTRY) {
            /* A resumption where the update stopped at its first instruction
             * is the same pass. */
            if (fa18_recomp_stop_pc == REG_PC && fa18_recomp_stop_sp == REG_A[7]) fa18_recomp_stop_pc = 0;
            else fa18_loop_iteration();
        }
        if (call_entry) note_edge(function);
    }
    port = port_of_function[function];
    tail_call = port >= 0 && fa18_ports[port].tail_from != 0 &&
                REG_PPC == fa18_ports[port].tail_from &&
                fa18_bus_read16(REG_PPC) == 0x4ED4; /* JMP (A4) */
    if (port < 0 || mode == FA18_PORTS_OFF || fa18_write_log_active || REG_PC != fa18_ports[port].entry ||
        (!call_entry && !tail_call))
        return fa18_recomp_functions[function].fn(label);
    stats[port].calls++;
    if (mode == FA18_PORTS_SHADOW) return run_shadow(function, label, port);
    if (mode == FA18_PORTS_SANDBOX) return run_sandbox(function, label, port);
    return run_glue(port);
}

long fa18_ports_report(const char *path) {
    FILE *out = path ? fopen(path, "w") : NULL;
    long bad = 0;
    int i;
    if (out) fputs("[\n", out);
    for (i = 0; i < fa18_port_count; i++) {
        const PortStats *s = &stats[i];
        bad += (long)s->mismatched;
        if (out)
            fprintf(out,
                    "  {\"entry\": \"%06X\", \"name\": \"%s\", \"calls\": %llu, \"compared\": %llu, "
                    "\"matched\": %llu, \"mismatched\": %llu, \"hardware\": %llu, \"incomplete\": %llu, "
                    "\"mean_cycles\": %llu, \"busy_input_calls\": %llu, \"busy_input_reads\": %llu}%s\n",
                    fa18_ports[i].entry, fa18_ports[i].name, (unsigned long long)s->calls,
                    (unsigned long long)s->compared, (unsigned long long)s->matched,
                    (unsigned long long)s->mismatched, (unsigned long long)s->hardware,
                    (unsigned long long)s->incomplete,
                    (unsigned long long)(s->compared ? s->reference_cycles / s->compared : 0),
                    (unsigned long long)s->busy_input_calls, (unsigned long long)s->busy_input_reads,
                    i + 1 < fa18_port_count ? "," : "");
    }
    if (out) {
        fputs("]\n", out);
        fclose(out);
    }
    return bad;
}

int fa18_recomp_write_profile(const char *path) {
    FILE *out = fopen(path, "w");
    int f, first = 1;
    if (!out) return 0;
    fputs("{", out);
    for (f = 0; f < fa18_recomp_function_count; f++) {
        if (!profile[f]) continue;
        fprintf(out, "%s\"%06X\": %llu", first ? "" : ", ", fa18_recomp_functions[f].entry,
                (unsigned long long)profile[f]);
        first = 0;
    }
    fputs("}\n", out);
    fclose(out);
    return 1;
}
