#include "context_refresh_packet.h"

#include <assert.h>

typedef struct {
    unsigned selector, a, b, clear, set, render, error;
    FA18ContextRefreshPacketState *state;
    int consume_later, change_stage;
} Log;
static int flag_records(void *context) { (void)context; return 0; }
static int error(void *context, uint16_t code) { Log *log = context; assert(code == 0x27); ++log->error; return 0; }
static int selector(void *context) {
    Log *log = context;
    ++log->selector;
    if (log->consume_later) log->state->request_bits &= (uint8_t)~0x0au;
    return 0;
}
static int a(void *context) { ++((Log *)context)->a; return 0; }
static int b(void *context) { ++((Log *)context)->b; return 0; }
static int clear(void *context) { ++((Log *)context)->clear; return 0; }
static int set(void *context) {
    Log *log = context;
    ++log->set;
    if (log->change_stage) log->state->stage_c_selector = 0;
    return 0;
}
static int render(void *context) { ++((Log *)context)->render; return 0; }

int main(void) {
    uint8_t record[10] = {0,0,0,0,0,0, 0,8, 0,12};
    Log log = {0};
    FA18ContextRefreshPacketState state = {
        0, 0x0b, 0, 0, 0, 0, record, sizeof record, {0,0,0},
        0,0,0,0,1,0,1,0,0,0};
    FA18ContextRefreshPacketOps ops = {
        flag_records, error, selector, a, b, clear, set, render, &log};
    assert(fa18_run_context_refresh_packet(&state, &ops) == 0);
    assert(state.selector_x == 2 && state.selector_z == 3 && !state.request_bits &&
           !state.prepared_flag && state.callback_a_flag && state.callback_b_flag &&
           state.trace_word == 0x4e && state.render_flag == 0x000fffff &&
           state.render_selector == 0x0f && log.selector == 3 && log.a == 1 &&
           log.b == 1 && !log.clear && log.set == 1 && log.render == 1);
    state.guard_long = (int32_t)UINT32_C(0xf7ffffff);
    assert(fa18_run_context_refresh_packet(&state, &ops) == 0 && log.a == 1);
    state.guard_long = 0; state.request_bits = 0x0b;
    state.context_selection = 1;
    state.origin[0] = (int32_t)UINT32_C(0x1fffffff);
    state.origin[2] = (int32_t)UINT32_C(0x01000000);
    state.stage_c_selector = 1;
    log = (Log){0}; log.state = &state;
    log.consume_later = log.change_stage = 1;
    assert(fa18_run_context_refresh_packet(&state, &ops) == 0);
    assert(state.selector_x == 31 && state.selector_z == 1);
    assert(log.selector == 1 && !state.request_bits && log.set == 1);
    assert(state.stage_c_selector == 0 && state.trace_word == 0x4e);
    return 0;
}
