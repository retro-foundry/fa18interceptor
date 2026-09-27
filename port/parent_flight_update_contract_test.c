#include "parent_flight_update.h"

#include <assert.h>
#include <limits.h>

typedef struct {
    char calls[8];
    unsigned count;
    int32_t decision;
} Log;

static int append(void *context, char value) {
    Log *log = context;
    log->calls[log->count++] = value;
    return 0;
}
static int first(void *context) { return append(context, '1'); }
static int second(void *context) { return append(context, '2'); }
static int renderer(void *context) { return append(context, 'r'); }
static int branch(void *context) { return append(context, 'b'); }
static int followup(void *context) { return append(context, 'f'); }
static int decide(void *context, int32_t *result) {
    Log *log = context;
    append(context, 'd');
    *result = log->decision;
    return 0;
}

int main(void) {
    Log log = { 0 };
    FA18ParentFlightUpdateOps ops = { first, second, renderer, decide, branch, followup, &log };
    FA18ParentFlightUpdateState state = { 0, 0, 0 };
    FA18ParentFlightUpdateRoute route;

    log.decision = 1;
    assert(fa18_run_parent_flight_update(&state, &ops, &route) == 0);
    assert(route == FA18_PARENT_FLIGHT_UPDATE_DECISION_TRUE && state.stage_marker == 0xa8);
    assert(log.count == 6 && log.calls[0] == '1' && log.calls[1] == '2' &&
           log.calls[2] == 'r' && log.calls[3] == 'd' && log.calls[4] == 'b' &&
           log.calls[5] == 'f');
    log.count = 0;
    log.decision = 0;
    assert(fa18_run_parent_flight_update(&state, &ops, &route) == 0);
    assert(route == FA18_PARENT_FLIGHT_UPDATE_DECISION_FALSE && state.stage_marker == 0xb0);
    assert(log.count == 6 && log.calls[3] == 'd' && log.calls[4] == 'f' && log.calls[5] == 'b');
    log.count = 0;
    state.flight_update_flag = 1;
    assert(fa18_run_parent_flight_update(&state, &ops, &route) == 0);
    assert(route == FA18_PARENT_FLIGHT_UPDATE_FLAGGED_BRANCH && state.stage_marker == 0xa0);
    assert(log.count == 4 && log.calls[3] == 'b');
    log.count = 0;
    state.signed_stage_value = INT32_MIN;
    assert(fa18_run_parent_flight_update(&state, &ops, &route) == 0);
    assert(route == FA18_PARENT_FLIGHT_UPDATE_SKIPPED && !log.count);
    return 0;
}
