#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_context_commands.h"
#include "globals.h"

static const struct { uint32_t entry,ret; } children[]={
    {0xc091e0,0xc1b720},{0xc0915a,0xc1b762},
    {0xc0f4a6,0xc1bfbc},{0xc0f4a6,0xc1c09a}
};
static ContextCommandResult consume(void *context,enum ContextCommandChild child,
                                    const ContextCommandInput *input) {
    ContextCommandResult result;
    (void)context;
    if(child==CONTEXT_COMMAND_SET_OBSERVER) {
        D(0)=(uint32_t)input->position[0]; D(1)=(uint32_t)input->position[1];
        D(2)=(uint32_t)input->position[2];
    }
    glue_complete_child(children[child].entry,children[child].ret);
    result.event=D(0); result.position[0]=(int32_t)D(0);
    result.position[1]=(int32_t)D(1); result.position[2]=(int32_t)D(2); return result;
}
static void outputs(void *context,enum ContextCommandPhase phase,uint32_t value,
                    uint32_t limit,gaddr address) {
    (void)context;
    switch(phase) {
    case CONTEXT_BYTE_TEST: case CONTEXT_BYTE_STORE: case CONTEXT_MODIFIER_TEST:
    case CONTEXT_ORIGIN_TEST: flags_logic_b(value); break;
    case CONTEXT_WORD_STORE: flags_logic_w(value); break;
    case CONTEXT_LONG_STORE: flags_logic_l(value); break;
    case CONTEXT_REQUEST_BIT: FLAG_Z=value&(1u<<limit); break;
    case CONTEXT_RECORD_COPY_BEGIN: case CONTEXT_POSE_BEGIN: A(0)=address; break;
    case CONTEXT_RECORD_COPY_OFFSET: SET_W(D(4),value); flags_logic_w(value); break;
    case CONTEXT_POSE_INDEX:
        SET_B(D(0),value); SET_W(D(0),(int16_t)(int8_t)D(0));
        renderer_asl_word(&D(0),4); SET_W(D(1),D(0)); flags_logic_w(D(1));
        A(0)+=(int32_t)(int16_t)D(0); break;
    case CONTEXT_POSE_VALUE: SET_W(D(0),value); flags_logic_w(value); break;
    case CONTEXT_RECORD_SELECT:
        SET_W(D(0),D(0)&0x7fff); flags_logic_w(D(0)); renderer_asl_word(&D(0),8);
        step_add_word(&D(0),D(0)); A(1)=address; break;
    case CONTEXT_RECORD_COMPARE: step_compare_byte(limit,value); break;
    case CONTEXT_LOCAL_POINT:
        if(value==0x20) { SET_W(D(3),-36); SET_W(D(4),47); SET_W(D(5),-48); }
        else { D(3)=0; SET_W(D(4),20); SET_W(D(5),-106); }
        flags_logic_w(D(5)); break;
    case CONTEXT_PRESET_POSITION:
        A(0)=address; renderer_load(A(0),0x3f,2,-1);
        step_swap(&D(0)); step_asl_long(&D(0),6);
        step_swap(&D(1)); step_asl_long(&D(1),6);
        step_add_word(&D(2),D(2)); step_add_word(&D(2),D(2));
        A(4)=GRID_ADJUST_WORDS;
        renderer_load(A(4)+(gaddr)(int32_t)(int16_t)D(2),0xc0,2,-1);
        step_asl_long(&D(6),8); step_asl_long(&D(7),8);
        step_add_long(&D(0),D(6)); step_add_long(&D(1),D(7));
        step_asl_long(&D(4),8); step_asl_long(&D(5),8);
        step_add_long(&D(0),D(4)); step_add_long(&D(1),D(5));
        step_asl_long(&D(3),8); D(2)=D(1); flags_logic_l(D(2));
        D(1)=D(3); flags_logic_l(D(1)); break;
    case CONTEXT_EVENT_SAVE:
        A(7)-=2; wr_u16(A(7),(uint16_t)D(0)); flags_logic_w(D(0)); break;
    case CONTEXT_EVENT_RESTORE: SET_W(D(0),rd_u16(A(7))); A(7)+=2; flags_logic_w(D(0)); break;
    case CONTEXT_MAP_POSITION: D(4)=0x10c00000; D(1)=value; D(2)=0x11400000; flags_logic_l(D(2)); break;
    case CONTEXT_MAP_NEGATE:
        D(4)&=0x3fffff; flags_logic_l(D(4)); D(2)&=0x3fffff; flags_logic_l(D(2));
        renderer_negate(&D(4),4); renderer_negate(&D(1),4); renderer_negate(&D(2),4); break;
    case CONTEXT_MAP_CACHED_POSITION: flags_logic_l(value); break;
    case CONTEXT_GATE_ADDRESS: A(0)=address; break;
    case CONTEXT_RECORDER_CURSOR: D(3)=value; flags_logic_l(value); break;
    case CONTEXT_RECORDER_CLEAR_BEGIN: A(0)=address; D(3)=0xffffffffu; flags_logic_l(D(3)); break;
    case CONTEXT_RECORDER_CLEAR_BYTE: ++A(0); flags_logic_b(0xff); break;
    }
}
static const ContextCommandHooks hooks={consume,outputs,NULL};
const ContextCommandHooks *glue_context_command_hooks(void) { return &hooks; }
uint32_t glue_execute_context_command(const CommandRequest *request) {
    return execute_context_command(request,&hooks);
}
