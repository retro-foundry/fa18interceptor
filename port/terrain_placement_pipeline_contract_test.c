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
    return 0;
}
