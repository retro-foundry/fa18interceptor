#include "menu_record.h"

#include <string.h>

static int read_word(const FA18HunkSegment *segment, uint32_t offset, int16_t *out) {
    if (offset > segment->size || segment->size - offset < 2) return -1;
    *out = (int16_t)fa18_be16(segment->data + offset);
    return 0;
}

int fa18_menu_select_message_record(const FA18Hunks *exe, uint16_t selector,
                                    FA18MenuRecord *record) {
    if (!exe || !record || selector == 0 || FA18_MENU_TEXT_HUNK >= exe->count) return -1;
    const FA18HunkSegment *segment = &exe->segments[FA18_MENU_TEXT_HUNK];
    uint32_t table_entry = FA18_MENU_RECORD_TABLE_OFFSET + ((uint32_t)selector - 1u) * 2u;
    int16_t relative;
    if (read_word(segment, table_entry, &relative) != 0) return -1;
    int64_t descriptor = (int64_t)FA18_MENU_RECORD_TABLE_OFFSET + relative;
    if (descriptor < 0 || (uint64_t)descriptor > segment->size ||
        segment->size - (uint32_t)descriptor < 4) return -1;
    const uint8_t *header = segment->data + (uint32_t)descriptor;
    const uint8_t *text = header + 4;
    size_t remaining = segment->size - ((uint32_t)descriptor + 4u);
    const uint8_t *end = memchr(text, 0, remaining);
    if (!end) return -1;
    record->text = text;
    record->text_length = (size_t)(end - text);
    record->layout_offset = (int32_t)header[0] * 40 + (int8_t)header[1];
    record->attribute = header[2];
    record->layout_index = header[3] >> 4;
    return 0;
}

int fa18_menu_select_inline_followup(const FA18Hunks *exe,
                                     const FA18MenuRecord *previous,
                                     uint32_t layout_increment,
                                     FA18MenuRecord *record) {
    if (!exe || !previous || !record || FA18_MENU_TEXT_HUNK >= exe->count) return -1;
    const FA18HunkSegment *segment = &exe->segments[FA18_MENU_TEXT_HUNK];
    uintptr_t start = (uintptr_t)segment->data;
    uintptr_t end = start + segment->size;
    uintptr_t previous_text = (uintptr_t)previous->text;
    if (!previous->text || previous_text < start || previous_text >= end ||
        previous->text_length >= end - previous_text)
        return -1;
    /* `$C32C3A-$C32C5C` skips the completed string's NUL, then reads the
     * inline glyph offset, attribute, and layout/subtype bytes. */
    const uint8_t *metadata = previous->text + previous->text_length + 1;
    if ((uintptr_t)metadata > end || end - (uintptr_t)metadata < 3)
        return -1;
    const uint8_t *text = metadata + 3;
    const uint8_t *text_end = memchr(text, 0, (size_t)(end - (uintptr_t)text));
    if (!text_end) return -1;
    if (layout_increment > INT32_MAX || previous->layout_offset > INT32_MAX - (int32_t)layout_increment)
        return -1;
    record->text = text;
    record->text_length = (size_t)(text_end - text);
    record->layout_offset = previous->layout_offset + (int32_t)layout_increment;
    record->attribute = metadata[1];
    record->layout_index = metadata[2] >> 4;
    return 0;
}
