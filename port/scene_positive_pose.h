#ifndef FA18_SCENE_POSITIVE_POSE_H
#define FA18_SCENE_POSITIVE_POSE_H

#include <stdint.h>

#include "record_matrix_update.h"

/* `$C093BE-$C095BE`, the nonnegative entry selected by the cold-boot root
 * table.  The two table-derived byte pairs are supplied by the owner of the
 * original `$C1D8D6/$C1D7E2` data; this routine does not assign them a world
 * meaning. */
typedef struct {
    int16_t entry_words[5];
    int16_t tail_words[3];
    int8_t grid_adjustment[2];
    int16_t position_adjustment[2];
} FA18ScenePositivePoseInput;

typedef struct {
    uint16_t header_word_00;
    uint16_t header_word_02;
    uint8_t header_byte_04;
    uint8_t byte_0b;
    uint16_t word_06, word_08, word_0a, word_0c, word_0e;
    int32_t position[3];
    int32_t published_delta[3];
    int16_t published_grid[2];
    FA18RecordMatrixUpdateState matrix_update;
} FA18ScenePositivePoseState;

/* `$C093BE-$C095BE`: publish the selected root pose and issue the existing
 * `$C2D954` matrix-update boundary.  Negative table entries take a different
 * route and are rejected here. */
int fa18_initialize_positive_scene_pose(
    const FA18ScenePositivePoseInput *input,
    FA18ScenePositivePoseState *state,
    const FA18RecordMatrixUpdateOps *matrix_ops);

#endif
