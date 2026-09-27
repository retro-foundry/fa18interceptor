#include "record_delta_scan.h"

static int32_t sub_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left - (uint32_t)right);
}

static int32_t abs_long_68000(int32_t value) {
    return value < 0 ? (int32_t)(UINT32_C(0) - (uint32_t)value) : value;
}

static int32_t arithmetic_shift_right_long(int32_t value, unsigned count) {
    if (!count) return value;
    if (count >= 32u) return value < 0 ? -1 : 0;
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + ((INT64_C(1) << count) - 1)) >> count);
}

static int32_t smaller_largest_component(const int32_t component[3]) {
    const int32_t x = abs_long_68000(component[0]);
    const int32_t y = abs_long_68000(component[1]);
    const int32_t z = abs_long_68000(component[2]);
    if (x < y) return x > z ? z : x;
    return y > z ? z : y;
}

int fa18_scan_record_delta_submission(const FA18RecordDeltaScanInput *input,
                                      FA18RecordDeltaScanState *state) {
    size_t selected;
    if (!input || !state || !input->records || !input->submit ||
        (input->selected_offset & 0x01ffu))
        return -1;
    selected = input->selected_offset >> 9;
    if (selected >= FA18_RECORD_DELTA_SCAN_RECORDS) return -1;

    *state = (FA18RecordDeltaScanState){0};
    if (input->scene_active) return 0;
    if (input->activity > 1) {
        ++state->mode;
    } else {
        if (!(input->selector & 2u)) ++state->mode;
        if (input->selector & 3u) return 0;
    }

    for (size_t index = 0; index != FA18_RECORD_DELTA_SCAN_RECORDS; ++index) {
        int32_t component[3];
        int32_t bound;
        unsigned shift = 0;
        if (index == selected || !(input->records[index].flags_byte_1 & 0x40u))
            continue;
        for (unsigned axis = 0; axis != 3; ++axis)
            component[axis] = sub_long(input->records[index].component[axis],
                                       input->records[selected].component[axis]);
        bound = smaller_largest_component(component);
        while (bound > INT32_C(0x007fffff)) {
            shift += 2;
            bound = arithmetic_shift_right_long(bound, 2);
        }
        shift += 8;
        for (unsigned axis = 0; axis != 3; ++axis)
            component[axis] = arithmetic_shift_right_long(component[axis], shift);
        if (input->submit(input->submit_context,
                          (uint16_t)((index << 8) | 0x10u), component[0],
                          component[1], component[2]) != 0)
            return -1;
        ++state->submitted_count;
    }
    return 0;
}
