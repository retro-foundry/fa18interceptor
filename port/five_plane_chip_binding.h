#ifndef FA18_FIVE_PLANE_CHIP_BINDING_H
#define FA18_FIVE_PLANE_CHIP_BINDING_H

#include <stddef.h>
#include <stdint.h>

#include "five_plane_page.h"

/* Bridge one native five-plane page to caller-owned Chip RAM.  Plane bases
 * come from the live source pointer table/Copper state; this module never
 * chooses an Amiga address or a page-selection policy. */
typedef struct {
    uint8_t *chip_bytes;
    size_t chip_byte_count;
    uint32_t plane_pointers[FA18_COPPER_PAGE_PLANES];
} FA18FivePlaneChipBinding;

int fa18_five_plane_chip_binding_init(
    FA18FivePlaneChipBinding *binding, uint8_t *chip_bytes,
    size_t chip_byte_count,
    const uint32_t plane_pointers[FA18_COPPER_PAGE_PLANES]);

int fa18_five_plane_chip_binding_load_page(
    const FA18FivePlaneChipBinding *binding, FA18FivePlanePage *page);
int fa18_five_plane_chip_binding_store_page(
    const FA18FivePlaneChipBinding *binding, const FA18FivePlanePage *page);

/* `$C456B6` records BPL4..BPL1 at offsets +0,+4,+8,+12. */
int fa18_five_plane_chip_binding_renderer_lane_pointers(
    const FA18FivePlaneChipBinding *binding, uint32_t lane_pointers[4]);

#endif
