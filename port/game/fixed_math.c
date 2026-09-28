/* Fixed-point trigonometry. */
#include "fixed_math.h"

#include "globals.h"
#include "memory.h"

/* sin(i / 10 degrees) for i = 0..900, 2.14 fixed point. */
static Fixed14 quarter_sine(int16_t tenths) {
    return rd_s16(SINE_TABLE + (gaddr)(int32_t)(int16_t)(tenths * 2));
}

void sin_cos(Angle angle, Fixed14 *sine, Fixed14 *cosine) {
    if (angle < 900) {
        *sine = quarter_sine(angle);
        *cosine = quarter_sine((int16_t)(900 - angle));
    } else if (angle < 1800) {
        *sine = quarter_sine((int16_t)(1800 - angle));
        *cosine = (Fixed14)-quarter_sine((int16_t)(angle - 900));
    } else if (angle < 2700) {
        *sine = (Fixed14)-quarter_sine((int16_t)(angle - 1800));
        *cosine = (Fixed14)-quarter_sine((int16_t)(2700 - angle));
    } else {
        *sine = (Fixed14)-quarter_sine((int16_t)(3600 - angle));
        *cosine = quarter_sine((int16_t)(angle - 2700));
    }
}

int16_t attenuate_offset(int16_t x, int16_t y) {
    int16_t sum = (int16_t)(x + y);
    int16_t size = (int16_t)(sum < 0 ? -sum : sum);

    if (size > 4) sum = (int16_t)(sum >> 2);
    else if (size > 2) sum = (int16_t)(sum >> 1);
    return (int16_t)(sum - y);
}
