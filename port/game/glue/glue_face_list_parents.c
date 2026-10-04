#include "glue_face_list_parents_math.h"
#include "glue_face_list_parents.h"
#include "glue_child_call.h"
#include "face_list_parents.h"
#include "glue_unsigned_division_step.h"
static FaceListState working(void) {
    FaceListState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),COND_LT(),COND_EQ()}; return w;
}
static FaceListState consume(void *context,enum FaceListChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc1fb82,0xc1ff3c},
        {0xc1fb82,0xc200be},
        {0xc2469e,0xc200ec},
        {0xc1fb82,0xc20178},
        {0xc2469e,0xc20196},
        {0xc246a0,0xc210d4},
        {0xc246a0,0xc20d46},
        {0xc246a0,0xc20b7a},
        {0xc246a0,0xc20c10},
        {0xc2574a,0xc20862},
        {0xc2574a,0xc20886},
        {0xc21c4c,0xc21c44},
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum FaceListPhase phase,enum FaceListField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case FLS_WORD: SET_W(D(r),v); flags_logic_w(v); break; case FLS_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case FLS_LONG: D(r)=v; flags_logic_l(v); break; case FLS_POINTER: A(r-FLS_REGISTERS)=v; break;
    case FLS_ADD_WORD: step_add_word(&D(r),v); break; case FLS_SUB_WORD: step_subtract_word(&D(r),v); break;
    case FLS_ADD_LONG: step_add_long(&D(r),v); break; case FLS_SUB_LONG: step_subtract_long(&D(r),v); break;
    case FLS_NEG_WORD: renderer_negate(&D(r),2); break; case FLS_SWAP: step_swap(&D(r)); break;
    case FLS_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case FLS_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case FLS_ASL_WORD: renderer_asl_word(&D(r),v); break; case FLS_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case FLS_ASR_WORD: renderer_asr_word(&D(r),v); break; case FLS_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case FLS_LSR_LONG: step_lsr_long(&D(r),v); break; case FLS_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case FLS_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case FLS_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case FLS_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case FLS_COMPARE_WORD: step_compare_word(other,v); break; case FLS_COMPARE_BYTE: step_compare_byte(other,v); break;
    case FLS_TEST_WORD: case FLS_STORE_WORD: flags_logic_w(v); break;
    case FLS_TEST_BYTE: case FLS_STORE_BYTE: flags_logic_b(v); break; case FLS_STORE_LONG: flags_logic_l(v); break;
    case FLS_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case FLS_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case FLS_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case FLS_DECREMENT: SET_W(D(r),v); break;
    case FLS_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case FLS_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case FLS_SAVE_WORDS: renderer_store(A(7),v,2,7); break;
    case FLS_ASR_LONG: step_asr_long(&D(r),v); break;
    case FLS_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case FLS_DIVU: step_divide_unsigned(&D(r),(uint16_t)v); break;
    case FLS_DIVS: renderer_divide(&D(r),(int16_t)v); break;
    case FLS_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break;
    case FLS_COMPARE_LONG: step_compare_long(other,v); break;
    case FLS_MEMORY_LOGIC_LONG: flags_logic_l(v); break;
    case FLS_SAVE_LONGS: renderer_store(A(7),v,4,7); break;
    case FLS_RESTORE_LONGS: renderer_load(A(7),v,4,7); break;
    case FLS_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case FLS_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case FLS_RESTORE_WORDS: renderer_load(A(7),v,2,7); break;
    case FLS_RAW_LONG: D(r)=v; break;
    case FLS_MULS: renderer_multiply(&D(r),(uint16_t)v); break;
    case FLS_MULU: timer_multiply_unsigned(&D(r),(uint16_t)v); break;
    case FLS_POP_POINTER: A(r-FLS_REGISTERS)=m68ki_pull_32(); break;
    case FLS_LOAD_WORDS_AT: renderer_load(v,other,2,-1); break;
    case FLS_STORE_WORDS_AT: renderer_store(v,other,2,-1); break;
    case FLS_LOAD_LONGS_AT: renderer_load(v,other,4,-1); break;
    case FLS_LINK_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break;
    case FLS_UNLINK_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case FLS_NEG_LONG: renderer_negate(&D(r),4); break;
    case FLS_ADD_BYTE: renderer_add_byte(&D(r),v); break;
    case FLS_SUB_BYTE: step_subtract_byte(&D(r),v); break;
    case FLS_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break;
    case FLS_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break;
    case FLS_TEST_LONG: flags_logic_l(v); break;
    case FLS_POP_MEMORY_LONG: temporary=m68ki_pull_32(); wr_u32(v,temporary); flags_logic_l(temporary); break;
    case FLS_ASL_LONG: step_asl_long(&D(r),v); break;
    case FLS_LSR_WORD: message_lsr_word(&D(r),v); break;
    case FLS_STORE_LONGS_AT: renderer_store(v,other,4,-1); break;
    case FLS_EXCHANGE_DATA: temporary=D(r);D(r)=D(v);D(v)=temporary;break;
    case FLS_MEMORY_SUB_WORD: temporary=v;step_subtract_word(&temporary,other);break;
    case FLS_MEMORY_ADD_WORD: temporary=v;step_add_word(&temporary,other);break;
    }
}
static FaceListState restored(void *context) { (void)context; return working(); }
static const FaceListHooks hooks={consume,outputs,restored,NULL};
int glue_C1FF0A(void) { face_list_test_stream_face(working(),&hooks);return glue_return(); }
int glue_C2005C(void) { face_list_tested_face(working(),&hooks);return glue_return(); }
int glue_C20100(void) { face_list_indexed_face_list(working(),&hooks);return glue_return(); }
int glue_C21060(void) { face_list_quad_list(working(),&hooks);return glue_return(); }
int glue_C20C38(void) { face_list_face_grid(working(),&hooks);return glue_return(); }
int glue_C20C22(void) { face_list_face_grid_plain(working(),&hooks);return glue_return(); }
int glue_C20A52(void) { face_list_face_lattice(working(),&hooks);return glue_return(); }
int glue_C20A40(void) { face_list_face_lattice_plain(working(),&hooks);return glue_return(); }
int glue_C20002(void) { face_list_tested_parallelogram(working(),&hooks);return glue_return(); }
int glue_C2084A(void) { face_list_edge_alignment(working(),&hooks);return glue_return(); }
int glue_C2082A(void) { face_list_edge_alignment_test(working(),&hooks);return glue_return(); }
int glue_C219AE(void) { face_list_derive_edge_vertices(working(),&hooks);return glue_return(); }
int glue_C21C4C(void) { face_list_split_edge(working(),&hooks);return glue_return(); }
int glue_C21C2E(void) { face_list_split_record_edges(working(),&hooks);return glue_return(); }
