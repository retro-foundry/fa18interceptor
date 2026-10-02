#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "context_refresh.h"
#include "globals.h"
typedef struct { uint32_t template_ret; } ContextRefreshCPU;

static void context_outputs(void *context,const ContextRefreshEvent *event) {
    unsigned i;
    ContextRefreshCPU *cpu=context;
    switch(event->phase) {
    case CONTEXT_REFRESH_GUARD: step_compare_long(0xf8000000u,event->value); break;
    case CONTEXT_REFRESH_SAVE:
        m68ki_push_32(A(0)); for(i=6;i>0;--i) m68ki_push_32(D(i-1)); break;
    case CONTEXT_REFRESH_FRAME_GATE:
        m68k_write_memory_8(A(6)-0x2c,event->value); flags_logic_b(event->value); break;
    case CONTEXT_REFRESH_REQUESTS:
        SET_B(D(0),event->value); flags_logic_b(event->value); break;
    case CONTEXT_REFRESH_FLAGGED:
        A(0)=WORKSPACE_RECORDS; SET_B(D(7),0x10); break;
    case CONTEXT_REFRESH_SELECTOR:
        SET_B(D(1),event->value&15u);
        if(event->record) {
            A(1)=event->record; SET_W(D(1),event->x); SET_W(D(2),event->z);
        } else { D(1)=event->x; D(2)=event->z; }
        renderer_asr_word(&D(1),event->shift); renderer_asr_word(&D(2),event->shift);
        flags_logic_b(0); break;
    case CONTEXT_REFRESH_TEMPLATE_CALL:
        cpu->template_ret=event->bit==0?0xc1c924u:event->bit==1?0xc1c94au:0xc1c982u;
        flags_logic_b(event->bit==1?1:0);
        FLAG_Z=event->bit==1?1:event->value&(1u<<event->bit); break;
    case CONTEXT_REFRESH_SORT_CALL:
        flags_logic_b(0); break;
    case CONTEXT_REFRESH_CACHE_CALL: flags_logic_b(0); break;
    case CONTEXT_REFRESH_CONDITION_CALL:
        SET_W(D(1),event->value&1u); flags_logic_w(D(1)); break;
    case CONTEXT_REFRESH_RESTORE:
        for(i=0;i<6;++i) D(i)=m68ki_pull_32(); A(0)=m68ki_pull_32(); break;
    case CONTEXT_REFRESH_RENDER_CALL:
        SET_W(D(0),0x72); SET_W(D(1),0xa0); flags_logic_w(event->value); break;
    case CONTEXT_REFRESH_DONE: flags_logic_b(0); break;
    }
}
static void consume(void *context,enum ContextRefreshChild child) {
    static const struct { uint32_t routine,ret; } calls[]={
        {0xc1d10cu,0}, {0xc1e328u,0xc1c99cu}, {0xc1e540u,0xc1c9aeu},
        {0xc09a78u,0xc1c9c8u}, {0xc09a98u,0xc1c9d8u}, {0xc2f66eu,0xc1ca26u}
    };
    uint32_t ret=calls[child].ret;
    ContextRefreshCPU *cpu=context;
    if(child==CONTEXT_REFRESH_TEMPLATES) {
        ret=cpu->template_ret;
    }
    glue_complete_child(calls[child].routine,ret);
}
int glue_C1C860(void) {
    ContextRefreshCPU cpu={0};
    ContextRefreshHooks hooks={consume,context_outputs,&cpu};
    refresh_context_packet(&hooks);
    return glue_return();
}
