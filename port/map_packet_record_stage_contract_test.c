#include "map_packet_record_stage.h"

#include <assert.h>

typedef struct { uint16_t calls; } Fixture;

static int display(void *context, const FA18MapPacketProjectionRecord *records,
                   uint16_t count) {
    Fixture *fixture = context;
    if (!fixture || !records || count != 1) return -1;
    ++fixture->calls;
    return 0;
}

int main(void) {
    const uint8_t packet[] = {0,0,0,0, 0,1, 0,2, 0,4};
    Fixture fixture = {0};
    const FA18MapPacketRecordStageInput input = {
        3,
        {0, 0, 0, 0, 0},
        {0, 0, 0, 0, 1, 0x80, 0, 0, 0, 0},
        {{packet, sizeof packet, INT32_C(0x00120000), 0, {256, -256, 128},
          16, 0, 0, 0},
         {0, {0,0,0}, 0, {{256, 0, 0, 0, 0, 256, 128, 0, 128}}},
         display, &fixture}
    };
    FA18MapPacketProjectionRecord records[0x12];
    uint16_t count;
    FA18MapPacketRecordStageRoute route;
    assert(fa18_run_map_packet_record_stage(&input, records, 0x12, &count, 0,
                                            &route) == 0);
    assert(route == FA18_MAP_PACKET_RECORD_DISPLAYED && count == 1 &&
           fixture.calls == 1);

    FA18MapPacketRecordStageInput terminator = input;
    terminator.encoded_mode = -1;
    assert(fa18_run_map_packet_record_stage(&terminator, records, 0x12, &count, 0,
                                            &route) == 0 &&
           route == FA18_MAP_PACKET_RECORD_TERMINATOR && !count);
    terminator.encoded_mode = 3;
    terminator.gate.frame_flag = 1;
    assert(fa18_run_map_packet_record_stage(&terminator, records, 0x12, &count, 0,
                                            &route) == 0 &&
           route == FA18_MAP_PACKET_RECORD_FRAME_STOP && !count);
    return 0;
}
