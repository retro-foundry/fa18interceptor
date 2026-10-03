#include "glue_main_loop_control_messages_math.h"
#include "glue_child_call.h"
#include "glue_main_loop_control_messages.h"
#include "main_loop_control_messages.h"
static MessageWorking working(void) {
    MessageWorking w={A(0),A(1),A(2),A(3),A(4),A(5),D(1),D(4),D(5),D(3),D(6)}; return w;
}
static void full(unsigned r,uint32_t v) { D(r)=v; flags_logic_l(v); }
static void push(uint32_t v) { m68ki_push_32(v); flags_logic_l(v); }
static MessageWorking consume(void *context,enum MainControlChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2f490,0xc151a4},{0xc17f8c,0xc152dc},{0xc181a0,0xc15350},{0xc181a0,0xc15362},
        {0xc17f8c,0xc15374},{0xc15688,0xc153aa},{0xc153fc,0xc153d2},
        {0xc3316a,0xc32c38},{0xc25246,0xc32ebe},{0xc1643a,0xc32ed8},{0xc3316a,0xc33058},
        {0xc330fe,0xc3307a},{0xc330fe,0xc33096},{0xc330fe,0xc330b2},{0xc330fe,0xc330ce}
    };
    unsigned arguments=0; (void)context;
    if(child==MC_CONTROL_TONE || child==MC_CONTROL_FALLBACK_TONE) {
        full(0,48); push(D(0)); full(0,28); push(D(0)); arguments=8;
    } else if(child==MC_CONTROL_ALERT_FIRST || child==MC_CONTROL_ALERT_SECOND) {
        full(0,child==MC_CONTROL_ALERT_FIRST?30:22); push(D(0)); full(0,6); push(D(0)); arguments=8;
    } else if(child==MC_BEGIN_RECORD || child==MC_ADVANCE_RECORD) { push(D(0)); arguments=4; }
    glue_complete_child(sites[child].entry,sites[child].ret); A(7)+=arguments; return working();
}
static void outputs(void *context,enum MainControlPhase p,uint32_t v,uint32_t other) {
    uint32_t t; (void)context;
    switch(p) {
    case MC_PRIMARY_BYTE: SET_B(D(0),v); flags_logic_b(v); break;
    case MC_PRIMARY_WORD: SET_W(D(0),v); flags_logic_w(v); break;
    case MC_PRIMARY_LONG: full(0,v); break;
    case MC_SECONDARY_BYTE: SET_B(D(1),v); flags_logic_b(v); break;
    case MC_SECONDARY_WORD: SET_W(D(1),v); flags_logic_w(v); break;
    case MC_SECONDARY_LONG: full(1,v); break;
    case MC_CHARACTER_BYTE: SET_B(D(4),v); flags_logic_b(v); break;
    case MC_CHARACTER_LONG: full(4,v); break;
    case MC_INDEX_BYTE: SET_B(D(5),v); flags_logic_b(v); break;
    case MC_INDEX_WORD: SET_W(D(5),v); flags_logic_w(v); break;
    case MC_STYLE_WORD: SET_W(D(3),v); flags_logic_w(v); break;
    case MC_COLOUR_WORD: SET_W(D(6),v); flags_logic_w(v); break;
    case MC_STRIDE_WORD: SET_W(D(7),v); flags_logic_w(v); break;
    case MC_LOOKUP: A(0)=v; break;
    case MC_POSITIONS: A(1)=v; break;
    case MC_TEXT: A(2)=v; break;
    case MC_GLYPH: A(3)=v; break;
    case MC_ORIGIN: A(4)=v; break;
    case MC_PLANES: A(5)=v; break;
    case MC_STORE_BYTE: case MC_TEST_BYTE: flags_logic_b(v); break;
    case MC_STORE_WORD: case MC_TEST_WORD: flags_logic_w(v); break;
    case MC_STORE_LONG: flags_logic_l(v); break;
    case MC_COMPARE_BYTE: step_compare_byte(other,v); break;
    case MC_COMPARE_WORD: step_compare_word(other,v); break;
    case MC_BIT_TEST: FLAG_Z=v&(1u<<other); break;
    case MC_PRIMARY_SUB_BYTE: step_subtract_byte(&D(0),(uint8_t)v); break;
    case MC_PRIMARY_SUB_WORD: step_subtract_word(&D(0),(uint16_t)v); break;
    case MC_PRIMARY_ADD_LONG: step_add_long(&D(0),v); break;
    case MC_PRIMARY_AND_BYTE: SET_B(D(0),D(0)&v); flags_logic_b(D(0)); break;
    case MC_PRIMARY_AND_WORD: SET_W(D(0),D(0)&v); flags_logic_w(D(0)); break;
    case MC_PRIMARY_OR_BYTE: SET_B(D(0),D(0)|v); flags_logic_b(D(0)); break;
    case MC_PRIMARY_OR_WORD: SET_W(D(0),D(0)|v); flags_logic_w(D(0)); break;
    case MC_PRIMARY_EXT_WORD: SET_W(D(0),v); flags_logic_w(v); break;
    case MC_PRIMARY_EXT_LONG: full(0,v); break;
    case MC_PRIMARY_SHIFT_WORD: renderer_asl_word(&D(0),v); break;
    case MC_PRIMARY_SHIFT_LONG: step_asl_long(&D(0),v); break;
    case MC_PRIMARY_DOUBLE: step_add_word(&D(0),D(0)); break;
    case MC_PRIMARY_ADD_SECONDARY: step_add_word(&D(0),D(1)); break;
    case MC_SECONDARY_AND_BYTE: SET_B(D(1),D(1)&v); flags_logic_b(D(1)); break;
    case MC_SECONDARY_AND_WORD: SET_W(D(1),D(1)&v); flags_logic_w(D(1)); break;
    case MC_SECONDARY_OR_BYTE: SET_B(D(1),D(1)|v); flags_logic_b(D(1)); break;
    case MC_SECONDARY_ADD_WORD: step_add_word(&D(1),v); break;
    case MC_SECONDARY_SUB_BYTE: step_subtract_byte(&D(1),(uint8_t)v); break;
    case MC_SECONDARY_EXT_WORD: SET_W(D(1),v); flags_logic_w(v); break;
    case MC_CHARACTER_AND_WORD: SET_W(D(4),D(4)&v); flags_logic_w(D(4)); break;
    case MC_CHARACTER_SUB_WORD: step_subtract_word(&D(4),(uint16_t)v); break;
    case MC_CHARACTER_DOUBLE: step_add_word(&D(4),D(4)); break;
    case MC_INDEX_EXT_WORD: SET_W(D(5),v); flags_logic_w(v); break;
    case MC_INDEX_INCREMENT: renderer_add_byte(&D(5),(uint8_t)v); break;
    case MC_OFFSET_EXT_LONG: full(5,v); break;
    case MC_OFFSET_ADD_LONG: step_add_long(&D(5),v); break;
    case MC_COLOUR_FROM_CONTROL: SET_B(D(6),D(1)); flags_logic_b(D(6)); SET_W(D(6),D(6)&0xf0); flags_logic_w(D(6)); message_lsr_word(&D(6),4); break;
    case MC_MEMORY_ADD_BYTE: t=v; renderer_add_byte(&t,(uint8_t)other); break;
    case MC_MEMORY_SUB_BYTE: t=v; step_subtract_byte(&t,(uint8_t)other); break;
    case MC_MEMORY_ADD_WORD: t=v; step_add_word(&t,other); break;
    case MC_MEMORY_SUB_WORD: t=v; step_subtract_word(&t,(uint16_t)other); break;
    case MC_MESSAGE_CLEAR_BEGIN: A(0)=0xc457e1; A(3)=0xc457eb; full(0,9); break;
    case MC_MESSAGE_CLEAR_BYTE: ++A(0); ++A(3); flags_logic_b(0); SET_W(D(0),(uint16_t)(D(0)-1)); break;
    case MC_MESSAGE_CLEAR_DONE: full(4,0); break;
    case MC_CURSOR_LOAD: A(1)=rd_u32(v); A(2)=rd_u32(v+4); A(4)=rd_u32(v+8); break;
    case MC_CURSOR_STORE: break;
    case MC_SELECT_MESSAGE: SET_W(D(2),v); flags_logic_w(v); break;
    case MC_GLYPH_SELECT: A(3)=v; full(4,v); break;
    case MC_PLANE_ARGUMENT: full(1,v); step_add_long(&D(1),other); break;
    case MC_PLANE_STYLE: SET_W(D(2),v); flags_logic_w(v); SET_W(D(2),D(2)|other); flags_logic_w(D(2)); break;
    }
}
static const MainControlHooks hooks={consume,outputs,NULL};
int glue_C1518C(void) {
    m68ki_push_32(A(6)); A(6)=A(7); A(7)-=22; advance_main_loop_control_records(A(6),&hooks);
    A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return();
}
int glue_C32CEE(void) { advance_main_loop_message_sequence(working(),&hooks); return glue_return(); }
