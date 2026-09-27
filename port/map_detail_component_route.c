#include "map_detail_component_route.h"

uint32_t fa18_complete_map_detail_component_route(int32_t coordinate_x,
                                                  int32_t coordinate_y) {
    const uint32_t x = (uint32_t)coordinate_x;
    const uint32_t swapped_x = (x << 16) | (x >> 16);
    return (swapped_x & UINT32_C(0xffff0000)) |
           ((uint32_t)coordinate_y & UINT32_C(0x0000ffff));
}
