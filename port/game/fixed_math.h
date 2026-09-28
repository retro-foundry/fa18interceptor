#ifndef FA18_GAME_FIXED_MATH_H
#define FA18_GAME_FIXED_MATH_H

#include <stdint.h>

/* Angles are tenths of a degree, 0-3599. Sines are 2.14 fixed point
 * ($4000 = 1.0). */
typedef int16_t Angle;
typedef int16_t Fixed14;

#define FIXED14_ONE 0x4000

/* Sine and cosine from the quarter-wave table. */
void sin_cos(Angle angle, Fixed14 *sine, Fixed14 *cosine);

/* x + y attenuated (quartered above 4, halved above 2), less y. */
int16_t attenuate_offset(int16_t x, int16_t y);

#endif
