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

/* A line of small (3x5) characters for one plane of the draw page. */
typedef struct {
    int count;            /* characters */
    gaddr layout;         /* per character: word byte column, word mode */
    gaddr chars;          /* character codes from ' ' */
    int16_t plane_offset; /* offset into the draw page's plane table */
    uint16_t mode;        /* ORed into each character's mode (glyph shift/draw bits) */
    int16_t column;       /* added to each byte column */
    int16_t x_origin;     /* the line's left byte, for clipping to 0..39 */
    gaddr rows;           /* row offset within the plane */
} SmallText;

/* Draw a SmallText line (characters outside the 40-byte row are skipped). */
void draw_small_text(const SmallText *text);

/* Write `count` hex digits of DISPLAY_VALUE_BCD backwards ending before
 * `end`, leading zeros (not the last digit) as spaces. */
void format_small_hex(gaddr end, int count);

#endif
