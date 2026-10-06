/* Caller compatibility for the direct C indexed-record phase. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "flight_dynamics.h"
#include "glue_flight_record_calls.h"
#include "recomp_ports.h"
#include "memory.h"

void record_76_78_registers(gaddr r, int16_t m6c, int16_t m6e,
                            int16_t old76, int enabled); /* glue_batch15.c */

typedef struct { uint32_t saved_term; gaddr geometry, scene, table, face; } IndexedRecordCall;

static int call_indexed_record(const void *arguments) {
    const IndexedRecordCall *input = arguments;
    IndexedRecordWork work;
    uint32_t saved_d2 = input->saved_term;
    gaddr saved_a2 = input->geometry, saved_a3 = input->scene, saved_a4 = input->table, saved_a5 = input->face;

    update_dynamics_selected_record(&work);
    fa18_ports_note_native_edge(0xC25B66, 0xC13D84);
    D(0) = 0;
    record_76_78_registers(work.helper_record, work.helper_m6c,
                           work.helper_m6e, work.helper_old76,
                           work.helper_enabled);
    SET_W(D(0), rd_u16(work.record + 0x6E));
    SET_W(D(1), rd_u16(work.record + 0x6C));
    A(0) = work.record;
    D(2) = saved_d2;
    A(2) = saved_a2; A(3) = saved_a3; A(4) = saved_a4; A(5) = saved_a5;
    return glue_return();
}

int glue_schedule_indexed_record(void) {
    IndexedRecordCall input = {D(2), A(2), A(3), A(4), A(5)};
    return fa18_ports_schedule_native_child(call_indexed_record, &input, sizeof input, 18000);
}
