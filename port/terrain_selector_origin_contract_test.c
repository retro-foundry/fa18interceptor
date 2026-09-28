#include "terrain_selector_origin.h"

#include <assert.h>

static void put16(uint8_t *bytes, uint16_t value) {
    bytes[0] = (uint8_t)(value >> 8); bytes[1] = (uint8_t)value;
}

static void put32(uint8_t *bytes, uint32_t value) {
    bytes[0] = (uint8_t)(value >> 24); bytes[1] = (uint8_t)(value >> 16);
    bytes[2] = (uint8_t)(value >> 8); bytes[3] = (uint8_t)value;
}

static void prepared(void *context) {
    unsigned *calls = context;
    ++*calls;
}

int main(void) {
    uint8_t record[FA18_TERRAIN_SELECTOR_ORIGIN_DIRECT_RECORD_BYTES] = {0};
    unsigned calls = 0;
    FA18TerrainSelectorOriginDirectState state = {
        .origin_enable = 1, .gate_b = 1, .gate_mode = 1,
        .active_record = record, .active_record_size = sizeof record,
        .origin = {0, -0x100, 0}, .prepare_matrix = prepared,
        .prepare_context = &calls
    };
    FA18TerrainSelectorOriginResult result;
    int32_t origin[3] = {0};

    put32(record + 0x14, 0x10203040); put32(record + 0x1c, 0xfedcba98);
    put16(record + 0x4e, 0xfffe);
    assert(fa18_publish_terrain_selector_origin_direct(&state, &result) == 0);
    assert(calls == 1 && result == FA18_TERRAIN_SELECTOR_ORIGIN_DIRECT_PUBLISHED);
    assert(state.origin[0] == 0x10203040 && state.origin[1] == 0x500 &&
           (uint32_t)state.origin[2] == 0xfedcba98);
    assert(fa18_publish_terrain_selector_origin_direct_callback(&state, origin) == 0);
    assert(calls == 2 && origin[0] == state.origin[0] && origin[1] == state.origin[1] &&
           origin[2] == state.origin[2]);
    state.gate_a = 1;
    assert(fa18_publish_terrain_selector_origin_direct(&state, &result) == 0);
    assert(result == FA18_TERRAIN_SELECTOR_ORIGIN_GATE_EXIT && calls == 3);
    state.gate_a = 0; state.gate_mode = 0;
    assert(fa18_publish_terrain_selector_origin_direct(&state, &result) == 0);
    assert(result == FA18_TERRAIN_SELECTOR_ORIGIN_UNPORTED_MATRIX_ROUTE && calls == 4);
    return 0;
}
