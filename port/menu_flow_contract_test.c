#include "menu_flow.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    FA18MenuFlow flow;
    FA18ReplayControlState controls = {0};
    FA18Video video = {0};
    memset(video.pixels, 5, sizeof video.pixels);
    fa18_menu_flow_init(&flow);
    controls.keyboard_down[FA18_FRONTEND_KEY_1] = 1;
    if (fa18_menu_flow_apply_controls(&flow, &controls, &video) != 0 ||
        flow.selected_mode != 0x7f || flow.selection_marker != 1 ||
        video.pixels[0] != 5) {
        fputs("menu key-down selection contract failed\n", stderr);
        return 1;
    }
    controls.keyboard_down[FA18_FRONTEND_KEY_1] = 0;
    if (fa18_menu_flow_apply_controls(&flow, &controls, &video) != 0 ||
        flow.selectors[0] != 101 || flow.selectors[1] != 0 ||
        flow.display_delay != 0x00d2 || !flow.complete_clear_after_present ||
        video.pixels[36607] != 0 || video.pixels[36608] != 5) {
        fputs("menu key-up clear-prefix contract failed\n", stderr);
        return 1;
    }
    fa18_menu_flow_finish_presented_frame(&flow, &video);
    for (size_t i = 0; i < FA18_PIXELS; ++i) {
        if (video.pixels[i] != 0) {
            fputs("menu completed clear contract failed\n", stderr);
            return 1;
        }
    }
    uint16_t selector = 0;
    if (fa18_menu_flow_take_selector(&flow, &selector) != 0 || selector != 101 ||
        fa18_menu_flow_take_selector(&flow, &selector) != -1) {
        fputs("menu queued selector contract failed\n", stderr);
        return 1;
    }
    for (int tick = 0; tick < 210; ++tick) {
        if (fa18_menu_flow_post_input_tick(&flow) != 0 ||
            flow.display_delay != 209 - tick) {
            fputs("menu delayed-tick countdown contract failed\n", stderr);
            return 1;
        }
    }
    if (fa18_menu_flow_post_input_tick(&flow) != 1 ||
        !flow.transition_started || flow.display_delay != 4 ||
        flow.transition_row_limit != 179 || flow.transition_stage != 3 ||
        flow.root_table_index != 3 || flow.root_type != 0x11 ||
        flow.post_input_phase != 2 || !flow.transition_auxiliary ||
        !flow.demo_followup_pending || !flow.blank_presentation_pending ||
        fa18_menu_flow_post_input_tick(&flow) != -1) {
        fputs("menu delayed-transition contract failed\n", stderr);
        return 1;
    }
    memset(video.pixels, 5, sizeof video.pixels);
    fa18_menu_flow_finish_presented_frame(&flow, &video);
    if (!flow.blank_presentation_pending || !flow.blank_presentation_waited ||
        flow.blank_presentation_selected || video.pixels[0] != 5) {
        fputs("menu blank-handoff retained-presentation contract failed\n", stderr);
        return 1;
    }
    fa18_menu_flow_finish_presented_frame(&flow, &video);
    if (flow.blank_presentation_pending || !flow.blank_presentation_selected ||
        video.pixels[0] != 0 || video.pixels[FA18_PIXELS - 1] != 0) {
        fputs("menu blank-handoff selected-presentation contract failed\n", stderr);
        return 1;
    }
    FA18MenuFlow wrap = {0};
    wrap.selected_mode = 0x7f;
    wrap.display_delay = INT16_MIN;
    if (fa18_menu_flow_post_input_tick(&wrap) != 0 ||
        wrap.display_delay != INT16_MAX || wrap.post_input_tick_count != 1) {
        fputs("menu delayed-tick wrap contract failed\n", stderr);
        return 1;
    }
    int16_t callback_countdown = INT16_MIN;
    wrap.post_input_tick_count = 0xff;
    if (fa18_menu_flow_advance_post_input_countdown(&wrap, &callback_countdown) != 0 ||
        callback_countdown != INT16_MAX || wrap.post_input_tick_count != 0 ||
        fa18_menu_flow_advance_post_input_countdown(NULL, &callback_countdown) != -1 ||
        fa18_menu_flow_advance_post_input_countdown(&wrap, NULL) != -1) {
        fputs("menu shared callback-tick contract failed\n", stderr);
        return 1;
    }
    if (fa18_menu_flow_apply_controls(NULL, &controls, &video) != -1) {
        fputs("menu flow argument contract failed\n", stderr);
        return 1;
    }
    puts("menu flow contract passed");
    return 0;
}
