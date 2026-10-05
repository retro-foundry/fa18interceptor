#include "scene_negative_pose.h"

#include <assert.h>

typedef struct { unsigned calls; } Log;

static int build(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->calls == 1);
    assert(!input[0] && input[1] == 0x6fb8 && !input[2]);
    output[0][0] = 1;
    return 0;
}
static int compose(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->calls == 2);
    assert(!input[0] && input[1] == 0x00c8 && !input[2]);
    output[2][2] = 2;
    return 0;
}

int main(void) {
    const FA18SceneNegativePoseRecord record = {
        .flags_byte_01 = 0x40,
        .matrix = {{{16, 0, 0}, {0, 16, 0}, {0, 0, 16}}},
        .angles = {0, 0x6fb8, 0}
    };
    const FA18SceneNegativePoseDescriptor descriptor = {(int32_t)0x80000070u};
    Log log = {0};
    const FA18RecordMatrixUpdateOps matrix_ops = {build, compose, &log};
    const FA18SceneNegativePoseOps ops = {NULL, &matrix_ops, NULL};
    FA18SceneNegativePoseState state = { .flags_byte_04 = 3,
                                          .matrix_update = {.control_byte_03 = 4} };
    uint8_t index = 3;
    assert(fa18_initialize_negative_scene_pose((int16_t)0x800e, &index, &record,
                                                &descriptor, 0, &state, &ops) == 0);
    assert(index == 3 && log.calls == 2 && state.word_10 == 0x77 &&
           state.position[0] == 11 && state.position[1] == 0x7708 &&
           state.position[2] == 0x68 && state.flags_byte_04 == 0xcb &&
           state.matrix_update.published[1] == 0x6fb8 &&
           state.matrix_update.control_byte_03 == 0 &&
           state.matrix_update.build_matrix[0][0] == 1);

    FA18SceneNegativePoseRecord rejected = record;
    rejected.flags_byte_01 = 0;
    index = 3;
    assert(fa18_initialize_negative_scene_pose((int16_t)0x800e, &index, &rejected,
                                                &descriptor, 0, &state, &ops) == 1 && !index);
    assert(fa18_initialize_negative_scene_pose(0, &index, &record, &descriptor, 0,
                                                &state, &ops) == -1);
    return 0;
}
