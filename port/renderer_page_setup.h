#ifndef FA18_RENDERER_PAGE_SETUP_H
#define FA18_RENDERER_PAGE_SETUP_H

#include <stdint.h>

#include "five_plane_chip_binding.h"

enum {
    FA18_RENDERER_PAGE_SETUP_CHIP_BYTES =
        FA18_COPPER_PAGE_BYTES * FA18_COPPER_PAGE_PLANES
};

/* Native ownership for the five newly allocated `$C15ED0-$C15FA8` planes and
 * the `$C2F4DE` source/table layout derived from them. All values are private
 * offsets within `chip_bytes`, never imported Chip-RAM addresses. */
typedef struct {
    FA18FivePlanePage page;
    uint8_t chip_bytes[FA18_RENDERER_PAGE_SETUP_CHIP_BYTES];
    FA18FivePlaneChipBinding chip_binding;
    /* `$C456BE-$C456DE`: five planes, then the duplicated lower four. */
    uint32_t source[9];
    /* `$C4566E` and `$C4568E`, as ordered by `$C2F4DE-$C2F556`. */
    uint32_t table_a[8];
    uint32_t table_b[10];
} FA18RendererPageSetup;

/* `$C15DB4-$C1601E`: initialize the newly allocated native five-plane render
 * page, duplicate its lower four sources, then construct both renderer table
 * layouts. The preceding graphics/ViewPort and palette setup remains owned by
 * its separate source callbacks. */
int fa18_initialize_renderer_page_setup(FA18RendererPageSetup *setup);

#endif
