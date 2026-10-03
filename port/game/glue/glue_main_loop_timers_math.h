#ifndef FA18_GLUE_MAIN_LOOP_TIMERS_MATH_H
#define FA18_GLUE_MAIN_LOOP_TIMERS_MATH_H
#include "glue_renderer_step_math.h"
/* Family-local unsigned multiply: 68000 variable charge is two cycles per
 * set bit in the source word. The result replaces the entire destination. */
static void timer_multiply_unsigned(uint32_t *reg,uint16_t source) {
    uint16_t bits=source; unsigned count=0;
    while(bits) { count+=bits&1u; bits>>=1; }
    USE_CYCLES(2*count); *reg=(uint16_t)*reg*(uint32_t)source; flags_logic_l(*reg);
}
#endif
