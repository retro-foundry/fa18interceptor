#include "scene_finalization.h"

int fa18_finalize_scene_state(FA18SceneFinalizationState *state) {
    if (!state) return -1;
    state->stage_word = 0x0090;
    for (unsigned index = 0; index < FA18_SCENE_FINALIZATION_CONTROL_COUNT; ++index)
        state->controls[index] = 3;
    if (!state->guard) {
        state->guarded_word = 0;
        state->guarded_long = 0;
    }
    return 0;
}

int fa18_finalize_scene_callback(void *context) {
    return fa18_finalize_scene_state(context);
}
