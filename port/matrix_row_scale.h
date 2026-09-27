#ifndef FA18_MATRIX_ROW_SCALE_H
#define FA18_MATRIX_ROW_SCALE_H

#include <stdint.h>

/* `$C2E5AC-$C2E5F5`: multiply each matrix row by its corresponding signed
 * scale word, arithmetic-shift the longword product by eight, and retain the
 * low word. */
int fa18_scale_matrix_rows(int16_t matrix[3][3], const int16_t scale[3]);

#endif
