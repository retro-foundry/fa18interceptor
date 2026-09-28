#ifndef FA18_GAME_RENDER_LINE_H
#define FA18_GAME_RENDER_LINE_H

#include <stdint.h>

/* Blitter line setup shared by lines and polygon edges. Lines are drawn from
 * the row below their upper end to their lower end, and clipped against a
 * last row along their major axis. */
typedef struct {
    int16_t row;         /* first row drawn */
    int16_t x;           /* x of the upper end */
    int16_t rows;        /* rows after the first: |dy| - 1 (0 for horizontal lines) */
    int16_t dx;          /* x extent, made positive */
    int x_major;         /* 1 when x steps every dot */
    int clipped;         /* the x-major length was shortened by the last row */
    int16_t length;      /* dots after the first */
    int16_t error;       /* initial Bresenham term (BLTAPTL) */
    int16_t step_minor;  /* 4 * minor (BLTBMOD) */
    int16_t step_both;   /* 4 * minor - 4 * major (BLTAMOD) */
    uint16_t con1;       /* octant, SIGN, LINE (| ONEDOT when requested) */
    uint16_t shift;      /* pixel within the first word, BLTCON0 bits 12-15 */
    uint16_t size;       /* BLTSIZE: length + 1 rows, 2 words */
    int32_t offset;      /* byte offset of the first pixel's word in a plane */
} LineSetup;

/* Returns 0 when the line starts below `last_row` (nothing to draw). A
 * horizontal line is kept when `keep_horizontal`, dropped otherwise. */
int setup_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t last_row,
               int keep_horizontal, int one_dot, LineSetup *line);

/* Draw a line from (x0,y0) to (x1,y1) into every enabled plane of the draw
 * page, in the current line colour. */
void draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1);

#endif
