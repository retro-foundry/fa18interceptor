#include "signed_matrix_product.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
    const int16_t left[3][3] = {
        {1, 2, 3}, {4, 5, 6}, {7, 8, 9}
    };
    const int16_t right[3][3] = {
        {-2, 3, 4}, {5, -6, 7}, {8, 9, -10}
    };
    int32_t result[3][3] = {{0}};
    assert(fa18_compute_signed_matrix_product(left, right, result) == 0);
    assert(result[0][0] == 38 && result[0][1] == 43 && result[0][2] == 48);
    assert(result[1][0] == 30 && result[1][1] == 36 && result[1][2] == 42);
    assert(result[2][0] == -26 && result[2][1] == -19 && result[2][2] == -12);
    const int16_t maximum[3][3] = {
        {32767, 32767, 32767}, {32767, 32767, 32767}, {32767, 32767, 32767}
    };
    assert(fa18_compute_signed_matrix_product(maximum, maximum, result) == 0);
    assert((uint32_t)result[0][0] == UINT32_C(0xBFFD0003));
    assert(fa18_compute_signed_matrix_product(NULL, right, result) == -1);
    assert(fa18_compute_signed_matrix_product(left, NULL, result) == -1);
    assert(fa18_compute_signed_matrix_product(left, right, NULL) == -1);
    puts("signed matrix product contract passed");
    return 0;
}
