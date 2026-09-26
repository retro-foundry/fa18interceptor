#include "glyph.h"

static uint32_t read_be32(const uint8_t *bytes);
static void write_be32(uint8_t *bytes, uint32_t value);

int fa18_pack_decimal_workspace(uint32_t value, uint32_t *packed_value) {
    static const uint32_t decimal_places[] = {10000000u, 1000000u,
        100000u, 10000u, 1000u, 100u, 10u, 1u};
    if (!packed_value) return -1;
    uint32_t result = 0;
    uint32_t place_bit = 0x10000000u;
    for (size_t index = 0; index < sizeof decimal_places / sizeof decimal_places[0]; ++index) {
        while (value >= decimal_places[index]) {
            value -= decimal_places[index];
            result += place_bit;
        }
        place_bit >>= 4;
    }
    *packed_value = result;
    return 0;
}

int fa18_prepare_cockpit_numeric(uint32_t raw_value, uint8_t digit_count,
                                 FA18CockpitNumericValue *value) {
    if (!value || digit_count == 0 || digit_count > 8 ||
        fa18_pack_decimal_workspace(raw_value, &value->packed_value) != 0 ||
        fa18_format_packed_decimal(value->packed_value, digit_count,
                                    value->digits) != 0) return -1;
    value->raw_value = raw_value;
    value->digit_count = digit_count;
    return 0;
}

int fa18_prepare_scaled_record_numeric(int32_t record_value,
                                       FA18CockpitNumericValue *value) {
    const int64_t shifted = ((int64_t)record_value - 0x10000000ll) >> 8;
    const int32_t scaled = (int32_t)(shifted / 0x7000ll) + 0x177;
    if (scaled < 0) return 1;
    return fa18_prepare_cockpit_numeric((uint32_t)scaled, 4u, value);
}

int fa18_update_scaled_numeric(FA18ScaledNumericState *state,
                               int32_t record_value,
                               FA18NumericDrawRequest requests[2]) {
    if (!state || !requests) return -1;
    const int64_t shifted = ((int64_t)record_value - 0x10000000ll) >> 8;
    const int32_t scaled = (int32_t)(shifted / 0x7000ll) + 0x177;
    if ((int16_t)scaled != state->previous_word) {
        state->previous_word = (int16_t)scaled;
        state->repeat_count = 2;
    } else if (state->repeat_count == 0) {
        return 0;
    } else {
        --state->repeat_count;
    }
    if (!state->disable_conversion && scaled >= 0 &&
        fa18_prepare_cockpit_numeric((uint32_t)scaled, 4u,
                                     &requests[0].value) != 0) return -1;
    requests[1].value = requests[0].value;
    requests[0].compositor_mask = 4;
    requests[1].compositor_mask = 0x0c;
    return 1;
}

int fa18_scale_record_byte(int8_t source_byte, FA18ThreeDigitScale *scale) {
    if (!scale) return -1;
    const int32_t signed_value = source_byte;
    const uint32_t magnitude = signed_value < 0 ? (uint32_t)-signed_value :
                                                        (uint32_t)signed_value;
    scale->source_byte = source_byte;
    scale->negative = signed_value < 0;
    scale->scaled_magnitude = (uint16_t)((magnitude << 8) / 0x133u);
    return 0;
}

int fa18_prepare_feet_display(int32_t record_value, int alternate_mode,
                              int override_active, int32_t override_value,
                              FA18FeetDisplayValue *display) {
    if (!display) return -1;
    int32_t selected = override_active ? override_value : (record_value >> 10) * 5;
    if (override_active && selected > 0x1869f) selected = 0x1869f;
    if (selected < 0) return 1;
    uint32_t packed = 0;
    if (fa18_pack_decimal_workspace((uint32_t)selected, &packed) != 0 ||
        fa18_format_packed_decimal(packed, 6u, display->digits) != 0) return -1;
    display->scaled_value = selected;
    display->append_feet_suffix = alternate_mode != 0;
    display->suffix[0] = 'F';
    display->suffix[1] = 'T';
    display->geometry_base = alternate_mode ? 0x1cda : 0x18ce;
    display->lane_base = alternate_mode ? 0x1a : 0x1e;
    display->compositor_shift = alternate_mode ? 0x0f3a : 0x0fca;
    display->lane_parameter = alternate_mode ? 0x000c : 0x0004;
    return 0;
}

int fa18_decode_feet_coordinate_table(const uint16_t words[12],
                                      FA18FeetCoordinateTable *table) {
    if (!words || !table) return -1;
    table->count = 6;
    for (uint8_t index = 0; index < table->count; ++index) {
        table->pair[index].position = (int16_t)words[index * 2u];
        table->pair[index].mask = words[index * 2u + 1u];
    }
    return 0;
}

int fa18_format_packed_decimal(uint32_t packed_value, uint8_t digit_count,
                               uint8_t *output) {
    if (!output || digit_count == 0 || digit_count > 8) return -1;
    for (uint8_t index = 0; index < digit_count; ++index) {
        uint8_t digit = (uint8_t)((packed_value >> (index * 4u)) & 0x0fu);
        uint8_t character = (uint8_t)('0' + digit);
        if (character > '9') character = (uint8_t)(character + 7u);
        output[digit_count - 1u - index] = character;
    }
    return 0;
}

const uint8_t *fa18_skip_leading_zero_digits(const uint8_t *digits,
                                             uint8_t digit_count) {
    if (!digits || digit_count == 0) return NULL;
    uint8_t first = 0;
    while (first + 1u < digit_count && digits[first] == '0') ++first;
    return &digits[first];
}

