#include "map_packet_pass_runner.h"

#include <assert.h>

typedef struct { uint16_t displays; } Fixture;

static int low_row(void *context, uint32_t address, int16_t selector,
                   int8_t row[4]) {
    (void)context; (void)address; (void)selector;
    row[0] = 1; row[1] = 2; row[2] = 3; row[3] = 4;
    return 0;
}

static int stream(void *context, uint32_t address, const uint8_t **data,
                  size_t *size) {
    static const uint8_t controls[] = {3, 0xff};
    (void)context;
    if (address != UINT32_C(0x00c2a04e)) return -1;
    *data = controls;
    *size = sizeof controls;
    return 0;
}

static int display(void *context, const FA18MapPacketProjectionRecord *records,
                   uint16_t count, uint16_t coordinate_shift) {
    Fixture *fixture = context;
    (void)coordinate_shift;
    if (!records || count != 1) return -1;
    ++fixture->displays;
    return 0;
}

static int record(void *context, const FA18MapPacketPassSelectorResult *pass,
                  uint8_t mode, FA18MapPacketRecordStageInput *result) {
    static const uint8_t packet[] = {0,0,0,0, 0,1, 0,2, 0,4};
    Fixture *fixture = context;
    if (pass->directory.layout != FA18_MAP_PACKET_DIRECTORY_NORMAL || mode != 3)
        return -1;
    *result = (FA18MapPacketRecordStageInput){
        0, {0,0,0,0,0}, {0,0,0,0,1,0x80,0,0,0,0},
        {{packet, sizeof packet, INT32_C(0x00120000), 0, {256,-256,128},
          16,0,0,0}, {0,{0,0,0},0,{{256,0,0,0,0,256,128,0,128}}},
         0, display, fixture}
    };
    return 0;
}

int main(void) {
    Fixture fixture = {0};
    FA18MapPacketPassRunnerInput input = {
        {FA18_MAP_PACKET_DIRECTORY_NORMAL,
         {0,{INT32_C(0x11223344),INT32_C(0x55667788),INT32_C(0x99aabbcc)},
          {0,0,0},0,0}, 0, 0, 0, low_row, &fixture},
        stream, record, &fixture
    };
    FA18MapPacketProjectionRecord records[0x12];
    FA18MapPacketPassSelectorResult pass;
    FA18MapPacketControlWalkerRoute route;
    uint16_t count;
    assert(fa18_run_map_packet_pass(&input, records, 0x12, &count, &pass, &route) == 0 &&
           route == FA18_MAP_PACKET_CONTROL_TERMINATOR && fixture.displays == 1 &&
           pass.control_stream_address == UINT32_C(0x00c2a04e));
    return 0;
}
