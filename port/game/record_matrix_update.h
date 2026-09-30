#ifndef FA18_GAME_RECORD_MATRIX_UPDATE_H
#define FA18_GAME_RECORD_MATRIX_UPDATE_H

#include "memory.h"
#include "matrix.h"

/* $C2D408-$C2D492: class-$30 route through $C2D954. Returns 1 when the
 * tracking call ran, 0 when the existing angle words went straight through. */
int update_record_class30_matrix(gaddr record);

/* $C2D496-$C2D99A. `d` starts as the caller's data registers and retains
 * the pre-transform arithmetic for register glue. The optional side hook
 * runs the already ported $C1342C once and publishes its register output. */
typedef struct RecordMatrixRun {
    uint32_t d[8];
    MatrixTransformAngleState transform;
    uint32_t last_term;
    int16_t transformed_angles[3];
    uint16_t angles[3];
    int orientation_written;
    int used_matrix_side;
    int used_depth;
} RecordMatrixRun;
typedef void (*RecordMatrixSideHook)(void *context, uint32_t d[8]);
void update_record_nonclass_matrix(gaddr record, RecordMatrixRun *run,
                                   RecordMatrixSideHook side_hook, void *context);

/* $C2D704-$C2D99A: the continuation after the record matrix transform.
 * Angles are D4-D6 on entry. Returns 1 when it reaches the orientation
 * writer, 0 when the source returns early after changing a velocity word. */
int finish_record_matrix_angles(gaddr record, uint16_t angles[3]);

#endif
