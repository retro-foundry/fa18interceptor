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

typedef struct {
    int16_t x, y, points, shift;
    int clipped;
    gaddr at;
} Ring;

static int ring_inside(const Ring *r, int16_t x, int16_t y) {
    return x > 14 && x < 0x132 && x > (int16_t)(0x55 + r->shift) && x < (int16_t)(0xE9 + r->shift)
        && y > 0x2D && y < 0x90;
}

/* One quarter: the pairs forward or backward to the sentinel, dx mirrored
 * by `sx`, dy by `sy` (the lift only below the centre). 0 once the points
 * run out. */
static int ring_quarter(Ring *r, int forward, int sx, int sy) {
    for (;;) {
        int8_t dx, dy;
        int16_t x, y;
        if (forward) {
            dx = (int8_t)rd_u8(r->at);
            dy = (int8_t)rd_u8(r->at + 1);
            r->at += 2;
            if (dy < 0) return 1;
        } else {
            dy = (int8_t)rd_u8(r->at - 1);
            dx = (int8_t)rd_u8(r->at - 2);
            r->at -= 2;
            if (dx < 0) return 1;
        }
        x = (int16_t)(r->x + (int8_t)(sx < 0 ? -dx : dx));
        y = (int16_t)(r->y + (int8_t)(sy < 0 ? -dy : dy - !r->clipped));
        if (!r->clipped || ring_inside(r, x, y)) plot_pixel(x, y);
        if (--r->points < 0) return 0;
    }
}

void plot_ring(int16_t x, int16_t y, int16_t points, gaddr outline) {
    Ring r;

    r.x = x;
    r.y = y;
    r.points = points;
    r.shift = rd_s16(SPAN_ORIGIN_Y);
    r.at = outline;
    r.clipped = !(x > 14 && x < 0x132 && x > (int16_t)(0x63 + r.shift) && x < (int16_t)(0xDB + r.shift)
                  && y > 0x39 && y < 0x84);
    if (!ring_quarter(&r, 1, 1, -1)) return;   /* upper right */
    r.at -= 2;
    if (!ring_quarter(&r, 0, 1, 1)) return;    /* lower right, back up the table */
    r.at += 2;
    if (!ring_quarter(&r, 1, -1, 1)) return;   /* lower left */
    r.at -= 2;
    ring_quarter(&r, 0, -1, -1);               /* upper left */
}

void plot_ring_point(int16_t x, int16_t y, int16_t steps, gaddr outline) {
    int16_t shift = rd_s16(SPAN_ORIGIN_Y), px, py;
    gaddr at = outline;
    int8_t dx, dy;

    for (;;) { /* upper right */
        dx = (int8_t)rd_u8(at);
        dy = (int8_t)rd_u8(at + 1);
        at += 2;
        if (dy < 0) break;
        dy = (int8_t)-dy;
        if (--steps < 0) goto found;
    }
    at -= 2;
    y = (int16_t)(y - 1);
    for (;;) { /* lower right, back up the table */
        at -= 2;
        dy = (int8_t)rd_u8(at + 1);
        dx = (int8_t)rd_u8(at);
        if (dx < 0) break;
        if (--steps < 0) goto found;
    }
    at += 2;
    for (;;) { /* lower left */
        dx = (int8_t)rd_u8(at);
        dy = (int8_t)rd_u8(at + 1);
        at += 2;
        if (dy < 0) break;
        dx = (int8_t)-dx;
        if (--steps < 0) goto found;
    }
    at -= 2;
    y = (int16_t)(y + 1);
    for (;;) { /* upper left */
        at -= 2;
        dy = (int8_t)rd_u8(at + 1);
        dx = (int8_t)rd_u8(at);
        if (dx < 0) return;
        dx = (int8_t)-dx;
        dy = (int8_t)-dy;
        if (--steps < 0) break;
    }
found:
    px = (int16_t)(dx + x);
    py = (int16_t)(dy + y);
    if (px <= 2 || px >= 0x13E || px <= (int16_t)(0x55 + shift) || px >= (int16_t)(0xE9 + shift)) return;
    if (py <= 0x2D || py >= 0x90) return;
    plot_pixel_block(px, py);
}

void plot_symbol(int16_t x, int16_t y, int16_t large) {
    gaddr shape;
    if (!large) {
        if (x <= 0x58 || x >= 0xE6 || y <= 0x27 || y >= 0x8D) return;
        shape = SYMBOL_SMALL;
    } else {
        if (!(rd_u16(STREAM_SKIP) & 3) || x <= 0x60 || x >= 0xDE || y <= 0x2E || y >= 0x86) return;
        shape = SYMBOL_LARGE;
    }
    x = (int16_t)(x + rd_s16(SPAN_ORIGIN_Y));
    if (x < 10 || x > 0x136) return;
    y = (int16_t)(y + rd_s16(REDRAW_STATE_WORD));
    for (;; shape += 2) {
        int8_t dx = (int8_t)rd_u8(shape), dy = (int8_t)rd_u8(shape + 1);
        if (!dx && !dy) break;
        plot_pixel((int16_t)(x + dx), (int16_t)(y + dy));
    }
}
