#include "matrix_transform_components.h"

#include <assert.h>

int main(void) {
    const int16_t input[3] = {1, 2, 3};
    const int16_t matrix[3][3] = {
        {0x100, 0, 0}, {0, 0x100, 0}, {0, 0, 0x100}
    };
    const int32_t translation[3] = {100, -200, 300};
    int32_t output[3];

    assert(fa18_calculate_matrix_transform_components(input, matrix, translation, output) == 0);
    assert(output[0] == 116 && output[1] == -168 && output[2] == 348);
    assert(fa18_calculate_matrix_transform_components(0, matrix, translation, output) == -1);
    return 0;
}
