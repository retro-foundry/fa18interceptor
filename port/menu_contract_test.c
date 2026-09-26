#include "menu.h"

#include <stdio.h>

int main(void) {
    /* Actual pre/post values from build/port_run075_c0fd10_trace/{slow.bin,
     * trace_final_slow.bin}; report: analysis/routines/c0fcb4_run075_demo.md.
     */
    FA18MenuState state = {
        .selected_mode = 0x00,
        .video_auxiliary = 0x00,
        .video_flags = 0xf7,
        .delay_ticks = (int16_t)0xef0a,
        .callback = FA18_MENU_CALLBACK_SELECTION_FOLLOWUP,
    };
    /* Actual $C1BD78 pre/post values at run075 direct-core frame 230. */
    if (fa18_select_run075_demo_mode(&state) != 0 ||
        state.selected_mode != 0x7f || state.selection_marker != 1) {
        fputs("run075 frame-230 demo mode selection contract failed\n", stderr);
        return 1;
    }
    if (fa18_schedule_demo_selection(&state) != 0 ||
        state.selected_mode != 0x7f ||
        state.selector_queue[0] != 101 || state.selector_queue[1] != 0 ||
        state.delay_ticks != 0x00d2 ||
        state.callback != FA18_MENU_CALLBACK_DELAYED_TRANSITION) {
        fputs("run075 frame-234 menu transition contract failed\n", stderr);
        return 1;
    }
    /* $C0FECE uses a signed test: run075 reaches the transition body only
     * after the common tick changes the delay from zero to $FFFF. */
    state.delay_ticks = 0;
    if (fa18_menu_delay_expired(&state)) {
        fputs("nonnegative menu delay incorrectly expired\n", stderr);
        return 1;
    }
    state.delay_ticks = -1;
    if (!fa18_menu_delay_expired(&state)) {
        fputs("negative menu delay did not expire\n", stderr);
        return 1;
    }
    /* $C0F7E4-$C0F7EE increments a byte and performs SUBQ.W #1 on delay. */
    state.post_input_tick_count = 0xff;
    state.delay_ticks = 0;
    fa18_menu_post_input_tick(&state);
    if (state.post_input_tick_count != 0 || state.delay_ticks != -1 ||
        !fa18_menu_delay_expired(&state)) {
        fputs("post-input tick delay update contract failed\n", stderr);
        return 1;
    }
    state.delay_ticks = INT16_MIN;
    fa18_menu_post_input_tick(&state);
    if (state.delay_ticks != INT16_MAX) {
        fputs("post-input tick word-wrap contract failed\n", stderr);
        return 1;
    }
    /* The alternate delay is byte-exact static source, not a run075 path. */
    state.video_flags = 0;
    if (fa18_schedule_demo_selection(&state) != 0 || state.delay_ticks != 0x0096) {
        fputs("menu standard-delay source contract failed\n", stderr);
        return 1;
    }
    /* Actual run075 frame-270 common-setup and frame-271 demo-arm fixture. */
    state.delay_ticks = -1;
    state.transition_stage = 0;
    state.post_input_phase = 0;
    state.transition_auxiliary = 0;
    state.transition_row_limit = 0;
    state.demo_followup_pending = 1;
    if (fa18_prepare_run075_demo_transition(&state) != 0 ||
        state.delay_ticks != 4 || state.transition_stage != 3 ||
        state.post_input_phase != 2 || state.transition_auxiliary != 1 ||
        state.transition_row_limit != 179 || state.demo_followup_pending != 0 ||
        state.callback != FA18_MENU_CALLBACK_DEMO_SETUP) {
        fputs("run075 frame-270 common demo transition contract failed\n", stderr);
        return 1;
    }
    if (fa18_enter_demo_followup(&state) != 0 ||
        state.demo_followup_pending != 1 || state.delay_ticks != 4 ||
        state.callback != FA18_MENU_CALLBACK_DEMO_ENTRY) {
        fputs("run075 frame-271 demo followup contract failed\n", stderr);
        return 1;
    }
    /* Byte-exact $C0FA4C and $C0FA80 state gates. These are not assigned to
     * run075 without a matching callback-use trace. */
    state.callback = FA18_MENU_CALLBACK_DEMO_FOLLOWUP_MATCH;
    state.delay_ticks = -1;
    state.transition_auxiliary = 1;
    state.followup_input = 7;
    state.followup_expected_input = 7;
    if (fa18_advance_demo_followup_match(&state) != 1 ||
        state.transition_auxiliary != 0 || state.delay_ticks != 2 ||
        state.callback != FA18_MENU_CALLBACK_DEMO_FOLLOWUP_COMPLETE) {
        fputs("post-input followup match contract failed\n", stderr);
        return 1;
    }
    state.delay_ticks = -1;
    state.demo_followup_pending = 1;
    if (fa18_complete_demo_followup(&state) != 1 ||
        state.demo_followup_pending != 0 || state.transition_auxiliary != 1 ||
        state.callback != FA18_MENU_CALLBACK_DEMO_CONTINUATION) {
        fputs("post-input followup completion contract failed\n", stderr);
        return 1;
    }
    puts("run075 menu transition contract passed");
    return 0;
}
