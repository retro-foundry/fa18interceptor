#include "post_input_command_dispatch.h"

#include <assert.h>

typedef struct { int result; unsigned calls; uint8_t mode; } Probe;
static int prepare(void *context, uint8_t mode) {
    Probe *probe = context;
    ++probe->calls;
    probe->mode = mode;
    return probe->result;
}

int main(void) {
    FA18PostInputCommandDispatchState state = {0, 3, 0, 1, 0, {0, 0, 0}, 0};
    FA18PostInputCommandDispatchRoute route;
    Probe probe = {0, 0, 0};

    assert(fa18_dispatch_post_input_commands(&state, prepare, &probe, &route) == 0);
    assert(route == FA18_POST_INPUT_COMMAND_WAIT && state.activity_flag == 1 && !probe.calls);
    state.countdown = -1;
    assert(fa18_dispatch_post_input_commands(&state, prepare, &probe, &route) == 0);
    assert(route == FA18_POST_INPUT_COMMAND_C10CFE && !state.activity_flag &&
           probe.calls == 1 && probe.mode == 3 && state.command_count == 3 &&
           state.commands[0] == 0x58 && state.commands[1] == 0x59 &&
           state.commands[2] == 0);

    state = (FA18PostInputCommandDispatchState){-1, 7, 8, 1, 0, {0, 0, 0}, 0};
    probe = (Probe){-1, 0, 0};
    assert(fa18_dispatch_post_input_commands(&state, prepare, &probe, &route) == 0);
    assert(state.command_count == 2 && state.commands[0] == 0x5a && state.commands[1] == 0);

    state = (FA18PostInputCommandDispatchState){-1, 2, 0, 1, 0, {0, 0, 0}, 0};
    assert(fa18_dispatch_post_input_commands(&state, 0, 0, &route) == 0);
    assert(state.command_count == 1 && state.commands[0] == 0 &&
           state.completion_flag == 0xff);
    assert(fa18_dispatch_post_input_commands(0, prepare, &probe, &route) == -1);
    return 0;
}
