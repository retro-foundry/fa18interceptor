#include "recomp_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m68kcpu.h"
#include "m68kops.h"
#include "bus.h"
#include "machine.h"
#include "recomp_ports.h"
#include "graphics_glue.h"
#include "exec_glue.h"

extern int64_t fa18_cycle_origin;

int fa18_ports_enter(int function, int label, int via_call);

int fa18_recomp_abort;
FA18RecompStats fa18_recomp_stats;

/* Per-word entry index (+1) and per-byte "translated" bits for Chip and Slow
 * RAM. Addresses are folded into one 1 MiB space: Chip at 0, Slow at 512K. */
#define SPACE (FA18_CHIP_SIZE + FA18_SLOW_SIZE)
static uint32_t *entry_map;
static uint8_t *code_bits;
static uint8_t *disabled;
static uint8_t *fallback_seen;
static int enabled_flag;
static int vbeam_shim_enabled;
static int exec_interrupt_shim_enabled;
#define ROM_TRANSITION_SLOTS 65536u
typedef struct {
    uint64_t key; /* source PC in bits 47..24, ROM entry PC in bits 23..0; zero means empty */
    uint64_t count;
} RomTransition;
static RomTransition *rom_transitions;
static uint64_t rom_transitions_dropped;
uint32_t fa18_recomp_stop_pc, fa18_recomp_stop_sp;
static int depth;

static int fold(uint32_t a) {
    a &= 0xFFFFFF;
    if (a < FA18_CHIP_SIZE) return (int)a;
    if (a >= FA18_SLOW_BASE && a < FA18_SLOW_BASE + FA18_SLOW_SIZE) return (int)(a - FA18_SLOW_BASE + FA18_CHIP_SIZE);
    return -1;
}

void fa18_recomp_init(int enabled) {
    int i;
    enabled_flag = enabled;
    vbeam_shim_enabled = 0;
    exec_interrupt_shim_enabled = 0;
    free(entry_map);
    free(code_bits);
    free(disabled);
    free(fallback_seen);
    free(rom_transitions);
    rom_transitions = NULL;
    rom_transitions_dropped = 0;
    entry_map = calloc(SPACE / 2, sizeof *entry_map);
    code_bits = calloc(SPACE / 8, 1);
    disabled = calloc((size_t)fa18_recomp_function_count + 1, 1);
    fallback_seen = calloc(SPACE / 2, 1);
    memset(&fa18_recomp_stats, 0, sizeof fa18_recomp_stats);
    if (!enabled) return;
    for (i = 0; i < fa18_recomp_entry_count; i++) {
        int f = fold(fa18_recomp_entries[i].pc);
        if (f >= 0) entry_map[f / 2] = (uint32_t)i + 1;
    }
    for (i = 0; i < fa18_recomp_span_count; i++) {
        uint32_t a;
        for (a = fa18_recomp_spans[i].start; a < fa18_recomp_spans[i].end; a++) {
            int f = fold(a);
            if (f >= 0) code_bits[f >> 3] |= (uint8_t)(1u << (f & 7));
        }
    }
}

int fa18_recomp_enable_vbeam_shim(void) {
    vbeam_shim_enabled = fa18_os_vbeam_signature_matches(fa18_machine->rom);
    return vbeam_shim_enabled;
}

int fa18_recomp_enable_exec_interrupt_shim(void) {
    exec_interrupt_shim_enabled = fa18_os_exec_interrupt_signature_matches(fa18_machine->rom);
    return exec_interrupt_shim_enabled;
}

int fa18_recomp_track_rom_transitions(void) {
    rom_transitions = calloc(ROM_TRANSITION_SLOTS, sizeof *rom_transitions);
    return rom_transitions != NULL;
}

static void note_rom_transition(uint32_t source, uint32_t target) {
    uint64_t key;
    uint32_t slot, probes;
    if (!rom_transitions || target < 0xF80000u || source >= 0xF80000u) return;
    key = ((uint64_t)(source & 0xFFFFFFu) << 24) | (target & 0xFFFFFFu);
    if (!key) return;
    slot = (uint32_t)((key * 0x9E3779B97F4A7C15ull) >> 48);
    for (probes = 0; probes < ROM_TRANSITION_SLOTS && rom_transitions[slot].key && rom_transitions[slot].key != key;
         probes++)
        slot = (slot + 1u) & (ROM_TRANSITION_SLOTS - 1u);
    if (probes == ROM_TRANSITION_SLOTS) { rom_transitions_dropped++; return; }
    rom_transitions[slot].key = key;
    rom_transitions[slot].count++;
}

