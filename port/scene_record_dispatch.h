#ifndef FA18_SCENE_RECORD_DISPATCH_H
#define FA18_SCENE_RECORD_DISPATCH_H

#include <stdint.h>

#include "record_matrix_update.h"

/* `$C46184 + (index << 9)`: only the direct fields written by the creation
 * arm at `$C28BEE-$C28E08` are named here. */
typedef struct {
    uint8_t bytes[512];
    FA18RecordMatrixUpdateState matrix_update;
} FA18SceneDispatchRecord;

typedef struct {
    uint8_t record_type;
    uint8_t record_index;
    uint8_t class_nibble;
    uint16_t source_flags;
    uint16_t source_word_1;
    uint16_t source_word_2;
    int16_t coordinate_x;
    int16_t coordinate_z;
    int16_t component_x;
    int16_t component_z;
    int32_t altitude;
    int16_t inherited_d7;
} FA18SceneDispatchCreateInput;

/* Direct creation arm of `$C28B34`.  The enclosing pointer-table iteration,
 * geometry-table lookup, and `$C28F16` positive-coordinate helper are caller
 * boundaries. */
int fa18_create_scene_dispatch_record(FA18SceneDispatchRecord *record,
                                      const FA18SceneDispatchCreateInput *input,
                                      const FA18RecordMatrixUpdateOps *matrix_ops);

#endif
