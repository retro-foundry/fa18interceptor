#include "glyph.h"

static uint16_t rol16(uint16_t value, unsigned count) {
    count &= 15u;
    if (!count) return value;
    return (uint16_t)((uint16_t)(value << count) | (value >> (16u - count)));
}

static uint32_t read_be32(const uint8_t *bytes) {
    return ((uint32_t)bytes[0] << 24) | ((uint32_t)bytes[1] << 16) |
           ((uint32_t)bytes[2] << 8) | bytes[3];
}

static void write_be32(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)(value >> 24);
    bytes[1] = (uint8_t)(value >> 16);
    bytes[2] = (uint8_t)(value >> 8);
    bytes[3] = (uint8_t)value;
}

int fa18_apply_glyph_mask_lane(FA18PlanarPage *page,
                               const FA18GlyphMaskLane *lane) {
    if (!page || !lane || lane->plane_index >= FA18_PLANES ||
        !lane->glyph_bytes || !lane->row_count ||
        lane->glyph_byte_count < lane->row_count) return -1;
    if (lane->first_byte_offset > FA18_PLANAR_PAGE_BYTES - 4u ||
        lane->row_count > 1u +
            (FA18_PLANAR_PAGE_BYTES - 4u - lane->first_byte_offset) / FA18_PLANAR_ROW_BYTES) {
        return -1;
    }

    /* `$C330FE`: D0's high nibble chooses the set form. The low word rotates
     * four bits then retains its low nibble as the longword right-shift count
     * once, before either DBRA loop body begins. */
    const int set_masked_bits = (lane->encoded_shift & 0x00f0u) != 0;
    const uint16_t shift_count = (uint16_t)(rol16(lane->encoded_shift, 4) & 0x000fu);
    for (uint16_t row = 0; row < lane->row_count; ++row) {
        uint32_t mask = (uint32_t)lane->glyph_bytes[row] << 24;
        mask >>= shift_count;
        uint8_t *destination = &page->plane[lane->plane_index]
            [lane->first_byte_offset + (size_t)row * FA18_PLANAR_ROW_BYTES];
        uint32_t value = read_be32(destination);
        value &= ~mask;
        if (set_masked_bits) value |= mask;
        write_be32(destination, value);
    }
    return 0;
}

int fa18_submit_static_glyph(FA18PlanarPage *page,
                             const FA18StaticGlyphSubmission *submission) {
    if (!page || !submission) return -1;
    for (uint8_t lane = 0; lane < FA18_PLANES; ++lane) {
        const uint8_t lane_bit = (uint8_t)(1u << (FA18_PLANES - 1u - lane));
        const uint16_t base_mode = (submission->active_lane_mask & lane_bit)
            ? 0x0bfau : 0x0b0au;
        const FA18GlyphMaskLane mask_lane = {
            (uint8_t)(FA18_PLANES - 1u - lane), submission->first_byte_offset,
            submission->glyph_bytes, submission->glyph_byte_count,
            (uint16_t)(base_mode | submission->mask_word), submission->row_count
        };
        if (fa18_apply_glyph_mask_lane(page, &mask_lane) != 0) return -1;
    }
    return 0;
}
