#ifndef FA18_SCENE_DISPATCH_NEGATIVE_COORDINATE_POSE_H
#define FA18_SCENE_DISPATCH_NEGATIVE_COORDINATE_POSE_H

#include <stdint.h>

#include "scene_dispatch_coordinate_pose.h"

typedef int (*FA18SceneDispatchNegativeGeometryLookup)(
    void *context, uint16_t selector_index, int16_t geometry[5]);

/* `$C28808-$C288C4`, negative `$04(A2)` route.  The signed selector supplies
 * a `$C295E0` word-table index; its resolved five-word tuple is copied through
 * `$C28F16`, expanded into the exact `$C123FA` coordinate packet, and then
 * published as `(0, C45AC2, 0)` by `$C2D954`.  Geometry-table ownership stays
 * with the source dispatch caller. */
int fa18_publish_scene_dispatch_negative_coordinate_pose(
    FA18SceneDispatchRecord *target, int16_t source_selector,
    FA18SceneDispatchNegativeGeometryLookup geometry_lookup,
    void *geometry_context, FA18SceneCoordinateUpdate coordinate_update,
    void *coordinate_context, const FA18RecordMatrixUpdateOps *matrix_ops);

#endif
