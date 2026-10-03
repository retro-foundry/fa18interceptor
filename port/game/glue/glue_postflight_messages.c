#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_postflight_messages.h"
#include "postflight_messages.h"
#include "globals.h"
static void full(unsigned reg,uint32_t v) { D(reg)=v; flags_logic_l(v); }
static void push(uint32_t v) { m68ki_push_32(v); flags_logic_l(v); }
static int32_t consume(void *context,enum PostflightMessageChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc16cd8,0xc0f4e2},{0xc16b8c,0xc0f4e8},{0xc53b74,0xc0f4f4},
        {0xc53b88,0xc0f506},{0xc16d4c,0xc0f50e},{0xc2527c,0xc0f514},
        {0xc1787a,0xc0f51a},{0xc17104,0xc0f534},{0xc1712c,0xc0f548},
        {0xc08ee4,0xc0f556},{0xc08eb8,0xc0f55c},{0xc08f26,0xc0f81c},
        {0xc0f56a,0xc0f900},{0xc17b96,0xc0f91a},{0xc11312,0xc110dc},
        {0xc24fa4,0xc1110e},{0xc1643a,0xc11182},{0xc11350,0xc11302},
        {0xc1643a,0xc11308},{0xc08ed0,0xc11374},{0xc11acc,0xc113f8},
        {0xc2fd22,0xc1141c},{0xc11acc,0xc1145a},{0xc2fd22,0xc11476},
        {0xc11acc,0xc1149c},{0xc11312,0xc114a4},{0xc2fd22,0xc114aa},
        {0xc162e4,0xc114c6},{0xc2fd22,0xc114dc},{0xc162e4,0xc114f4},
        {0xc11acc,0xc11530},{0xc2fd22,0xc115ae},{0xc11acc,0xc115e8},
        {0xc2fd22,0xc11684},{0xc11312,0xc1168c},{0xc11312,0xc116d4},
        {0xc11312,0xc11748}
    };
    unsigned arguments=0; int32_t result; (void)context;
    switch(child) {
    case PM_OPEN_TEXT: m68ki_push_32(0xc07fec); arguments=4; break;
    case PM_SELECT_TEXT:
        full(1,0xffffff80u); push(D(1)); push(D(0));
        wr_u32(A(6)-4,D(0)); flags_logic_l(D(0)); arguments=8; break;
    case PM_SET_BOUNDS:
        full(0,960); push(D(0)); push(D(0)); full(0,0xfffffc40u); push(D(0)); push(D(0)); arguments=16; break;
    case PM_SET_CENTRE: full(0,100); push(D(0)); push(160); arguments=8; break;
    case PM_FORMAT_TEXT:
        full(0,4); push(D(0)); push(rd_u32(A(6)-8)); push(A(0));
        wr_u32(A(6)-14,A(0)); flags_logic_l(A(0)); arguments=12; break;
    case PM_DELAY_TEXT: full(0,50); push(D(0)); arguments=4; break;
    case PM_INDEXED_MESSAGE:
        full(1,0); SET_W(D(1),rd_u16(A(6)-2)); flags_logic_w(D(1)); full(0,4);
        push(D(0)); push(0); push(D(1)); arguments=12; break;
    case PM_LOAD_ERROR_TABLE: case PM_LOAD_INTRO_TABLE: case PM_LOAD_STATUS_TABLE: case PM_LOAD_RETRY_TABLE:
        m68ki_push_32(0xc08490); arguments=4; break;
    case PM_LOAD_RETURN_TABLE: m68ki_push_32(0xc08510); arguments=4; break;
    default: break;
    }
    result=glue_complete_child(sites[child].entry,sites[child].ret); A(7)+=arguments; return result;
}
static void outputs(void *context,enum PostflightMessagePhase p,uint32_t value,uint32_t other) {
    uint32_t temporary; (void)context;
    switch(p) {
    case PM_D0_BYTE: SET_B(D(0),value); flags_logic_b(value); break;
    case PM_D0_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case PM_D0_LONG: full(0,value); break;
    case PM_D1_BYTE: SET_B(D(1),value); flags_logic_b(value); break;
    case PM_EXT_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case PM_EXT_LONG: full(0,value); break;
    case PM_TEST_BYTE: case PM_STORE_BYTE: flags_logic_b(value); break;
    case PM_TEST_WORD: case PM_STORE_WORD: flags_logic_w(value); break;
    case PM_TEST_LONG: case PM_STORE_LONG: flags_logic_l(value); break;
    case PM_A0: A(0)=value; break;
    case PM_A1: A(1)=value; break;
    case PM_CMP_BYTE: step_compare_byte((uint8_t)other,(uint8_t)value); break;
    case PM_CMP_WORD: step_compare_word((uint16_t)other,(uint16_t)value); break;
    case PM_CMP_LONG: step_compare_long(other,value); break;
    case PM_ADD_BYTE: renderer_add_byte(&D(0),value); break;
    case PM_ADD_WORD: step_add_word(&D(0),(uint16_t)value); break;
    case PM_SUB_WORD: step_subtract_word(&D(0),(uint16_t)value); break;
    case PM_SUB_LONG: step_subtract_long(&D(0),value); break;
    case PM_SHIFT_LONG: step_asl_long(&D(0),value); break;
    case PM_OR_D1: SET_B(D(1),D(1)|value); flags_logic_b(D(1)); break;
    case PM_BIT_D0: FLAG_Z=value&(1u<<other); break;
    case PM_ADD_MEMORY_WORD: temporary=value; step_add_word(&temporary,(uint16_t)other); break;
    case PM_ADD_MEMORY_LONG: temporary=value; step_add_long(&temporary,other); break;
    case PM_DIRECT_CALLBACK: flags_logic_l(value); break;
    }
}
static const PostflightMessageHooks hooks={consume,outputs,NULL};
static void link_frame(unsigned size) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=size; }
static int leave_frame(void) { A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C0F4D8(void) { link_frame(4); initialise_postflight_text(&hooks); return leave_frame(); }
int glue_C0F812(void) { link_frame(14); copy_postflight_text(A(6),&hooks); return leave_frame(); }
int glue_C11078(void) { raise_postflight_message_event(&hooks); return glue_return(); }
int glue_C110A4(void) { link_frame(6); prepare_postflight_messages(A(6),&hooks); return leave_frame(); }
int glue_C11350(void) { link_frame(4); record_postflight_outcome(A(6),&hooks); return leave_frame(); }
int glue_C113E4(void) { queue_postflight_text_error(&hooks); return glue_return(); }
int glue_C1141E(void) { wait_postflight_text_error(&hooks); return glue_return(); }
int glue_C11446(void) { queue_postflight_intro(&hooks); return glue_return(); }
int glue_C11478(void) { accept_postflight_return(&hooks); return glue_return(); }
int glue_C114D2(void) { link_frame(4); prepare_postflight_status(A(6),&hooks); return leave_frame(); }
int glue_C1159E(void) { wait_postflight_status(&hooks); return glue_return(); }
int glue_C115BA(void) { link_frame(10); prepare_postflight_retry(A(6),&hooks); return leave_frame(); }
int glue_C1169A(void) { wait_postflight_retry_message(&hooks); return glue_return(); }
int glue_C116B0(void) { wait_postflight_retry_input(&hooks); return glue_return(); }
int glue_C116CE(void) { advance_postflight_retry(&hooks); return glue_return(); }
int glue_C11738(void) { finish_postflight_retry(&hooks); return glue_return(); }
