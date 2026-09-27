#include "menu_text.h"

#include <string.h>

int fa18_menu_queue_top_level_text(FA18MenuTextState *state) {
    static const uint16_t selectors[FA18_MENU_TEXT_SELECTORS] = {
        6, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 0
    };
    if (!state) return -1;
    state->display_mode = 0x001f0000u;
    if ((state->video_flags & 0x80u) != 0 && state->auxiliary_latch == 0)
        state->video_latch = 0x001f0000u;
    state->auxiliary_latch = 1;
    state->mode_latch = 0;
    state->display_delay = 0x000001e0u;
    memcpy(state->selectors, selectors, sizeof selectors);
    state->selector_count = sizeof selectors / sizeof selectors[0];
    return 0;
}
