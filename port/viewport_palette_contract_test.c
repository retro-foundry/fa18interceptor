#include "viewport_palette.h"

#include <assert.h>
#include <string.h>

static void put_be16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

int main(void) {
    enum { table_offset = 0x80, stride = FA18_VIEWPORT_PALETTE_WORDS * 2 };
    uint8_t table[table_offset + FA18_VIEWPORT_PALETTE_MODE_COUNT * stride] = { 0 };
    FA18HunkSegment segments[22] = { 0 };
    FA18Hunks exe = { segments, 22 };
    uint16_t palette[FA18_VIEWPORT_PALETTE_WORDS];
    uint8_t stream_bytes[] = {
        0x01, 0x80, 0x0a, 0xaa, 0x01, 0x82, 0x0a, 0xaa,
        0x01, 0x9e, 0x0a, 0xaa, 0x01, 0xa0, 0x0b, 0xbb,
        0xff, 0xff, 0xff, 0xfe
    };
    FA18CopperMutableInstructionStream stream = { stream_bytes, sizeof stream_bytes };
    FA18ViewportPaletteBuffer buffer;
    uint32_t updated = 0;

    segments[21] = (FA18HunkSegment){ .data = table, .size = sizeof table };
    for (unsigned mode = 0; mode < FA18_VIEWPORT_PALETTE_MODE_COUNT; ++mode) {
        const uint32_t offset = table_offset +
            (FA18_VIEWPORT_PALETTE_MODE_COUNT - 1u - mode) * stride;
        for (unsigned colour = 0; colour < FA18_VIEWPORT_PALETTE_WORDS; ++colour)
            put_be16(table + offset + colour * 2u,
                     (uint16_t)((mode << 8) | colour));
    }

    assert(fa18_initialize_viewport_palette_buffer(&exe, &buffer) == 0);
    assert(buffer.words[0] == 0x0f00 && buffer.words[15] == 0x0f0f);
    assert(buffer.words[16] == 0x0e00 && buffer.words[31] == 0x0e0f);
    buffer.words[16] = 0x0a5a;
    assert(fa18_copy_viewport_mode_palette_to_buffer(&exe, 8, &buffer) == 0);
    assert(buffer.words[0] == 0x0800 && buffer.words[15] == 0x080f);
    assert(buffer.words[16] == 0x0a5a);

    assert(fa18_load_viewport_mode_palette(&exe, 8, palette) == 0);
    assert(palette[0] == 0x0800 && palette[1] == 0x0801 && palette[15] == 0x080f);
    assert(fa18_load_viewport_mode_palette_into_copper(&exe, 8, &stream, 1,
                                                        &updated) == 0);
    assert(updated == ((UINT32_C(1) << 0) | (UINT32_C(1) << 1) |
                       (UINT32_C(1) << 15)));
    assert(stream_bytes[2] == 0x08 && stream_bytes[3] == 0x00);
    assert(stream_bytes[6] == 0x08 && stream_bytes[7] == 0x01);
    assert(stream_bytes[10] == 0x08 && stream_bytes[11] == 0x0f);
    assert(stream_bytes[14] == 0x0b && stream_bytes[15] == 0xbb);
    uint8_t video_stream_bytes[FA18_VIEWPORT_PALETTE_WORDS * 4u + 4u] = {0};
    FA18CopperMutableInstructionStream video_stream = {
        video_stream_bytes, sizeof video_stream_bytes
    };
    FA18Video video = {0};
    for (unsigned colour = 0; colour < FA18_VIEWPORT_PALETTE_WORDS; ++colour) {
        put_be16(video_stream_bytes + colour * 4u, (uint16_t)(0x0180u + colour * 2u));
        put_be16(video_stream_bytes + colour * 4u + 2u, (uint16_t)(0x0f00u + colour));
    }
    put_be16(video_stream_bytes + FA18_VIEWPORT_PALETTE_WORDS * 4u, 0xffff);
    put_be16(video_stream_bytes + FA18_VIEWPORT_PALETTE_WORDS * 4u + 2u, 0xfffe);
    video.palette[16] = 0x0abcu;
    assert(fa18_apply_viewport_copper_palette_to_video(&video_stream, 1, &video) == 0);
    assert(video.palette[0] == 0x0f00 && video.palette[15] == 0x0f0f &&
           video.palette[16] == 0x0abcu);
    video_stream.byte_count = (FA18_VIEWPORT_PALETTE_WORDS - 1u) * 4u;
    assert(fa18_apply_viewport_copper_palette_to_video(&video_stream, 1, &video) == -1);
    assert(fa18_load_viewport_mode_palette(&exe, 16, palette) == -1);
    assert(fa18_load_viewport_mode_palette_into_copper(&exe, 16, &stream, 1,
                                                        &updated) == -1);
    assert(fa18_initialize_viewport_palette_buffer(NULL, &buffer) == -1);
    segments[21].size = table_offset - 1;
    assert(fa18_load_viewport_mode_palette(&exe, 0, palette) == -1);
    return 0;
}
