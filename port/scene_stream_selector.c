#include "scene_stream_selector.h"

#include "hunk.h"

static int16_t asr_word(int16_t value, unsigned count) {
    count &= 63u;
    if (!count) return value;
    if (count >= 16u) return value < 0 ? -1 : 0;
    if (value >= 0) return (int16_t)((uint16_t)value >> count);
    const int32_t magnitude = -(int32_t)value;
    return (int16_t)-((magnitude + ((1 << count) - 1)) >> count);
}

static int read_word(const uint8_t *stream, size_t size, uint32_t offset,
                     int16_t *word) {
    if (!stream || !word || offset > size || size - offset < 2u) return -1;
    *word = (int16_t)fa18_be16(stream + offset);
    return 0;
}

int fa18_select_scene_stream_threshold(const uint8_t *stream, size_t stream_size,
                                       uint32_t cursor, uint32_t record_base,
                                       int16_t first_word, int16_t shift,
                                       int16_t limit,
                                       FA18SceneStreamSelectorResult *result,
                                       FA18SceneStreamSelectorRoute *route) {
    if (!stream || !result || !route) return -1;
    const uint16_t flags = (uint16_t)first_word;
    const int16_t scaled = asr_word((int16_t)(flags & 0x3fffu), (uint16_t)shift);
    if (scaled > limit) {
        const uint32_t selected_offset = cursor + ((flags & 0x4000u) ? 0u : 2u);
        if (read_word(stream, stream_size, selected_offset, &result->selected_word) != 0)
            return -1;
        result->next_cursor = selected_offset + 2u;
        *route = FA18_SCENE_STREAM_SELECTOR_SELECTED;
    } else if (flags & 0x4000u) {
        result->next_cursor = cursor + 4u;
        *route = FA18_SCENE_STREAM_SELECTOR_RETRY;
    } else {
        int16_t relative;
        if (read_word(stream, stream_size, cursor, &relative) != 0) return -1;
        if (relative < 0) {
            result->next_cursor = cursor;
            *route = FA18_SCENE_STREAM_SELECTOR_SENTINEL;
        } else {
            const int64_t target = (int64_t)record_base + relative;
            if (target < 0 || (uint64_t)target >= stream_size) return -1;
            result->next_cursor = (uint32_t)target;
            *route = FA18_SCENE_STREAM_SELECTOR_RETRY;
        }
    }
    return 0;
}
