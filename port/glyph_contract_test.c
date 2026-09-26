#include "glyph.h"
#include "run075_font_asset.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    uint32_t packed = 0;
    if (fa18_pack_decimal_workspace(375u, &packed) != 0 || packed != 0x375u ||
        fa18_pack_decimal_workspace(0u, &packed) != 0 || packed != 0u ||
        fa18_pack_decimal_workspace(1u, NULL) != -1) {
        fputs("workspace packed-decimal contract failed\n", stderr);
        return 1;
    }
    FA18CockpitNumericValue numeric;
    if (fa18_prepare_cockpit_numeric(375u, 4u, &numeric) != 0 ||
        numeric.raw_value != 375u || numeric.packed_value != 0x375u ||
        memcmp(numeric.digits, "0375", 4u) != 0) {
        fputs("cockpit numeric composition contract failed\n", stderr);
        return 1;
    }
    if (fa18_prepare_scaled_record_numeric(0x10000000, &numeric) != 0 ||
        numeric.raw_value != 375u || numeric.packed_value != 0x375u ||
        memcmp(numeric.digits, "0375", 4u) != 0 ||
        fa18_prepare_scaled_record_numeric(0, &numeric) != 0 ||
        numeric.raw_value != 339u) {
        fputs("scaled record numeric contract failed\n", stderr);
        return 1;
    }
    FA18ScaledNumericState numeric_state = {0, 0, 0};
    FA18NumericDrawRequest requests[2];
    if (fa18_update_scaled_numeric(&numeric_state, 0x10000000,
                                   requests) != 1 ||
        requests[0].compositor_mask != 4 || requests[1].compositor_mask != 0x0c ||
        fa18_update_scaled_numeric(&numeric_state, 0x10000000,
                                   requests) != 1 ||
        fa18_update_scaled_numeric(&numeric_state, 0x10000000,
                                   requests) != 1 ||
        fa18_update_scaled_numeric(&numeric_state, 0x10000000,
                                   requests) != 0) {
        fputs("scaled numeric redraw contract failed\n", stderr);
        return 1;
    }
    FA18ThreeDigitScale three_digit;
    if (fa18_scale_record_byte(19, &three_digit) != 0 ||
        three_digit.source_byte != 19 || three_digit.negative != 0 ||
        three_digit.scaled_magnitude != (uint16_t)((19u << 8) / 0x133u) ||
        fa18_scale_record_byte(-19, &three_digit) != 0 ||
        three_digit.negative != 1 || three_digit.scaled_magnitude != (uint16_t)((19u << 8) / 0x133u)) {
        fputs("three-digit scale contract failed\n", stderr);
        return 1;
    }
    FA18FeetDisplayValue feet;
    if (fa18_prepare_feet_display(0x7708, 1, 0, 0, &feet) != 0 ||
        feet.scaled_value != 145 || feet.append_feet_suffix != 1 ||
        feet.geometry_base != 0x1cda || feet.lane_base != 0x1a ||
        feet.compositor_shift != 0x0f3a || feet.lane_parameter != 0x000c ||
        memcmp(feet.digits, "000145", 6u) != 0 ||
        fa18_prepare_feet_display(0x400, 0, 0, 0, &feet) != 0 ||
        feet.scaled_value != 5 || feet.append_feet_suffix != 0 ||
        feet.geometry_base != 0x18ce || feet.lane_base != 0x1e ||
        feet.compositor_shift != 0x0fca || feet.lane_parameter != 4 ||
        memcmp(feet.digits, "000005", 6u) != 0 ||
        fa18_prepare_feet_display(0, 1, 1, 0x20000, &feet) != 0 ||
        feet.scaled_value != 0x1869f || feet.append_feet_suffix != 1 ||
        memcmp(feet.suffix, "FT", 2u) != 0 ||
        fa18_prepare_feet_display(-0x400, 0, 0, 0, &feet) != 1) {
        fputs("feet display contract failed\n", stderr);
        return 1;
    }
    static const uint16_t feet_words[12] = {
        0, 0, 0, 64, 0x4000, 0, 0, 128, 0x8000, 0,
        0, 224
    };
    FA18FeetCoordinateTable coordinates;
    if (fa18_decode_feet_coordinate_table(feet_words, &coordinates) != 0 ||
        coordinates.count != 6 || coordinates.pair[1].position != 0 ||
        coordinates.pair[1].mask != 64 ||
        coordinates.pair[2].position != 0x4000 ||
        coordinates.pair[2].mask != 0) {
        fputs("feet coordinate table contract failed\n", stderr);
        return 1;
    }
    FA18PlaneGlyphPlacement plane_placement;
    if (fa18_prepare_plane_glyph_placement(2, 0x1e, 0,
                                           &coordinates.pair[0], 0x18ce,
                                           &plane_placement) != 0 ||
        plane_placement.plane_index != 2 ||
        plane_placement.destination_offset != 0x18ce ||
        plane_placement.visible_coordinate != 0x1e ||
        fa18_prepare_plane_glyph_placement(4, 0, 0,
                                           &coordinates.pair[0], 0,
                                           &plane_placement) != -1) {
        fputs("plane glyph placement contract failed\n", stderr);
        return 1;
    }
    uint8_t formatted[8] = {0};
    if (fa18_format_packed_decimal(0x0171u, 4u, formatted) != 0 ||
        memcmp(formatted, "0171", 4u) != 0) {
        fputs("packed decimal formatter contract failed\n", stderr);
        return 1;
    }
    const uint8_t *digits = fa18_skip_leading_zero_digits(formatted, 4u);
    if (!digits || digits != &formatted[1] || *digits != '1') {
        fputs("packed decimal retained-digit contract failed\n", stderr);
        return 1;
    }
    if (fa18_format_packed_decimal(0xab9fu, 4u, formatted) != 0 ||
        memcmp(formatted, "AB9F", 4u) != 0) {
        fputs("packed hexadecimal nibble contract failed\n", stderr);
        return 1;
    }
    if (fa18_format_packed_decimal(0x0007u, 4u, formatted) != 0) {
        fputs("packed decimal second format failed\n", stderr);
        return 1;
    }
    digits = fa18_skip_leading_zero_digits(formatted, 4u);
    if (!digits || digits != &formatted[3] || *digits != '7') {
        fputs("packed decimal leading-zero contract failed\n", stderr);
        return 1;
    }

    FA18PlanarPage merge_page;
    memset(&merge_page, 0, sizeof merge_page);
    const uint8_t merge_bytes[2] = {0xf0u, 0x80u};
    if (fa18_merge_glyph_stream(&merge_page, 1u, 0u, merge_bytes, 2u, 0u) != 0 ||
        merge_page.plane[1][0] != 0xe0u ||
        merge_page.plane[1][1] != 0x00u ||
        merge_page.plane[1][FA18_PLANAR_ROW_BYTES] != 0x80u) {
        fputs("strided glyph merge contract failed\n", stderr);
        return 1;
    }

    const uint16_t glyph_offsets[2] = {2u, 4u};
    const uint8_t glyph_stream[6] = {0xaa, 0xbb, 0xc1, 0xc2, 0xd1, 0xd2};
    const FA18GlyphTable glyph_table = {
        glyph_offsets, 2u, glyph_stream, sizeof glyph_stream
    };
    const uint8_t *selected = NULL;
    size_t remaining = 0;
    if (fa18_select_glyph(&glyph_table, 0x20u, &selected, &remaining) != 0 ||
        selected != &glyph_stream[2] || remaining != 4u ||
        fa18_select_glyph(&glyph_table, 0x21u, &selected, &remaining) != 0 ||
        selected != &glyph_stream[4] || remaining != 2u ||
        fa18_select_glyph(&glyph_table, 0x22u, &selected, &remaining) != -1) {
        fputs("glyph offset table contract failed\n", stderr);
        return 1;
    }

    const FA18GlyphTable run075_font = {
        fa18_run075_font_offsets, 436u,
        fa18_run075_font_bytes, sizeof fa18_run075_font_bytes
    };
    if (fa18_select_glyph(&run075_font, (uint8_t)'1', &selected, &remaining) != 0 ||
        selected != &fa18_run075_font_bytes[0x00b7] || remaining != 872u - 0x00b7u) {
        fputs("run075 font asset lookup contract failed\n", stderr);
        return 1;
    }
    memset(&merge_page, 0, sizeof merge_page);
    if (fa18_render_glyph(&merge_page, &run075_font, (uint8_t)'1', 0u,
                          0u, 0u, 6u) != 0 ||
        (merge_page.plane[0][0] | merge_page.plane[0][1] |
         merge_page.plane[0][FA18_PLANAR_ROW_BYTES]) == 0u) {
        fputs("run075 glyph render contract failed\n", stderr);
        return 1;
    }

    FA18GlyphPlacement placement;
    if (fa18_prepare_glyph_placement(2, 4, 3, 0x100, 0x20, 7, &placement) != 0 ||
        placement.visible_coordinate != 9 ||
        placement.destination_offset != 0x127 ||
        placement.compositor_shift != 7 ||
        fa18_prepare_glyph_placement(-8, 0, 0, 0, 0, 0, &placement) != 1) {
        fputs("glyph placement arithmetic contract failed\n", stderr);
        return 1;
    }

    FA18PlanarPage page;
    const uint8_t glyph[2] = {0xf0u, 0x80u};
    memset(&page, 0xff, sizeof page);

    /* `$0B0A` takes the clear form. With shift zero, F0 clears the high
     * nibble of the first big-endian destination longword. */
    FA18GlyphMaskLane clear_lane = {2u, 0u, glyph, 2u, 0x0b0au, 2u};
    if (fa18_apply_glyph_mask_lane(&page, &clear_lane) != 0 ||
        page.plane[2][0] != 0x0fu || page.plane[2][1] != 0xffu ||
        page.plane[2][FA18_PLANAR_ROW_BYTES] != 0x7fu) {
        fputs("glyph clear-mask contract failed\n", stderr);
        return 1;
    }

    memset(&page, 0, sizeof page);
    /* `$0BFA` takes the complementary set form using the same source bytes. */
    FA18GlyphMaskLane set_lane = {1u, 4u, glyph, 2u, 0x0bfau, 2u};
    if (fa18_apply_glyph_mask_lane(&page, &set_lane) != 0 ||
        page.plane[1][4] != 0xf0u ||
        page.plane[1][4 + FA18_PLANAR_ROW_BYTES] != 0x80u) {
        fputs("glyph set-mask contract failed\n", stderr);
        return 1;
    }

    /* The first iteration may derive a nonzero source shift from bits 12..15. */
    memset(&page, 0xff, sizeof page);
    const uint8_t shifted[1] = {0x80u};
    FA18GlyphMaskLane shifted_lane = {0u, 0u, shifted, 1u, 0xa001u, 1u};
    if (fa18_apply_glyph_mask_lane(&page, &shifted_lane) != 0 ||
        page.plane[0][0] != 0xffu || page.plane[0][1] != 0xdfu) {
        fputs("glyph shift contract failed\n", stderr);
        return 1;
    }

    /* run060 frame-9285, `LANDING SUCCESSFUL` compositor: live `$C330FE`
     * entry has D1=$018E3A (plane 4 + $04BA), D2=$FBFA, D4's seven source
     * bytes 80 80 80 C0 C0 C0 F8, and D7=$01C2. The bounded trace's final
     * Chip-RAM words are listed row-for-row below. */
    memset(&page, 0, sizeof page);
    const uint8_t run060_glyph[7] = {0x80u, 0x80u, 0x80u, 0xc0u,
                                      0xc0u, 0xc0u, 0xf8u};
    FA18GlyphMaskLane run060_lane = {3u, 0x04bau, run060_glyph,
                                     7u, 0xfbfau, 7u};
    const uint32_t expected_words[7] = {0x00010000u, 0x00010000u,
                                        0x00010000u, 0x00018000u,
                                        0x00018000u, 0x00018000u,
                                        0x0001f000u};
    if (fa18_apply_glyph_mask_lane(&page, &run060_lane) != 0) {
        fputs("run060 glyph fixture rejected\n", stderr);
        return 1;
    }
    for (size_t row = 0; row < 7; ++row) {
        const size_t offset = 0x04bau + row * FA18_PLANAR_ROW_BYTES;
        const uint32_t actual = ((uint32_t)page.plane[3][offset] << 24) |
            ((uint32_t)page.plane[3][offset + 1] << 16) |
            ((uint32_t)page.plane[3][offset + 2] << 8) |
            page.plane[3][offset + 3];
        if (actual != expected_words[row]) {
            fputs("run060 glyph trace fixture failed\n", stderr);
            return 1;
        }
    }

    /* The enclosing run060 `$C33058` packet immediately submits all four
     * lanes at that same offset. It maps mask bits 3..0 to planes 4..1. */
    memset(&page, 0, sizeof page);
    for (size_t row = 0; row < 7; ++row) {
        const size_t offset = 0x04bau + row * FA18_PLANAR_ROW_BYTES;
        memset(&page.plane[2][offset], 0xff, 4);
        memset(&page.plane[0][offset], 0xff, 4);
    }
    const FA18StaticGlyphSubmission run060_submission = {
        run060_glyph, 7u, 0x04bau, 0xf000u, 0x09u, 7u
    };
    if (fa18_submit_static_glyph(&page, &run060_submission) != 0) {
        fputs("run060 four-lane submission rejected\n", stderr);
        return 1;
    }
    for (size_t row = 0; row < 7; ++row) {
        const size_t offset = 0x04bau + row * FA18_PLANAR_ROW_BYTES;
        const uint32_t plane4 = ((uint32_t)page.plane[3][offset] << 24) |
            ((uint32_t)page.plane[3][offset + 1] << 16) |
            ((uint32_t)page.plane[3][offset + 2] << 8) | page.plane[3][offset + 3];
        const uint32_t plane3 = ((uint32_t)page.plane[2][offset] << 24) |
            ((uint32_t)page.plane[2][offset + 1] << 16) |
            ((uint32_t)page.plane[2][offset + 2] << 8) | page.plane[2][offset + 3];
        if (plane4 != expected_words[row] || plane3 != ~expected_words[row] ||
            page.plane[1][offset] != 0u || page.plane[0][offset] != 0xffu) {
            fputs("run060 four-lane trace fixture failed\n", stderr);
            return 1;
        }
    }

    set_lane.first_byte_offset = FA18_PLANAR_PAGE_BYTES - 3u;
    if (fa18_apply_glyph_mask_lane(&page, &set_lane) != -1) {
        fputs("glyph bounds contract failed\n", stderr);
        return 1;
    }
    puts("glyph mask lane contract passed");
    return 0;
}
