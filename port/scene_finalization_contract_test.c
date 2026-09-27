#include "scene_finalization.h"

#include <assert.h>

int main(void) {
    FA18SceneFinalizationState state = {
        .guarded_word = 0xffff, .guarded_long = 0xffffffffu
    };
    assert(fa18_finalize_scene_callback(&state) == 0);
    assert(state.stage_word == 0x0090 && !state.guarded_word && !state.guarded_long);
    for (unsigned index = 0; index < FA18_SCENE_FINALIZATION_CONTROL_COUNT; ++index)
        assert(state.controls[index] == 3);

    state.guard = 1;
    state.guarded_word = 0x1357;
    state.guarded_long = 0x2468ace0u;
    assert(fa18_finalize_scene_state(&state) == 0);
    assert(state.guarded_word == 0x1357 && state.guarded_long == 0x2468ace0u);
    assert(fa18_finalize_scene_state(NULL) == -1);
    return 0;
}
