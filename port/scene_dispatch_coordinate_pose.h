#ifndef FA18_SCENE_DISPATCH_COORDINATE_POSE_H
#define FA18_SCENE_DISPATCH_COORDINATE_POSE_H

#include <stddef.h>
#include <stdint.h>

#include "scene_record_dispatch.h"

typedef struct {
    int32_t d0, d1, d2, d3, d4, d5;
} FA18SceneCoordinateUpdateInput;

typedef int (*FA18SceneCoordinateUpdate)(void *context,
                                         const FA18SceneCoordinateUpdateInput *input,
                                         int16_t output[2]);

/* `$C28800-$C288C4`, nonnegative `$04(A2)` route: use its high byte to select
 * a published `$C46184` slot, copy its placement fields into the caller-owned
 * record, derive the exact six-longword `$C123FA` packet, then publish
 * `(0, C45AC2, 0)` through `$C2D954`.  The coordinate callback is required:
 * no captured coordinate output is a runtime substitute. */
int fa18_publish_scene_dispatch_coordinate_pose(
    FA18SceneDispatchRecord *target, const FA18SceneDispatchRecord *linked_record,
    int16_t source_selector, FA18SceneCoordinateUpdate coordinate_update,
    void *coordinate_context, const FA18RecordMatrixUpdateOps *matrix_ops);

#endif
