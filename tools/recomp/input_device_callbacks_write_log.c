/* Dispatch-fixture reference: use the production source-input capture policy
 * while measuring whether the original call touches unreplayable hardware.
 * The complete original CPU call still supplies the reference result. */
#include "structural_write_log.c"

void fa18_structural_source_input_begin(uint32_t entry) {
    int i;
    if (mode != FA18_PORTS_OFF || busy_phase || fa18_write_log_active != 2) abort();
    for (i = 0; i < fa18_port_count; ++i) if (fa18_ports[i].entry == entry) {
        if (fa18_ports[i].shadow_busy_reads) {
            busy_port = i;
            busy_count = busy_cursor = 0;
            busy_phase = 1;
        }
        return;
    }
    abort();
}

void fa18_structural_source_input_end(void) {
    busy_phase = 0;
}
