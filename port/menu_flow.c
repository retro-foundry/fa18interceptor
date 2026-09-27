#include "menu_flow.h"

#include <string.h>

enum { MENU_DEMONSTRATION_MODE = 0x7f, MENU_DEMONSTRATION_SELECTOR = 101 };

void fa18_menu_flow_init(FA18MenuFlow *flow) {
    if (flow) memset(flow, 0, sizeof *flow);
}

static void clear_renderer_first_loop_prefix(FA18Video *video, uint32_t iterations) {
    /* `$C2FD44-$C2FD56` clears four longwords, one in each displayed plane.
     * Four bytes per 40-byte plane row make one 32-pixel chunky span. */
    uint32_t pixels = iterations * 32u;
    if (pixels > FA18_PIXELS) pixels = FA18_PIXELS;
    memset(video->pixels, 0, pixels);
}

int fa18_menu_flow_apply_controls(FA18MenuFlow *flow,
                                  const FA18ReplayControlState *controls,
                                  FA18Video *video) {
    if (!flow || !controls || !video) return -1;
    uint8_t key_down = controls->keyboard_down[FA18_FRONTEND_KEY_1] != 0;
    if (key_down && !flow->key_1_down) {
        /* `$C1BD78-$C1BDEC`: the observed zero-context, enabled-record route. */
        flow->selection_marker = 1;
        flow->selected_mode = MENU_DEMONSTRATION_MODE;
    } else if (!key_down && flow->key_1_down &&
               flow->selected_mode == MENU_DEMONSTRATION_MODE) {
        /* `$C0FD10-$C0FDCE`: the observed mode-$7F branch. The source clear
         * starts during this presentation; the completed clear is deferred. */
        flow->selectors[0] = MENU_DEMONSTRATION_SELECTOR;
        flow->selectors[1] = 0;
        flow->display_delay = 0x00d2;
        clear_renderer_first_loop_prefix(video, FA18_MENU_CLEAR_PRESENT_ITERATIONS);
        flow->complete_clear_after_present = 1;
    }
    flow->key_1_down = key_down;
    return 0;
}

void fa18_menu_flow_finish_presented_frame(FA18MenuFlow *flow, FA18Video *video) {
    if (!flow || !video || !flow->complete_clear_after_present) return;
    memset(video->pixels, 0, sizeof video->pixels);
    flow->complete_clear_after_present = 0;
    flow->selector_render_pending = 1;
}

int fa18_menu_flow_take_selector(FA18MenuFlow *flow, uint16_t *selector) {
    if (!flow || !selector || !flow->selector_render_pending) return -1;
    *selector = flow->selectors[0];
    flow->selector_render_pending = 0;
    return 0;
}
