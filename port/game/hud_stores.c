/* HUD stores marks, from $C30AE2. */
#include "hud_stores.h"

#include "globals.h"
#include "plot.h"
#include "render_line.h"

gaddr draw_stores_icon_stream(gaddr points, uint32_t *remaining) {
    uint32_t count = *remaining;

    for (;;) {
        int8_t before = (int8_t)count;
        int16_t x, y;
        uint16_t colour;

        /* SUBQ.B and BGE use the signed result including overflow. */
        count = (count & 0xFFFFFF00u) | (uint8_t)(count - 1);
        if (before <= 0) colour = 0;
        else if ((int32_t)count > 0 || (int8_t)count > 0) colour = 4;
        else colour = 1;
        wr_u16(CURRENT_COLOUR, colour);

        x = rd_s16(points);
        points += 2;
        if (x < 0) break;
        y = rd_s16(points);
        points += 2;
        x = (int16_t)(x + rd_s16(SPAN_ORIGIN_Y));
        if (x <= 0 || x >= 0x13E) continue;
        y = (int16_t)(y + rd_s16(REDRAW_STATE_WORD));
        draw_line_to_row(x, y, x, (int16_t)(y + 6), 0xC7);
        plot_pixel((int16_t)(x - 1), (int16_t)(y + 6));
        plot_pixel((int16_t)(x + 1), (int16_t)(y + 6));
    }
    *remaining = count;
    return points;
}

void draw_stores_icons(uint32_t count_state, uint8_t status) {
    gaddr record;
    int16_t x;
    uint32_t count;

    if (rd_s8(STORES_REDRAWS) <= 0) return;
    wr_u8(STORES_REDRAWS, (uint8_t)(rd_u8(STORES_REDRAWS) - 1));
    record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    if (!(rd_u8(record + 0x62) & 0xF0)) return;

    wr_u32(LINE_STYLE, 0x000FFFFFu);
    x = (int16_t)(0x44 + rd_s16(SPAN_ORIGIN_Y));
    if (x >= 0 && x <= 0x13F) {
        int16_t y = (int16_t)(0xA3 + rd_s16(REDRAW_STATE_WORD));
        status = (uint8_t)(rd_u8(record + 0x63) & 0xF0);
        wr_u16(CURRENT_COLOUR, status == 0x10 ? 1 : 0);
        plot_pixel(x, y);
        if (status != 0x10) wr_u16(CURRENT_COLOUR, 4);
        plot_pixel(x, (int16_t)(y + 1));
    }

    count = (count_state & 0xFFFFFF00u) | (rd_u8(record + 0x5F) >> 4);
    if (status == 0x20) count |= 0x80000000u;
    else count &= 0x7FFFFFFFu;
    draw_stores_icon_stream(0xC309E2u, &count);

    count = (count & 0xFFFFFF00u) | (rd_u8(record + 0x5F) & 15);
    if (status == 0x30) count |= 0x80000000u;
    else count &= 0x7FFFFFFFu;
    draw_stores_icon_stream(0xC309ECu, &count);
}
