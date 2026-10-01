#ifndef FA18_GLUE_UNSIGNED_DIVISION_STEP_H
#define FA18_GLUE_UNSIGNED_DIVISION_STEP_H
/* Source-width unsigned 68000 division, including overflow and operand-
 * dependent cycles. Shared by map projection, date fields and aim math. */
#include "glue_step.h"

static inline void step_divide_unsigned(uint32_t *reg, uint16_t divisor) {
    uint32_t dividend = *reg, quotient, remainder, shifting, high;
    int cycles = 38, i;
    if (!divisor) { m68ki_exception_trap(EXCEPTION_ZERO_DIVIDE); return; }
    if ((dividend >> 16) >= divisor) {
        USE_CYCLES(10 - 140); FLAG_V = VFLAG_SET; return;
    }
    shifting = dividend; high = (uint32_t)divisor << 16;
    for (i = 0; i < 15; ++i) {
        uint32_t before = shifting; shifting <<= 1;
        if ((int32_t)before < 0) shifting -= high;
        else {
            cycles += 2;
            if (shifting >= high) { shifting -= high; --cycles; }
        }
    }
    USE_CYCLES(cycles * 2 - 140);
    quotient = dividend / divisor; remainder = dividend % divisor;
    *reg = (remainder << 16) | quotient;
    FLAG_N = NFLAG_16(quotient); FLAG_Z = quotient; FLAG_V = FLAG_C = 0;
}
#endif
