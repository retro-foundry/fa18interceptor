#include "flagged_record_component_bound.h"

#include "hunk.h"

static int16_t asr(int16_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 16u) return value < 0 ? -1 : 0;
    return value < 0 ? (int16_t)-((-(int32_t)value + ((1 << count) - 1)) >> count)
                     : (int16_t)(value >> count);
}

int fa18_test_flagged_record_component_bound(const FA18FlaggedRecordComponentBoundInput *input,
                                             FA18FlaggedRecordComponentBoundResult *result) {
    uint16_t selector;
    int16_t component, bound_word = 0;
    int32_t adjusted, bound;
    uint16_t offset;
    if (!input || !result || !input->record_bytes) return -1;
    selector = ((uint16_t)input->input_d7 >> 10) & 3u;
    offset = selector == 1u ? 12u : selector == 2u ? 10u : 14u;
    if (input->record_offset < 0 || (uint32_t)input->record_offset > input->record_size ||
        input->record_size - (uint32_t)input->record_offset < offset + 2u ||
        input->record_size < 7u) return -1;
    component = asr((int16_t)fa18_be16(input->record_bytes + input->record_offset + offset),
                    input->record_bytes[6] & 15u);
    if (selector == 1u) {
        adjusted = component;
        bound = (int32_t)(UINT32_C(0) - (uint32_t)input->bound_long);
    } else {
        if (selector == 2u) {
            component = (int16_t)((uint16_t)component +
                                  ((uint16_t)input->component_x << (input->stream_stage_shift & 15u)));
            bound_word = (int16_t)(-(int32_t)input->bound_x);
        } else {
            component = (int16_t)((uint16_t)component +
                                  ((uint16_t)input->component_z << (input->stream_stage_shift & 15u)));
            bound_word = (int16_t)(-(int32_t)input->bound_z);
        }
        adjusted = component;
        bound = bound_word;
    }
    { const uint8_t below = bound < adjusted;
      const uint8_t sense = ((uint16_t)input->input_d7 & UINT16_C(0x1000)) != 0;
      result->zero = below == sense;
      result->d7 = below && !sense ? 1u : (below ? ((uint32_t)(uint16_t)input->input_d7 & UINT32_C(0xffff0000)) : (uint32_t)bound);
    }
    return 0;
}
