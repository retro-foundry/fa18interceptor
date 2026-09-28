#include "record_stream_selector.h"

#include "hunk.h"

static int offset_at_address(const FA18RecordStreamSelectorInput *input,
                             uint32_t address, uint32_t *offset) {
    const uint64_t end = (uint64_t)input->stream_base + input->stream_size;
    if (address < input->stream_base || (uint64_t)address >= end) return -1;
    *offset = address - input->stream_base;
    return 0;
}

static int read_word_at(const FA18RecordStreamSelectorInput *input,
                        uint32_t address, int16_t *word) {
    uint32_t offset;
    if (!word || offset_at_address(input, address, &offset) != 0 ||
        offset > input->stream_size || input->stream_size - offset < 2u)
        return -1;
    *word = (int16_t)fa18_be16(input->stream_bytes + offset);
    return 0;
}

static int read_long_at(const FA18RecordStreamSelectorInput *input,
                        uint32_t address, uint32_t *value) {
    uint32_t offset;
    if (!value || offset_at_address(input, address, &offset) != 0 ||
        offset > input->stream_size || input->stream_size - offset < 4u)
        return -1;
    *value = fa18_be32(input->stream_bytes + offset);
    return 0;
}

static uint32_t base_offset_address(const FA18RecordStreamSelectorInput *input,
                                    int16_t control_word) {
    return input->stream_base + ((uint16_t)control_word & UINT16_C(0x0fff));
}

int fa18_select_record_streams(const FA18RecordStreamSelectorInput *input,
                               FA18RecordStreamSelectorResult *result,
                               FA18RecordStreamSelectorRoute *route) {
    uint16_t control;
    int16_t a2_selector;

    if (!input || !result || !route || !input->stream_bytes) return -1;
    control = (uint16_t)input->control_word;
    *result = (FA18RecordStreamSelectorResult){input->control_cursor, 0, 0, 0, 0, 0, 0};
    if (input->control_word == -1) {
        *route = FA18_RECORD_STREAM_SELECTOR_RETURN_ZERO;
        return 0;
    }
    if (control & UINT16_C(0x2000)) {
        result->a2_count = 1;
        if (control & UINT16_C(0x1000)) {
            if (read_long_at(input, result->next_control_cursor,
                             &result->a2_address) != 0)
                return -1;
            result->next_control_cursor += 4u;
        } else {
            result->a2_address = base_offset_address(input, input->control_word);
        }
        *route = FA18_RECORD_STREAM_SELECTOR_DISPATCH;
        return 0;
    }
    if (control & UINT16_C(0x4000)) result->a1_count = 1;
    if (control & UINT16_C(0x1000)) {
        if (read_long_at(input, result->next_control_cursor, &result->a1_cursor) != 0)
            return -1;
        result->next_control_cursor += 4u;
    } else {
        result->a1_cursor = base_offset_address(input, input->control_word);
    }
    if (result->a1_count) {
        *route = FA18_RECORD_STREAM_SELECTOR_POST_STREAM_EXTERNAL;
        return 0;
    }
    if (read_word_at(input, result->a1_cursor, &a2_selector) != 0) return -1;
    result->a1_cursor += 2u;
    if ((uint16_t)a2_selector & UINT16_C(0x8000)) result->a1_flag = 1;
    if ((uint16_t)a2_selector & UINT16_C(0x1000)) {
        if (read_long_at(input, result->a1_cursor, &result->a2_address) != 0)
            return -1;
        result->a1_cursor += 4u;
    } else {
        result->a2_address = base_offset_address(input, a2_selector);
    }
    *route = FA18_RECORD_STREAM_SELECTOR_DISPATCH;
    return 0;
}
