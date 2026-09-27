#ifndef FA18_FIXED_MATRIX_TUPLE_H
#define FA18_FIXED_MATRIX_TUPLE_H

#include <stdint.h>

typedef struct {
    int32_t d0, d1, d2;
    uint16_t child_state_word;
    int16_t child_mode;
} FA18FixedMatrixTuple;

/* `$C0DAEE-$C0DB3A`: fixed ($e000,$3800,$e000) input through three
 * consecutive matrix rows. The caller owns `$C2EC9C` dispatch. */
int fa18_build_fixed_matrix_product_tuple(const int16_t matrix[3][3],
                                          FA18FixedMatrixTuple *tuple);

#endif
