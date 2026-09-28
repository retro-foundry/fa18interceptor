#include "recomp_runtime.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m68kcpu.h"
#include "bus.h"
#include "machine.h"
#include "recomp_ports.h"

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
    free(entry_map);
    free(code_bits);
    free(disabled);
    free(fallback_seen);
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
    trace_pc(REG_PC);
    if (enabled_flag) {
        int f = fold(REG_PC);
        if (f >= 0) {
            fa18_recomp_stats.interpreted_game++;
            fallback_seen[f / 2] = 1;
        }
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
