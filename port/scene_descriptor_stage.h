#ifndef FA18_SCENE_DESCRIPTOR_STAGE_H
#define FA18_SCENE_DESCRIPTOR_STAGE_H

#include <stdint.h>

#include "scene_placement.h"

/* Four consecutive longwords consumed by `$C1CF96-$C1CFA0`. */
typedef struct {
    uint32_t entry;
    uint32_t context;
    uint32_t control_stream;
    uint32_t secondary_stream;
} FA18SceneDescriptorStageRecord;

typedef int32_t (*FA18SceneDescriptorStageCall)(void *context,
                                                const FA18SceneDescriptorStageRecord *record);

typedef struct {
    uint32_t context;
    uint32_t control_stream;
    uint32_t secondary_stream;
    int16_t stored_result;
} FA18SceneDescriptorStageState;

/* `$C1CF96-$C1CFBA`: publish a descriptor's indirect-stage fields, invoke its
 * caller-supplied native implementation, and commit the result to `+20` of
 * the mutable placement record. */
int fa18_run_scene_descriptor_stage(FA18ScenePlacementRecord *placement,
                                    const FA18SceneDescriptorStageRecord *record,
                                    FA18SceneDescriptorStageCall call,
                                    void *context,
                                    FA18SceneDescriptorStageState *state);

#endif
