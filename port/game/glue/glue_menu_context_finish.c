#include "glue_renderer_step_math.h"
#include "glue_child_call.h"
#include "glue_menu_context_finish.h"
#include "menu_context_finish.h"
#include "globals.h"

static int32_t consume(void *context,enum MenuContextChild child) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc10bae,0xc10a32},{0xc11312,0xc10a46},{0xc11312,0xc10b04},
        {0xc10b90,0xc10b32},{0xc09192,0xc10b38},{0xc17e4a,0xc10b74},
        {0xc17cf6,0xc10b82},{0xc25070,0xc10ca2},{0xc16d04,0xc10d5a},
        {0xc33180,0xc10da2},{0xc0f4a6,0xc10e56},{0xc17f8c,0xc10e64},
        {0xc11bb0,0xc10e90},{0xc16d04,0xc10eb6},{0xc11312,0xc10f34},
        {0xc082b8,0xc10fb4},{0xc11312,0xc11a68},{0xc16d04,0xc11a8c},
        {0xc091e6,0xc0919e},{0xc53c78,0xc16d34}
    };
    int32_t result; (void)context;
    if(child==MC_NOISE || child==MC_ENGINE) { m68ki_push_32(D(0)); flags_logic_l(D(0)); }
    if(child==MC_STAGE_SOUND) {
        m68ki_push_32(D(0)); flags_logic_l(D(0)); D(0)=0x3f; flags_logic_l(D(0));
        m68ki_push_32(D(0)); flags_logic_l(D(0));
    }
    if(child==MC_STAGE_COMMAND) { m68ki_push_32(0x4021); flags_logic_l(0x4021); }
    if(child==MC_TIMER_REQUEST) m68ki_push_32(MENU_TIME_REQUEST);
    result=glue_complete_child(sites[child].entry,sites[child].ret);
    if(child==MC_NOISE || child==MC_ENGINE || child==MC_STAGE_COMMAND || child==MC_TIMER_REQUEST) A(7)+=4;
    if(child==MC_STAGE_SOUND) A(7)+=8;
    return result;
}
static void position_result(void *context,uint32_t result[3]) {
    (void)context; result[0]=D(0); result[1]=D(1); result[2]=D(2);
}
static void outputs(void *context,enum MenuContextPhase phase,uint32_t value,gaddr address) {
    uint32_t temporary; (void)context;
    switch(phase) {
    case MC_BYTE_STORE: case MC_BYTE_TEST: case MC_LOCAL_BYTE: flags_logic_b(value); break;
    case MC_WORD_STORE: case MC_WORD_TEST: flags_logic_w(value); break;
    case MC_LONG_STORE: case MC_LONG_TEST: case MC_LOCAL_CURSOR: flags_logic_l(value); break;
    case MC_D0_BYTE: SET_B(D(0),value); flags_logic_b(value); break;
    case MC_D0_WORD: SET_W(D(0),value); flags_logic_w(value); break;
    case MC_D0_LONG: D(0)=value; flags_logic_l(value); break;
    case MC_D1_LONG: D(1)=value; flags_logic_l(value); break;
    case MC_SUB_BYTE: step_subtract_byte(&D(0),(uint8_t)address); break;
    case MC_CMP_BYTE: step_compare_byte((uint8_t)address,(uint8_t)value); break;
    case MC_CMP_LONG: step_compare_long(address,value); break;
    case MC_BIT_BYTE: FLAG_Z=value&(1u<<(address&7u)); break;
    case MC_BIT_WORD: FLAG_Z=value&(1u<<(address&31u)); break;
    case MC_CALLBACK: A(0)=value; flags_logic_l(value); break;
    case MC_VIEWPORT_COMPARE: SET_B(D(0),value); SET_B(D(1),address); step_compare_byte((uint8_t)address,(uint8_t)value); break;
    case MC_WORD_OR_D0: SET_W(D(0),D(0)|value); flags_logic_w(D(0)); break;
    case MC_WORD_AND_D0: SET_W(D(0),D(0)&value); flags_logic_w(D(0)); break;
    case MC_ADD_WORD_D0: step_add_word(&D(0),(uint16_t)value); break;
    case MC_SHIFT_WORD_D0: renderer_asl_word(&D(0),value); break;
    case MC_QUEUE_WORD: A(0)=address; flags_logic_w(value); break;
    case MC_QUEUE_NEXT: temporary=value; step_add_long(&temporary,address); break;
    case MC_SUB_LONG_D1: step_subtract_long(&D(1),value); break;
    case MC_ADD_MEMORY_D1: temporary=value; step_add_long(&temporary,D(1)); break;
    case MC_ADD_MEMORY_D0: temporary=value; step_add_long(&temporary,D(0)); break;
    case MC_PRESET_INPUT:
        D(3)=0; flags_logic_l(0); D(4)=0; flags_logic_l(0);
        SET_W(D(5),0xe980); flags_logic_w(D(5)); break;
    case MC_TIMER_ZERO: A(0)=0; break;
    case MC_TABLE_POINTER: A(0)=value; break;
    }
}
static const MenuContextHooks hooks={consume,outputs,position_result,NULL};
int glue_C10A24(void) { follow_menu_smoothing(&hooks); return glue_return(); }
int glue_C10AB2(void) { queue_menu_smoothing_message(&hooks); return glue_return(); }
int glue_C10AE6(void) { restart_menu_smoothing(&hooks); return glue_return(); }
int glue_C10B1E(void) { reset_menu_smoothing_view(&hooks); return glue_return(); }
int glue_C10C08(void) { begin_menu_context(&hooks); return glue_return(); }
int glue_C10CFE(void) { finish_menu_context_message(&hooks); return glue_return(); }
int glue_C10D8A(void) { expire_menu_context(&hooks); return glue_return(); }
int glue_C11A26(void) { queue_menu_viewport_message(&hooks); return glue_return(); }
int glue_C11A50(void) { finish_menu_viewport_message(&hooks); return glue_return(); }
int glue_C09192(void) { load_menu_position_preset(&hooks); return glue_return(); }
int glue_C16D04(void) { read_menu_time_sample(&hooks); return glue_return(); }
int glue_C10C68(void) {
    m68ki_push_32(A(6)); A(6)=A(7); A(7)-=6; queue_menu_context_command(A(6),&hooks);
    A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return();
}
int glue_C10DAE(void) {
    m68ki_push_32(A(6)); A(6)=A(7); A(7)-=2; update_menu_context(A(6),&hooks);
    A(7)=A(6); A(6)=m68ki_pull_32(); return glue_return();
}
