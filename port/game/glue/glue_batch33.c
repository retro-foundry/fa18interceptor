/* Glue for the pixel plots $C2F5F4, $C2F60A, $C2F66E. Their callers read
 * every register: the masks, plane addresses and writer the shared body
 * leaves (it ends with a JMP through A4 into a per-colour writer that only
 * stores). */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "plot.h"

/* The shared body at $C2F688 with A3 = `masks`, A4 = `writers`, from the
 * D0.w/D1.w already in the registers. */
void plot_registers(gaddr masks, gaddr writers);
void plot_registers(gaddr masks, gaddr writers) {
    int16_t x = (int16_t)D(0), y = (int16_t)D(1);
    uint16_t mask, offset, row40, colour;
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
    colour = rd_u16(CURRENT_COLOUR) & 15;
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

int glue_C2F5F4(void) {
    plot_pixel((int16_t)D(0), (int16_t)D(1));
    plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
    return glue_return();
}

/* $C2F60A at a word boundary plots x then x - 1 through $C2F5F4, with D0/D1
 * restored sign-extended from the stack in between. */
void pair_registers(void);
void pair_registers(void) {
    if ((D(0) & 15) == 0) {
        uint32_t x = D(0), y = D(1);
        plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
        D(0) = (uint32_t)(int32_t)(int16_t)x;
        D(1) = (uint32_t)(int32_t)(int16_t)y;
        FLAG_X = ((uint16_t)D(0) == 0) ? XFLAG_SET : XFLAG_CLEAR; /* SUBQ.W #1 */
        SET_W(D(0), (uint16_t)(D(0) - 1));
        plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
        return;
    }
    SET_W(D(2), (uint16_t)(D(0) & 15));
    plot_registers(PAIR_MASKS, PLOT_ROWS_1);
}

int glue_C2F60A(void) {
    plot_pixel_pair((int16_t)D(0), (int16_t)D(1));
    pair_registers();
    return glue_return();
}
