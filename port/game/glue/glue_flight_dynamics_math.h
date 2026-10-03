#ifndef FA18_GLUE_FLIGHT_DYNAMICS_MATH_H
#define FA18_GLUE_FLIGHT_DYNAMICS_MATH_H
#include "glue_flight_motion_helpers_math.h"
static void dynamics_rol_long(uint32_t *reg,unsigned count) {
    uint32_t old=*reg; unsigned shift; count&=63u; shift=count&31u;
    *reg=shift?(old<<shift)|(old>>(32-shift)):old; flags_logic_l(*reg);
    FLAG_C=count?(*reg&1u)<<8:0; USE_CYCLES(count<<CYC_SHIFT);
}
#endif
