#include "outer_loop_child.h"

#include <string.h>

static int run_idle_tail(FA18OuterLoopChildState *outer,
                         FA18ViewportModeState *viewport_mode,
                         const FA18OuterLoopChildOps *ops,
                         FA18OuterLoopChildStep *step) {
    if (!outer || !viewport_mode || !step || outer->activity_counter) return -1;
    if (viewport_mode->state && !(outer->status_word & UINT16_C(0x0100)) &&
        (!ops || !ops->wait_viewport || !ops->load_rgb4 ||
         !outer->dynamic_palette))
        return -1;
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

int fa18_advance_outer_loop_idle_child(FA18OuterLoopChildState *outer,
                                       FA18ViewportModeState *viewport_mode,
                                       const FA18OuterLoopChildOps *ops,
                                       FA18OuterLoopChildStep *step) {
    if (!step) return -1;
    memset(step, 0, sizeof *step);
    return run_idle_tail(outer, viewport_mode, ops, step);
}

int fa18_run_outer_loop_child(FA18OuterLoopChildState *outer,
                              FA18ViewportModeState *viewport_mode,
                              const uint32_t *pointer_table_1,
                              const uint32_t *pointer_table_2,
                              uint16_t pointer_table_entries,
                              const FA18OuterLoopChildOps *ops,
                              FA18OuterLoopChildStep *step) {
    if (!outer || !viewport_mode || !pointer_table_1 || !pointer_table_2 ||
        !pointer_table_entries || !ops || !ops->wait_viewport || !ops->load_view ||
        !step)
        return -1;
    memset(step, 0, sizeof *step);

    /* `$C16132-$C1617D`: WaitBOVP, table publication, then LoadView. */
    if (ops->wait_viewport(ops->context) != 0 ||
        fa18_prepare_outer_page_publication(outer->selected_index,
                                            pointer_table_1, pointer_table_2,
                                            pointer_table_entries,
                                            &step->publication) != 0 ||
        ops->load_view(ops->context, &step->publication) != 0)
        return -1;
    step->prefix_waited = 1;
    step->display_view_loaded = 1;

    if (!outer->activity_counter)
        return run_idle_tail(outer, viewport_mode, ops, step);

    /* `$C16188-$C1621E`: the signed activity loop uses the static and
     * dynamic 32-word RGB4 sources, with WaitBOVP/WaitBlit boundaries kept
     * explicit. */
    if (!ops->wait_blit || !ops->load_rgb4 || !ops->static_palette ||
        !outer->dynamic_palette || ops->wait_viewport(ops->context) != 0 ||
        ops->wait_blit(ops->context) != 0)
        return -1;
    while ((int8_t)outer->activity_counter > 0) {
        if (ops->load_rgb4(ops->context, ops->static_palette,
                           FA18_OUTER_LOOP_CHILD_RGB4_WORDS) != 0 ||
            ops->wait_viewport(ops->context) != 0 ||
            ops->wait_viewport(ops->context) != 0 ||
            ops->load_rgb4(ops->context, outer->dynamic_palette->words,
                           FA18_OUTER_LOOP_CHILD_RGB4_WORDS) != 0 ||
            ops->wait_viewport(ops->context) != 0 ||
            ops->wait_viewport(ops->context) != 0)
            return -1;
        --outer->activity_counter;
    }
    step->activity_loop_completed = 1;
    outer->selected_index = fa18_advance_outer_page_index(outer->selected_index);
    step->selected_index_after = outer->selected_index;
    return 0;
}
