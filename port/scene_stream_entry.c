#include "scene_stream_entry.h"

#include "hunk.h"
#include "scene_stream_selector.h"

static int32_t asr_long(int32_t value, unsigned count) {
    if (!count) return value;
    if (count >= 32u) return value < 0 ? -1 : 0;
    if (value >= 0) return value >> count;
    return (int32_t)-((-(int64_t)value + ((INT64_C(1) << count) - 1)) >> count);
}

static int read_word(const uint8_t *bytes, size_t size, uint32_t offset,
                     int16_t *word) {
    if (!bytes || !word || offset > size || size - offset < 2u) return -1;
    *word = (int16_t)fa18_be16(bytes + offset);
    return 0;
}

static int16_t effective_limit(const FA18SceneStreamEntryInput *input) {
    uint32_t scaled;
    uint32_t product;
    if (input->depth_scale_enabled) return input->selector_limit;
    /* `$C1EE2E-$C1EE3E`: EXT.L/ASL #8/DIVU #$80, then MULU and ASR #8. */
    scaled = (uint32_t)((int32_t)input->depth_scale_word * 256) / 0x80u;
    product = (uint32_t)(uint16_t)input->selector_limit * (uint16_t)scaled;
    return (int16_t)asr_long((int32_t)product, 8);
}

static int selector_row_matches(const FA18SceneStreamEntryInput *input) {
    size_t offset = 0;
    if (!input->activity_selector_rows || !input->activity_selector_rows_size ||
        input->activity_selector_rows_size % 24u)
        return -1;
    while (offset < input->activity_selector_rows_size) {
        const int16_t word = (int16_t)fa18_be16(input->activity_selector_rows + offset);
        if (word < 0) return 0;
        if ((uint16_t)word >> 8 == input->activity_selector_index) return 1;
        offset += 24u;
    }
    return -1;
}

int fa18_enter_scene_stream(const FA18SceneStreamEntryInput *input,
                            FA18SceneStreamEntryResult *result,
                            FA18SceneStreamEntryRoute *route) {
    uint32_t cursor;
    int16_t word;

    if (!input || !result || !route || !input->control_stream) return -1;
    result->effective_limit = effective_limit(input);
    cursor = input->stream_cursor;
    if (read_word(input->control_stream, input->control_stream_size, cursor, &word) != 0)
        return -1;
    cursor += 2u;
    while (word >= 0) {
        FA18SceneStreamSelectorResult selection;
        FA18SceneStreamSelectorRoute selection_route;
        if (fa18_select_scene_stream_threshold(
                input->control_stream, input->control_stream_size, cursor,
                input->control_base,
                word, input->selector_shift, result->effective_limit,
                &selection, &selection_route) != 0)
            return -1;
        if (selection_route == FA18_SCENE_STREAM_SELECTOR_SENTINEL) {
            *route = FA18_SCENE_STREAM_ENTRY_RETURN_ZERO;
            return 0;
        }
        if (selection_route == FA18_SCENE_STREAM_SELECTOR_SELECTED) {
            word = selection.selected_word;
            cursor = selection.next_cursor;
            break;
        }
        cursor = selection.next_cursor;
        if (read_word(input->control_stream, input->control_stream_size, cursor, &word) != 0)
            return -1;
        cursor += 2u;
    }
    result->selected_word = word;
    if (word == -1) {
        *route = FA18_SCENE_STREAM_ENTRY_RETURN_ZERO;
        return 0;
    }
    if (word & UINT16_C(0x2000)) {
        if (input->activity_flag) {
            *route = FA18_SCENE_STREAM_ENTRY_RETURN_ONE;
            return 0;
        }
    }
    if (input->depth_scale_word < 0x80) {
        if (!input->activity_flag) {
            const int matches = selector_row_matches(input);
            if (matches < 0) return -1;
            if (matches) {
                *route = FA18_SCENE_STREAM_ENTRY_RETURN_ONE;
                return 0;
            }
        }
    } else if (!input->activity_flag) {
        *route = FA18_SCENE_STREAM_ENTRY_RETURN_ONE;
        return 0;
    }
    result->descriptor.local_flag = !(word & UINT16_C(0x4000));
    if (result->descriptor.local_flag && input->descriptor_gate_flag &&
        !(input->descriptor_gate_word & 1u)) {
        *route = FA18_SCENE_STREAM_ENTRY_RETURN_ZERO;
        return 0;
    }
    result->descriptor.descriptor_cursor = input->descriptor_base +
                                            ((uint16_t)word & UINT16_C(0x0fff));
    result->descriptor.published_stage_cursor = cursor;
    *route = FA18_SCENE_STREAM_ENTRY_DESCRIPTOR_READY;
    return 0;
}
