#ifndef FA18_GAME_POLYGON_CLIP_H
#define FA18_GAME_POLYGON_CLIP_H

/* Polygon clipping against the four view planes y = z, -y = z, x = z and
 * -x = z ($C2469E-$C24DA6), Sutherland-Hodgman as a pipeline: each stage
 * takes one vertex at a time, passes crossings and inside vertices to the
 * next, and the last stage appends to the output list. Each stage keeps its
 * first and previous vertex (CLIP_STATES) and a started flag and passed
 * count (CLIP_FLAGS); vertices travel through CLIP_SCRATCH. */

#include "memory.h"

typedef struct {
    int16_t x, y, z;
} ClipPoint;

/* The output list: where the next vertex goes, and how many there are. */
typedef struct {
    gaddr next;
    uint16_t count;
} ClipOutput;

enum { CLIP_Y_POS, CLIP_Y_NEG, CLIP_X_POS, CLIP_X_NEG }; /* stage numbers */

/* Feed vertex `p` into stage `stage` ($C247C0, $C248B2 and $C24996 are
 * stages 1-3; stage 0 is inline in the outer routine). */
void clip_stage(int stage, ClipPoint p, ClipOutput *out);

/* Where the edge from `prev` to `cur` crosses the stage's plane, with the
 * original's rounding (quotients past half a divisor round away from zero,
 * by the quotient's sign). */
ClipPoint clip_crossing(int stage, ClipPoint prev, ClipPoint cur);

#endif
