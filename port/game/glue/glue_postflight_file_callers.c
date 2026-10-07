#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_postflight_file_callers.h"
#include "postflight_file_callers.h"
#include "globals.h"
static void full(unsigned reg,uint32_t v) { D(reg)=v; flags_logic_l(v); }
static void push(uint32_t v) { m68ki_push_32(v); flags_logic_l(v); }
static int32_t consume(void *context,enum PostflightFileChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc53b30,0xc0ef1c},{0xc53a7c,0xc0ef46},{0xc53ac8,0xc0ef5a},
        {0xc53a98,0xc0ef66},{0xc53b48,0xc0efc8},{0xc53fc0,0xc162ea},
        {0xc0ef08,0xc162f0},{0xc16386,0xc162fe},{0xc1631c,0xc16314},
        {0xc53fb0,0xc1631a},{0xc539a8,0xc16332},{0xc539f4,0xc1635c},
        {0xc539c4,0xc16380},{0xc539a8,0xc1639c},{0xc539d8,0xc163d4},
        {0xc539c4,0xc163e6}
    };
    unsigned arguments=0; int32_t result; (void)context;
    switch(child) {
    case PFF_ALLOCATE_CHECK: push(0x10001); full(0,40); push(D(0)); arguments=8; break;
    case PFF_LOCK_CHECK:
        full(1,0xfffffffeu); push(D(1)); m68ki_push_32(0xc07fe4);
        wr_u32(A(6)-16,D(0)); flags_logic_l(D(0)); arguments=8; break;
    case PFF_INFO_CHECK:
        push(rd_u32(MODE_FILE_INFO_POINTER)); push(D(0)); wr_u32(A(6)-12,D(0)); flags_logic_l(D(0)); arguments=8; break;
    case PFF_UNLOCK_CHECK: push(rd_u32(A(6)-12)); arguments=4; break;
    case PFF_FREE_CHECK: full(0,40); push(D(0)); push(rd_u32(MODE_FILE_INFO_POINTER)); arguments=8; break;
    case PFF_OPEN_SAVE: push(0x3ee); m68ki_push_32(0xc08012); arguments=8; break;
    case PFF_OPEN_LOAD: push(0x3ed); m68ki_push_32(0xc0801d); arguments=8; break;
    case PFF_WRITE_SAVE: case PFF_READ_LOAD:
        A(0)=rd_u32(MODE_TABLE); full(0,78); push(D(0)); push(A(0)); push(rd_u32(A(6)-12));
        wr_u32(A(6)-4,A(0)); flags_logic_l(A(0)); arguments=12; break;
    case PFF_CLOSE_SAVE: push(rd_u32(A(6)-12)); arguments=4; break;
    case PFF_CLOSE_LOAD: arguments=4; break;
    default: break;
    }
    result=glue_complete_child(sites[child].entry,sites[child].ret); A(7)+=arguments; return result;
}
static void outputs(void *context,enum PostflightFilePhase p,uint32_t value,uint32_t other) {
    uint32_t temporary; (void)context;
    switch(p) {
    case PFF_D0_BYTE: SET_B(D(0),value); flags_logic_b(value); break;
    case PFF_D0_LONG: full(0,value); break;
    case PFF_EXT_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case PFF_EXT_LONG: full(0,value); break;
    case PFF_A0: A(0)=value; break;
    case PFF_STORE_BYTE: flags_logic_b(value); break;
    case PFF_STORE_WORD: case PFF_TEST_WORD: flags_logic_w(value); break;
    case PFF_STORE_LONG: case PFF_TEST_LONG: flags_logic_l(value); break;
    case PFF_CMP_BYTE: step_compare_byte((uint8_t)other,(uint8_t)value); break;
    case PFF_CMP_LONG: step_compare_long(other,value); break;
    case PFF_ADD_LONG: step_add_long(&D(0),value); break;
    case PFF_SUB_WORD: step_subtract_word(&D(0),(uint16_t)value); break;
    case PFF_AND_LONG: D(0)&=value; flags_logic_l(D(0)); break;
    case PFF_LSR_LONG: step_lsr_long(&D(0),value); break;
    case PFF_ADD_MEMORY_BYTE: temporary=value; renderer_add_byte(&temporary,(uint8_t)other); break;
    case PFF_SUB_MEMORY_BYTE: temporary=value; step_subtract_byte(&temporary,(uint8_t)other); break;
    case PFF_ADD_MEMORY_LONG: temporary=value; step_add_long(&temporary,other); break;
    case PFF_SUB_MEMORY_LONG: temporary=value; step_subtract_long(&temporary,other); break;
    case PFF_PUSH_LOAD_HANDLE: push(rd_u32(A(6)-12)); break;
    }
}
static const PostflightFileHooks hooks={consume,outputs,NULL};
static void link_frame(unsigned size) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=size; }
static int leave_frame(void) { A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C0F56A(void) { link_frame(2); format_postflight_hex_frame(A(6),&hooks); return leave_frame(); }
int glue_C0EF08(void) { link_frame(16); check_postflight_mode_file(A(6),&hooks); return leave_frame(); }
int glue_C162E4(void) { refresh_postflight_mode_file(&hooks); return glue_return(); }
int glue_C1631C(void) { link_frame(12); save_postflight_mode_file(A(6),&hooks); return leave_frame(); }
int glue_C16386(void) { link_frame(12); read_postflight_mode_file(A(6),&hooks); return leave_frame(); }
