#include "record_matrix_update.h"

#include <assert.h>

typedef struct { unsigned calls; } Log;

static int build(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->calls == 1);
    assert(input[0] == 0 && input[1] == 0x6fb8 && input[2] == 0);
    output[0][0] = 1;
    return 0;
}

static int compose(void *context, const int16_t input[3], int16_t output[3][3]) {
    Log *log = context;
    assert(++log->calls == 2);
    assert(input[0] == 0x00c8 && input[1] == 0 && input[2] == 0);
    output[2][2] = 2;
    return 0;
}

int main(void) {
    FA18RecordMatrixUpdateState state = { .control_byte_03 = 0xff };
    const FA18RecordMatrixUpdateInput input = { 0, 0x6fb8, 0, 0 };
    Log log = {0};
    const FA18RecordMatrixUpdateOps ops = { build, compose, &log };
    assert(fa18_update_record_matrix(&state, &input, &ops) == 0);
    assert(log.calls == 2 && state.control_byte_03 == 0xfbu &&
           state.published[0] == 0 && state.published[1] == 0x6fb8 &&
           state.published[2] == 0 && state.build_matrix[0][0] == 1 &&
           state.attitude_matrix[2][2] == 2);
    assert(fa18_update_record_matrix(NULL, &input, &ops) == -1);
    return 0;
}
