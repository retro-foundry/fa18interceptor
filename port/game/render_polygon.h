#ifndef FA18_GAME_RENDER_POLYGON_H
#define FA18_GAME_RENDER_POLYGON_H

#include <stdint.h>

/* How one plane of a polygon's colour is combined with the page. */
typedef enum {
    PLANE_CLEAR,      /* colour bit 0: clear the plane under the mask */
    PLANE_SET,        /* colour bit 1: set the plane under the mask */
    PLANE_COMPLEMENT  /* complement the plane under the mask */
} PlaneOp;

/* Composite the filled polygon mask into one bitplane of the draw page.
 * `plane_index` selects the plane (0-3). Consumes one bit of POLY_PLANE_BITS. */
void composite_polygon_plane(int plane_index, PlaneOp op);

/* Draw one polygon edge from (x0,y0) to (x1,y1) into the mask buffer as a
 * one-dot line, clipped below `last_row`. */
void draw_polygon_edge(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t last_row);

/* Clear the polygon mask buffer over the polygon's bounding box. */
void clear_polygon_mask(void);

/* Prepare the polygon at POLY_VERTICES ($C301F6). Its bounding box decides:
 * above the last row, nothing; one or two rows and columns, a pixel, pair
 * or 2x2 block; thin in one direction, a line from corner to corner (in all
 * planes and the current colour unless KEEP_LINE_STYLE); otherwise its
 * edges into the mask plane and an area fill over the box, with the bounds
 * and the compositing blit's POLY_* parameters stored. Returns 0 when a
 * fill was started (compositing follows), 1 otherwise. */
int prepare_polygon(void);
/* The same with `last` for the last row ($C301F0 gives $C7). */
int prepare_polygon_to_row(int16_t last);

/* Draw the polygon at POLY_VERTICES ($C2FF48): with blitter priority,
 * prepare it; when it needs compositing, either blit the mask between the
 * first planes (LINE_COLOUR given and POLY_MASK_BLIT set) or composite it
 * into each plane in LINE_PLANES: set or cleared by LINE_COLOUR's bit (or
 * CURRENT_COLOUR's), complemented by POLY_COMPLEMENT's (with a given
 * colour; otherwise its bit 0 applies to every plane once plane 0 is
 * drawn). Then clear the mask. Priority stays on after a direct draw. */
void draw_polygon(void);

/* The mark polygon at MARK_POLYGON scaled into POLY_VERTICES (x * 24/256
 * + 193, y * 31/256 + 162, moved by the view origin): 0 when it is empty
 * ($C3019C's first part). */
int scale_mark_polygon(void);

/* Draw it: fill to row $C7 and composite into plane 1 ($C3019C). */
void draw_mark_polygon(void);

#endif
