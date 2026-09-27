#include "planar_pixel.h"

#include <assert.h>
#include <string.h>

static uint8_t page_pixel(const uint8_t lanes[FA18_PLANAR_PIXEL_LANES]
                                           [FA18_PLANAR_PIXEL_PAGE_BYTES],
                          int x, int y) {
    const size_t offset = (size_t)y * FA18_PLANAR_PIXEL_ROW_BYTES + (unsigned)x / 8u;
    const uint8_t bit = (uint8_t)(0x80u >> (x & 7));
    uint8_t value = 0;
    for (unsigned lane = 0; lane < FA18_PLANAR_PIXEL_LANES; ++lane)
        if (lanes[lane][offset] & bit) value |= (uint8_t)(1u << lane);
    return value;
}

int main(void) {
    FA18Video video;
    memset(&video, 0, sizeof video);
    FA18PlanarPixelState state = { 13, 15, -1, 0 };
    {
        const FA18PlanarPixelSourceState source = { 0x100b, 0x0f, -1, 0 };
        assert(fa18_decode_planar_pixel_state(&source, &state) == 0);
        assert(state.draw_mode == 0x0b && state.active_plane_mask == 0x0f &&
               state.output_xor_enable == -1 && state.output_xor_plane_mask == 0);
        assert(fa18_decode_planar_pixel_state(0, &state) == -1);
        assert(fa18_decode_planar_pixel_state(&source, 0) == -1);
    }
    state = (FA18PlanarPixelState){ 13, 15, -1, 0 };

    video.pixels[125 * FA18_WIDTH + 100] = 0x12;
    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, 100, 125) == 0);
    assert(video.pixels[125 * FA18_WIDTH + 100] == 0x1c);

    memset(&video, 0, sizeof video);
    state = (FA18PlanarPixelState){ 11, 15, -1, 0 };
    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_TWO_ROWS, 172, 99) == 0);
    assert(video.pixels[99 * FA18_WIDTH + 172] == 11);
    assert(video.pixels[100 * FA18_WIDTH + 172] == 11);

    video.pixels[125 * FA18_WIDTH + 100] = 0x12;
    state = (FA18PlanarPixelState){ 13, 15, 0, 5 };
    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, 100, 125) == 0);
    assert(video.pixels[125 * FA18_WIDTH + 100] == 0x17);

    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, 0, 0) == 1);
    assert(fa18_apply_planar_pixel_mask(&video, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, FA18_WIDTH, 1) == -1);
    assert(fa18_apply_planar_pixel_mask(0, &state,
                                        FA18_PLANAR_PIXEL_PRIMARY, 1, 1) == -1);

    memset(&video, 0, sizeof video);
    state = (FA18PlanarPixelState){ 11, 15, -1, 0 };
    assert(fa18_submit_primary_renderer_pixel(&video, &state, 100, 125) == 0);
    assert(video.pixels[125 * FA18_WIDTH + 100] == 10);

    /* `$C2F786[1]` is `$C2F83A`, the lane-0-set handler. The adjacent
     * `$C2F830` all-XOR helper is not a dispatch-table entry. */
    memset(&video, 0, sizeof video);
    state = (FA18PlanarPixelState){ 1, 15, -1, 0 };
    assert(fa18_submit_primary_renderer_pixel(&video, &state, 100, 125) == 0);
    assert(video.pixels[125 * FA18_WIDTH + 100] == 1);

    memset(&video, 0, sizeof video);
    state = (FA18PlanarPixelState){ 11, 15, -1, 0 };
    assert(fa18_submit_alternate_renderer_pixel(&video, &state, 172, 99) == 0);
    assert(video.pixels[99 * FA18_WIDTH + 171] == 11);
    assert(video.pixels[99 * FA18_WIDTH + 172] == 11);
    assert(video.pixels[100 * FA18_WIDTH + 171] == 0);

    memset(&video, 0, sizeof video);
    assert(fa18_submit_adjacent_renderer_pixels(&video, &state, 160, 99) == 0);
    assert(video.pixels[99 * FA18_WIDTH + 159] == 10);
    assert(video.pixels[99 * FA18_WIDTH + 160] == 10);

    memset(&video, 0, sizeof video);
    assert(fa18_submit_bounded_renderer_pixels(&video, &state, 172, 99, 179) == 0);
    assert(video.pixels[99 * FA18_WIDTH + 171] == 11);
    assert(video.pixels[100 * FA18_WIDTH + 171] == 11);
    memset(&video, 0, sizeof video);
    assert(fa18_submit_bounded_renderer_pixels(&video, &state, 172, 179, 179) == 0);
    assert(video.pixels[179 * FA18_WIDTH + 171] == 11);
    assert(video.pixels[180 * FA18_WIDTH + 171] == 0);

    memset(&video, 0, sizeof video);
    assert(fa18_submit_adjusted_renderer_pixels(&video, &state, 99, 124, 1, 1) == 0);
    assert(video.pixels[125 * FA18_WIDTH + 100] == 10);
    assert(fa18_submit_adjusted_renderer_pixels(&video, &state, -1, 99, 0, 0) == 1);
    assert(fa18_submit_adjusted_renderer_pixels(&video, &state, 319, 99, 1, 0) == 1);

    memset(&video, 0, sizeof video);
    const FA18PlanarPixelRendererContext renderer = { &video, &state };
    assert(fa18_emit_primary_renderer_pixel(&renderer, 100, 125) == 0);
    assert(video.pixels[125 * FA18_WIDTH + 100] == 10);
    assert(fa18_emit_adjacent_renderer_pixels(&renderer, 160, 99) == 0);
    assert(video.pixels[99 * FA18_WIDTH + 159] == 10);
    assert(video.pixels[99 * FA18_WIDTH + 160] == 10);
    assert(fa18_emit_primary_renderer_pixel(0, 100, 125) == -1);

    uint8_t lanes[FA18_PLANAR_PIXEL_LANES][FA18_PLANAR_PIXEL_PAGE_BYTES] = {{0}};
    FA18PlanarPixelPage page = {
        { lanes[0], lanes[1], lanes[2], lanes[3] }, sizeof lanes[0]
    };
    memset(&video, 0, sizeof video);
    state = (FA18PlanarPixelState){ 11, 15, -1, 0 };
    assert(fa18_submit_primary_renderer_pixel(&video, &state, 100, 125) ==
           fa18_submit_primary_renderer_pixel_to_page(&page, &state, 100, 125));
    assert(fa18_submit_alternate_renderer_pixel(&video, &state, 172, 99) ==
           fa18_submit_alternate_renderer_pixel_to_page(&page, &state, 172, 99));
    assert(fa18_submit_adjacent_renderer_pixels(&video, &state, 160, 99) ==
           fa18_submit_adjacent_renderer_pixels_to_page(&page, &state, 160, 99));
    assert(fa18_submit_bounded_renderer_pixels(&video, &state, 172, 99, 179) ==
           fa18_submit_bounded_renderer_pixels_to_page(&page, &state, 172, 99, 179));
    assert(fa18_submit_adjusted_renderer_pixels(&video, &state, 99, 124, 1, 1) ==
           fa18_submit_adjusted_renderer_pixels_to_page(&page, &state, 99, 124, 1, 1));
    for (int y = 0; y < FA18_HEIGHT; ++y)
        for (int x = 0; x < FA18_WIDTH; ++x)
            assert((video.pixels[y * FA18_WIDTH + x] & 15u) == page_pixel(lanes, x, y));
    assert(fa18_apply_planar_pixel_mask_to_page(&page, &state,
                                                FA18_PLANAR_PIXEL_PRIMARY, 0, 0) == 1);
    page.bytes_per_lane = FA18_PLANAR_PIXEL_PAGE_BYTES - 1u;
    assert(fa18_submit_primary_renderer_pixel_to_page(&page, &state, 1, 1) == -1);
    page.bytes_per_lane = sizeof lanes[0];
    const FA18PlanarPixelPageRendererContext page_renderer = { &page, &state };
    assert(fa18_emit_primary_renderer_pixel_to_page(&page_renderer, 100, 125) == 0);
    assert(fa18_emit_adjacent_renderer_pixels_to_page(&page_renderer, 160, 99) == 0);
    assert(fa18_emit_primary_renderer_pixel_to_page(0, 100, 125) == -1);
    return 0;
}
