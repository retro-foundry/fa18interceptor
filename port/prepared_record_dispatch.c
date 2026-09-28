#include "prepared_record_dispatch.h"

static int copy_triple(const FA18PreparedRecordDispatchInput *input,
                       uint16_t offset, int16_t *destination) {
    size_t word_index;
    if ((offset & 1u) != 0) return -1;
    word_index = (size_t)offset / 2u;
    if (word_index + 3u > input->vertex_word_count) return -1;
    destination[0] = input->vertex_words[word_index];
    destination[1] = input->vertex_words[word_index + 1u];
    destination[2] = input->vertex_words[word_index + 2u];
    return 0;
}

static int copy_next_ordinary_triple(const FA18PreparedRecordDispatchInput *input,
                                     uint16_t *cursor, uint16_t count) {
    uint16_t offset;
    if (*cursor >= input->stream_word_count ||
        (size_t)(count + 1u) * 3u > input->workspace_word_capacity)
        return -1;
    offset = (uint16_t)input->stream_words[(*cursor)++];
    if ((int16_t)offset < 0) return -1;
    return copy_triple(input, offset, input->workspace_words + (size_t)count * 3u);
}

int fa18_dispatch_prepared_record_triples(
    const FA18PreparedRecordDispatchInput *input,
    FA18PreparedRecordDispatchResult *result) {
    FA18RecordComponentPredicateState predicate;
    FA18RecordComponentPredicateRoute predicate_route;
    uint16_t cursor, count = 0;
    int16_t depth_and, descriptor;
    uint32_t d7;

    if (!input || !result || !input->vertex_words || !input->stream_words ||
        !input->workspace_words || input->stream_word_index > input->stream_word_count)
        return -1;
    cursor = input->stream_word_index;
    if (copy_next_ordinary_triple(input, &cursor, count++) != 0 ||
        copy_next_ordinary_triple(input, &cursor, count++) != 0)
        return -1;
    depth_and = input->workspace_words[2] & input->workspace_words[5];
    for (;;) {
        uint16_t offset;
        int terminal;
        if (cursor >= input->stream_word_count ||
            (size_t)(count + 1u) * 3u > input->workspace_word_capacity)
            return -1;
        offset = (uint16_t)input->stream_words[cursor++];
        terminal = (int16_t)offset < 0;
        if (terminal) offset &= UINT16_C(0x7fff);
        if (copy_triple(input, offset, input->workspace_words + (size_t)count * 3u) != 0)
            return -1;
        depth_and = (int16_t)((uint16_t)depth_and &
                              (uint16_t)input->workspace_words[(size_t)count * 3u + 2u]);
        ++count;
        if (terminal) break;
    }
    result->next_stream_word_index = cursor;
    result->triple_count = count;
    result->component_test_count_delta = 0;
    result->component_reject_count_delta = 0;
    if (depth_and < 0) {
        result->route = FA18_PREPARED_RECORD_NEGATIVE_STATUS;
        return 0;
    }
    if (cursor >= input->stream_word_count) return -1;
    descriptor = input->stream_words[cursor++];
    if (descriptor < 0) {
        result->next_stream_word_index = cursor;
        result->selected_record_word = descriptor;
        result->route = FA18_PREPARED_RECORD_DISPLAY_READY;
        return 0;
    }
    predicate = input->predicate;
    predicate.descriptor_word = descriptor;
    for (unsigned index = 0; index < 9; ++index)
        predicate.workspace[index] = input->workspace_words[index];
    ++result->component_test_count_delta;
    if (fa18_test_record_component_predicate(&predicate, (uint16_t)descriptor,
                                             cursor, &d7, &cursor,
                                             &predicate_route) != 0)
        return -1;
    if (predicate_route == FA18_RECORD_COMPONENT_PREDICATE_C1FC3A_EXTERNAL) {
        result->next_stream_word_index = cursor;
        result->route = FA18_PREPARED_RECORD_C1FC3A_EXTERNAL;
        return 0;
    }
    if (predicate_route == FA18_RECORD_COMPONENT_PREDICATE_REJECTED) {
        ++result->component_reject_count_delta;
        result->selected_record_word = descriptor;
        if (((uint16_t)descriptor & UINT16_C(0x4000)) != 0) {
            if (cursor >= input->stream_word_count) return -1;
            ++cursor;
        }
    } else {
        if (((uint16_t)descriptor & UINT16_C(0x4000)) == 0) {
            result->next_stream_word_index = cursor;
            result->route = FA18_PREPARED_RECORD_NEGATIVE_STATUS;
            return 0;
        }
        if (cursor >= input->stream_word_count) return -1;
        result->selected_record_word = input->stream_words[cursor++];
    }
    result->next_stream_word_index = cursor;
    result->route = FA18_PREPARED_RECORD_DISPLAY_READY;
    return 0;
}
