#include "five_plane_chip_binding.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t chip[0xd000] = { 0 };
    const uint32_t planes[FA18_COPPER_PAGE_PLANES] = {
        0x1000, 0x3000, 0x5000, 0x7000, 0x9000
    };
    FA18FivePlaneChipBinding binding;
    FA18FivePlanePage page;
    uint32_t lanes[4];

    fa18_five_plane_page_init(&page);
    for (unsigned plane = 0; plane < FA18_COPPER_PAGE_PLANES; ++plane)
        page.planes[plane][0] = (uint8_t)(plane + 1u);
    assert(fa18_five_plane_chip_binding_init(&binding, chip, sizeof chip, planes) == 0);
    assert(fa18_five_plane_chip_binding_store_page(&binding, &page) == 0);
    assert(chip[planes[0]] == 1 && chip[planes[4]] == 5);
    memset(&page.planes[0][0], 0, 1);
    chip[planes[0]] = 0xaa;
    assert(fa18_five_plane_chip_binding_load_page(&binding, &page) == 0);
    assert(page.planes[0][0] == 0xaa && page.planes[4][0] == 5);
    assert(fa18_five_plane_chip_binding_renderer_lane_pointers(&binding, lanes) == 0);
    assert(lanes[0] == planes[3] && lanes[1] == planes[2] &&
           lanes[2] == planes[1] && lanes[3] == planes[0]);

    uint32_t overlapping[FA18_COPPER_PAGE_PLANES];
    memcpy(overlapping, planes, sizeof planes);
    overlapping[1] = planes[0] + 2;
    assert(fa18_five_plane_chip_binding_init(&binding, chip, sizeof chip, overlapping) == -1);
    overlapping[1] = planes[1];
    overlapping[4] = sizeof chip - FA18_COPPER_PAGE_BYTES + 1u;
    assert(fa18_five_plane_chip_binding_init(&binding, chip, sizeof chip, overlapping) == -1);
    return 0;
}
