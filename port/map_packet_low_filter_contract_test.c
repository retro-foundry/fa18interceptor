#include "map_packet_low_filter.h"

#include <assert.h>

typedef struct { uint32_t address; int16_t selector; int8_t row[4]; } Fixture;

static int resolve(void *context, uint32_t address, int16_t selector,
                   int8_t row[4]) {
    Fixture *fixture = context;
    fixture->address = address;
    fixture->selector = selector;
    for (unsigned index = 0; index != 4; ++index) row[index] = fixture->row[index];
    return 0;
}

int main(void) {
    Fixture fixture = {0, 0, {5, 6, 7, 8}};
    FA18MapPacketLowFilterInput input = {7, 2, 0x10, resolve, &fixture};
    FA18MapPacketLowFilterResult result;
    FA18MapPacketLowFilterRoute route;
    assert(fa18_run_map_packet_low_filter(&input, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_LOW_FILTER_MATCH && result.detail_flag &&
           result.stream_address == UINT32_C(0x00c2aca8) &&
           fixture.address == UINT32_C(0x00c2aa5c) && fixture.selector == 2);
    input.metric = 0x11;
    input.metric_selector = 9;
    assert(fa18_run_map_packet_low_filter(&input, &result, &route) == 0 &&
           route == FA18_MAP_PACKET_LOW_FILTER_COLUMN_STAGE &&
           fixture.address == UINT32_C(0x00c2aa1c));
    input.metric = 0x161;
    fixture.row[0] = -1;
    assert(fa18_run_map_packet_low_filter(&input, &result, &route) == 0 &&
           route == FA18_MAP_PACKET_LOW_FILTER_MATCH &&
           fixture.address == UINT32_C(0x00c2a9dc));
    input.metric = 0x3a1;
    fixture.address = 0;
    assert(fa18_run_map_packet_low_filter(&input, &result, &route) == 0 &&
           route == FA18_MAP_PACKET_LOW_FILTER_COLUMN_STAGE && !fixture.address);
    return 0;
}
