#include "parent_update_middle.h"

static int run(FA18ParentUpdateMiddleStage stage, void *context) {
    return stage && stage(context) == 0 ? 0 : -1;
}
int fa18_run_parent_update_middle(FA18ParentUpdateMiddleState *s,
                                  const FA18ParentUpdateMiddleOps *o) {
    if (!s || !o || !o->post_c1c63e || !o->matrix_pipeline || !o->post_matrix ||
        !o->angle_octant || !o->post_octant || !o->context_refresh || !o->marker60_a ||
        !o->marker60_b || !o->marker60_c || !o->conditional_stage || !o->later_stage) return -1;
    s->stage_marker=0x20;
    if(run(o->post_c1c63e,o->context)||run(o->matrix_pipeline,o->context)||run(o->post_matrix,o->context)||run(o->angle_octant,o->context)||run(o->post_octant,o->context)||run(o->context_refresh,o->context)) return -1;
    s->stage_marker=0x60;
    if(run(o->marker60_a,o->context)||run(o->marker60_b,o->context)||run(o->marker60_c,o->context)) return -1;
    s->stage_marker=0x68;
    if (!s->conditional_flag || s->conditional_inhibit) if(run(o->conditional_stage,o->context)) return -1;
    s->stage_marker=0x70;
    return run(o->later_stage,o->context);
}
