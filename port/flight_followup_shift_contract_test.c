#include "flight_followup_shift.h"

#include <assert.h>

static int lookup(void *context, int16_t index, int8_t *shift) {
    int16_t *seen = context;
    *seen = index;
    *shift = 4;
    return 0;
}
int main(void) {
    int16_t seen = -1;
    FA18FlightFollowupShiftInput input = {0x10000, -0x20000, 7, 0x8000, 0x8000};
    FA18FlightFollowupShiftState state;
    assert(fa18_shift_flight_followup_components(&input, lookup, &seen, &state) == 0);
    assert(seen == 7 && state.shift_count == 4 && state.shifted_depth == 0x1000 &&
           state.shifted_components[0] == 0x1000 && state.shifted_components[1] == 0x800 &&
           state.shifted_components[2] == -0x2000 && state.component_words[0] == 16 &&
           state.component_words[1] == 8 && state.component_words[2] == -32 && state.ready_flag);
    assert(fa18_shift_flight_followup_components(&input, 0, &seen, &state) == -1);
    return 0;
}
