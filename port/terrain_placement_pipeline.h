#ifndef FA18_TERRAIN_PLACEMENT_PIPELINE_H
#define FA18_TERRAIN_PLACEMENT_PIPELINE_H
#include <stddef.h>
#include <stdint.h>
#include "scene_placement_builder_tail.h"
typedef int (*FA18TerrainPlacementEmit)(void *, const uint8_t[FA18_SCENE_PLACEMENT_BYTES]);
/* Caller supplies the source-selected workspace items and their live terms. */
int fa18_emit_workspace_placement_records(const uint8_t *workspace,size_t workspace_size,
 const FA18ScenePlacementBuildInput *items,size_t item_count,FA18TerrainPlacementEmit emit,void *context);
#endif
