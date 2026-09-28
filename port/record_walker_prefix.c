#include "record_walker_prefix.h"

#include "hunk.h"

static int read_word(const uint8_t *bytes, size_t size, uint32_t offset,
                     int16_t *word) {
    if (!bytes || !word || offset > size || size - offset < 2u) return -1;
    *word = (int16_t)fa18_be16(bytes + offset);
    return 0;
}

static int read_triple(const uint8_t *bytes, size_t size, int16_t offset,
                       FA18RecordWalkerTriple *triple) {
    const int32_t first = offset;
    if (!triple || first < 0 || (uint32_t)first > size ||
        size - (uint32_t)first < 6u)
        return -1;
    for (unsigned index = 0; index < 3; ++index)
        triple->value[index] = (int16_t)fa18_be16(bytes + (uint32_t)first + index * 2u);
    return 0;
}

static int relative_cursor(const FA18RecordWalkerPrefixInput *input,
                           int16_t relative, uint32_t *cursor) {
    const int64_t target = (int64_t)input->control_base + relative;
    if (!cursor || target < 0 || (uint64_t)target >= input->control_stream_size)
        return -1;
    *cursor = (uint32_t)target;
    return 0;
}

int fa18_run_record_walker_prefix(const FA18RecordWalkerPrefixInput *input,
                                  FA18RecordWalkerPrefixResult *result,
                                  FA18RecordWalkerPrefixRoute *route) {
    uint32_t cursor;

    if (!input || !result || !route || !input->control_stream ||
        !input->step_budget)
        return -1;
    *result = (FA18RecordWalkerPrefixResult){input->initial_cursor, 0,
                                             UINT32_C(0x000fffff), 0};
    cursor = input->initial_cursor;
    while (result->handled_controls < input->step_budget) {
        int16_t first, flags;
        int source_status;
        if (read_word(input->control_stream, input->control_stream_size, cursor,
                      &first) != 0)
            return -1;
        cursor += 2u;
        if (first < 0) {
            result->cursor = cursor;
            if (first == -1) {
                *route = FA18_RECORD_WALKER_RETURN_ZERO;
                return 0;
            }
            *route = FA18_RECORD_WALKER_NEGATIVE_CONTROL_EXTERNAL;
            return 0;
        }
        if (read_word(input->control_stream, input->control_stream_size, cursor,
                      &flags) != 0)
            return -1;
        cursor += 2u;
        if (((uint16_t)flags & UINT16_C(0xfc00)) == UINT16_C(0xfc00)) {
            int16_t relative;
            if (read_word(input->control_stream, input->control_stream_size, cursor,
                          &relative) != 0 || relative_cursor(input, relative, &cursor) != 0)
                return -1;
            ++result->handled_controls;
            continue;
        }
        source_status = 0;
        if ((uint16_t)flags & UINT16_C(0x0c00)) {
            if (!input->other_handler ||
                input->other_handler(input->context, first, flags, &source_status) != 0)
                return -1;
        } else if ((uint16_t)flags & UINT16_C(0x2000)) {
            int16_t words[6];
            if (!input->hex_handler) return -1;
            for (unsigned index = 0; index < 6; ++index) {
                if (read_word(input->control_stream, input->control_stream_size, cursor,
                              &words[index]) != 0)
                    return -1;
                cursor += 2u;
            }
            if (input->hex_handler(input->context, words, &source_status) != 0) return -1;
        } else {
            FA18RecordWalkerTriple triples[3];
            int16_t offsets[2];
            if (!input->vertex_table || !input->triple_handler ||
                read_triple(input->vertex_table, input->vertex_table_size, first,
                            &triples[0]) != 0)
                return -1;
            for (unsigned index = 0; index < 2; ++index) {
                if (read_word(input->control_stream, input->control_stream_size, cursor,
                              &offsets[index]) != 0)
                    return -1;
                cursor += 2u;
                if (read_triple(input->vertex_table, input->vertex_table_size, offsets[index],
                                &triples[index + 1u]) != 0)
                    return -1;
            }
            if (input->triple_handler(input->context, flags, triples, &source_status) != 0)
                return -1;
        }
        ++result->handled_controls;
        if (!source_status) {
            int16_t relative;
            if (read_word(input->control_stream, input->control_stream_size, cursor,
                          &relative) != 0 || relative_cursor(input, relative, &cursor) != 0)
                return -1;
        } else {
            int16_t relative;
            if (read_word(input->control_stream, input->control_stream_size, cursor + 2u,
                          &relative) != 0 || relative_cursor(input, relative, &cursor) != 0)
                return -1;
        }
    }
    return -1;
}
