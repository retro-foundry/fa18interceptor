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

/* `v` rotated by VIEW_ANGLE_MATRIX (8 fraction bits); the depth's low
 * word also to VIEW_DEPTH ($C2CE82). */
void rotate_by_view_matrix(const int16_t v[3], int32_t out[3]);

/* Ground points (x, z word pairs at `src`, y = 0): each shifted down by
 * `shift`, moved by offset[0] + BOUND_OFFSET_X and offset[2] +
 * BOUND_OFFSET_Z, rotated by VIEW_ANGLE_MATRIX (8 fraction bits) and moved
 * by offset[3..5], to three words at `out`; at least one point ($C098C6). */
void transform_ground_points(gaddr src, int16_t count, int16_t shift, const int16_t offset[6], gaddr out);

#endif
