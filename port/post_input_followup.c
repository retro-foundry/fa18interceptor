#include "post_input_followup.h"

int fa18_finish_post_input_followup(FA18PostInputFollowupState *followup,
                                    FA18ViewportModeState *viewport_mode,
                                    FA18SceneInitializer scene_initializer,
                                    void *context) {
    if (!followup || !viewport_mode || !scene_initializer ||
        followup->callback != FA18_POST_INPUT_CALLBACK_FINISH_FOLLOWUP)
        return -1;
    if (followup->countdown >= 0) return 0;

    if (scene_initializer(context) != 0) return -1;
    followup->command_mode = 3;
    followup->auxiliary = 0;
    followup->countdown = 2;
    viewport_mode->target = 15;
    viewport_mode->current = 0;
    followup->callback = FA18_POST_INPUT_CALLBACK_AFTER_FINISH_FOLLOWUP;
    return 1;
}

int fa18_advance_post_input_followup_match(FA18PostInputFollowupState *followup,
                                           const FA18ViewportModeState *viewport_mode) {
    if (!followup || !viewport_mode ||
        followup->callback != FA18_POST_INPUT_CALLBACK_AFTER_FINISH_FOLLOWUP)
        return -1;
    if (followup->countdown >= 0) return 0;

    followup->match_auxiliary = 0;
    if (viewport_mode->current != viewport_mode->target) return 0;
    followup->countdown = 2;
    followup->callback = FA18_POST_INPUT_CALLBACK_COMPLETE_FOLLOWUP;
    return 1;
}
