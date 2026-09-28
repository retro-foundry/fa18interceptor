#include "terrain_placement_pipeline.h"

#include <assert.h>
#include <string.h>

typedef struct {
    unsigned resolved;
    unsigned emitted;
    uint8_t ordinal;
    uint8_t record[FA18_SCENE_PLACEMENT_BYTES];
} Observation;

static int resolve_descriptor(void *context, uint8_t index, uint32_t *reference) {
    (void)context;
    if (index != 0x6e) return -1;
    *reference = UINT32_C(0x00c22408);
    return 0;
}

static int resolve_input(void *context, const uint8_t *item, uint8_t ordinal,
                         FA18ScenePlacementBuildInput *input) {
    static const uint8_t shifts[0xf0] = { 0 };
    static const FA18ScenePlacementWorkInput work = {
        { 8, 12 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, 1
    };
    Observation *observation = context;
    ++observation->resolved;
    observation->ordinal = ordinal;
    *input = (FA18ScenePlacementBuildInput){
        item, &work, { { 0, 0, 0 }, { 0, 0, 0 }, shifts, sizeof shifts,
                       UINT32_C(0x10000000), ordinal },
        resolve_descriptor, 0
    };
    return 0;
}

static int emit_record(void *context, const uint8_t record[FA18_SCENE_PLACEMENT_BYTES]) {
    Observation *observation = context;
    ++observation->emitted;
    memcpy(observation->record, record, sizeof observation->record);
    return 0;
}

int main(void) {
    uint8_t cell[0x60] = { 0, 0x6e, 0, 8, 0, 12, 0xff };
    Observation observation = { 0 };
    FA18TerrainPlacementCellEnd end;

    assert(fa18_emit_workspace_placement_records(cell, sizeof cell, resolve_input,
                                                  emit_record, &observation, &end) == 0);
    assert(end == FA18_TERRAIN_PLACEMENT_CELL_TERMINATOR);
    assert(observation.resolved == 1 && observation.emitted == 1 &&
           observation.ordinal == 1);
    assert(!memcmp(observation.record, (uint8_t[]){ 0x6e, 0, 0, 0xc2, 0x24, 8,
        0, 8, 0, 0, 0, 12, 0x10, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 }, 24));
    cell[0] = 0x10;
    cell[1] = 0x6e;
    cell[2] = 0xff;
    observation = (Observation){ 0 };
    assert(fa18_emit_workspace_placement_records(cell, sizeof cell, resolve_input,
                                                  emit_record, &observation, &end) == 0);
    assert(observation.resolved == 1 && end == FA18_TERRAIN_PLACEMENT_CELL_TERMINATOR);
    assert(fa18_emit_workspace_placement_records(cell, 0x5f, resolve_input,
                                                  emit_record, &observation, &end) == -1);
    {
        int8_t type_map[21] = { 0 };
        int8_t type_pairs[2] = { 0, 0 };
        int8_t placement_map[16] = { 0 };
        int16_t pairs[4] = { 0 };
        uint8_t workspace[16 * 0x60] = { 0 };
        FA18ScenePlacementBuilderPrefixInput prefix_input = {
            0, type_map, 21, type_pairs, 2, { 0, 0 }, 0, placement_map, 16,
            pairs, 4, pairs, 4, workspace, sizeof workspace
        };
        FA18ScenePlacementBuilderPrefix prefix;
        placement_map[0] = 1;
        workspace[0x60] = 0;
        workspace[0x61] = 0x6e;
        workspace[0x66] = 0xff;
        observation = (Observation){ 0 };
        assert(fa18_emit_prefixed_workspace_placement_records(&prefix_input, &prefix,
                                                               resolve_input, emit_record,
                                                               &observation, &end) == 0);
        assert(prefix.workspace_cursor == workspace + 0x60 &&
               observation.emitted == 1 &&
               end == FA18_TERRAIN_PLACEMENT_CELL_TERMINATOR);
    }
    return 0;
}
