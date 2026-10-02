#include "parent_flight_update.h"

static int run_stage(FA18ParentFlightUpdateStage stage, void *context) {
    return stage && stage(context) == 0 ? 0 : -1;
}

int fa18_run_parent_flight_update(FA18ParentFlightUpdateState *state,
                                  const FA18ParentFlightUpdateOps *ops,
                                  FA18ParentFlightUpdateRoute *route) {
    int32_t decision;
    if (!state || !ops || !route || !ops->first_update_stage ||
        !ops->second_update_stage || !ops->renderer_packet ||
        !ops->branch_stage || !ops->followup_stage)
        return -1;
    /* C0F090 compares against $F8000000, then branches on signed <=. */
    if (state->signed_stage_value <= -0x08000000) {
        *route = FA18_PARENT_FLIGHT_UPDATE_SKIPPED;
        return 0;
    }
    state->stage_marker = 0x78;
    if (run_stage(ops->first_update_stage, ops->context) != 0) return -1;
    state->stage_marker = 0x80;
    if (run_stage(ops->second_update_stage, ops->context) != 0) return -1;
    state->stage_marker = 0x90;
    if (run_stage(ops->renderer_packet, ops->context) != 0) return -1;
    state->stage_marker = 0xa0;
    if (state->flight_update_flag) {
        if (run_stage(ops->branch_stage, ops->context) != 0) return -1;
        *route = FA18_PARENT_FLIGHT_UPDATE_FLAGGED_BRANCH;
        return 0;
    }
    if (!ops->decision || ops->decision(ops->context, &decision) != 0) return -1;
    if (decision) {
        state->stage_marker = 0xa4;
        if (run_stage(ops->branch_stage, ops->context) != 0) return -1;
        state->stage_marker = 0xa8;
        if (run_stage(ops->followup_stage, ops->context) != 0) return -1;
        *route = FA18_PARENT_FLIGHT_UPDATE_DECISION_TRUE;
    } else {
        state->stage_marker = 0xac;
        if (run_stage(ops->followup_stage, ops->context) != 0) return -1;
        state->stage_marker = 0xb0;
        if (run_stage(ops->branch_stage, ops->context) != 0) return -1;
        *route = FA18_PARENT_FLIGHT_UPDATE_DECISION_FALSE;
    }
    return 0;
}
