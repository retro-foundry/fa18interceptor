#include "five_plane_page.h"

#include <assert.h>

int main(void) {
    FA18FivePlanePage page;
    FA18PlanarPixelPage lower;
    FA18Video video = { 0 };
    const uint16_t initial[] = { 0x0111, 0x0222, 0x0333 };
    const uint16_t lower_update[] = { 0x0aaa };

    fa18_five_plane_page_init(&page);
    assert(page.display_state.bplcon0 == 0x5200);
    assert(page.display_state.palette_written_mask == UINT32_MAX);
    assert(fa18_five_plane_page_lower_lanes(&page, &lower) == 0);
    assert(lower.lanes[0] == page.planes[0] && lower.lanes[3] == page.planes[3]);
    assert(lower.bytes_per_lane == FA18_COPPER_PAGE_BYTES);
    assert(fa18_five_plane_page_load_rgb4(&page, initial, 3) == 0);
    assert(page.display_state.palette[0] == 0x0111 && page.display_state.palette[2] == 0x0333);
    page.display_state.palette[1] = 0x0444;
    assert(fa18_five_plane_page_load_rgb4(&page, lower_update, 1) == 0);
    assert(page.display_state.palette[0] == 0x0aaa && page.display_state.palette[1] == 0x0444);

    page.planes[0][0] = 0x80;
    page.planes[4][0] = 0x40;
    page.display_state.palette[1] = 0x0123;
    page.display_state.palette[16] = 0x0456;
    assert(fa18_five_plane_page_present(&page, &video) == 0);
    assert(video.pixels[0] == 1 && video.palette[1] == 0x0123);
    assert(video.pixels[1] == 16 && video.palette[16] == 0x0456);
    assert(fa18_five_plane_page_load_rgb4(&page, initial, 33) == -1);
    return 0;
}
