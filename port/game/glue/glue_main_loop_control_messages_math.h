#ifndef FA18_GLUE_MAIN_LOOP_CONTROL_MESSAGES_MATH_H
#define FA18_GLUE_MAIN_LOOP_CONTROL_MESSAGES_MATH_H
#include "glue_renderer_step_math.h"
static void message_lsr_word(uint32_t *reg,unsigned count) {
    uint16_t old=(uint16_t)*reg,result; count&=63u;
    result=count<16?(uint16_t)(old>>count):0; SET_W(*reg,result); flags_logic_w(result); FLAG_C=0;
    if(count) FLAG_X=FLAG_C=count<=16?((old>>(count-1))&1u)<<8:0;
    USE_CYCLES(count<<CYC_SHIFT);
}
#endif
