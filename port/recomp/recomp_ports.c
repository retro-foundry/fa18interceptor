/* Stage D port dispatch and SHADOW-mode proof (see recomp_ports.h). */
#include "recomp_ports.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m68kcpu.h"
#include "machine.h"
#include "recomp_runtime.h"

extern int64_t fa18_cycle_origin, fa18_next_event;

/* Write log used by SHADOW mode: every Chip/Slow byte written while active,
 * with its previous value. Hardware (custom/CIA) access while active is not
 * performed; it marks the call as not comparable. */
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
    int reported;
} PortStats;

static FA18PortMode mode;
static int *port_of_function; /* function id -> port index, or -1 */
static PortStats *stats;
static uint64_t *profile;
static unsigned char *context_before, *context_reference;

void fa18_ports_init(FA18PortMode new_mode, const char *only) {
    int i, f;
    mode = new_mode;
    free(port_of_function);
    free(stats);
    free(profile);
    port_of_function = malloc(sizeof(int) * (size_t)(fa18_recomp_function_count + 1));
    stats = calloc((size_t)fa18_port_count + 1, sizeof *stats);
    profile = calloc((size_t)fa18_recomp_function_count + 1, sizeof *profile);
    for (f = 0; f < fa18_recomp_function_count; f++) port_of_function[f] = -1;
    for (i = 0; i < fa18_port_count; i++) {
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

static int run_glue(int port) {
    int r = fa18_ports[port].glue();
    USE_CYCLES(fa18_ports[port].cycles);
    return r;
}

static void report_mismatch(int port, const char *what, uint32_t detail, uint32_t ref, uint32_t got) {
    PortStats *s = &stats[port];
    if (s->reported >= 8) return;
    s->reported++;
    fprintf(stderr, "port %s ($%06X) mismatch: %s %06X reference %08X port %08X (call %llu, caller $%06X)\n",
            fa18_ports[port].name, fa18_ports[port].entry, what, detail, ref, got,
            (unsigned long long)s->calls, fa18_bus_read32(REG_A[7]));
}

/* SHADOW: reference first, then the port on the same state; the game keeps
 * the reference result. */
static int run_shadow(int function, int label, int port) {
    PortStats *s = &stats[port];
    int64_t saved_event = fa18_next_event;
    int cycles_before = GET_CYCLES(), cycles_reference, r, i, mismatch = 0;
    size_t reference_end, port_start, reference_custom_count, k;
    CustomWrite *reference_custom;
    uint32_t caller = fa18_bus_read32(REG_A[7]);
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
    r = fa18_ports[port].glue();
    fa18_write_log_active = 0;
    s->compared++;
    s->reference_cycles += (uint64_t)cycles_reference;
    if (r != FA18_RET) {
        report_mismatch(port, "glue did not return", 0, 0, (uint32_t)r);
        mismatch = 1;
    } else {
        for (i = 0; i < 16; i++) {
            uint32_t want = ((m68ki_cpu_core *)context_reference)->dar[i];
            if (REG_DA[i] != want) {
                report_mismatch(port, i < 8 ? "D" : "A", (uint32_t)(i & 7), want, REG_DA[i]);
                mismatch = 1;
            }
        }
        {
            unsigned char *now = malloc(m68k_context_size());
            uint32_t want_sr, got_sr, want_pc;
            m68k_get_context(now);
            m68k_set_context(context_reference);
            want_sr = m68k_get_reg(NULL, M68K_REG_SR);
            want_pc = m68k_get_reg(NULL, M68K_REG_PC);
            m68k_set_context(now);
            got_sr = m68k_get_reg(NULL, M68K_REG_SR);
            free(now);
            if ((want_sr & 0xFF1F) != (got_sr & 0xFF1F)) {
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
        /* Memory: every byte either run wrote must end with the same value. */
        for (i = 0; i < (int)reference_end; i++) {
            uint8_t got = *byte_at(reference[i].address);
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
    for (k = 0; k < reference_custom_count; k++)
        fa18_custom_write(fa18_machine, reference_custom[k].reg, reference_custom[k].value);
    free(reference_custom);
    free(reference);
    free(reference_new);
    (void)caller;
    return FA18_RET;
}

/* Called by the runtime for every entry into a generated routine. */
int fa18_ports_enter(int function, int label, int via_call) {
    int port;
    if (REG_PC == fa18_recomp_functions[function].entry) profile[function]++;
    port = port_of_function[function];
    if (port < 0 || mode == FA18_PORTS_OFF || fa18_write_log_active || REG_PC != fa18_ports[port].entry ||
        (!via_call && !entered_by_call()))
        return fa18_recomp_functions[function].fn(label);
    stats[port].calls++;
    if (mode == FA18_PORTS_SHADOW) return run_shadow(function, label, port);
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
                    "\"mean_cycles\": %llu}%s\n",
                    fa18_ports[i].entry, fa18_ports[i].name, (unsigned long long)s->calls,
                    (unsigned long long)s->compared, (unsigned long long)s->matched,
                    (unsigned long long)s->mismatched, (unsigned long long)s->hardware,
                    (unsigned long long)s->incomplete,
                    (unsigned long long)(s->compared ? s->reference_cycles / s->compared : 0),
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
