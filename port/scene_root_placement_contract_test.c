#include "scene_root_placement.h"

#include <assert.h>
#include <string.h>

typedef struct { unsigned record_calls, descriptor_calls, matrix_calls; } Log;

static int resolve_record(void *context, uint16_t index,
                          FA18SceneNegativePoseRecord *record) {
    Log *log = context;
    assert(index == 14);
    ++log->record_calls;
    *record = (FA18SceneNegativePoseRecord){ .flags_byte_01 = 0x40 };
    record->matrix.value[0][0] = 0x4000;
    record->matrix.value[1][1] = 0x4000;
    record->matrix.value[2][2] = 0x4000;
    return 0;
}

static int resolve_descriptor(void *context, uint16_t index,
                              FA18SceneNegativePoseDescriptor *descriptor) {
    Log *log = context;
    assert(index == 14);
    ++log->descriptor_calls;
    descriptor->value_02 = (int32_t)0x80000070u;
    return 0;
}

static int matrix_build(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->matrix_calls == 1 && input[0] == 0 && input[1] == 0 && input[2] == 0);
    output[0][0] = 0x4000;
    return 0;
}

static int matrix_compose(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->matrix_calls == 2 && !input[0] && !input[1] && !input[2]);
    output[1][1] = 0x4000;
    return 0;
}

static int resolve_positive(void *context, uint8_t index, FA18ScenePositivePoseInput *input) {
    Log *log = context;
    assert(index == 3);
    ++log->record_calls;
    *input = (FA18ScenePositivePoseInput){
        {1, 0, 0, 0, 0}, {0, 0, 0}, {0, 0}, {0, 0}
    };
    return 0;
}

int main(void) {
    uint8_t hunk_bytes[0x100] = {0};
    FA18HunkSegment segments[FA18_SCENE_RECORD_TABLE_HUNK + 1] = {{0}};
    FA18Hunks hunks = {segments, FA18_SCENE_RECORD_TABLE_HUNK + 1};
    FA18SceneRecordTable table;
    FA18SceneRootPlacementState state = {0};
    Log log = {0};
    const FA18RecordMatrixUpdateOps matrix_ops = {matrix_build, matrix_compose, &log};
    const FA18SceneNegativePoseOps negative_ops = {0, &matrix_ops, &log};
    const FA18SceneRootPlacementOps ops = {
        resolve_record, resolve_descriptor, resolve_positive, &negative_ops, &matrix_ops, &log
    };
    FA18SceneRootPlacementRoute route;

    segments[FA18_SCENE_RECORD_TABLE_HUNK] =
        (FA18HunkSegment){FA18_HUNK_DATA, hunk_bytes, sizeof hunk_bytes, 0, 0};
    hunk_bytes[FA18_SCENE_RECORD_TABLE_A_OFFSET + 3 * 16] = 0x80;
    hunk_bytes[FA18_SCENE_RECORD_TABLE_A_OFFSET + 3 * 16 + 1] = 0x0e;
    assert(fa18_load_scene_record_table(&hunks, &table) == 0);
    assert(fa18_prepare_scene_root_placement(
               &table, &(FA18SceneRootPlacementInput){3, 0}, &state, &ops, &route) == 0);
    assert(route == FA18_SCENE_ROOT_PLACEMENT_NEGATIVE_APPLIED &&
           state.selected_record_index == 14 && log.record_calls == 1 &&
           log.descriptor_calls == 1 && log.matrix_calls == 2);
    assert(state.setup.word_00 == 0x11c8 && state.pose.position[1] == 0x7708);
    hunk_bytes[FA18_SCENE_RECORD_TABLE_A_OFFSET + 3 * 16] = 0;
    hunk_bytes[FA18_SCENE_RECORD_TABLE_A_OFFSET + 3 * 16 + 1] = 1;
    log.matrix_calls = 0;
    assert(fa18_prepare_scene_root_placement(
               &table, &(FA18SceneRootPlacementInput){3, 0}, &state, &ops, &route) == 0);
    assert(route == FA18_SCENE_ROOT_PLACEMENT_POSITIVE_APPLIED &&
           state.positive_pose.position[0] == INT32_C(0x01000000));
    return 0;
}
