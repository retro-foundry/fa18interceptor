#ifndef FA18_FIVE_PLANE_PAGE_H
#define FA18_FIVE_PLANE_PAGE_H

#include <stddef.h>
#include <stdint.h>

#include "copper_page.h"
#include "planar_pixel.h"

/* Native counterpart of one active `$5200` five-plane display family. The
 * pointer identities are private native keys; they are not Amiga addresses. */
typedef struct {
    uint8_t planes[FA18_COPPER_PAGE_PLANES][FA18_COPPER_PAGE_BYTES];
    FA18CopperPageState display_state;
} FA18FivePlanePage;

void fa18_five_plane_page_init(FA18FivePlanePage *page);

/* Expose only BPL1--BPL4, matching `$C2F688-$C2FA6F` ownership. */
int fa18_five_plane_page_lower_lanes(FA18FivePlanePage *page,
                                     FA18PlanarPixelPage *lanes);

/* Native display boundary for graphics.library LoadRGB4. It updates exactly
 * the leading count RGB4 registers, leaving the remainder caller-owned. */
int fa18_five_plane_page_load_rgb4(void *context, const uint16_t *palette,
                                   size_t word_count);

int fa18_five_plane_page_present(const FA18FivePlanePage *page, FA18Video *video);

#endif
