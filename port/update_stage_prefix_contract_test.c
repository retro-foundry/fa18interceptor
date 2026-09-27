#include "update_stage_prefix.h"

#include <assert.h>

static int called(void *context) {
    unsigned *count = context;
    ++*count;
    return 0;
}

static int failed(void *context) {
    (void)context;
    return -1;
}

int main(void) {
    FA18UpdateStagePrefixState state = { 1, 0, 0, -0x00200000, 0, 0, 0 };
    unsigned calls = 0;
    uint8_t changed = 0;
    assert(fa18_prepare_update_stage_prefix(&state, called, &calls, &changed) == 0);
    assert(calls == 1 && changed == 0x0b && state.input_byte_mirror == 1 &&
           state.long_mirror == 0x00200000 && state.scaled_word == 1);

    state.input_byte = 1;
    state.change_inhibit = 1;
    state.signed_long = -0x00100000;
    state.long_mirror = 0x00200000;
    state.scaled_word = 0;
    state.mode_byte = 2;
    changed = 0xff;
    assert(fa18_prepare_update_stage_prefix(&state, called, &calls, &changed) == 0);
    assert(calls == 2 && changed == 0 && state.long_mirror == 0x00100000 &&
           state.scaled_word == 0);

    state.signed_long = INT32_MIN;
    state.long_mirror = 0;
    state.mode_byte = 0;
    assert(fa18_prepare_update_stage_prefix(&state, called, &calls, &changed) == 0);
    assert(changed == 0x0b && state.long_mirror == INT32_MIN &&
           state.scaled_word == -1024);
    assert(fa18_prepare_update_stage_prefix(&state, failed, &calls, &changed) == -1);
    assert(fa18_prepare_update_stage_prefix(0, called, &calls, &changed) == -1);
    return 0;
}
