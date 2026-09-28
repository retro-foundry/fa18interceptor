#ifndef FA18_SCENE_ROOT_PLACEMENT_H
#define FA18_SCENE_ROOT_PLACEMENT_H

#include <stdint.h>

#include "scene_negative_pose.h"
#include "scene_record_table.h"
#include "scene_root_setup.h"

/* `$C0924A-$C095BE` observed negative-table route.  The selected record and
 * descriptor are mutable owners outside this packet, so resolve them from the
 * live bank/template rather than assigning a native scene or page. */
typedef int (*FA18SceneRootPlacementRecordResolver)(void *context,
                                                    uint16_t record_index,
                                                    FA18SceneNegativePoseRecord *record);
typedef int (*FA18SceneRootPlacementDescriptorResolver)(void *context,
                                                        uint16_t record_index,
                                                        FA18SceneNegativePoseDescriptor *descriptor);

typedef struct {
    uint8_t table_index;
    int16_t inherited_d7;
} FA18SceneRootPlacementInput;

typedef struct {
    FA18SceneRootPlacementRecordResolver resolve_record;
    FA18SceneRootPlacementDescriptorResolver resolve_descriptor;
    const FA18SceneNegativePoseOps *negative_pose_ops;
    void *context;
} FA18SceneRootPlacementOps;

typedef struct {
    FA18SceneRootSetupState setup;
    FA18SceneNegativePoseState pose;
    uint16_t selected_record_index;
    uint8_t retry_scene_index;
} FA18SceneRootPlacementState;

typedef enum {
    FA18_SCENE_ROOT_PLACEMENT_NEGATIVE_APPLIED,
    FA18_SCENE_ROOT_PLACEMENT_NEGATIVE_RETRY,
    FA18_SCENE_ROOT_PLACEMENT_POSITIVE_UNPORTED
} FA18SceneRootPlacementRoute;

/* The root-reset prefix calls `$C09620` then `$C095C0`; a negative Hunk-67
 * entry then follows `$C09498-$C095BE`. A nonnegative entry is the distinct
 * unported `$C093BC` family and returns its route without a substitute. */
int fa18_prepare_scene_root_placement(
    const FA18SceneRecordTable *table,
    const FA18SceneRootPlacementInput *input,
    FA18SceneRootPlacementState *state,
    const FA18SceneRootPlacementOps *ops,
    FA18SceneRootPlacementRoute *route);

#endif
