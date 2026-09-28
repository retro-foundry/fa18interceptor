#include "c1f99a_record_transform.h"

#include "hunk.h"

static int16_t word_from_bits(uint16_t bits) {
    return (int16_t)bits;
}

static int16_t add_word(int16_t left, int16_t right) {
    return word_from_bits((uint16_t)((uint16_t)left + (uint16_t)right));
}

static int16_t asr_word(int16_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 16u) return value < 0 ? -1 : 0;
    if (value >= 0) return (int16_t)(value >> count);
    return (int16_t)-((-(int32_t)value + ((1 << count) - 1)) >> count);
}

static int32_t asr_long(int32_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 32u) return value < 0 ? -1 : 0;
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + ((INT64_C(1) << count) - 1)) >> count);
}

static int16_t matrix_component(const int16_t vector[3], const int16_t row[3]) {
    uint32_t sum = 0;
    for (size_t index = 0; index < 3u; ++index)
        sum += (uint32_t)((int32_t)vector[index] * row[index]);
    return (int16_t)(uint16_t)asr_long((int32_t)sum, 8u);
}

static int transform_vector(const FA18TransformMatrix *matrix, const int16_t input[3],
                            int16_t output[3]) {
    if (!matrix || !input || !output) return -1;
    for (size_t row = 0; row < 3u; ++row)
        output[row] = matrix_component(input, matrix->value[row]);
    return 0;
}

int fa18_transform_c1f99a_record(const FA18C1F99ARecordTransformInput *input) {
    int16_t translation[3];
    int64_t source_start, destination_start;
    if (!input || !input->descriptor_bytes || !input->workspace_bytes ||
        input->descriptor_cursor > input->descriptor_size ||
        input->descriptor_size - input->descriptor_cursor < 10u)
        return -1;
    /* `BTST #0,7(A1)` falls through only to `$C1F9B8`; the other route enters
     * `$C1FA92` and its unported `$C1FB24` continuation. */
    if (!(input->descriptor_bytes[input->descriptor_cursor + 7u] & 1u)) return -1;
    source_start = (int64_t)input->descriptor_cursor + 10 + input->descriptor_offset;
    destination_start = input->descriptor_offset;
    if (source_start < 0 || destination_start < 0 ||
        (uint64_t)source_start > input->descriptor_size ||
        (uint64_t)destination_start > input->workspace_size ||
        input->record_count > (input->descriptor_size - (size_t)source_start) / 6u ||
        input->record_count > (input->workspace_size - (size_t)destination_start) / 6u)
        return -1;

    /* `$C1F9CA-$C1F9EC`: D1/D2 exchange means the later coordinate additions
     * use prepared[0], prepared[2], prepared[1] against stream[0..2]. */
    translation[0] = (int16_t)(uint16_t)asr_long(
        (int32_t)((uint32_t)input->prepared_component[0] + input->stream_component[0]),
        8u - input->frame_shift);
    translation[1] = (int16_t)(uint16_t)asr_long(
        (int32_t)((uint32_t)input->prepared_component[2] + input->stream_component[1]),
        8u - input->frame_shift);
    translation[2] = (int16_t)(uint16_t)asr_long(
        (int32_t)((uint32_t)input->prepared_component[1] + input->stream_component[2]),
        8u - input->frame_shift);

    for (uint16_t index = 0; index < input->record_count; ++index) {
        const uint8_t *source = input->descriptor_bytes + (size_t)source_start + index * 6u;
        uint8_t *destination = input->workspace_bytes + (size_t)destination_start + index * 6u;
        int16_t local[3], first[3], translated[3], output[3];
        for (size_t component = 0; component < 3u; ++component)
            local[component] = asr_word((int16_t)fa18_be16(source + component * 2u),
                                        input->record_shift);
        if (transform_vector(&input->first_matrix, local, first) != 0) return -1;
        for (size_t component = 0; component < 3u; ++component)
            translated[component] = add_word(first[component], translation[component]);
        if (transform_vector(&input->second_matrix, translated, output) != 0) return -1;
        for (size_t component = 0; component < 3u; ++component) {
            destination[component * 2u] = (uint8_t)((uint16_t)output[component] >> 8);
            destination[component * 2u + 1u] = (uint8_t)output[component];
        }
    }
    return 0;
}

int fa18_transform_c1f99a_record_callback(void *context, int16_t count,
                                          int16_t descriptor_offset) {
    FA18C1F99ARecordTransformInput input;
    if (!context || count < 0) return -1;
    input = *(const FA18C1F99ARecordTransformInput *)context;
    input.record_count = (uint16_t)count;
    input.descriptor_offset = descriptor_offset;
    return fa18_transform_c1f99a_record(&input);
}
