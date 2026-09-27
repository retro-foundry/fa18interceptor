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
    uint16_t selectors[2];
    int16_t display_delay;
} FA18MenuFlow;

void fa18_menu_flow_init(FA18MenuFlow *flow);

/* Apply the observed key-1 edge route before the current video frame is
 * presented. `$C2FD22`'s first-loop prefix is visible in that same frame. */
int fa18_menu_flow_apply_controls(FA18MenuFlow *flow,
                                  const FA18ReplayControlState *controls,
                                  FA18Video *video);

/* Complete `$C2FD22` after its partially visible first presentation. */
void fa18_menu_flow_finish_presented_frame(FA18MenuFlow *flow, FA18Video *video);

#endif
