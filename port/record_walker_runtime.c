#include "record_walker_runtime.h"

#include "hunk.h"

static int stream_offset(const FA18RecordWalkerRuntimeInput *input,
                         uint32_t address, uint32_t *offset) {
    const uint64_t end = (uint64_t)input->stream_base + input->stream_size;
    if (!offset || address < input->stream_base || (uint64_t)address >= end) return -1;
    *offset = address - input->stream_base;
    return 0;
}

static int read_stream_word(const FA18RecordWalkerRuntimeInput *input,
                            uint32_t address, int16_t *word) {
    uint32_t offset;
    if (!word || stream_offset(input, address, &offset) != 0 ||
        offset > input->stream_size || input->stream_size - offset < 2u)
        return -1;
    *word = (int16_t)fa18_be16(input->stream_bytes + offset);
    return 0;
}

static int read_stream_long(const FA18RecordWalkerRuntimeInput *input,
                            uint32_t address, uint32_t *value) {
    uint32_t offset;
    if (!value || stream_offset(input, address, &offset) != 0 ||
        offset > input->stream_size || input->stream_size - offset < 4u)
        return -1;
    *value = fa18_be32(input->stream_bytes + offset);
    return 0;
}

static int read_vertex(const FA18RecordWalkerRuntimeInput *input, int16_t offset,
                       FA18RecordWalkerTriple *triple) {
    const int32_t first = offset;
    if (!triple || !input->vertex_table || first < 0 ||
        (uint32_t)first > input->vertex_table_size ||
        input->vertex_table_size - (uint32_t)first < 6u)
        return -1;
    for (unsigned index = 0; index < 3; ++index)
        triple->value[index] =
            (int16_t)fa18_be16(input->vertex_table + (uint32_t)first + index * 2u);
    return 0;
}

static int branch_cursor(const FA18RecordWalkerRuntimeInput *input, int16_t relative,
                         uint32_t *cursor) {
    const int64_t target = (int64_t)input->control_base + relative;
    if (!cursor || target < 0 || stream_offset(input, (uint32_t)target, cursor) != 0)
        return -1;
    *cursor = (uint32_t)target;
    return 0;
}

static int run_ordinary_record(const FA18RecordWalkerRuntimeInput *input,
                               FA18RecordWalkerRuntimeResult *result,
                               int16_t first, int16_t flags, int *status) {
    int source_status = 0;
    if ((uint16_t)flags & UINT16_C(0x0c00)) {
        if (!input->other_handler ||
            input->other_handler(input->context, first, flags, &source_status) != 0)
            return -1;
    } else if ((uint16_t)flags & UINT16_C(0x2000)) {
        int16_t words[6];
        if (!input->hex_handler) return -1;
        for (unsigned index = 0; index < 6; ++index) {
            if (read_stream_word(input, result->cursor, &words[index]) != 0) return -1;
            result->cursor += 2u;
        }
        if (input->hex_handler(input->context, words, &source_status) != 0) return -1;
    } else {
        FA18RecordWalkerTriple triples[3];
        int16_t offset;
        if (!input->triple_handler || read_vertex(input, first, &triples[0]) != 0)
            return -1;
        for (unsigned index = 1; index < 3; ++index) {
            if (read_stream_word(input, result->cursor, &offset) != 0 ||
                read_vertex(input, offset, &triples[index]) != 0)
                return -1;
            result->cursor += 2u;
        }
        if (input->triple_handler(input->context, flags, triples, &source_status) != 0)
            return -1;
    }
    *status = source_status;
    return 0;
}

