#include "outer_update_loop.h"

#include <assert.h>

typedef struct { unsigned calls[16]; unsigned count; uint32_t delay; } Trace;

static int append(void *context, unsigned value) {
    Trace *trace = context;
    trace->calls[trace->count++] = value;
    return 0;
}

static int acquire(void *context) { return append(context, 1); }
static int select_renderer_page(void *context) { return append(context, 3); }
static int own(void *context) { return append(context, 4); }
static int disown(void *context) { return append(context, 5); }
static int parent(void *context) { return append(context, 6); }
static int wait_display(void *context) { return append(context, 7); }
static int child(void *context) { return append(context, 8); }

static int delay(void *context, uint32_t argument) {
    Trace *trace = context;
    trace->delay = argument;
    return append(context, 2);
}

int main(void) {
    Trace trace = {0};
    FA18OuterUpdateLoopState state = {0};
    FA18OuterUpdateLoopOps ops = {
        acquire, delay, select_renderer_page, own, disown, parent, wait_display, child, &trace
    };

    assert(fa18_run_outer_update_loop_iteration(&state, &ops) == -1);
    assert(fa18_initialize_outer_update_loop(&state, &ops) == 0);
    assert(state.initialized && !state.completed_iterations && trace.count == 2 &&
           trace.delay == 0x186a0 && trace.calls[0] == 1 && trace.calls[1] == 2);
    assert(fa18_run_outer_update_loop_iteration(&state, &ops) == 0);
    assert(state.completed_iterations == 1 && trace.count == 8 &&
           trace.calls[2] == 3 && trace.calls[3] == 4 && trace.calls[4] == 5 &&
           trace.calls[5] == 6 && trace.calls[6] == 7 && trace.calls[7] == 8);
    assert(fa18_run_outer_update_loop_iteration(&state, &ops) == 0);
    assert(state.completed_iterations == 2 && trace.count == 14);
    ops.wait_display = 0;
    assert(fa18_run_outer_update_loop_iteration(&state, &ops) == -1);
    return 0;
}
