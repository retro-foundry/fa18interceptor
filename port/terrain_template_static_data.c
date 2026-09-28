#include "terrain_template_static_data.h"

int fa18_load_terrain_template_static_data(const FA18Hunks *hunks,
                                           FA18TerrainTemplateStaticData *data) {
    const FA18HunkSegment *control;
    const FA18HunkSegment *groups;
    const FA18HunkSegment *helper;
    if (!hunks || !data || hunks->count <= FA18_TERRAIN_TEMPLATE_GROUP_HUNK ||
        hunks->count <= FA18_TERRAIN_TEMPLATE_HELPER_HUNK)
        return -1;
    control = &hunks->segments[FA18_TERRAIN_TEMPLATE_CONTROL_HUNK];
    groups = &hunks->segments[FA18_TERRAIN_TEMPLATE_GROUP_HUNK];
    helper = &hunks->segments[FA18_TERRAIN_TEMPLATE_HELPER_HUNK];
    if (!control->data ||
        control->size < FA18_TERRAIN_TEMPLATE_CONTROL_TRANSLATE_OFFSET +
                            FA18_TERRAIN_TEMPLATE_CONTROL_TRANSLATE_COUNT ||
        control->size <= FA18_TERRAIN_TEMPLATE_BAND_CONTROL_OFFSET ||
        !groups->data || groups->size <= FA18_TERRAIN_TEMPLATE_GROUP_DIRECTORY_OFFSET)
        return -1;
    if (!helper->data ||
        helper->size < FA18_TERRAIN_TEMPLATE_DELTA_PAIR_OFFSET +
                           FA18_TERRAIN_TEMPLATE_DELTA_PAIR_COUNT * 2u ||
        helper->size < FA18_TERRAIN_TEMPLATE_SPECIAL_PAIR_OFFSET +
                           FA18_TERRAIN_TEMPLATE_SPECIAL_PAIR_COUNT * 2u)
        return -1;
    *data = (FA18TerrainTemplateStaticData){0};
    data->band_control = control->data + FA18_TERRAIN_TEMPLATE_BAND_CONTROL_OFFSET;
    data->band_control_size = control->size - FA18_TERRAIN_TEMPLATE_BAND_CONTROL_OFFSET;
    data->control_translate = (const int8_t *)(control->data +
                                                FA18_TERRAIN_TEMPLATE_CONTROL_TRANSLATE_OFFSET);
    data->control_translate_count = FA18_TERRAIN_TEMPLATE_CONTROL_TRANSLATE_COUNT;
    data->group_bytes = groups->data;
    data->group_size = groups->size;
    data->group_directory_offset = FA18_TERRAIN_TEMPLATE_GROUP_DIRECTORY_OFFSET;
    data->delta_pairs = (const int8_t *)(helper->data +
                                         FA18_TERRAIN_TEMPLATE_DELTA_PAIR_OFFSET);
    data->delta_pair_count = FA18_TERRAIN_TEMPLATE_DELTA_PAIR_COUNT;
    data->special_pairs = helper->data + FA18_TERRAIN_TEMPLATE_SPECIAL_PAIR_OFFSET;
    data->special_pair_count = FA18_TERRAIN_TEMPLATE_SPECIAL_PAIR_COUNT;
    return 0;
}
