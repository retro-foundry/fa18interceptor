#ifndef FA18_GLUE_HUD_PARENTS_MATH_H
#define FA18_GLUE_HUD_PARENTS_MATH_H
#include "glue_projection_readouts_math.h"
#include "glue_menu_context_math.h"
/* Only this family adds byte arithmetic shift and word rotation recipes. */
static void hud_parent_asr_byte(uint32_t *reg,unsigned count) {
    uint8_t old=(uint8_t)*reg,result;
    count&=63u;
    result=(uint8_t)((int8_t)old>>(count<8?count:7));
    SET_B(*reg,result); flags_logic_b(result);
    if(count) FLAG_X=FLAG_C=(count<=8?((old>>(count-1))&1u):(old>>7))<<8;
    USE_CYCLES(count<<CYC_SHIFT);
}
static void hud_parent_ror_word(uint32_t *reg,unsigned count) {
    uint16_t old=(uint16_t)*reg,result; unsigned rotation;
    count&=63u; rotation=count&15u;
    result=rotation?(uint16_t)((old>>rotation)|(old<<(16-rotation))):old;
    SET_W(*reg,result); flags_logic_w(result);
    if(count) FLAG_C=(result>>15)<<8;
    USE_CYCLES(count<<CYC_SHIFT);
}
#endif
