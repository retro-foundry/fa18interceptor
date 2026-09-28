#include "magnitude_refinement.h"

#include <limits.h>

static int divu_word(uint32_t dividend, uint16_t divisor, uint16_t *quotient) {
    uint32_t value;
    if (!quotient || divisor == 0) return -1;
    value = dividend / divisor;
    if (value > UINT16_MAX) return -1;
    *quotient = (uint16_t)value;
    return 0;
}

static int refine(uint32_t value, uint16_t output_shift, uint16_t *threshold) {
    uint16_t divisor;
    uint16_t quotient;
    int16_t difference;

    if (divu_word(value, 0x00c8u, &divisor) != 0) return -1;
    divisor = (uint16_t)(divisor + 2u);
    for (;;) {
        if (divu_word(value, divisor, &quotient) != 0) return -1;
        difference = (int16_t)(uint16_t)(quotient - divisor);
        if (difference == 0 || difference == 1 || difference == -1) break;
        divisor = (uint16_t)((uint16_t)(divisor + quotient) >> 1);
    }
    *threshold = (uint16_t)(quotient << output_shift);
    return 0;
}

int fa18_refine_component_magnitude(uint32_t square_sum, uint16_t *threshold) {
    if (!threshold) return -1;
    if (square_sum <= UINT32_C(0x0063f000))
        return refine(square_sum, 0, threshold);
    if (square_sum <= UINT32_C(0x063f0000))
        return refine(square_sum >> 4, 2, threshold);
    return refine(square_sum >> 8, 4, threshold);
}
