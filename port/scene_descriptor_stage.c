#include "scene_descriptor_stage.h"

int fa18_run_scene_descriptor_stage(FA18ScenePlacementRecord *placement,
                                    const FA18SceneDescriptorStageRecord *record,
                                    FA18SceneDescriptorStageCall call,
                                    void *context,
                                    FA18SceneDescriptorStageState *state) {
    if (!placement || !record || !call || !state) return -1;
    state->context = record->context;
    state->control_stream = record->control_stream;
    state->secondary_stream = record->secondary_stream;
    const int32_t result = call(context, record);
    state->stored_result = result > 0 ? (int16_t)result : -1;
    placement->per_frame[3] = (uint16_t)state->stored_result;
    return 0;
}
