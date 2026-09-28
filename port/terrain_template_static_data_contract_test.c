#include "terrain_template_static_data.h"

#include <assert.h>

int main(void) {
    uint8_t control[0x1bd] = {0};
    uint8_t groups[0x101] = {0};
    uint8_t helper[0x160e] = {0};
    FA18HunkSegment segments[67] = {{0}};
    FA18Hunks hunks = {segments, 67};
    FA18TerrainTemplateStaticData data;
    control[0x1bc] = 0x7e;
    segments[65] = (FA18HunkSegment){FA18_HUNK_CODE, control, sizeof control, 0, 0};
    segments[66] = (FA18HunkSegment){FA18_HUNK_CODE, groups, sizeof groups, 0, 0};
    segments[8] = (FA18HunkSegment){FA18_HUNK_CODE, helper, sizeof helper, 0, 0};
    assert(fa18_load_terrain_template_static_data(&hunks, &data) == 0);
    assert(data.band_control == control + 0x1bc && data.band_control_size == 1 &&
           data.band_control[0] == 0x7e && data.control_translate ==
           (const int8_t *)(control + 0xc0) && data.control_translate_count == 21 &&
           data.group_bytes == groups && data.group_size == sizeof groups &&
           data.group_directory_offset == 0x100 && data.delta_pairs ==
           (const int8_t *)(helper + 0x149c) && data.delta_pair_count == 21 &&
           data.special_pairs == helper + 0x15ee && data.special_pair_count == 16);
    segments[66].size = 0x100;
    assert(fa18_load_terrain_template_static_data(&hunks, &data) == -1);
    return 0;
}
