#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_menu_return.h"
#include "menu_return.h"
static void consume(void *context,enum MenuReturnChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc11312,0xc0fbaa},{0xc11312,0xc0fbd4},{0xc10bae,0xc1090e},
        {0xc11312,0xc10926},{0xc10bae,0xc1097e},{0xc10bae,0xc10950},
        {0xc10bae,0xc109ba},{0xc10362,0xc10318},{0xc11312,0xc10388},
        {0xc10b90,0xc10bb8},{0xc11312,0xc10bca}
    };
    (void)context; glue_complete_child(sites[child].entry,sites[child].ret);
}
static void outputs(void *context,enum MenuReturnPhase phase,uint32_t value,gaddr address) {
    (void)context;
    switch(phase) {
    case MR_BYTE_TEST: case MR_BYTE_STORE: flags_logic_b(value); break;
    case MR_WORD_STORE: flags_logic_w(value); break;
    case MR_BYTE_D0: SET_B(D(0),value); flags_logic_b(value); break;
    case MR_WORD_D0: SET_W(D(0),value); flags_logic_w(value); break;
    case MR_FULL_D0: D(0)=value; flags_logic_l(value); break;
    case MR_CALLBACK: A(0)=value; flags_logic_l(value); break;
    case MR_SUBTRACT_BYTE: step_subtract_byte(&D(0),(uint8_t)address); break;
    case MR_COMPARE_BYTE: step_compare_byte((uint8_t)address,(uint8_t)value); break;
    case MR_COMPARE_LONG: step_compare_long(address,value); break;
    case MR_SIGNED_MODE: D(0)=(uint32_t)(int32_t)(int8_t)value; flags_logic_l(D(0)); break;
    case MR_VIEWPORT_COMPARE:
        SET_B(D(0),value); SET_B(D(1),address); step_compare_byte((uint8_t)address,(uint8_t)value); break;
    case MR_CURSOR_INIT: wr_u32(A(6)-4,value); flags_logic_l(value); break;
    case MR_QUEUE_CODE: A(0)=address; flags_logic_w(value); break;
    case MR_CURSOR_NEXT:
        { uint32_t cursor=rd_u32(A(6)-4); step_add_long(&cursor,2); wr_u32(A(6)-4,cursor); } break;
    }
}
static const MenuReturnHooks hooks={consume,outputs,NULL};
int glue_C1064C(void) { finish_menu_context_three(&hooks); return glue_return(); }
/* Original C108FE consists solely of RTS; this is its complete behavior. */
int glue_C108FE(void) { return glue_return(); }
int glue_C10900(void) { follow_menu_return_message(&hooks); return glue_return(); }
int glue_C10970(void) { follow_menu_return_context(&hooks); return glue_return(); }
int glue_C102D8(void) { begin_menu_context_ready(&hooks); return glue_return(); }
int glue_C0FB70(void) { choose_menu_exit_after_countdown(&hooks); return glue_return(); }
int glue_C0FBB6(void) { leave_menu_on_key_or_message(&hooks); return glue_return(); }
int glue_C101FC(void) { reset_menu_viewport_after_countdown(&hooks); return glue_return(); }
int glue_C10228(void) { enter_menu_mode_four(&hooks); return glue_return(); }
int glue_C10942(void) { start_menu_smoothing(&hooks); return glue_return(); }
int glue_C109AC(void) { complete_menu_return_after_countdown(&hooks); return glue_return(); }
int glue_C10302(void) {
    m68ki_push_32(A(6)); A(6)=A(7); A(7)-=4; select_menu_return_message(&hooks);
    A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return();
}
int glue_C10BAE(void) { cancel_menu_return(&hooks); return glue_return(); }
int glue_C10362(void) { leave_menu_return_on_key(&hooks); return glue_return(); }
