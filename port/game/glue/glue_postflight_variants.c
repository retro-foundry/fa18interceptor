/* Register replay for the two postflight variant drawing heads. Their shared
 * $C31392 tail still needs a bridge before either parent can be registered. */
#include "glue.h"
#include "glue_postflight_variants.h"

#include "globals.h"
#include "glue_text.h"
#include "memory.h"

#define POSTFLIGHT_TUPLES 0xC3128Au
#define POSTFLIGHT_PREFIX_GATE 0xC45838u
#define POSTFLIGHT_PREFIX_TICK 0xC45883u

static int horizontal_visible(void) {
    return (int16_t)D(0) >= 0 && (int16_t)D(0) < 0x140;
}

static void add_word_register(int n, uint16_t value) {
    uint32_t sum = (uint16_t)D(n) + (uint32_t)value;
    SET_W(D(n), sum);
    FLAG_X = sum > 0xFFFFu ? XFLAG_SET : XFLAG_CLEAR;
}

static void line_final_add_extend(void) {
    int bit, last_bit = -1;
    uint32_t table = rd_u32(PAGE_PLANE_TABLE);
    uint64_t sum;
    if (A(0) != 0xDFF000u || A(2) != table) return;
    for (bit = 0; bit < 4; ++bit)
        if (rd_u8(LINE_PLANES) & (1u << bit)) last_bit = bit;
    if (last_bit < 0) return;
    sum = (uint64_t)rd_u32(table + (gaddr)(4 * (3 - last_bit))) + A(1);
    FLAG_X = sum > 0xFFFFFFFFu ? XFLAG_SET : XFLAG_CLEAR;
}

void postflight_tuple_head_registers(void) {
    int pair, i;
    for (pair = 0; pair < 2; ++pair) {
        gaddr tuple = POSTFLIGHT_TUPLES + (gaddr)(8 * pair);
        for (i = 0; i < 4; ++i)
            D(i) = (uint32_t)(int32_t)rd_s16(tuple + (gaddr)(2 * i));
        A(0) = tuple + 8;
        add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
        if (!horizontal_visible()) continue;
        add_word_register(2, rd_u16(SPAN_ORIGIN_Y));
        if ((int16_t)D(2) < 0 || (int16_t)D(2) >= 0x140) continue;
        add_word_register(1, rd_u16(REDRAW_STATE_WORD));
        add_word_register(3, rd_u16(REDRAW_STATE_WORD));
        if (pair == 0) {
            gaddr saved_a0 = A(0);
            line_registers_to_row(0xC7);
            line_final_add_extend();
            A(0) = saved_a0;
        } else {
            line_registers_to_row(0xC7);
            line_final_add_extend();
        }
    }
}

void postflight_fixed_head_registers(void) {
    SET_W(D(0), 0x9D);
    add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
    if (!horizontal_visible()) return;
    SET_W(D(1), 0xA8);
    add_word_register(1, rd_u16(REDRAW_STATE_WORD));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);

    SET_W(D(0), 0x9F);
    add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
    if (!horizontal_visible()) return;
    SET_W(D(1), 0xA8);
    add_word_register(1, rd_u16(REDRAW_STATE_WORD));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);

    SET_W(D(0), 0x9E);
    add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
    SET_W(D(1), 0xA8);
    add_word_register(1, rd_u16(REDRAW_STATE_WORD));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);
    SET_W(D(0), 0x9E);
    add_word_register(0, rd_u16(SPAN_ORIGIN_Y));
    SET_W(D(1), 0xA7);
    add_word_register(1, rd_u16(REDRAW_STATE_WORD));
    plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0xC);
}

/* $C31392-$C3141D. Called while the C tail still holds the original point
 * table, before record submissions replace its entries. */
void postflight_tail_prefix_registers(const PostflightVariantWork *work) {
    uint16_t count;
    if (!rd_u8(POSTFLIGHT_PREFIX_GATE)) {
        flags_logic_b(0);
        return;
    }
    A(2) = work->table;
    D(2) = 10;
    for (;;) {
        uint16_t x = rd_u16(A(2));
        gaddr cursor;
        A(2) += 2;
        SET_W(D(0), x);
        if ((int16_t)x < 0 && x == 0xFFFFu) break;
        if ((int16_t)x < 0) D(0) &= ~0x8000u;
        SET_W(D(1), rd_u16(A(2)));
        A(2) += 2;
        count = (uint16_t)D(2);
        cursor = A(2);
        if ((int16_t)x < 0) {
            pair_registers_colour(0);
        } else {
            plot_registers_colour(PIXEL_MASKS, PLOT_ROWS_1, 0);
        }
        A(2) = cursor;
        SET_W(D(2), count);
        SET_W(D(2), (uint16_t)(D(2) - 1u));
        if ((uint16_t)D(2) == 0xFFFFu) break;
    }
    /* ADDQ.B #1 of the cadence byte is the tail's last X-writing operation
     * before the vector handoff. The C path has already incremented it. */
    FLAG_X = rd_u8(POSTFLIGHT_PREFIX_TICK) == 0 ? XFLAG_SET : XFLAG_CLEAR;
    D(7) = 0;
    D(6) = 1;
    A(2) = work->table;
    A(0) = work->vector_stream;
    D(0) = (uint32_t)work->vector[0];
    D(1) = (uint32_t)work->vector[1];
    D(2) = (uint32_t)work->vector[2];
    D(3) = D(0) | D(1) | D(2);
    flags_logic_l(D(3));
}
