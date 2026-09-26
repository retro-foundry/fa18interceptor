#include "display.h"

#include <stddef.h>

uint16_t fa18_rgb4_colour(uint16_t rgb4) {
    return (uint16_t)(rgb4 & 0x0fffu);
}

void fa18_decode_planar_page(const FA18PlanarPage *page,
                             FA18IndexedFrameBuffer *framebuffer) {
    if (!page || !framebuffer) return;
    for (int y = 0; y < FA18_HEIGHT; ++y) {
        for (int byte_x = 0; byte_x < FA18_PLANAR_ROW_BYTES; ++byte_x) {
            const size_t source = (size_t)y * FA18_PLANAR_ROW_BYTES + (size_t)byte_x;
            for (int bit = 0; bit < 8; ++bit) {
                const uint8_t mask = (uint8_t)(0x80u >> bit);
                uint8_t index = 0;
                for (int plane = 0; plane < FA18_PLANES; ++plane) {
                    if (page->plane[plane][source] & mask) index |= (uint8_t)(1u << plane);
                }
                framebuffer->pixels[(size_t)y * FA18_WIDTH + (size_t)byte_x * 8u + (size_t)bit] = index;
            }
        }
    }
}

int fa18_apply_palette(const FA18IndexedFrameBuffer *framebuffer,
                       const FA18Palette *palette,
                       uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT]) {
    if (!framebuffer || !palette || !rgb444) return -1;
    for (size_t pixel = 0; pixel < FA18_WIDTH * FA18_HEIGHT; ++pixel) {
        if (framebuffer->pixels[pixel] > 15u) return -1;
        rgb444[pixel] = fa18_rgb4_colour(palette->rgb4[framebuffer->pixels[pixel]]);
    }
    return 0;
}
