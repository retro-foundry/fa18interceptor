#include "glue_step.h"
#include "glue_child_call.h"
#include "post_input_tick.h"

static void tick_outputs(void *context,const PostInputTickEvent *event) {
    (void)context;
    switch(event->phase) {
    case POST_TICK_OFFSET_BEGIN: case POST_TICK_OFFSET_FIXED:
        D(0)=event->offset; m68k_write_memory_32(A(6)-4,event->offset); break;
    case POST_TICK_OFFSET_SECONDARY: case POST_TICK_OFFSET_EXTRA:
        D(1)=event->term; m68k_write_memory_32(A(6)-4,event->offset); break;
    case POST_TICK_OFFSET_ACCEPTED: D(0)=event->offset; break;
    case POST_TICK_PHASE_RESET: D(0)=0; break;
    case POST_TICK_PHASE_START: D(0)=1; break;
    case POST_TICK_DISPATCH:
        /* All intermediate byte loads preserve the high word. The final
         * word decrement supplies the exact stage argument and all CCR bits. */
        SET_W(D(0),event->countdown);
        step_subtract_word(&D(0),1); flags_logic_w(D(0));
        A(0)=event->routine; break;
    }
}
static void consume(void *context,gaddr routine) {
    (void)context;
    glue_complete_child(routine,0xc0f808u);
}
int glue_C0F5F8(void) {
    PostInputTickHooks hooks={consume,tick_outputs,NULL};
    /* Preserve the original LINK -4 frame for the indirect stage call. */
    m68ki_push_32(A(6)); A(6)=A(7); A(7)-=4;
    run_post_input_tick(&hooks);
    flags_logic_b(0);
    A(7)=A(6); A(6)=m68ki_pull_32();
    return glue_return();
}
