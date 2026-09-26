#include "glyph.h"

#include <stdio.h>
#include <string.h>

int main(void) {
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
