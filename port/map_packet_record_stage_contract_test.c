#include "map_packet_record_stage.h"

#include <assert.h>

typedef struct { uint16_t calls, expected_shift; } Fixture;

typedef struct {
    Fixture display;
    uint8_t directory[128];
    const uint8_t *packet;
    size_t packet_size;
    uint32_t packet_address;
} RelativeFixture;

static int display(void *context, const FA18MapPacketProjectionRecord *records,
                   uint16_t count, uint16_t coordinate_shift) {
    Fixture *fixture = context;
    if (!fixture || !records || count != 1 ||
        coordinate_shift != fixture->expected_shift) return -1;
    ++fixture->calls;
    return 0;
}

static int resolve_inline_stream(void *context, uint32_t reference,
                                 const uint8_t **stream, size_t *size) {
    const uint8_t *packet = context;
    if (!packet || reference != 0) return -1;
    *stream = packet + 4;
    *size = 6;
    return 0;
}

static int resolve_relative_pair(void *context, uint8_t mode, int8_t pair[2]) {
    (void)context;
    assert(mode == 3);
    pair[0] = 0;
    pair[1] = 0;
    return 0;
}

static int resolve_relative_packet(void *context, uint32_t address,
                                   const uint8_t **packet, size_t *packet_size) {
    RelativeFixture *fixture = context;
    if (address != fixture->packet_address) return -1;
    *packet = fixture->packet;
    *packet_size = fixture->packet_size;
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
         0, display, &fixture}
    };
    FA18MapPacketProjectionRecord records[0x12];
    uint16_t count;
    FA18MapPacketRecordStageRoute route;
    assert(fa18_run_map_packet_record_stage(&input, records, 0x12, &count, 0,
                                            &route) == 0);
    assert(route == FA18_MAP_PACKET_RECORD_DISPLAYED && count == 1 &&
           fixture.calls == 1);

    FA18MapPacketRecordStageInput shifted = input;
    shifted.encoded_mode = 4;
    shifted.gate = (FA18MapDetailGateInput){0, 1, 0, 1, 0x400};
    shifted.packet_stage.selector.resolve_stream = resolve_inline_stream;
    shifted.packet_stage.selector.context = (void *)packet;
    fixture.expected_shift = 2;
    assert(fa18_run_map_packet_record_stage(&shifted, records, 0x12, &count, 0,
                                            &route) == 0 &&
           route == FA18_MAP_PACKET_RECORD_DISPLAYED && count == 1 &&
           fixture.calls == 2);

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

    const uint8_t relative_packet[] = {0,0,0,0, 0,1, 0,2, 0,4};
    RelativeFixture relative_fixture = {{0}, {0}, relative_packet,
        sizeof relative_packet, UINT32_C(0x00c42ca8) + 0x20};
    relative_fixture.directory[1] = 0x20;
    FA18MapPacketRecordStageInput relative = input;
    relative.use_relative_offset = 1;
    relative.relative_offset = (FA18MapPacketRelativeOffsetInput){
        0, 0, 7, 0, 7, 0, 0, UINT32_C(0x00c42ca8),
        relative_fixture.directory, sizeof relative_fixture.directory,
        resolve_relative_pair, resolve_relative_packet, &relative_fixture
    };
    relative.packet_stage.selector.packet = (const uint8_t[]){0x80,0,0,0};
    relative.packet_stage.selector.packet_size = 4;
    relative.packet_stage.display_context = &relative_fixture.display;
    assert(fa18_run_map_packet_record_stage(&relative, records, 0x12, &count, 0,
                                             &route) == 0 &&
           route == FA18_MAP_PACKET_RECORD_DISPLAYED &&
           relative_fixture.display.calls == 1);
    relative_fixture.directory[1] = 0;
    assert(fa18_run_map_packet_record_stage(&relative, records, 0x12, &count, 0,
                                             &route) == 0 &&
           route == FA18_MAP_PACKET_RECORD_REJECTED && !count);
    return 0;
}
