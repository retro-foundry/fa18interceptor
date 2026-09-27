#include "map_packet_pass_selector.h"

#include <assert.h>

typedef struct { int8_t row[4]; } Fixture;

static int resolve(void *context, uint32_t address, int16_t selector,
                   int8_t row[4]) {
    Fixture *fixture = context;
    (void)address;
    (void)selector;
    for (unsigned index = 0; index != 4; ++index) row[index] = fixture->row[index];
    return 0;
}

int main(void) {
    Fixture fixture = {{5, 6, 7, 8}};
    FA18MapPacketPassSelectorInput input = {
        FA18_MAP_PACKET_DIRECTORY_NORMAL,
        {0, {INT32_C(0x11223344), INT32_C(0x55667788), INT32_C(0x99aabbcc)},
         {0, 0, 0}, 0, 0},
        0, 7, 2, resolve, &fixture
    };
    FA18MapPacketPassSelectorResult result;
    assert(fa18_select_map_packet_pass(&input, &result) == 0);
    assert(result.directory.layout == FA18_MAP_PACKET_DIRECTORY_NORMAL &&
           result.coordinate.row_min == 5 &&
           result.control_stream_address == UINT32_C(0x00c2a04e));

    input.layout = FA18_MAP_PACKET_DIRECTORY_WIDE;
    input.metric = 0x10001;
    assert(fa18_select_map_packet_pass(&input, &result) == 0 &&
           result.control_stream_address == UINT32_C(0x00c2a0c2));
    input.metric = 0x300;
    input.metric_selector = 9;
    assert(fa18_select_map_packet_pass(&input, &result) == 0 &&
           result.control_stream_address == UINT32_C(0x00c2a840));
    fixture.row[0] = -1;
    assert(fa18_select_map_packet_pass(&input, &result) == 0 &&
           result.control_stream_address == UINT32_C(0x00c2aca8));
    return 0;
}
