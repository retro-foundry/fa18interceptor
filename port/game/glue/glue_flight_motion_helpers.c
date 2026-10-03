#include "glue_flight_motion_helpers_math.h"
#include "glue_flight_motion_helpers.h"
#include "glue_child_call.h"
#include "flight_motion_helpers.h"
#include <stdio.h>
#include <stdlib.h>
static MotionState working(void) {
    MotionState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),COND_EQ()}; return w;
}
static MotionState consume_face(void *context,int upper) {
    (void)context; glue_complete_child(0xc27456u,upper?0xc26d7cu:0xc26d2eu); return working();
}
static MotionState divide_exception(void *context,unsigned site) {
    static const uint32_t pcs[]={0xc26338u,0xc2633au,0xc26c9au,0xc26c9cu};
    uint32_t sp=A(7),pc=pcs[site]; unsigned r=site&1?2:0; (void)context;
    REG_PPC=pc; REG_PC=pc+2; REG_IR=m68k_read_memory_16(pc); renderer_divide(&D(r),0);
    if(fa18_recomp_resume(pc+2,sp)!=FA18_RET) {
        fprintf(stderr,"motion divide exception cannot complete at %06X\n",REG_PC); abort();
    }
    return working();
}
static void outputs(void *context,enum MotionPhase phase,enum MotionValue field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case MH_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case MH_WORD: SET_W(D(r),v); flags_logic_w(v); break;
    case MH_LONG: D(r)=v; flags_logic_l(v); break;
    case MH_POINTER: A(r-MH_ROOT)=v; break;
    case MH_ADD_WORD: step_add_word(&D(r),v); break; case MH_ADD_LONG: step_add_long(&D(r),v); break;
    case MH_SUB_WORD: step_subtract_word(&D(r),v); break; case MH_SUB_LONG: step_subtract_long(&D(r),v); break;
    case MH_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break;
    case MH_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case MH_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case MH_ASR_WORD: renderer_asr_word(&D(r),v); break; case MH_ASR_LONG: step_asr_long(&D(r),v); break;
    case MH_ASL_WORD: renderer_asl_word(&D(r),v); break; case MH_ASL_LONG: step_asl_long(&D(r),v); break;
    case MH_LSR_BYTE: action_lsr_byte(&D(r),v); break; case MH_LSR_LONG: motion_lsr_long(&D(r),v); break;
    case MH_NEG_WORD: renderer_negate(&D(r),2); break; case MH_MULTIPLY: renderer_multiply(&D(r),(uint16_t)v); break;
    case MH_DIVIDE: renderer_divide(&D(r),(int16_t)v); break;
    case MH_COMPARE_BYTE: step_compare_byte(other,v); break; case MH_COMPARE_WORD: step_compare_word(other,v); break;
    case MH_COMPARE_LONG: step_compare_long(other,v); break;
    case MH_TEST_WORD: case MH_STORE_WORD: flags_logic_w(v); break;
    case MH_STORE_BYTE: flags_logic_b(v); break; case MH_STORE_LONG: flags_logic_l(v); break;
    case MH_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case MH_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)=v&~(1u<<other); break;
    case MH_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break;
    case MH_LOAD_POSITION: renderer_load(v,0x1c,4,-1); break;
    case MH_LOAD_NORMAL: renderer_load(v,0x1c,2,-1); break;
    case MH_LOAD_VECTOR: renderer_load(v,0xe0,2,-1); break;
    case MH_LOAD_SLOT: renderer_load(v,0x21fc,4,-1); break;
    case MH_LOAD_FACE: renderer_load(v,7,4,-1); break;
    case MH_SAVE_CURSORS: renderer_store(A(7),0x14,4,7); break;
    case MH_RESTORE_CURSORS: renderer_load(A(7),0x2800,4,7); break;
    case MH_SCAN_EXHAUSTED: SET_W(D(0),v); break;
    }
}
static const MotionHooks hooks={outputs,consume_face,divide_exception,NULL};
int glue_C26322(void) { project_record_motion(working(),&hooks); return glue_return(); }
int glue_C26352(void) { publish_motion_slot(working(),&hooks); return glue_return(); }
int glue_C26C72(void) {
    m68ki_push_32(A(6)); A(6)=A(7); project_scene_motion(working(),A(6),&hooks);
    A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return();
}
int glue_C26CC0(void) { test_component_motion(working(),A(6),&hooks); return glue_return(); }
int glue_C26D8A(void) { test_face_motion(working(),A(6),&hooks); return glue_return(); }
