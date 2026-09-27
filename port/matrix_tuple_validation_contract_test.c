#include "matrix_tuple_validation.h"
#include <assert.h>
#include <stdio.h>
int main(void) {
    FA18MatrixTupleValidationState state = {0};
    assert(fa18_validate_matrix_product_tuple(2120, 0, -5536, &state) == 0);
    assert(state.published_result == -1);
    assert(fa18_validate_matrix_product_tuple(-19, 1452, 7990, &state) == 1);
    assert(fa18_validate_matrix_product_tuple(0, 0, 0, &state) == 0);
    assert(fa18_validate_matrix_product_tuple(0, 0, 1, NULL) == -1);
    puts("matrix tuple validation contract passed"); return 0;
}
