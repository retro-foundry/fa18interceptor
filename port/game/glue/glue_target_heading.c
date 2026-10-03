#include "glue_menu_context_math.h"
#include "glue_unsigned_division_step.h"
#include "glue_child_call.h"
#include "glue_menu_context_finish.h"
#include "target_heading.h"
#include "globals.h"
static void world(void *context,gaddr record,uint32_t result[3]) {
    (void)context; (void)record; glue_complete_child(0xc091e0,0xc250b8);
    result[0]=D(0); result[1]=D(1); result[2]=D(2);
}
static void track(void *context,uint32_t x,uint32_t z) {
    unsigned i; (void)context; (void)x; (void)z;
    for(i=6;i>0;--i) m68ki_push_32(D(i-1));
    glue_complete_child(0xc123fa,0xc250de); A(7)+=24;
}
static void pack(void *context) { (void)context; glue_complete_child(0xc25a08,0xc250fa); }
static void outputs(void *context,enum MenuHeadingPhase phase,uint32_t value,gaddr address) {
    (void)context;
    switch(phase) {
    case MH_LIST_START: A(1)=CONTROL_RECORDS; A(2)=value; break;
    case MH_LIST_TEST: flags_logic_w(value); break;
    case MH_RECORD_OFFSET:
        SET_W(D(3),value); flags_logic_w(value); A(2)+=10;
        menu_lsl_word(&D(3),8); step_add_word(&D(3),(uint16_t)D(3)); break;
    case MH_TYPE: SET_B(D(1),value); flags_logic_b(value); break;
    case MH_TYPE_MASK: SET_B(D(1),value); flags_logic_b(value); break;
    case MH_TYPE_COMPARE: step_compare_byte((uint8_t)address,(uint8_t)value); break;
    case MH_RECORD_FLAG: FLAG_Z=value&0x40; break;
    case MH_RECORD_SELECTED: A(1)=value; break;
    case MH_WORLD_INPUT:
        D(3)=0; flags_logic_l(0); D(4)=0; flags_logic_l(0); SET_W(D(5),0x1f4); flags_logic_w(D(5)); break;
    case MH_DIRECTION:
        D(4)=D(2); flags_logic_l(D(4)); D(2)=D(0); flags_logic_l(D(2));
        step_subtract_long(&D(2),value); step_asr_long(&D(2),8); D(3)=0; flags_logic_l(0);
        step_subtract_long(&D(4),address); step_asr_long(&D(4),8);
        D(5)=0xffffffffu; flags_logic_l(D(5)); D(0)=0; flags_logic_l(0); D(1)=0; flags_logic_l(0); break;
    case MH_DIVIDE:
        SET_W(D(0),value); flags_logic_w(value); D(0)=(uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0));
        step_divide_unsigned(&D(0),0x50); D(0)=(uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case MH_PACKED:
        A(1)=POST_INPUT_HEADING_TEXT; D(0)=value; flags_logic_l(value);
        SET_B(D(1),value); flags_logic_b(value); A(3)=DISPLAY_VALUE_BCD+2; break;
    case MH_NIBBLE_CLEAR: flags_logic_b(value); break;
    case MH_DIGIT_MASK: SET_B(D(1),value); flags_logic_b(value); break;
    case MH_DIGIT_COMPARE: step_compare_byte((uint8_t)address,(uint8_t)value); break;
    case MH_ROUND_BEGIN:
        A(3)+=2; A(2)=POSTFLIGHT_BCD_STEP; flags_logic_l(10); renderer_add_byte(&D(1),0); break;
    case MH_DECIMAL: --A(2); --A(3); menu_decimal_flags((uint8_t)(value>>8),(uint8_t)value); break;
    case MH_ROUND_COMPARE: step_compare_long(address,value); break;
    case MH_WORD_CLEAR: flags_logic_w(0); break;
    case MH_DIGIT_LOAD: SET_B(D(1),value); flags_logic_b(value); A(3)+=address; break;
    case MH_DIGIT_SHIFT: menu_lsr_byte(&D(1),value); break;
    case MH_ASCII: renderer_add_byte(&D(1),(uint8_t)value); break;
    case MH_CHARACTER: ++A(1); flags_logic_b(value); break;
    case MH_COMPLETE: D(0)=0; flags_logic_l(0); break;
    case MH_NOT_FOUND: D(0)=0xffffffffu; flags_logic_l(D(0)); break;
    }
}
int glue_C25070(void) {
    static const MenuHeadingHooks hooks={outputs,world,track,pack,NULL};
    refresh_menu_heading(&hooks); return glue_return();
}
