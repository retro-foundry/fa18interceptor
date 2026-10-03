#ifndef FA18_GLUE_FLIGHT_MOTION_HELPERS_MATH_H
#define FA18_GLUE_FLIGHT_MOTION_HELPERS_MATH_H
#include "glue_flight_record_actions_math.h"
static void motion_lsr_long(uint32_t *reg,unsigned count) {
    uint32_t old=*reg; count&=63u; *reg=count<32?old>>count:0;
    flags_logic_l(*reg); FLAG_C=0;
    if(count) FLAG_X=FLAG_C=count<=32?((old>>(count-1))&1u)<<8:0;
    USE_CYCLES(count<<CYC_SHIFT);
}
#endif
