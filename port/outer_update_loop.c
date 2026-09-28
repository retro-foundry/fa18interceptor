#include "outer_update_loop.h"

enum { FA18_OUTER_DELAY_ARGUMENT = 0x186a0 };

static int run_stage(FA18OuterUpdateLoopStage stage, void *context) {
    return stage && stage(context) == 0 ? 0 : -1;
}

int fa18_initialize_outer_update_loop(FA18OuterUpdateLoopState *state,
                                      const FA18OuterUpdateLoopOps *ops) {
    if (!state || !ops || !ops->acquire_blitter || !ops->initial_delay)
        return -1;
    if (run_stage(ops->acquire_blitter, ops->context) != 0 ||
        ops->initial_delay(ops->context, FA18_OUTER_DELAY_ARGUMENT) != 0)
        return -1;
    state->initialized = 1;
    state->completed_iterations = 0;
    return 0;
}

int fa18_run_outer_update_loop_iteration(FA18OuterUpdateLoopState *state,
                                         const FA18OuterUpdateLoopOps *ops) {
    if (!state || !ops || !state->initialized || !ops->select_renderer_page ||
        !ops->own_blitter ||
        !ops->disown_blitter || !ops->parent_update || !ops->wait_display ||
        !ops->outer_child)
        return -1;
    if (run_stage(ops->select_renderer_page, ops->context) != 0 ||
        run_stage(ops->own_blitter, ops->context) != 0 ||
        run_stage(ops->parent_update, ops->context) != 0 ||
        run_stage(ops->disown_blitter, ops->context) != 0 ||
        run_stage(ops->wait_display, ops->context) != 0 ||
        run_stage(ops->outer_child, ops->context) != 0)
        return -1;
    ++state->completed_iterations;
    return 0;
}
