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
    if (!flow || !video) return;
    if (flow->complete_clear_after_present) {
        memset(video->pixels, 0, sizeof video->pixels);
        flow->complete_clear_after_present = 0;
        flow->selector_render_pending = 1;
    }
    if (!flow->blank_presentation_pending) return;
    if (!flow->blank_presentation_waited) {
        flow->blank_presentation_waited = 1;
        return;
    }
    /* run075: `$C0FEEA`'s indexed-record path changes the Copper selection
     * during frame 271; the old menu page presents in 272 and the selected
     * blank page presents in 273. The record/page ownership is not modeled. */
    memset(video->pixels, 0, sizeof video->pixels);
    flow->blank_presentation_pending = 0;
    flow->blank_presentation_selected = 1;
}

int fa18_menu_flow_take_selector(FA18MenuFlow *flow, uint16_t *selector) {
    if (!flow || !selector || !flow->selector_render_pending) return -1;
    *selector = flow->selectors[0];
    flow->selector_render_pending = 0;
    return 0;
}

int fa18_menu_flow_post_input_tick(FA18MenuFlow *flow) {
    if (!flow || flow->selected_mode != MENU_DEMONSTRATION_MODE ||
        flow->transition_started) {
        return -1;
    }
    /* `$C0F7D8-$C0F7FE`: byte tick counter plus wrapping signed word
     * decrement. `$C0F804` invokes the active callback after this update. */
    ++flow->post_input_tick_count;
    if (flow->display_delay == INT16_MIN) flow->display_delay = INT16_MAX;
    else --flow->display_delay;
    if (flow->display_delay >= 0) return 0;

    /* Bounded run075 delayed callback: `$C0FEEA-$C10020`. These are the
     * direct state writes established before/at the mode-$7F table arm.
     * Helper effects and the indexed Copper record ownership are separate
     * contracts, so this function deliberately performs no video update. */
    flow->transition_row_limit = 179;
    flow->display_delay = 4;
    flow->transition_stage = 3;
    flow->post_input_phase = 2;
    flow->transition_auxiliary = 1;
    flow->demo_followup_pending = 1;
    flow->blank_presentation_pending = 1;
    flow->transition_started = 1;
    return 1;
}
