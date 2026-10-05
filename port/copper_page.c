#include "copper_page.h"

#include <string.h>

enum {
    COPPER_BPLCON0 = 0x0100,
    COPPER_BPL1PTH = 0x00e0,
    COPPER_BPL5PTL = 0x00f2,
    COPPER_COLOR00 = 0x0180,
    COPPER_COLOR31 = 0x01be,
    COPPER_END_1 = 0xffff,
    COPPER_END_2 = 0xfffe
};

static uint16_t read_be16(const uint8_t *bytes) {
    return (uint16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static void write_be16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8);
    bytes[1] = (uint8_t)value;
}

static const uint8_t *find_plane_buffer(const FA18CopperPlaneBuffer *buffers,
                                        size_t buffer_count, uint32_t pointer) {
    for (size_t index = 0; index < buffer_count; ++index) {
        if (buffers[index].source_pointer == pointer && buffers[index].bytes &&
            buffers[index].byte_count >= FA18_COPPER_PAGE_BYTES)
            return buffers[index].bytes;
    }
    return NULL;
}

int fa18_decode_copper_page_at_vpos(const FA18CopperInstructionStream *streams,
                                    size_t stream_count, uint8_t vpos,
                                    FA18CopperPageState *state) {
    uint16_t plane_high[FA18_COPPER_PAGE_PLANES] = { 0 };
    uint16_t plane_low[FA18_COPPER_PAGE_PLANES] = { 0 };
    uint8_t plane_fields[FA18_COPPER_PAGE_PLANES] = { 0 };
    int have_bplcon0 = 0;

    if (!streams || !stream_count || !state) return -1;
    memset(state, 0, sizeof *state);

    for (size_t stream_index = 0; stream_index < stream_count; ++stream_index) {
        const FA18CopperInstructionStream stream = streams[stream_index];
        if (!stream.bytes || stream.byte_count % 4u) return -1;

        for (size_t offset = 0; offset < stream.byte_count; offset += 4) {
            const uint16_t first = read_be16(stream.bytes + offset);
            const uint16_t second = read_be16(stream.bytes + offset + 2);

            if (first == COPPER_END_1 && second == COPPER_END_2) goto complete;
            if (first & 1u) {
                /* A Copper SKIP instruction is a different source route. */
                if (second & 1u) return -1;
                if ((uint8_t)(first >> 8) > vpos) goto complete;
                continue;
            }

            if (first == COPPER_BPLCON0) {
                state->bplcon0 = second;
                have_bplcon0 = 1;
            } else if (first >= COPPER_COLOR00 && first <= COPPER_COLOR31) {
                const unsigned colour = (unsigned)(first - COPPER_COLOR00) / 2u;
                if ((first - COPPER_COLOR00) % 2u || colour >= 32u) return -1;
                state->palette[colour] = (uint16_t)(second & 0x0fffu);
                state->palette_written_mask |= UINT32_C(1) << colour;
            } else if (first >= COPPER_BPL1PTH && first <= COPPER_BPL5PTL) {
                const unsigned register_offset = (unsigned)(first - COPPER_BPL1PTH);
                if (register_offset % 4u > 2u) return -1;
                const unsigned plane = register_offset / 4u;
                if (plane >= FA18_COPPER_PAGE_PLANES) return -1;
                if (register_offset % 4u == 0) {
                    plane_high[plane] = second;
                    plane_fields[plane] |= 1u;
                } else {
                    plane_low[plane] = second;
                    plane_fields[plane] |= 2u;
                }
            }
        }
    }

complete:
    if (!have_bplcon0) return -1;
    /* Retain the existing full-state decoder for blank/other BPLCON0 values.
     * The actual second setup list provides only four active plane pairs. */
    const unsigned depth = ((state->bplcon0 >> 12) & 7u) == 4 ? 4 : FA18_COPPER_PAGE_PLANES;
    for (unsigned plane = 0; plane < depth; ++plane) {
        if (plane_fields[plane] != 3u) return -1;
        state->plane_pointers[plane] = (uint32_t)plane_high[plane] << 16 |
                                      plane_low[plane];
    }
    return 0;
}

