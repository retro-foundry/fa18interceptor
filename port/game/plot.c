#include "plot.h"

#include "globals.h"

#define ROW_BYTES 40

/* The four plane words at (x's word, y), plane table order. */
static void plane_words(int16_t x, int16_t y, gaddr word[4]) {
    gaddr table = rd_u32(PAGE_PLANE_TABLE);
    int16_t offset = (int16_t)((int16_t)((int16_t)(x & 0xFFF0) >> 3) + (int16_t)(y * ROW_BYTES));
    int k;
    for (k = 0; k < 4; k++) word[k] = rd_u32(table + (gaddr)(4 * k)) + (gaddr)(int32_t)offset;
}

static void plot(int16_t x, int16_t y, gaddr masks, int rows) {
    uint16_t mask, colour = rd_u16(CURRENT_COLOUR) & 15;
    uint8_t planes = rd_u8(LINE_PLANES);
    gaddr word[4];
    int bit, row;

    if (y <= 0) return;
    mask = rd_u16(masks + (gaddr)(2 * (x & 15)));
    plane_words(x, y, word);
    if (rd_s16(LINE_COLOUR) >= 0) {
        uint8_t toggle = rd_u8(POINT_XOR_PLANES);
        if (toggle & 15) {
            for (bit = 0; bit < 4; bit++) {
                gaddr w = word[3 - bit];
                if ((toggle & (1 << bit)) && (planes & (1 << bit))) wr_u16(w, (uint16_t)(rd_u16(w) ^ mask));
            }
            return;
        }
    }
    for (bit = 0; bit < 4; bit++) {
        if (!(planes & (1 << bit))) continue;
        for (row = 0; row < rows; row++) {
            gaddr w = word[3 - bit] + (gaddr)(ROW_BYTES * row);
            if (colour & (1 << bit)) wr_u16(w, (uint16_t)(rd_u16(w) | mask));
            else wr_u16(w, (uint16_t)(rd_u16(w) & ~mask));
        }
    }
}

void plot_pixel(int16_t x, int16_t y) {
    plot(x, y, PIXEL_MASKS, 1);
}

void plot_pixel_pair(int16_t x, int16_t y) {
    if ((x & 15) == 0) {
        plot_pixel(x, y);
        plot_pixel((int16_t)(x - 1), y);
        return;
    }
    plot(x, y, PAIR_MASKS, 1);
}

void plot_pixel_block(int16_t x, int16_t y) {
    if (y >= rd_s16(LINE_LAST_ROW)) {
        plot_pixel_pair(x, y);
        return;
    }
    plot(x, y, PAIR_MASKS, 2);
}
