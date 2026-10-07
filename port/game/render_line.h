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
/* The same with a fixed last row in place of LINE_LAST_ROW ($C2FA78 draws
 * to row $C7). */
void draw_line_to_row(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t last_row);

/* C2FA7E publishes its actual blit size even when no plane is enabled.
 * A rejected lower-end-first line has already computed its signed X delta;
 * the other rejected starts preserve the caller's preceding output. */
enum LineDrawKind { LINE_DRAW_NONE, LINE_DRAW_X_DELTA, LINE_DRAW_SIZE };
typedef struct { enum LineDrawKind kind; int16_t x_delta; uint16_t size; } LineDrawResult;
LineDrawResult draw_line_to_row_result(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t last_row);

/* The two view-space points at SEGMENT_POINTS projected and joined by a
 * line; 0, drawing nothing, when either is behind or outside the view
 * pyramid ($C2ED70). */
int draw_projected_segment(void);

/* The segment at SEGMENT_POINTS clipped to the view pyramid, projected into
 * POLY_VERTICES and drawn; each end is itself when in view, else where the
 * segment enters the view through the planes it is beyond. 0 when it does
 * not ($ERROR_CODE $16 for an end at z <= 0). The two points are left
 * exchanged ($C2EE4A). */
int draw_clipped_segment(void);

/* C2EE4A's final endpoint-Y load, reflected screen Y, or line output.
 * The clipping probes save/restore the incoming endpoint values. */
enum SegmentDrawKind { SEGMENT_DRAW_ENDPOINT_Y, SEGMENT_DRAW_SCREEN_Y, SEGMENT_DRAW_LINE };
typedef struct { int drawn; enum SegmentDrawKind kind; int16_t y; LineDrawResult line; } SegmentDrawResult;
SegmentDrawResult draw_clipped_segment_result(void);

/* Draw lines into all four planes in the current object colour. */
void reset_line_style(void);

#endif
