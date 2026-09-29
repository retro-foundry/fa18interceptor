/* Glue for prepare_polygon $C301F6. Every register and flag is live after
 * it, so its register flow is replayed here (without drawing) on the real
 * registers; the callees' leftovers come from their glue helpers. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "render_line.h"
#include "render_polygon.h"

void polygon_edge_registers(uint16_t size);        /* glue_render_polygon.c */
void line_registers(void);                          /* glue_render_polygon.c */
void plot_registers(gaddr masks, gaddr writers);    /* glue_batch33.c */
void pair_registers(void);                          /* glue_batch33.c */

#define W(n) ((int16_t)D(n))
#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))

static void exchange(int a, int b) {
    uint32_t t = D(a);
    D(a) = D(b);
    D(b) = t;
}

static void abs_word(int n) {
    if (W(n) < 0) SET_W(D(n), (uint16_t)-W(n));
}

/* One polygon edge's leftovers, tracking the last BLTSIZE written. */
static void edge(uint16_t *last_size) {
    LineSetup e;
    if (setup_line(W(0), W(1), W(2), W(3), (int16_t)A(4), 0, 1, &e)) *last_size = e.size;
    polygon_edge_registers(*last_size);
}

/* The registers prepare_polygon leaves (after it ran); returns its D0.
 * `last_size` is the BLTSIZE written before it. */
int prepare_registers(uint16_t last_size);

static int done(uint32_t d0) {
    D(0) = d0;
    flags_logic_l(d0);
    return (int)d0;
}

int prepare_registers(uint16_t last_size) {
    gaddr v = POLY_VERTICES + 2;
    int16_t count;
    int i;

    A(4) = SEXT(rd_u16(LINE_LAST_ROW));
    SET_W(D(6), rd_u16(POLY_VERTICES) - 3);
    for (i = 0; i < 6; i++) D(i) = SEXT(rd_u16(v + (gaddr)(2 * i)));
    A(0) = v + 12;
    if (W(2) < W(0)) exchange(0, 2);
    if (W(4) < W(0)) SET_W(D(0), D(4)); else if (W(4) > W(2)) SET_W(D(2), D(4));
    if (W(3) < W(1)) exchange(1, 3);
    if (W(5) < W(1)) SET_W(D(1), D(5)); else if (W(5) > W(3)) SET_W(D(3), D(5));
    for (;;) {
        SET_W(D(6), W(6) - 1);
        if (W(6) < 0) break;
        SET_W(D(4), rd_u16(A(0)));
        SET_W(D(5), rd_u16(A(0) + 2));
        A(0) += 4;
        if (!(W(4) > W(0))) SET_W(D(0), D(4)); else if (W(2) < W(4)) SET_W(D(2), D(4));
        if (!(W(5) > W(1))) SET_W(D(1), D(5)); else if (W(3) < W(5)) SET_W(D(3), D(5));
    }
    if (W(1) > (int16_t)A(4)) return done(1);
    SET_W(D(7), W(3) - W(1));
    abs_word(7);
    SET_W(D(6), W(2) - W(0));
    abs_word(6);

    if (W(7) > 2 ? W(6) > 1 : W(7) == 2 ? W(6) > 2 : 0) goto fill;
    if (W(7) > 2 || (W(7) < 2 && W(6) > 1)) {
        uint32_t style = rd_u32(LINE_STYLE);
        if (!rd_u8(KEEP_LINE_STYLE)) fa18_bus_write32(LINE_STYLE, 0x000FFFFFu);
        line_registers();
        fa18_bus_write32(LINE_STYLE, style);
        return done(1);
    }
    SET_W(D(1), W(1) + 1);
    if (W(1) > (int16_t)A(4)) return done(1);
    if (W(7) == 2) {
        SET_W(D(0), D(2));
        if (W(1) >= rd_s16(LINE_LAST_ROW)) pair_registers();
        else plot_registers(PAIR_MASKS, PLOT_ROWS_2);
    } else {
        SET_W(D(6), W(6) - 1);
        if (W(6) >= 0) {
            SET_W(D(0), D(2));
            pair_registers();
        } else {
            plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
        }
    }
    return done(1);

fill:
    SET_W(D(0), W(0) - 1);
    if (W(0) < 0) SET_W(D(0), 0);
    exchange(1, 2);
    A(2) = POLY_VERTICES + 2;
    A(3) = POLY_EDGES_LEFT;
    A(0) = 0xDFF000u;
    count = (int16_t)(rd_s16(POLY_VERTICES) - 1);
    do {
        for (i = 0; i < 4; i++) D(i) = SEXT(rd_u16(A(2) + (gaddr)(2 * i)));
        A(2) += 4;
        edge(&last_size);
        count--;
    } while (count > 0);
    SET_W(D(0), rd_u16(A(2)));
    A(2) += 2;
    SET_W(D(1), rd_u16(A(2)));
    D(2) = SEXT(rd_u16(POLY_VERTICES + 2));
    D(3) = SEXT(rd_u16(POLY_VERTICES + 4));
    edge(&last_size);

    D(1) = SEXT(rd_u16(POLY_MIN_X));
    D(3) = SEXT(rd_u16(POLY_MAX_X));
    SET_W(D(3), W(3) >> 4);
    SET_W(D(2), D(3));
    SET_W(D(1), W(1) >> 4);
    SET_W(D(3), W(3) - W(1));
    SET_W(D(6), D(3));
    SET_W(D(3), 0x27 - 2 * W(3));
    SET_W(D(5), rd_u16(POLY_MAX_Y));
    SET_W(D(1), D(5));
    SET_W(D(7), D(5));
    SET_W(D(1), W(1) << 3);
    SET_W(D(0), D(1));
    SET_W(D(0), W(0) << 2);
    SET_W(D(1), W(1) + W(0));
    SET_W(D(2), W(2) * 2);
    D(2) = SEXT(D(2));
    D(1) += D(2);
    D(2) = D(1);
    if (W(7) > (int16_t)A(4)) {
        SET_W(D(7), W(7) - (int16_t)A(4));
        SET_W(D(0), D(7));
        SET_W(D(7), W(7) << 3);
        SET_W(D(4), D(7));
        SET_W(D(4), W(4) << 2);
        SET_W(D(7), W(7) + W(4));
        D(7) = SEXT(D(7));
        D(1) -= D(7);
    } else {
        D(0) = 0;
        D(7) = 0;
    }
    D(1) = D(2) + rd_u32(POLY_MASK_PLANE) - D(7);
    D(2) = D(1);
    SET_W(D(5), W(5) - rd_s16(POLY_MIN_Y) + 1);
    SET_W(D(7), D(5));
    SET_W(D(6), W(6) + 1);
    SET_W(D(7), W(7) - W(0));
    SET_W(D(7), (uint16_t)((uint16_t)D(7) << 6));
    SET_W(D(7), W(7) + W(6));
    A(0) = 0xDFF000u;
    return done(0);
}

int glue_C301F6(void) {
    uint16_t last_size = custom_written(BLTSIZE);
    (void)prepare_polygon();
    (void)prepare_registers(last_size);
    return glue_return();
}
