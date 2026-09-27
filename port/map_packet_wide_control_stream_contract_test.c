#include "map_packet_wide_control_stream.h"

#include <assert.h>

int main(void) {
    FA18MapPacketWideControlStreamResult result;
    FA18MapPacketWideControlStreamRoute route;
    assert(fa18_select_wide_map_packet_control_stream(0x9000, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_WIDE_LOW_FILTER && !result.stream_address);
    assert(fa18_select_wide_map_packet_control_stream(0x9001, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_WIDE_MIDDLE_STREAM &&
           result.stream_address == UINT32_C(0x00c2a072));
    assert(fa18_select_wide_map_packet_control_stream(0x10000, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_WIDE_MIDDLE_STREAM);
    assert(fa18_select_wide_map_packet_control_stream(0x10001, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_WIDE_HIGH_STREAM &&
           result.stream_address == UINT32_C(0x00c2a0c2));
    return 0;
}
