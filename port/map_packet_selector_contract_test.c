#include "map_packet_selector.h"

#include <assert.h>

typedef struct { const uint8_t *stream; size_t size; uint32_t reference; } Fixture;

static int resolve(void *context, uint32_t reference, const uint8_t **stream, size_t *size) {
    Fixture *fixture = context;
    if (!fixture || reference != fixture->reference) return -1;
    *stream = fixture->stream;
    *size = fixture->size;
    return 0;
}

int main(void) {
    const uint8_t inline_packet[] = {0, 0, 0, 0, 0, 2, 0, 1, 0, 2, 0, 3, 0, 4};
    FA18MapPacketSelectorInput input = {
        inline_packet, sizeof inline_packet, 0x00120000, 0, {256, -256, 128},
        16, 0, 0, 0
    };
    FA18MapPacketSelectorResult result;
    FA18MapPacketSelectorRoute route;
    assert(fa18_select_map_packet_stream(&input, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_READY && result.pair_count == 2 &&
           result.pair_stream_size == 8 && result.origin_component[0] == -18 &&
           result.origin_component[1] == 18 && result.origin_component[2] == -9);

    const uint8_t guarded_packet[] = {0,0,0,0, 0x80,0x02, 0,3};
    input.packet = guarded_packet;
    input.packet_size = sizeof guarded_packet;
    input.detail_metric = 7;
    assert(fa18_select_map_packet_stream(&input, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_REJECTED);
    input.detail_metric = 8;
    assert(fa18_select_map_packet_stream(&input, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_READY && result.pair_count == 3);

    const uint8_t alternate_packet[] = {0,0,0,0x40};
    const uint8_t alternate_stream[] = {0,1, 0,0, 0,0};
    Fixture fixture = {alternate_stream, sizeof alternate_stream, 0x40};
    input.packet = alternate_packet;
    input.packet_size = sizeof alternate_packet;
    input.alternate_stream = 1;
    input.resolve_stream = resolve;
    input.context = &fixture;
    assert(fa18_select_map_packet_stream(&input, &result, &route) == 0);
    assert(route == FA18_MAP_PACKET_READY && result.pair_count == 1);
    return 0;
}
