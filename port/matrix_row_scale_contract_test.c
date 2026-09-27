#include "matrix_row_scale.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    int16_t matrix[3][3] = {
        {0x0100, -0x0100, 0x7fff},
        {0x0100, -0x0100, (int16_t)0x8000},
        {0x0100, -0x0100, 0x0200}
    };
    const int16_t scale[3] = {0x0100, -0x0100, 0x0080};
    assert(fa18_scale_matrix_rows(matrix, scale) == 0);
    assert(matrix[0][0] == 0x0100 && matrix[0][1] == -0x0100 &&
           matrix[0][2] == 0x7fff);
    assert(matrix[1][0] == -0x0100 && matrix[1][1] == 0x0100 &&
           matrix[1][2] == (int16_t)0x8000);
    assert(matrix[2][0] == 0x0080 && matrix[2][1] == -0x0080 &&
           matrix[2][2] == 0x0100);
    assert(fa18_scale_matrix_rows(NULL, scale) == -1);
    assert(fa18_scale_matrix_rows(matrix, NULL) == -1);
    puts("matrix row scale contract passed");
    return 0;
}
