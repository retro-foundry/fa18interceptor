#include "terrain_scene_runtime.h"

int fa18_run_terrain_scene_runtime(const FA18TerrainSceneRuntimeInput *input,
                                   FA18TerrainSceneRuntimeResult *result) {
    if (!input || !result || !input->cache ||
        !input->selector.workspace ||
        input->selector.workspace != input->placement_prefix.workspace ||
        input->selector.workspace_size != input->placement_prefix.workspace_size)
        return -1;
    if (fa18_run_terrain_template_selector_pass(&input->selector, &result->selector) != 0)
        return -1;
    return fa18_build_terrain_placement_cache(&input->placement_prefix,
                                              &input->placement_direct,
                                              input->cache, &result->placement);
}
