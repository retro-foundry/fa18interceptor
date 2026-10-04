#ifndef FA18_GLUE_CONTROL_READOUTS_MATH_H
#define FA18_GLUE_CONTROL_READOUTS_MATH_H
#include "glue_render_leaf_helpers_math.h"
/* Original C52EE2 rotates the partial remainder through the dividend carry. */
static void readout_roxl_long(uint32_t *reg,unsigned count){
 unsigned n=count&63u,i,extend=FLAG_X?1u:0u;
 for(i=0;i<n;++i){unsigned next=*reg>>31;*reg=(*reg<<1)|extend;extend=next;}
 flags_logic_l(*reg);FLAG_C=extend?CFLAG_SET:0;
 if(n)FLAG_X=extend?XFLAG_SET:0;
 USE_CYCLES(n<<CYC_SHIFT);
}
#endif
