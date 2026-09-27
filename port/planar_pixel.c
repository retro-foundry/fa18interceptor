#include "planar_pixel.h"

#include <stddef.h>

static const uint16_t two_row_masks[16] = {
    0xc000, 0xc000, 0x6000, 0x3000,
    0x1800, 0x0c00, 0x0600, 0x0300,
    0x0180, 0x00c0, 0x0060, 0x0030,
    0x0018, 0x000c, 0x0006, 0x0003
};

int fa18_decode_planar_pixel_state(const FA18PlanarPixelSourceState *source,
                                   FA18PlanarPixelState *state) {
    if (!source || !state) return -1;
    *state = (FA18PlanarPixelState){
        (uint8_t)source->draw_mode_word,
        source->active_plane_mask_byte,
        source->output_xor_enable_word,
        source->output_xor_plane_mask_byte
    };
    return 0;
}

static uint16_t pixel_mask(FA18PlanarPixelTable table, int x) {
    return table == FA18_PLANAR_PIXEL_PRIMARY ? (uint16_t)(0x8000u >> (x & 15))
                                              : two_row_masks[x & 15];
}

static int valid_page(const FA18PlanarPixelPage *page) {
    if (!page || page->bytes_per_lane < FA18_PLANAR_PIXEL_PAGE_BYTES) return 0;
    for (unsigned lane = 0; lane < FA18_PLANAR_PIXEL_LANES; ++lane)
        if (!page->lanes[lane]) return 0;
    return 1;
}

static uint8_t pixel_page_lanes(const FA18PlanarPixelPage *page,
                                size_t offset, uint8_t bit) {
    uint8_t lanes = 0;
    for (unsigned lane = 0; lane < FA18_PLANAR_PIXEL_LANES; ++lane)
        if (page->lanes[lane][offset] & bit) lanes |= (uint8_t)(1u << lane);
    return lanes;
}

static void set_pixel_page_lanes(FA18PlanarPixelPage *page, size_t offset,
                                 uint8_t bit, uint8_t lanes) {
    for (unsigned lane = 0; lane < FA18_PLANAR_PIXEL_LANES; ++lane) {
        if (lanes & (uint8_t)(1u << lane)) page->lanes[lane][offset] |= bit;
        else page->lanes[lane][offset] &= (uint8_t)~bit;
    }
}

static uint8_t updated_lanes(uint8_t previous, const FA18PlanarPixelState *state) {
    const uint8_t active_lanes = (uint8_t)(state->active_plane_mask & 15u);
    const uint8_t xor_lanes = (uint8_t)(state->output_xor_plane_mask & active_lanes & 15u);
    if (state->output_xor_enable >= 0 && xor_lanes)
        return (uint8_t)(previous ^ xor_lanes);
    /* `$C2F786[0]` is `$C2F826` (all clear); entries 1..15 point at
     * `$C2F83A`..`$C2F8C6`, the matching four-lane set combinations.
     * `$C2F830` is an all-XOR helper between table entries, not a selector. */
    const uint8_t target = (uint8_t)(state->draw_mode & 15u);
    return (uint8_t)((previous & (uint8_t)~active_lanes) | (target & active_lanes));
}

int fa18_apply_planar_pixel_mask(FA18Video *video,
                                 const FA18PlanarPixelState *state,
                                 FA18PlanarPixelTable table, int x, int y) {
    const int rows = table == FA18_PLANAR_PIXEL_TWO_ROWS ? 2 : 1;
    if (!video || !state ||
        (table != FA18_PLANAR_PIXEL_PRIMARY && table != FA18_PLANAR_PIXEL_TWO_ROWS) ||
        x < 0 || x >= FA18_WIDTH || y >= FA18_HEIGHT || y + rows > FA18_HEIGHT)
        return -1;
    if (y <= 0) return 1;

    const uint16_t mask = pixel_mask(table, x);
    const int word_x = x & ~15;
    for (int bit = 0; bit < 16; ++bit) {
        if (!(mask & (uint16_t)(0x8000u >> bit))) continue;
        for (int row = y; row < y + rows; ++row) {
            uint8_t *pixel = &video->pixels[(size_t)row * FA18_WIDTH + word_x + bit];
            const uint8_t preserved_fifth_lane = (uint8_t)(*pixel & 16u);
            *pixel = (uint8_t)(preserved_fifth_lane |
                               updated_lanes((uint8_t)(*pixel & 15u), state));
        }
    }
    return 0;
}

