#include "post_input_followup.h"

#include <assert.h>

typedef struct {
    FA18PostInputFollowupState *followup;
    FA18ViewportModeState *viewport_mode;
    unsigned calls;
} InitializerObservation;

static int observe_initializer(void *context) {
    InitializerObservation *observation = context;
    ++observation->calls;
    assert(observation->followup->command_mode == 7);
    assert(observation->followup->auxiliary == 9);
    assert(observation->followup->countdown == -1);
    assert(observation->viewport_mode->current == 4);
    assert(observation->viewport_mode->target == 6);
    return 0;
}

int main(void) {
    FA18PostInputFollowupState followup = {
        7, 9, 5, -1, FA18_POST_INPUT_CALLBACK_FINISH_FOLLOWUP
    };
    FA18ViewportModeState viewport_mode = { 4, 6, 2, 1 };
    InitializerObservation observation = { &followup, &viewport_mode, 0 };

    assert(fa18_finish_post_input_followup(&followup, &viewport_mode,
                                            observe_initializer, &observation) == 1);
    assert(observation.calls == 1);
    assert(followup.command_mode == 3 && followup.auxiliary == 0 &&
           followup.countdown == 2 &&
           followup.callback == FA18_POST_INPUT_CALLBACK_AFTER_FINISH_FOLLOWUP);
    assert(viewport_mode.current == 0 && viewport_mode.target == 15);

    followup.callback = FA18_POST_INPUT_CALLBACK_AFTER_FINISH_FOLLOWUP;
    followup.countdown = -1;
    followup.match_auxiliary = 1;
    assert(fa18_advance_post_input_followup_match(&followup, &viewport_mode) == 0);
    assert(!followup.match_auxiliary && followup.countdown == -1);
    viewport_mode.current = viewport_mode.target;
    assert(fa18_advance_post_input_followup_match(&followup, &viewport_mode) == 1);
    assert(followup.countdown == 2 &&
           followup.callback == FA18_POST_INPUT_CALLBACK_COMPLETE_FOLLOWUP);

    followup.countdown = 0;
    uint8_t event_flag = 1;
    assert(fa18_complete_post_input_followup(&followup, &event_flag) == 0);
    assert(event_flag == 1 &&
           followup.callback == FA18_POST_INPUT_CALLBACK_COMPLETE_FOLLOWUP);
    followup.countdown = -1;
    assert(fa18_complete_post_input_followup(&followup, &event_flag) == 1);
    assert(!event_flag && followup.match_auxiliary == 1 &&
           followup.callback == FA18_POST_INPUT_CALLBACK_CONTINUE_AFTER_COMPLETE_FOLLOWUP);

    followup.callback = FA18_POST_INPUT_CALLBACK_FINISH_FOLLOWUP;
    followup.countdown = 0;
    assert(fa18_finish_post_input_followup(&followup, &viewport_mode,
                                            observe_initializer, &observation) == 0);
    assert(observation.calls == 1);
    assert(fa18_advance_post_input_followup_match(&followup, &viewport_mode) == -1);
    assert(fa18_complete_post_input_followup(&followup, &event_flag) == -1);
    assert(fa18_finish_post_input_followup(NULL, &viewport_mode,
                                            observe_initializer, &observation) == -1);
    return 0;
}
