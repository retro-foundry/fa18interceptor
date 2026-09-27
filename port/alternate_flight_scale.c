#include "alternate_flight_scale.h"

static int32_t shift_left_8(int16_t value) {
    return (int32_t)((uint32_t)(int32_t)value << 8);
}

static int32_t shift_right_8(int32_t value) {
    if (value >= 0) return value >> 8;
    return (int32_t)-((-(int64_t)value + 255) >> 8);
}

int fa18_scale_alternate_flight_record(const FA18AlternateFlightScaleInput *input,
                                       FA18AlternateFlightScaleState *state,
                                       FA18AlternateFlightScaleRoute *route) {
    if (!input || !state || !route) return -1;
    if (input->header & 0x40u) {
        if (!input->alternate_accumulate ||
            input->alternate_accumulate(input->alternate_context, input->header,
                                        input->tuple, state->component_long) != 0)
            return -1;
    } else if (input->header & 0x10u) {
        FA18SceneComponentAccumulation accumulation;
        if (fa18_accumulate_scene_components(input->header, input->component_records,
                                             input->component_record_count,
                                             input->tuple[0], input->tuple[2],
                                             input->middle_bias, &accumulation) != 0)
            return -1;
        for (unsigned axis = 0; axis != 3; ++axis)
            state->component_long[axis] = accumulation.component[axis];
    } else {
        for (unsigned axis = 0; axis != 3; ++axis) {
            state->component_word[axis] = input->tuple[axis];
            state->component_long[axis] = shift_left_8(input->tuple[axis]);
        }
        state->component_ready = 0;
        *route = FA18_ALTERNATE_FLIGHT_SCALE_VALUE_GATE;
        return 0;
    }
    for (unsigned axis = 0; axis != 3; ++axis)
        state->component_word[axis] = (int16_t)shift_right_8(state->component_long[axis]);
    state->component_ready = 1;
    *route = FA18_ALTERNATE_FLIGHT_SCALE_FIXED_POINT;
    return 0;
}
