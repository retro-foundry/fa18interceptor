#include "menu_text.h"

#include <stdio.h>
#include <string.h>

int main(void) {
    const uint16_t expected[FA18_MENU_TEXT_SELECTORS] = {
        6, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 0
    };
    FA18MenuTextState state = {.video_flags = 0x80u};
    if (fa18_menu_queue_top_level_text(&state) != 0 ||
        state.display_mode != 0x001f0000u || state.video_latch != 0x001f0000u ||
        state.auxiliary_latch != 1 || state.mode_latch != 0 ||
        state.text_layout_increment != 0x000001e0u ||
        state.selector_count != FA18_MENU_TEXT_SELECTORS ||
        memcmp(state.selectors, expected, sizeof expected) != 0) {
        fputs("top-level menu queue contract failed\n", stderr);
        return 1;
    }
    state.video_latch = 0;
    state.auxiliary_latch = 1;
    if (fa18_menu_queue_top_level_text(&state) != 0 || state.video_latch != 0) {
        fputs("top-level menu video latch guard failed\n", stderr);
        return 1;
    }
    if (fa18_menu_queue_top_level_text(NULL) != -1) {
        fputs("top-level menu argument contract failed\n", stderr);
        return 1;
    }
    puts("top-level menu queue contract passed");
    return 0;
}
