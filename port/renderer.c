#include "renderer.h"

#include <stddef.h>
#include <string.h>

/* Exact words at original $C2F7C6, confirmed in run075 frame-315 Slow RAM.
 * Index zero and one are both $C000, an intentional boundary case.
 */
static const uint16_t two_row_masks[16] = {
    0xc000, 0xc000, 0x6000, 0x3000,
    0x1800, 0x0c00, 0x0600, 0x0300,
    0x0180, 0x00c0, 0x0060, 0x0030,
    0x0018, 0x000c, 0x0006, 0x0003
};

static uint16_t pixel_mask(FA18PixelTable table, int x) {
    if (table == FA18_PIXEL_PRIMARY) return (uint16_t)(0x8000u >> (x & 15));
    return two_row_masks[x & 15];
}

void fa18_clear_renderer_work_buffer(FA18IndexedFrameBuffer *framebuffer) {
    if (framebuffer) memset(framebuffer->pixels, 0, sizeof framebuffer->pixels);
}

static int validate_pixels(const uint8_t *indices, uint16_t mask, int word_x,
                           int y, int rows) {
    for (int bit = 0; bit < 16; ++bit) {
        if (!(mask & (uint16_t)(0x8000u >> bit))) continue;
        for (int row = y; row < y + rows; ++row) {
            if (indices[(size_t)row * FA18_WIDTH + (size_t)(word_x + bit)] > 15u) {
                return 0;
            }
        }
    }
    return 1;
}

int fa18_apply_pixel_mask(FA18IndexedFrameBuffer *framebuffer,
                          const FA18RendererState *state,
                          FA18PixelTable table, int x, int y) {
    /* $C2F688 itself can address arbitrary Chip RAM. This native primitive
     * deliberately owns only the proved 320x200 visual buffer. */
    const int rows = table == FA18_PIXEL_TWO_ROWS ? 2 : 1;
    if (!framebuffer || !state ||
        (table != FA18_PIXEL_PRIMARY && table != FA18_PIXEL_TWO_ROWS) ||
        x < 0 || x >= FA18_WIDTH || y >= FA18_HEIGHT || y + rows > FA18_HEIGHT) return -1;
    if (y <= 0) return 1; /* $C2F622: D2=-1, no stores */
    const uint16_t mask = pixel_mask(table, x);
    const int word_x = x & ~15;
    const uint8_t mode = state->draw_mode & 15u;
    const uint8_t enabled_planes = state->active_plane_mask & 15u;
    const uint8_t output_mask = state->output_xor_plane_mask & 15u;
    if (!validate_pixels(framebuffer->pixels, mask, word_x, y, rows)) return -1;

    /* $C2F718-$C2F765: enabled XOR has priority and only affects row y. */
    if (state->output_xor_enable >= 0 && output_mask) {
        const uint8_t xor_lanes = (uint8_t)(output_mask & enabled_planes);
        for (int bit = 0; bit < 16; ++bit) {
            if (mask & (uint16_t)(0x8000u >> bit)) {
                size_t offset = (size_t)y * FA18_WIDTH + (size_t)(word_x + bit);
                framebuffer->pixels[offset] ^= xor_lanes;
            }
        }
        return 0;
    }

    const int primary_all_xor = table == FA18_PIXEL_PRIMARY && mode == 1u;
    const uint8_t target = table == FA18_PIXEL_PRIMARY && mode > 1u
        ? (uint8_t)(mode - 1u) : mode;
    for (int bit = 0; bit < 16; ++bit) {
        if (!(mask & (uint16_t)(0x8000u >> bit))) continue;
        const int px = word_x + bit;
        for (int row = y; row < y + rows; ++row) {
            size_t offset = (size_t)row * FA18_WIDTH + (size_t)px;
            uint8_t old = framebuffer->pixels[offset];
            if (primary_all_xor) {
                framebuffer->pixels[offset] = (uint8_t)(old ^ enabled_planes);
            } else {
                framebuffer->pixels[offset] = (uint8_t)((old & (uint8_t)~enabled_planes) |
                                                         (target & enabled_planes));
            }
        }
    }
    return 0;
}
