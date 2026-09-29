/* Register replay for the HUD stores icon stream ($C30AE2). The C routine
 * draws first; pixel and line register effects are rebuilt in stream order. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "glue_text.h"
#include "hud_stores.h"
#include "memory.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void line_registers_to_row(int16_t last_row); /* glue_render_polygon.c */

static void stores_stream_registers(void) {
    uint16_t final_colour = rd_u16(CURRENT_COLOUR);

    for (;;) {
        int8_t before = (int8_t)D(4);
        uint16_t colour;
        int16_t x, y;

        SET_B(D(4), (uint8_t)(D(4) - 1));
        if (before <= 0) colour = 0;
        else if ((int32_t)D(4) > 0 || (int8_t)D(4) > 0) colour = 4;
        else colour = 1;
        wr_u16(CURRENT_COLOUR, colour);

        SET_W(D(0), rd_u16(A(1)));
        A(1) += 2;
        if (W(0) < 0) break;
        SET_W(D(1), rd_u16(A(1)));
        A(1) += 2;
        SET_W(D(0), (uint16_t)(W(0) + rd_s16(SPAN_ORIGIN_Y)));
        if (W(0) <= 0 || W(0) >= 0x13E) continue;
        SET_W(D(2), (uint16_t)D(0));
        SET_W(D(1), (uint16_t)(W(1) + rd_s16(REDRAW_STATE_WORD)));
        SET_W(D(3), (uint16_t)(W(1) + 6));
        {
            uint32_t d4 = D(4), d5 = D(5), a0 = A(0), a1 = A(1);
            x = W(2);
            y = W(3);
            line_registers_to_row(0xC7);
            D(0) = SEXT(x);
            D(1) = SEXT(y);
            SET_W(D(0), (uint16_t)(x - 1));
            plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
            D(0) = SEXT((int16_t)(x - 1));
            D(1) = SEXT(y);
            SET_W(D(0), (uint16_t)(x + 1));
            plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
            D(4) = d4;
            D(5) = d5;
            A(0) = a0;
            A(1) = a1;
        }
    }
    wr_u16(CURRENT_COLOUR, final_colour);
}

int glue_C30AE2(void) {
    uint32_t count = D(4);
    draw_stores_icon_stream(A(1), &count);
    stores_stream_registers();
    return glue_return();
}

int glue_C30A00(void) {
    gaddr record = CONTROL_RECORDS + (gaddr)(int32_t)rd_s16(VIEW_RECORD);
    int active = rd_s8(STORES_REDRAWS) > 0;
    int16_t centre = (int16_t)(0x44 + rd_s16(SPAN_ORIGIN_Y));
    int16_t y = (int16_t)(0xA3 + rd_s16(REDRAW_STATE_WORD));
    int in_view = centre >= 0 && centre <= 0x13F;
    uint8_t type = rd_u8(record + 0x62) & 0xF0;
    uint8_t status = in_view ? rd_u8(record + 0x63) & 0xF0 : (uint8_t)D(5);
    uint32_t count_state = D(4);
    uint16_t final_colour;

    if (active && type && in_view && (int16_t)(y + 1) > 0)
        count_state = (count_state & 0xFFFF0000u) | rd_u16(PIXEL_MASKS + (gaddr)(2 * (centre & 15)));
    draw_stores_icons(count_state, status);
    final_colour = rd_u16(CURRENT_COLOUR);

    if (!active) return glue_return();
    A(0) = record;
    SET_B(D(1), type);
    if (!type) return glue_return();
    SET_W(D(0), (uint16_t)centre);
    if (in_view) {
        uint32_t d0, d1, d5, a0;
        SET_W(D(1), (uint16_t)y);
        SET_B(D(5), status);
        wr_u16(CURRENT_COLOUR, status == 0x10 ? 1 : 0);
        d0 = D(0); d1 = D(1); d5 = D(5); a0 = A(0);
        plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
        D(0) = d0; D(1) = d1; D(5) = d5; A(0) = a0;
        if (status != 0x10) wr_u16(CURRENT_COLOUR, 4);
        SET_W(D(1), (uint16_t)(W(1) + 1));
        d5 = D(5); a0 = A(0);
        plot_registers(PIXEL_MASKS, PLOT_ROWS_1);
        D(5) = d5; A(0) = a0;
    }
    A(1) = 0xC309E2u;
    SET_B(D(4), rd_u8(record + 0x5F) >> 4);
    if (status == 0x20) D(4) |= 0x80000000u;
    else D(4) &= 0x7FFFFFFFu;
    stores_stream_registers();
    A(1) = 0xC309ECu;
    SET_B(D(4), rd_u8(record + 0x5F) & 15);
    if (status == 0x30) D(4) |= 0x80000000u;
    else D(4) &= 0x7FFFFFFFu;
    stores_stream_registers();
    wr_u16(CURRENT_COLOUR, final_colour);
    return glue_return();
}
