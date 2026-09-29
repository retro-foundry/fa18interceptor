/* Filled circle's symmetric span table and four-plane blit ($C2F1C0). */
#include "circle.h"

#include "globals.h"
#include "hardware.h"
#include "memory.h"
#include "plot.h"

static void circle_spans(gaddr table, int16_t radius) {
    int16_t x = radius, y = 0;
    int16_t error = (int16_t)(3 - 2 * radius);
    gaddr near = table + 2;
    gaddr far = near + 4 * (gaddr)radius;

    wr_u16(table, (uint16_t)radius);
    while (y < x) {
        wr_u16(near, (uint16_t)-y);
        wr_u16(near + 2, (uint16_t)y);
        wr_u16(far, (uint16_t)-x);
        wr_u16(far + 2, (uint16_t)x);
        if (error < 0) {
            error = (int16_t)(error + 4 * y + 6);
        } else {
            error = (int16_t)(error + 4 * (y - x) + 10);
            x--;
            near += 4;
        }
        far -= 4;
        y++;
    }
    if (y == x) {
        wr_u16(near, (uint16_t)-y);
        wr_u16(near + 2, (uint16_t)y);
    }
}

void draw_filled_circle(int16_t x, int16_t y, int16_t radius) {
    gaddr table, span, row_base;
    int16_t top, rising, falling, limit;
    uint16_t colour;

    if (radius <= 0) {
        plot_pixel_pair(x, y);
        return;
    }
    table = rd_u32(CIRCLE_SPANS_PTR);
    if (radius == 1) {
        wr_u16(table, 1);
        wr_u32(table + 2, 0xFFFF0001u);
    } else {
        if (radius > 127) radius = 127;
        circle_spans(table, radius);
    }
    span = table + 2;
    top = (int16_t)(y - (radius - 1));
    rising = (int16_t)(radius - 1);
    falling = radius;
    if (top < 1) {
        top--;
        rising = (int16_t)(rising + top);
        if (rising < 0) {
            falling = (int16_t)(falling + rising);
            if (falling <= 0) return;
        }
        span += (gaddr)(uint16_t)((uint16_t)(-top) * 4u);
        top = 1;
    }
    limit = rd_s16(LINE_LAST_ROW);
    if ((int16_t)(top + rising + falling) >= limit) {
        int16_t cut = (int16_t)(top + rising + falling - limit);
        if (top > limit) return;
        falling = (int16_t)(falling - cut);
        if (falling < 0) rising = (int16_t)(rising + falling);
    }

    row_base = rd_u32(rd_u32(PAGE_PLANE_TABLE) + 12) + (gaddr)(int32_t)(int16_t)(top * 40);
    colour = rd_u16(CURRENT_COLOUR);
    wait_blitter();
    custom_write(BLTCON1, 0);
    custom_write(BLTADAT, 0xFFFF);
    for (;;) {
        int16_t left = (int16_t)(rd_s16(span) + x);
        int16_t right = (int16_t)(rd_s16(span + 2) + x);
        int16_t count, remainder, extra;
        uint16_t first, last, size;
        gaddr dest;
        int plane;

        if (left < 0) left = 0;
        if (right >= 320) right = 319;
        count = (int16_t)(right - left);
        remainder = (int16_t)(left & 15);
        dest = row_base + (gaddr)(int16_t)((left & (int16_t)0xFFF0) >> 3);
        first = (uint16_t)(16 - remainder);
        if (first > count) first = (uint16_t)(count + 1);
        extra = (int16_t)(count - first);
        first = rd_u16(0xC2F342u + 2u * first);
        first = (uint16_t)((first >> remainder) | (first << (16 - remainder)));
        size = 0x41;
        if (extra >= 15) {
            int16_t words = (int16_t)((extra + 1) & (int16_t)0xFFF0);
            extra = (int16_t)(extra - words);
            size = (uint16_t)(size + (words >> 4));
        }
        if (extra >= 0) {
            size++;
            last = rd_u16(0xC2F38Au + 2u * (uint16_t)extra);
        } else {
            last = 0xFFFF;
        }
        custom_write(BLTAFWM, first);
        custom_write(BLTALWM, last);
        for (plane = 0; plane < 4; plane++) {
            if (plane) wait_blitter();
            custom_write(BLTCON0, (colour & (1u << plane)) ? 0x3FA : 0x30A);
            custom_write_ptr(BLTCPT, dest + (gaddr)(plane * 0x1F40));
            custom_write_ptr(BLTDPT, dest + (gaddr)(plane * 0x1F40));
            custom_write(BLTSIZE, size);
        }
        row_base += 40;
        if (rising >= 0) {
            span += 4;
            rising--;
            if (rising >= 0) continue;
        }
        span -= 4;
        falling--;
        if (falling < 0) break;
    }
}
