#ifndef FA18_GLUE_HUD_RENDER_PARENTS_MATH_H
#define FA18_GLUE_HUD_RENDER_PARENTS_MATH_H
#include "glue_face_list_parents_math.h"
static void hud_render_rol_byte(uint32_t *reg,unsigned count) {
    unsigned i;uint8_t value=(uint8_t)*reg;count&=63u;
    for(i=0;i<count;++i)value=(uint8_t)((value<<1)|(value>>7));
    SET_B(*reg,value);flags_logic_b(value);FLAG_C=count?((value&1u)<<8):0;USE_CYCLES(2*count);
}
#endif
