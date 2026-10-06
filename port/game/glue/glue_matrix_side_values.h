#ifndef FA18_GLUE_MATRIX_SIDE_VALUES_H
#define FA18_GLUE_MATRIX_SIDE_VALUES_H

#include "memory.h"

/* Temporary original-call observations. These ordinary C values carry no
 * CPU/register-file pointer; game behavior remains in control_records.c. */
typedef struct {
    int16_t index, lane_z, before_y, before_z;
    gaddr record, header;
    uint16_t header_before;
    uint32_t work, relaxed;
    gaddr final_address;
} MatrixSideValues;

void matrix_side_capture(MatrixSideValues *values);
void matrix_side_finish(MatrixSideValues *values, uint32_t *work, uint32_t *relaxed);

#endif
