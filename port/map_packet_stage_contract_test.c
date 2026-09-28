#include "map_packet_stage.h"

#include <assert.h>

typedef struct { uint16_t calls; FA18MapPacketProjectionRecord record; } Fixture;

static int display(void *context, const FA18MapPacketProjectionRecord *records,
                   uint16_t count, uint16_t coordinate_shift) {
    Fixture *fixture = context;
    if (!fixture || count != 1 || coordinate_shift != 0) return -1;
    ++fixture->calls;
    fixture->record = records[0];
    return 0;
}

int main(void) {
    const uint8_t packet[] = {0,0,0,0, 0,1, 0,2, 0,4};
    Fixture fixture = {0};
    const FA18MapPacketStageInput input = {
        {packet, sizeof packet, 0x00120000, 0, {256, -256, 128}, 16, 0, 0, 0},
        {UINT32_C(0x000a0014), {0,0,0}, 0,
         {{256, 0, 0, 0, 0, 256, 128, 0, 128}}},
        0, display, &fixture
    };
    FA18MapPacketProjectionRecord records[0x12];
    uint16_t count;
    FA18MapPacketStageRoute route;
    assert(fa18_run_map_packet_stage(&input, records, 0x12, &count, &route) == 0);
    assert(route == FA18_MAP_PACKET_STAGE_DISPLAYED && count == 1 && fixture.calls == 1);
    assert(fixture.record.value[0] == -6 && fixture.record.value[1] == 42 &&
           fixture.record.value[2] == 9);
    return 0;
}
