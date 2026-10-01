#include "map_packet_relative_offset.h"

#include <assert.h>

typedef struct {
    int8_t pair[2];
    uint32_t expected_address;
    const uint8_t *packet;
    size_t packet_size;
} Fixture;

static int resolve_pair(void *context, uint8_t mode, int8_t pair[2]) {
    Fixture *fixture = context;
    assert(mode == 3);
    pair[0] = fixture->pair[0];
    pair[1] = fixture->pair[1];
    return 0;
}

static int resolve_packet(void *context, uint32_t address, const uint8_t **packet,
                          size_t *packet_size) {
    Fixture *fixture = context;
    if (address != fixture->expected_address) return -1;
    *packet = fixture->packet;
    *packet_size = fixture->packet_size;
    return 0;
}

int main(void) {
    uint8_t directory[128] = {0};
    const uint8_t packet[] = {0, 0, 0, 0};
    Fixture fixture = {{-1, 2}, UINT32_C(0x00c42ca8) + 0x24, packet,
                       sizeof packet};
    directory[2u * 16u + 2u] = 0;
    directory[2u * 16u + 3u] = 0x24;
    FA18MapPacketRelativeOffsetInput input = {
        3, 2, 7, 0, 7, 0, 0, UINT32_C(0x00c42ca8), directory,
        sizeof directory, resolve_pair, resolve_packet, &fixture
    };
    FA18MapPacketRelativeOffsetResult result;
    FA18MapPacketRelativeOffsetRoute route;
    assert(fa18_select_map_packet_relative_offset(&input, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_RELATIVE_OFFSET_READY && result.row == 1 &&
           result.column == 2 && result.packet == packet &&
           result.packet_address == fixture.expected_address && !result.error_code);

    fixture.pair[0] = -3;
    assert(fa18_select_map_packet_relative_offset(&input, &result, &route) == 0 &&
           route == FA18_MAP_PACKET_RELATIVE_OFFSET_OUT_OF_BOUNDS);
    fixture.pair[0] = -1;
    directory[2u * 16u + 2u] = 0;
    directory[2u * 16u + 3u] = 0;
    assert(fa18_select_map_packet_relative_offset(&input, &result, &route) == 0 &&
           route == FA18_MAP_PACKET_RELATIVE_OFFSET_INVALID && result.error_code == 0x40);

    directory[2u * 16u + 3u] = 0x24;
    const uint8_t negative_packet[] = {0x80, 0, 0, 0};
    fixture.packet = negative_packet;
    assert(fa18_select_map_packet_relative_offset(&input, &result, &route) == 0 &&
           route == FA18_MAP_PACKET_RELATIVE_OFFSET_RETRY &&
           result.packet_address == fixture.expected_address);
    input.allow_negative_packet = 1;
    assert(fa18_select_map_packet_relative_offset(&input, &result, &route) == 0 &&
           route == FA18_MAP_PACKET_RELATIVE_OFFSET_READY);

    uint8_t wide_directory[2048] = {0};
    wide_directory[3u * 64u + 4u] = 0;
    wide_directory[3u * 64u + 5u] = 0x24;
    fixture.pair[0] = 2;
    fixture.pair[1] = 3;
    fixture.packet = packet;
    input.row_min = 0;
    input.row_max = 31;
    input.column_min = 0;
    input.column_max = 31;
    input.wide_layout = 1;
    input.record_directory = wide_directory;
    input.record_directory_size = sizeof wide_directory;
    assert(fa18_select_map_packet_relative_offset(&input, &result, &route) == 0 &&
           route == FA18_MAP_PACKET_RELATIVE_OFFSET_READY && result.row == 2 &&
           result.column == 3);
    return 0;
}
