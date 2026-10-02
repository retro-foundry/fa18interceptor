#include "glue_step.h"
#include "glue_child_call.h"
#include "scene_bootstrap.h"
#include "globals.h"

static void bootstrap_outputs(void *context,const SceneBootstrapEvent *event) {
    (void)context;
    switch(event->phase) {
    case BOOTSTRAP_BUFFERS: D(0)=0xffffffffu; flags_logic_l(0x1b8); break;
    case BOOTSTRAP_RECORDS_CLEARED:
        D(0)=D(4)=0xffffu; A(0)=A(1)=WORKSPACE_RECORDS+512u; flags_logic_l(0); break;
    case BOOTSTRAP_POSITION: flags_logic_w(0x800); break;
    case BOOTSTRAP_INITIALIZED:
        D(0)=0xffffu; D(1)=0x20; A(0)=0xc45826u;
        flags_logic_b(0x20); FLAG_X=0; break;
    case BOOTSTRAP_RESET_CALLBACK:
        D(0)=0; A(0)=event->value; flags_logic_l(event->value); break;
    case BOOTSTRAP_FOLLOWUP_MODE:
        SET_B(D(0),event->value); step_subtract_byte(&D(0),3); break;
    case BOOTSTRAP_FOLLOWUP_CALLBACK:
        A(0)=event->value; flags_logic_l(event->value); break;
    }
}
static void consume(void *context,enum SceneBootstrapChild child) {
    static const struct { uint32_t routine,ret; } calls[]={
        {0xc090c2u,0xc08f2au}, {0xc090f2u,0xc08f2eu}, {0xc2fd22u,0xc08f76u},
        {0xc09620u,0xc08faeu}, {0xc0910cu,0xc08fdau}, {0xc0915au,0xc08fdeu},
        {0xc09266u,0xc090aeu}, {0xc1c40cu,0xc090b4u}, {0xc1c63eu,0xc090bau},
        {0xc1c860u,0xc090c0u}, {0xc08f26u,0}, {0xc0f4a6u,0xc0f998u},
        {0xc11accu,0xc0f9f6u}
    };
    uint32_t owner=*(uint32_t *)context,ret=calls[child].ret;
    if(child==BOOTSTRAP_RUN) ret=owner==0xc0f920u?0xc0f926u:0xc0f99eu;
    if(child==BOOTSTRAP_LOAD_MENU_TABLE) m68ki_push_32(0xc08490u);
    glue_complete_child(calls[child].routine,ret);
    if(child==BOOTSTRAP_LOAD_MENU_TABLE) A(7)+=4;
}
static int bootstrap_call(uint32_t owner) {
    SceneBootstrapHooks hooks={consume,bootstrap_outputs,&owner};
    if(owner==0xc08f26u) bootstrap_scene(&hooks);
    else if(owner==0xc0f920u) reset_sequence_after_bootstrap(&hooks);
    else begin_sequence_after_bootstrap(&hooks);
    return glue_return();
}
int glue_C08F26(void) { return bootstrap_call(0xc08f26u); }
int glue_C0F920(void) { return bootstrap_call(0xc0f920u); }
int glue_C0F992(void) { return bootstrap_call(0xc0f992u); }
