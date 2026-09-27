#include "five_plane_page.h"

#include <string.h>

void fa18_five_plane_page_init(FA18FivePlanePage *page) {
    if (!page) return;
    memset(page, 0, sizeof *page);
    page->display_state.bplcon0 = 0x5200;
    page->display_state.palette_written_mask = UINT32_MAX;
    for (unsigned plane = 0; plane < FA18_COPPER_PAGE_PLANES; ++plane)
        page->display_state.plane_pointers[plane] = plane + 1u;
}

int fa18_five_plane_page_lower_lanes(FA18FivePlanePage *page,
                                     FA18PlanarPixelPage *lanes) {
    if (!page || !lanes) return -1;
    for (unsigned lane = 0; lane < FA18_PLANAR_PIXEL_LANES; ++lane)
        lanes->lanes[lane] = page->planes[lane];
    lanes->bytes_per_lane = FA18_COPPER_PAGE_BYTES;
    return 0;
}

int fa18_five_plane_page_load_rgb4(void *context, const uint16_t *palette,
                                   size_t word_count) {
    FA18FivePlanePage *page = context;
    if (!page || !palette || !word_count || word_count > 32u) return -1;
    for (size_t colour = 0; colour < word_count; ++colour)
        page->display_state.palette[colour] = (uint16_t)(palette[colour] & 0x0fffu);
    return 0;
}

int fa18_five_plane_page_present(const FA18FivePlanePage *page, FA18Video *video) {
    FA18CopperPlaneBuffer buffers[FA18_COPPER_PAGE_PLANES];
    if (!page || !video) return -1;
    for (unsigned plane = 0; plane < FA18_COPPER_PAGE_PLANES; ++plane) {
        buffers[plane].source_pointer = plane + 1u;
        buffers[plane].bytes = page->planes[plane];
        buffers[plane].byte_count = FA18_COPPER_PAGE_BYTES;
    }
    return fa18_present_copper_page(&page->display_state, buffers,
                                    FA18_COPPER_PAGE_PLANES, video);
}
