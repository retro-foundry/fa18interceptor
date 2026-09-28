#include "extended_record_dispatch.h"

#include "hunk.h"

static int read_word(const FA18ExtendedRecordDispatchInput *input,
                     uint32_t address, int16_t *word) {
    const uint64_t end = (uint64_t)input->stream_base + input->stream_size;
    uint32_t offset;
    if (!word || address < input->stream_base || (uint64_t)address >= end) return -1;
    offset = address - input->stream_base;
    if (offset > input->stream_size || input->stream_size - offset < 2u) return -1;
    *word = (int16_t)fa18_be16(input->stream_bytes + offset);
    return 0;
}

static int16_t asr_word(int16_t value, unsigned count) {
    count &= 31u;
    if (!count) return value;
    if (value >= 0) return (int16_t)(value >> count);
    return (int16_t)-((-(int32_t)value + ((1 << count) - 1)) >> count);
}

int fa18_dispatch_extended_record(const FA18ExtendedRecordDispatchInput *input,
                                  FA18ExtendedRecordDispatchResult *result,
                                  FA18ExtendedRecordDispatchRoute *route) {
    int16_t control;
    uint32_t cursor;
    if (!input || !result || !route || !input->stream_bytes) return -1;
    *result = (FA18ExtendedRecordDispatchResult){input->a2_cursor, 0, 0, 0};
    control = input->initial_word;
    cursor = input->a2_cursor;
    while (control != INT16_MAX) {
        int16_t next;
        if (asr_word(control, input->stage_shift) < input->selector_limit) {
            if (read_word(input, cursor, &next) != 0) return -1;
            cursor += 2u;
            if (next < 0) {
                result->next_a2_cursor = cursor;
                *route = FA18_EXTENDED_RECORD_DISPATCH_CONTROL_EXTERNAL;
                return 0;
            }
            control = next;
            continue;
        }
        if (read_word(input, cursor, &next) != 0) return -1;
        cursor += 2u;
        if (next >= 0) {
            result->next_a2_cursor = cursor;
            *route = FA18_EXTENDED_RECORD_DISPATCH_CONTROL_EXTERNAL;
            return 0;
        }
        break;
    }
    {
        int16_t count, selector_word, source_status;
        if (read_word(input, cursor, &count) != 0) return -1;
        cursor += 2u;
        if (count > 0) {
            int16_t descriptor_offset;
            if (!input->transform || read_word(input, cursor, &descriptor_offset) != 0 ||
                input->transform(input->context, count, descriptor_offset) != 0)
                return -1;
            cursor += 2u;
        }
        if (read_word(input, cursor, &selector_word) != 0) return -1;
        cursor += 2u;
        if ((uint16_t)selector_word & UINT16_C(0x4000)) ++result->record_count;
        result->selector = (uint16_t)selector_word & UINT16_C(0x3fff);
        if (!input->target || input->target(input->context, result->selector,
                                            &source_status) != 0)
            return -1;
        result->next_a2_cursor = cursor;
        if (source_status < 0) {
            *route = FA18_EXTENDED_RECORD_DISPATCH_RESTART;
            return 0;
        }
        result->record_status = (uint16_t)source_status;
        *route = FA18_EXTENDED_RECORD_DISPATCH_CONTINUE;
        return 0;
    }
}
