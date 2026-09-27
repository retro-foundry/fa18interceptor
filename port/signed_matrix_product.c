#include "signed_matrix_product.h"

#include <stdint.h>

static int32_t sum_products(int16_t first_left, int16_t first_right,
                            int16_t second_left, int16_t second_right,
                            int16_t third_left, int16_t third_right) {
    const uint32_t first = (uint32_t)((int32_t)first_left * first_right);
    const uint32_t second = (uint32_t)((int32_t)second_left * second_right);
    const uint32_t third = (uint32_t)((int32_t)third_left * third_right);
    return (int32_t)(first + second + third);
}

int fa18_compute_signed_matrix_product(const int16_t left[3][3],
                                       const int16_t right[3][3],
                                       int32_t result[3][3]) {
    if (!left || !right || !result) return -1;
    for (unsigned right_row = 0; right_row != 3; ++right_row) {
        for (unsigned left_column = 0; left_column != 3; ++left_column) {
            result[right_row][left_column] = sum_products(
                left[0][left_column], right[right_row][0],
                left[1][left_column], right[right_row][1],
                left[2][left_column], right[right_row][2]);
        }
    }
    return 0;
}