static int apply_planar_pixel_mask_to_page_rows(FA18PlanarPixelPage *page,
                                                const FA18PlanarPixelState *state,
                                                FA18PlanarPixelTable table,
                                                int x, int y, int rows) {
    if (!valid_page(page) || !state ||
        (table != FA18_PLANAR_PIXEL_PRIMARY && table != FA18_PLANAR_PIXEL_TWO_ROWS) ||
        x < 0 || x >= FA18_WIDTH || y >= FA18_HEIGHT || y + rows > FA18_HEIGHT)
        return -1;
    if (y <= 0) return 1;

    const uint16_t mask = pixel_mask(table, x);
    const int word_x = x & ~15;
    for (int bit_index = 0; bit_index < 16; ++bit_index) {
        if (!(mask & (uint16_t)(0x8000u >> bit_index))) continue;
        const int pixel_x = word_x + bit_index;
        const size_t byte_x = (size_t)pixel_x >> 3;
        const uint8_t bit = (uint8_t)(0x80u >> (pixel_x & 7));
        for (int row = y; row < y + rows; ++row) {
            const size_t offset = (size_t)row * FA18_PLANAR_PIXEL_ROW_BYTES + byte_x;
            set_pixel_page_lanes(page, offset, bit,
                                 updated_lanes(pixel_page_lanes(page, offset, bit), state));
        }
    }
    return 0;
}

int fa18_apply_planar_pixel_mask_to_page(FA18PlanarPixelPage *page,
                                         const FA18PlanarPixelState *state,
                                         FA18PlanarPixelTable table, int x, int y) {
    return apply_planar_pixel_mask_to_page_rows(
        page, state, table, x, y,
        table == FA18_PLANAR_PIXEL_TWO_ROWS ? 2 : 1);
}

int fa18_submit_primary_renderer_pixel(FA18Video *video,
                                       const FA18PlanarPixelState *state,
                                       int16_t x, int16_t y) {
    return fa18_apply_planar_pixel_mask(video, state, FA18_PLANAR_PIXEL_PRIMARY,
                                        x, y);
}

int fa18_submit_primary_renderer_pixel_to_page(FA18PlanarPixelPage *page,
                                               const FA18PlanarPixelState *state,
                                               int16_t x, int16_t y) {
    return fa18_apply_planar_pixel_mask_to_page(page, state, FA18_PLANAR_PIXEL_PRIMARY,
                                                x, y);
}

int fa18_submit_alternate_renderer_pixel(FA18Video *video,
                                         const FA18PlanarPixelState *state,
                                         int16_t x, int16_t y) {
    /* `$C2F626` loads `$C2F7C6`, but retains `$C2F786`'s one-row handlers. */
    const int rows = 1;
    if (!video || !state || x < 0 || x >= FA18_WIDTH || y < 0 ||
        y + rows > FA18_HEIGHT)
        return -1;
    if (y <= 0) return 1;

    const uint16_t mask = pixel_mask(FA18_PLANAR_PIXEL_TWO_ROWS, x);
    const int word_x = x & ~15;
    const uint8_t active_lanes = (uint8_t)(state->active_plane_mask & 15u);
    const uint8_t xor_lanes = (uint8_t)(state->output_xor_plane_mask & active_lanes & 15u);
    if (state->output_xor_enable >= 0 && xor_lanes) {
        for (int bit = 0; bit < 16; ++bit) {
            if (mask & (uint16_t)(0x8000u >> bit)) {
                uint8_t *pixel = &video->pixels[(size_t)y * FA18_WIDTH + word_x + bit];
                *pixel = (uint8_t)(*pixel ^ xor_lanes);
            }
        }
        return 0;
    }

    const uint8_t mode = (uint8_t)(state->draw_mode & 15u);
    for (int bit = 0; bit < 16; ++bit) {
        if (!(mask & (uint16_t)(0x8000u >> bit))) continue;
        uint8_t *pixel = &video->pixels[(size_t)y * FA18_WIDTH + word_x + bit];
        const uint8_t preserved_fifth_lane = (uint8_t)(*pixel & 16u);
        const uint8_t low_lanes = (uint8_t)(*pixel & 15u);
        *pixel = (uint8_t)(preserved_fifth_lane |
                           ((low_lanes & (uint8_t)~active_lanes) |
                            (mode & active_lanes)));
    }
    return 0;
}

int fa18_submit_alternate_renderer_pixel_to_page(FA18PlanarPixelPage *page,
                                                 const FA18PlanarPixelState *state,
                                                 int16_t x, int16_t y) {
    return apply_planar_pixel_mask_to_page_rows(page, state,
                                                FA18_PLANAR_PIXEL_TWO_ROWS,
                                                x, y, 1);
}

