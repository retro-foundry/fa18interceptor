#include "static_template_stream_selector.h"

#include <string.h>

#include "hunk.h"
#include "static_template_search.h"

enum {
    WORKSPACE_CELL_BYTES = 0x60,
    WORKSPACE_CELL_COUNT = 16,
    WORKSPACE_BAND_BYTES = WORKSPACE_CELL_BYTES * WORKSPACE_CELL_COUNT
};

static int read_word(const uint8_t *bytes, size_t size, size_t offset,
                     int16_t *value) {
    if (!bytes || !value || offset > size || size - offset < 2u) return -1;
    *value = (int16_t)fa18_be16(bytes + offset);
    return 0;
}

static int read_long(const uint8_t *bytes, size_t size, ptrdiff_t offset,
                     uint32_t *value) {
    if (!bytes || !value || offset < 0 || (size_t)offset > size ||
        size - (size_t)offset < 4u)
        return -1;
    *value = fa18_be32(bytes + offset);
    return 0;
}

static int append_matches(const FA18StaticTemplateStreamInput *input,
                          uint8_t *cursor, size_t remaining,
                          int8_t block, int16_t group, int16_t row,
                          FA18StaticTemplateStreamResult *result) {
    FA18TemplateWorkspaceAppend append;
    size_t written;

    if (!input->append || !input->append->append_enable) return 0;
    append = *input->append;
    append.out = cursor;
    append.out_size = remaining;
    if (fa18_append_template_workspace_matches(&append, block, group, row,
                                               &written) != 0)
        return -1;
    result->append_marker_count = (uint16_t)(result->append_marker_count + written);
    return 0;
}

int fa18_select_static_template_stream(
    const FA18StaticTemplateStreamInput *input, int16_t group_selector,
    int16_t row_key, FA18StaticTemplateStreamResult *result,
    FA18StaticTemplateStreamRoute *route) {
    uint16_t selector_word;
    size_t directory_entry;
    int16_t relative_group;
    ptrdiff_t group_offset;
    int16_t row_bytes;
    uint16_t row_index;
    uint32_t stream_offset;
    ptrdiff_t bitset_offset;
    uint32_t bitset_word;
    size_t cursor_offset = 0;
    int16_t prior_block = -1;
    int item_budget = WORKSPACE_CELL_COUNT;

    if (!input || !result || !route || !input->immutable_bytes ||
        !input->bitset_bytes || !input->special_pairs || !input->workspace ||
        input->workspace_size < WORKSPACE_BAND_BYTES ||
        input->special_pair_size < 32u)
        return -1;
    memset(result, 0, sizeof *result);

    selector_word = (uint16_t)group_selector;
    directory_entry = input->directory_offset + (size_t)selector_word * 2u;
    if (directory_entry < input->directory_offset ||
        read_word(input->immutable_bytes, input->immutable_size, directory_entry,
                  &relative_group) != 0)
        return -1;
    group_offset = (ptrdiff_t)input->directory_offset + relative_group;
    if (group_offset < 0 || read_word(input->immutable_bytes, input->immutable_size,
                                      (size_t)group_offset, &row_bytes) != 0)
        return -1;
    if (row_bytes < 0) {
        *route = FA18_STATIC_TEMPLATE_STREAM_REJECTED_GROUP;
        return 0;
    }

    bitset_offset = (ptrdiff_t)((int16_t)((uint16_t)group_selector << 3)) +
                    (ptrdiff_t)((int16_t)(row_key >> 5) * 4);
    if (read_long(input->bitset_bytes, input->bitset_size, bitset_offset,
                  &bitset_word) != 0)
        return -1;
    if (!(bitset_word & (UINT32_C(1) << ((uint16_t)row_key & 31u)))) {
        *route = FA18_STATIC_TEMPLATE_STREAM_REJECTED_BIT;
        return 0;
    }

    if ((uint16_t)row_bytes & 1u ||
        fa18_search_static_template_row(input->immutable_bytes + group_offset,
                                        input->immutable_size - (size_t)group_offset,
                                        row_key, &row_index) != 0) {
        result->status_word = 0x1c;
        *route = FA18_STATIC_TEMPLATE_STREAM_REJECTED_ROW;
        return 0;
    }
    result->selected_row_index = row_index;
    if ((size_t)row_bytes > input->immutable_size - (size_t)group_offset - 2u ||
        (size_t)row_bytes > SIZE_MAX - (size_t)group_offset - 2u)
        return -1;
    {
        const size_t pointer_offset = (size_t)group_offset + 2u + (size_t)row_bytes +
                                      (size_t)row_index * 4u;
        if (pointer_offset > input->immutable_size ||
            input->immutable_size - pointer_offset < 4u)
            return -1;
        stream_offset = fa18_be32(input->immutable_bytes + pointer_offset);
    }
    if (stream_offset >= input->immutable_size) return -1;

    while (stream_offset < input->immutable_size) {
        uint8_t header = input->immutable_bytes[stream_offset++];
        uint8_t block;
        uint8_t flags;
        uint8_t *cursor;

        if (header == 0xffu) {
            if (cursor_offset > input->workspace_size ||
                append_matches(input, input->workspace + cursor_offset,
                               input->workspace_size - cursor_offset,
                               (int8_t)prior_block, group_selector, row_key,
                               result) != 0)
                return -1;
            *route = FA18_STATIC_TEMPLATE_STREAM_EXPANDED;
            return 0;
        }
        block = (uint8_t)(header & 0x0fu);
        if ((int16_t)block != prior_block) {
            if (cursor_offset > input->workspace_size ||
                append_matches(input, input->workspace + cursor_offset,
                               input->workspace_size - cursor_offset,
                               (int8_t)prior_block, group_selector, row_key,
                               result) != 0)
                return -1;
            prior_block = (int16_t)block;
            cursor_offset = (size_t)block * WORKSPACE_CELL_BYTES;
        }
        if (--item_budget < 0) {
            result->status_word = 0x38;
            return -1;
        }
        if (cursor_offset >= input->workspace_size ||
            input->workspace_size - cursor_offset < WORKSPACE_CELL_BYTES)
            return -1;
        cursor = input->workspace + cursor_offset;
        if (stream_offset >= input->immutable_size) return -1;
        flags = input->immutable_bytes[stream_offset++];
        cursor[0] = (uint8_t)(flags & 0x80u);
        cursor[1] = (uint8_t)(flags & 0x7fu);
        if (flags & 0x80u) {
            const size_t pair = (size_t)(header >> 4) * 2u;
            int16_t first;
            if (pair + 1u >= input->special_pair_size) return -1;
            first = (int8_t)input->special_pairs[pair];
            cursor[2] = (uint8_t)((uint16_t)first >> 8);
            cursor[3] = (uint8_t)first;
            /* MOVE.B preserves D0's sign-extension high byte from `first`. */
            cursor[4] = (uint8_t)((uint16_t)first >> 8);
            cursor[5] = input->special_pairs[pair + 1u];
            cursor[6] = 0xffu;
        } else {
            if (stream_offset > input->immutable_size ||
                input->immutable_size - stream_offset < 4u)
                return -1;
            memcpy(cursor + 2u, input->immutable_bytes + stream_offset, 4u);
            stream_offset += 4u;
            cursor[6] = 0xffu;
        }
        cursor_offset += 7u;
        ++result->expanded_item_count;
    }
    return -1;
}
