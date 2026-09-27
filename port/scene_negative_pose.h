#ifndef FA18_SCENE_NEGATIVE_POSE_H
#define FA18_SCENE_NEGATIVE_POSE_H

#include <stdint.h>

#include "record_matrix_update.h"
#include "scene_vector_transform.h"

typedef struct {
    uint8_t flags_byte_01;
    FA18SceneVectorMatrix matrix;
    FA18SceneVectorBase base;
    int16_t angles[3];
} FA18SceneNegativePoseRecord;

typedef struct {
    int32_t value_02;
} FA18SceneNegativePoseDescriptor;

typedef int (*FA18SceneNegativePoseCall)(void *context);

typedef struct {
    FA18SceneNegativePoseCall nonnegative_descriptor;
    const FA18RecordMatrixUpdateOps *matrix_update;
    void *context;
} FA18SceneNegativePoseOps;

/* Direct root fields reached by `$C09498-$C095BE`. */
typedef struct {
    uint32_t word_10;
    int32_t position[3];
    uint8_t flags_byte_04;
    uint16_t word_06, word_08, word_0a, word_0b, word_0c, word_0e, word_a4;
    FA18RecordMatrixUpdateState matrix_update;
} FA18SceneNegativePoseState;

/* `$C09498` negative table-entry route. `scene_index` is cleared and one is
 * returned when the selected record lacks bit 6, matching the source retry. */
int fa18_initialize_negative_scene_pose(int16_t table_word,
                                        uint8_t *scene_index,
                                        const FA18SceneNegativePoseRecord *selected,
                                        const FA18SceneNegativePoseDescriptor *descriptor,
                                        int16_t inherited_d7,
                                        FA18SceneNegativePoseState *state,
                                        const FA18SceneNegativePoseOps *ops);

#endif
