#ifndef FA18_RECORD_MATRIX_UPDATE_H
#define FA18_RECORD_MATRIX_UPDATE_H

#include <stdint.h>

typedef struct {
    int16_t published[3];
    int16_t build_matrix[3][3];
    int16_t attitude_matrix[3][3];
    uint8_t control_byte_03;
} FA18RecordMatrixUpdateState;

/* Register-shaped input at `$C2D954`: D4-D7. */
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

#endif
