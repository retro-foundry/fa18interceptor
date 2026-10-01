#include "map_packet_control_walker.h"

#include <assert.h>

typedef struct { uint16_t displays; uint16_t resolutions; } Fixture;

static int display(void *context, const FA18MapPacketProjectionRecord *records,
                   uint16_t count, uint16_t coordinate_shift) {
    Fixture *fixture = context;
    (void)coordinate_shift;
    if (!fixture || !records || count != 1) return -1;
    ++fixture->displays;
    return 0;
}

static int resolve(void *context, uint8_t mode, FA18MapPacketRecordStageInput *input) {
    static const uint8_t packet[] = {0,0,0,0, 0,1, 0,2, 0,4, 0xff,0xff};
    Fixture *fixture = context;
    if (!fixture || !input) return -1;
    ++fixture->resolutions;
    *input = (FA18MapPacketRecordStageInput){
        (int8_t)mode, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 0x80, 0, 0, 0, 0},
        {{packet, sizeof packet, INT32_C(0x00120000), 0, {256, -256, 128},
         16, 0, 0, 0},
         {0, {0,0,0}, 0, {{256, 0, 0, 0, 0, 256, 128, 0, 128}}},
         0, display, fixture}
    };
    return 0;
}

int main(void) {
    const uint8_t controls[] = {3, 0x83, 9, 0xff};
    Fixture fixture = {0};
    const FA18MapPacketControlWalkerInput input = {
        controls, sizeof controls, resolve, &fixture
    };
    FA18MapPacketProjectionRecord records[0x12];
    uint16_t count;
    FA18MapPacketControlWalkerRoute route;
    assert(fa18_walk_map_packet_controls(&input, records, 0x12, &count, &route) == 0);
    assert(route == FA18_MAP_PACKET_CONTROL_FRAME_STOP && fixture.displays == 2 &&
           fixture.resolutions == 2 && !count);
    const uint8_t terminator[] = {0xff};
    const FA18MapPacketControlWalkerInput end = {terminator, sizeof terminator,
                                                  resolve, &fixture};
    assert(fa18_walk_map_packet_controls(&end, records, 0x12, &count, &route) == 0 &&
           route == FA18_MAP_PACKET_CONTROL_TERMINATOR);
    return 0;
}