int fa18_submit_adjacent_renderer_pixels(FA18Video *video,
                                         const FA18PlanarPixelState *state,
                                         int16_t x, int16_t y) {
    if ((x & 15) != 0)
        return fa18_submit_alternate_renderer_pixel(video, state, x, y);

    const int first = fa18_submit_primary_renderer_pixel(video, state, x, y);
    if (first != 0) return first;
    /* `$C2F60A` restores D0/D1 then decrements D0 before its second BSR. */
    return fa18_submit_primary_renderer_pixel(video, state,
                                              (int16_t)((uint16_t)x - 1u), y);
}

int fa18_submit_adjacent_renderer_pixels_to_page(FA18PlanarPixelPage *page,
                                                 const FA18PlanarPixelState *state,
                                                 int16_t x, int16_t y) {
    if ((x & 15) != 0)
        return fa18_submit_alternate_renderer_pixel_to_page(page, state, x, y);
    const int first = fa18_submit_primary_renderer_pixel_to_page(page, state, x, y);
    if (first != 0) return first;
    return fa18_submit_primary_renderer_pixel_to_page(page, state,
                                                       (int16_t)((uint16_t)x - 1u), y);
}

int fa18_submit_bounded_renderer_pixels(FA18Video *video,
                                        const FA18PlanarPixelState *state,
                                        int16_t x, int16_t y,
                                        int16_t row_limit) {
    if (y >= row_limit)
        return fa18_submit_adjacent_renderer_pixels(video, state, x, y);
    return fa18_apply_planar_pixel_mask(video, state, FA18_PLANAR_PIXEL_TWO_ROWS,
                                        x, y);
}

int fa18_submit_bounded_renderer_pixels_to_page(FA18PlanarPixelPage *page,
                                                const FA18PlanarPixelState *state,
                                                int16_t x, int16_t y,
                                                int16_t row_limit) {
    if (y >= row_limit)
        return fa18_submit_adjacent_renderer_pixels_to_page(page, state, x, y);
    return fa18_apply_planar_pixel_mask_to_page(page, state, FA18_PLANAR_PIXEL_TWO_ROWS,
                                                x, y);
}

int fa18_submit_adjusted_renderer_pixels(FA18Video *video,
                                         const FA18PlanarPixelState *state,
                                         int16_t x, int16_t y,
                                         int16_t x_adjustment,
                                         int16_t y_adjustment) {
    const int16_t adjusted_x = (int16_t)((uint16_t)x + (uint16_t)x_adjustment);
    if (adjusted_x < 0 || adjusted_x >= FA18_WIDTH) return 1;
    const int16_t adjusted_y = (int16_t)((uint16_t)y + (uint16_t)y_adjustment);
    return fa18_submit_primary_renderer_pixel(video, state, adjusted_x, adjusted_y);
}

int fa18_submit_adjusted_renderer_pixels_to_page(FA18PlanarPixelPage *page,
                                                 const FA18PlanarPixelState *state,
                                                 int16_t x, int16_t y,
                                                 int16_t x_adjustment,
                                                 int16_t y_adjustment) {
    const int16_t adjusted_x = (int16_t)((uint16_t)x + (uint16_t)x_adjustment);
    if (adjusted_x < 0 || adjusted_x >= FA18_WIDTH) return 1;
    const int16_t adjusted_y = (int16_t)((uint16_t)y + (uint16_t)y_adjustment);
    return fa18_submit_primary_renderer_pixel_to_page(page, state, adjusted_x, adjusted_y);
}

int fa18_emit_primary_renderer_pixel(void *context, int16_t x, int16_t y) {
    const FA18PlanarPixelRendererContext *renderer = context;
    if (!renderer) return -1;
    return fa18_submit_primary_renderer_pixel(renderer->video, renderer->state, x, y);
}

int fa18_emit_adjacent_renderer_pixels(void *context, int16_t x, int16_t y) {
    const FA18PlanarPixelRendererContext *renderer = context;
    if (!renderer) return -1;
    return fa18_submit_adjacent_renderer_pixels(renderer->video, renderer->state, x, y);
}

int fa18_emit_primary_renderer_pixel_to_page(void *context, int16_t x, int16_t y) {
    const FA18PlanarPixelPageRendererContext *renderer = context;
    if (!renderer) return -1;
    return fa18_submit_primary_renderer_pixel_to_page(renderer->page, renderer->state, x, y);
}

int fa18_emit_adjacent_renderer_pixels_to_page(void *context, int16_t x, int16_t y) {
    const FA18PlanarPixelPageRendererContext *renderer = context;
    if (!renderer) return -1;
    return fa18_submit_adjacent_renderer_pixels_to_page(renderer->page, renderer->state, x, y);
}
