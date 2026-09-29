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

/* One end of an edge from `p` toward `q`, clipped to the view pyramid:
 * itself when in view (CLIP_END_POINT), else where the edge enters the
 * view through the planes p is beyond (CLIP_END_CROSSING, the crossing in
 * CLIP_POINT and its plane in *plane), trying the opposite plane when a
 * crossing is behind the eye and then the other axis; CLIP_END_NONE when it
 * does not enter. `probe` computes a crossing into CLIP_POINT and says
 * whether it is in view. */
enum { CLIP_END_NONE, CLIP_END_POINT, CLIP_END_CROSSING };
#define CLIP_PLANE(axis, side) ((axis) * 2 + ((side) < 0))
typedef int (*ClipProbe)(void *context, int axis, int side);
int clip_edge_end(const int16_t p[3], const int16_t q[3], ClipProbe probe, void *context, int *plane);

/* The eight CORNER_RECORDS as a ring: for each corner, its edge to the
 * corner two on (odd corners: from the next one back to the previous), the
 * start moved (1, 1) in: where it enters the view, counted per plane in
 * CROSSING_COUNTS / CROSSING_LAST and put on screen (rounded, not
 * mirrored) in CORNER_SCREEN; an edge that never enters clears its entry
 * and the corner's +$E word; one wholly in view leaves them, and from an
 * odd corner lets the next corner go ($C2E758). */
void project_corner_edges(void);

#endif
