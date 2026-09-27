#include "scene_descriptor_stage.h"

#include <assert.h>

static int32_t return_value(void *context, const FA18SceneDescriptorStageRecord *record) {
    int32_t *value = context;
    assert(record->entry == 0x00c1ee14u);
    return *value;
}

int main(void) {
    FA18ScenePlacementRecord placement = {0};
    const FA18SceneDescriptorStageRecord record = {
        0x00c1ee14u, 0x00c35568u, 0x00c3558cu, 0x00c35adeu
    };
    FA18SceneDescriptorStageState state;
    int32_t result = 0x12345;
    assert(fa18_run_scene_descriptor_stage(&placement, &record, return_value,
                                           &result, &state) == 0);
    assert(state.context == 0x00c35568u && state.control_stream == 0x00c3558cu &&
           state.secondary_stream == 0x00c35adeu && state.stored_result == 0x2345 &&
           placement.per_frame[3] == 0x2345u);

    result = 0;
    assert(fa18_run_scene_descriptor_stage(&placement, &record, return_value,
                                           &result, &state) == 0);
    assert(state.stored_result == -1 && placement.per_frame[3] == 0xffffu);
    result = -7;
    assert(fa18_run_scene_descriptor_stage(&placement, &record, return_value,
                                           &result, &state) == 0);
    assert(placement.per_frame[3] == 0xffffu);
    assert(fa18_run_scene_descriptor_stage(0, &record, return_value, &result,
                                           &state) == -1);
    return 0;
}
