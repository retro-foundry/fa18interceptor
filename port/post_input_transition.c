#include "post_input_transition.h"

int fa18_begin_post_input_transition(FA18PostInputTransitionState *state,
                                     FA18PostInputTransitionRoute *route) {
    if (!state || !route) return -1;
    if (!state->flight_mode) {
        state->controller_enable = 1;
        state->selector_a = 0;
        state->selector_b = 0;
        state->selector_c = 0;
        state->countdown = 5;
        *route = FA18_POST_INPUT_TRANSITION_C10C68;
        return 0;
    }
    if (state->event_flag < 0) {
        state->countdown = 2;
        state->command_selector = 10;
        *route = FA18_POST_INPUT_TRANSITION_C11A26;
        return 0;
    }
    *route = FA18_POST_INPUT_TRANSITION_RETURN;
    return 0;
}
