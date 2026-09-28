#include "terrain_template_workspace_pass.h"

#include <string.h>

#include "static_template_stream_selector.h"

typedef struct {
    const FA18TerrainTemplateWorkspacePassInput *input;
    uint16_t expanded_band_count;
} PassContext;

static int select_band(void *opaque, int16_t row, int16_t group,
                       uint8_t *workspace, size_t workspace_size) {
    PassContext *context = opaque;
    const FA18TerrainTemplateWorkspacePassInput *input = context->input;
    const FA18TerrainTemplateStaticData *data = input->static_data;
    FA18StaticTemplateStreamInput stream = {
        data->group_bytes, data->group_size, data->group_directory_offset,
        input->bitset_bytes, input->bitset_size, data->special_pairs,
        data->special_pair_count * 2u, workspace, workspace_size, input->append};
    FA18StaticTemplateStreamResult stream_result;
    FA18StaticTemplateStreamRoute route;

    if (fa18_select_static_template_stream(&stream, group, row, &stream_result,
                                           &route) != 0)
        return -1;
    if (route == FA18_STATIC_TEMPLATE_STREAM_EXPANDED)
        ++context->expanded_band_count;
    return 0;
}

int fa18_build_terrain_template_workspace(
    const FA18TerrainTemplateWorkspacePassInput *input,
    FA18TerrainTemplateWorkspacePassResult *result) {
    FA18TemplateBandWalkInput walk;
    PassContext context;

    if (!input || !result || !input->static_data || !input->bitset_bytes ||
        !input->workspace)
        return -1;
    memset(result, 0, sizeof *result);
    context = (PassContext){input, 0};
    walk = (FA18TemplateBandWalkInput){
        input->static_data->band_control, input->static_data->band_control_size,
        input->static_data->control_translate,
        input->static_data->control_translate_count,
        input->static_data->delta_pairs, input->static_data->delta_pair_count,
        input->row_term, input->group_term, input->append_enable,
        input->workspace, input->workspace_size, select_band, &context};
    if (fa18_walk_static_template_bands(&walk, &result->band_walk) != 0)
        return -1;
    result->expanded_band_count = context.expanded_band_count;
    return 0;
}
