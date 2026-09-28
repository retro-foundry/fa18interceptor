/* Glue for the small-text entries $C32794, $C3271A, $C32736, $C32662. They
 * share one loop whose register leftovers the callers partly read. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "text.h"

/* Draw and rebuild the loop's registers. On entry the registers hold the
 * loop's inputs: D0.w count - 1, A1 layout, A2 chars, D5.w plane offset,
 * D6 = mode (high word) and column (low word), A5.w x origin, A4 rows
 * (already including any row offset). */
static int small_text_loop(void) {
    SmallText text, head, tail;
    uint32_t d2 = D(2), d3 = D(3), d4 = D(4), d5 = D(5), a0, a3 = A(3), before_last = 0;
    gaddr plane;
    int16_t column;
    int i, last_plotted = -1;

    text.count = (int)(uint16_t)D(0) + 1;
    text.layout = A(1);
    text.chars = A(2);
    text.plane_offset = (int16_t)D(5);
    text.mode = (uint16_t)(D(6) >> 16);
    text.column = (int16_t)D(6);
    text.x_origin = (int16_t)A(5);
    text.rows = A(4);
    plane = rd_u32(rd_u32(PAGE_PLANE_TABLE) + (gaddr)(int32_t)text.plane_offset);
    column = (int16_t)(text.column * 2);
    a0 = rd_u32(PAGE_PLANE_TABLE);

    /* Which character is drawn last (inside the row, at an even address). */
    for (i = 0; i < text.count; i++) {
        int16_t at = rd_s16(text.layout + (gaddr)(4 * i)), left = (int16_t)(text.x_origin + column);
        gaddr dest = (gaddr)(int32_t)(int16_t)(column + at) + text.rows + plane;
        if ((int32_t)left + at < 0 || (int16_t)(left + at) >= 40 || (dest & 1)) continue;
        last_plotted = i;
    }
    /* Draw up to that character, note its last row, then draw the rest. */
    head = text;
    head.count = last_plotted < 0 ? text.count : last_plotted;
    draw_small_text(&head);
    if (last_plotted >= 0) {
        int16_t at = rd_s16(text.layout + (gaddr)(4 * last_plotted));
        gaddr dest = (gaddr)(int32_t)(int16_t)(column + at) + text.rows + plane;
        before_last = rd_u32(dest + 4 * 40);
        tail = text;
        tail.count = text.count - last_plotted;
        tail.layout = text.layout + (gaddr)(4 * last_plotted);
        tail.chars = text.chars + (gaddr)last_plotted;
        draw_small_text(&tail);
    }

    for (i = 0; i < text.count; i++) {
        gaddr layout = text.layout + (gaddr)(4 * i);
        int16_t at = rd_s16(layout), left = (int16_t)(text.x_origin + column);
        uint16_t mode = rd_u16(layout + 2);
        uint8_t ch = rd_u8(text.chars + (gaddr)i);
        uint16_t bits;
        gaddr dest, glyph;
        d5 = (d5 & 0xFFFF0000u) | (uint16_t)at;
        d3 = (d3 & 0xFFFF0000u) | mode;
        d4 = (d4 & 0xFFFFFF00u) | ch;
        d2 = (d2 & 0xFFFF0000u) | (uint16_t)(left + at);
        if ((int32_t)left + at < 0 || (int16_t)(left + at) >= 40) continue;
        dest = (gaddr)(int32_t)(int16_t)(column + at) + text.rows + plane;
        glyph = SMALL_GLYPHS + (uint32_t)(int32_t)rd_s16(SMALL_GLYPHS + (gaddr)(int32_t)(int16_t)((ch - 0x20) * 2));
        bits = (uint16_t)(text.mode | mode);
        d5 = dest;
        d4 = glyph;
        a3 = glyph;
        d2 = (d2 & 0xFFFF0000u) | bits;
        if (dest & 1) continue;
        /* plot_glyph3 ($C32806) leftovers. */
        a0 = glyph + 5;
        a3 = dest + 5 * 40;
        d2 = (uint32_t)((bits >> 12) & 15) << 16 | (d3 & 0xFFFF);
        if (i == last_plotted) d3 = (before_last & 0xFFFF0000u) | (d3 & 0xFFFF);
    }
    D(0) = (D(0) & 0xFFFF0000u) | 0xFFFF;
    D(1) = plane;
    D(2) = d2;
    D(3) = d3;
    D(4) = d4;
    D(5) = d5;
    SET_W(D(6), column);
    SET_W(D(7), 0x142);
    A(0) = a0;
    A(1) = text.layout + (gaddr)(4 * text.count);
    A(2) = text.chars + (gaddr)text.count;
    A(3) = a3;
    return glue_return();
}

/* $C32794: the loop itself; A4 += D7 first. */
int glue_C32794(void) {
    A(4) += D(7);
    return small_text_loop();
}

/* $C32662: draw only when no context runs or TEXT_ALWAYS is set. */
int glue_C32662(void) {
    if (rd_u8(CONTEXT_SELECT) && !rd_u8(TEXT_ALWAYS)) return glue_return();
    A(4) += D(7);
    return small_text_loop();
}

/* Hex digits before the loop ($C32750): leaves D4 = 0, D1.w = the last digit
 * and A0 = where the zero scan stopped (A0 is reloaded by the loop). */
static void small_hex(void) {
    int count = (int)(uint16_t)D(0) + 1;
    uint32_t bcd = rd_u32(DISPLAY_VALUE_BCD);
    format_small_hex(A(0), count);
    D(3) = count >= 8 ? 0 : bcd >> (4 * count); /* LSR.L #4 per digit */
    D(4) = 0;
}

/* $C3271A: mode = the caller's D6.w, column 0, no row offset. */
int glue_C3271A(void) {
    D(6) = D(6) << 16;
    D(7) = 0;
    small_hex();
    return small_text_loop();
}

/* $C32736: mode $F3A, column SPAN_ORIGIN, rows offset by REDRAW_STATE_LONG. */
int glue_C32736(void) {
    D(6) = 0x0F3Au << 16 | rd_u16(SPAN_ORIGIN);
    D(7) = rd_u32(REDRAW_STATE_LONG);
    small_hex();
    A(4) += D(7);
    return small_text_loop();
}
