#ifndef FA18_PARENT_UPDATE_MIDDLE_H
#define FA18_PARENT_UPDATE_MIDDLE_H

#include <stdint.h>

typedef int (*FA18ParentUpdateMiddleStage)(void *context);
typedef struct {
    FA18ParentUpdateMiddleStage post_c1c63e, matrix_pipeline, post_matrix;
    FA18ParentUpdateMiddleStage angle_octant, post_octant, context_refresh;
    FA18ParentUpdateMiddleStage marker60_a, marker60_b, marker60_c;
    FA18ParentUpdateMiddleStage conditional_stage, later_stage;
    void *context;
} FA18ParentUpdateMiddleOps;

typedef struct { uint16_t stage_marker; uint8_t conditional_flag, conditional_inhibit; } FA18ParentUpdateMiddleState;

/* `$C0F01C-$C0F08F`: ordered parent-update middle; all children stay caller-owned. */
int fa18_run_parent_update_middle(FA18ParentUpdateMiddleState *state,
                                  const FA18ParentUpdateMiddleOps *ops);
#endif
