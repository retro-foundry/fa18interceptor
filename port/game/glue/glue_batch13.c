/* Glue for format_decimal, step_cockpit_slide and record_position_history. */
#include "glue.h"
#include "ports_glue.h"

#include "cockpit.h"
#include "control_records.h"
#include "globals.h"
#include "memory.h"
#include "numbers.h"

/* $C3267A: D0 value, D2.w digits - 1, D4.b nonzero = keep zeros, A2 end.
 * Leaves D1.w = the last digit character written, D2.w = -1 (DBRA) and D3
 * = the BCD shifted past the written digits. */
int glue_C3267A(void) {
    int count = (int)(uint16_t)D(2) + 1;
    uint32_t bcd;

    format_decimal(A(2), D(0), count, (D(4) & 0xFF) != 0);

    bcd = rd_u32(DISPLAY_VALUE_BCD);
    SET_W(D(1), ((count <= 8 ? (bcd >> (4 * (count - 1))) : 0) & 15) + '0');
    SET_W(D(2), 0xFFFF);
    D(3) = count >= 8 ? 0 : bcd >> (4 * count);
    return glue_return();
}

/* $C2559A: the original's register state depends on the path it took. */
int glue_C2559A(void) {
    int8_t animation = (int8_t)rd_u8(SLIDE_ANIMATION);
    int16_t origin = rd_s16(SPAN_ORIGIN), size = (int16_t)(origin < 0 ? -origin : origin);
    uint8_t step_before = rd_u8(SLIDE_STEP);
    uint16_t flags_before = rd_u16(COCKPIT_FLAGS);

    step_cockpit_slide();

    SET_B(D(0), animation);
    if (!animation) return glue_return();
    SET_W(D(1), size);
    if (size > 14) return glue_return();
    {
        int16_t index = (int16_t)((animation - 1) * 8);
        gaddr entry = SLIDE_TABLE + (gaddr)(int32_t)index;
        gaddr counts = rd_u32(entry);
        A(0) = SLIDE_TABLE;
        if ((int8_t)rd_u8(counts) > (int8_t)step_before) {
            int8_t step = (flags_before & 0x400) ? (int8_t)step_before : 0;
            int16_t rows = rd_s8(rd_u32(entry + 4) + (gaddr)(int32_t)step);
            A(1) = rd_u32(entry + 4);
            SET_W(D(2), step);
            SET_W(D(1), (int16_t)(rows * 8));
            D(0) = (uint32_t)(int32_t)(int16_t)(rows * 40);
        } else {
            A(1) = counts;
            SET_W(D(0), index);
            SET_B(D(1), rd_u8(counts));
        }
        D(4) = 3; /* request_cockpit_redraw's MOVEQ */
    }
    return glue_return();
}

/* $C2651E: the result is in the flags. A record mismatch leaves CMP.W's
 * flags, an event TST.B's, otherwise MOVE.B D0,+$3D's. A0 ends past the
 * slots copied, D3.w = the slot index * 4. */
int glue_C2651E(void) {
    uint16_t offset = rd_u16(SCRIPT_RECORD), follow = rd_u16(HISTORY_RECORD);
    int8_t index = (int8_t)rd_u8(HISTORY_NEXT);
    int copies = (int8_t)rd_u8(HISTORY_COUNT) < 5 ? 2 : 1;

    record_position_history();

    SET_W(D(0), offset);
    if (offset != follow) {
        uint32_t r = (uint32_t)offset - follow;
        FLAG_N = NFLAG_16(r);
        FLAG_Z = r & 0xFFFF;
        FLAG_V = VFLAG_SUB_16(follow, offset, r);
        FLAG_C = CFLAG_16(r);
        return glue_return();
    }
    A(1) = CONTROL_RECORDS + (gaddr)(int32_t)(int16_t)offset;
    if (rd_u8(POST_INPUT_EVENT)) {
        flags_logic_b(rd_u8(POST_INPUT_EVENT));
        return glue_return();
    }
    A(0) = HISTORY_SLOTS + (gaddr)(int32_t)(int16_t)(index * 12) + (gaddr)(12 * copies);
    SET_W(D(3), index * 4);
    SET_W(D(0), rd_u8(A(1) + 0x3D));
    flags_logic_b(rd_u8(A(1) + 0x3D));
    return glue_return();
}
