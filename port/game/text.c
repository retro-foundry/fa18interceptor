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