int fa18_merge_glyph_stream(FA18PlanarPage *page, uint8_t plane_index,
                            size_t destination_offset,
                            const uint8_t *glyph_bytes, uint16_t row_count,
                            uint8_t shift_count) {
    if (!page || plane_index >= FA18_PLANES || !glyph_bytes || row_count == 0 ||
        shift_count > 31u || destination_offset > FA18_PLANAR_PAGE_BYTES - 4u ||
        (size_t)(row_count - 1u) * FA18_PLANAR_ROW_BYTES >
            FA18_PLANAR_PAGE_BYTES - 4u - destination_offset) return -1;

    const uint32_t mask = 0xe0000000u >> shift_count;
    for (uint16_t row = 0; row < row_count; ++row) {
        const size_t offset = destination_offset +
            (size_t)row * FA18_PLANAR_ROW_BYTES;
        uint8_t *destination = &page->plane[plane_index][offset];
        uint32_t old = read_be32(destination);
        /* The source byte is loaded into the low byte, then ROR.L #8 moves it
         * into the high byte before the shared right shift. */
        const uint32_t rotated = (uint32_t)glyph_bytes[row] << 24;
        const uint32_t shifted = rotated >> shift_count;
        write_be32(destination, (shifted & mask) | (old & ~mask));
    }
    return 0;
}

int fa18_select_glyph(const FA18GlyphTable *table, uint8_t character,
                      const uint8_t **glyph_stream,
                      size_t *remaining_bytes) {
    if (!table || !glyph_stream || !remaining_bytes || character < 0x20u) {
        return -1;
    }
    const size_t index = (size_t)(character - 0x20u);
    if (!table->offsets || index >= table->offset_count ||
        !table->glyph_bytes) return -1;
    const size_t offset = table->offsets[index];
    if (offset > table->glyph_byte_count) return -1;
    *glyph_stream = &table->glyph_bytes[offset];
    *remaining_bytes = table->glyph_byte_count - offset;
    return 0;
}

int fa18_prepare_glyph_placement(int16_t lane_base,
                                 int16_t doubled_render_lane,
                                 int16_t glyph_position,
                                 int32_t geometry_base,
                                 int32_t selected_pointer_value,
                                 uint16_t compositor_shift,
                                 FA18GlyphPlacement *placement) {
    if (!placement) return -1;
    const int32_t coordinate = (int32_t)lane_base +
        (int32_t)doubled_render_lane + (int32_t)glyph_position;
    if (coordinate < 0 || coordinate >= 0x28) return 1;
    placement->visible_coordinate = coordinate;
    placement->destination_offset = geometry_base + selected_pointer_value +
        (int32_t)doubled_render_lane + (int32_t)glyph_position;
    placement->compositor_shift = compositor_shift;
    return 0;
}

int fa18_prepare_plane_glyph_placement(uint8_t plane_index,
                                       int16_t lane_base,
                                       int16_t doubled_render_lane,
                                       const FA18GlyphCoordinatePair *pair,
                                       int16_t geometry_base,
                                       FA18PlaneGlyphPlacement *placement) {
    if (!pair || !placement || plane_index >= FA18_PLANES) return -1;
    const int32_t visible = (int32_t)lane_base + doubled_render_lane + pair->position;
    if (visible < 0 || visible >= 0x28) return 1;
    const int32_t destination = (int32_t)geometry_base + doubled_render_lane + pair->position;
    if (destination < 0 || destination > FA18_PLANAR_PAGE_BYTES - 4 ||
        (destination & 1) != 0) return 1;
    placement->plane_index = plane_index;
    placement->destination_offset = destination;
    placement->visible_coordinate = visible;
    placement->mask = pair->mask;
    return 0;
}

int fa18_render_glyph(FA18PlanarPage *page, const FA18GlyphTable *table,
                      uint8_t character, uint8_t plane_index,
                      size_t destination_offset, uint8_t shift_count,
                      uint16_t row_count) {
    const uint8_t *glyph_stream = NULL;
    size_t remaining = 0;
    if (fa18_select_glyph(table, character, &glyph_stream, &remaining) != 0 ||
        remaining < row_count) return -1;
    return fa18_merge_glyph_stream(page, plane_index, destination_offset,
                                   glyph_stream, row_count, shift_count);
}

int fa18_render_placed_glyph(FA18PlanarPage *page, const FA18GlyphTable *table,
                             uint8_t character,
                             const FA18PlaneGlyphPlacement *placement,
                             uint8_t shift_count, uint16_t row_count) {
    if (!placement || placement->destination_offset < 0) return -1;
    return fa18_render_glyph(page, table, character, placement->plane_index,
                             (size_t)placement->destination_offset,
                             shift_count, row_count);
}

int fa18_render_numeric_glyphs(FA18PlanarPage *page,
                               const FA18GlyphTable *table,
                               const FA18CockpitNumericValue *value,
                               const FA18FeetCoordinateTable *coordinates,
                               uint8_t plane_index, int16_t lane_base,
                               int16_t doubled_render_lane,
                               int16_t geometry_base, uint8_t shift_count,
                               uint16_t row_count) {
    if (!page || !table || !value || !coordinates ||
        value->digit_count > coordinates->count) return -1;
    for (uint8_t index = 0; index < value->digit_count; ++index) {
        FA18PlaneGlyphPlacement placement;
        int status = fa18_prepare_plane_glyph_placement(
            plane_index, lane_base, doubled_render_lane,
            &coordinates->pair[index], geometry_base, &placement);
        if (status == 1) continue;
        if (status != 0 || fa18_render_placed_glyph(
                page, table, value->digits[index], &placement,
                shift_count, row_count) != 0) return -1;
    }
    return 0;
}

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
