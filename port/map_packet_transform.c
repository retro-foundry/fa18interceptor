#include "map_packet_transform.h"

static int16_t add_word(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left + (uint16_t)right);
}

static int32_t add_long(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int32_t asr_long_8(int32_t value) {
    if (value >= 0) return value >> 8;
    return (int32_t)-((-(int64_t)value + 255) >> 8);
}

static int16_t asl_word(int16_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 16u) return 0;
    return (int16_t)((uint16_t)value << count);
}

static int16_t project_component(int16_t first, int16_t second,
                                 int16_t first_coefficient,
                                 int16_t second_coefficient,
                                 int16_t base, uint16_t shift) {
    const int32_t sum = add_long((int32_t)first * first_coefficient,
                                 (int32_t)second * second_coefficient);
    return asl_word(add_word((int16_t)(uint16_t)asr_long_8(sum), base), shift);
}

int fa18_transform_map_packet_pairs(const FA18MapPacketTransform *transform,
                                    const FA18MapPacketPair *pairs,
                                    size_t pair_count, int16_t count,
                                    FA18MapPacketProjectionRecord *output,
                                    size_t output_count,
                                    uint16_t *transformed_count) {
    if (!transform || !transformed_count ||
        (count > 0 && (!pairs || !output)))
        return -1;
    *transformed_count = 0;
    if (count <= 0) return 0;
    const size_t entries = (uint16_t)count;
    if (entries > pair_count || entries > output_count) return -1;

    const int16_t seed_high = (int16_t)(transform->packed_seed >> 16);
    const int16_t seed_low = (int16_t)transform->packed_seed;
    for (size_t index = 0; index != entries; ++index) {
        const int16_t first = add_word(seed_high, pairs[index].value[0]);
        const int16_t second = add_word(seed_low, pairs[index].value[1]);
        output[index].value[0] = project_component(first, second,
                                                   transform->matrix.value[0],
                                                   transform->matrix.value[2],
                                                   transform->output_base[0],
                                                   transform->detail_shift);
        output[index].value[1] = project_component(first, second,
                                                   transform->matrix.value[3],
                                                   transform->matrix.value[5],
                                                   transform->output_base[1],
                                                   transform->detail_shift);
        output[index].value[2] = project_component(first, second,
                                                   transform->matrix.value[6],
                                                   transform->matrix.value[8],
                                                   transform->output_base[2],
                                                   transform->detail_shift);
    }
    *transformed_count = (uint16_t)entries;
    return 0;
}