int fa18_update_copper_palette_moves(FA18CopperMutableInstructionStream *streams,
                                     size_t stream_count,
                                     const uint16_t palette[32],
                                     uint32_t palette_mask,
                                     uint32_t *updated_mask) {
    uint32_t present_mask = 0;

    if (!streams || !stream_count || !palette) return -1;
    for (size_t stream_index = 0; stream_index < stream_count; ++stream_index) {
        const FA18CopperMutableInstructionStream stream = streams[stream_index];
        if (!stream.bytes || stream.byte_count % 4u) return -1;
        for (size_t offset = 0; offset < stream.byte_count; offset += 4) {
            const uint16_t first = read_be16(stream.bytes + offset);
            const uint16_t second = read_be16(stream.bytes + offset + 2);
            if (first == COPPER_END_1 && second == COPPER_END_2) break;
            if (first & 1u) {
                if (second & 1u) return -1;
                continue;
            }
            if (first >= COPPER_COLOR00 && first <= COPPER_COLOR31) {
                const unsigned colour = (unsigned)(first - COPPER_COLOR00) / 2u;
                if ((first - COPPER_COLOR00) % 2u || colour >= 32u) return -1;
                present_mask |= UINT32_C(1) << colour;
            }
        }
    }
    for (size_t stream_index = 0; stream_index < stream_count; ++stream_index) {
        const FA18CopperMutableInstructionStream stream = streams[stream_index];
        for (size_t offset = 0; offset < stream.byte_count; offset += 4) {
            const uint16_t first = read_be16(stream.bytes + offset);
            const uint16_t second = read_be16(stream.bytes + offset + 2);
            if (first == COPPER_END_1 && second == COPPER_END_2) break;
            if (!(first & 1u) && first >= COPPER_COLOR00 && first <= COPPER_COLOR31) {
                const unsigned colour = (unsigned)(first - COPPER_COLOR00) / 2u;
                const uint32_t bit = UINT32_C(1) << colour;
                if (palette_mask & bit)
                    write_be16(stream.bytes + offset + 2, (uint16_t)(palette[colour] & 0x0fffu));
            }
        }
    }
    if (updated_mask) *updated_mask = present_mask & palette_mask;
    return 0;
}

int fa18_present_copper_page(const FA18CopperPageState *state,
                             const FA18CopperPlaneBuffer *buffers,
                             size_t buffer_count, FA18Video *video) {
    const uint8_t *planes[FA18_COPPER_PAGE_PLANES];
    unsigned depth;

    if (!state || !buffers || !video) return -1;
    depth = (state->bplcon0 >> 12) & 7u;
    if (depth != 4 && depth != FA18_COPPER_PAGE_PLANES) return -1;
    for (size_t plane = 0; plane < depth; ++plane) {
        planes[plane] = find_plane_buffer(buffers, buffer_count,
                                          state->plane_pointers[plane]);
        if (!planes[plane]) return -1;
    }
    if (state->palette_written_mask == UINT32_MAX)
        memcpy(video->palette, state->palette, sizeof state->palette);

    for (int y = 0; y < FA18_HEIGHT; ++y) {
        for (int byte_x = 0; byte_x < FA18_COPPER_PAGE_ROW_BYTES; ++byte_x) {
            const size_t source = (size_t)y * FA18_COPPER_PAGE_ROW_BYTES + byte_x;
            for (int bit = 0; bit < 8; ++bit) {
                const uint8_t mask = (uint8_t)(0x80u >> bit);
                uint8_t colour_index = 0;
                for (unsigned plane = 0; plane < depth; ++plane) {
                    if (planes[plane][source] & mask)
                        colour_index |= (uint8_t)(1u << plane);
                }
                video->pixels[(size_t)y * FA18_WIDTH + (size_t)byte_x * 8u + bit] =
                    colour_index;
            }
        }
    }
    return 0;
}
