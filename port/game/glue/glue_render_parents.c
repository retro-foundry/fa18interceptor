#include "glue_render_parents_math.h"
#include "glue_render_parents.h"
#include "glue_child_call.h"
#include "render_parents.h"
#include "glue_unsigned_division_step.h"
static RenderParentState working(void) {
    RenderParentState w={D(0),D(1),D(2),D(3),D(4),D(5),D(6),D(7),A(0),A(1),A(2),A(3),A(4),A(5),A(6),A(7),COND_LT(),COND_EQ()}; return w;
}
static RenderParentState consume(void *context,enum RenderParentChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2f1c0,0xc2d38a},
        {0xc2ff48,0xc2d392},
        {0xc2fa7e,0xc2d3a2},
        {0xc2469e,0xc21590},
        {0xc2ee4a,0xc21282},
        {0xc2469e,0xc20650},
        {0xc2469e,0xc21784},
        {0xc2469e,0xc217dc},
        {0xc2469e,0xc20498},
        {0xc2469e,0xc2052e},
        {0xc2469e,0xc20580},
        {0xc2469e,0xc203c4},
        {0xc301f0,0xc301e0},
        {0xc304fa,0xc301ea},
        {0xc304b2,0xc301ee},
        {0xc1f2ee,0xc1fa3c},
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret); return working();
}
static void outputs(void *context,enum RenderParentPhase phase,enum RenderParentField field,uint32_t v,uint32_t other) {
    unsigned r=(unsigned)field; uint32_t temporary; (void)context;
    switch(phase) {
    case RDP_WORD: SET_W(D(r),v); flags_logic_w(v); break; case RDP_BYTE: SET_B(D(r),v); flags_logic_b(v); break;
    case RDP_LONG: D(r)=v; flags_logic_l(v); break; case RDP_POINTER: A(r-RDP_REGISTERS)=v; break;
    case RDP_ADD_WORD: step_add_word(&D(r),v); break; case RDP_SUB_WORD: step_subtract_word(&D(r),v); break;
    case RDP_ADD_LONG: step_add_long(&D(r),v); break; case RDP_SUB_LONG: step_subtract_long(&D(r),v); break;
    case RDP_NEG_WORD: renderer_negate(&D(r),2); break; case RDP_SWAP: step_swap(&D(r)); break;
    case RDP_EXT_WORD: SET_W(D(r),(int16_t)(int8_t)D(r)); flags_logic_w(D(r)); break;
    case RDP_EXT_LONG: D(r)=(uint32_t)(int32_t)(int16_t)D(r); flags_logic_l(D(r)); break;
    case RDP_ASL_WORD: renderer_asl_word(&D(r),v); break; case RDP_LSL_WORD: menu_lsl_word(&D(r),v); break;
    case RDP_ASR_WORD: renderer_asr_word(&D(r),v); break; case RDP_ASR_BYTE: hud_parent_asr_byte(&D(r),v); break;
    case RDP_LSR_LONG: step_lsr_long(&D(r),v); break; case RDP_ROR_WORD: hud_parent_ror_word(&D(r),v); break;
    case RDP_AND_WORD: SET_W(D(r),D(r)&v); flags_logic_w(D(r)); break; case RDP_AND_BYTE: SET_B(D(r),D(r)&v); flags_logic_b(D(r)); break;
    case RDP_OR_WORD: SET_W(D(r),D(r)|v); flags_logic_w(D(r)); break;
    case RDP_COMPARE_WORD: step_compare_word(other,v); break; case RDP_COMPARE_BYTE: step_compare_byte(other,v); break;
    case RDP_TEST_WORD: case RDP_STORE_WORD: flags_logic_w(v); break;
    case RDP_TEST_BYTE: case RDP_STORE_BYTE: flags_logic_b(v); break; case RDP_STORE_LONG: flags_logic_l(v); break;
    case RDP_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case RDP_BIT_CLEAR: FLAG_Z=v&(1u<<other); D(r)&=~(1u<<other); break;
    case RDP_MEMORY_SUB_BYTE: temporary=v; step_subtract_byte(&temporary,(uint8_t)other); break;
    case RDP_DECREMENT: SET_W(D(r),v); break;
    case RDP_PUSH_LONG: m68ki_push_32(v); flags_logic_l(v); break;
    case RDP_POP_LONG: D(r)=m68ki_pull_32(); flags_logic_l(D(r)); break;
    case RDP_SAVE_WORDS: renderer_store(A(7),v,2,7); break;
    case RDP_ASR_LONG: step_asr_long(&D(r),v); break;
    case RDP_LSR_BYTE: action_lsr_byte(&D(r),v); break;
    case RDP_DIVU: step_divide_unsigned(&D(r),(uint16_t)v); break;
    case RDP_DIVS: renderer_divide(&D(r),(int16_t)v); break;
    case RDP_AND_LONG: D(r)&=v; flags_logic_l(D(r)); break;
    case RDP_COMPARE_LONG: step_compare_long(other,v); break;
    case RDP_MEMORY_LOGIC_LONG: flags_logic_l(v); break;
    case RDP_SAVE_LONGS: renderer_store(A(7),v,4,7); break;
    case RDP_RESTORE_LONGS: renderer_load(A(7),v,4,7); break;
    case RDP_PUSH_WORD: m68ki_push_16((uint16_t)v); flags_logic_w(v); break;
    case RDP_POP_WORD: SET_W(D(r),m68ki_pull_16()); flags_logic_w(D(r)); break;
    case RDP_RESTORE_WORDS: renderer_load(A(7),v,2,7); break;
    case RDP_RAW_LONG: D(r)=v; break;
    case RDP_MULS: renderer_multiply(&D(r),(uint16_t)v); break;
    case RDP_MULU: timer_multiply_unsigned(&D(r),(uint16_t)v); break;
    case RDP_POP_POINTER: A(r-RDP_REGISTERS)=m68ki_pull_32(); break;
    case RDP_LOAD_WORDS_AT: renderer_load(v,other,2,-1); break;
    case RDP_STORE_WORDS_AT: renderer_store(v,other,2,-1); break;
    case RDP_LOAD_LONGS_AT: renderer_load(v,other,4,-1); break;
    case RDP_LINK_FRAME: m68ki_push_32(A(6)); A(6)=A(7); A(7)-=v; break;
    case RDP_UNLINK_FRAME: A(7)=A(6); A(6)=m68ki_pull_32(); break;
    case RDP_NEG_LONG: renderer_negate(&D(r),4); break;
    case RDP_ADD_BYTE: renderer_add_byte(&D(r),v); break;
    case RDP_SUB_BYTE: step_subtract_byte(&D(r),v); break;
    case RDP_MEMORY_ADD_BYTE: temporary=v; renderer_add_byte(&temporary,other); break;
    case RDP_MEMORY_ADD_LONG: temporary=v; step_add_long(&temporary,other); break;
    case RDP_TEST_LONG: flags_logic_l(v); break;
    case RDP_POP_MEMORY_LONG: temporary=m68ki_pull_32(); wr_u32(v,temporary); flags_logic_l(temporary); break;
    case RDP_ASL_LONG: step_asl_long(&D(r),v); break;
    case RDP_LSR_WORD: message_lsr_word(&D(r),v); break;
    case RDP_STORE_LONGS_AT: renderer_store(v,other,4,-1); break;
    case RDP_EXCHANGE_DATA: temporary=D(r);D(r)=D(v);D(v)=temporary;break;
    case RDP_MEMORY_SUB_WORD: temporary=v;step_subtract_word(&temporary,other);break;
    case RDP_MEMORY_ADD_WORD: temporary=v;step_add_word(&temporary,other);break;
    }
}
static RenderParentState restored(void *context) { (void)context; return working(); }
static const RenderParentHooks hooks={consume,outputs,restored,NULL};
int glue_C2D16C(void) { render_shape(working(),&hooks);return glue_return(); }
int glue_C21500(void) { render_block_face(working(),&hooks);return glue_return(); }
int glue_C2122A(void) { render_offset_run(working(),&hooks);return glue_return(); }
int glue_C20592(void) { render_split_square(working(),&hooks);return glue_return(); }
int glue_C2168A(void) { render_side_triangle(working(),&hooks);return glue_return(); }
int glue_C203D0(void) { render_square_faces(working(),&hooks);return glue_return(); }
int glue_C201A6(void) { render_record_shadow(working(),&hooks);return glue_return(); }
int glue_C3019C(void) { render_marker_polygon(working(),&hooks);return glue_return(); }
int glue_C1FB82(void) { render_face_side(working(),&hooks);return glue_return(); }
int glue_C1F99A(void) { render_vertices(working(),&hooks);return glue_return(); }
