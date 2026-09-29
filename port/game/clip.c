#include "clip.h"

#include "globals.h"

/* 68000 DIVS.W: on overflow the dividend's words stand in for the quotient
 * and remainder. */
static void divs_w(int32_t dividend, int16_t divisor, int16_t *quotient, int16_t *remainder) {
    int32_t q = dividend / divisor;
    if (q != (int16_t)q) {
        *quotient = (int16_t)dividend;
        *remainder = (int16_t)((uint32_t)dividend >> 16);
        return;
    }
    *quotient = (int16_t)q;
    *remainder = (int16_t)(dividend % divisor);
}

/* A quotient, rounded away from zero (by the quotient's sign) when the
 * remainder is at least half the divisor. */
static int16_t rounded_quotient(int32_t dividend, int16_t divisor) {
    int16_t q, r, half = (int16_t)((divisor < 0 ? (int16_t)-divisor : divisor) >> 1);
    divs_w(dividend, divisor, &q, &r);
    if (r < 0) r = (int16_t)-r;
    if (half > r) return q;
    return (int16_t)(q < 0 ? q - 1 : q + 1);
}

int view_plane_crossing(const int16_t far[3], int16_t qx, int16_t qy, int16_t qz, int axis, int side, int rounded,
                        int16_t out[3]) {
    int16_t px = far[0], py = far[1], pz = far[2];
    int16_t dx, denominator, x, y, z, qy_in, t;

    /* The y planes are the x planes with the two axes exchanged. */
    if (axis == CLIP_Y) {
        t = px; px = py; py = t;
        t = qx; qx = qy; qy = t;
    }

    if (rounded) {
        qx = (int16_t)(qx - 1);
        qy = (int16_t)(qy - 1);
    }
    if (side < 0) {
        px = (int16_t)-px;
        qx = (int16_t)-qx;
    }
    dx = (int16_t)(pz - px);
    denominator = (int16_t)(qx - qz + dx);
    if (denominator == 0) {
        if (!rounded) for (;;) {} /* the original spins here (BEQ to itself) */
        return -1;
    }
    qy_in = (int16_t)(py - qy);
    if (rounded) {
        y = (int16_t)(py - rounded_quotient((int32_t)qy_in * dx, denominator));
        z = (int16_t)(pz - rounded_quotient((int32_t)(int16_t)(pz - qz) * dx, denominator));
    } else {
        int16_t q, r;
        divs_w((int32_t)qy_in * dx, denominator, &q, &r);
        y = (int16_t)(py - q);
        divs_w((int32_t)(int16_t)(pz - qz) * dx, denominator, &q, &r);
        z = (int16_t)(pz - q);
    }
    x = side < 0 ? (int16_t)-z : z;
    if (axis == CLIP_Y) {
        t = x; x = y; y = t;
    }

    out[0] = x;
    out[1] = y;
    out[2] = z;
    if (z < 0) return 1;
    if (x > z || (int16_t)-x > z) return 1;
    if (y > z || (int16_t)-y > z) return 1;
    return 0;
}

int clip_to_view_plane(gaddr p, int16_t qx, int16_t qy, int16_t qz, int axis, int side, int rounded) {
    int16_t far[3], out[3];
    int k, outside;
    for (k = 0; k < 3; k++) far[k] = rd_s16(p + (gaddr)(2 * k));
    outside = view_plane_crossing(far, qx, qy, qz, axis, side, rounded, out);
    if (outside < 0) return 1;
    for (k = 0; k < 3; k++) wr_s16(CLIP_POINT + (gaddr)(2 * k), out[k]);
    return outside;
}

/* ---- one end of an edge, clipped to the view pyramid ---------------------- */

static int crossing_behind(void) { return rd_s16(CLIP_POINT + 4) < 0; }

/* After the planes on one axis failed: the other axis's planes, tried only
 * when `p` is beyond one of them (a, qa: that axis of p and of q). */
static int other_axis(const int16_t p[3], int16_t a, int16_t qa, int16_t qz, int axis, ClipProbe probe, void *context,
                      int *plane) {
    int first = a < 0 ? -1 : 1;
    int16_t pa = (int16_t)(first * a), qfirst = (int16_t)(first * qa), qsecond = (int16_t)(-first * qa);
    if (pa < p[2] || qz <= qfirst) return CLIP_END_NONE;
    *plane = CLIP_PLANE(axis, first);
    if (probe(context, axis, first)) return CLIP_END_CROSSING;
    if (!crossing_behind()) return CLIP_END_NONE;
    if ((int16_t)-pa < p[2] || qz <= qsecond) return CLIP_END_NONE;
    *plane = CLIP_PLANE(axis, -first);
    return probe(context, axis, -first) ? CLIP_END_CROSSING : CLIP_END_NONE;
}

/* Beyond the plane axis = side * z: its crossing, else the opposite plane's
 * if the first crossing is behind the eye, else the other axis. */
