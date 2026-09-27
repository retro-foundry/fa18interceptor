#include "matrix_row_scale.h"

#include <stdint.h>

static int32_t long_asr8(int32_t value) {
    if (value >= 0) return value >> 8;
    const int64_t magnitude = -(int64_t)value;
    return (int32_t)-((magnitude + 255) >> 8);
}

int fa18_scale_matrix_rows(int16_t matrix[3][3], const int16_t scale[3]) {
    if (!matrix || !scale) return -1;
    for (unsigned row = 0; row != 3; ++row) {
        for (unsigned column = 0; column != 3; ++column) {
            const int32_t product = (int32_t)matrix[row][column] * scale[row];
            matrix[row][column] = (int16_t)(uint16_t)long_asr8(product);
        }
    }
    return 0;
}