int fa18_recomp_write_rom_transitions(const char *path) {
    FILE *out = fopen(path, "w");
    uint32_t i;
    int first = 1;
    if (!out || !rom_transitions || rom_transitions_dropped) {
        if (out) fclose(out);
        return 0;
    }
    fputs("[\n", out);
    for (i = 0; i < ROM_TRANSITION_SLOTS; i++) {
        uint64_t key = rom_transitions[i].key;
        if (!key) continue;
        fprintf(out, "%s  {\"source\": \"%06X\", \"rom_entry\": \"%06X\", \"count\": %llu}",
                first ? "" : ",\n", (unsigned)(key >> 24), (unsigned)(key & 0xFFFFFFu),
                (unsigned long long)rom_transitions[i].count);
        first = 0;
    }
    fputs("\n]\n", out);
    return fclose(out) == 0;
}

void fa18_recomp_note_write(uint32_t address, int size) {
    int n;
    if (!enabled_flag) return;
    for (n = 0; n < size; n++) {
        int f = fold(address + (uint32_t)n);
        int i;
        if (f < 0 || !(code_bits[f >> 3] & (1u << (f & 7)))) continue;
        /* Translated bytes changed: stop using every routine that covers them. */
        fa18_recomp_stats.code_writes++;
        code_bits[f >> 3] &= (uint8_t)~(1u << (f & 7));
        for (i = 0; i < fa18_recomp_span_count; i++) {
            const FA18RecompSpan *s = &fa18_recomp_spans[i];
            uint32_t a = (address + (uint32_t)n) & 0xFFFFFF;
            if (a >= s->start && a < s->end && !disabled[s->function]) {
                disabled[s->function] = 1;
                fa18_recomp_stats.disabled_functions++;
                if (fa18_recomp_stats.disabled_functions <= 20)
                    fprintf(stderr, "recomp: write to $%06X invalidates %06X\n", a,
                            fa18_recomp_functions[s->function].entry);
            }
        }
        fa18_recomp_abort = 1;
    }
}

void fa18_recomp_begin_slice(void) {}
int fa18_recomp_pending_cycles(void) { return 0; }

int fa18_recomp_invoke(int function, int label, uint32_t pc) {
    int r;
    if (disabled[function] || pc != fa18_recomp_functions[function].entry || depth > 4000)
        return FA18_EXIT_DISPATCH;
    depth++;
    r = fa18_ports_enter(function, label, 1);
    depth--;
    return r;
}

static const FA18RecompEntry *lookup(uint32_t pc) {
    int f = fold(pc);
    uint32_t i;
    if (f < 0 || (f & 1)) return NULL;
    i = entry_map[f / 2];
    if (!i) return NULL;
    if (disabled[fa18_recomp_entries[i - 1].function]) return NULL;
    return &fa18_recomp_entries[i - 1];
}

int fa18_recomp_call_dynamic(void) {
    const FA18RecompEntry *e = lookup(REG_PC);
    int r;
    if (!e || depth > 4000) return FA18_EXIT_DISPATCH;
    depth++;
    r = fa18_ports_enter((int)e->function, (int)e->label, 1);
    depth--;
    return r;
}

static long trace_left = -1;

static int slice_hpos(void) {
    int v, h;
    fa18_machine_beam(&v, &h);
    return h;
}

static void trace_pc(unsigned int pc) {
    char text[128];
    if (trace_left < 0) {
        const char *env = getenv("FA18_TRACE");
        trace_left = env ? atol(env) : 0;
    }
    if (!trace_left) return;
    trace_left--;
    m68k_disassemble(text, pc, M68K_CPU_TYPE_68000);
    {
        int r;
        fprintf(stderr, "T %06X", pc);
        for (r = 0; r < 16; r++) fprintf(stderr, " %08X", REG_DA[r]);
        fprintf(stderr, " %04X %d %d | %s | %lld\n", m68k_get_reg(NULL, M68K_REG_SR), fa18_machine->vpos,
                slice_hpos(), text, (long long)(fa18_cycle_origin - GET_CYCLES()));
    }
}

