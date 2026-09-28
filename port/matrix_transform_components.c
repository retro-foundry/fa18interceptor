#include "matrix_transform_components.h"

static int32_t asr_long_4(int32_t value) {
    return value >= 0 ? value >> 4 :
        (int32_t)-((-(int64_t)value + 15) >> 4);
}

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t dot(const int16_t input[3], const int16_t row[3]) {
    uint32_t result = 0;
    for (unsigned index = 0; index != 3; ++index)
        result += (uint32_t)((int32_t)input[index] * row[index]);
    return (int32_t)result;
}

int fa18_calculate_matrix_transform_components(const int16_t input[3],
                                               const int16_t matrix[3][3],
                                               const int32_t translation[3],
                                               int32_t output[3]) {
    if (!input || !matrix || !translation || !output)
        return -1;
    for (unsigned row = 0; row != 3; ++row)
        output[row] = add_long(asr_long_4(dot(input, matrix[row])), translation[row]);
    return 0;
}
