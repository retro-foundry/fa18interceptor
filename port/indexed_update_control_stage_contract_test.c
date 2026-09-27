#include "indexed_update_control_stage.h"

#include <assert.h>

static void called(void *context) {
    unsigned *calls = context;
    ++*calls;
}

int main(void) {
    FA18IndexedUpdateControlStageRoute route;
    unsigned calls = 0;

    assert(fa18_run_indexed_update_control_stage(1, -1, called, &calls, &route) == 0 &&
           calls == 1 && route == FA18_INDEXED_UPDATE_SELECTED_RECORD_ROUTE);
    assert(fa18_run_indexed_update_control_stage(0, 0, called, &calls, &route) == 0 &&
           calls == 2 && route == FA18_INDEXED_UPDATE_NONNEGATIVE_VALUE_ROUTE);
    assert(fa18_run_indexed_update_control_stage(0, INT32_MAX, called, &calls, &route) == 0 &&
           calls == 3 && route == FA18_INDEXED_UPDATE_NONNEGATIVE_VALUE_ROUTE);
    assert(fa18_run_indexed_update_control_stage(0, INT32_MIN, called, &calls, &route) == 0 &&
           calls == 4 && route == FA18_INDEXED_UPDATE_NEGATIVE_VALUE_CONTINUATION);
    assert(fa18_run_indexed_update_control_stage(0, 0, 0, &calls, &route) == -1);
    assert(fa18_run_indexed_update_control_stage(0, 0, called, &calls, 0) == -1);
    return 0;
}
