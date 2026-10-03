#include "glue_flight_markers_math.h"
#include "glue_flight_markers.h"
#include "glue_child_call.h"
#include "flight_markers.h"
static MarkerState working(void) {
    MarkerState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),COND_LT(),COND_EQ()}; return w;
}
static MarkerState consume(void *context,enum MarkerChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2eca8,0xc2b52e},{0xc32a44,0xc2b554},{0xc2affa,0xc2b5d8},{0xc2affa,0xc2b5ea},
        {0xc2ee4a,0xc2b5fc},{0xc32a44,0xc2b64c},{0xc2affa,0xc2b69e},{0xc2affa,0xc2b6b0},
        {0xc2ee4a,0xc2b6c2},{0xc32a44,0xc2b710},{0xc2affa,0xc2b776},{0xc2b928,0xc2b794},
        {0xc2b952,0xc2b7d0},{0xc2ec90,0xc2b93e},{0xc2affa,0xc2b970},{0xc2ec90,0xc2b97e},{0xc2fa7e,0xc2bae6}
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum MarkerPhase phase,enum MarkerValue field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case MM_BYTE: SET_B(D(r),v); flags_logic_b(v); break; case MM_WORD: SET_W(D(r),v); flags_logic_w(v); break;
    case MM_LONG: D(r)=v; flags_logic_l(v); break; case MM_POINTER: A(r-MM_RECORD)=v; break;
    case MM_ADD_BYTE: renderer_add_byte(&D(r),v); break; case MM_ADD_WORD: step_add_word(&D(r),v); break; case MM_ADD_LONG: step_add_long(&D(r),v); break;
    case MM_SUB_BYTE: step_subtract_byte(&D(r),v); break; case MM_SUB_WORD: step_subtract_word(&D(r),v); break; case MM_SUB_LONG: step_subtract_long(&D(r),v); break;
    case MM_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break; case MM_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break;
    case MM_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break; case MM_OR_BYTE: SET_B(D(r),D(r)|v); flags_logic_b(D(r)); break;
    case MM_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break; case MM_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case MM_SWAP: step_swap(&D(r)); break; case MM_ASR_WORD: renderer_asr_word(&D(r),v); break; case MM_ASR_LONG: step_asr_long(&D(r),v); break;
    case MM_ASL_WORD: renderer_asl_word(&D(r),v); break; case MM_ASL_LONG: step_asl_long(&D(r),v); break; case MM_LSR_WORD: flight_lsr_word(&D(r),v); break;
    case MM_ROL_LONG: dynamics_rol_long(&D(r),v); break; case MM_NEG_WORD: renderer_negate(&D(r),2); break; case MM_NEG_LONG: renderer_negate(&D(r),4); break;
    case MM_NEG_BYTE: renderer_negate(&D(r),1); break; case MM_MULTIPLY: renderer_multiply(&D(r),(uint16_t)v); break;
    case MM_ASL_BYTE: marker_asl_byte(&D(r),v); break; case MM_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case MM_TEST_BYTE: case MM_STORE_BYTE: flags_logic_b(v); break; case MM_TEST_WORD: case MM_STORE_WORD: flags_logic_w(v); break;
    case MM_TEST_LONG: case MM_STORE_LONG: flags_logic_l(v); break;
    case MM_COMPARE_BYTE: step_compare_byte(other,v); break; case MM_COMPARE_WORD: step_compare_word(other,v); break; case MM_COMPARE_LONG: step_compare_long(other,v); break;
    case MM_BIT_TEST: case MM_BIT_SET: case MM_BIT_CLEAR: FLAG_Z=v&(1u<<other); break;
    case MM_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break; case MM_MEMORY_ADD_WORD: temporary=v; step_add_word(&temporary,other); break;
    case MM_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break; case MM_MEMORY_SUB_WORD: temporary=v; step_subtract_word(&temporary,other); break;
    case MM_MEMORY_SUB_LONG: temporary=v; step_subtract_long(&temporary,other); break; case MM_DECREMENT: SET_W(D(r),v); break;
    case MM_LOAD_WORDS: renderer_load(v,(uint16_t)other,2,-1); break; case MM_LOAD_LONGS: renderer_load(v,(uint16_t)other,4,-1); break;
    case MM_STORE_WORDS: case MM_STORE_LONGS: break;
    case MM_EXCHANGE: temporary=D(r); D(r)=D(v); D(v)=temporary; break;
    case MM_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case MM_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case MM_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case MM_POP_LONG: temporary=m68ki_pull_32(); if(r<8) { D(r)=temporary; flags_logic_l(temporary); } else A(r-MM_RECORD)=temporary; break;
    case MM_SAVE_SCENE: renderer_store(A(7),0x88,4,7); break; case MM_RESTORE_SCENE: renderer_load(A(7),0x1100,4,7); break;
    case MM_SAVE_DRAW: renderer_store(A(7),0x8080,4,7); break; case MM_RESTORE_DRAW: renderer_load(A(7),0x101,4,7); break;
    case MM_BEGIN_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break; case MM_END_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    }
}
static gaddr frame(void *context) { (void)context; return A(6); }
static MarkerState restored_state(void *context) { (void)context; return working(); }
static gaddr stack(void *context) { (void)context; return A(7); }
static const MarkerHooks hooks={consume,outputs,frame,restored_state,stack,NULL};
int glue_C2AFFA(void) { transform_marker_point(working(),&hooks); return glue_return(); }
int glue_C2B3C2(void) { draw_scene_position_labels(working(),&hooks); return glue_return(); }
int glue_C2B564(void) { draw_view_grid_and_markers(working(),&hooks); return glue_return(); }
int glue_C2B928(void) { draw_class_twenty_marker(working(),&hooks); return glue_return(); }
int glue_C2B952(void) { draw_record_position_marker(working(),A(6),&hooks); return glue_return(); }
