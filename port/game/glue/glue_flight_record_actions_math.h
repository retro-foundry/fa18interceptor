#ifndef FA18_GLUE_FLIGHT_RECORD_ACTIONS_MATH_H
#define FA18_GLUE_FLIGHT_RECORD_ACTIONS_MATH_H
#include "glue_main_loop_flight_controls_math.h"
static void action_lsr_byte(uint32_t *reg,unsigned count) {
    uint8_t old=(uint8_t)*reg,result; count&=63u;
    result=count<8?(uint8_t)(old>>count):0; SET_B(*reg,result); flags_logic_b(result); FLAG_C=0;
    if(count) FLAG_X=FLAG_C=count<=8?((old>>(count-1))&1u)<<8:0;
    USE_CYCLES(count<<CYC_SHIFT);
}
#endif
