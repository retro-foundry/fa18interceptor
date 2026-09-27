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
    if (fa18_menu_flow_apply_controls(NULL, &controls, &video) != -1) {
        fputs("menu flow argument contract failed\n", stderr);
        return 1;
    }
    puts("menu flow contract passed");
    return 0;
}
