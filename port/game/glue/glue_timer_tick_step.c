/* C25482: source timing for tick_timer(), called by the live timer owner.
 * Preserve the signed-byte test and conditional decrement before returning;
 * original listing: generated/recomp_002.c, C25482-C25488. */
#include "glue_cache_step_operands.h"
#include "ports_glue.h"

int glue_C25482_step(void) {
    uint32_t pc = REG_PC, value;
    uint16_t opcode = step_begin(pc);
    switch (pc) {
    case 0xC25482:
        flags_logic_b(m68k_read_memory_8(A(0))); break;
    case 0xC25484:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC25486:
        value = m68k_read_memory_8(A(0));
        step_subtract_byte(&value, 1);
        m68k_write_memory_8(A(0), value); break;
    case 0xC25488:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
