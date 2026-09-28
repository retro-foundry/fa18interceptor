#include "terrain_placement_pipeline.h"

enum { WORKSPACE_CELL_BYTES = 0x60 };

int fa18_emit_workspace_placement_records(
    const uint8_t *workspace_cell, size_t workspace_cell_size,
    FA18TerrainPlacementBuildInputResolve resolve, FA18TerrainPlacementEmit emit,
    void *context, FA18TerrainPlacementCellEnd *end) {
    const uint8_t *cursor;
    const uint8_t *limit;
    uint8_t ordinal = 0;

    if (!workspace_cell || !resolve || !emit || !end ||
        workspace_cell_size < WORKSPACE_CELL_BYTES)
        return -1;
    cursor = workspace_cell;
    limit = workspace_cell + WORKSPACE_CELL_BYTES;
    while (cursor < limit) {
        FA18ScenePlacementBuildInput input;
        FA18ScenePlacementBuilderTailResult result;
        uint8_t record[FA18_SCENE_PLACEMENT_BYTES];
        size_t stride;

        if (*cursor == 0xffu) {
            *end = FA18_TERRAIN_PLACEMENT_CELL_TERMINATOR;
            return 0;
        }
        if ((size_t)(limit - cursor) < 2u) return -1;
        /* Bits 4/6 suppress both `$C1DD98` and `$C1DE0A` payload reads. */
        stride = (cursor[0] & 0x50u) ? 2u : 6u;
        if ((size_t)(limit - cursor) < stride || ordinal == UINT8_MAX) return -1;
        ++ordinal; /* `$C1DD34`: ADDQ.B #1,D1 before the record is built. */
        if (resolve(context, cursor, ordinal, &input) != 0 ||
            input.workspace_item != cursor ||
            fa18_build_scene_placement_record(&input, record, &result) != 0 ||
            emit(context, record) != 0)
            return -1;
        cursor += stride;
    }
    *end = FA18_TERRAIN_PLACEMENT_CELL_END;
    return 0;
}

int fa18_emit_prefixed_workspace_placement_records(
    const FA18ScenePlacementBuilderPrefixInput *prefix_input,
    FA18ScenePlacementBuilderPrefix *prefix,
    FA18TerrainPlacementBuildInputResolve resolve, FA18TerrainPlacementEmit emit,
    void *context, FA18TerrainPlacementCellEnd *end) {
    if (!prefix || fa18_prepare_scene_placement_builder_prefix(prefix_input, prefix) != 0)
        return -1;
    return fa18_emit_workspace_placement_records(prefix->workspace_cursor,
                                                  WORKSPACE_CELL_BYTES, resolve, emit,
                                                  context, end);
}
