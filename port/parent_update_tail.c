#include "parent_update_tail.h"

static int run(FA18ParentUpdateTailStage stage, void *context) {
    return stage && stage(context) == 0 ? 0 : -1;
}

int fa18_run_parent_update_tail(FA18ParentUpdateTailState *state,
                                const FA18ParentUpdateTailOps *ops) {
    if (!state || !ops || !ops->postflight_a || !ops->postflight_b ||
        !ops->postflight_c || !ops->postflight_mode_two || !ops->tail_start_a ||
        !ops->tail_start_b || !ops->tail_start_c || !ops->modulo_seven ||
        !ops->modulo_fifteen || !ops->modulo_thirty_two ||
        !ops->activity_mode_clear || !ops->flight_state_set ||
        !ops->tail_after_counter || !ops->conditional_a || !ops->conditional_b ||
        !ops->tail_end)
        return -1;

    if (state->postflight_flag) {
        state->postflight_latch = 1;
        if (run(ops->postflight_a, ops->context) != 0 ||
            run(ops->postflight_b, ops->context) != 0 ||
            run(ops->postflight_c, ops->context) != 0 ||
            (uint8_t)(state->postflight_mode - 2u) == 0 &&
                run(ops->postflight_mode_two, ops->context) != 0)
            return -1;
    }

    state->stage_marker = UINT16_C(0x01d4);
    if (run(ops->tail_start_a, ops->context) != 0 ||
        run(ops->tail_start_b, ops->context) != 0 ||
        run(ops->tail_start_c, ops->context) != 0)
        return -1;
    if ((state->local_frame & 7u) == 7u && run(ops->modulo_seven, ops->context) != 0)
        return -1;
    if ((state->local_frame & 15u) == 4u &&
        ops->modulo_fifteen(ops->context, 0) != 0)
        return -1;
    if ((state->local_frame & 31u) == 8u) {
        if (run(ops->modulo_thirty_two, ops->context) != 0) return -1;
    } else if (!state->activity_mode && (state->local_frame & 31u) == 16u &&
               run(ops->activity_mode_clear, ops->context) != 0) {
        return -1;
    }

    if (!state->flight_state) {
        ++state->frame_counter;
    } else if (run(ops->flight_state_set, ops->context) != 0) {
        return -1;
    }
    if (run(ops->tail_after_counter, ops->context) != 0) return -1;
    if (state->tail_condition_1 && state->tail_condition_2) {
        state->stage_marker = UINT16_C(0x0210);
        if (run(ops->conditional_a, ops->context) != 0) return -1;
        state->stage_marker = UINT16_C(0x0218);
        if (run(ops->conditional_b, ops->context) != 0) return -1;
    }
    state->stage_marker = UINT16_C(0x0220);
    return run(ops->tail_end, ops->context);
}
