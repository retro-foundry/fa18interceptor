#include "glue_projection_readouts_math.h"
#include "glue_projection_readouts.h"
#include "glue_child_call.h"
#include "projection_readouts.h"
static ReadoutState working(void) {
    ReadoutState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5)}; return w;
}
static ReadoutState consume(void *context,enum ReadoutChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc06c02,0xc2ec7e},{0xc2f5f4,0xc2ed2a},{0xc2f60a,0xc2ed32},{0xc2f66e,0xc2ed3a},
        {0xc2f1c0,0xc2ed40},{0xc25a08,0xc32a52},{0xc330fe,0xc32b72},{0xc330fe,0xc32b90},
        {0xc330fe,0xc32bae},{0xc330fe,0xc32bcc},{0xc32aa6,0xc33fb0},{0xc2f60a,0xc33fde},
        {0xc2f5f4,0xc3400a},{0xc2f60a,0xc34016},{0xc2f5f4,0xc3404e},{0xc2f60a,0xc3405a}
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum ReadoutPhase phase,enum ReadoutValue field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case PR_WORD: SET_W(D(r),v); flags_logic_w(v); break; case PR_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case PR_LONG: D(r)=v; flags_logic_l(v); break; case PR_POINTER: A(r-PR_OUTPUT)=v; break;
    case PR_ADD_WORD: step_add_word(&D(r),v); break; case PR_ADD_LONG: step_add_long(&D(r),v); break;
    case PR_SUB_WORD: step_subtract_word(&D(r),v); break;
    case PR_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case PR_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case PR_SWAP: step_swap(&D(r)); break; case PR_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case PR_ASR_WORD: renderer_asr_word(&D(r),v); break; case PR_ASL_WORD: renderer_asl_word(&D(r),v); break; case PR_LSR_LONG: step_lsr_long(&D(r),v); break;
    case PR_NEG_WORD: renderer_negate(&D(r),2); break; case PR_MULTIPLY: renderer_multiply(&D(r),(uint16_t)v); break;
    case PR_DIVIDE: renderer_divide(&D(r),(int16_t)v); break;
    case PR_COMPARE_WORD: step_compare_word(other,v); break; case PR_COMPARE_BYTE: step_compare_byte(other,v); break;
    case PR_TEST_BYTE: case PR_STORE_BYTE: flags_logic_b(v); break; case PR_STORE_WORD: flags_logic_w(v); break; case PR_STORE_LONG: flags_logic_l(v); break;
    case PR_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case PR_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case PR_DECREMENT: SET_W(D(r),v); break;
    case PR_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case PR_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case PR_BEGIN_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break;
    case PR_END_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case PR_MEMORY_ADD_WORD: temporary=v; step_add_word(&temporary,other); break;
    }
}
static ReadoutState restored(void *context) { (void)context; return working(); }
static gaddr frame(void *context) { (void)context; return A(6); }
static const ReadoutHooks hooks={consume,outputs,restored,frame,NULL};
int glue_C2EC90(void) { project_and_plot_point(working(),-5,&hooks); return glue_return(); }
int glue_C2EC94(void) { project_and_plot_point(working(),1,&hooks); return glue_return(); }
int glue_C2EC9C(void) { project_and_plot_point(working(),-4,&hooks); return glue_return(); }
int glue_C2ECA4(void) { project_and_plot_point(working(),-2,&hooks); return glue_return(); }
int glue_C2ECA8(void) { project_and_plot_point(working(),-1,&hooks); return glue_return(); }
int glue_readout_x_limit(void) { finish_projected_x_limit(working(),&hooks); return glue_return(); }
int glue_readout_y_limit(void) { finish_projected_y_limit(working(),&hooks); return glue_return(); }
int glue_C32A44(void) { draw_scene_numeric_label(working(),&hooks); return glue_return(); }
int glue_C32AC8(void) { draw_packed_numeric_readout(working(),&hooks); return glue_return(); }
int glue_C33F70(void) { draw_speed_readout_tick(working(),&hooks); return glue_return(); }
int glue_C33F8A(void) { draw_altitude_readout_tick(working(),&hooks); return glue_return(); }
int glue_C33FB4(void) { draw_bounded_readout_sweep(working(),&hooks); return glue_return(); }
