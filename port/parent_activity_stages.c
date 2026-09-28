#include "parent_activity_stages.h"

static int run(FA18ParentActivityStage stage, void *context) {
    return stage && stage(context) == 0 ? 0 : -1;
}

int fa18_run_parent_activity_stages(FA18ParentActivityStagesState *state,
                                    const FA18ParentActivityStagesOps *ops) {
    if (!state || !ops) return -1;
    for (unsigned index = 0; index < 13; ++index)
        if (!ops->helper[index]) return -1;

    if ((int8_t)state->activity_byte > 0 || (state->local_frame & 3u) < 2u ||
        state->signed_stage_value > -32768)
        if (run(ops->helper[0], ops->context) != 0) return -1;
    if ((int8_t)state->activity_byte > 0 || (state->local_frame & 7u) < 2u)
        if (run(ops->helper[1], ops->context) != 0) return -1;
    if ((int8_t)state->activity_byte > 0 || (state->local_frame & 15u) < 2u)
        if (run(ops->helper[2], ops->context) != 0) return -1;

    state->stage_marker = 0x01a0;
    for (unsigned index = 3; index < 13; ++index)
        if (run(ops->helper[index], ops->context) != 0) return -1;
    if ((int8_t)state->activity_byte >= 0) --state->activity_byte;
    state->stage_marker = 0x01d0;
    return 0;
}
