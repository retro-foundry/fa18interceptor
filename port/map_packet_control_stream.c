#include "map_packet_control_stream.h"

int fa18_select_normal_map_packet_control_stream(
    const FA18MapPacketControlStreamInput *input,
    FA18MapPacketControlStreamResult *result) {
    if (!input || !result) return -1;
    const uint16_t x = (uint16_t)input->local_x;
    const uint16_t y = (uint16_t)input->local_y;
    const uint16_t column = (uint16_t)(3u - ((x >> 10) & 3u));
    const uint16_t row = (uint16_t)(3u - ((y >> 10) & 3u));
    const uint16_t cell = (uint16_t)(row * 4u + column);
    result->cell = cell;
    if (input->metric <= INT32_C(0x5000))
        result->stream_address = UINT32_C(0x00c2a032) + cell * 4u;
    else
        result->stream_address = UINT32_C(0x00c29f32) + cell * 16u;
    return 0;
}
