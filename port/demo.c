#include "demo.h"

int fa18_demo_start_run075(FA18DemoController *controller) {
    if (!controller || controller->phase != FA18_DEMO_IDLE ||
        fa18_select_run075_demo_mode(&controller->menu) != 0 ||
        fa18_schedule_demo_selection(&controller->menu) != 0) {
        return -1;
    }
    controller->phase = FA18_DEMO_WAITING_FOR_TRANSITION;
    return 0;
}

int fa18_demo_tick(FA18DemoController *controller) {
    if (!controller) return -1;
    if (controller->phase == FA18_DEMO_ENTRY_READY) {
        if (controller->menu.callback != FA18_MENU_CALLBACK_DEMO_ENTRY) return -1;
        fa18_menu_post_input_tick(&controller->menu);
        if (!fa18_menu_delay_expired(&controller->menu)) {
            /* run075 frame 291: $C0FA04 -> $C2FD22 while waiting. */
            fa18_clear_renderer_work_buffer(&controller->work_buffer);
            return 0;
        }
        if (fa18_expire_run075_demo_entry(&controller->menu) != 0) return -1;
        controller->phase = FA18_DEMO_FOLLOWUP_MATCH;
        return 1;
    }
    if (controller->phase != FA18_DEMO_WAITING_FOR_TRANSITION ||
        controller->menu.callback != FA18_MENU_CALLBACK_DELAYED_TRANSITION) return -1;
    fa18_menu_post_input_tick(&controller->menu);
    if (!fa18_menu_delay_expired(&controller->menu)) return 0;
    if (fa18_prepare_run075_demo_transition(&controller->menu) != 0 ||
        fa18_enter_demo_followup(&controller->menu) != 0) {
        return -1;
    }
    controller->phase = FA18_DEMO_ENTRY_READY;
    return 1;
}
