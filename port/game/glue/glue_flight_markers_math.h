#ifndef FA18_GLUE_FLIGHT_MARKERS_MATH_H
#define FA18_GLUE_FLIGHT_MARKERS_MATH_H
#include "glue_flight_dynamics_math.h"
static void marker_asl_byte(uint32_t *reg,unsigned count) {
    uint8_t old=(uint8_t)*reg,result; uint32_t mask;
    count&=63u; result=count<8?(uint8_t)(old<<count):0;
    SET_B(*reg,result); flags_logic_b(result); FLAG_C=FLAG_V=0;
    if(count) {
        FLAG_X=FLAG_C=count<=8?((old>>(8-count))&1u)<<8:0;
        if(count<8) { mask=(0xffu<<(7-count))&0xffu; old&=mask; FLAG_V=(old!=0 && old!=mask)<<7; }
        else FLAG_V=(old!=0)<<7;
    }
    USE_CYCLES(count<<CYC_SHIFT);
}
#endif
