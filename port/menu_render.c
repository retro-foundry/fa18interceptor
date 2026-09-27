#include "menu_render.h"

#include <string.h>

enum {
    PALETTE_HUNK = 21,
    GLYPH_HUNK = 61,
    TEXT_HUNK = 64,
    PALETTE_OFFSET = 0,
    GLYPH_TABLE_OFFSET = 0x16c,
    LAYOUT_TABLE_OFFSET = 0x2366,
    GLYPH_ROWS = 7
};

static int word_at(const FA18HunkSegment *segment, uint32_t offset, uint16_t *word) {
    if (offset > segment->size || segment->size - offset < 2) return -1;
    *word = fa18_be16(segment->data + offset);
    return 0;
}

static int draw_glyph(FA18Video *video, const FA18Hunks *exe,
                      const FA18MenuRecord *record, size_t character, uint8_t ascii) {
    const FA18HunkSegment *glyphs = &exe->segments[GLYPH_HUNK];
    const FA18HunkSegment *text = &exe->segments[TEXT_HUNK];
    if (ascii < 0x20u) return -1;
    uint32_t table_entry = GLYPH_TABLE_OFFSET + ((uint32_t)ascii - 0x20u) * 2u;
    uint16_t glyph_relative;
    uint16_t layout_offset;
    uint16_t layout_word;
    if (word_at(glyphs, table_entry, &glyph_relative) != 0 ||
        word_at(text, LAYOUT_TABLE_OFFSET + (uint32_t)character * 4u, &layout_offset) != 0 ||
        word_at(text, LAYOUT_TABLE_OFFSET + (uint32_t)character * 4u + 2u, &layout_word) != 0)
        return -1;
    uint32_t glyph_offset = GLYPH_TABLE_OFFSET + glyph_relative;
    if (glyph_offset > glyphs->size || glyphs->size - glyph_offset < GLYPH_ROWS) return -1;
    int32_t byte_offset = record->layout_offset + (int16_t)layout_offset;
    if (byte_offset < 0) return -1;
    int x = (byte_offset % 40) * 8 + (layout_word >> 12);
    int y = byte_offset / 40;
    uint8_t color = record->layout_index;
    for (int row = 0; row < GLYPH_ROWS; ++row) {
        uint8_t bits = glyphs->data[glyph_offset + row];
        for (int column = 0; column < 8; ++column) {
            if ((bits & (0x80u >> column)) == 0) continue;
            int pixel_x = x + column;
            int pixel_y = y + row;
            if (pixel_x < 0 || pixel_x >= FA18_WIDTH || pixel_y < 0 || pixel_y >= FA18_HEIGHT)
                return -1;
            video->pixels[(size_t)pixel_y * FA18_WIDTH + (size_t)pixel_x] = color;
        }
    }
    return 0;
}

int fa18_render_top_level_menu(FA18Video *video, const FA18Hunks *exe,
                               const FA18MenuRecord *records, size_t count) {
    if (!video || !exe || !records || PALETTE_HUNK >= exe->count ||
        GLYPH_HUNK >= exe->count || TEXT_HUNK >= exe->count) return -1;
    const FA18HunkSegment *palette = &exe->segments[PALETTE_HUNK];
    if (palette->size - PALETTE_OFFSET < 32) return -1;
    memset(video, 0, sizeof *video);
    for (uint32_t index = 0; index < 16; ++index)
        video->palette[index] = fa18_be16(palette->data + PALETTE_OFFSET + index * 2u) & 0x0fffu;
    for (size_t record = 0; record < count; ++record)
        for (size_t character = 0; character < records[record].text_length; ++character)
            if (draw_glyph(video, exe, &records[record], character,
                           records[record].text[character]) != 0)
                return -1;
    return 0;
}
