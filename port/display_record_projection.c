#include "display_record_projection.h"

#include <limits.h>

static int16_t clamp_word(int32_t value, int16_t low, int16_t high) {
    if (value < low) return low;
    if (value > high) return high;
    return (int16_t)value;
}

/* The source divides the signed DIVS quotient by two with ASR, then adds one
 * when that shift carried.  This is truncation toward zero for negative odd
 * values and ceiling for positive odd values. */
static int project_half_quotient(int32_t dividend, int16_t divisor,
                                 int16_t *result) {
    int32_t quotient;

    if (!result || divisor <= 0) return -1;
    quotient = dividend / divisor;
    if (quotient < INT16_MIN || quotient > INT16_MAX) return -1;
    if (quotient > 0 && (quotient & 1)) ++quotient;
    *result = (int16_t)(quotient / 2);
    return 0;
}

int fa18_project_adjusted_display_pair(int16_t x, int16_t y, int16_t divisor,
                                       FA18DisplayRecordPair *pair) {
    int16_t projected_x;
    int16_t projected_y;

    if (!pair ||
        project_half_quotient((int32_t)x * 0x140, divisor, &projected_x) ||
        project_half_quotient((int32_t)y * 0xb4, divisor, &projected_y)) {
        return -1;
    }
    pair->x = clamp_word((int32_t)projected_x + 0xa0, 0, 0x13f);
    pair->y = clamp_word((int32_t)projected_y + 0x5a, 0, 0xb3);
    return 0;
}
