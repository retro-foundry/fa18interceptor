#ifndef FA18_GAME_VIEW_TRANSFORM_H
#define FA18_GAME_VIEW_TRANSFORM_H

/* Rotate a point into view space ($C1F2EE): the three words at `in`,
 * offset by (0, -$28, -$A3), shifted down by `shift`, multiplied by
 * CAMERA_MATRIX (8 fraction bits); three words stored at `out`. */

#include "memory.h"

void view_transform(gaddr in, int16_t shift, gaddr out);

/* A record's position relative to the grid origin ($C1EBE0): its +$06,
 * +$08, +$0A words shifted up by `shift` (0-15), x and z less the grid
 * cell (+$0E high and low bytes, minus GRID_ORIGIN_X/Z) as 16.16 / 4. */
void grid_relative_position(gaddr record, int shift, int32_t out[3]);

/* Append a point to the list at LIST_WRITE ($C25876): rows 0 and 2 of
 * LIST_MATRIX applied to (x, y, z) (8 fraction bits) and shifted up by
 * `shift`, then `tag`; the next three longs are cleared. */
void append_list_point(int16_t x, int16_t y, int16_t z, int shift, uint16_t tag);

#endif
