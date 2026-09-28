#include "terrain_template_static_data.h"

int fa18_load_terrain_template_static_data(const FA18Hunks *hunks,
                                           FA18TerrainTemplateStaticData *data) {
    const FA18HunkSegment *control;
    const FA18HunkSegment *groups;
    if (!hunks || !data || hunks->count <= FA18_TERRAIN_TEMPLATE_GROUP_HUNK)
        return -1;
    control = &hunks->segments[FA18_TERRAIN_TEMPLATE_CONTROL_HUNK];
    groups = &hunks->segments[FA18_TERRAIN_TEMPLATE_GROUP_HUNK];
    if (!control->data || control->size <= FA18_TERRAIN_TEMPLATE_BAND_CONTROL_OFFSET ||
        !groups->data || groups->size <= FA18_TERRAIN_TEMPLATE_GROUP_DIRECTORY_OFFSET)
        return -1;
    *data = (FA18TerrainTemplateStaticData){
        control->data + FA18_TERRAIN_TEMPLATE_BAND_CONTROL_OFFSET,
        control->size - FA18_TERRAIN_TEMPLATE_BAND_CONTROL_OFFSET,
        groups->data, groups->size, FA18_TERRAIN_TEMPLATE_GROUP_DIRECTORY_OFFSET};
    return 0;
}
