#include "viewport_palette.h"

#include <string.h>

enum {
    VIEWPORT_PALETTE_HUNK = 21,
    VIEWPORT_PALETTE_TABLE_OFFSET = 0x80,
    VIEWPORT_PALETTE_STRIDE = FA18_VIEWPORT_PALETTE_WORDS * 2
};

static uint16_t read_be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

int fa18_load_viewport_mode_palette(const FA18Hunks *exe, uint8_t mode,
                                    uint16_t palette[FA18_VIEWPORT_PALETTE_WORDS]) {
    const FA18HunkSegment *segment;
    uint32_t offset;

    if (!exe || !palette || mode >= FA18_VIEWPORT_PALETTE_MODE_COUNT ||
        VIEWPORT_PALETTE_HUNK >= exe->count)
        return -1;
    segment = &exe->segments[VIEWPORT_PALETTE_HUNK];
    offset = VIEWPORT_PALETTE_TABLE_OFFSET +
             (FA18_VIEWPORT_PALETTE_MODE_COUNT - 1u - mode) * VIEWPORT_PALETTE_STRIDE;
    if (!segment->data || offset > segment->size ||
        segment->size - offset < VIEWPORT_PALETTE_STRIDE)
        return -1;
    for (unsigned colour = 0; colour < FA18_VIEWPORT_PALETTE_WORDS; ++colour)
        palette[colour] = (uint16_t)(fa18_be16(segment->data + offset + colour * 2u) &
                                     0x0fffu);
    return 0;
}

int fa18_initialize_viewport_palette_buffer(const FA18Hunks *exe,
                                             FA18ViewportPaletteBuffer *buffer) {
    const FA18HunkSegment *segment;
    if (!exe || !buffer || VIEWPORT_PALETTE_HUNK >= exe->count) return -1;
    segment = &exe->segments[VIEWPORT_PALETTE_HUNK];
    if (!segment->data || segment->size < VIEWPORT_PALETTE_TABLE_OFFSET +
                         FA18_VIEWPORT_DYNAMIC_PALETTE_WORDS * 2u)
        return -1;
    for (unsigned index = 0; index < FA18_VIEWPORT_DYNAMIC_PALETTE_WORDS; ++index)
        buffer->words[index] = (uint16_t)(fa18_be16(segment->data +
            VIEWPORT_PALETTE_TABLE_OFFSET + index * 2u) & 0x0fffu);
    return 0;
}

int fa18_copy_viewport_mode_palette_to_buffer(const FA18Hunks *exe, uint8_t mode,
                                               FA18ViewportPaletteBuffer *buffer) {
    if (!buffer) return -1;
    return fa18_load_viewport_mode_palette(exe, mode, buffer->words);
}

int fa18_load_viewport_mode_palette_into_copper(
    const FA18Hunks *exe, uint8_t mode,
    FA18CopperMutableInstructionStream *streams, size_t stream_count,
    uint32_t *updated_mask) {
    uint16_t source[FA18_VIEWPORT_PALETTE_WORDS];
    uint16_t palette[32] = { 0 };

    if (fa18_load_viewport_mode_palette(exe, mode, source) != 0) return -1;
    memcpy(palette, source, sizeof source);
    return fa18_update_copper_palette_moves(streams, stream_count, palette,
                                            UINT32_C(0x0000ffff), updated_mask);
}

int fa18_apply_viewport_copper_palette_to_video(
    const FA18CopperMutableInstructionStream *streams, size_t stream_count,
    FA18Video *video) {
    uint16_t palette[FA18_VIEWPORT_PALETTE_WORDS] = {0};
    uint16_t present = 0;

    if (!streams || !stream_count || !video) return -1;
    for (size_t stream_index = 0; stream_index < stream_count; ++stream_index) {
        const FA18CopperMutableInstructionStream stream = streams[stream_index];
        if (!stream.bytes || stream.byte_count % 4u) return -1;
        for (size_t offset = 0; offset < stream.byte_count; offset += 4) {
            const uint16_t register_word = read_be16(stream.bytes + offset);
            const uint16_t value = read_be16(stream.bytes + offset + 2);
            if (register_word == UINT16_C(0xffff) && value == UINT16_C(0xfffe)) break;
            if (!(register_word & 1u) && register_word >= UINT16_C(0x0180) &&
                register_word <= UINT16_C(0x019e) &&
                !((register_word - UINT16_C(0x0180)) & 1u)) {
                const unsigned colour = (unsigned)(register_word - UINT16_C(0x0180)) / 2u;
                palette[colour] = (uint16_t)(value & 0x0fffu);
                present |= (uint16_t)(UINT16_C(1) << colour);
            }
        }
    }
    if (present != UINT16_C(0xffff)) return -1;
    memcpy(video->palette, palette, sizeof palette);
    return 0;
}