void fa18_machine_instruction_hook(unsigned int pc) {
    const FA18RecompEntry *e;
    (void)pc;
    for (;;) {
        int before, r;
        fa18_bus_finish(REG_PC);
        fa18_bus_instruction();
        if (fa18_machine_service()) break;
        fa18_bus_instruction();
        if (vbeam_shim_enabled && fa18_os_vbeam_step()) continue;
        if (exec_interrupt_shim_enabled && fa18_os_exec_interrupt_step()) continue;
        if (!enabled_flag || (e = lookup(REG_PC)) == NULL) break;
        before = GET_CYCLES();
        fa18_recomp_abort = 0;
        fa18_recomp_stats.dispatches++;
        depth = 1;
        r = fa18_ports_enter((int)e->function, (int)e->label, 0);
        depth = 0;
        fa18_recomp_stats.generated_cycles += (uint64_t)(before - GET_CYCLES());
        /* EXIT_INTERP at a due chipset event resumes after servicing. */
        if (r == FA18_EXIT_INTERP && !fa18_machine_event_due()) break;
    }
    fa18_bus_begin(REG_PC); /* the interpreter's opcode fetch follows */
    note_rom_transition(REG_PPC, REG_PC);
    trace_pc(REG_PC);
    if (enabled_flag) {
        int f = fold(REG_PC);
        if (f >= 0) {
            fa18_recomp_stats.interpreted_game++;
            fallback_seen[f / 2] = 1;
        }
    }
}

/* Carry a routine that stopped for due chipset work on to its return (to
 * `ret`, stack pointer `sp`), servicing and dispatching exactly as the
 * instruction hook would. FA18_RET once there; otherwise what stopped it
 * (a frame end, or code only the interpreter can run), with the machine
 * where the hook would have it. */
/* Instructions the translation hands to the interpreter (recomp.py
 * classify: RTE, STOP, RESET, TRAP, ILLEGAL, BKPT, line A and F). */
static int interpreter_only(uint16_t op) {
    return op == 0x4E73 || op == 0x4E72 || op == 0x4E70 || (op & 0xFFF0) == 0x4E40 || op == 0x4AFC ||
           (op & 0xFFF8) == 0x4848 || (op & 0xF000) == 0xA000 || (op & 0xF000) == 0xF000;
}

int fa18_recomp_resume(uint32_t ret, uint32_t sp) {
    for (;;) {
        const FA18RecompEntry *e;
        uint32_t pc;
        uint16_t op;
        int r;
        if ((REG_PC & 0xFFFFFF) == ret && REG_A[7] == sp) return FA18_RET;
        fa18_bus_finish(REG_PC);
        fa18_bus_instruction();
        if (fa18_machine_service()) return FA18_EXIT_INTERP;
        fa18_bus_instruction();
        if (!enabled_flag) return FA18_EXIT_INTERP;
        if ((e = lookup(REG_PC)) != NULL) {
            fa18_recomp_abort = 0;
            r = fa18_ports_enter((int)e->function, (int)e->label, 0);
            if (r == FA18_EXIT_INTERP && !fa18_machine_event_due()) return r;
            continue;
        }
        /* Between labels: one instruction, as generated code executes it. */
        pc = REG_PC;
        op = fa18_bus_read16(pc);
        if (interpreter_only(op)) return FA18_EXIT_INTERP;
        fa18_bus_begin(pc);
        fa18_bus_fetch(pc);
        REG_PPC = pc;
        REG_PC = pc + 2;
        REG_IR = op;
        m68ki_instruction_jump_table[op]();
        USE_CYCLES(CYC_INSTRUCTION[op]);
    }
}

int fa18_recomp_write_fallback_log(const char *path) {
    FILE *out = fopen(path, "w");
    int i, first = 1;
    if (!out) return 0;
    fputs("[", out);
    for (i = 0; i < SPACE / 2; i++) {
        uint32_t a;
        if (!fallback_seen[i]) continue;
        a = (uint32_t)i * 2;
        a = a < FA18_CHIP_SIZE ? a : a - FA18_CHIP_SIZE + FA18_SLOW_BASE;
        fprintf(out, "%s\"%06X\"", first ? "" : ",", a);
        first = 0;
    }
    fputs("]\n", out);
    fclose(out);
    return 1;
}
