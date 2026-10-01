#include "map_packet_original_pass.h"

#include <assert.h>

typedef struct { uint16_t calls; FA18MapPacketProjectionRecord record; } Fixture;
static int display(void *context, const FA18MapPacketProjectionRecord *records,
                   uint16_t count, uint16_t workspace_shift) {
    Fixture *fixture = context;
    if (!fixture || !records || count != 1 || workspace_shift != 0) return -1;
    ++fixture->calls;
    fixture->record = records[0];
    return 0;
}

int main(void) {
    enum { CONTROL_HIGH_OFFSET = 0x1c2, WIDE_DIRECTORY_OFFSET = 0x1c4,
           PACKET_OFFSET = 0x1e4 };
    uint8_t control[0x200] = {0};
    uint8_t packet[0x240] = {0};
    FA18MapPacketStaticData static_data = {control, sizeof control,
                                            packet, sizeof packet};
    Fixture fixture = {0};
    FA18MapPacketOriginalPassInput input = {
        &static_data,
        {FA18_MAP_PACKET_DIRECTORY_WIDE,
         {0, {0,0,0}, {0,0,0}, 0, 0}, 0x10001, 0, 0, 0, 0},
        {0, {0,0,0,0,0}, {0,0,0,0,1,0x80,0,0,0,0},
         {{packet + PACKET_OFFSET, sizeof packet - PACKET_OFFSET, 0, 0,
           {0,0,0}, 0, 0, 0, 0},
          {0, {0,0,0}, 0, {{256,0,0,0,256,0,0,0,256}}},
          0, display, &fixture},
         0, {0}},
        0
    };
    FA18MapPacketProjectionRecord records[0x12];
    FA18MapPacketPassSelectorResult pass;
    FA18MapPacketControlWalkerRoute route;
    uint16_t count;

    control[CONTROL_HIGH_OFFSET] = 0;
    control[CONTROL_HIGH_OFFSET + 1] = 0xff;
    packet[WIDE_DIRECTORY_OFFSET] = 0;
    packet[WIDE_DIRECTORY_OFFSET + 1] = 0x20;
    packet[PACKET_OFFSET + 4] = 0;
    packet[PACKET_OFFSET + 5] = 1;
    packet[PACKET_OFFSET + 6] = 0;
    packet[PACKET_OFFSET + 7] = 1;
    packet[PACKET_OFFSET + 8] = 0;
    packet[PACKET_OFFSET + 9] = 2;
    packet[PACKET_OFFSET + 10] = 0xff;
    packet[PACKET_OFFSET + 11] = 0xff;
    assert(fa18_run_original_map_packet_pass(&input, records, 0x12, &count,
                                              &pass, &route) == 0);
    assert(route == FA18_MAP_PACKET_CONTROL_TERMINATOR);
    assert(count == 0);
    assert(fixture.calls == 1);
    assert(pass.control_stream_address == 0x00c2a0c2);
    assert(fixture.record.value[0] == 1 && fixture.record.value[2] == 2);
    return 0;
}
