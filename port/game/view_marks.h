#ifndef FA18_GAME_VIEW_MARKS_H
#define FA18_GAME_VIEW_MARKS_H

/* Small marks plotted with pixel pairs, placed from SPAN_ORIGIN_Y
 * across and the cockpit slide rows (REDRAW_STATE_WORD) down. */

/* A two-pixel stem, three rows from (160, 129), on a four-pixel foot, in
 * colour 8 ($C332FE); nothing within two pixels of the edges. */
void draw_view_marker(void);

/* A 22-row bar at x = 112 rising from row 180, the bottom level - 1 rows in
 * colour 4 and the rest in 0, for level = GAUGE_SOURCE bits 10-14
 * ($C30918). It is drawn when GAUGE_REFRESH is positive or the level has
 * changed from GAUGE_SHOWN; the level is kept only when refreshing or
 * while DISPLAY_FORCE bit 0 is set. */
void draw_gauge_bar(void);

#endif
