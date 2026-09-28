#ifndef FA18_GAME_FIXED_MATH_H
#define FA18_GAME_FIXED_MATH_H

#include <stdint.h>

#include "memory.h"

/* Angles are tenths of a degree, 0-3599. Sines are 2.14 fixed point
 * ($4000 = 1.0). */
typedef int16_t Angle;
typedef int16_t Fixed14;

#define FIXED14_ONE 0x4000

/* Sine and cosine from the quarter-wave table. */
void sin_cos(Angle angle, Fixed14 *sine, Fixed14 *cosine);

/* x + y attenuated (quartered above 4, halved above 2), less y. */
int16_t attenuate_offset(int16_t x, int16_t y);

/* Quotient rounded to nearest, halves away from zero (68000 DIVS.W: on
 * overflow the dividend words stand in for quotient and remainder). */
int16_t rounded_divide(int32_t dividend, int16_t divisor);

/* DIVIDE_QUOTIENT = rounded_divide(DIVIDE_NUMERATOR, DIVIDE_DENOMINATOR). */
void divide_rounded(void);

/* 3x3 rotation about the vertical axis, 2.14: [c 0 s / 0 1 0 / -s 0 c].
 * `angle` is in eighths of the sin_cos unit. */
void y_rotation_matrix(int16_t angle, gaddr out);

/* Move *value toward zero: by value >> shift outside +-15, by 1 inside
 * (so 0 becomes -1). */
void decay_toward_zero(gaddr value, int16_t shift);

/* x * 5/8 (as x/2 + x/8, each rounded down). */
int16_t five_eighths(int16_t x);

/* One pseudo-random bit from the 31-bit shift register. */
int32_t random_bit(void);

/* Past +-limit, move *value toward zero by value >> shift; within it, zero. */
void decay_outside_limit(gaddr value, int16_t limit, int16_t shift);

/* As y_rotation_matrix, in 2.8 fixed point ($100 = 1.0). */
void y_rotation_matrix8(int16_t angle, gaddr out);

/* Grid-cell difference as a 16.16 position step (a quarter unit per cell). */
int32_t cell_step(int16_t cells);

/* `count` pseudo-random bits, first bit highest. */
int32_t random_bits(int32_t count);

/* Integer square root of SQRT_INPUT into SQRT_RESULT (Newton's method;
 * large inputs are scaled down by 16 or 256 first). */
void square_root(void);

/* Signed 32-bit division truncating toward zero, remainder with the
 * dividend's sign; x / 0 gives 0 remainder 0 (the compiler's runtime divide
 * at $C52EC8). */
int32_t long_divide(int32_t dividend, int32_t divisor, int32_t *remainder);

/* Approximate length of (x, y, z), components nonnegative ($C1D974): the
 * larger of x and y scaled by sqrt(1 + ratio^2) from MAGNITUDE_TABLE, then
 * the same with z. Capped at $7FFF (low word) and stored in MAGNITUDE. */
int32_t magnitude3(int16_t x, int16_t y, int16_t z);

/* NORMALIZED = (x, y, z) scaled to about `scale` / length, signed by the
 * sign of `scale` ($C25754, using magnitude3). All zero when the scale or
 * the length is zero. */
void normalize_vector(int32_t scale, int32_t x, int32_t y, int32_t z);

/* $C265E8: the first flagged slot (bit 0 of +$27, scanning 19 down to 0)
 * that is armed (bit 5 of +$26, or any context running): 1 when its
 * record's distance from the observer is at most the slot's own offset
 * point's distance, else 0. */
int flagged_slot_in_range(void);

#endif
