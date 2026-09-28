#include "terrain_placement_direct.h"

#include <assert.h>
#include <string.h>

typedef struct { uint8_t records[3][FA18_SCENE_PLACEMENT_BYTES]; unsigned count; } Observation;

static int emit_record(void *context, const uint8_t record[FA18_SCENE_PLACEMENT_BYTES]) {
    Observation *observation = context;
    ++observation->count;
    assert(observation->count <= 3);
    memcpy(observation->records[observation->count - 1u], record,
           FA18_SCENE_PLACEMENT_BYTES);
    return 0;
}

int main(void) {
    int8_t type_map[21] = { 0 };
    int8_t type_pairs[2] = { 0 };
    int8_t placement_map[16] = { 0 };
    int16_t pair_table[4] = { 0 };
    uint8_t workspace[16 * 0x60] = { 0 };
    uint8_t shifts[0xf0] = { 0 };
    uint8_t bit4_records[16 * 512] = { 0 };
    uint8_t bit6_records[16 * 32] = { 0 };
    FA18ScenePlacementBuilderPrefixInput prefix_input = {
        0, type_map, 21, type_pairs, 2, { 0, 0 }, 0, placement_map, 16,
        pair_table, 4, pair_table, 4, workspace, sizeof workspace
    };
    FA18TerrainPlacementDirectInput input = {
        .correction_word = { 0, 0 }, .projection_packet = { 0, 0 },
        .projection_depth = 0, .shift_table = shifts, .shift_table_size = sizeof shifts,
        .descriptor_table_base = UINT32_C(0x00c22188), .descriptor_table_entries = 128,
        .bit4_records = bit4_records, .bit4_records_size = sizeof bit4_records,
        .bit6_records = bit6_records, .bit6_records_size = sizeof bit6_records,
        .retained_third_work = 0x1234, .flagged_tail_byte = 0x12,
        .append_enabled = 1, .cycle_byte = 0
    };
    FA18TerrainPlacementDirectResult result;
    Observation observation = { 0 };

    placement_map[0] = 1;
    workspace[0x60] = 0;
    workspace[0x61] = 0x6e;
    workspace[0x62] = 0;
    workspace[0x63] = 8;
    workspace[0x64] = 0;
    workspace[0x65] = 12;
    workspace[0x66] = 0x10;
    workspace[0x67] = 2;
    workspace[0x68] = 0x40;
    workspace[0x69] = 3;
    workspace[0x6a] = 0xff;
    bit4_records[2 * 512 + 1] = 0xff;
    bit6_records[3 * 32 + 3] = 4;
    assert(fa18_emit_direct_prefixed_placement_records(&prefix_input, &input,
                                                        emit_record, &observation,
                                                        &result) == 0);
    assert(result.cell_end == FA18_TERRAIN_PLACEMENT_CELL_TERMINATOR &&
           result.emitted_count == 3 && result.next_cycle_byte == 1 &&
           result.next_record_tail_word == 0x92 && observation.count == 3);
    assert(!memcmp(observation.records[0], (uint8_t[]){ 0x6e, 0, 0, 0xc2, 0x2a, 0x20,
        0, 8, 0, 0, 0, 12, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 }, 24));
    assert(!memcmp(observation.records[1], (uint8_t[]){ 2, 0x10, 0, 0xc2, 0x21, 0xb0,
        0, 0, 0, 0, 0x12, 0x34, 0x92, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0 }, 24));
    assert(!memcmp(observation.records[2], (uint8_t[]){ 3, 0x40, 0, 0xc2, 0x21, 0xd8,
        0, 0, 0, 0, 0x12, 0x34, 0x92, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0 }, 24));
    assert(bit4_records[2 * 512 + 1] == 0xfbu);
    input.bit4_records = NULL;
    workspace[0x60] = 0x10;
    workspace[0x61] = 2;
    assert(fa18_emit_direct_prefixed_placement_records(&prefix_input, &input,
                                                        emit_record, &observation,
                                                        &result) == -1);
    return 0;
}
