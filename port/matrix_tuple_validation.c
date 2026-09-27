#include "matrix_tuple_validation.h"

static int16_t low_word(int32_t value) { return (int16_t)(uint16_t)value; }
static int16_t word_negate(int16_t value) {
    return (int16_t)(uint16_t)(UINT16_C(0) - (uint16_t)value);
}

int fa18_validate_matrix_product_tuple(int32_t d0, int32_t d1, int32_t d2,
                                       FA18MatrixTupleValidationState *state) {
    if (!state) return -1;
    const int16_t first = low_word(d0);
    const int16_t second = low_word(d1);
    const int16_t bound = low_word(d2);
    if (first >= bound || second >= bound || word_negate(first) >= bound ||
        word_negate(second) >= bound) {
        state->published_result = -1;
        return 0;
    }
    if (bound <= 0) return -2;
    return 1;
}
