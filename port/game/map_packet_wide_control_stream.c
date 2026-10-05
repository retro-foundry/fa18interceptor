#include "map_packet_wide_control_stream.h"

int fa18_select_wide_map_packet_control_stream(
    int32_t metric, FA18MapPacketWideControlStreamResult *result,
    FA18MapPacketWideControlStreamRoute *route) {
    if (!result || !route) return -1;
    result->stream_address = 0;
    if (metric <= INT32_C(0x9000)) {
        *route = FA18_MAP_PACKET_WIDE_LOW_FILTER;
    } else if (metric <= INT32_C(0x10000)) {
        result->stream_address = UINT32_C(0x00c2a072);
        *route = FA18_MAP_PACKET_WIDE_MIDDLE_STREAM;
    } else {
        result->stream_address = UINT32_C(0x00c2a0c2);
        *route = FA18_MAP_PACKET_WIDE_HIGH_STREAM;
    }
    return 0;
}
