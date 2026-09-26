#include "display.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    /* The first byte encodes index 0..7 from left to right in the Amiga's
     * MSB-first bit order. The next byte proves the high palette bit. */
    FA18PlanarPage page;
    FA18IndexedFrameBuffer indices;
    FA18Palette palette;
    uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT];
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
    palette.rgb4[0] = 0x0000u;
    palette.rgb4[7] = 0x0d92u;
    palette.rgb4[8] = 0x0036u;
    if (fa18_apply_palette(&indices, &palette, rgb444) != 0 ||
        rgb444[0] != 0x0000u || rgb444[7] != 0x0d92u || rgb444[8] != 0x0036u) {
        fputs("RGB4 palette contract failed\n", stderr);
        return 1;
    }
    indices.pixels[0] = 16u;
    if (fa18_apply_palette(&indices, &palette, rgb444) != -1) {
        fputs("invalid palette index contract failed\n", stderr);
        return 1;
    }
    puts("planar page and RGB4 palette contract passed");
    return 0;
}
