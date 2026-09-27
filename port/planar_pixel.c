#include "planar_pixel.h"

#include <stddef.h>

static const uint16_t two_row_masks[16] = {
    0xc000, 0xc000, 0x6000, 0x3000,
    0x1800, 0x0c00, 0x0600, 0x0300,
    0x0180, 0x00c0, 0x0060, 0x0030,
    0x0018, 0x000c, 0x0006, 0x0003
};

static uint16_t pixel_mask(FA18PlanarPixelTable table, int x) {
    return table == FA18_PLANAR_PIXEL_PRIMARY ? (uint16_t)(0x8000u >> (x & 15))
                                              : two_row_masks[x & 15];
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
    const int primary_all_xor = table == FA18_PLANAR_PIXEL_PRIMARY && mode == 1u;
    const uint8_t target = table == FA18_PLANAR_PIXEL_PRIMARY && mode > 1u
        ? (uint8_t)(mode - 1u) : mode;
    for (int bit = 0; bit < 16; ++bit) {
        if (!(mask & (uint16_t)(0x8000u >> bit))) continue;
        for (int row = y; row < y + rows; ++row) {
            uint8_t *pixel = &video->pixels[(size_t)row * FA18_WIDTH + word_x + bit];
            const uint8_t preserved_fifth_lane = (uint8_t)(*pixel & 16u);
            const uint8_t low_lanes = (uint8_t)(*pixel & 15u);
            const uint8_t result = primary_all_xor
                ? (uint8_t)(low_lanes ^ active_lanes)
                : (uint8_t)((low_lanes & (uint8_t)~active_lanes) |
                            (target & active_lanes));
            *pixel = (uint8_t)(preserved_fifth_lane | result);
        }
    }
    return 0;
}
