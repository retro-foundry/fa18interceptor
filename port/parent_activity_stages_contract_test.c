#include "parent_activity_stages.h"
#include <assert.h>

static int hit(void *context) { ++*(unsigned *)context; return 0; }

int main(void) {
    unsigned calls = 0;
    FA18ParentActivityStagesOps ops = {{hit, hit, hit, hit, hit, hit, hit, hit, hit, hit, hit, hit, hit}, &calls};
    FA18ParentActivityStagesState state = {.local_frame = 2, .signed_stage_value = -32768};
    assert(fa18_run_parent_activity_stages(&state, &ops) == 0);
    assert(calls == 10 && state.activity_byte == 0xff && state.stage_marker == 0x1d0);
    calls = 0; state = (FA18ParentActivityStagesState){.activity_byte = 1, .local_frame = 15};
    assert(fa18_run_parent_activity_stages(&state, &ops) == 0 && calls == 13 && state.activity_byte == 0);
    calls = 0; state = (FA18ParentActivityStagesState){.activity_byte = 0x80, .local_frame = 15, .signed_stage_value = -32768};
    assert(fa18_run_parent_activity_stages(&state, &ops) == 0 && calls == 10 && state.activity_byte == 0x80);
    ops.helper[12] = 0;
    assert(fa18_run_parent_activity_stages(&state, &ops) == -1);
    return 0;
}
