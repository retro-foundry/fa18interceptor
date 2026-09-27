#include "scene_stream_pair_transform.h"

static int16_t add_word(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left + (uint16_t)right);
}

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int16_t asr_word(int16_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 16u) return value < 0 ? -1 : 0;
    if (value >= 0) return (int16_t)((uint16_t)value >> count);
    const int32_t magnitude = -(int32_t)value;
    return (int16_t)-((magnitude + ((1 << count) - 1)) >> count);
}

static int32_t asr_long_8(int32_t value) {
    if (value >= 0) return value >> 8;
    return (int32_t)-((-(int64_t)value + 255) >> 8);
}

static int16_t scaled_pair_component(int16_t first, int16_t second,
                                     int16_t first_coefficient,
                                     int16_t second_coefficient,
                                     int16_t base) {
    const int32_t products = add_long((int32_t)first * first_coefficient,
                                      (int32_t)second * second_coefficient);
    return add_word((int16_t)(uint16_t)asr_long_8(products), base);
}

int fa18_transform_scene_stream_pairs(
    const FA18SceneStreamPairTransform *transform,
    const FA18SceneStreamPair *pairs, size_t pair_count, int16_t count,
    FA18SceneStreamPairOutput *output, size_t output_count,
    uint16_t *transformed_count) {
    if (!transform || !transformed_count ||
        (count > 0 && (!pairs || !output)))
        return -1;
    *transformed_count = 0;
    if (count <= 0) return 0;
    const size_t entries = (uint16_t)count;
    if (entries > pair_count || entries > output_count) return -1;

    for (size_t index = 0; index != entries; ++index) {
        const int16_t first = add_word(asr_word(pairs[index].value[0], transform->shift),
                                       transform->pair_offset[0]);
        const int16_t second = add_word(asr_word(pairs[index].value[1], transform->shift),
                                        transform->pair_offset[1]);
        output[index].value[0] = scaled_pair_component(first, second,
                                                        transform->matrix.value[0],
                                                        transform->matrix.value[2],
                                                        transform->output_base[0]);
        output[index].value[1] = scaled_pair_component(first, second,
                                                        transform->matrix.value[3],
                                                        transform->matrix.value[5],
                                                        transform->output_base[1]);
        output[index].value[2] = scaled_pair_component(first, second,
                                                        transform->matrix.value[6],
                                                        transform->matrix.value[8],
                                                        transform->output_base[2]);
    }
    *transformed_count = (uint16_t)entries;
    return 0;
}
