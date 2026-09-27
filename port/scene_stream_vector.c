#include "scene_stream_vector.h"

static int32_t asr_long(int32_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 32u) return value < 0 ? -1 : 0;
    if (value >= 0) return value >> count;
    const int64_t magnitude = -(int64_t)value;
    return (int32_t)-((magnitude + ((INT64_C(1) << count) - 1)) >> count);
}

int fa18_prepare_scene_stream_vector(const FA18SceneStreamVectorInput *input,
                                     FA18SceneStreamVector *vector) {
    if (!input || !vector) return -1;
    for (unsigned index = 0; index != 3; ++index) {
        const int32_t shifted = asr_long(input->component[index], input->shift);
        const int32_t scaled = asr_long(shifted, 8);
        vector->shifted_component[index] = shifted;
        vector->scaled_component[index] = (int16_t)(uint16_t)scaled;
        vector->negated_scaled_component[index] =
            (int16_t)(UINT16_C(0) - (uint16_t)vector->scaled_component[index]);
    }
    return 0;
}
