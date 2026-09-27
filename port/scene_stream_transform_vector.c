#include "scene_stream_transform_vector.h"

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t asr_long(int32_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 32u) return value < 0 ? -1 : 0;
    if (value >= 0) return value >> count;
    const int64_t magnitude = -(int64_t)value;
    return (int32_t)-((magnitude + ((INT64_C(1) << count) - 1)) >> count);
}

int fa18_prepare_scene_stream_transform_vector(
    const FA18SceneStreamTransformVectorInput *input,
    FA18SceneStreamTransformVector *vector) {
    if (!input || !vector || input->descriptor_low_nibble > 0x0fu) return -1;
    vector->shift_count = (uint16_t)(8u - input->descriptor_low_nibble);
    const int32_t x = asr_long(add_long(input->prepared_component[0], input->placement_x),
                               vector->shift_count);
    const int32_t y = asr_long(input->placement_z, vector->shift_count);
    const int32_t z = asr_long(add_long(input->prepared_component[2], input->placement_y),
                               vector->shift_count);
    vector->component[0] = (int16_t)(uint16_t)x;
    vector->component[1] = (int16_t)(uint16_t)y;
    vector->component[2] = (int16_t)(uint16_t)z;
    return 0;
}
