#include "display_record_candidates.h"

#include <stdint.h>

static int16_t word_asr2(int16_t value) {
    if (value >= 0) return (int16_t)(value >> 2);
    return (int16_t)-(((-(int32_t)value) + 3) >> 2);
}

static int32_t long_from_bits(uint32_t value) {
    if (value <= INT32_MAX) return (int32_t)value;
    return (int32_t)((int64_t)value - (INT64_C(1) << 32));
}

/* `$C0D790/$C0D7A8/$C0D7BA`: ASR.L #8 followed by BCC/ADDQ. */
static int16_t rounded_asr8(uint32_t bits) {
    int64_t value = long_from_bits(bits);
    int64_t quotient = value >= 0 ? value / 256 : -(((-value) + 255) / 256);

    if (bits & UINT32_C(0x80)) ++quotient;
    return (int16_t)quotient;
}

static int16_t transform_row(int16_t x, int16_t source, int16_t y,
                             const int16_t row[3]) {
    uint32_t sum = (uint32_t)((int32_t)x * row[0]);
    sum += (uint32_t)((int32_t)source * row[1]);
    sum += (uint32_t)((int32_t)y * row[2]);
    return rounded_asr8(sum);
}

int fa18_prepare_display_record_candidates(
    const int16_t input_pairs[FA18_DISPLAY_RECORD_CANDIDATE_COUNT][2],
    int16_t source_component, const int16_t matrix[3][3],
    FA18DisplayRecordCandidate output[FA18_DISPLAY_RECORD_CANDIDATE_COUNT]) {
    int16_t shifted_source;

    if (!input_pairs || !matrix || !output) return -1;
    shifted_source = word_asr2(source_component);
    for (uint16_t index = 0; index < FA18_DISPLAY_RECORD_CANDIDATE_COUNT;
         ++index) {
        output[index].x = transform_row(input_pairs[index][0], shifted_source,
                                        input_pairs[index][1], matrix[0]);
        output[index].y = transform_row(input_pairs[index][0], shifted_source,
                                        input_pairs[index][1], matrix[1]);
        output[index].depth = transform_row(input_pairs[index][0], shifted_source,
                                            input_pairs[index][1], matrix[2]);
    }
    return 0;
}
