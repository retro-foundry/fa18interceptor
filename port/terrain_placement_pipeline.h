#ifndef FA18_TERRAIN_PLACEMENT_PIPELINE_H
#define FA18_TERRAIN_PLACEMENT_PIPELINE_H
#include <stddef.h>
#include <stdint.h>
#include "scene_placement_builder_tail.h"

typedef int (*FA18TerrainPlacementEmit)(void *, const uint8_t[FA18_SCENE_PLACEMENT_BYTES]);
typedef int (*FA18TerrainPlacementBuildInputResolve)(
    void *context, const uint8_t *workspace_item, uint8_t ordinal,
    FA18ScenePlacementBuildInput *input);

typedef enum {
    FA18_TERRAIN_PLACEMENT_CELL_END,
    FA18_TERRAIN_PLACEMENT_CELL_TERMINATOR
} FA18TerrainPlacementCellEnd;

/* `$C1DD22-$C1DD34`: walk exactly one selected 96-byte `$C48390` cell.  The
 * resolver supplies live work/packet/descriptor inputs for each source item;
 * this routine owns only the original terminator and flag-controlled stride. */
int fa18_emit_workspace_placement_records(
    const uint8_t *workspace_cell, size_t workspace_cell_size,
    FA18TerrainPlacementBuildInputResolve resolve, FA18TerrainPlacementEmit emit,
    void *context, FA18TerrainPlacementCellEnd *end);
#endif
