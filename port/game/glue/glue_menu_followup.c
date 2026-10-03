#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_menu_followup.h"
#include "menu_followup.h"
#include "globals.h"
static void push(uint32_t value) { step_predecrement_long(value); flags_logic_l(value); }
static uint32_t consume(void *context,enum MenuFollowupChild child,uint32_t value,gaddr address) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc11312,0xc1048a},{0xc53fc0,0xc16450},{0xc0ef08,0xc1645c},
        {0xc53f9c,0xc16470},{0xc539a8,0xc16494},{0xc53f9c,0xc164a2},
        {0xc539f4,0xc164d0},{0xc53f9c,0xc164e0},{0xc539c4,0xc164ec},
        {0xc53f9c,0xc164f6},{0xc53fb0,0xc1650e}
    };
    unsigned arguments=0;
    (void)context;
    switch(child) {
    case MF_FILE_YIELD_BEFORE_OPEN: case MF_FILE_YIELD_AFTER_CLOSE: push(0); arguments=4; break;
    case MF_FILE_YIELD_AFTER_OPEN: case MF_FILE_YIELD_AFTER_READ: arguments=4; break;
    case MF_FILE_OPEN: push(value); m68ki_push_32(address); arguments=8; break;
    case MF_FILE_READ:
        A(0)=address; D(0)=78; flags_logic_l(D(0)); push(D(0)); push(A(0)); push(rd_u32(A(6)-12));
        wr_u32(A(6)-4,A(0)); flags_logic_l(A(0)); arguments=12; break;
    case MF_FILE_CLOSE: push(rd_u32(A(6)-12)); arguments=4; break;
    default: break;
    }
    glue_complete_child(sites[child].entry,sites[child].ret); A(7)+=arguments;
    return D(0);
}
static void outputs(void *context,enum MenuFollowupPhase phase,uint32_t value,gaddr address) {
    (void)context;
    switch(phase) {
    case MF_BYTE_TEST: case MF_BYTE_STORE: flags_logic_b(value); break;
    case MF_WORD_STORE: flags_logic_w(value); break;
    case MF_WORD_D0: SET_W(D(0),value); flags_logic_w(value); break;
    case MF_CALLBACK: A(0)=value; flags_logic_l(value); break;
    case MF_VIEWPORT_COMPARE:
        SET_B(D(0),value); SET_B(D(1),address); step_compare_byte((uint8_t)address,(uint8_t)value); break;
    case MF_ONE_D0: D(0)=1; flags_logic_l(1); break;
    case MF_LATCH: SET_B(D(0),value); SET_B(D(1),address); flags_logic_b(value); break;
    case MF_SEQUENCE_D0: SET_B(D(0),value); flags_logic_b(value); break;
    case MF_QUEUE_START:
        SET_W(D(0),value); flags_logic_w(value); wr_u32(A(6)-4,address); flags_logic_l(address); break;
    case MF_QUEUE_OFF:
        SET_B(D(1),value); flags_logic_b(value); wr_u16(A(6)-6,(uint16_t)address);
        flags_logic_w(address); flags_logic_b(D(1)); break;
    case MF_MODE_COMPARE: step_compare_word((uint16_t)address,(uint16_t)value); break;
    case MF_MODE_SUBTRACT: step_subtract_word(&D(0),(uint16_t)address); break;
    case MF_QUEUE_CODE: A(0)=address; break;
    case MF_QUEUE_NEXT:
        { uint32_t cursor=rd_u32(A(6)-4); step_add_long(&cursor,2); wr_u32(A(6)-4,cursor); } break;
    case MF_FILE_STATUS: flags_logic_w(value); break;
    case MF_FILE_HANDLE:
        push(0); wr_u32(A(6)-12,value); flags_logic_l(value); break;
    case MF_FILE_HANDLE_TEST: flags_logic_l(value); break;
    case MF_FILE_READ_RESULT: push(0); wr_u32(A(6)-8,value); flags_logic_l(value); break;
    case MF_FILE_COMPARE: step_compare_long(0xffffffffu,value); break;
    case MF_FILE_ZERO: D(0)=0; flags_logic_l(0); break;
    }
}
static const MenuFollowupHooks hooks={consume,outputs,NULL};
static void link_frame(unsigned bytes) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=bytes; }
static int leave_frame(void) { A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C1029E(void) { poll_menu_viewport(&hooks,0); return glue_return(); }
int glue_C10418(void) { poll_menu_viewport(&hooks,1); return glue_return(); }
int glue_C10458(void) { follow_menu_key_or_countdown(&hooks); return glue_return(); }
int glue_C10678(void) { link_frame(6); advance_menu_mode_messages(&hooks); return leave_frame(); }
int glue_C1643A(void) { link_frame(12); load_menu_mode_file(&hooks); return leave_frame(); }
