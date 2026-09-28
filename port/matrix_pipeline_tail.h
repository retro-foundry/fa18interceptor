#ifndef FA18_MATRIX_PIPELINE_TAIL_H
#define FA18_MATRIX_PIPELINE_TAIL_H

#include "flight.h"

#include <stdint.h>

/* Caller-owned state at the proved `$C2DAB0-$C2DAF1` tail of `$C2D9BA`.
 * `$C091E0/$C123FA` remain the owner of the two angle inputs. */
typedef struct {
    int16_t input_x;
    int16_t input_z;
    int16_t row_scale[3];
    int16_t record_auxiliary[3];
} FA18MatrixPipelineTailInput;

typedef struct {
    int16_t projection_matrix[3][3];
    int16_t single_angle_matrix[3][3];
    int16_t auxiliary[3];
} FA18MatrixPipelineTailState;

/* `$C2DAB0-$C2DAF1`: build `$C45BD8`, scale its rows from `$C45A3E`,
 * build `$C45BFC`, then copy `$C461EA` to `$C45A88`. */
int fa18_run_matrix_pipeline_tail(const FA18FlightTrigTable *trig_table,
                                  const FA18MatrixPipelineTailInput *input,
                                  FA18MatrixPipelineTailState *state);

#endif
