/* Glue for the pixel plots $C2F5F4, $C2F60A, $C2F66E. Their callers read
 * every register: the masks, plane addresses and writer the shared body
 * leaves (it ends with a JMP through A4 into a per-colour writer that only
 * stores). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "plot.h"
#include "glue_text.h"

/* The shared body at $C2F688 with A3 = `masks`, A4 = `writers`, from the
 * D0.w/D1.w already in the registers. */
void plot_registers_colour(gaddr masks, gaddr writers, uint16_t colour);
void plot_registers_colour(gaddr masks, gaddr writers, uint16_t colour) {
    int16_t x = (int16_t)D(0), y = (int16_t)D(1);
    uint16_t mask, offset, row40;
    uint8_t planes;
    uint32_t sum;
    int k;

    A(1) = rd_u32(PAGE_PLANE_TABLE);
    A(3) = masks;
    A(4) = writers;
    if (y <= 0) {
        D(2) = 0xFFFFFFFFu;
        flags_logic_l(D(2));
        return;
    }
    colour &= 15;
    A(4) = rd_u32(writers + (gaddr)(4 * colour));
    mask = rd_u16(masks + (gaddr)(2 * (x & 15)));
    row40 = (uint16_t)(y * 40);
    offset = (uint16_t)((int16_t)(x & 0xFFF0) >> 3);
    sum = (uint32_t)offset + row40;
    FLAG_X = (sum & 0x10000) ? XFLAG_SET : XFLAG_CLEAR; /* ADD.W D1,D2 */
    offset = (uint16_t)sum;
    for (k = 0; k < 4; k++) A(k) = rd_u32(rd_u32(PAGE_PLANE_TABLE) + (gaddr)(4 * k)) + (gaddr)(int32_t)(int16_t)offset;
    planes = rd_u8(LINE_PLANES);
    for (k = 0; k < 4; k++) {
        SET_W(D(k), (planes & (1 << k)) ? (uint16_t)~mask : 0xFFFF);
        SET_W(D(4 + k), (planes & (1 << k)) ? mask : 0);
    }
    if (rd_s16(LINE_COLOUR) >= 0) {
        D(0) &= 0xFFFF;
        if (rd_u8(POINT_XOR_PLANES) & 15) D(0) = 0xFFFFFFFFu;
    }
}

void plot_registers(gaddr masks, gaddr writers) {
    plot_registers_colour(masks, writers, rd_u16(CURRENT_COLOUR));
}



/* $C2F60A at a word boundary plots x then x - 1 through $C2F5F4, with D0/D1
 * restored sign-extended from the stack in between. */
void pair_registers_colour(uint16_t colour);
void pair_registers_colour(uint16_t colour) {
    if ((D(0) & 15) == 0) {
        uint32_t x = D(0), y = D(1);
        plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, colour);
        D(0) = (uint32_t)(int32_t)(int16_t)x;
        D(1) = (uint32_t)(int32_t)(int16_t)y;
        FLAG_X = ((uint16_t)D(0) == 0) ? XFLAG_SET : XFLAG_CLEAR; /* SUBQ.W #1 */
        SET_W(D(0), (uint16_t)(D(0) - 1));
        plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, colour);
        return;
    }
    SET_W(D(2), (uint16_t)(D(0) & 15));
    plot_registers_colour(PAIR_MASKS, PLOT_ROWS_1, colour);
}

void pair_registers(void) {
    pair_registers_colour(rd_u16(CURRENT_COLOUR));
}



/* $C2F5D4: the body with D0.w/D1.w pushed round it and popped back (the
 * words only). */
void restored_plot_registers(void) {
    uint16_t x = (uint16_t)D(0), y = (uint16_t)D(1);
    plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
    SET_W(D(1), y);
    SET_W(D(0), x);
    flags_logic_w(D(0));
}



/* $C2F5C0: D0 moved by SPAN_ORIGIN_Y and D1 by REDRAW_STATE_WORD first;
 * a column outside 0..319 returns D2 = -1. */
void plot_in_view_registers(void) {
    int32_t across = (int32_t)(int16_t)D(0) + rd_s16(SPAN_ORIGIN_Y);
    SET_W(D(0), (uint16_t)across);
    if (across < 0 || (int16_t)across >= 0x140) {
        D(2) = 0xFFFFFFFFu;
        flags_logic_l(D(2));
        return;
    }
    SET_W(D(1), (uint16_t)(D(1) + rd_u16(REDRAW_STATE_WORD)));
    restored_plot_registers();
}



/* $C2F64E: the body with the pair masks and two-row writers, D0.w/D1.w
 * pushed round it. */
void square_registers(void) {
    uint16_t x = (uint16_t)D(0), y = (uint16_t)D(1);
    plot_registers(PAIR_MASKS, PLOT_ROWS_2);
    SET_W(D(1), y);
    SET_W(D(0), x);
    flags_logic_w(D(0));
}



/* $C2F63A: moved by the view's origin first; a column outside 0..318
 * returns D2 = -1. */
void square_in_view_registers(void) {
    int32_t across = (int32_t)(int16_t)D(0) + rd_s16(SPAN_ORIGIN_Y);
    SET_W(D(0), (uint16_t)across);
    if (across < 0 || (int16_t)across >= 0x13F) {
        D(2) = 0xFFFFFFFFu;
        flags_logic_l(D(2));
        return;
    }
    SET_W(D(1), (uint16_t)(D(1) + rd_u16(REDRAW_STATE_WORD)));
    square_registers();
}
