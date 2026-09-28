#include "scene_positive_pose.h"

#include <assert.h>

typedef struct { unsigned calls; } Log;

static int build(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->calls == 1);
    assert(input[0] == 0 && input[1] == 2240 && input[2] == 0);
    output[0][0] = 1;
    return 0;
}

static int compose(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->calls == 2);
    assert(input[0] == (int16_t)0x67c0 && input[1] == 0 && input[2] == (int16_t)0x67c0);
    output[2][2] = 2;
    return 0;
}

int main(void) {
    const FA18ScenePositivePoseInput input = {
        {16, 16, 6, 1184, 2528}, {7570, -14257, 28}, {1, 2}, {4096, 8192}
    };
    Log log = {0};
    const FA18RecordMatrixUpdateOps ops = {build, compose, &log};
    FA18ScenePositivePoseState state = {.matrix_update = {.control_byte_03 = 4}};

    assert(fa18_initialize_positive_scene_pose(&input, &state, &ops) == 0);
    assert(log.calls == 2 && state.byte_0b == 6 && state.word_06 == 65 &&
           state.word_08 == 66 &&
           state.position[0] == 273963296 && state.position[1] == 0x708 &&
           state.position[2] == 279184624 && state.word_0c == 5209 &&
           state.word_0e == 9220 && state.published_delta[0] == -1333536 &&
           state.published_delta[1] == -0x708 && state.published_delta[2] == -2360560 &&
           state.matrix_update.published[1] == 2240 &&
           state.matrix_update.attitude_matrix[2][2] == 2);

    FA18ScenePositivePoseInput negative = input;
    negative.entry_words[0] = -1;
    assert(fa18_initialize_positive_scene_pose(&negative, &state, &ops) == -1);
    return 0;
}
