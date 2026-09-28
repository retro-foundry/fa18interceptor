#ifndef FA18_OUTER_UPDATE_LOOP_H
#define FA18_OUTER_UPDATE_LOOP_H

#include <stdint.h>

typedef int (*FA18OuterUpdateLoopStage)(void *context);
typedef int (*FA18OuterUpdateLoopDelay)(void *context, uint32_t argument);

typedef struct {
    FA18OuterUpdateLoopStage acquire_blitter;
    FA18OuterUpdateLoopDelay initial_delay;
    FA18OuterUpdateLoopStage own_blitter;
    FA18OuterUpdateLoopStage disown_blitter;
    FA18OuterUpdateLoopStage parent_update;
    FA18OuterUpdateLoopStage wait_display;
    FA18OuterUpdateLoopStage outer_child;
    void *context;
} FA18OuterUpdateLoopOps;

typedef struct {
    uint8_t initialized;
    uint32_t completed_iterations;
} FA18OuterUpdateLoopState;

/* `$C15D80-$C15DB3`: run the one-time acquire/delay prefix, then exactly one
 * display-synchronized loop iteration.  The caller controls repetition at the
 * source back-edge; no presentation-frame cadence is inferred here. */
int fa18_initialize_outer_update_loop(FA18OuterUpdateLoopState *state,
                                      const FA18OuterUpdateLoopOps *ops);
int fa18_run_outer_update_loop_iteration(FA18OuterUpdateLoopState *state,
                                         const FA18OuterUpdateLoopOps *ops);

#endif
