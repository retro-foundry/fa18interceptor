/* Text and numeral plotting with the CPU (cockpit displays). */
#include "text.h"

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
