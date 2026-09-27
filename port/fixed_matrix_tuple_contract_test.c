#include "fixed_matrix_tuple.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    const int16_t identity[3][3] = {
        {0x4000, 0, 0}, {0, 0x4000, 0}, {0, 0, 0x4000}
    };
    FA18FixedMatrixTuple tuple = {0};
    assert(fa18_build_fixed_matrix_product_tuple(identity, &tuple) == 0);
    assert(tuple.d0 == -0x080000 && tuple.d1 == 0x0e0000 &&
           tuple.d2 == -0x080000 && tuple.child_mode == 8 &&
           tuple.child_state_word == 9);
    assert(fa18_build_fixed_matrix_product_tuple(NULL, &tuple) == -1);
    assert(fa18_build_fixed_matrix_product_tuple(identity, NULL) == -1);
    puts("fixed matrix tuple contract passed");
    return 0;
}
