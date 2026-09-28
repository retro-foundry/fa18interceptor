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

void draw_small_text(const SmallText *text) {
    gaddr plane = rd_u32(rd_u32(PAGE_PLANE_TABLE) + (gaddr)(int32_t)text->plane_offset);
    int16_t column = (int16_t)(text->column * 2);
    gaddr layout = text->layout, chars = text->chars;
    int i;

    for (i = 0; i < text->count; i++, layout += 4) {
        int16_t at = rd_s16(layout);
        uint16_t mode = rd_u16(layout + 2);
        uint8_t ch = rd_u8(chars++);
        int16_t left = (int16_t)(text->x_origin + column);
        gaddr dest, glyph;
        /* BLT tests the true sign of the sum; the 40 limit the wrapped word. */
        if ((int32_t)left + at < 0 || (int16_t)(left + at) >= 40) continue;
        dest = (gaddr)(int32_t)(int16_t)(column + at) + text->rows + plane;
        glyph = SMALL_GLYPHS + (gaddr)(int32_t)rd_s16(SMALL_GLYPHS + (gaddr)(int32_t)(int16_t)((ch - 0x20) * 2));
        if (dest & 1) {
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
}

void format_small_hex(gaddr end, int count) {
    uint32_t bcd = rd_u32(DISPLAY_VALUE_BCD);
    gaddr p = end;
    int i;
    for (i = 0; i < count; i++) {
        int digit = (int)(bcd & 15) + '0';
        if (digit > '9') digit += 7;
        wr_u8(--p, (uint8_t)digit);
        bcd >>= 4;
    }
    for (i = 0; i < count - 1 && rd_u8(p) == '0'; i++) wr_u8(p++, ' ');
}
