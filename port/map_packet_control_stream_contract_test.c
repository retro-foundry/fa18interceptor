#include "map_packet_control_stream.h"

#include <assert.h>

int main(void) {
    FA18MapPacketControlStreamInput input = {0x1122, (int16_t)0x99aa, 0x5000};
    FA18MapPacketControlStreamResult result;
    assert(fa18_select_normal_map_packet_control_stream(&input, &result) == 0);
    assert(result.cell == 7 && result.stream_address == UINT32_C(0x00c2a04e));
    input.metric = 0x5001;
    assert(fa18_select_normal_map_packet_control_stream(&input, &result) == 0);
    assert(result.cell == 7 && result.stream_address == UINT32_C(0x00c29fa2));
    return 0;
}