static int beyond(const int16_t p[3], const int16_t q[3], int axis, int side, ClipProbe probe, void *context, int *plane) {
    int16_t qa = axis == CLIP_X ? q[0] : q[1], pb = axis == CLIP_X ? p[1] : p[0], qb = axis == CLIP_X ? q[1] : q[0];
    if (q[2] <= (int16_t)(side * qa)) return CLIP_END_NONE;
    *plane = CLIP_PLANE(axis, side);
    if (probe(context, axis, side)) return CLIP_END_CROSSING;
    if (crossing_behind() && q[2] > (int16_t)(-side * qa)) {
        *plane = CLIP_PLANE(axis, -side);
        if (probe(context, axis, -side)) return CLIP_END_CROSSING;
    }
    return other_axis(p, pb, qb, q[2], axis == CLIP_X ? CLIP_Y : CLIP_X, probe, context, plane);
}

int clip_edge_end(const int16_t p[3], const int16_t q[3], ClipProbe probe, void *context, int *plane) {
    if (p[0] >= p[2]) return beyond(p, q, CLIP_X, 1, probe, context, plane);
    if ((int16_t)-p[0] >= p[2]) return beyond(p, q, CLIP_X, -1, probe, context, plane);
    if (p[1] >= p[2]) return beyond(p, q, CLIP_Y, 1, probe, context, plane);
    if ((int16_t)-p[1] >= p[2]) return beyond(p, q, CLIP_Y, -1, probe, context, plane);
    return p[2] >= 0 ? CLIP_END_POINT : CLIP_END_NONE;
}

/* ---- the corners' edges ($C2E758) ----------------------------------------- */

typedef struct { gaddr q; int16_t p[3]; } CornerProbe;

static int enters_rounded(void *context, int axis, int side) {
    const CornerProbe *c = context;
    return !clip_to_view_plane(c->q, c->p[0], c->p[1], c->p[2], axis, side, 1);
}

/* The rounded screen coordinate of v / z at `half_span` (x 160, y 90). */
static int16_t rounded_screen(int16_t v, int16_t z, int16_t half_span) {
    int16_t q, r, s;
    divs_w((int32_t)v * (2 * half_span), z, &q, &r);
    s = (int16_t)((int16_t)(q >> 1) + (q & 1) + half_span);
    if (s < 0) return 0;
    if (s >= 2 * half_span) return (int16_t)(2 * half_span - 1);
    return s;
}

void project_corner_edges(void) {
    /* Which counter each plane's crossings go to (CLIP_PLANE order). */
    static const int counter[4] = {1, 3, 0, 2};
    int i, skip = 0;

    for (i = 0; i <= 7; i++) {
        int from, to, plane = 0, end;
        gaddr out = CORNER_SCREEN + (gaddr)(8 * i);
        CornerProbe probe;
        int16_t q[3];
        if (i & 1) {
            from = i + 1 > 7 ? 0 : i + 1;
            to = i - 1;
        } else {
            from = i;
            to = i + 2 > 7 ? 0 : i + 2;
        }
        if (skip) {
            skip = 0;
            continue;
        }
        probe.q = CORNER_RECORDS + (gaddr)(16 * to);
        probe.p[0] = (int16_t)(rd_s16(CORNER_RECORDS + (gaddr)(16 * from)) + 1);
        probe.p[1] = (int16_t)(rd_s16(CORNER_RECORDS + (gaddr)(16 * from) + 2) + 1);
        probe.p[2] = rd_s16(CORNER_RECORDS + (gaddr)(16 * from) + 4);
        q[0] = rd_s16(probe.q);
        q[1] = rd_s16(probe.q + 2);
        q[2] = rd_s16(probe.q + 4);
        end = clip_edge_end(probe.p, q, enters_rounded, &probe, &plane);
        if (end == CLIP_END_POINT && probe.p[2] >= 0) {
            if (i & 1) skip = 1; /* in view: the next corner needs nothing */
            continue;
        }
        if (end != CLIP_END_CROSSING) {
            wr_u32(out, 0);
            wr_u16(CORNER_RECORDS + (gaddr)(16 * i) + 0xE, 0);
            continue;
        }
        {
            gaddr n = CROSSING_COUNTS + (gaddr)(2 * counter[plane]);
            int16_t x = rd_s16(CLIP_POINT), y = rd_s16(CLIP_POINT + 2), z = rd_s16(CLIP_POINT + 4);
            wr_u16(n, (uint16_t)(rd_u16(n) + 1));
            wr_u16(CROSSING_LAST + (gaddr)(2 * counter[plane]), (uint16_t)i);
            if (z <= 0) for (;;) {} /* the original spins here */
            wr_s16(out, rounded_screen(x, z, 0xA0));
            wr_s16(out + 2, rounded_screen(y, z, 0x5A));
        }
    }
}
