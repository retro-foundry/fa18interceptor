/* Glue for track_direction $C123FA, compiled C with six long stack
 * arguments: elevation, azimuth, x, y, z, max_step. It updates its argument
 * slots in place (the angles, and x, y, z made positive and, when large,
 * divided by 8), and its callers read D1 and A0: the last value the path
 * put in D1 and the last ARCTAN_TABLE entry read. The azimuth target the
 * step reads D1 from is replayed here; the rest is read after the C. */
#include "glue.h"
#include "ports_glue.h"

#include "globals.h"
#include "memory.h"
#include "tracking.h"

#define SEXT(v) ((uint32_t)(int32_t)(int16_t)(v))

static int16_t rounded(int32_t numerator, int16_t divisor) {
    int32_t q = numerator / divisor;
    int16_t quotient, remainder, half = (int16_t)((divisor < 0 ? -divisor : divisor) >> 1);
    if (q != (int16_t)q) { quotient = (int16_t)numerator; remainder = (int16_t)((uint32_t)numerator >> 16); }
    else { quotient = (int16_t)q; remainder = (int16_t)(numerator % divisor); }
    if (remainder < 0) remainder = (int16_t)-remainder;
    if (half > remainder) return quotient;
    return (int16_t)(quotient < 0 ? quotient - 1 : quotient + 1);
}

static int32_t table(int16_t i) { return (int32_t)rd_s16(ARCTAN_TABLE + (gaddr)(2 * (int32_t)i)) * 8; }

/* The registers $C123FA leaves (D1 and A0), on their own, for the glue of
 * routines that end in it. `*x`, `*y` and `*z` come back as the call made
 * them positive and, when large, divided by 8; `before` is the azimuth as
 * it was and `snap` the test taken before the C, since it can set
 * TRACK_STARTED. Nothing here writes game memory. */
void track_direction_registers(int32_t elevation, int32_t azimuth, int32_t before, int32_t *x_io,
                               int32_t *y_io, int32_t *z_io, int32_t max_step, int snap) {
    int32_t x = *x_io, y = *y_io, z = *z_io, largest, heading;
    int x_neg = x < 0, z_neg = z < 0, shift, lift, heading_divided = 1, pitch_divided = 1;
    int16_t xs, zs, ys, level;
    uint32_t pitch_d1;

    (void)elevation;
    if (x < 0) x = -x;
    if (y < 0) y = -y;
    if (z < 0) z = -z;
    largest = x > y ? (x > z ? x : z) : (y > z ? y : z);
    if (largest > 0x10000000) { x >>= 3; y >>= 3; z >>= 3; shift = 14; lift = 0; }
    else if (largest > 0x2000000) { shift = 14; lift = 0; }
    else if (largest > 0x800000) { shift = 12; lift = 2; }
    else if (largest > 0x200000) { shift = 10; lift = 4; }
    else { shift = 8; lift = 6; }
    *x_io = x;
    *y_io = y;
    *z_io = z;

    /* The azimuth target, replayed (its quotient is gone from memory). */
    xs = (int16_t)(x >> shift);
    zs = (int16_t)(z >> shift);
    if (xs <= zs) {
        if (zs) heading = table((int16_t)(rounded(x << lift, zs) >> 6));
        else { heading = 0x0E10; heading_divided = 0; }
    } else {
        heading = 900 * 8 - table((int16_t)(rounded(z << lift, xs) >> 6));
    }
    if (x_neg && z_neg) heading = 0x3840 - heading;
    else if (z_neg) heading += 0x3840;
    else if (!x_neg) heading = 0x7080 - heading;
    if (heading >= 0x7080) heading = 0;

    /* The elevation's D1, from the square root and quotient it left. */
    level = rd_s16(SQRT_RESULT);
    ys = (int16_t)(y >> shift);
    if (ys <= level) {
        pitch_d1 = (uint32_t)SEXT(ys) << 8;
        if (!level) pitch_divided = 0;
    } else {
        pitch_d1 = (uint32_t)(900 * 8 - table(rd_s16(DIVIDE_QUOTIENT)));
    }
    if (heading_divided || pitch_divided) A(0) = ARCTAN_TABLE + (SEXT(rd_u16(DIVIDE_QUOTIENT)) << 1);

    if (max_step < 0) {
        D(1) = pitch_d1;
    } else if (snap) {
        D(1) = pitch_d1 & 0xFFFFFF00u; /* MOVE.B of the clear TRACK_STARTED */
    } else {
        int32_t d = heading - before;
        if (d < 0) D(1) = (uint32_t)max_step;
        else if (d > 0x3840) D(1) = (uint32_t)azimuth;
        else D(1) = (uint32_t)(d >> 2);
    }
}

int glue_C123FA(void) {
    gaddr args = A(7) + 4;
    int32_t elevation = rd_s32(args), azimuth = rd_s32(args + 4), x = rd_s32(args + 8), y = rd_s32(args + 12),
            z = rd_s32(args + 16), max_step = rd_s32(args + 20), before = azimuth;
    int snap = max_step < 0 || !(rd_u8(CONTEXT_STATE) | rd_u8(TRACK_STARTED));

    track_direction(&elevation, &azimuth, x, y, z, max_step);
    track_direction_registers(elevation, azimuth, before, &x, &y, &z, max_step, snap);
    /* The compiled C updates its argument slots in place. */
    wr_s32(args + 4, azimuth);
    wr_s32(args, elevation);
    wr_s32(args + 8, x);
    wr_s32(args + 12, y);
    wr_s32(args + 16, z);
    return glue_return();
}
