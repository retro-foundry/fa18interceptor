#include "scene_placement_builder_prefix.h"

#include <assert.h>
#include <string.h>

int main(void) {
    int8_t type_map[21] = { 0 };
    int8_t type_pairs[4] = { 0 };
    int8_t placement_map[16] = { 0 };
    int16_t workspace_pairs[8] = { 0 };
    int16_t translation_pairs[8] = { 0 };
    uint8_t workspace[16 * 0x60] = { 0 };
    FA18ScenePlacementBuilderPrefixInput input = {
        3, type_map, 21, type_pairs, 4, { -2, 5 }, 2, placement_map, 16,
        workspace_pairs, 8, translation_pairs, 8, workspace, sizeof workspace
    };
    FA18ScenePlacementBuilderPrefix output;

    type_map[3] = 1;
    type_pairs[2] = -5;
    type_pairs[3] = 12;
    placement_map[2] = 3;
    workspace_pairs[3] = -16;
    workspace_pairs[4] = 24;
    translation_pairs[3] = 7;
    translation_pairs[4] = -9;
    assert(fa18_prepare_scene_placement_builder_prefix(&input, &output) == 0);
    assert(output.selector_packet == 0x007911);
    assert(output.workspace_component[0] == -64 && output.workspace_component[1] == 96);
    assert(output.translation_component[0] == 28 && output.translation_component[1] == -36);
    assert(output.workspace_cursor == workspace + 3 * 0x60);
    assert(output.workspace_end == workspace + 4 * 0x60);
    input.type_selector = 21;
    assert(fa18_prepare_scene_placement_builder_prefix(&input, &output) == -1);
    input.type_selector = 3;
    placement_map[2] = 16;
    assert(fa18_prepare_scene_placement_builder_prefix(&input, &output) == -1);
    memset(workspace, 0, sizeof workspace);
    return 0;
}
