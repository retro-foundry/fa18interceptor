#include "scene_component_accumulation.h"

static int32_t arithmetic_shift_right_long(int32_t value, unsigned count) {
    if (!count) return value;
    if (value >= 0) return value >> count;
    const int64_t magnitude = -(int64_t)value;
    return (int32_t)-((magnitude + ((INT64_C(1) << count) - 1)) >> count);
}

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t shift_left_8_long(int32_t value) {
    return (int32_t)((uint32_t)value << 8);
}

int fa18_accumulate_scene_components(uint16_t descriptor_header,
                                     const FA18SceneComponentRecord *records,
                                     size_t record_count,
                                     int32_t input_x,
                                     int32_t input_z,
                                     int32_t middle_bias,
                                     FA18SceneComponentAccumulation *output) {
    if (!records || !output) return -1;
    const size_t index = descriptor_header >> 8;
    if (index >= record_count) return -1;
    const unsigned shift = descriptor_header & 0x0fu;
    const FA18SceneComponentRecord *record = &records[index];
    const int32_t x_source = (int32_t)(record->word_14 & UINT32_C(0x000fffff));
    const int32_t z_source = (int32_t)(record->word_1c & UINT32_C(0x000fffff));
    output->component[0] = add_long(shift_left_8_long(input_x),
                                    arithmetic_shift_right_long(x_source, shift));
    output->component[1] = arithmetic_shift_right_long(add_long(record->word_18,
                                                                  middle_bias),
                                                         shift);
    output->component[2] = add_long(shift_left_8_long(input_z),
                                    arithmetic_shift_right_long(z_source, shift));
    output->accumulated = 1;
    return 0;
}
