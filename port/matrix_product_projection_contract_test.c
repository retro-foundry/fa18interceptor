#include "matrix_product_projection.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    FA18MatrixProductProjectionState state = {0, 0, 100};
    int16_t d7 = -5, returned_x = 0;
    assert(fa18_project_matrix_product_tuple(-19, 1452, 7990, &d7, &state,
                                             &returned_x) == 1);
    assert(state.projected_x == 159 && state.projected_y == 74);
    assert(d7 == -1 && returned_x == 159);

    state.project_limit = 73;
    d7 = -5;
    assert(fa18_project_matrix_product_tuple(-19, 1452, 7990, &d7, &state,
                                             &returned_x) == 0);
    assert(state.projected_x == -1 && state.projected_y == -1 && d7 == -5);

    state.project_limit = 200;
    d7 = -5;
    assert(fa18_project_matrix_product_tuple(-32768, 32767, 160, &d7, &state,
                                             &returned_x) == 1);
    assert(state.projected_x == 319 && state.projected_y == 1);
    d7 = -1;
    assert(fa18_project_matrix_product_tuple(-19, 1452, 7990, &d7, &state,
                                             &returned_x) == 2);
    d7 = -5;
    assert(fa18_project_matrix_product_tuple(1, 1, 0, &d7, &state,
                                             &returned_x) == -2);
    assert(fa18_project_matrix_product_tuple(32767, 1, 1, &d7, &state,
                                             &returned_x) == -2);
    assert(fa18_project_matrix_product_tuple(0, 0, 1, NULL, &state,
                                             &returned_x) == -1);
    puts("matrix product projection contract passed");
    return 0;
}
