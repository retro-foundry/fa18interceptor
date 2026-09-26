#ifndef FA18_DISPLAY_H
#define FA18_DISPLAY_H

#include "renderer.h"

#include <stdint.h>

enum { FA18_PLANES = 4, FA18_PLANAR_ROW_BYTES = FA18_WIDTH / 8,
       FA18_PLANAR_PAGE_BYTES = FA18_PLANAR_ROW_BYTES * FA18_HEIGHT };

/* Native four-plane page used only at the display boundary. It has no Chip-RAM
 * addresses, Copper list, or hardware register state. Plane zero is the low
 * bit of the resulting chunky colour index. */
typedef struct {
    uint8_t plane[FA18_PLANES][FA18_PLANAR_PAGE_BYTES];
} FA18PlanarPage;

/* Amiga RGB4 words, held as palette state rather than COLORxx registers. */
typedef struct {
    uint16_t rgb4[16];
} FA18Palette;

/* Convert one 12-bit Amiga RGB4 word to the port's RGB444 pixel format. */
uint16_t fa18_rgb4_colour(uint16_t rgb4);

/* Deplanarize an observed 320x200, four-plane display page into the port's
 * native indexed render target. */
void fa18_decode_planar_page(const FA18PlanarPage *page,
                             FA18IndexedFrameBuffer *framebuffer);

/* Translate an indexed native framebuffer through named palette state. */
int fa18_apply_palette(const FA18IndexedFrameBuffer *framebuffer,
                       const FA18Palette *palette,
                       uint16_t rgb444[FA18_WIDTH * FA18_HEIGHT]);

#endif
