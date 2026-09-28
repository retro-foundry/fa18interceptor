#include "map_packet_parent_pass.h"

#include <assert.h>

typedef struct { uint16_t calls; } Fixture;
static int display(void *context, const FA18MapPacketProjectionRecord *records,
                   uint16_t count, uint16_t workspace_shift) {
    Fixture *fixture = context;
    if (!fixture || !records || count != 1 || workspace_shift != 0) return -1;
    ++fixture->calls;
    return 0;
}
static int low_row(void *context, uint32_t address, int16_t selector,
                   int8_t row[4]) {
    (void)context;
    if (address != 0x00c2aa1c || selector != 0) return -1;
    row[0] = -1; row[1] = 0; row[2] = 0; row[3] = 0;
    return 0;
}

int main(void) {
    enum { LOW_STREAM_OFFSET = 0xda8, WIDE_DIRECTORY_OFFSET = 0x1c4,
           PACKET_OFFSET = 0x1e4 };
    uint8_t control[0xe00] = {0};
    uint8_t packet[0x240] = {0};
    FA18MapPacketStaticData static_data = {control, sizeof control,
                                            packet, sizeof packet};
    FA18ProjectionPacket projection = {0, 0, 0, -125};
    Fixture fixture = {0};
    FA18MapPacketParentPassInput input = {
        {&projection, 0, 1, 0x80},
        {&static_data,
         {FA18_MAP_PACKET_DIRECTORY_WIDE,
          {0,{0,0,0},{0,0,0},0,0}, 0, 0, 0, low_row, 0},
         {0,{0,0,0,0,0},{0,0,0,0,1,0x80,0,0,0,0},
          {{packet + PACKET_OFFSET, sizeof packet - PACKET_OFFSET, 0, 0,
            {0,0,0},0,0,0,0},
           {0,{0,0,0},0,{{256,0,0,0,256,0,0,0,256}}},
           0,display,&fixture},0,{0}},0}
    };
    FA18MapPacketProjectionRecord records[0x12];
    FA18MapPacketParentPassResult result;

    control[LOW_STREAM_OFFSET] = 4;
    control[LOW_STREAM_OFFSET + 1] = 0xff;
    packet[WIDE_DIRECTORY_OFFSET] = 0;
    packet[WIDE_DIRECTORY_OFFSET + 1] = 0x20;
    packet[PACKET_OFFSET + 4] = 0;
    packet[PACKET_OFFSET + 5] = 1;
    packet[PACKET_OFFSET + 6] = 0;
    packet[PACKET_OFFSET + 7] = 1;
    packet[PACKET_OFFSET + 8] = 0;
    packet[PACKET_OFFSET + 9] = 2;
    assert(fa18_run_map_packet_parent_pass(&input, records, 0x12, &result) == 0);
    assert(result.depth.metric == 125 && !result.depth.run_normal_pass &&
           result.wide_route == FA18_MAP_PACKET_CONTROL_TERMINATOR &&
           result.wide_pass.control_stream_address == 0x00c2aca8 &&
           fixture.calls == 1);
    return 0;
}
