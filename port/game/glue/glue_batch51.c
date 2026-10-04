/* Remaining edge register replay used by glue_batch55.c. The complete
 * C2EE4A owner is in glue_segment_projection.c. */
#include "glue.h"
#include "ports_glue.h"

#include "clip.h"
#include "globals.h"
#include "memory.h"
#include "render_line.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))
#define W(n) ((int16_t)D(n))

void line_registers(void); /* glue_render_polygon.c */

enum { NONE, POINT, CROSSING };

typedef struct {
    int16_t p[3], q[3], cross[3];
    int rounded;
} Segment;

/* One plane test: 1 when the crossing is in view (the BEQ taken). A rounded
 * test that finds the edge parallel leaves the last crossing. */
static int cross(Segment *s, int axis, int side) {
    int16_t at[3];
    int outside = view_plane_crossing(s->q, s->p[0], s->p[1], s->p[2], axis, side, s->rounded, at);
    int k;
    if (outside < 0) return 0;
    for (k = 0; k < 3; k++) s->cross[k] = at[k];
    return !outside;
}

static int behind(const Segment *s) { return s->cross[2] < 0; }

static void w(int n, int16_t v) { SET_W(D(n), (uint16_t)v); }

/* $C2EECA and $C2EFBE: the other axis after one failed (a = 1 for y). */
static int other_regs(Segment *s, int a, int axis) {
    int16_t pa = s->p[a], pz = s->p[2], qa = s->q[a], qz = s->q[2];
    if (pa < 0) {
        w(6, (int16_t)-pa);
        if ((int16_t)-pa < pz) return NONE;
        w(2, qa); w(6, qz); w(2, (int16_t)-qa);
        if (qz <= (int16_t)-qa) return NONE;
        if (cross(s, axis, -1)) return CROSSING;
        if (!behind(s) || pa < pz) return NONE;
        w(2, qa);
        if (qz <= qa) return NONE;
        return cross(s, axis, 1) ? CROSSING : NONE;
    }
    if (pa < pz) return NONE;
    w(2, qa); w(6, qz);
    if (qz <= qa) return NONE;
    if (cross(s, axis, 1)) return CROSSING;
    if (!behind(s)) return NONE;
    w(6, (int16_t)-pa);
    if ((int16_t)-pa < pz) return NONE;
    w(6, qz); w(2, (int16_t)-qa);
    if (qz <= (int16_t)-qa) return NONE;
    return cross(s, axis, -1) ? CROSSING : NONE;
}

/* Beyond the plane on axis `a` (0 x, 1 y) at `side`: that plane, then the
 * opposite one, then the other axis. Entered with D6 as the side test left
 * it. */
static int beyond_regs(Segment *s, int a, int side) {
    int axis = a ? CLIP_Y : CLIP_X;
    int16_t qa = s->q[a], qz = s->q[2];
    w(2, qa); w(6, qz);
    if (side < 0) w(2, (int16_t)-qa);
    if (qz <= (int16_t)(side * qa)) return NONE;
    if (cross(s, axis, side)) return CROSSING;
    if (behind(s)) {
        w(2, (int16_t)(-side * qa));
        if (qz > (int16_t)(-side * qa) && cross(s, axis, -side)) return CROSSING;
    }
    return other_regs(s, 1 - a, a ? CLIP_X : CLIP_Y);
}

static int end_regs(Segment *s) {
    int16_t px = s->p[0], py = s->p[1], pz = s->p[2];
    if (px >= pz) return beyond_regs(s, 0, 1);
    w(6, (int16_t)-px);
    if ((int16_t)-px >= pz) return beyond_regs(s, 0, -1);
    if (py >= pz) return beyond_regs(s, 1, 1);
    w(6, (int16_t)-py);
    if ((int16_t)-py >= pz) return beyond_regs(s, 1, -1);
    return pz >= 0 ? POINT : NONE;
}

/* The decision chain's D2/D6 for an edge from `p` (registers D3-D5 as they
 * hold it) to `q`; `last` is the last crossing computed (read by the
 * behind-the-eye tests) and is updated. Returns 0 none, 1 the point, 2 a
 * crossing (then in `last`). */
int edge_end_registers(const int16_t p[3], const int16_t q[3], int rounded, int16_t last[3]);
int edge_end_registers(const int16_t p[3], const int16_t q[3], int rounded, int16_t last[3]) {
    Segment s;
    int k, end;
    for (k = 0; k < 3; k++) {
        s.p[k] = p[k];
        s.q[k] = q[k];
        s.cross[k] = last[k];
    }
    s.rounded = rounded;
    end = end_regs(&s);
    for (k = 0; k < 3; k++) last[k] = s.cross[k];
    return end;
}
