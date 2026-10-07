/* Text and numeral plotting with the CPU (cockpit displays). */
#include "text.h"

#include "globals.h"

#define ROW_BYTES 40

void plot_glyph8(gaddr glyph, gaddr dest, int shift, int rows, int draw) {
    while (rows-- > 0) {
        uint32_t pixels = ((uint32_t)rd_u8(glyph) << 24) >> shift;
        uint32_t plane = rd_u32(dest);
        wr_u32(dest, draw ? (plane | pixels) : (plane & ~pixels));
        glyph += 1;
        dest += ROW_BYTES;
    }
}

void plot_glyph3(gaddr glyph, gaddr dest, int shift, int rows, GlyphMode mode) {
    const uint32_t cell = 0xE0000000u >> shift;
    while (rows-- > 0) {
        uint32_t plane = rd_u32(dest) & ~cell;
        if (mode != GLYPH_CLEAR) {
            uint32_t pixels = ((uint32_t)rd_u8(glyph) << 24) >> shift;
            plane |= (mode == GLYPH_DRAW ? pixels : ~pixels) & cell;
        }
        wr_u32(dest, plane);
        glyph += 1;
        dest += ROW_BYTES;
    }
}

void format_hex(gaddr p, uint32_t value, int8_t width) {
    int8_t i;

    p += (gaddr)(int32_t)width;
    for (i = 0; i < width; i++) {
        int8_t digit = (int8_t)((value & 15) + '0');
        if (digit > '9') digit = (int8_t)(digit + 7);
        wr_u8(p--, (uint8_t)digit);
        value >>= 4;
    }
    width--;
    p++;
    for (i = 0; i < width; i++) {
        if (rd_u8(p) != '0') break;
        wr_u8(p++, ' ');
    }
}

SmallText small_text_line(int count, gaddr chars, gaddr layout, gaddr rows, int16_t x_origin, int16_t plane,
                          uint16_t mode, int in_view) {
    SmallText text;
    text.count = count;
    text.layout = layout;
    text.chars = chars;
    text.plane_offset = plane;
    text.mode = mode;
    text.column = in_view ? rd_s16(SPAN_ORIGIN) : 0;
    text.x_origin = x_origin;
    text.rows = rows + (in_view ? rd_u32(REDRAW_STATE_LONG) : 0);
    return text;
}

TextDrawResult draw_small_text(const SmallText *text) {
    TextDrawResult result={0};
    gaddr plane = rd_u32(rd_u32(PAGE_PLANE_TABLE) + (gaddr)(int32_t)text->plane_offset);
    int16_t column = (int16_t)(text->column * 2);
    gaddr layout = text->layout, chars = text->chars;
    int i;

    for (i = 0; i < text->count; i++, layout += 4) {
        int16_t at = rd_s16(layout);
        uint16_t mode = rd_u16(layout + 2);
        uint8_t ch = rd_u8(chars++);
        result=(TextDrawResult){TEXT_DRAW_CHARACTER,ch,0}; /* C327AA */
        int16_t left = (int16_t)(text->x_origin + column);
        gaddr dest, glyph;
        /* BLT tests the true sign of the sum; the 40 limit the wrapped word. */
        if ((int32_t)left + at < 0 || (int16_t)(left + at) >= 40) continue;
        dest = (gaddr)(int32_t)(int16_t)(column + at) + text->rows + plane;
        glyph = SMALL_GLYPHS + (gaddr)(int32_t)rd_s16(SMALL_GLYPHS + (gaddr)(int32_t)(int16_t)((ch - 0x20) * 2));
        result=(TextDrawResult){TEXT_DRAW_GLYPH,ch,glyph}; /* C327D6 */
        if (dest & 1) {
            result.kind=TEXT_DRAW_FAULT;
            wr_u16(ERROR_CODE, 0x46);
            continue;
        }
        {
            uint16_t bits = (uint16_t)(text->mode | mode);
            uint16_t selector = (uint16_t)(bits & 0xF0);
            GlyphMode how = !selector ? GLYPH_CLEAR : (selector & 0xC0) ? GLYPH_DRAW : GLYPH_INVERSE;
            plot_glyph3(glyph, dest, (bits >> 12) & 15, 5, how);
        }
    }
    return result;
}

void format_digits(gaddr end, int count, int hex, int keep_zeros) {
    uint32_t bcd = rd_u32(DISPLAY_VALUE_BCD);
    gaddr p = end;
    int i;
    for (i = 0; i < count; i++) {
        int digit = (int)(bcd & 15) + '0';
        if (hex && digit > '9') digit += 7;
        wr_u8(--p, (uint8_t)digit);
        bcd >>= 4;
    }
    if (keep_zeros) return;
    for (i = 0; i < count - 1 && rd_u8(p) == '0'; i++) wr_u8(p++, ' ');
}

void format_small_hex(gaddr end, int count) { format_digits(end, count, 1, 0); }

Text text_line(int count, gaddr chars, gaddr layout, gaddr rows, int16_t x_origin) {
    Text text;
    text.count = count;
    text.layout = layout;
    text.chars = chars;
    text.column = 0;
    text.x_origin = x_origin;
    text.rows = rows;
    return text;
}

void draw_text(const Text *text) {
    gaddr planes = rd_u32(PAGE_PLANE_TABLE);
    uint8_t colour = rd_u8(CURRENT_COLOUR + 1);
    int16_t column = (int16_t)(text->column * 2);
    int i, k;

    for (i = 0; i < text->count; i++) {
        int16_t at = rd_s16(text->layout + (gaddr)(4 * i));
        uint16_t mode = rd_u16(text->layout + (gaddr)(4 * i) + 2);
        uint8_t ch = rd_u8(text->chars + (gaddr)i);
        int16_t left = (int16_t)(text->x_origin + column + at);
        gaddr offset, glyph;
        if (ch == ' ' || left < 0 || left >= 40) continue;
        offset = (gaddr)(int32_t)(int16_t)(at + column) + text->rows;
        if ((rd_u32(planes) + offset) & 1) continue;
        glyph = SMALL_GLYPHS + (uint32_t)(int32_t)rd_s16(SMALL_GLYPHS + (gaddr)(int32_t)(int16_t)((ch - 0x20) * 2));
        for (k = 0; k < 4; k++) {
            uint16_t bits = (uint16_t)(((colour >> (3 - k)) & 1 ? 0x0BFA : 0x0B0A) | mode);
            plot_glyph8(glyph, rd_u32(planes + (gaddr)(4 * k)) + offset, (bits >> 12) & 15, 5, (bits & 0xF0) != 0);
        }
    }
}

void draw_text_in_view(Text *text) {
    text->column = rd_s16(SPAN_ORIGIN);
    text->rows += rd_u32(REDRAW_STATE_LONG);
    draw_text(text);
}

void print_bcd_in_view(gaddr end, int digits, int keep_zeros, Text *text) {
    format_digits(end, digits, 0, keep_zeros);
    /* C32AEE-C32AFE tests the first digit even when the decremented width
     * is negative. A one-digit zero stock is blank, as well as leading zeros
     * in wider fields. The small-text formatter has a separate contract. */
    if (!keep_zeros && digits == 1 && rd_u8(end - 1) == '0') wr_u8(end - 1, ' ');
    draw_text_in_view(text);
}
