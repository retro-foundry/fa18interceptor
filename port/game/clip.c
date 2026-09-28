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

int clip_to_side_plane(gaddr p, int16_t qx, int16_t qy, int16_t qz, int side, int rounded) {
    int16_t px = rd_s16(p), py = rd_s16(p + 2), pz = rd_s16(p + 4);
    int16_t dx, denominator, x, y, z, qy_in;

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
        return 1;
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

    wr_s16(CLIP_POINT, x);
    wr_s16(CLIP_POINT + 2, y);
    wr_s16(CLIP_POINT + 4, z);
    if (z < 0) return 1;
    if (x > z || (int16_t)-x > z) return 1;
    if (y > z || (int16_t)-y > z) return 1;
    return 0;
}
