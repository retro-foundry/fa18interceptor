/* Turning a pair of angles toward a direction. */
#include "tracking.h"

#include "fixed_math.h"
#include "globals.h"
#include "memory.h"

#define TURN        0x7080
#define HALF_TURN   0x3840
#define EIGHTH_TURN 0x0E10
#define STEP_SHIFT  2 /* a quarter of the way each call */


static int16_t quotient(int32_t numerator, int16_t divisor) {
    wr_s32(DIVIDE_NUMERATOR, numerator);
    wr_s16(DIVIDE_DENOMINATOR, divisor);
    divide_rounded();
    return rd_s16(DIVIDE_QUOTIENT);
}

/* ARCTAN_TABLE entry `i`, in angle units (tenths of a degree times 8). */
static int32_t arctan_units(int16_t i) { return (int32_t)rd_s16(ARCTAN_TABLE + (gaddr)(2 * (int32_t)i)) * 8; }

/* The angle of x over z in the first quadrant: the table at the ratio
 * times 256 (the shifts make x << lift / (z >> shift) that times 64), taken
 * as z over x past 45 degrees. */
static int32_t azimuth_of(int32_t x, int32_t z, int shift, int lift) {
    int16_t xs = (int16_t)(x >> shift), zs = (int16_t)(z >> shift), q;
    if (xs <= zs) {
        wr_s32(DIVIDE_NUMERATOR, x << lift);
        wr_s16(DIVIDE_DENOMINATOR, zs);
        if (!zs) return EIGHTH_TURN;
        divide_rounded();
        q = (int16_t)(rd_s16(DIVIDE_QUOTIENT) >> 6);
        wr_s16(DIVIDE_QUOTIENT, q);
        return arctan_units(q);
    }
    q = (int16_t)(quotient(z << lift, xs) >> 6);
    wr_s16(DIVIDE_QUOTIENT, q);
    return 900 * 8 - arctan_units(q);
}

static int32_t elevation_of(int32_t x, int32_t y, int32_t z, int shift) {
    int16_t zs = (int16_t)(z >> shift), xs = (int16_t)(x >> shift), ys, level;
    wr_s32(SQRT_INPUT, (int32_t)zs * zs + (int32_t)xs * xs);
    square_root();
    level = rd_s16(SQRT_RESULT);
    ys = (int16_t)(y >> shift);
    if (ys <= level) {
        wr_s32(DIVIDE_NUMERATOR, (int32_t)((uint32_t)(int32_t)ys << 8));
        wr_s16(DIVIDE_DENOMINATOR, level);
        if (!level) return EIGHTH_TURN;
        divide_rounded();
        return arctan_units(rd_s16(DIVIDE_QUOTIENT));
    }
    return 900 * 8 - arctan_units(quotient((int32_t)level << 8, ys));
}

/* A quarter of the way from `current` to `target`, the short way round,
 * at most `max_step`. */
static int32_t step_angle(int32_t current, int32_t target, int32_t max_step) {
    int32_t d = target - current, step;
    if (d < 0) {
        if (d < -HALF_TURN) {
            step = (d + TURN) >> STEP_SHIFT;
            if (step > max_step) step = max_step;
            step += current;
            return step >= TURN ? step - TURN : step;
        }
        step = -d >> STEP_SHIFT;
        if (step > max_step) step = max_step;
        return current - step;
    }
    if (d > HALF_TURN) {
        step = (TURN - d) >> STEP_SHIFT;
        if (step > max_step) step = max_step;
        step = current - step;
        return step < 0 ? step + TURN : step;
    }
    step = d >> STEP_SHIFT;
    if (step > max_step) step = max_step;
    return current + step;
}

void track_direction(int32_t *elevation, int32_t *azimuth, int32_t x, int32_t y, int32_t z, int32_t max_step) {
    int x_neg = x < 0, y_neg = y < 0, z_neg = z < 0;
    int32_t largest, pitch, heading;
    int shift, lift;

    if (x_neg) x = -x;
    if (y_neg) y = -y;
    if (z_neg) z = -z;
    largest = x > y ? (x > z ? x : z) : (y > z ? y : z);
    /* Scale so the tested components fit a word; shift + lift is 14. */
    if (largest > 0x10000000) { x >>= 3; y >>= 3; z >>= 3; shift = 14; lift = 0; }
    else if (largest > 0x2000000) { shift = 14; lift = 0; }
    else if (largest > 0x800000) { shift = 12; lift = 2; }
    else if (largest > 0x200000) { shift = 10; lift = 4; }
    else { shift = 8; lift = 6; }

    heading = azimuth_of(x, z, shift, lift);
    if (x_neg && z_neg) heading = HALF_TURN - heading;
    else if (z_neg) heading += HALF_TURN;
    else if (!x_neg) heading = TURN - heading;

    pitch = elevation_of(x, y, z, shift);
    if (!y_neg) pitch = TURN - pitch;
    if (pitch >= TURN) pitch = 0;
    if (heading >= TURN) heading = 0;

    if (max_step < 0 || !(rd_u8(CONTEXT_STATE) | rd_u8(TRACK_STARTED))) {
        wr_u8(TRACK_STARTED, 1);
        *elevation = pitch;
        *azimuth = heading;
    } else {
        *elevation = step_angle(*elevation, pitch, max_step);
        *azimuth = step_angle(*azimuth, heading, max_step);
    }
    wr_s16(TRACKED_PITCH, (int16_t)*elevation);
    wr_s16(TRACKED_HEADING, (int16_t)*azimuth);
}
