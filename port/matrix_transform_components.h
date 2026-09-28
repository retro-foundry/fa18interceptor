#ifndef FA18_MATRIX_TRANSFORM_COMPONENTS_H
#define FA18_MATRIX_TRANSFORM_COMPONENTS_H

#include <stdint.h>

/* `$C091F0-$C09249`: transform the three signed source words through the
 * active 8.8 matrix, arithmetic-shift each wrapped dot product by four, then
 * add the caller-owned record `+$14/+$18/+$1C` triple. */
int fa18_calculate_matrix_transform_components(const int16_t input[3],
                                               const int16_t matrix[3][3],
                                               const int32_t translation[3],
                                               int32_t output[3]);

#endif
