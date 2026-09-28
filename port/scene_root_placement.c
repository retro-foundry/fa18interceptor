#include "scene_root_placement.h"

int fa18_prepare_scene_root_placement(
    const FA18SceneRecordTable *table,
    const FA18SceneRootPlacementInput *input,
    FA18SceneRootPlacementState *state,
    const FA18SceneRootPlacementOps *ops,
    FA18SceneRootPlacementRoute *route) {
    int16_t words[FA18_SCENE_RECORD_TABLE_ENTRY_WORDS];
    FA18SceneNegativePoseRecord selected;
    FA18SceneNegativePoseDescriptor descriptor;
    int result;

    if (!table || !input || !state || !ops || !route ||
        !ops->resolve_record || !ops->resolve_descriptor ||
        !ops->negative_pose_ops ||
        fa18_initialize_scene_root_setup(&state->setup) != 0 ||
        fa18_scene_record_table_a_entry(table, input->table_index, words) != 0)
        return -1;

    if (words[0] >= 0) {
        *route = FA18_SCENE_ROOT_PLACEMENT_POSITIVE_UNPORTED;
        return 0;
    }
    state->selected_record_index = (uint16_t)words[0] & UINT16_C(0x7fff);
    if (ops->resolve_record(ops->context, state->selected_record_index, &selected) != 0 ||
        ops->resolve_descriptor(ops->context, state->selected_record_index,
                                &descriptor) != 0)
        return -1;
    result = fa18_initialize_negative_scene_pose(
        words[0], &state->retry_scene_index, &selected, &descriptor,
        input->inherited_d7, &state->pose, ops->negative_pose_ops);
    if (result < 0) return -1;
    *route = result == 0 ? FA18_SCENE_ROOT_PLACEMENT_NEGATIVE_APPLIED
                         : FA18_SCENE_ROOT_PLACEMENT_NEGATIVE_RETRY;
    return 0;
}
