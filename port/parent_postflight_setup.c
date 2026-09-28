#include "parent_postflight_setup.h"

static int run(FA18ParentPostflightStage stage, void *context) {
    return stage && stage(context) == 0 ? 0 : -1;
}

int fa18_run_parent_postflight_setup(FA18ParentPostflightSetupState *state,
                                     const FA18ParentPostflightSetupOps *ops,
                                     FA18ParentPostflightSetupRoute *route) {
    if (!state || !ops || !route || !ops->prepare || !ops->secondary_prepare)
        return -1;
    for (unsigned index = 0; index < 9; ++index)
        if (!ops->helper[index]) return -1;

    state->stage_marker = 0xc0;
    if (run(ops->prepare, ops->context) != 0 ||
        run(ops->secondary_prepare, ops->context) != 0)
        return -1;
    state->stage_marker = 0xd0;
    if (state->postflight_flag && !state->mode_flag) {
        *route = FA18_PARENT_POSTFLIGHT_SKIP_TO_TAIL;
        return 0;
    }
    if (state->selected_record_type == 0x30 && !state->mode_flag) {
        *route = FA18_PARENT_POSTFLIGHT_SKIP_TO_TAIL_CODE;
        return 0;
    }
    for (unsigned index = 0; index < 6; ++index)
        if (run(ops->helper[index], ops->context) != 0) return -1;
    if ((int8_t)state->activity_byte > 0 || (state->local_frame & 3u) > 1u)
        for (unsigned index = 6; index < 8; ++index)
            if (run(ops->helper[index], ops->context) != 0) return -1;
    if ((int8_t)state->activity_byte > 0 || (state->local_frame & 3u) < 2u)
        if (run(ops->helper[8], ops->context) != 0) return -1;
    *route = FA18_PARENT_POSTFLIGHT_CONTINUE_ACTIVITY;
    return 0;
}
