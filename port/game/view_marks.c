/* Pixel-pair marks over the view. */
#include "view_marks.h"

#include "globals.h"
#include "memory.h"
#include "plot.h"

void draw_view_marker(void) {
    int16_t x = (int16_t)(0xA0 + rd_s16(SPAN_ORIGIN_Y)), y;

    if (x <= 1 || x >= 0x13D) return;
    y = (int16_t)(0x81 + rd_s16(REDRAW_STATE_WORD));
    wr_u16(CURRENT_COLOUR, 8);
    plot_pixel_pair(x, y);
    plot_pixel_pair(x, (int16_t)(y + 1));
    plot_pixel_pair(x, (int16_t)(y + 2));
    plot_pixel_pair((int16_t)(x - 1), (int16_t)(y + 3));
    plot_pixel_pair((int16_t)(x + 1), (int16_t)(y + 3));
}

void draw_gauge_bar(void) {
    int16_t level = (int16_t)((rd_u16(GAUGE_SOURCE) & 0x7FFF) >> 10), x, y, row;

    if ((int8_t)rd_u8(GAUGE_REFRESH) > 0) {
        wr_s16(GAUGE_SHOWN, level);
    } else {
        if (level == rd_s16(GAUGE_SHOWN)) return;
        if (rd_u8(DISPLAY_FORCE) & 1) wr_s16(GAUGE_SHOWN, level);
    }
    x = (int16_t)(0x70 + rd_s16(SPAN_ORIGIN_Y));
    if (x <= 0 || x > 0x13C) return;
    y = (int16_t)(0xB4 + rd_s16(REDRAW_STATE_WORD));
    for (row = 0; row < 22; row++, y--) {
        wr_u16(CURRENT_COLOUR, level - 1 - row > 0 ? 4 : 0);
        plot_pixel_pair(x, y);
        plot_pixel_pair((int16_t)(x + 2), y);
    }
}
