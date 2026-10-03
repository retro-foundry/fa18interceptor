#ifndef FA18_GLUE_MENU_CONTEXT_MATH_H
#define FA18_GLUE_MENU_CONTEXT_MATH_H
#include "glue_renderer_step_math.h"
/* Family-local CPU outputs for the heading formatter's original operations. */
static void menu_lsl_word(uint32_t *reg,unsigned count) {
    uint16_t old=(uint16_t)*reg,result; count&=63u;
    result=count<16?(uint16_t)(old<<count):0; SET_W(*reg,result); flags_logic_w(result); FLAG_C=0;
    if(count) FLAG_X=FLAG_C=count<=16?((old>>(16-count))&1u)<<8:0;
    USE_CYCLES(count<<CYC_SHIFT);
}
static void menu_lsr_byte(uint32_t *reg,unsigned count) {
    uint8_t old=(uint8_t)*reg,result; count&=63u;
    result=count<8?(uint8_t)(old>>count):0; SET_B(*reg,result); flags_logic_b(result); FLAG_C=0;
    if(count) FLAG_X=FLAG_C=count<=8?((old>>(count-1))&1u)<<8:0;
    USE_CYCLES(count<<CYC_SHIFT);
}
static uint8_t menu_decimal_flags(uint8_t source,uint8_t destination) {
    uint32_t result=(source&15u)+(destination&15u)+XFLAG_AS_1();
    FLAG_V=~result;
    if(result>9) result+=6;
    result+=(source&0xf0u)+(destination&0xf0u); FLAG_X=FLAG_C=(result>0x99u)<<8;
    if(FLAG_C) result-=0xa0u;
    FLAG_V&=result; FLAG_N=NFLAG_8(result); result&=0xffu; FLAG_Z|=result;
    return (uint8_t)result;
}
#endif
