/* Source-order drawing heads of the postflight tuple and fixed-point variants. */
#include "postflight_variants.h"

#include "globals.h"
#include "memory.h"
#include "plot.h"
#include "render_line.h"

#define POSTFLIGHT_TUPLES 0xC3128Au

static int16_t add_word(int16_t a, int16_t b) {
    return (int16_t)((uint16_t)a + (uint16_t)b);
}

static int horizontal_visible(int16_t x) {
    return x >= 0 && x < 0x140;
}

void draw_postflight_tuple_pairs(void) {
    gaddr tuple = POSTFLIGHT_TUPLES;
    int pair;
    wr_u16(CURRENT_COLOUR, 3);
    for (pair = 0; pair < 2; ++pair, tuple += 8) {
        int16_t x0 = add_word(rd_s16(tuple), rd_s16(SPAN_ORIGIN_Y));
        int16_t x1 = add_word(rd_s16(tuple + 4), rd_s16(SPAN_ORIGIN_Y));
        int16_t y0, y1;
        if (!horizontal_visible(x0) || !horizontal_visible(x1)) continue;
        y0 = add_word(rd_s16(tuple + 2), rd_s16(REDRAW_STATE_WORD));
        y1 = add_word(rd_s16(tuple + 6), rd_s16(REDRAW_STATE_WORD));
        draw_line_to_row(x0, y0, x1, y1, 0xC7);
    }
}

void draw_postflight_fixed_quad(void) {
    int16_t x, y;
    x = add_word(0x9D, rd_s16(SPAN_ORIGIN_Y));
    if (!horizontal_visible(x)) return;
    y = add_word(0xA8, rd_s16(REDRAW_STATE_WORD));
    wr_u16(CURRENT_COLOUR, 0xC);
    plot_pixel(x, y);

    x = add_word(0x9F, rd_s16(SPAN_ORIGIN_Y));
    if (!horizontal_visible(x)) return;
    y = add_word(0xA8, rd_s16(REDRAW_STATE_WORD));
    plot_pixel(x, y);

    x = add_word(0x9E, rd_s16(SPAN_ORIGIN_Y));
    y = add_word(0xA8, rd_s16(REDRAW_STATE_WORD));
    plot_pixel(x, y);
    y = add_word(0xA7, rd_s16(REDRAW_STATE_WORD));
    plot_pixel(x, y);
}
