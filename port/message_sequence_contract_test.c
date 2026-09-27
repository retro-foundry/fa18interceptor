#include "message_sequence.h"

#include <assert.h>

int main(void) {
    FA18MessageSequenceState state = {
        .selector_head = { 1, 2 }, .delay = 3, .cursor = 4,
        .active = 5, .effect_counter = 6, .inhibit = 7
    };
    assert(fa18_initialize_message_sequence_callback(&state) == 0);
    assert(!state.selector_head[0] && !state.selector_head[1] &&
           state.delay == 0x01b8 && !state.cursor && !state.active &&
           !state.effect_counter && !state.inhibit);
    assert(fa18_initialize_message_sequence(NULL) == -1);
    return 0;
}
