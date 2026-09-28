#ifndef FA18_MENU_FLOW_H
#define FA18_MENU_FLOW_H

#include <stdint.h>

#include "replay.h"
#include "video.h"

enum {
    FA18_FRONTEND_KEY_1 = 49,
    FA18_MENU_CLEAR_PRESENT_ITERATIONS = 1144
};

/* The bounded first numeric-menu command path: `$C1BD78` selects mode `$7F`,
 * then `$C0FD10-$C0FDCE` resets the message sequence, clears the renderer
 * work buffers, and queues selector 101. */
typedef struct {
    uint8_t key_1_down;
    uint8_t selected_mode;
    uint8_t selection_marker;
    uint8_t complete_clear_after_present;
    uint8_t selector_render_pending;
    uint8_t post_input_tick_count;
    uint8_t transition_started;
    uint8_t transition_stage;
    uint8_t root_table_index;
    uint8_t root_type;
    uint8_t post_input_phase;
    uint8_t transition_auxiliary;
    uint8_t demo_followup_pending;
    uint8_t blank_presentation_pending;
    uint8_t blank_presentation_waited;
    uint8_t blank_presentation_selected;
    uint16_t selectors[2];
    int16_t display_delay;
    int16_t transition_row_limit;
} FA18MenuFlow;

void fa18_menu_flow_init(FA18MenuFlow *flow);

/* Apply the observed key-1 edge route before the current video frame is
 * presented. `$C2FD22`'s first-loop prefix is visible in that same frame. */
int fa18_menu_flow_apply_controls(FA18MenuFlow *flow,
                                  const FA18ReplayControlState *controls,
                                  FA18Video *video);

/* Complete `$C2FD22` after its partially visible first presentation. It also
 * advances the separately proved run075 blank-page handoff after one retained
 * presentation, when a scheduler has delivered the delayed transition tick. */
void fa18_menu_flow_finish_presented_frame(FA18MenuFlow *flow, FA18Video *video);

/* Consume the selector that follows the completed display clear. */
int fa18_menu_flow_take_selector(FA18MenuFlow *flow, uint16_t *selector);

/* Execute exactly one scheduler-supplied `$C0F5F8` post-input tick for the
 * bounded run075 mode-$7F continuation. It returns zero while the delayed
 * transition is waiting, one after the observed `$C0FEEA`/`$C1000A` direct
 * state subset is reached, or -1 outside this proved callback boundary.
 * A presentation frame is not a tick: the caller must supply an independently
 * authorized timing source. */
int fa18_menu_flow_post_input_tick(FA18MenuFlow *flow);

#endif
