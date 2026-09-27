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
    record->layout_offset = (uint32_t)header[0] * 40u + (int8_t)header[1];
    record->attribute = header[2];
    record->layout_index = header[3] & 0x0fu;
    return 0;
}
