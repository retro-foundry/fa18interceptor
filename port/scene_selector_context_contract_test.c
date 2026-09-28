#include "scene_selector_context.h"

#include <assert.h>

int main(void) {
    const uint8_t record[10] = {
        0, 0, 0, 0, 0, 0, 0xff, 0xf1, 0x00, 0x42
    };
    FA18SceneSelectorContextInput input = {
        0, record, sizeof record, 0, 0
    };
    FA18SceneSelectorContextState state = { 0 };

    assert(fa18_update_scene_selector_context(&input, &state) == 0);
    assert(state.first_selector == -4 && state.second_selector == 16 &&
           state.append_enabled == 1 && !state.first_status && !state.second_status);

    input.alternate_source_enabled = 1;
    input.alternate_first = (int32_t)UINT32_C(0x10100000);
    input.alternate_second = (int32_t)UINT32_C(0x1abc0000);
    state.first_status = 3;
    state.second_status = 7;
    assert(fa18_update_scene_selector_context(&input, &state) == 0);
    assert(state.first_selector == 16 && state.second_selector == 26 &&
           !state.first_status && !state.second_status);

    input.alternate_source_enabled = 0;
    input.active_record_size = sizeof record - 1;
    assert(fa18_update_scene_selector_context(&input, &state) == -1);
    assert(fa18_update_scene_selector_context(0, &state) == -1);
    assert(fa18_update_scene_selector_context(&input, 0) == -1);
    return 0;
}
