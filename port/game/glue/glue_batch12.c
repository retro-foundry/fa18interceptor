/* Glue for update_view_octant, paired_record_ready and attitude_term. */
#include "glue.h"
#include "ports_glue.h"

#include "control_records.h"
#include "globals.h"
#include "memory.h"
#include "view.h"

/* $C254E8: leaves the angle in D0 (the rotate word, or the whole heading
 * long) and the octant in D1 (MOVEQ). */
int glue_C254E8(void) {
    update_view_octant();
    if (rd_u8(CONTEXT_SELECT)) SET_W(D(0), rd_u16(VIEW_ROTATE));
    else D(0) = rd_u32(HEADING_ANGLE);
    D(1) = rd_u8(VIEW_OCTANT);
    return glue_return();
}

/* $C231A2: A2 record -> D0 = 1 ready / 0 not (MOVEQ flags); A3 = the record
 * table when the partner was examined. */
int glue_C231A2(void) {
    gaddr record = A(2);
    int reached_partner = !(rd_u16(record) & 0x8700) && !(rd_u8(record + 0x20) & 0x02) &&
                          !rd_u8(PAIR_OVERRIDE) && (rd_u8(record + 0x01) & 0x40) &&
                          (int8_t)rd_u8(record + 0x38) < 0;

    D(0) = (uint32_t)paired_record_ready(record);
    if (reached_partner) A(3) = CONTROL_RECORDS;
    flags_logic_l(D(0));
    return glue_return();
}
