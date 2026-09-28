#include "parent_postflight_setup.h"
#include <assert.h>

static int hit(void *context) { ++*(unsigned *)context; return 0; }

int main(void) {
    unsigned calls = 0;
    FA18ParentPostflightSetupOps ops = {hit, hit, {hit, hit, hit, hit, hit, hit, hit, hit, hit}, &calls};
    FA18ParentPostflightSetupState state = {.local_frame = 2};
    FA18ParentPostflightSetupRoute route;
    assert(fa18_run_parent_postflight_setup(&state, &ops, &route) == 0);
    assert(route == FA18_PARENT_POSTFLIGHT_CONTINUE_ACTIVITY && calls == 10 && state.stage_marker == 0xd0);
    calls = 0; state = (FA18ParentPostflightSetupState){.postflight_flag = 1};
    assert(fa18_run_parent_postflight_setup(&state, &ops, &route) == 0 &&
           route == FA18_PARENT_POSTFLIGHT_SKIP_TO_TAIL && calls == 2);
    calls = 0; state = (FA18ParentPostflightSetupState){.selected_record_type = 0x30};
    assert(fa18_run_parent_postflight_setup(&state, &ops, &route) == 0 &&
           route == FA18_PARENT_POSTFLIGHT_SKIP_TO_TAIL_CODE && calls == 2);
    ops.helper[4] = 0;
    assert(fa18_run_parent_postflight_setup(&state, &ops, &route) == -1);
    return 0;
}
