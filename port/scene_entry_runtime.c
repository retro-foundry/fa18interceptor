#include "scene_entry_runtime.h"
#include "scene_root_record.h"

#include <string.h>

static int build_rotation(void *context, const int16_t input[3], int16_t output[3][3]) {
    FA18SceneEntryRuntime *runtime = context;
    return runtime ? fa18_build_rotation_matrix(&runtime->trig_table, input[0], input[1],
                                                input[2], output) : -1;
}

static int compose_attitude(void *context, const int16_t input[3], int16_t output[3][3]) {
    FA18SceneEntryRuntime *runtime = context;
    FA18FlightPose pose = {0};
    if (!runtime || fa18_flight_update_attitude(&runtime->trig_table, input[0], input[1],
                                                input[2], &pose) != 0)
        return -1;
    memcpy(output, pose.attitude, sizeof pose.attitude);
    return 0;
}

static const FA18RecordMatrixUpdateOps *matrix_ops(FA18SceneEntryRuntime *runtime) {
    static FA18RecordMatrixUpdateOps ops;
    ops = (FA18RecordMatrixUpdateOps){build_rotation, compose_attitude, runtime};
    return &ops;
}

static int initialize_records(void *context) {
    FA18SceneEntryRuntime *runtime = context;
    const FA18SceneDispatchSelectionInput input = {.mode = runtime ? runtime->mode : 0};
    /* `$C28B12` starts each observed dispatch record with D7 = -2. */
    return runtime && fa18_initialize_scene_dispatch_runtime(
                          runtime->hunks, runtime->dispatch_table, &input, -2,
                          &runtime->coordinate_table, matrix_ops(runtime),
                          &runtime->dispatch_runtime) == 0 ? 0 : -1;
}

static int resolve_negative_record(void *context, uint16_t record_index,
                                   FA18SceneNegativePoseRecord *record) {
    FA18SceneEntryRuntime *runtime = context;
    return runtime ? fa18_scene_dispatch_runtime_resolve_negative_record(
                         &runtime->dispatch_runtime, record_index, record) : -1;
}

static int resolve_negative_descriptor(void *context, uint16_t record_index,
                                       FA18SceneNegativePoseDescriptor *descriptor) {
    FA18SceneEntryRuntime *runtime = context;
    return runtime ? fa18_scene_dispatch_runtime_resolve_negative_descriptor(
                         &runtime->dispatch_runtime, record_index, descriptor) : -1;
}

static int resolve_positive(void *context, uint8_t table_index,
                            FA18ScenePositivePoseInput *input) {
    FA18SceneEntryRuntime *runtime = context;
    return runtime ? fa18_resolve_scene_positive_pose(&runtime->positive_resolver,
                                                       table_index, input) : -1;
}

static int initialize_root(void *context) {
    FA18SceneEntryRuntime *runtime = context;
    FA18SceneRootPlacementOps ops;
    FA18SceneRootPlacementInput input;
    FA18SceneNegativePoseOps negative_ops;
    if (!runtime || !runtime->initialization_state) return -1;
    /* `$C092D4` reads the stage byte written by `$C0FAA4`; in the traced
     * mode-$7F route it is three and selects Hunk-67 table-A entry three.
     * `$C28722` returns with D7 = -1, which `$C09498` forwards to C2D954. */
    input = (FA18SceneRootPlacementInput){runtime->initialization_state->scene_stage, -1};
    negative_ops = (FA18SceneNegativePoseOps){0, matrix_ops(runtime), runtime};
    ops = (FA18SceneRootPlacementOps){
        resolve_negative_record,
        resolve_negative_descriptor,
        resolve_positive,
        &negative_ops, matrix_ops(runtime), runtime
    };
    /* `$C092EC` stores the caller-published `$C45849` byte at root +$62
     * before the table route writes the root pose fields. */
    runtime->dispatch_runtime.record[0].bytes[0x62] = runtime->root_type;
    if (fa18_prepare_scene_root_placement(runtime->record_table, &input,
                                          &runtime->root_placement, &ops,
                                          &runtime->root_route) != 0)
        return -1;
    return fa18_publish_scene_root_record(&runtime->dispatch_runtime.record[0],
                                          &runtime->root_placement,
                                          runtime->root_route);
}

static int prepare_message(void *context) {
    FA18SceneEntryRuntime *runtime = context;
    return runtime ? fa18_initialize_message_sequence(&runtime->message) : -1;
}

static int finalize_scene(void *context) {
    FA18SceneEntryRuntime *runtime = context;
    return runtime ? fa18_finalize_scene_state(&runtime->finalization) : -1;
}

int fa18_scene_entry_runtime_init(FA18SceneEntryRuntime *runtime,
                                  const FA18Hunks *hunks,
                                  const FA18SceneDispatchTable *dispatch_table,
                                  const FA18SceneRecordTable *record_table) {
    if (!runtime || !hunks || !dispatch_table || !record_table) return -1;
    memset(runtime, 0, sizeof *runtime);
    runtime->hunks = hunks;
    runtime->dispatch_table = dispatch_table;
    runtime->record_table = record_table;
    if (fa18_load_two_angle_trig_table(hunks, &runtime->trig_table) != 0 ||
        fa18_load_coordinate_angle_table(hunks, &runtime->coordinate_table) != 0 ||
        fa18_load_scene_positive_pose_tables(hunks, &runtime->positive_tables) != 0)
        return -1;
    runtime->positive_resolver = (FA18ScenePositivePoseResolver){
        record_table, &runtime->positive_tables
    };
    return 0;
}

int fa18_run_scene_entry_runtime(FA18SceneEntryRuntime *runtime, uint8_t mode,
                                 uint8_t root_type,
                                 FA18SceneInitializationState *state,
                                 int16_t *countdown) {
    const FA18SceneInitializationOps ops = {
        initialize_records, initialize_root, prepare_message, finalize_scene, runtime
    };
    if (!runtime || !state || !countdown) return -1;
    runtime->mode = mode;
    runtime->root_type = root_type;
    runtime->initialization_state = state;
    return fa18_initialize_scene_state(state, countdown, &ops);
}
