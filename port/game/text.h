#ifndef FA18_GAME_TEXT_H
#define FA18_GAME_TEXT_H

#include <stdint.h>

#include "memory.h"

/* Glyphs are one byte per row. They are plotted into one bitplane through a
 * longword window at `dest` (40-byte rows), `shift` pixels in from the
 * window's left edge. */

/* How a glyph row combines with the plane. */
typedef enum {
    GLYPH_CLEAR,  /* clear the glyph's cell */
    GLYPH_DRAW,   /* glyph pixels set, rest of the cell cleared */
    GLYPH_INVERSE /* glyph pixels cleared, rest of the cell set */
} GlyphMode;

/* 8-pixel glyph: set (`draw`) or clear the glyph's pixels, leaving the rest
 * of the plane as it is. */
void plot_glyph8(gaddr glyph, gaddr dest, int shift, int rows, int draw);

/* 3-pixel glyph cell (small numerals): the whole cell is written. */
void plot_glyph3(gaddr glyph, gaddr dest, int shift, int rows, GlyphMode mode);

/* Write `value` as `width` hex digits ending at p + width (so the digits
 * occupy p+1 .. p+width), with leading zeros shown as spaces (the last
 * digit is always shown). */
void format_hex(gaddr p, uint32_t value, int8_t width);

#endif
