#ifndef FA18_MATRIX_PRODUCT_PROJECTION_H
#define FA18_MATRIX_PRODUCT_PROJECTION_H

#include <stdint.h>

/* Caller-owned projection workspace at `$C45958` and `$C45984`. */
typedef struct {
    int16_t projected_x;
    int16_t projected_y;
    int16_t project_limit;
} FA18MatrixProductProjectionState;

/* `$C2ECC6-$C2ED6B`: project a tuple already accepted by `$C2EC9C`.  Returns
 * 0 for the shared project-limit rejection, 1 for the observed negative-D7
 * return, 2 when the source reaches an unported continuation, and -2 when a
 * 68000 DIVS.W fault (zero divisor or quotient overflow) prevents projection.
 * On the observed return, `*returned_x` receives D0's projected low word. */
int fa18_project_matrix_product_tuple(int32_t d0, int32_t d1, int32_t d2,
                                      int16_t *d7,
                                      FA18MatrixProductProjectionState *state,
                                      int16_t *returned_x);

#endif
