#include "scene_initialization.h"

#include <assert.h>

typedef struct {
    FA18SceneInitializationState *state;
    unsigned calls;
} Log;

static int observe(void *context) {
    Log *log = context;
    ++log->calls;
    if (log->calls == 1) {
        assert(log->state->scene_latch_previous == 9 && !log->state->scene_latch_current);
        assert(log->state->scene_stage == 3 && !log->state->activity_mode &&
               !log->state->update_flag && log->state->scene_ready);
    } else if (log->calls == 2) {
        assert(log->state->callback_mode == 3);
    } else if (log->calls == 3) {
        assert(log->state->callback_mode == 3 && log->state->scene_ready);
    } else if (log->calls == 4) {
        assert(!log->state->scene_flag && log->state->transition_auxiliary &&
               !log->state->horizontal_offset && !log->state->vertical_offset);
    }
    return 0;
}

int main(void) {
    FA18SceneInitializationState state = { .scene_latch_current = 9, .scene_flag = 1,
                                            .horizontal_offset = 7, .vertical_offset = 8 };
    int16_t countdown = -1;
    Log log = { &state, 0 };
    FA18SceneInitializationOps ops = { observe, observe, observe, observe, &log };
    FA18SceneInitializationContext context = { &state, &countdown, &ops };
    assert(fa18_initialize_scene_callback(&context) == 0);
    assert(log.calls == 4 && state.marker == 0xff && countdown == 1);
    assert(fa18_initialize_scene_callback(NULL) == -1);
    return 0;
}
