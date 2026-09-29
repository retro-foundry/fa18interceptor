#ifndef FA18_GAME_VIEW_MARKS_H
#define FA18_GAME_VIEW_MARKS_H

/* Small marks plotted over the view, placed from SPAN_ORIGIN_Y
 * across and the cockpit slide rows (REDRAW_STATE_WORD) down. */

#include "memory.h"

/* A two-pixel stem, three rows from (160, 129), on a four-pixel foot, in
 * colour 8 ($C332FE); nothing within two pixels of the edges. */
void draw_view_marker(void);

/* A 22-row bar at x = 112 rising from row 180, the bottom level - 1 rows in
 * colour 4 and the rest in 0, for level = GAUGE_SOURCE bits 10-14
 * ($C30918). It is drawn when GAUGE_REFRESH is positive or the level has
 * changed from GAUGE_SHOWN; the level is kept only when refreshing or
 * while DISPLAY_FORCE bit 0 is set. */
void draw_gauge_bar(void);

/* A ring through the quarter outline at `outline` (dx, dy byte pairs
 * between $FF $FF sentinels), mirrored into four quarters around (x, y),
 * at most points + 1 pixels ($C345A0). Inside the view window it plots
 * every point and lifts the lower quarters one row; otherwise it plots only
 * points inside x 15-305, SPAN_ORIGIN_Y + $56-$E8 and y $2E-$8F. Not yet
 * proven: the recordings never call it (its glue is ready, unregistered). */
void plot_ring(int16_t x, int16_t y, int16_t points, gaddr outline);

/* A pixel symbol (dx, dy byte pairs to a 0, 0 pair) at (x, y) in the
 * view window: the small one, or the large one while STREAM_SKIP bits 0-1
 * are nonzero, each only well inside the window ($C348B2). */
void plot_symbol(int16_t x, int16_t y, int16_t large);

#endif
