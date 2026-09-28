#ifndef FA18_GLUE_H
#define FA18_GLUE_H

/* Glue between generated 68000-level code and the recreated C source.
 *
 * Temporary by design: a glue function exists only while some caller of its
 * routine is still generated. It is entered at the routine's first
 * instruction (return address at (A7)), converts registers to C arguments,
 * calls the game function, stores results and the register/flag effects the
 * original routine leaves behind, and returns like RTS. */

#include <stdint.h>

#include "m68kcpu.h"
#include "recomp_runtime.h"

#define D(n) REG_D[n]
#define A(n) REG_A[n]

/* Low word / byte of a data register, with the upper part preserved. */
#define SET_W(reg, v) ((reg) = ((reg) & 0xFFFF0000u) | ((uint32_t)(v) & 0xFFFFu))
#define SET_B(reg, v) ((reg) = ((reg) & 0xFFFFFF00u) | ((uint32_t)(v) & 0xFFu))

/* Condition codes as the named instruction would leave them. */
static inline void flags_logic_w(uint32_t v) {
    FLAG_N = NFLAG_16(v);
    FLAG_Z = v & 0xFFFF;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
}
static inline void flags_logic_l(uint32_t v) {
    FLAG_N = NFLAG_32(v);
    FLAG_Z = v;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
}
static inline void flags_logic_b(uint32_t v) {
    FLAG_N = NFLAG_8(v);
    FLAG_Z = v & 0xFF;
    FLAG_V = VFLAG_CLEAR;
    FLAG_C = CFLAG_CLEAR;
}

/* RTS. */
static inline int glue_return(void) {
    REG_PC = m68ki_read_32(REG_A[7]);
    REG_A[7] += 4;
    return FA18_RET;
}

#endif
