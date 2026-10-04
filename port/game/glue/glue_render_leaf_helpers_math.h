#ifndef FA18_GLUE_RENDER_LEAF_HELPERS_MATH_H
#define FA18_GLUE_RENDER_LEAF_HELPERS_MATH_H
#include "glue_hud_render_parents_math.h"
static void render_leaf_rol_word(uint32_t *reg,unsigned count) {
 unsigned i,n=count&63u;uint16_t b=(uint16_t)*reg;
 for(i=0;i<n;++i)b=(uint16_t)((b<<1)|(b>>15));
 SET_W(*reg,b);flags_logic_w(b);if(n)FLAG_C=(b&1)?CFLAG_SET:0;USE_CYCLES(2*n);
}
static void render_leaf_ror_long(uint32_t *reg,unsigned count) {
 unsigned i,n=count&63u;uint32_t b=*reg;
 for(i=0;i<n;++i)b=(b>>1)|(b<<31);
 *reg=b;flags_logic_l(b);if(n)FLAG_C=(b&0x80000000u)?CFLAG_SET:0;USE_CYCLES(2*n);
}
#endif
