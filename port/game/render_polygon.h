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

#endif
