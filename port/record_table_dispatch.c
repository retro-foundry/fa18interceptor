#include "record_table_dispatch.h"

#include "hunk.h"

static int read_word_at(const FA18RecordTableDispatchInput *input,
                        uint32_t address, int16_t *word) {
    uint32_t offset;
    const uint64_t end = (uint64_t)input->stream_base + input->stream_size;
    if (!word || address < input->stream_base || (uint64_t)address >= end) return -1;
    offset = address - input->stream_base;
    if (offset > input->stream_size || input->stream_size - offset < 2u) return -1;
    *word = (int16_t)fa18_be16(input->stream_bytes + offset);
    return 0;
}

int fa18_dispatch_record_table_entry(const FA18RecordTableDispatchInput *input,
                                     FA18RecordTableDispatchResult *result,
                                     FA18RecordTableDispatchRoute *route) {
    int16_t control_word;
    int16_t source_status;

    if (!input || !result || !route || !input->stream_bytes) return -1;
    if (read_word_at(input, input->a2_cursor, &control_word) != 0) return -1;
    *result = (FA18RecordTableDispatchResult){input->a2_cursor + 2u,
                                               input->record_count, input->record_status,
                                               control_word, 0};
    if (control_word >= 0) {
        *route = FA18_RECORD_TABLE_DISPATCH_NEXT_CONTROL_EXTERNAL;
        return 0;
    }
    if (control_word == -1) {
        if (!input->error_handler || input->error_handler(input->context, control_word) != 0)
            return -1;
        *route = FA18_RECORD_TABLE_DISPATCH_CONTROL_EXTERNAL;
        return 0;
    }
    if ((uint16_t)control_word & UINT16_C(0x4000)) ++result->record_count;
    result->selector = (uint16_t)control_word & UINT16_C(0x3fff);
    if (!input->dispatch_handler) return -1;
    source_status = 0;
    if (input->dispatch_handler(input->context, result->selector, &source_status) != 0)
        return -1;
    if (source_status < 0) {
        *route = FA18_RECORD_TABLE_DISPATCH_RESTART_EXTERNAL;
        return 0;
    }
    result->record_status |= (uint16_t)source_status;
    *route = FA18_RECORD_TABLE_DISPATCH_CONTINUE_EXTERNAL;
    return 0;
}
