#ifndef FA18_POST_INPUT_TRANSITION_H
#define FA18_POST_INPUT_TRANSITION_H

#include <stdint.h>

/* Direct mutable state at `$C10C08-$C10C66`.  The two continuation targets
 * remain typed routes because their bodies are separate source owners. */
typedef struct {
    uint8_t flight_mode;
    int8_t event_flag;
    uint8_t controller_enable;
    uint8_t selector_a;
    uint8_t selector_b;
    uint8_t selector_c;
    uint8_t command_selector;
    int16_t countdown;
} FA18PostInputTransitionState;

typedef enum {
    FA18_POST_INPUT_TRANSITION_C10C68,
    FA18_POST_INPUT_TRANSITION_C11A26,
    FA18_POST_INPUT_TRANSITION_RETURN
} FA18PostInputTransitionRoute;

/* `$C10C08-$C10C66`: initialize the next post-input controller only on the
 * exact source branches.  It does not execute the selected continuation. */
int fa18_begin_post_input_transition(FA18PostInputTransitionState *state,
                                     FA18PostInputTransitionRoute *route);

#endif
