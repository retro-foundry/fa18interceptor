/* Glue for the faces $C09952 and $C099F6 and the view marks $C332FE,
 * $C30918 and $C345A0. Their callers read nearly every register: what the clipper or
 * the last plot leaves, with the counters and pointers the routines
 * walk. Each runs the C, then replays the register flow (the plots and the
 * clipper from the state they saw). */
#include "glue.h"
#include "ports_glue.h"

#include "faces.h"
#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "view_marks.h"
#include "glue_clip.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void pair_registers(void); /* glue_batch33.c */

/* JSR $C2F60A with D0-D3 (or D0-D1) saved by MOVEM.W around it. */
static void saved_pair(int saved) {
    uint32_t keep[4];
    int i;
    for (i = 0; i < saved; i++) keep[i] = D(i);
    pair_registers();
    for (i = 0; i < saved; i++) D(i) = SEXT(keep[i]);
}

static void subq_word(int n, uint16_t by) {
    FLAG_X = (uint16_t)D(n) < by ? XFLAG_SET : XFLAG_CLEAR;
    SET_W(D(n), (uint16_t)(D(n) - by));
}

static void addq_word(int n, uint16_t by) {
    FLAG_X = (uint32_t)(uint16_t)D(n) + by > 0xFFFF ? XFLAG_SET : XFLAG_CLEAR;
    SET_W(D(n), (uint16_t)(D(n) + by));
}



int glue_C30918(void) {
    int16_t shown = rd_s16(GAUGE_SHOWN);
    int row;

    draw_gauge_bar();
    SET_W(D(2), (uint16_t)((rd_u16(GAUGE_SOURCE) & 0x7FFF) >> 10));
    if (!((int8_t)rd_u8(GAUGE_REFRESH) > 0) && W(2) == shown) return glue_return();
    SET_W(D(3), 0x15);
    SET_W(D(0), 0x70 + rd_u16(SPAN_ORIGIN_Y));
    if (W(0) <= 0 || W(0) > 0x13C) return glue_return();
    SET_W(D(1), 0xB4 + rd_u16(REDRAW_STATE_WORD));
    for (row = 0; row < 22; row++) {
        uint32_t keep[4];
        int i;
        subq_word(2, 1);
        saved_pair(4);
        for (i = 0; i < 4; i++) keep[i] = D(i);
        addq_word(0, 2);
        pair_registers();
        for (i = 0; i < 4; i++) D(i) = SEXT(keep[i]);
        subq_word(1, 1);
        SET_W(D(3), (uint16_t)(D(3) - 1)); /* DBRA */
    }
    return glue_return();
}

/* The copy loop's leftovers, then the clipper's with A2 kept round it. */






/* plot_ring $C345A0: the table walk in D0/D1/A0 (MOVE.B keeps the upper
 * bytes), the plots' leftovers, and D2 from the window test. */
void plot_registers(gaddr masks, gaddr writers); /* glue_batch33.c */
