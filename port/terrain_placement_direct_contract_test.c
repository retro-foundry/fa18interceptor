#include "terrain_placement_direct.h"

#include <assert.h>
#include <string.h>

typedef struct { uint8_t record[FA18_SCENE_PLACEMENT_BYTES]; unsigned count; } Observation;

static int emit_record(void *context, const uint8_t record[FA18_SCENE_PLACEMENT_BYTES]) {
    Observation *observation = context;
    ++observation->count;
    memcpy(observation->record, record, sizeof observation->record);
    return 0;
}

int main(void) {
    int8_t type_map[21] = { 0 };
    int8_t type_pairs[2] = { 0 };
    int8_t placement_map[16] = { 0 };
    int16_t pair_table[4] = { 0 };
    uint8_t workspace[16 * 0x60] = { 0 };
    uint8_t shifts[0xf0] = { 0 };
    FA18ScenePlacementBuilderPrefixInput prefix_input = {
        0, type_map, 21, type_pairs, 2, { 0, 0 }, 0, placement_map, 16,
        pair_table, 4, pair_table, 4, workspace, sizeof workspace
    };
    FA18TerrainPlacementDirectInput input = {
        { 0, 0 }, { 0, 0 }, 0, shifts, sizeof shifts,
        UINT32_C(0x00c22188), 128, 1, 0
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
    workspace[0x66] = 0xff;
    assert(fa18_emit_direct_prefixed_placement_records(&prefix_input, &input,
                                                        emit_record, &observation,
                                                        &result) == 0);
    assert(result.cell_end == FA18_TERRAIN_PLACEMENT_CELL_TERMINATOR &&
           result.emitted_count == 1 && result.next_cycle_byte == 3 &&
           observation.count == 1);
    assert(!memcmp(observation.record, (uint8_t[]){ 0x6e, 0, 0, 0xc2, 0x2a, 0x20,
        0, 8, 0, 0, 0, 12, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 24));
    workspace[0x60] = 0x10;
    assert(fa18_emit_direct_prefixed_placement_records(&prefix_input, &input,
                                                        emit_record, &observation,
                                                        &result) == -1);
    return 0;
}
