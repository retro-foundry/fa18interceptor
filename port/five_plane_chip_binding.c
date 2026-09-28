#include "five_plane_chip_binding.h"

#include <string.h>

static int plane_range_is_valid(const FA18FivePlaneChipBinding *binding,
                                uint32_t pointer) {
    return binding && pointer <= binding->chip_byte_count &&
           FA18_COPPER_PAGE_BYTES <= binding->chip_byte_count - pointer;
}

int fa18_five_plane_chip_binding_init(
    FA18FivePlaneChipBinding *binding, uint8_t *chip_bytes,
    size_t chip_byte_count,
    const uint32_t plane_pointers[FA18_COPPER_PAGE_PLANES]) {
    if (!binding || !chip_bytes || !plane_pointers) return -1;
    *binding = (FA18FivePlaneChipBinding){ chip_bytes, chip_byte_count, { 0 } };
    for (unsigned plane = 0; plane < FA18_COPPER_PAGE_PLANES; ++plane) {
        binding->plane_pointers[plane] = plane_pointers[plane];
        if (!plane_range_is_valid(binding, plane_pointers[plane])) return -1;
        for (unsigned earlier = 0; earlier < plane; ++earlier) {
            const uint32_t prior = plane_pointers[earlier];
            if (plane_pointers[plane] < prior + FA18_COPPER_PAGE_BYTES &&
                prior < plane_pointers[plane] + FA18_COPPER_PAGE_BYTES)
                return -1;
        }
    }
    return 0;
}

int fa18_five_plane_chip_binding_load_page(
    const FA18FivePlaneChipBinding *binding, FA18FivePlanePage *page) {
    if (!binding || !page) return -1;
    for (unsigned plane = 0; plane < FA18_COPPER_PAGE_PLANES; ++plane) {
        if (!plane_range_is_valid(binding, binding->plane_pointers[plane])) return -1;
        memcpy(page->planes[plane], binding->chip_bytes + binding->plane_pointers[plane],
               FA18_COPPER_PAGE_BYTES);
    }
    return 0;
}

int fa18_five_plane_chip_binding_store_page(
    const FA18FivePlaneChipBinding *binding, const FA18FivePlanePage *page) {
    if (!binding || !page) return -1;
    for (unsigned plane = 0; plane < FA18_COPPER_PAGE_PLANES; ++plane) {
        if (!plane_range_is_valid(binding, binding->plane_pointers[plane])) return -1;
        memcpy(binding->chip_bytes + binding->plane_pointers[plane], page->planes[plane],
               FA18_COPPER_PAGE_BYTES);
    }
    return 0;
}

int fa18_five_plane_chip_binding_renderer_lane_pointers(
    const FA18FivePlaneChipBinding *binding, uint32_t lane_pointers[4]) {
    if (!binding || !lane_pointers) return -1;
    for (unsigned lane = 0; lane < 4; ++lane) {
        if (!plane_range_is_valid(binding, binding->plane_pointers[3u - lane])) return -1;
        lane_pointers[lane] = binding->plane_pointers[3u - lane];
    }
    return 0;
}
