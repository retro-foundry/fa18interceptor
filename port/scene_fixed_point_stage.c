#include "scene_fixed_point_stage.h"

#include <limits.h>

static int16_t asr_word(int16_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 16u) return value < 0 ? -1 : 0;
    if (value >= 0) return (int16_t)((uint16_t)value >> count);
    const int32_t magnitude = -(int32_t)value;
    return (int16_t)-((magnitude + ((1 << count) - 1)) >> count);
}

static int32_t asr_long(int32_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 32u) return value < 0 ? -1 : 0;
    if (value >= 0) return value >> count;
    const int64_t magnitude = -(int64_t)value;
    return (int32_t)-((magnitude + ((INT64_C(1) << count) - 1)) >> count);
}

static int16_t add_word(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left + (uint16_t)right);
}

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int16_t abs_word_68000(int16_t value) {
    return value < 0 ? (int16_t)(UINT16_C(0) - (uint16_t)value) : value;
}

static int32_t abs_long_68000(int32_t value) {
    return value < 0 ? (int32_t)(UINT32_C(0) - (uint32_t)value) : value;
}

int fa18_run_scene_fixed_point_stage(const FA18SceneFixedPointInput *input,
                                     const FA18SceneMagnitudeTable *table,
                                     int16_t *result,
                                     FA18SceneFixedPointRoute *route) {
    if (!input || !table || !result || !route) return -1;
    const unsigned shift = input->variable_shift;
    int16_t d2 = abs_word_68000(add_word(input->component_0,
                                         asr_word(input->offset_word, shift)));
    int32_t d3 = input->alternate_long_mode
                     ? asr_long(input->alternate_long, 8)
                     : add_long((int32_t)(int16_t)input->component_1,
                                asr_long(input->offset_long, shift));
    d3 = abs_long_68000(d3);
    if (d3 >= INT32_C(0x0007fff0)) {
        *route = FA18_SCENE_FIXED_POINT_C1D90A_BOUNDARY;
        return 0;
    }
    int16_t d4 = abs_word_68000(add_word(input->component_2,
                                         asr_word(input->offset_second_word,
                                                  shift)));
    d2 = asr_word(d2, 4);
    d3 = asr_long(d3, 4);
    d4 = asr_word(d4, 4);
    if (fa18_scene_component_magnitude(table, d2, (int16_t)d3, d4, result) != 0)
        return -1;
    *route = FA18_SCENE_FIXED_POINT_MAGNITUDE;
    return 0;
}
