#ifndef FA18_SIGNED_MATRIX_PRODUCT_H
#define FA18_SIGNED_MATRIX_PRODUCT_H

#include <stdint.h>

/* `$C2DEFC-$C2E017`: multiply the two caller-owned signed word matrices in
 * the source's storage order and retain the nine signed longword sums. */
int fa18_compute_signed_matrix_product(const int16_t left[3][3],
                                       const int16_t right[3][3],
                                       int32_t result[3][3]);

#endif
