#include "glue_renderer_step_math.h"
#include "glue_unsigned_division_step.h"
#include "glue_child_call.h"
#include "glue_menu_transition.h"
#include "menu_transition.h"
#include "globals.h"

static void push(uint32_t value) { step_predecrement_long(value); flags_logic_l(value); }
static void full_d0(uint32_t value) { D(0)=value; flags_logic_l(value); }
/* C24E8A falls through into the existing C24F76 field body. Preserve that
 * original stack, rather than adding a return address for a fictitious call. */
static void final_field(void) {
    wr_u32(DISPLAY_VALUE,D(0)); flags_logic_l(D(0));
    glue_complete_child(0xc25a08u,0xc24f80u);
    D(1)=rd_u32(DISPLAY_VALUE_BCD); flags_logic_l(D(1));
    A(0)+=(uint32_t)(int32_t)(int16_t)D(3); D(0)=A(0); flags_logic_l(D(0));
    step_predecrement_long(D(2)); step_predecrement_long(D(1)); step_predecrement_long(D(0));
    glue_complete_child(0xc0f56au,0xc24f94u); A(7)+=12;
    full_d0(0); A(1)=rd_u32(MODE_TABLE);
}
static MenuTransitionResult consume(void *context,enum MenuTransitionCall call,uint32_t value) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc11312,0xc0fce6},{0xc11312,0xc0fd0a},{0xc2fd22,0xc0fd10},
        {0xc11312,0xc0fdde},{0xc2fd22,0xc0fde4},{0xc2fd22,0xc0fe02},{0xc24e8a,0xc0fe08},
        {0xc17b08,0xc0fef8},{0xc17b08,0xc0ff04},{0xc17c2a,0xc0ff2a},
        {0xc17e4a,0xc0ff42},{0xc17cf6,0xc0ff56},
        {0xc11312,0xc0ff64},{0xc28722,0xc0ff82},{0xc11b0e,0xc0ffa8},
        {0xc09120,0xc1002a},{0xc29490,0xc10056},{0xc10b90,0xc100de},
        {0xc2949a,0xc100e4},{0xc09148,0xc100ea},{0xc28722,0xc100f0},
        {0xc28722,0xc1011e},{0xc0924a,0xc10124},{0xc11312,0xc10128},
        {0xc082b0,0xc1014a},{0xc1c860,0xc1017a},
        {0xc17b2c,0xc17c46},{0xc17b2c,0xc17c5a},
        {0xc24f76,0xc24ea4},{0xc24f76,0xc24eb6},{0xc24f76,0xc24ec8},
        {0xc24f76,0xc24eda},{0xc24f76,0xc24eec},{0xc24f76,0xc24efe},
        {0xc24f76,0xc24f10},{0xc24f76,0xc24f22},{0xc24f76,0xc24f3c},
        {0xc24f76,0xc24f56},{0xc24f76,0xc24f68}
    };
    MenuTransitionResult result;
    unsigned arguments=0;
    (void)context;
    switch(call) {
    case MENU_STOP_ZERO: push(0); arguments=4; break;
    case MENU_STOP_ONE: case MENU_SOUND_PAIR: case MENU_NOISE:
        full_d0(value); push(value); arguments=4; break;
    case MENU_SCRIPTED_NOISE: full_d0(value); push(value); push(0x300); arguments=8; break;
    case MENU_PAIR_FIRST:
        push(rd_u32(A(6)+8)); push(0); full_d0(0x23); push(D(0)); arguments=12; break;
    case MENU_PAIR_SECOND:
        push(rd_u32(A(6)+8)); full_d0(1); push(D(0)); full_d0(0x24); push(D(0)); arguments=12; break;
    case MENU_FIELD_LAST: final_field(); result.value=D(0); result.record=A(1); return result;
    default: break;
    }
    glue_complete_child(sites[call].entry,sites[call].ret); A(7)+=arguments;
    result.value=D(0); result.record=A(1); return result;
}
static void outputs(void *context,enum MenuTransitionPhase phase,uint32_t value,uint32_t extra,gaddr address) {
    (void)context;
    switch(phase) {
    case MENU_BYTE_TEST: case MENU_BYTE_STORE: flags_logic_b(value); break;
    case MENU_WORD_TEST: case MENU_WORD_STORE: flags_logic_w(value); break;
    case MENU_LONG_STORE: flags_logic_l(value); break;
    case MENU_BYTE_D0: SET_B(D(0),value); flags_logic_b(value); break;
    case MENU_WORD_D0: SET_W(D(0),value); flags_logic_w(value); break;
    case MENU_LONG_D0: case MENU_FULL_D0: case MENU_EXTEND_TIME: full_d0(value); break;
    case MENU_FULL_D1: D(1)=value; flags_logic_l(value); break;
    case MENU_COMPARE_BYTE: step_compare_byte(extra,value); break;
    case MENU_COMPARE_WORD: step_compare_word(extra,value); break;
    case MENU_BIT_TEST: FLAG_Z=value&(1u<<extra); break;
    case MENU_FOLLOW_LOCALS:
        wr_u32(A(6)-4,address); flags_logic_l(address);
        wr_u8(A(6)-5,value); flags_logic_b(value); break;
    case MENU_DELAY_LOCALS:
        SET_B(D(0),value); flags_logic_b(value); SET_W(D(0),(int16_t)(int8_t)value); flags_logic_w(D(0));
        SET_W(D(1),extra); flags_logic_w(extra); wr_u16(A(6)-2,D(0)); flags_logic_w(D(0));
        flags_logic_w(D(1)); break;
    case MENU_PHASE_SUBTRACT: SET_W(D(0),value); flags_logic_w(value); step_subtract_word(&D(0),6); break;
    case MENU_QUEUE_WORD: A(0)=address; flags_logic_w(value); break;
    case MENU_QUEUE_ADVANCE:
        extra=rd_u32(A(6)-4); step_add_long(&extra,2); wr_u32(A(6)-4,extra); break;
    case MENU_CALLBACK: A(0)=value; flags_logic_l(value); break;
    case MENU_TABLE_BEGIN:
        full_d0(0); SET_W(D(0),value); flags_logic_w(value); D(1)=0x30; flags_logic_l(D(1)); break;
    case MENU_TABLE_NEXT: step_subtract_long(&D(1),8); break;
    case MENU_TABLE_COMPARE: step_compare_long(extra,D(0)); break;
    case MENU_POSE_SUBTRACT: step_subtract_byte(&D(0),3); break;
    case MENU_REQUEST_WORD:
        SET_W(D(0),value); flags_logic_w(value); SET_W(D(0),D(0)|0x100u); flags_logic_w(D(0)); break;
    case MENU_FORMAT_BEGIN: full_d0(0); A(1)=address; break;
    case MENU_FORMAT_FIELD:
        D(0)=value; A(0)=address; D(3)=extra>>8; flags_logic_l(D(3));
        D(2)=extra&255u; flags_logic_l(D(2)); break;
    case MENU_DIVIDE: step_divide_unsigned(&D(0),(uint16_t)value); break;
    case MENU_SAVE_TIME: push(value); break;
    case MENU_RESTORE_TIME: full_d0(m68ki_pull_32()); break;
    case MENU_SWAP_TIME: step_swap(&D(0)); break;
    }
}
static const MenuTransitionHooks hooks={consume,outputs,NULL};
static void link_frame(int bytes) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=(unsigned)bytes; }
static int leave_frame(void) { A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C0FCB4(void) { link_frame(6); follow_top_level_menu(&hooks); return leave_frame(); }
int glue_C0FECE(void) { link_frame(2); advance_delayed_menu(&hooks); return leave_frame(); }
/* These table-arm entries belong to the already active enclosing LINK frame. */
int glue_C0FFE2(void) { enter_menu_mode_nine(&hooks); return leave_frame(); }
int glue_C1000A(void) { enter_menu_demonstration(&hooks); return leave_frame(); }
int glue_C17C2A(void) { link_frame(0); start_menu_alert_pair(rd_u32(A(6)+8),&hooks); return leave_frame(); }
int glue_C24E8A(void) { format_menu_summary(&hooks); return glue_return(); }
