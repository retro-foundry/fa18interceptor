#include "scene_initialization.h"

static int call(FA18SceneInitializationCall function, void *context) {
    return function && function(context) == 0 ? 0 : -1;
}

int fa18_initialize_scene_state(FA18SceneInitializationState *state,
                                int16_t *countdown,
                                const FA18SceneInitializationOps *ops) {
    if (!state || !countdown || !ops || !ops->initialize_records || !ops->initialize_scene ||
        !ops->prepare_callback || !ops->finalize_scene)
        return -1;
    state->scene_latch_previous = state->scene_latch_current;
    state->scene_latch_current = 0;
    state->scene_stage = 4;
    --state->scene_stage;
    state->activity_mode = 0;
    state->update_flag = 0;
    state->scene_ready = 1;
    if (call(ops->initialize_records, ops->context) != 0) return -1;
    state->callback_mode = 3;
    if (call(ops->initialize_scene, ops->context) != 0) return -1;
    if (call(ops->prepare_callback, ops->context) != 0) return -1;
    state->scene_flag = 0;
    state->transition_auxiliary = 1;
    state->horizontal_offset = 0;
    state->vertical_offset = 0;
    if (call(ops->finalize_scene, ops->context) != 0) return -1;
    state->marker = 0xff;
    *countdown = 1;
    return 0;
}

int fa18_initialize_scene_callback(void *context) {
    FA18SceneInitializationContext *initializer = context;
    if (!initializer) return -1;
    return fa18_initialize_scene_state(initializer->state, initializer->countdown,
                                       initializer->ops);
}
