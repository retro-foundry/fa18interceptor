#include "outer_loop_child.h"

#include <string.h>

int fa18_advance_outer_loop_idle_child(FA18OuterLoopChildState *outer,
                                       FA18ViewportModeState *viewport_mode,
                                       const FA18OuterLoopChildOps *ops,
                                       FA18OuterLoopChildStep *step) {
    if (!outer || !viewport_mode || !step || outer->activity_counter) return -1;
    if (viewport_mode->state && !(outer->status_word & UINT16_C(0x0100)) &&
        (!ops || !ops->wait_viewport || !ops->load_rgb4 ||
         !outer->dynamic_palette))
        return -1;
    memset(step, 0, sizeof *step);

    if (viewport_mode->state) {
        --viewport_mode->state;
        step->mode_state_decremented = 1;
        if (!(outer->status_word & UINT16_C(0x0100))) {
            if (ops->wait_viewport(ops->context) != 0 ||
                ops->load_rgb4(ops->context, outer->dynamic_palette->words,
                               FA18_OUTER_LOOP_CHILD_RGB4_WORDS) != 0)
                return -1;
            step->dynamic_palette_loaded = 1;
        }
    }
    outer->selected_index = (uint16_t)(UINT16_C(1) - outer->selected_index);
    step->selected_index_after = outer->selected_index;
    return 0;
}
