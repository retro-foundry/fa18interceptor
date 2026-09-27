#include "map_packet_coordinate_setup.h"

#include <assert.h>

int main(void) {
    FA18MapPacketCoordinateSetupInput input = {
        0, {INT32_C(0x11223344), INT32_C(0x55667788), INT32_C(0x99aabbcc)},
        12, 0
    };
    FA18MapPacketCoordinateSetupResult result;
    FA18MapPacketCoordinateSetupRoute route;
    assert(fa18_prepare_map_packet_coordinate_setup(&input, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_COORDINATE_READY &&
           result.origin_component == (int16_t)0xa998 && result.row_min == 5 &&
           result.column_min == 13 && result.coordinate_x == INT32_C(0x33440005) &&
           result.coordinate_y == (int32_t)UINT32_C(0xbbcc000d));

    input.coordinate_bin_shift = 8;
    input.alternate_layout = 1;
    assert(fa18_prepare_map_packet_coordinate_setup(&input, &result, &route) == 0);
    assert(result.row_min == 0x11 && result.column_min == 0x99 &&
           result.coordinate_x == INT32_C(0x33440011) &&
           result.coordinate_y == (int32_t)UINT32_C(0xbbcc0099));

    input.directory_selector_gate = 1;
    assert(fa18_prepare_map_packet_coordinate_setup(&input, &result, &route) == 0 &&
           route == FA18_MAP_PACKET_COORDINATE_EXTERNAL_SELECTOR);
    return 0;
}
