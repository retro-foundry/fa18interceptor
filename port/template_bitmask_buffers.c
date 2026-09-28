#include "template_bitmask_buffers.h"

#include <string.h>

static int16_t read_be16(const uint8_t *bytes) {
    return (int16_t)((uint16_t)bytes[0] << 8 | bytes[1]);
}

static uint32_t read_be32(const uint8_t *bytes) {
    return (uint32_t)bytes[0] << 24 | (uint32_t)bytes[1] << 16 |
           (uint32_t)bytes[2] << 8 | bytes[3];
}

static void write_be32(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)(value >> 24);
    bytes[1] = (uint8_t)(value >> 16);
    bytes[2] = (uint8_t)(value >> 8);
    bytes[3] = (uint8_t)value;
}

static int expand_stream(const uint8_t *stream, size_t stream_size,
                         uint8_t buffer[FA18_TEMPLATE_BITMASK_BYTES]) {
    if (!stream || stream_size < FA18_TEMPLATE_BITMASK_ROWS * 2u) return -1;
    for (size_t row = 0; row != FA18_TEMPLATE_BITMASK_ROWS; ++row) {
        const int16_t relative = read_be16(stream + row * 2u);
        uint8_t *destination = buffer + row * FA18_TEMPLATE_BITMASK_ROW_BYTES;
        if (relative <= 0 || (size_t)relative > stream_size - 2u) return -1;
        const uint8_t *items = stream + (uint16_t)relative;
        const int16_t byte_count = read_be16(items);
        if (byte_count < 0) continue;
        if ((byte_count & 1) != 0 || byte_count == 0 ||
            (size_t)byte_count > stream_size - (size_t)relative - 2u)
            return -1;
        for (size_t index = 0; index != (size_t)byte_count / 2u; ++index) {
            const uint16_t bit = (uint16_t)read_be16(items + 2u + index * 2u);
            const size_t word_offset = (size_t)(bit >> 5) * 4u;
            uint32_t word;
            if (word_offset + 4u > FA18_TEMPLATE_BITMASK_ROW_BYTES) return -1;
            word = read_be32(destination + word_offset);
            word |= UINT32_C(1) << (bit & 31u);
            write_be32(destination + word_offset, word);
        }
    }
    return 0;
}

int fa18_build_template_bitmask_buffers(const uint8_t *first_stream,
                                        size_t first_size,
                                        const uint8_t *second_stream,
                                        size_t second_size,
                                        const uint8_t *third_stream,
                                        size_t third_size,
                                        FA18TemplateBitmaskBuffers *buffers) {
    if (!buffers) return -1;
    memset(buffers, 0, sizeof *buffers);
    if (expand_stream(first_stream, first_size, buffers->first) != 0) return 0x43;
    if (expand_stream(second_stream, second_size, buffers->second) != 0) return 0x44;
    if (expand_stream(third_stream, third_size, buffers->third) != 0) return 0x45;
    return 0;
}

int fa18_initialize_template_bitmask_buffers(const FA18Hunks *hunks,
                                             FA18TemplateBitmaskBuffers *buffers) {
    const FA18HunkSegment *segment;
    if (!hunks || !buffers || hunks->count <= FA18_TEMPLATE_BITMASK_HUNK) return -1;
    segment = &hunks->segments[FA18_TEMPLATE_BITMASK_HUNK];
    if (!segment->data || segment->size <= FA18_TEMPLATE_BITMASK_STREAM_C_OFFSET)
        return -1;
    return fa18_build_template_bitmask_buffers(
        segment->data + FA18_TEMPLATE_BITMASK_STREAM_A_OFFSET,
        segment->size - FA18_TEMPLATE_BITMASK_STREAM_A_OFFSET,
        segment->data + FA18_TEMPLATE_BITMASK_STREAM_B_OFFSET,
        segment->size - FA18_TEMPLATE_BITMASK_STREAM_B_OFFSET,
        segment->data + FA18_TEMPLATE_BITMASK_STREAM_C_OFFSET,
        segment->size - FA18_TEMPLATE_BITMASK_STREAM_C_OFFSET, buffers);
}
