#include "display.h"
#include "run060_frame7992_area_assets.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(void) {
    assert(fa18_visible_lane_plane(0) == 3);
    assert(fa18_visible_lane_plane(1) == 2);
    assert(fa18_visible_lane_plane(2) == 1);
    assert(fa18_visible_lane_plane(3) == 0);
    assert(fa18_visible_lane_plane(4) == -1);
    assert(fa18_visible_lane_mask_to_plane_mask(0x01) == 0x08);
    assert(fa18_visible_lane_mask_to_plane_mask(0x0a) == 0x05);
    int word_x = -1, y = -1;
    assert(fa18_decode_planar_word_offset(0x0e30, &word_x, &y) == 0);
    assert(word_x == 16 && y == 90);
    assert(fa18_decode_planar_word_offset(0x0e31, &word_x, &y) == -1);
    FA18PlanarPage source_page, destination_page;
    memset(&source_page, 0, sizeof source_page);
    memset(&destination_page, 0, sizeof destination_page);
    source_page.plane[0][2 * FA18_PLANAR_ROW_BYTES + 2] = 0xA5;
    assert(fa18_blit_planar_words(&source_page, &destination_page, 1, 2, 3, 4,
                                  1, 1, 0xffff, 0xffff, 1) == 0);
    assert(destination_page.plane[0][4 * FA18_PLANAR_ROW_BYTES + 6] == 0xA5);
    assert(fa18_execute_planar_blit(&source_page, &destination_page, 0, 0, 0, 1,
                                    1, 2, 3, 4, 1, 1, 0xfc,
                                    0xffff, 0xffff) == 0);
    assert(destination_page.plane[1][4 * FA18_PLANAR_ROW_BYTES + 6] == 0xA5);

    memset(&destination_page, 0, sizeof destination_page);
    source_page.plane[3][2 * FA18_PLANAR_ROW_BYTES + 2] = 0xA5;
    assert(fa18_blit_visible_lanes(&source_page, &destination_page, 1, 2, 3, 4,
                                   1, 1, 0xffff, 0xffff, 0x01) == 0);
    assert(destination_page.plane[3][4 * FA18_PLANAR_ROW_BYTES + 6] == 0xA5);

    /* The first byte encodes index 0..7 from left to right in the Amiga's
     * MSB-first bit order. The next byte proves the high palette bit. */
    FA18PlanarPage page;
    FA18IndexedFrameBuffer indices;
    FA18Palette palette;
    uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT];
    FA18PlanarPage adjusted_page;
    FA18DisplayPagePair page_pair = {&page, &adjusted_page};
    if (fa18_select_display_page(&page_pair, 0) != &page ||
        fa18_select_display_page(&page_pair, 1) != &adjusted_page ||
        fa18_select_display_page(NULL, 0) != NULL) {
        fputs("display page selector contract failed\n", stderr);
        return 1;
    }
    if (fa18_palette_set(&palette, 4, 0x0151) != 0 ||
        palette.rgb4[4] != 0x0151 ||
        fa18_palette_set(&palette, 16, 0) != -1 ||
        fa18_palette_set(&palette, 4, 0x1000) != -1) {
        fputs("palette entry update contract failed\n", stderr);
        return 1;
    }
    memset(&page, 0, sizeof page);
    memset(&palette, 0, sizeof palette);
    for (int pixel = 0; pixel < 8; ++pixel) {
        for (int plane = 0; plane < 3; ++plane) {
            if (pixel & (1 << plane)) page.plane[plane][0] |= (uint8_t)(0x80u >> pixel);
        }
    }
    page.plane[3][1] = 0x80u;
    fa18_decode_planar_page(&page, &indices);
    for (int pixel = 0; pixel < 8; ++pixel) {
        if (indices.pixels[pixel] != (uint8_t)pixel) {
            fputs("planar low-index order contract failed\n", stderr);
            return 1;
        }
    }
    if (indices.pixels[8] != 8u || indices.pixels[9] != 0u ||
        indices.pixels[FA18_WIDTH] != 0u) {
        fputs("planar page boundary contract failed\n", stderr);
        return 1;
    }
    FA18PlanarPage roundtrip_page;
    FA18IndexedFrameBuffer roundtrip_indices;
    if (fa18_encode_planar_page(&indices, &roundtrip_page) != 0) {
        fputs("planar encoder contract failed\n", stderr);
        return 1;
    }
    fa18_decode_planar_page(&roundtrip_page, &roundtrip_indices);
    if (memcmp(indices.pixels, roundtrip_indices.pixels,
               sizeof indices.pixels) != 0) {
        fputs("planar encode/decode roundtrip failed\n", stderr);
        return 1;
    }
    palette.rgb4[0] = 0x0000u;
    palette.rgb4[7] = 0x0d92u;
    palette.rgb4[8] = 0x0036u;
    if (fa18_apply_palette(&indices, &palette, rgb444) != 0 ||
        rgb444[0] != 0x0000u || rgb444[7] != 0x0d92u || rgb444[8] != 0x0036u) {
        fputs("RGB4 palette contract failed\n", stderr);
        return 1;
    }
    memset(&page, 0, sizeof page);
    assert(fa18_apply_run060_frame7991_fill(&page) == 0);
    fa18_decode_planar_page(&page, &indices);
    assert((indices.pixels[94 * FA18_WIDTH + 144] & 1u) == 0u);
    assert((indices.pixels[94 * FA18_WIDTH + 145] & 1u) != 0u);
    assert((indices.pixels[94 * FA18_WIDTH + 208] & 1u) != 0u);
    assert((indices.pixels[94 * FA18_WIDTH + 209] & 1u) == 0u);
    assert((indices.pixels[115 * FA18_WIDTH + 2] & 1u) == 0u);
    assert((indices.pixels[115 * FA18_WIDTH + 3] & 1u) != 0u);
    assert((indices.pixels[144 * FA18_WIDTH + 319] & 1u) != 0u);
    assert((indices.pixels[145 * FA18_WIDTH] & 1u) == 0u);
    FA18AreaBlitJob jobs[4];
    assert(fa18_build_run060_frame7992_area_jobs(jobs) == 0);
    memset(&page, 0, sizeof page);
    for (size_t job = 0; job < 4; ++job) {
        assert(fa18_execute_run060_frame7992_area_job(
                   &page, &jobs[job], fa18_run060_frame7992_a_source,
                   fa18_run060_frame7992_b_source[job]) == 0);
    }
    fa18_decode_planar_page(&page, &indices);
    size_t area_pixels = 0;
    for (size_t pixel = 0; pixel < sizeof indices.pixels; ++pixel)
        if (indices.pixels[pixel] != 0u) ++area_pixels;
    assert(area_pixels > 0u);
    indices.pixels[0] = 16u;
    if (fa18_apply_palette(&indices, &palette, rgb444) != -1) {
        fputs("invalid palette index contract failed\n", stderr);
        return 1;
    }
    puts("planar page and RGB4 palette contract passed");
    return 0;
}
