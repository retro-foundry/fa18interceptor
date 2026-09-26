#include "demo.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    /* run075 starts with video bit 7 set, which selects the $00D2 delay. */
    FA18DemoController controller;
    memset(&controller, 0, sizeof controller);
    controller.menu.video_flags = 0xf7;
    if (fa18_demo_start_run075(&controller) != 0 ||
        controller.phase != FA18_DEMO_WAITING_FOR_TRANSITION ||
        controller.menu.selected_mode != 0x7f ||
        controller.menu.delay_ticks != 210 ||
        controller.menu.callback != FA18_MENU_CALLBACK_DELAYED_TRANSITION ||
        controller.menu.selector_queue[0] != 101 ||
        controller.menu.selector_queue[1] != 0) {
        fputs("run075 native demo start contract failed\n", stderr);
        return 1;
    }
    for (int tick = 0; tick < 210; ++tick) {
        if (fa18_demo_tick(&controller) != 0 || controller.menu.delay_ticks != 209 - tick) {
            fputs("run075 native demo countdown contract failed\n", stderr);
            return 1;
        }
    }
    if (fa18_demo_tick(&controller) != 1 ||
        controller.phase != FA18_DEMO_ENTRY_READY ||
        controller.menu.post_input_tick_count != 211 ||
        controller.menu.delay_ticks != 4 ||
        controller.menu.transition_row_limit != 179 ||
        controller.menu.transition_stage != 3 ||
        controller.menu.post_input_phase != 2 ||
        controller.menu.demo_followup_pending != 1 ||
        controller.menu.callback != FA18_MENU_CALLBACK_DEMO_ENTRY) {
        fputs("run075 native demo entry contract failed\n", stderr);
        return 1;
    }
    for (int tick = 0; tick < 4; ++tick) {
        memset(controller.work_buffer.pixels, 6, sizeof controller.work_buffer.pixels);
        if (fa18_demo_tick(&controller) != 0 || controller.menu.delay_ticks != 3 - tick ||
            controller.work_buffer.pixels[0] != 0 ||
            controller.work_buffer.pixels[FA18_WIDTH * FA18_HEIGHT - 1] != 0) {
            fputs("run075 native demo-entry wait contract failed\n", stderr);
            return 1;
        }
    }
    if (fa18_demo_tick(&controller) != 1 ||
        controller.phase != FA18_DEMO_FOLLOWUP_MATCH ||
        controller.menu.delay_ticks != 2 ||
        controller.menu.selected_mode != 3 ||
        controller.menu.scene_stage != 3 || controller.menu.scene_guard != 1 ||
        controller.menu.scene_counter_a != 0 || controller.menu.scene_counter_b != 0 ||
        controller.menu.scene_marker != -1 ||
        controller.menu.followup_command_mode != 3 ||
        controller.menu.followup_auxiliary != 0 ||
        controller.menu.followup_mode != 0x0f ||
        controller.menu.followup_input != 0 ||
        controller.menu.callback != FA18_MENU_CALLBACK_DEMO_FOLLOWUP_MATCH) {
        fputs("run075 native demo followup contract failed\n", stderr);
        return 1;
    }
    puts("run075 native demo controller contract passed");
    return 0;
}
