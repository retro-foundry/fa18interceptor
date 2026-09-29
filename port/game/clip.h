#ifndef FA18_GAME_CLIP_H
#define FA18_GAME_CLIP_H

/* Clipping against the view's side planes x = +-z and y = +-z. */

#include "memory.h"

enum { CLIP_X = 0, CLIP_Y = 1 };

/* Where the segment from the point at `p` (three words) towards (qx, qy, qz)
 * meets the plane axis = side * z (side +1 or -1): stored in CLIP_POINT.
 * Returns 0 when that point is inside the view (z >= 0, |x| <= z,
 * |y| <= z), 1 otherwise or when the segment is parallel to the plane.
 *
 * `rounded` selects the two forms the game uses: rounded (the far point's x
 * and y first moved one unit in, quotients rounded away from zero past
 * half; $C2EA5A, $C2EAD0, $C2EB4C, $C2EBC2) or truncated ($C2F0C6, $C2F0F4,
 * $C2F156, $C2F128). */
int clip_to_view_plane(gaddr p, int16_t qx, int16_t qy, int16_t qz, int axis, int side, int rounded);

/* The same on a point in hand, the crossing to `out` instead of
 * CLIP_POINT: -1 (and no crossing) when the rounded form finds the segment
 * parallel to the plane. */
int view_plane_crossing(const int16_t far[3], int16_t qx, int16_t qy, int16_t qz, int axis, int side, int rounded,
                        int16_t out[3]);

#endif
