#ifndef FA18_RECORD_MATRIX_UPDATE_H
#define FA18_RECORD_MATRIX_UPDATE_H

#include "flight.h"

#include <stdint.h>

typedef struct {
    int16_t published[3];
    int16_t build_matrix[3][3];
    int16_t attitude_matrix[3][3];
    uint8_t control_byte_03;
} FA18RecordMatrixUpdateState;

/* Legacy packet input. The source saves D4-D6 and restores them into D5-D7;
 * both matrices therefore use the same original three angles. d7 is retained
 * only for existing callers and is not used. New native callers use live
 * records through native_record_orientation.h. */
typedef struct {
    int16_t d4, d5, d6, d7;
} FA18RecordMatrixUpdateInput;

typedef int (*FA18RecordMatrixBuild)(void *context, const int16_t input[3],
                                     int16_t output[3][3]);
typedef int (*FA18RecordMatrixCompose)(void *context, const int16_t input[3],
                                       int16_t output[3][3]);

typedef struct {
    FA18RecordMatrixBuild build;
    FA18RecordMatrixCompose compose;
    void *context;
} FA18RecordMatrixUpdateOps;

/* `$C2D94E-$C2D99A`: direct publication and ordered matrix-owner calls. */
int fa18_update_record_matrix(FA18RecordMatrixUpdateState *state,
                              const FA18RecordMatrixUpdateInput *input,
                              const FA18RecordMatrixUpdateOps *ops);

/* Concrete `$C2D94E-$C2D99A` route: publish the record triple, construct its
 * `$C2E47A` rotation matrix, then construct the `$C2E514` attitude matrix. */
int fa18_update_record_matrix_native(FA18RecordMatrixUpdateState *state,
                                     const FA18RecordMatrixUpdateInput *input,
                                     const FA18FlightTrigTable *trig_table);

#endif
