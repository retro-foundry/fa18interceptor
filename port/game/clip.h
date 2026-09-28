#ifndef FA18_GAME_CLIP_H
#define FA18_GAME_CLIP_H

/* Clipping against the view's side planes x = z and x = -z. */

#include "memory.h"

enum { CLIP_RIGHT = 1, CLIP_LEFT = -1 };

/* Where the segment from the point at `p` (three words) towards (qx, qy, qz)
 * meets the plane x = side * z: stored in CLIP_POINT. Returns 0 when that
 * point is inside the view (z >= 0, |x| <= z, |y| <= z), 1 otherwise or
 * when the segment is parallel to the plane.
 *
 * `rounded` selects the two forms the game uses: rounded ($C2EA5A right,
 * $C2EAD0 left: the far point's x and y are first moved one unit in, and
 * quotients round away from zero past half) or truncated ($C2F0C6,
 * $C2F0F4). */
int clip_to_side_plane(gaddr p, int16_t qx, int16_t qy, int16_t qz, int side, int rounded);

#endif
