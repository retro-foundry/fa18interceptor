#include "post_input_tick.h"

static int invoke_invalid(const FA18PostInputTickHooks *hooks) {
    return hooks && hooks->invalid_offset ? hooks->invalid_offset(hooks->context) : -2;
}
static int invoke_callback(const FA18PostInputTickState *state,
                           const FA18PostInputTickHooks *hooks) {
    return hooks && hooks->call_callback
        ? hooks->call_callback(hooks->context, state->callback) : -3;
}

int fa18_run_post_input_tick(FA18PostInputTickState *state,
                             const FA18PostInputTickHooks *hooks) {
    int result = 0;
    if (!state) return -1;
    if (state->entry_guard >= 0 && state->enable > 0) {
        if (!(state->phase == 0 || state->secondary_guard != 0 ||
              state->signed_guard >= 0 || state->primary_offset == 0)) {
            uint32_t offset = state->counter_source - state->primary_offset -
                              state->tertiary_offset;
            if (state->secondary_offset != 0)
                offset -= state->counter_source - state->secondary_offset;
            offset -= state->quaternary_offset;
            if (state->additional_offset != 0)
                offset -= state->counter_source - state->additional_offset;
            /* C0F69A compares against #$4650, hexadecimal 18,000. */
            if ((int32_t)offset < 0 || (int32_t)offset >= 0x4650) {
                state->result_code = 0x003f;
                result = invoke_invalid(hooks);
            } else {
                state->result_target_offset8 += offset;
                state->result_flag = 1;
            }
            state->primary_offset = 0;
        }
        if (state->later_guard >= 0) {
            if (state->phase == 0xff) {
                state->phase = state->phase_flag = 0;
                state->callback = FA18_POST_INPUT_CALLBACK_C0F920;
            } else if (state->phase == 1) {
                state->phase = 0;
                if (state->mode_flag != 0) state->phase_flag = 1;
                state->event_flag = 1; state->countdown = 3;
                state->auxiliary_byte = state->mode_byte = 0;
                state->callback = FA18_POST_INPUT_CALLBACK_C0F946;
            } else if (state->phase == 2) {
                state->phase_flag = 0;
                if (state->signed_guard < 0) {
                    state->later_guard = (int8_t)0xf0; state->phase = 0;
                    state->mode_latch = state->event_flag = 1; state->countdown = 6;
                    state->callback = FA18_POST_INPUT_CALLBACK_C1104C;
                }
            }
        } else if (state->phase == 3 && state->signed_guard < 0) {
            state->phase_flag = state->phase = 0;
            state->countdown = state->configured_countdown;
            state->callback = FA18_POST_INPUT_CALLBACK_C11078;
        }
    }
    /* `$C0F7D2-$C0F811`, also reached after the invalid-offset call. */
    if (state->signed_guard >= 0)
        state->signed_guard = (int8_t)((uint8_t)state->signed_guard - 1u);
    ++state->tick_count;
    --state->countdown;
    { int callback_result = invoke_callback(state, hooks);
      if (result == 0 && callback_result != 0) result = callback_result; }
    state->command_input_pending = 0;
    return result;
}
