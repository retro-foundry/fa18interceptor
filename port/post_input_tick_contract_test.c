#include "post_input_tick.h"
#include <stdio.h>
typedef struct { unsigned invalid_calls, callback_calls; FA18PostInputCallback callback; } Calls;
static int invalid(void *p) { ++((Calls *)p)->invalid_calls; return 0; }
static int callback(void *p, FA18PostInputCallback c) { Calls *x = p; ++x->callback_calls; x->callback = c; return 0; }
int main(void) {
    Calls calls = {0}; FA18PostInputTickHooks hooks = {callback, invalid, &calls};
    FA18PostInputTickState state = {.entry_guard = -1, .signed_guard = 0, .tick_count = 0xff, .countdown = 0, .command_input_pending = 1, .callback = FA18_POST_INPUT_CALLBACK_EXISTING};
    if (fa18_run_post_input_tick(&state, &hooks) || state.signed_guard != -1 || state.tick_count || state.countdown != 0xffff || state.command_input_pending || calls.callback_calls != 1) goto fail;
    state = (FA18PostInputTickState){.enable = 1, .phase = 1, .mode_flag = 1, .signed_guard = 4, .countdown = 22}; calls = (Calls){0};
    if (fa18_run_post_input_tick(&state, &hooks) || state.phase || state.phase_flag != 1 || !state.event_flag || state.countdown != 2 || state.callback != FA18_POST_INPUT_CALLBACK_C0F946 || calls.callback != FA18_POST_INPUT_CALLBACK_C0F946) goto fail;
    state = (FA18PostInputTickState){.enable = 1, .phase = 3, .later_guard = -1, .signed_guard = -1, .configured_countdown = 0, .command_input_pending = 1}; calls = (Calls){0};
    if (fa18_run_post_input_tick(&state, &hooks) || state.phase || state.phase_flag || state.countdown != 0xffff || state.callback != FA18_POST_INPUT_CALLBACK_C11078 || state.command_input_pending) goto fail;
    state = (FA18PostInputTickState){.enable = 1, .phase = 1, .signed_guard = -1, .counter_source = 100, .primary_offset = 1, .tertiary_offset = 1, .result_target_offset8 = 10}; calls = (Calls){0};
    if (fa18_run_post_input_tick(&state, &hooks) || state.primary_offset || state.result_target_offset8 != 108 || state.result_flag != 1 || calls.invalid_calls) goto fail;
    state = (FA18PostInputTickState){.enable = 1, .phase = 1, .signed_guard = -1, .primary_offset = 1}; calls = (Calls){0};
    if (fa18_run_post_input_tick(&state, &hooks) || state.primary_offset || state.result_code != 0x003f || calls.invalid_calls != 1) goto fail;
    /* Original C0F69A uses hexadecimal $4650. Values beyond decimal 4650
     * remain valid through 17,999; 18,000 takes the returning fault edge. */
    {
        static const uint32_t offsets[] = {5000, 17999, 18000};
        unsigned i;
        for (i = 0; i < 3; ++i) {
            state = (FA18PostInputTickState){.enable = 1, .phase = 1,
                .signed_guard = -1, .counter_source = offsets[i] + 1,
                .primary_offset = 1, .result_target_offset8 = 10};
            calls = (Calls){0};
            if (fa18_run_post_input_tick(&state, &hooks) || state.primary_offset ||
                calls.invalid_calls != (i == 2) || calls.callback_calls != 1 ||
                (i < 2 && (state.result_target_offset8 != offsets[i] + 10 ||
                           state.result_flag != 1)) ||
                (i == 2 && (state.result_code != 0x003f ||
                            state.result_target_offset8 != 10))) goto fail;
        }
    }
    puts("post-input tick contract passed"); return 0;
fail: fputs("post-input tick contract failed\n", stderr); return 1;
}
