#ifndef FA18_MATRIX_TUPLE_VALIDATION_H
#define FA18_MATRIX_TUPLE_VALIDATION_H

#include <stdint.h>

typedef struct { int32_t published_result; } FA18MatrixTupleValidationState;

/* `$C2EC9C-$C2ECC5` plus `$C2EC82-$C2EC8F`. Returns 0 for the shared
 * rejection, 1 for the unported positive-bound continuation, and -2 for the
 * separate nonpositive-bound branch. */
int fa18_validate_matrix_product_tuple(int32_t d0, int32_t d1, int32_t d2,
                                       FA18MatrixTupleValidationState *state);

#endif