int fa18_run_record_walker_runtime(const FA18RecordWalkerRuntimeInput *input,
                                   FA18RecordWalkerRuntimeResult *result,
                                   FA18RecordWalkerRuntimeRoute *route) {
    if (!input || !result || !route || !input->stream_bytes || !input->step_budget ||
        stream_offset(input, input->initial_cursor, &result->cursor) != 0)
        return -1;
    *result = (FA18RecordWalkerRuntimeResult){input->initial_cursor, 0, 0, 0, 0,
                                               0, 0, 0, 0, 0, 0};
    for (;;) {
        int16_t first;
        if (result->handled_controls >= input->step_budget) return -1;
        if (result->a1_count) {
            *route = FA18_RECORD_WALKER_RUNTIME_POST_STREAM_EXTERNAL;
            return 0;
        }
        if (read_stream_word(input, result->cursor, &first) != 0) return -1;
        result->cursor += 2u;
        if (first >= 0) {
            int16_t flags, relative;
            int source_status;
            if (read_stream_word(input, result->cursor, &flags) != 0) return -1;
            result->cursor += 2u;
            if (((uint16_t)flags & UINT16_C(0xfc00)) == UINT16_C(0xfc00)) {
                if (read_stream_word(input, result->cursor, &relative) != 0 ||
                    branch_cursor(input, relative, &result->cursor) != 0)
                    return -1;
            } else {
                if (run_ordinary_record(input, result, first, flags, &source_status) != 0)
                    return -1;
                if (read_stream_word(input, result->cursor +
                                     (source_status ? 2u : 0u), &relative) != 0 ||
                    branch_cursor(input, relative, &result->cursor) != 0)
                    return -1;
            }
            ++result->handled_controls;
            continue;
        }
        ++result->handled_controls;
        if (first == -1) {
            *route = FA18_RECORD_WALKER_RUNTIME_RETURN_ZERO;
            return 0;
        }
        result->a2_count = 0;
        result->a2_flag = 0;
        if ((uint16_t)first & UINT16_C(0x2000)) {
            result->a2_count = 1;
            if ((uint16_t)first & UINT16_C(0x1000)) {
                if (read_stream_long(input, result->cursor, &result->a2_cursor) != 0) return -1;
                result->cursor += 4u;
            } else {
                result->a2_cursor = input->control_base +
                                    ((uint16_t)first & UINT16_C(0x0fff));
            }
        } else {
            if ((uint16_t)first & UINT16_C(0x4000)) result->a1_count = 1;
            if ((uint16_t)first & UINT16_C(0x1000)) {
                if (read_stream_long(input, result->cursor, &result->a1_cursor) != 0) return -1;
                result->cursor += 4u;
            } else {
                result->a1_cursor = input->control_base +
                                    ((uint16_t)first & UINT16_C(0x0fff));
            }
            result->a1_flag = 0;
        }
        for (;;) {
            int16_t selector_word, source_status;
            uint16_t selector;
            if (result->a2_flag || result->a1_flag) break;
            if (result->a2_count) {
                *route = FA18_RECORD_WALKER_RUNTIME_POST_STREAM_EXTERNAL;
                return 0;
            }
            if (read_stream_word(input, result->a1_cursor, &selector_word) != 0) return -1;
            result->a1_cursor += 2u;
            if ((uint16_t)selector_word & UINT16_C(0x8000)) result->a1_flag = 1;
            if ((uint16_t)selector_word & UINT16_C(0x1000)) {
                if (read_stream_long(input, result->a1_cursor, &result->a2_cursor) != 0)
                    return -1;
                result->a1_cursor += 4u;
            } else {
                result->a2_cursor = input->control_base +
                                    ((uint16_t)selector_word & UINT16_C(0x0fff));
            }
            result->record_count = 0;
            for (;;) {
                if (result->record_count) {
                    *route = FA18_RECORD_WALKER_RUNTIME_CONTROL_EXTERNAL;
                    return 0;
                }
                if (read_stream_word(input, result->a2_cursor, &selector_word) != 0) return -1;
                result->a2_cursor += 2u;
                if (selector_word >= 0) {
                    if (!input->extended_handler) {
                        *route = FA18_RECORD_WALKER_RUNTIME_EXTENDED_CONTROL_EXTERNAL;
                        return 0;
                    }
                    if (input->extended_handler(input->extended_context ? input->extended_context :
                                                input->context, selector_word,
                                                result->a2_cursor, &source_status) != 0)
                        return -1;
                    if (source_status < 0) break;
                    result->record_status |= (uint16_t)source_status;
                    continue;
                }
                if (selector_word == -1) {
                    if (!input->error_handler ||
                        input->error_handler(input->context, selector_word) != 0)
                        return -1;
                    *route = FA18_RECORD_WALKER_RUNTIME_CONTROL_EXTERNAL;
                    return 0;
                }
                if ((uint16_t)selector_word & UINT16_C(0x4000)) ++result->record_count;
                selector = (uint16_t)selector_word & UINT16_C(0x3fff);
                if (!input->dispatch_handler ||
                    input->dispatch_handler(input->context, selector, result->a2_cursor,
                                            &source_status) != 0)
                    return -1;
                ++result->dispatched_selectors;
                if (source_status < 0) break;
                result->record_status |= (uint16_t)source_status;
            }
            /* `$C1F944` negative return enters `$C1F7FA`: resume A1 selection
             * unless the source's count/flag state transfers elsewhere. */
            continue;
        }
        if (result->a1_count) {
            *route = FA18_RECORD_WALKER_RUNTIME_POST_STREAM_EXTERNAL;
            return 0;
        }
        continue;
    }
}
