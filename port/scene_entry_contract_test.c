#include "message_sequence.h"
#include "post_input_followup.h"
#include "scene_finalization.h"
#include "scene_initialization.h"

#include <assert.h>

typedef struct {
    FA18SceneInitializationState *state;
    const FA18PostInputFollowupState *followup;
    FA18MessageSequenceState *message;
    FA18SceneFinalizationState *finalization;
    unsigned calls;
} Log;

static int observe(void *context) {
    Log *log = context;
    ++log->calls;
    if (log->calls == 1) {
        assert(log->followup->countdown == -1);
        assert(log->state->scene_latch_previous == 9 && !log->state->scene_latch_current);
    }
    return 0;
}

static int initialize_message(void *context) {
    Log *log = context;
    ++log->calls;
    return fa18_initialize_message_sequence(log->message);
}

static int finalize_scene(void *context) {
    Log *log = context;
    ++log->calls;
    return fa18_finalize_scene_state(log->finalization);
}

int main(void) {
    FA18SceneInitializationState scene = { .scene_latch_current = 9 };
    FA18PostInputFollowupState followup = {
        .auxiliary = 1,
        .countdown = -1,
        .callback = FA18_POST_INPUT_CALLBACK_FINISH_FOLLOWUP
    };
    FA18ViewportModeState viewport = { .current = 7, .target = 3 };
    FA18MessageSequenceState message = { .selector_head = { 1, 2 }, .delay = 3,
                                          .cursor = 4, .active = 5,
                                          .effect_counter = 6, .inhibit = 7 };
    FA18SceneFinalizationState finalization = {
        .guarded_word = 0xffff, .guarded_long = 0xffffffffu
    };
    Log log = { &scene, &followup, &message, &finalization, 0 };
    FA18SceneInitializationOps ops = {
        observe, observe, initialize_message, finalize_scene, &log
    };
    FA18SceneInitializationContext initializer = { &scene, &followup.countdown, &ops };

    assert(fa18_finish_post_input_followup(&followup, &viewport,
                                            fa18_initialize_scene_callback,
                                            &initializer) == 1);
    assert(log.calls == 4);
    assert(scene.marker == 0xff && scene.scene_stage == 3 && scene.scene_ready);
    assert(!message.selector_head[0] && !message.selector_head[1] &&
           message.delay == 0x01b8 && !message.cursor && !message.active &&
           !message.effect_counter && !message.inhibit);
    assert(finalization.stage_word == 0x0090 && !finalization.guarded_word &&
           !finalization.guarded_long);
    assert(followup.command_mode == 3 && !followup.auxiliary && followup.countdown == 2);
    assert(viewport.current == 0 && viewport.target == 15);
    assert(followup.callback == FA18_POST_INPUT_CALLBACK_AFTER_FINISH_FOLLOWUP);
    return 0;
}
