#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_menu_setup.h"
#include "menu_setup.h"
#include "globals.h"

static void push(uint32_t value) { step_predecrement_long(value); flags_logic_l(value); }
static void full_d0(uint32_t value) { D(0)=value; flags_logic_l(value); }
static void consume(void *context,enum MenuSetupCall call,uint32_t value) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc17b96,0xc0fc1e},{0xc2fd22,0xc0fc2e},{0xc11312,0xc0fc32},
        {0xc0e78a,0xc0fc3e},{0xc11acc,0xc0fc52},
        {0xc0f4a6,0xc17bb6},{0xc17b2c,0xc17bc4},{0xc17b2c,0xc17bd8},
        {0xc17b2c,0xc17bfe},{0xc17b2c,0xc17c12},{0xc0f4a6,0xc17c26}
    };
    unsigned arguments=0;
    (void)context;
    switch(call) {
    case MENU_SETUP_SOUND: full_d0(value); push(value); arguments=4; break;
    case MENU_SETUP_DELAY: push(value); arguments=4; break;
    case MENU_SETUP_SCRIPT: m68ki_push_32(value); arguments=4; break; /* PEA preserves flags. */
    case MENU_SOUND_FIXED_FIRST:
        full_d0(63); push(D(0)); push(0); full_d0(13); push(D(0)); arguments=12; break;
    case MENU_SOUND_FIXED_SECOND:
        full_d0(63); push(D(0)); full_d0(1); push(D(0)); full_d0(14); push(D(0)); arguments=12; break;
    case MENU_SOUND_ARGUMENT_FIRST:
        push(rd_u32(A(6)+8)); push(0); full_d0(35); push(D(0)); arguments=12; break;
    case MENU_SOUND_ARGUMENT_SECOND:
        push(rd_u32(A(6)+8)); full_d0(1); push(D(0)); full_d0(36); push(D(0)); arguments=12; break;
    default: break;
    }
    glue_complete_child(sites[call].entry,sites[call].ret); A(7)+=arguments;
}
static void outputs(void *context,enum MenuSetupPhase phase,uint32_t value,gaddr address) {
    (void)context;
    switch(phase) {
    case MENU_SETUP_BYTE_TEST: case MENU_SETUP_BYTE_STORE: flags_logic_b(value); break;
    case MENU_SETUP_BYTE_D0: SET_B(D(0),value); flags_logic_b(value); break;
    case MENU_SETUP_WORD_STORE: flags_logic_w(value); break;
    case MENU_SETUP_LONG_STORE: flags_logic_l(value); break;
    case MENU_SETUP_BIT_TEST: FLAG_Z=value&(1u<<address); break;
    case MENU_SETUP_CURSOR: wr_u32(A(6)-4,value); flags_logic_l(value); break;
    case MENU_SETUP_QUEUE_WORD: A(0)=address; flags_logic_w(value); break;
    case MENU_SETUP_QUEUE_NEXT: A(0)=value; break;
    case MENU_SETUP_CALLBACK: A(1)=value; flags_logic_l(value); break;
    case MENU_SETUP_TAIL_CALLBACK: A(0)=value; flags_logic_l(value); break;
    case MENU_SETUP_COCKPIT_READ: SET_W(D(0),value); flags_logic_w(value); break;
    case MENU_SETUP_MESSAGE_READ: SET_W(D(1),value); flags_logic_w(value); break;
    case MENU_SETUP_COCKPIT_HELD: SET_W(D(0),value); flags_logic_w(value); break;
    case MENU_SETUP_MESSAGE_MASK:
        SET_W(D(1),value); flags_logic_w(value); wr_u16(A(6)+10,value); flags_logic_w(value); break;
    case MENU_SETUP_MESSAGE_COMPARE: step_compare_word(address,value); break;
    case MENU_SETUP_MESSAGE_STATE: SET_W(D(0),value); flags_logic_w(value); break;
    case MENU_SETUP_SELECTOR_BASE: A(0)=address; A(1)=MENU_SELECTOR_DIRECTORY; break;
    case MENU_SETUP_SELECTOR_MODE:
        full_d0(value); step_subtract_word(&D(0),3); step_add_word(&D(0),D(0)); A(2)=A(0); break;
    case MENU_SETUP_SELECTOR_ROW:
        A(2)=address; full_d0(value); step_add_word(&D(0),D(0)); break;
    case MENU_SETUP_SELECTOR_CODE: SET_W(D(1),value); flags_logic_w(value); break;
    case MENU_SETUP_SELECTOR_DESTINATION:
        full_d0(value); step_add_word(&D(0),D(0)); A(2)=address; break;
    }
}
static const MenuSetupHooks hooks={consume,outputs,NULL};
static void link_frame(unsigned bytes) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=bytes; }
static int leave_frame(void) { A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C0FBE0(void) { link_frame(4); start_top_level_menu(&hooks); return leave_frame(); }
int glue_C17B96(void) { link_frame(0); select_menu_sound_pair(rd_u32(A(6)+8),&hooks); return leave_frame(); }
int glue_C1082C(void) { begin_menu_countdown(&hooks); return leave_frame(); }
int glue_C11BB0(void) { link_frame(0); filter_cockpit_message(rd_u16(A(6)+10),&hooks); return leave_frame(); }
int glue_C24FA4(void) {
    link_frame(0); queue_indexed_menu_message(rd_u32(A(6)+8),rd_u32(A(6)+12),rd_u32(A(6)+16),&hooks);
    return leave_frame();
}
