#include "glue_flight_geometry_math.h"
#include "glue_flight_geometry.h"
#include "glue_child_call.h"
#include "flight_geometry.h"
#include "flight_dynamics.h"
#include "glue_flight_record_calls.h"
#include "recomp_ports.h"
static GeometryState working(void) {
    GeometryState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),COND_EQ()}; return w;
}
static GeometryState consume(void *context,enum GeometryChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc06c02,0xc28e24},{0xc28f16,0xc28f08},{0xc27456,0xc27184},{0xc27456,0xc27414}
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret);
    return working();
}
static void outputs(void *context,enum GeometryPhase phase,enum GeometryValue field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case FG_BYTE: SET_B(D(r),v); flags_logic_b(v); break; case FG_WORD: SET_W(D(r),v); flags_logic_w(v); break;
    case FG_LONG: D(r)=v; flags_logic_l(v); break; case FG_POINTER: A(r-FG_ROOT)=v; break;
    case FG_ADD_BYTE: renderer_add_byte(&D(r),v); break; case FG_ADD_WORD: step_add_word(&D(r),v); break; case FG_ADD_LONG: step_add_long(&D(r),v); break;
    case FG_SUB_BYTE: step_subtract_byte(&D(r),v); break; case FG_SUB_WORD: step_subtract_word(&D(r),v); break; case FG_SUB_LONG: step_subtract_long(&D(r),v); break;
    case FG_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break; case FG_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break;
    case FG_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break; case FG_OR_BYTE: SET_B(D(r),D(r)|v); flags_logic_b(D(r)); break;
    case FG_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case FG_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case FG_SWAP: step_swap(&D(r)); break; case FG_ASR_WORD: renderer_asr_word(&D(r),v); break; case FG_ASR_LONG: step_asr_long(&D(r),v); break;
    case FG_ASL_WORD: renderer_asl_word(&D(r),v); break; case FG_ASL_LONG: step_asl_long(&D(r),v); break; case FG_LSR_WORD: flight_lsr_word(&D(r),v); break;
    case FG_ROL_LONG: dynamics_rol_long(&D(r),v); break; case FG_NEG_WORD: renderer_negate(&D(r),2); break; case FG_NEG_LONG: renderer_negate(&D(r),4); break;
    case FG_MULTIPLY: renderer_multiply(&D(r),(uint16_t)v); break;
    case FG_TEST_BYTE: case FG_STORE_BYTE: flags_logic_b(v); break; case FG_TEST_WORD: case FG_STORE_WORD: flags_logic_w(v); break;
    case FG_TEST_LONG: case FG_STORE_LONG: flags_logic_l(v); break;
    case FG_COMPARE_BYTE: step_compare_byte(other,v); break; case FG_COMPARE_WORD: step_compare_word(other,v); break; case FG_COMPARE_LONG: step_compare_long(other,v); break;
    case FG_BIT_TEST: case FG_BIT_SET: case FG_BIT_CLEAR: FLAG_Z=v&(1u<<other); break;
    case FG_REGISTER_BIT_SET: FLAG_Z=v&(1u<<other); D(r)|=1u<<other; break;
    case FG_REGISTER_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case FG_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break; case FG_MEMORY_ADD_WORD: temporary=v; step_add_word(&temporary,other); break;
    case FG_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break; case FG_MEMORY_SUB_WORD: temporary=v; step_subtract_word(&temporary,other); break;
    case FG_MEMORY_SUB_LONG: temporary=v; step_subtract_long(&temporary,other); break; case FG_DECREMENT: SET_W(D(r),v); break;
    case FG_LOAD_WORDS: renderer_load(v,(uint16_t)other,2,-1); break;
    case FG_LOAD_LONGS: renderer_load(v?v:A(7),(uint16_t)other,4,-1); break;
    case FG_STORE_WORDS: case FG_STORE_LONGS: break; /* Domain owns all RAM writes; MOVEM preserves flags. */
    case FG_SAVE_RECORD: m68ki_push_32(A(1)); flags_logic_l(A(1)); break; case FG_RESTORE_RECORD: A(1)=m68ki_pull_32(); break;
    case FG_SAVE_CELL: renderer_store(A(7),0xc0,4,7); break; case FG_RESTORE_CELL: renderer_load(A(7),0x300,4,7); break;
    case FG_SAVE_RATES: renderer_store(A(7),0x740,4,7); break; case FG_RESTORE_RATES: renderer_load(A(7),0x2e0,4,7); break;
    case FG_SAVE_ORIENTATION: renderer_store(A(7),0x80a0,4,7); break; case FG_RESTORE_ORIENTATION: renderer_load(A(7),0x501,4,7); break;
    case FG_SAVE_SCAN: m68ki_push_32(D(5)); flags_logic_l(D(5)); break; case FG_RESTORE_SCAN: D(5)=m68ki_pull_32(); flags_logic_l(D(5)); break;
    case FG_SOUND_ARGUMENTS: m68ki_push_32(0x30); flags_logic_l(0x30); m68ki_push_32(0x1c); flags_logic_l(0x1c); break;
    case FG_NEG_BYTE: renderer_negate(&D(r),1); break;
    case FG_EXCHANGE: temporary=D(r); D(r)=D(v); D(v)=temporary; break;
    case FG_PUSH_INDEX: m68ki_push_16((uint16_t)D(0)); flags_logic_w(D(0)); break;
    case FG_POP_INDEX: SET_W(D(0),m68ki_pull_16()); flags_logic_w(D(0)); break;
    case FG_BEGIN_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break;
    case FG_END_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    }
}
static gaddr frame(void *context) { (void)context; return A(6); }
static GeometryState restored_state(void *context) { (void)context; return working(); }
static gaddr stack(void *context) { (void)context; return A(7); }
static const GeometryHooks hooks={consume,outputs,frame,restored_state,stack,NULL};
int glue_complete_record_history(void) { record_position_history_complete(working(),&hooks); return glue_return(); }
int glue_complete_zone_exit(void) { check_record_zone_exit_complete(working(),&hooks); return glue_return(); }
int glue_complete_candidate_update(void) { update_candidate_record_complete(working(),&hooks); return glue_return(); }
int glue_complete_candidate_faces(void) { test_candidate_faces_complete(working(),A(6),&hooks); return glue_return(); }

int glue_continue_record_zone_exit(const void *arguments) {
    NativeZoneExitCall *call=(NativeZoneExitCall *)arguments;
    if(call->started) call->frame.work=working();
    else { call->started=1; fa18_ports_note_native_edge(0xc25b66u,0xc28e28u); }
    if(update_dynamics_record_zone_exit(&call->frame,&hooks)) return glue_return();
    uint32_t sp=A(7);
    int fault=call->frame.phase==ZONE_AFTER_FAULT;
    uint32_t ret=fault?0xc28e24u:0xc28f08u;
    m68ki_push_32(ret); REG_PC=fault?0xc06c02u:0xc28f16u;
    fa18_ports_native_child_wait(ret,sp);
    return FA18_EXIT_DISPATCH;
}
int glue_schedule_record_zone_exit(void) {
    NativeZoneExitCall call={0};
    call.frame.work=working(); call.frame.phase=ZONE_BEGIN;
    /* Original child events remain live; parent timing is not yet modeled. */
    return fa18_ports_schedule_native_child(glue_continue_record_zone_exit,&call,sizeof call,0);
}
