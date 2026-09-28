#include "post_input_transition.h"

#include <assert.h>

int main(void) {
    FA18PostInputTransitionState state = {0, -1, 9, 8, 7, 6, 5, -1};
    FA18PostInputTransitionRoute route;

    assert(fa18_begin_post_input_transition(&state, &route) == 0);
    assert(route == FA18_POST_INPUT_TRANSITION_C10C68);
    assert(state.controller_enable == 1 && state.selector_a == 0 &&
           state.selector_b == 0 && state.selector_c == 0 && state.countdown == 5);

    state = (FA18PostInputTransitionState){1, -1, 9, 8, 7, 6, 5, -1};
    assert(fa18_begin_post_input_transition(&state, &route) == 0);
    assert(route == FA18_POST_INPUT_TRANSITION_C11A26);
    assert(state.countdown == 2 && state.command_selector == 10 &&
           state.controller_enable == 9 && state.selector_a == 8);

    state = (FA18PostInputTransitionState){1, 0, 9, 8, 7, 6, 5, -1};
    assert(fa18_begin_post_input_transition(&state, &route) == 0);
    assert(route == FA18_POST_INPUT_TRANSITION_RETURN && state.countdown == -1);
    assert(fa18_begin_post_input_transition(0, &route) == -1);
    assert(fa18_begin_post_input_transition(&state, 0) == -1);
    return 0;
}
