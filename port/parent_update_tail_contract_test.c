#include "parent_update_tail.h"

#include <assert.h>

typedef struct { unsigned calls[32]; unsigned count; uint32_t zero_argument; } Trace;

static int append(void *context, unsigned value) {
    Trace *trace = context;
    trace->calls[trace->count++] = value;
    return 0;
}

#define STAGE(number) static int stage_##number(void *context) { return append(context, number); }
STAGE(1) STAGE(2) STAGE(3) STAGE(4) STAGE(5) STAGE(6) STAGE(7) STAGE(8)
STAGE(10) STAGE(11) STAGE(12) STAGE(13) STAGE(14) STAGE(15) STAGE(16)

static int stage_9(void *context, uint32_t argument) {
    ((Trace *)context)->zero_argument = argument;
    return append(context, 9);
}

int main(void) {
    Trace trace = {0};
    FA18ParentUpdateTailOps ops = {
        stage_1, stage_2, stage_3, stage_4, stage_5, stage_6, stage_7,
        stage_8, stage_9, stage_10, stage_11, stage_12, stage_13, stage_14,
        stage_15, stage_16, &trace
    };
    FA18ParentUpdateTailState state = {
        .postflight_flag = 1, .postflight_mode = 2, .flight_state = 1,
        .tail_condition_1 = 1, .tail_condition_2 = 1, .local_frame = 0x8
    };

    assert(fa18_run_parent_update_tail(&state, &ops) == 0);
    assert(state.postflight_latch == 1 && state.stage_marker == 0x220);
    assert(trace.count == 13 && trace.calls[0] == 1 && trace.calls[3] == 4 &&
           trace.calls[7] == 10 && trace.calls[8] == 12 && trace.calls[9] == 13 &&
           trace.calls[10] == 14 && trace.calls[11] == 15 && trace.calls[12] == 16 &&
           state.frame_counter == 0);

    trace = (Trace){0};
    state = (FA18ParentUpdateTailState){.local_frame = 0x14};
    assert(fa18_run_parent_update_tail(&state, &ops) == 0);
    assert(trace.zero_argument == 0 && state.frame_counter == 1 &&
           trace.calls[3] == 9);

    trace = (Trace){0};
    state = (FA18ParentUpdateTailState){.local_frame = 0x10};
    assert(fa18_run_parent_update_tail(&state, &ops) == 0);
    assert(trace.calls[3] == 11);
    assert(fa18_run_parent_update_tail(0, &ops) == -1);
    ops.tail_end = 0;
    assert(fa18_run_parent_update_tail(&state, &ops) == -1);
    return 0;
}
