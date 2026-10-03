/* Glue for the cockpit's blitter pieces (hud_bars.c). Their callers read
 * every register: what the last bound_span, bar fill, image blit or marker
 * line left. The countdowns are read before the C runs; the register flow
 * is then replayed step by step (bound_span has no other effect, and the
 * blitter waits leave the registers alone). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "hud_bars.h"
#include "memory.h"
#include "glue_text.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void line_registers_to_row(int16_t last_row); /* glue_render_polygon.c */

static gaddr viewed_record(void) {
    return CONTROL_RECORDS + SEXT(rd_u16(VIEW_RECORD));
}

/* $C30CC4 from its entry registers. */
static void fill_registers(void) {
    A(2) = rd_u32(PAGE_PLANE_TABLE);
    D(4) = rd_u32(A(2) + SEXT(D(4)));
    if (W(5)) SET_W(D(3), 0xFFFF);
    if (W(7)) SET_W(D(0), 0xFFFF);
    SET_W(D(5), (uint16_t)(W(5) + W(7)));
    D(4) += D(1);
    SET_W(D(6), (uint16_t)(W(6) - W(5)));
    SET_W(D(5), (uint16_t)(W(5) * 2));
    SET_W(D(5), (uint16_t)(W(5) + (int16_t)A(5)));
    A(0) = 0xDFF000;
}

int glue_C30CC4(void) {
    fill_bar_words((uint16_t)D(2), (int16_t)D(4), D(1), W(5), W(7), (uint16_t)D(6), (int16_t)A(5), (uint16_t)D(0),
                   (uint16_t)D(3));
    fill_registers();
    return glue_return();
}

/* The loads before a bar's bound_span: 1 when the bar is drawn. */
static int bounded(uint32_t rows, int16_t words, uint16_t size, int16_t modulo, int16_t position) {
    D(1) = rows;
    A(4) = SEXT((uint16_t)words);
    SET_W(D(6), size);
    A(5) = SEXT((uint16_t)modulo);
    SET_W(D(7), (uint16_t)position);
    D(1) += rd_u32(REDRAW_STATE_LONG);
    bound_span_registers();
    return W(5) >= 0;
}

static void bar(uint16_t con0, uint16_t first_mask, uint16_t last_mask) {
    SET_W(D(2), con0);
    D(4) = 0xC;
    SET_W(D(0), first_mask);
    SET_W(D(3), last_mask);
    fill_registers();
}

/* x + SPAN_ORIGIN_Y into D`n`: 1 when the column is 0..last. */
static int column(int n, int16_t x, int16_t last) {
    int32_t across = (int32_t)x + rd_s16(SPAN_ORIGIN_Y);
    SET_W(D(n), (uint16_t)across);
    return across >= 0 && (int16_t)across <= last;
}

/* $C30EAA from its entry registers: D2.w BLTCON0, D0 mask, A1 plane image
 * pointers, D1 rows, D7.w position, A4 words, D6.w size, A5 modulo. */
static void image_registers(void) {
    int k;

    A(0) = 0;
    A(2) = rd_u32(PAGE_PLANE_TABLE);
    D(1) += rd_u32(REDRAW_STATE_LONG);
    bound_span_registers();
    if (W(5) < 0) {
        D(0) = 0xFFFFFFFFu;
        return;
    }
    SET_W(D(5), (uint16_t)(W(5) + W(7)));
    D(7) = SEXT((uint16_t)(W(7) * 2));
    SET_W(D(6), (uint16_t)(W(6) - W(5)));
    SET_W(D(5), (uint16_t)(W(5) * 2 + 1));
    D(2) &= 0xFFFF; /* SWAP, MOVE.W A0 (0), SWAP */
    D(5) = SEXT(D(5));
    while ((int32_t)D(1) < 40) {
        D(1) += 40;
        D(7) += 2 * A(4);
        SET_W(D(6), (uint16_t)(W(6) - 0x40));
        if (W(6) < 0) {
            D(0) = 0xFFFFFFFFu;
            return;
        }
    }
    for (k = 0; k < 4; k++) {
        D(4) = rd_u32(A(2)) + D(1);
        A(2) += 4;
        A(3) = rd_u32(A(1));
        A(1) += 4;
        D(3) = rd_u32(A(3)) + D(7);
        if (k == 0) {
            D(0) += D(7);
            A(0) = 0xDFF000;
            SET_W(D(5), (uint16_t)(W(5) - 1 + (int16_t)A(5)));
        }
    }
}

int glue_C30EAA(void) {
    blit_image((uint16_t)D(2), D(0), A(1), D(1), W(7), (int16_t)A(4), (uint16_t)D(6), (int16_t)A(5));
    image_registers();
    return glue_return();
}
