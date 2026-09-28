#ifndef FA18_MATRIX_PIPELINE_H
#define FA18_MATRIX_PIPELINE_H

#include <stddef.h>
#include <stdint.h>

#include "matrix_pipeline_tail.h"

/* Mutable `$C2D9BA` workspace. Record offsets are relative to the caller's
 * `$C46184`-equivalent record store; all table/page ownership remains outside
 * this source-local matrix pipeline. */
typedef struct {
    int8_t enable_state;
    uint8_t mode;
    uint8_t flag_byte;
    uint8_t skip_coordinate_update;
    uint8_t sign_state;
    int16_t active_record_offset;
    int16_t fallback_record_offset;
    int16_t selection_cache;
    int16_t matrix_input[2];
    int16_t coordinate_output[2];
    int16_t row_scale[3];
    int16_t record_auxiliary[3];
    int32_t origin[3];
} FA18MatrixPipelineState;

typedef int (*FA18MatrixPipelineTransform)(void *context, const uint8_t *record,
                                           int32_t selector, int32_t output[3]);
typedef int (*FA18MatrixPipelineCoordinateUpdate)(void *context,
                                                   const int32_t relative[3],
                                                   int32_t argument,
                                                   int16_t input_x, int16_t input_z,
                                                   int16_t output[2]);
typedef struct {
    FA18MatrixPipelineTransform transform_record;
    FA18MatrixPipelineCoordinateUpdate update_coordinates;
    void *context;
} FA18MatrixPipelineOps;

/* `$C2D9BA-$C2DADF`: select its source record, perform the required C091E0
 * transform, conditionally update the two angle inputs, then build the
 * `$C45BD8`/`$C45BFC` caches. A disabled source state reaches unported
 * `$C2D9B0` and returns -2; missing required external source calls return -1. */
int fa18_run_matrix_pipeline(const FA18FlightTrigTable *trig_table,
                             const uint8_t *record_store, size_t record_store_size,
                             FA18MatrixPipelineState *state,
                             const FA18MatrixPipelineOps *ops,
                             FA18MatrixPipelineTailState *result);

#endif
