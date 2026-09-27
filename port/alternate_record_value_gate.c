#include "alternate_record_value_gate.h"

static int32_t shift_left_long(int16_t value, uint16_t count) {
    if (count >= 32u) return 0;
    return (int32_t)((uint32_t)(int32_t)value << count);
}

int fa18_gate_alternate_record_value(const FA18AlternateRecordValueGateInput *input,
                                     FA18AlternateRecordValueGateState *state) {
    int call_fixed = 0;
    if (!input || !state) return -1;
    *state = (FA18AlternateRecordValueGateState){0};
    for (unsigned axis = 0; axis != 3; ++axis)
        state->component_long[axis] = input->component_long[axis];
    const int32_t scaled = shift_left_long(input->record_value, input->header & 0x0fu);
    if (scaled < 0x400) {
        if (scaled < 0x100) {
            call_fixed = 1;
        } else if (input->header & 0x0100u) {
            call_fixed = (input->selector_control & 3u) == 2u;
        } else {
            call_fixed = (input->selector_control & 3u) == 0;
        }
    }
    state->record_value = input->record_value;
    if (call_fixed) {
        if (!input->fixed_point ||
            input->fixed_point(input->fixed_point_context, input->component_word,
                               &state->record_value) != 0)
            return -1;
        state->fixed_point_called = 1;
    }
    return 0;
}
