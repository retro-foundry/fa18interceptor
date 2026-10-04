#include "glue_face_stream_parents_math.h"
#include "glue_face_stream_parents.h"
#include "glue_child_call.h"
#include "face_stream_parents.h"
#include "glue_unsigned_division_step.h"
static FaceStreamState working(void) {
    FaceStreamState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),COND_LT(),COND_EQ()}; return w;
}
static FaceStreamState consume(void *context,enum FaceStreamChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2ee4a,0xc2130a},
        {0xc2ee4a,0xc21214},
        {0xc2ee4a,0xc2138a},
        {0xc246a0,0xc20eb6},
        {0xc2469e,0xc214f6},
        {0xc2469e,0xc21408},
        {0xc2469e,0xc21486},
        {0xc2ee4a,0xc20e22},
        {0xc2ee4a,0xc209ce},
        {0xc2ee4a,0xc20a2c},
        {0xc2469e,0xc21648},
        {0xc2469e,0xc2167c},
        {0xc246a0,0xc211c6},
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum FaceStreamPhase phase,enum FaceStreamField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case FSP_WORD: SET_W(D(r),v); flags_logic_w(v); break; case FSP_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case FSP_LONG: D(r)=v; flags_logic_l(v); break; case FSP_POINTER: A(r-FSP_REGISTERS)=v; break;
    case FSP_ADD_WORD: step_add_word(&D(r),v); break; case FSP_SUB_WORD: step_subtract_word(&D(r),v); break;
    case FSP_ADD_LONG: step_add_long(&D(r),v); break; case FSP_SUB_LONG: step_subtract_long(&D(r),v); break;
    case FSP_NEG_WORD: renderer_negate(&D(r),2); break; case FSP_SWAP: step_swap(&D(r)); break;
    case FSP_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case FSP_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case FSP_ASL_WORD: renderer_asl_word(&D(r),v); break; case FSP_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case FSP_ASR_WORD: renderer_asr_word(&D(r),v); break; case FSP_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case FSP_LSR_LONG: step_lsr_long(&D(r),v); break; case FSP_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case FSP_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case FSP_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case FSP_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case FSP_COMPARE_WORD: step_compare_word(other,v); break; case FSP_COMPARE_BYTE: step_compare_byte(other,v); break;
    case FSP_TEST_WORD: case FSP_STORE_WORD: flags_logic_w(v); break;
    case FSP_TEST_BYTE: case FSP_STORE_BYTE: flags_logic_b(v); break; case FSP_STORE_LONG: flags_logic_l(v); break;
    case FSP_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case FSP_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case FSP_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case FSP_DECREMENT: SET_W(D(r),v); break;
    case FSP_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case FSP_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case FSP_SAVE_WORDS: renderer_store(A(7),v,2,7); break;
    case FSP_ASR_LONG: step_asr_long(&D(r),v); break;
    case FSP_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case FSP_DIVU: step_divide_unsigned(&D(r),(uint16_t)v); break;
    case FSP_DIVS: renderer_divide(&D(r),(int16_t)v); break;
    case FSP_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break;
    case FSP_COMPARE_LONG: step_compare_long(other,v); break;
    case FSP_MEMORY_LOGIC_LONG: flags_logic_l(v); break;
    case FSP_SAVE_LONGS: renderer_store(A(7),v,4,7); break;
    case FSP_RESTORE_LONGS: renderer_load(A(7),v,4,7); break;
    case FSP_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case FSP_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case FSP_RESTORE_WORDS: renderer_load(A(7),v,2,7); break;
    case FSP_RAW_LONG: D(r)=v; break;
    case FSP_MULS: renderer_multiply(&D(r),(uint16_t)v); break;
    case FSP_MULU: timer_multiply_unsigned(&D(r),(uint16_t)v); break;
    case FSP_POP_POINTER: A(r-FSP_REGISTERS)=m68ki_pull_32(); break;
    case FSP_LOAD_WORDS_AT: renderer_load(v,other,2,-1); break;
    case FSP_STORE_WORDS_AT: renderer_store(v,other,2,-1); break;
    case FSP_LOAD_LONGS_AT: renderer_load(v,other,4,-1); break;
    case FSP_LINK_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break;
    case FSP_UNLINK_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case FSP_NEG_LONG: renderer_negate(&D(r),4); break;
    case FSP_ADD_BYTE: renderer_add_byte(&D(r),v); break;
    case FSP_SUB_BYTE: step_subtract_byte(&D(r),v); break;
    case FSP_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break;
    case FSP_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break;
    case FSP_TEST_LONG: flags_logic_l(v); break;
    case FSP_POP_MEMORY_LONG: temporary=m68ki_pull_32(); wr_u32(v,temporary); flags_logic_l(temporary); break;
    case FSP_ASL_LONG: step_asl_long(&D(r),v); break;
    case FSP_LSR_WORD: message_lsr_word(&D(r),v); break;
    case FSP_STORE_LONGS_AT: renderer_store(v,other,4,-1); break;
    case FSP_EXCHANGE_DATA: temporary=D(r);D(r)=D(v);D(v)=temporary;break;
    case FSP_MEMORY_SUB_WORD: temporary=v;step_subtract_word(&temporary,other);break;
    case FSP_MEMORY_ADD_WORD: temporary=v;step_add_word(&temporary,other);break;
    }
}
static FaceStreamState restored(void *context) { (void)context; return working(); }
static const FaceStreamHooks hooks={consume,outputs,restored,NULL};
int glue_C2129C(void) { face_stream_segment_pairs_near(working(),&hooks);return glue_return(); }
int glue_C212B0(void) { face_stream_segment_pairs(working(),&hooks);return glue_return(); }
int glue_C211DC(void) { face_stream_segment_run(working(),&hooks);return glue_return(); }
int glue_C2131C(void) { face_stream_offset_segments(working(),&hooks);return glue_return(); }
int glue_C20E4E(void) { face_stream_parallelogram_face(working(),&hooks);return glue_return(); }
int glue_C20E40(void) { face_stream_parallelogram_face_two(working(),&hooks);return glue_return(); }
int glue_C21490(void) { face_stream_near_parallelogram_face(working(),&hooks);return glue_return(); }
int glue_C2139E(void) { face_stream_offset_face(working(),&hooks);return glue_return(); }
int glue_C21412(void) { face_stream_mixed_face(working(),&hooks);return glue_return(); }
int glue_C20F10(void) { face_stream_extend_parallelograms(working(),&hooks);return glue_return(); }
int glue_C20EC4(void) { face_stream_extend_parallelograms_scaled(working(),&hooks);return glue_return(); }
int glue_C20D68(void) { face_stream_segment_grid(working(),&hooks);return glue_return(); }
int glue_C20904(void) { face_stream_segment_lattice(working(),&hooks);return glue_return(); }
int glue_C21A20(void) { face_stream_offset_block_copies(working(),&hooks);return glue_return(); }
int glue_C217EA(void) { face_stream_extend_block_scaled(working(),&hooks);return glue_return(); }
int glue_C2159E(void) { face_stream_side_face(working(),&hooks);return glue_return(); }
int glue_C210E6(void) { face_stream_quad_strip(working(),&hooks);return glue_return(); }
