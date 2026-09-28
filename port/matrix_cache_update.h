#ifndef FA18_MATRIX_CACHE_UPDATE_H
#define FA18_MATRIX_CACHE_UPDATE_H

#include <stdint.h>

#include "flight.h"

typedef struct {
    int16_t angle[3];
    int16_t row_scale[3];
} FA18MatrixCacheUpdateInput;

/* `$C2DC9A-$C2DCC0`: compose the three live `$C45A8A/$8E/$92` words into
 * `$C45BD8`, then scale its rows through `$C2E5AC` using `$C45A3E`. */
int fa18_update_projection_matrix_cache(
    const FA18FlightTrigTable *trig_table,
    const FA18MatrixCacheUpdateInput *input,
    int16_t projection_matrix[3][3]);

#endif
