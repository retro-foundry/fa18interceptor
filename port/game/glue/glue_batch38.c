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

int glue_C332FE(void) {
    draw_view_marker();
    SET_W(D(0), 0xA0 + rd_u16(SPAN_ORIGIN_Y));
    if (W(0) <= 1 || W(0) >= 0x13D) return glue_return();
    SET_W(D(1), 0x81 + rd_u16(REDRAW_STATE_WORD));
    saved_pair(2);
    addq_word(1, 1);
    saved_pair(2);
    addq_word(1, 1);
    saved_pair(2);
    subq_word(0, 1);
    addq_word(1, 1);
    saved_pair(2);
    addq_word(0, 2);
    pair_registers();
    return glue_return();
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
static void face_tail(ClipperSnapshot *snapshot, uint16_t colour, int drawn, gaddr after) {
    if ((int16_t)D(6) < 0) {
        D(0) = 0;
        flags_logic_l(0);
    } else {
        clipper_registers(snapshot, colour, drawn);
    }
    A(2) = after;
}

int glue_C09952(void) {
    gaddr face = A(2);
    int16_t count = rd_s16(face), reads = count < 3 ? 3 : count, i;
    uint16_t colour = rd_u16(face + 2 + (gaddr)(2 * reads));
    ClipperSnapshot snapshot;
    int drawn;

    clipper_snapshot(&snapshot);
    drawn = draw_indexed_face(&face);
    SET_W(D(7), (uint16_t)count);
    A(0) = CLIP_INPUT + 4 + (gaddr)(6 * reads);
    A(3) = WORKSPACES;
    for (i = 0; i < reads; i++) {
        SET_W(D(1), rd_u16(A(2) + 2 + (gaddr)(2 * i)));
        A(4) = WORKSPACES + SEXT(D(1)) + 4;
        if (i == 0) SET_W(D(6), rd_u16(A(4)));
        else SET_W(D(6), (uint16_t)(D(6) & rd_u16(A(4))));
    }
    SET_W(D(7), (uint16_t)(count < 3 ? count - 4 : -1));
    face_tail(&snapshot, colour, drawn, face);
    return glue_return();
}

int glue_C099F6(void) {
    gaddr face = A(2);
    int16_t count = rd_s16(face);
    uint32_t copies = (uint32_t)(uint16_t)(count - 3) + 4, i;
    ClipperSnapshot snapshot;
    int drawn;

    clipper_snapshot(&snapshot);
    drawn = draw_outlined_face(&face);
    A(0) = CLIP_INPUT + 4 + 6 * copies;
    A(3) = WORKSPACES + SEXT(rd_u16(A(2) + 2));
    for (i = 0; i < copies; i++) {
        uint16_t z = rd_u16(A(3) + 4);
        SET_W(D(6), i ? (uint16_t)(D(6) & z) : z);
        A(3) += 6;
    }
    SET_W(D(7), 0xFFFF); /* DBRA */
    if ((int16_t)D(6) >= 0) {
        /* The replay sees the line style the clipper drew with. */
        SET_W(D(1), 3);
        SET_W(D(2), 3);
        fa18_bus_write32(LINE_STYLE, 0x00030003u);
        face_tail(&snapshot, 7, drawn, face);
        fa18_bus_write32(LINE_STYLE, 0x000FFFFFu); /* as the C left it */
        SET_W(D(1), 0xF);
        SET_W(D(2), 0xFFFF);
    } else {
        face_tail(&snapshot, 7, drawn, face);
    }
    return glue_return();
}

/* plot_ring $C345A0: the table walk in D0/D1/A0 (MOVE.B keeps the upper
 * bytes), the plots' leftovers, and D2 from the window test. */
void plot_registers(gaddr masks, gaddr writers); /* glue_batch33.c */

static int ring_quarter_regs(int16_t *points, int16_t x, int16_t y, int16_t shift, int clipped,
                             int forward, int sx, int sy) {
    for (;;) {
        if (forward) {
            SET_B(D(0), rd_u8(A(0)));
            SET_B(D(1), rd_u8(A(0) + 1));
            A(0) += 2;
            if ((int8_t)D(1) < 0) return 1;
        } else {
            A(0) -= 2;
            SET_B(D(1), rd_u8(A(0) + 1));
            SET_B(D(0), rd_u8(A(0)));
            if ((int8_t)D(0) < 0) return 1;
        }
        if (sx < 0) SET_B(D(0), (uint8_t)-(int8_t)D(0));
        if (sy < 0) SET_B(D(1), (uint8_t)-(int8_t)D(1));
        else if (!clipped) SET_B(D(1), (uint8_t)(D(1) - 1));
        SET_W(D(0), (uint16_t)(int8_t)D(0));
        SET_W(D(1), (uint16_t)(int8_t)D(1));
        SET_W(D(0), (uint16_t)(D(0) + (uint16_t)x));
        SET_W(D(1), (uint16_t)(D(1) + (uint16_t)y));
        if (!clipped || (W(0) > 14 && W(0) < 0x132 && W(0) > (int16_t)(0x55 + shift)
                         && W(0) < (int16_t)(0xE9 + shift) && W(1) > 0x2D && W(1) < 0x90)) {
            uint32_t a0 = A(0);
            plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
            A(0) = a0;
        }
        if (--*points < 0) return 0;
    }
}

void ring_registers(void) {
    int16_t x = W(0), y = W(1), points = W(2), shift = rd_s16(SPAN_ORIGIN_Y);
    int clipped = 1;

    if (x > 14 && x < 0x132) {
        SET_W(D(2), (uint16_t)(0x63 + shift));
        if (x > W(2)) {
            SET_W(D(2), (uint16_t)(0xDB + shift));
            clipped = !(x < W(2) && y > 0x39 && y < 0x84);
        }
    }
    if (clipped) {
        SET_W(D(0), (uint16_t)(0x55 + shift));
        SET_W(D(0), (uint16_t)(0xE9 + shift));
    }
    if (!ring_quarter_regs(&points, x, y, shift, clipped, 1, 1, -1)) return;
    A(0) -= 2;
    if (!ring_quarter_regs(&points, x, y, shift, clipped, 0, 1, 1)) return;
    A(0) += 2;
    if (!ring_quarter_regs(&points, x, y, shift, clipped, 1, -1, 1)) return;
    A(0) -= 2;
    ring_quarter_regs(&points, x, y, shift, clipped, 0, -1, -1);
}

int glue_C345A0(void) {
    plot_ring(W(0), W(1), W(2), A(0));
    ring_registers();
    return glue_return();
}
