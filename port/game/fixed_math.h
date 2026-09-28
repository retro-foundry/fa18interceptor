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

#endif
