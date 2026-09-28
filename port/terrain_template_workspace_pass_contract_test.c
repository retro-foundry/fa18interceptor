#include "terrain_template_workspace_pass.h"

#include <assert.h>
#include <string.h>

int main(void) {
    uint8_t control[] = {0, 0, 0xff};
    int8_t translate[] = {0};
    int8_t deltas[] = {0, 0};
    uint8_t groups[32] = {0};
    uint8_t bitset[4] = {0, 0, 0, 1};
    uint8_t special_pairs[32] = {0};
    uint8_t workspace[0x600] = {0};
    FA18TerrainTemplateStaticData data = {
        control, sizeof control, translate, 1, groups, sizeof groups, 0,
        deltas, 1, special_pairs, 16};
    FA18TerrainTemplateWorkspacePassInput input = {
        &data, bitset, sizeof bitset, 0, 0, 0, workspace, sizeof workspace, 0};
    FA18TerrainTemplateWorkspacePassResult result;

    groups[1] = 8;
    groups[9] = 2;
    groups[15] = 20;
    groups[22] = 1;
    groups[23] = 2;
    groups[24] = 3;
    groups[25] = 4;
    groups[26] = 0xff;

    assert(fa18_build_terrain_template_workspace(&input, &result) == 0);
    assert(result.band_walk.status == 0x52);
    assert(result.band_walk.accepted == 1);
    assert(result.expanded_band_count == 1);
    assert(!memcmp(workspace, (uint8_t[]){0, 0, 1, 2, 3, 4, 0xff}, 7));
    return 0;
}
