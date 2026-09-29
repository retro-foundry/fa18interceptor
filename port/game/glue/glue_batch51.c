/* Glue for draw_clipped_segment $C2EE4A. Its callers read every data
 * register: D2 and D6 as the plane tests last loaded them, D3-D5 the
 * projected end (DIVS remainders in the upper words), D0-D2 the first point
 * from the exchange, and draw_line's registers. The decision chain is
 * replayed on the two points as they were before the C exchanged them,
 * with the crossings computed in hand rather than through CLIP_POINT. */
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
} Segment;

/* One plane test: 1 when the crossing is in view (the BEQ taken). */
static int cross(Segment *s, int axis, int side) {
    return !view_plane_crossing(s->q, s->p[0], s->p[1], s->p[2], axis, side, 0, s->cross);
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

static void divs_reg(int n, int16_t divisor) {
    int32_t dividend = (int32_t)D(n), q = dividend / divisor;
    if (q != (int16_t)q) return;
    D(n) = ((uint32_t)(uint16_t)(dividend % divisor) << 16) | (uint16_t)q;
}

int glue_C2EE4A(void) {
    Segment s;
    int pass, k, drawn;

    for (k = 0; k < 3; k++) {
        s.p[k] = rd_s16(SEGMENT_POINTS + (gaddr)(2 * k));
        s.q[k] = rd_s16(SEGMENT_POINTS + 6 + (gaddr)(2 * k));
    }
    drawn = draw_clipped_segment();
    (void)drawn;
    for (pass = 0; pass < 2; pass++) {
        int end;
        for (k = 0; k < 3; k++) D(3 + k) = SEXT(s.p[k]);
        end = end_regs(&s);
        if (end == NONE) break;
        if (end == CROSSING)
            for (k = 0; k < 3; k++) D(3 + k) = SEXT(s.cross[k]);
        if (W(5) <= 0) break;
        D(3) = (uint32_t)((int32_t)W(3) * 0xA0);
        divs_reg(3, W(5));
        w(3, (int16_t)(W(3) + 0xA0));
        if (W(3) < 0) w(3, 0); else if (W(3) >= 0x140) w(3, 0x13F);
        D(4) = (uint32_t)((int32_t)W(4) * 0x5A);
        divs_reg(4, W(5));
        w(4, (int16_t)(W(4) + 0x5A));
        if (W(4) < 0) w(4, 0); else if (W(4) >= 0xB4) w(4, 0xB3);
        w(3, (int16_t)(0x13F - W(3)));
        w(4, (int16_t)(0xB3 - W(4)));
        if (pass) {
            for (k = 0; k < 4; k++) D(k) = SEXT(rd_u16(POLY_VERTICES + (gaddr)(2 * k)));
            line_registers();
            D(0) = 1;
            flags_logic_l(1);
            return glue_return();
        }
        for (k = 0; k < 3; k++) {
            int16_t t = s.p[k];
            D(k) = SEXT(t);
            s.p[k] = s.q[k];
            s.q[k] = t;
        }
    }
    D(0) = 0;
    flags_logic_l(0);
    return glue_return();
}
