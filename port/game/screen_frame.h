#ifndef FA18_GAME_SCREEN_FRAME_H
#define FA18_GAME_SCREEN_FRAME_H

#include <stdint.h>

#include "memory.h"

/* Coordinate lists for the 320x180 view: points are appended at `*cursor`
 * as (x, y) words. */
enum { VIEW_RIGHT = 319, VIEW_BOTTOM = 179 };

/* Append two FRAME_POINTS entries, mirrored through the view's far corner. */
void append_mirrored_points(gaddr *cursor, int16_t first, int16_t second);

/* Append one corner: (0,0), (319,0), (319,179) or (0,179). */
void append_point(gaddr *cursor, int16_t x, int16_t y);

#endif
