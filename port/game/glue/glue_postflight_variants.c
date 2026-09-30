/* Register replay for the two postflight variant drawing heads. Their shared
 * $C31392 tail still needs a bridge before either parent can be registered. */
#include "glue.h"

#include "globals.h"
#include "glue_text.h"
#include "memory.h"

#define POSTFLIGHT_TUPLES 0xC3128Au

static int horizontal_visible(void) {
    return (int16_t)D(0) >= 0 && (int16_t)D(0) < 0x140;
}

void postflight_tuple_head_registers(void);
void postflight_tuple_head_registers(void) {
    int pair, i;
    for (pair = 0; pair < 2; ++pair) {
        gaddr tuple = POSTFLIGHT_TUPLES + (gaddr)(8 * pair);
        for (i = 0; i < 4; ++i)
            D(i) = (uint32_t)(int32_t)rd_s16(tuple + (gaddr)(2 * i));
        A(0) = tuple + 8;
        SET_W(D(0), (uint16_t)(D(0) + rd_u16(SPAN_ORIGIN_Y)));
        if (!horizontal_visible()) continue;
        SET_W(D(2), (uint16_t)(D(2) + rd_u16(SPAN_ORIGIN_Y)));
        if ((int16_t)D(2) < 0 || (int16_t)D(2) >= 0x140) continue;
        SET_W(D(1), (uint16_t)(D(1) + rd_u16(REDRAW_STATE_WORD)));
        SET_W(D(3), (uint16_t)(D(3) + rd_u16(REDRAW_STATE_WORD)));
        if (pair == 0) {
            gaddr saved_a0 = A(0);
            line_registers_to_row(0xC7);
            A(0) = saved_a0;
        } else {
            line_registers_to_row(0xC7);
        }
    }
}

void postflight_fixed_head_registers(void);
void postflight_fixed_head_registers(void) {
    SET_W(D(0), 0x9D);
    SET_W(D(0), (uint16_t)(D(0) + rd_u16(SPAN_ORIGIN_Y)));
    if (!horizontal_visible()) return;
    SET_W(D(1), 0xA8);
    SET_W(D(1), (uint16_t)(D(1) + rd_u16(REDRAW_STATE_WORD)));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);

    SET_W(D(0), 0x9F);
    SET_W(D(0), (uint16_t)(D(0) + rd_u16(SPAN_ORIGIN_Y)));
    if (!horizontal_visible()) return;
    SET_W(D(1), 0xA8);
    SET_W(D(1), (uint16_t)(D(1) + rd_u16(REDRAW_STATE_WORD)));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);

    SET_W(D(0), 0x9E);
    SET_W(D(0), (uint16_t)(D(0) + rd_u16(SPAN_ORIGIN_Y)));
    SET_W(D(1), 0xA8);
    SET_W(D(1), (uint16_t)(D(1) + rd_u16(REDRAW_STATE_WORD)));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);
    SET_W(D(0), 0x9E);
    SET_W(D(0), (uint16_t)(D(0) + rd_u16(SPAN_ORIGIN_Y)));
    SET_W(D(1), 0xA7);
    SET_W(D(1), (uint16_t)(D(1) + rd_u16(REDRAW_STATE_WORD)));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);
}
