#include "map_packet_stage.h"

#include <assert.h>

typedef struct {
    uint16_t calls;
    FA18MapPacketProjectionRecord record;
    int16_t origin[3];
    uint16_t origins;
} Fixture;

static int display(void *context, const FA18MapPacketProjectionRecord *records,
                   uint16_t count, uint16_t coordinate_shift) {
    Fixture *fixture = context;
    if (!fixture || count != 1 || coordinate_shift != 0) return -1;
    ++fixture->calls;
    fixture->record = records[0];
    return 0;
}

static void publish_origin(void *context, const int16_t origin[3]) {
    Fixture *fixture = context;
    unsigned i;
    ++fixture->origins;
    for (i = 0; i < 3; ++i) fixture->origin[i] = origin[i];
}

int main(void) {
    const uint8_t packet[] = {0,0,0,0, 0,1, 0,2, 0,4, 0xff,0xff};
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

    /* One packet can contain more than one polygon; a relative-detail word
     * between them is consumed before the next vertex count. */
    {
        const uint8_t multiple[] = {
            0,0,0,0, 0,1, 0,2,0,4,
            0x80,1, 0,1, 0,3,0,5, 0xff,0xff
        };
        Fixture many = {0};
        FA18MapPacketStageInput repeated = input;
        repeated.selector.packet = multiple;
        repeated.selector.packet_size = sizeof multiple;
        repeated.display_context = &many;
        repeated.publish_origin = publish_origin;
        assert(fa18_run_map_packet_stage(&repeated, records, 0x12,
                                         &count, &route) == 0);
        assert(route == FA18_MAP_PACKET_STAGE_DISPLAYED);
        assert(many.calls == 2 && many.origins == 1 && count == 1);
        assert(many.origin[0] == -18 && many.origin[1] == 18 &&
               many.origin[2] == -9);
        assert(many.record.value[0] == -5 && many.record.value[1] == 43 &&
               many.record.value[2] == 10);
        many = (Fixture){0};
        repeated.selector.detail_metric = 0;
        assert(fa18_run_map_packet_stage(&repeated, records, 0x12,
                                         &count, &route) == 0);
        assert(route == FA18_MAP_PACKET_STAGE_DISPLAYED &&
               many.calls == 1 && many.origins == 1);
    }
    return 0;
}
