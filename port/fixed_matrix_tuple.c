#include "fixed_matrix_tuple.h"

#include <stdint.h>

static int32_t asr_long8(int32_t value) {
    if (value >= 0) return value >> 8;
    const int64_t magnitude = -(int64_t)value;
    return (int32_t)-((magnitude + 255) >> 8);
}

static int32_t matrix_row_tuple(const int16_t row[3]) {
    const uint32_t first = (uint32_t)((int32_t)row[0] * (int16_t)0xe000);
    const uint32_t second = (uint32_t)((int32_t)row[1] * 0x3800);
    const uint32_t third = (uint32_t)((int32_t)row[2] * (int16_t)0xe000);
    return asr_long8((int32_t)(first + second + third));
}

int fa18_build_fixed_matrix_product_tuple(const int16_t matrix[3][3],
                                          FA18FixedMatrixTuple *tuple) {
    if (!matrix || !tuple) return -1;
    tuple->d0 = matrix_row_tuple(matrix[0]);
    tuple->d1 = matrix_row_tuple(matrix[1]);
    tuple->d2 = matrix_row_tuple(matrix[2]);
    tuple->child_mode = 8;
    tuple->child_state_word = 9;
    return 0;
}
