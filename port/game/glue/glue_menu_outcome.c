#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_menu_outcome.h"
#include "menu_outcome.h"
#include "globals.h"
extern int glue_origin_control_record(void);
static void full(unsigned reg,uint32_t value) { D(reg)=value; flags_logic_l(value); }
static void push(uint32_t value) { step_predecrement_long(value); flags_logic_l(value); }
static void mode(unsigned reg,uint32_t value) { full(reg,0); SET_W(D(reg),value); flags_logic_w(value); }
static void consume(void *context,enum MenuOutcomeChild child,uint32_t value) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc11312,0xc10504},{0xc24fa4,0xc10576},{0xc24fa4,0xc10590},
        {0xc29368,0xc1061a},{0xc24fa4,0xc107e6},{0xc11312,0xc108a6},
        {0xc11312,0xc105d8},{0xc11312,0xc10776}
    };
    unsigned arguments=0; (void)context; (void)value;
    switch(child) {
    case MO_DELAYED_MESSAGE_DISABLED:
        mode(1,rd_u16(A(6)-2)); push(0); full(0,1); push(D(0)); push(D(1)); arguments=12; break;
    case MO_DELAYED_MESSAGE_ENABLED:
        mode(0,rd_u16(A(6)-2)); full(1,1); push(D(1)); push(D(1)); push(D(0)); arguments=12; break;
    case MO_OUTCOME_MESSAGE:
        mode(1,rd_u16(A(6)-2)); full(0,3); push(D(0)); full(0,1); push(D(0)); push(D(1)); arguments=12; break;
    default: break;
    }
    glue_complete_child(sites[child].entry,sites[child].ret); A(7)+=arguments;
}
static void outputs(void *context,enum MenuOutcomePhase phase,uint32_t value,gaddr address) {
    (void)context;
    switch(phase) {
    case MO_BYTE_TEST: case MO_BYTE_STORE: flags_logic_b(value); break;
    case MO_WORD_STORE: flags_logic_w(value); break;
    case MO_BYTE_D0: SET_B(D(0),value); flags_logic_b(value); break;
    case MO_WORD_D0: SET_W(D(0),value); flags_logic_w(value); break;
    case MO_FULL_D0: full(0,value); break;
    case MO_CALLBACK: A(0)=value; flags_logic_l(value); break;
    case MO_LATCH: SET_B(D(0),value); SET_B(D(1),address); flags_logic_b(value); break;
    case MO_MODE_LOCAL:
        SET_B(D(0),value); flags_logic_b(value); SET_W(D(0),value); flags_logic_w(value);
        wr_u16(A(6)-2,(uint16_t)value); flags_logic_w(value); break;
    case MO_MODE_COMPARE: step_compare_word((uint16_t)address,(uint16_t)value); break;
    case MO_SELECTED_MODE:
        mode(1,address); A(0)=rd_u32(MODE_TABLE)+0x12+address; SET_B(D(2),value); flags_logic_b(value); break;
    case MO_CONTEXT_MODE:
        SET_W(D(0),value); SET_B(D(1),address); flags_logic_b(address);
        wr_u16(A(6)-2,(uint16_t)value); flags_logic_w(value); flags_logic_b(D(1)); break;
    case MO_SEARCH_BEGIN: mode(0,value); full(1,32); break;
    case MO_SEARCH_SUBTRACT: step_subtract_long(&D(1),value); break;
    case MO_SEARCH_COMPARE: step_compare_long(value,address); break;
    case MO_TABLE_READ: A(0)=address; SET_W(D(0),value); flags_logic_w(value); break;
    case MO_TABLE_ADD: step_add_word(&D(0),(uint16_t)value); break;
    case MO_TABLE_STORE: A(0)=address; break;
    case MO_ATTEMPT_SUBTRACT: case MO_MESSAGE_SUBTRACT: step_subtract_byte(&D(0),1); break;
    }
}
static const MenuOutcomeHooks hooks={consume,outputs,NULL};
static void link_frame(void) { m68ki_push_32(A(6)); A(6)=A(7); A(7)-=2; }
static int leave_frame(void) { A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return(); }
int glue_C104C2(void) {
    link_frame(); m68ki_push_32(D(2)); select_delayed_menu_message(&hooks); D(2)=m68ki_pull_32(); return leave_frame();
}
int glue_C105F4(void) { pause_menu_after_countdown(&hooks); return glue_return(); }
int glue_C1072E(void) { queue_menu_message_four(&hooks); return glue_return(); }
int glue_C1078A(void) { link_frame(); finish_menu_outcome(&hooks); return leave_frame(); }
int glue_C105A6(void) { leave_delayed_menu_message(&hooks); return glue_return(); }
int glue_C10626(void) { start_menu_context_after_countdown(&hooks); return glue_return(); }
int glue_C1075A(void) { start_menu_outcome(&hooks); return glue_return(); }
int glue_C108DA(void) { queue_menu_attempts_exhausted(&hooks); return glue_return(); }
/* C105F4's original external JSR makes this an independent callable entry.
 * Reuse the already proven complete scan domain and normal CPU observer. */
int glue_C29368(void) { return glue_origin_control_record(); }
