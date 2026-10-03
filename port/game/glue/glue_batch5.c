/* Glue for notify.c, control_records.c, screen_frame.c and render_span.c. */
#include "glue.h"
#include "glue_text.h"
#include "ports_glue.h"

#include "control_records.h"
#include "fixed_math.h"
#include "globals.h"
#include "memory.h"
#include "notify.h"
#include "render_span.h"
#include "screen_frame.h"

/* $C11B44: leaves its last byte result in D0.b. */
int glue_C11B44(void) {
    int8_t step = (int8_t)(rd_u8(NOTIFY_COUNTDOWN) - 1);

    tick_notification_cadence();

    if (step <= 0) SET_B(D(0), step);
    else if (step == 4) SET_B(D(0), 0);
    else if ((step & 3) == 2) SET_B(D(0), 0);
    else SET_B(D(0), step);
    return glue_return();
}

/* $C310E2: D7.w position, D1 cursor, A4.w limit -> D5 and its flags. The
 * original returns MOVEQ #-1/#0 (full D5) except on the partial path, where
 * D5's high word is the sign extension of 2 * SPAN_ORIGIN. */
void bound_span_registers(void) {
    int16_t position = (int16_t)D(7), limit = (int16_t)A(4), origin = rd_s16(SPAN_ORIGIN), result;
    int32_t cursor = (int32_t)D(1);
    int before_origin = (int32_t)position + origin < 0;
    int16_t start = (int16_t)(position + origin), remaining = (int16_t)(start + limit);

    result = bound_span(&position, limit, &cursor);

    D(1) = (uint32_t)cursor;
    D(5) = (uint32_t)(int32_t)result;
    if (before_origin) {
        SET_W(D(7), position);
    } else {
        D(7) = 0;
        if ((int32_t)remaining - 20 >= 0 && result != -1)
            D(5) = ((int16_t)(origin * 2) < 0 ? 0xFFFF0000u : 0) | (uint16_t)result;
    }
    flags_logic_w(D(5));
}

int glue_C310E2(void) {
    bound_span_registers();
    return glue_return();
}

/* $C1EBC0: D1 high byte selects the record -> D2, D3, D4 fields; A2 record;
 * D1.w keeps the doubled index. */
int glue_C1EBC0(void) {
    gaddr record = control_record((uint16_t)D(1));
    int32_t f0c, f10, f0e;

    read_record_fields(record, &f0c, &f10, &f0e);
    SET_W(D(1), (D(1) & 0xFF00) * 2);
    A(2) = record;
    D(2) = (uint32_t)f0c;
    D(3) = (uint32_t)f10;
    D(4) = (uint32_t)f0e;
    return glue_return();
}

/* $C230B0: leaves the selection word in D0.w, the record in A1, and N set
 * when there was no selection or it was dropped (V and C clear). */
int glue_C230B0(void) {
    int16_t selected = rd_s16(SELECTED_RECORD);
    int dropped;

    release_lost_selection();

    dropped = rd_s16(SELECTED_RECORD) < 0;
    SET_W(D(0), selected);
    if (selected >= 0) A(1) = CONTROL_RECORDS + (gaddr)(int32_t)selected;
    FLAG_N = (selected < 0 || dropped) ? NFLAG_SET : NFLAG_CLEAR;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
    return glue_return();
}

/* $C2DE96: A1 record; no register outputs. */
int glue_C2DE96(void) {
    settle_record(A(1));
    return glue_return();
}

/* $C0DAA0: D0.w/D1.w point indices, A1 cursor. Leaves both indices scaled
 * by 8, the four written words in D2-D5 and the table in A0. */
int glue_C0DAA0(void) {
    gaddr cursor = A(1);
    int16_t first = (int16_t)D(0), second = (int16_t)D(1);

    append_mirrored_points(&cursor, first, second);

    SET_W(D(0), first << 3);
    SET_W(D(1), second << 3);
    SET_W(D(2), rd_u16(A(1)));
    SET_W(D(3), rd_u16(A(1) + 2));
    SET_W(D(4), rd_u16(A(1) + 4));
    SET_W(D(5), rd_u16(A(1) + 6));
    A(0) = FRAME_POINTS;
    A(1) = cursor;
    return glue_return();
}

static int corner(int16_t x, int16_t y) {
    gaddr cursor = A(1);
    append_point(&cursor, x, y);
    A(1) = cursor;
    return glue_return();
}

int glue_C0DAD0(void) { return corner(0, 0); }
int glue_C0DAD4(void) { return corner(VIEW_RIGHT, 0); }
int glue_C0DADC(void) { return corner(VIEW_RIGHT, VIEW_BOTTOM); }
int glue_C0DAE6(void) { return corner(0, VIEW_BOTTOM); }
