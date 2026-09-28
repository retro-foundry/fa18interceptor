#include "map_packet_static_data.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t control[0x20] = {1, 0xff};
    uint8_t packet[0x30] = {0};
    FA18HunkSegment segments[69] = {0};
    FA18Hunks hunks = {segments, 69};
    FA18MapPacketStaticData data;
    const uint8_t *resolved;
    size_t size;
    int8_t pair[2];

    control[0x12] = 0x80;
    control[0x13] = 2;
    packet[0x20] = 0x12;
    segments[FA18_MAP_PACKET_CONTROL_HUNK] =
        (FA18HunkSegment){FA18_HUNK_CODE, control, sizeof control, 0, 0};
    segments[FA18_MAP_PACKET_PACKET_HUNK] =
        (FA18HunkSegment){FA18_HUNK_CODE, packet, sizeof packet, 0, 0};
    assert(fa18_load_map_packet_static_data(&hunks, &data) == 0);
    assert(fa18_resolve_map_packet_control_pair(&data, 9, pair) == 0 &&
           pair[0] == -128 && pair[1] == 2);
    assert(fa18_resolve_map_packet_control_stream(
               &data, FA18_MAP_PACKET_CONTROL_RUNTIME_BASE + 0x12,
               &resolved, &size) == 0 && resolved == control + 0x12 &&
           size == sizeof control - 0x12);
    assert(fa18_resolve_map_packet_static_packet(
               &data, FA18_MAP_PACKET_PACKET_RUNTIME_BASE + 0x20,
               &resolved, &size) == 0 && resolved == packet + 0x20 &&
           size == sizeof packet - 0x20);
    assert(fa18_resolve_map_packet_control_stream(
               &data, FA18_MAP_PACKET_CONTROL_RUNTIME_BASE + sizeof control,
               &resolved, &size) == -1);
    assert(fa18_resolve_map_packet_static_packet(
               &data, FA18_MAP_PACKET_PACKET_RUNTIME_BASE - 1,
               &resolved, &size) == -1);
    return 0;
}
