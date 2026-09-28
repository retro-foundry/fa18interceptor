#include "context_refresh_packet.h"

#include <assert.h>

typedef struct { unsigned selector, a, b, clear, set, render, error; } Log;
static int classify(void *context, uint8_t *value) { (void)context; *value = 0x0c; return 0; }
static int error(void *context, uint16_t code) { Log *log = context; assert(code == 0x27); ++log->error; return 0; }
static int selector(void *context) { ++((Log *)context)->selector; return 0; }
static int a(void *context) { ++((Log *)context)->a; return 0; }
static int b(void *context) { ++((Log *)context)->b; return 0; }
static int clear(void *context) { ++((Log *)context)->clear; return 0; }
static int set(void *context) { ++((Log *)context)->set; return 0; }
static int render(void *context) { ++((Log *)context)->render; return 0; }

int main(void) {
    uint8_t record[10] = {0,0,0,0,0,0, 0,8, 0,12};
    Log log = {0};
    FA18ContextRefreshPacketState state = {
        0, 0x0b, 0, 0, 0, 0, record, sizeof record, {0,0,0},
        0,0,0,0,1,0,1,0,0,0};
    FA18ContextRefreshPacketOps ops = {
        classify, error, selector, a, b, clear, set, render, &log};
    assert(fa18_run_context_refresh_packet(&state, &ops) == 0);
    assert(state.selector_x == 2 && state.selector_z == 3 && !state.request_bits &&
           !state.prepared_flag && state.callback_a_flag && state.callback_b_flag &&
           state.trace_word == 0x4e && state.render_flag == 0x000fffff &&
           state.render_selector == 0x0f && log.selector == 3 && log.a == 1 &&
           log.b == 1 && !log.clear && log.set == 1 && log.render == 1);
    state.guard_long = (int32_t)UINT32_C(0xf7ffffff);
    assert(fa18_run_context_refresh_packet(&state, &ops) == 0 && log.a == 1);
    return 0;
}
