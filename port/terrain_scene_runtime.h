#ifndef FA18_TERRAIN_SCENE_RUNTIME_H
#define FA18_TERRAIN_SCENE_RUNTIME_H

#include "terrain_placement_cache.h"
#include "terrain_template_selector_pass.h"

typedef struct {
    FA18TerrainTemplateSelectorPassInput selector;
    FA18ScenePlacementBuilderPrefixInput placement_prefix;
    FA18TerrainPlacementDirectInput placement_direct;
    FA18TerrainPlacementCache *cache;
} FA18TerrainSceneRuntimeInput;

typedef struct {
    FA18TerrainTemplateSelectorPassResult selector;
    FA18TerrainPlacementDirectResult placement;
} FA18TerrainSceneRuntimeResult;

/* `$C1D330-$C1E11A`: static template selection expands the caller-owned
 * workspace, then the direct builder emits its mutable placement cache. */
int fa18_run_terrain_scene_runtime(const FA18TerrainSceneRuntimeInput *input,
                                   FA18TerrainSceneRuntimeResult *result);

#endif
