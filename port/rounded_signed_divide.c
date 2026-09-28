#include "rounded_signed_divide.h"

#include <limits.h>

static int16_t word_negate(int16_t value) {
    return (int16_t)(uint16_t)(UINT16_C(0) - (uint16_t)value);
}

static int16_t word_asr1(int16_t value) {
    if (value >= 0) return (int16_t)(value >> 1);
    return (int16_t)-((-(int32_t)value + 1) >> 1);
}

int fa18_round_signed_divide(int32_t dividend, int16_t divisor,
                             int16_t *result) {
    int64_t quotient;
    int32_t remainder;
    int16_t threshold;
    int16_t remainder_magnitude;

    if (!result || divisor == 0) return -1;
    quotient = (int64_t)dividend / divisor;
    if (quotient < INT16_MIN || quotient > INT16_MAX) return -1;
    remainder = (int32_t)((int64_t)dividend % divisor);
    threshold = divisor < 0 ? word_negate(divisor) : divisor;
    threshold = word_asr1(threshold);
    remainder_magnitude = (int16_t)remainder;
    if (remainder_magnitude < 0) remainder_magnitude = word_negate(remainder_magnitude);

    /* `$C259A2`: preserve the quotient when the half-divisor is strictly
     * greater than the remainder magnitude; otherwise round away from zero. */
    if (threshold > remainder_magnitude) {
        *result = (int16_t)quotient;
    } else if (quotient < 0) {
        *result = (int16_t)((uint16_t)quotient - UINT16_C(1));
    } else {
        *result = (int16_t)((uint16_t)quotient + UINT16_C(1));
    }
    return 0;
}
