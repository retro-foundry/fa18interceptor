#include "record_triple_transform.h"

static int16_t word_add(int16_t left, int16_t right) {
    return (int16_t)((uint16_t)left + (uint16_t)right);
}

static int16_t word_asr(int16_t value, unsigned count) {
    uint16_t bits;
    count &= 63u; /* 68000 register shift count */
    if (count == 0) return value;
    if (count >= 16) return value < 0 ? -1 : 0;
    bits = (uint16_t)value >> count;
    if (value < 0) bits |= (uint16_t)(UINT16_MAX << (16u - count));
    return (int16_t)bits;
}

static int32_t long_add(int32_t left, int32_t right) {
    return (int32_t)((uint32_t)left + (uint32_t)right);
}

static int16_t transform_row(const int16_t *row, int16_t x, int16_t y, int16_t z) {
    int32_t sum = (int32_t)row[0] * x;
    sum = long_add(sum, (int32_t)row[1] * y);
    sum = long_add(sum, (int32_t)row[2] * z);
    /* The source uses `ASR.L #8`, then writes only Dn.w. */
    return (int16_t)(uint16_t)(sum >> 8);
}

int fa18_transform_record_triples(const FA18RecordTripleTransformInput *input) {
    if (!input || !input->source_words || !input->matrix_words ||
        !input->destination_words ||
        (size_t)input->triple_count * 3u > input->source_word_count ||
        (size_t)input->triple_count * 3u > input->destination_word_capacity)
        return -1;
    for (uint16_t index = 0; index < input->triple_count; ++index) {
        const size_t at = (size_t)index * 3u;
        const int16_t x = word_add(word_asr(input->source_words[at], input->source_shift),
                                   input->translate_x);
        const int16_t y = word_add(word_asr(input->source_words[at + 1u], input->source_shift),
                                   input->translate_y);
        const int16_t z = word_add(word_asr(input->source_words[at + 2u], input->source_shift),
                                   input->translate_z);
        input->destination_words[at] = transform_row(input->matrix_words, x, y, z);
        input->destination_words[at + 1u] = transform_row(input->matrix_words + 3u, x, y, z);
        input->destination_words[at + 2u] = transform_row(input->matrix_words + 6u, x, y, z);
    }
    return 0;
}
