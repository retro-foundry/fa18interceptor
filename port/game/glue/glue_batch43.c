/* Glue for aim_view $C2D9BA. Every path ends in the same matrix calls and
 * the closing MOVEM.W, so what its caller reads is their leftovers: D0-D2
 * the player's orientation words, D3 the scaled matrix's last row first
 * product, and $C2E346's registers, each over the upper words the
 * calls before left. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "view.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))

void y_rotation8_registers(int16_t angle, gaddr out); /* glue_batch10.c */
void two_angle_registers(void);                         /* glue_batch19.c */

/* $C091E0's D7 in a context view: z times inverse word 5 of the record
 * followed (chosen as aim_view chooses it). */
static void follow_registers(void) {
    int16_t offset = rd_s16(VIEW_RECORD), ahead;
    gaddr record = CONTROL_RECORDS;
    if (offset) record += (gaddr)(int32_t)offset;
    else {
        offset = rd_s16(CONTEXT_RECORD);
        if ((rd_u8(record + (gaddr)(int32_t)offset + 1) & 0x40) && rd_u8(record + (gaddr)(int32_t)offset + 0x62) == 0x30)
            record += (gaddr)(int32_t)offset;
    }
    ahead = (rd_u8(record + 0x62) & 0xF0) == 0x30 ? -4 : rd_u8(CONTEXT_STATE) == 6 ? 5 : 1;
    D(7) = (uint32_t)((int32_t)ahead * rd_s16(record + RECORD_INVERSE + 10));
}

int glue_C2D9BA(void) {
    int k;
    if (rd_u8(CONTEXT_STARTED)) follow_registers();
    aim_view();
    /* $C2E38E, $C2E5AC (its products fit a word, so they come back
     * sign-extended), then $C2E346: each leaves upper words for the next. */
    SET_W(D(0), rd_u16(VIEW_PAN));
    SET_W(D(2), rd_u16(VIEW_ROTATE));
    two_angle_registers();
    for (k = 0; k < 3; k++) D(3 + k) = SEXT(rd_u16(VIEW_ANGLE_MATRIX + 12 + (gaddr)(2 * k)));
    SET_W(D(4), rd_u16(VIEW_ROTATE));
    y_rotation8_registers(rd_s16(VIEW_ROTATE), LIST_MATRIX);
    for (k = 0; k < 3; k++) D(k) = SEXT(rd_u16(CONTROL_RECORDS + 0x66 + (gaddr)(2 * k)));
    return glue_return();
}
