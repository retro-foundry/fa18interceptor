/* Reference-only update-dispatch observation. Build with --replace-source
 * port/recomp/recomp_ports.c=tools/native/original_update_entry_probe.c.
 * The wrapper forwards every argument/result and reads storage directly;
 * it never delivers input, issues guest bus accesses or changes game state. */
#define fa18_ports_enter original_ports_enter
#include "../../port/recomp/recomp_ports.c"
#undef fa18_ports_enter

static FILE *entry_probe;
static unsigned long probe_rows;

static void close_entry_probe(void) {
    if (entry_probe && fclose(entry_probe)) exit(2);
    entry_probe = NULL;
}

static uint32_t stored_long(uint32_t address) {
    const uint8_t *bytes;
    if (address < FA18_CHIP_SIZE - 3) bytes = fa18_machine->chip + address;
    else if (address >= FA18_SLOW_BASE && address < FA18_SLOW_BASE + FA18_SLOW_SIZE - 3)
        bytes = fa18_machine->slow + address - FA18_SLOW_BASE;
    else abort();
    return (uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
           (uint32_t)bytes[2] << 8 | bytes[3];
}

static void observe_entry(const char *kind, int via_call, int result) {
    if (++probe_rows > 200000 || fprintf(entry_probe,
        "%s,%ld,%llu,%06X,%06X,%08X,%08X,%06X,%08X,%d,%d\n",
        kind, fa18_loop_iterations(), (unsigned long long)fa18_machine->frame,
        REG_PC, REG_PPC, REG_A[7], stored_long(REG_A[7]),
        fa18_recomp_stop_pc, fa18_recomp_stop_sp, via_call, result) < 0)
        abort();
}

int fa18_ports_enter(int function, int label, int via_call) {
    static int initialized;
    int result, update = REG_PC == FA18_LOOP_UPDATE_ENTRY;
    if (update && !initialized) {
        const char *path = getenv("FA18_UPDATE_ENTRY_PROBE");
        initialized = 1;
        if (!path || !*path || !(entry_probe = fopen(path, "w"))) abort();
        if (fputs("kind,iteration,frame,pc,ppc,sp,stack_long,stop_pc,stop_sp,via_call,result\n",
                  entry_probe) == EOF) abort();
        atexit(close_entry_probe);
    }
    if (update) observe_entry("before", via_call, 0);
    result = original_ports_enter(function, label, via_call);
    if (update) observe_entry("after", via_call, result);
    return result;
}
