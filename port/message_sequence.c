#include "message_sequence.h"

int fa18_initialize_message_sequence(FA18MessageSequenceState *state) {
    if (!state) return -1;
    state->selector_head[0] = 0;
    state->selector_head[1] = 0;
    state->delay = 0x01b8;
    state->cursor = 0;
    state->active = 0;
    state->effect_counter = 0;
    state->inhibit = 0;
    return 0;
}

int fa18_initialize_message_sequence_callback(void *context) {
    return fa18_initialize_message_sequence(context);
}
