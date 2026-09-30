/* Register bridge for the indexed control-record update ($C13D84). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "indexed_record_update.h"
#include "memory.h"

void record_76_78_registers(gaddr r, int16_t m6c, int16_t m6e,
                            int16_t old76, int enabled); /* glue_batch15.c */

int glue_C13D84(void) {
    IndexedRecordWork work;
    uint32_t saved_d2 = D(2);
    gaddr saved_a2 = A(2), saved_a3 = A(3), saved_a4 = A(4), saved_a5 = A(5);

    update_indexed_record(&work);
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
