/* Source-timed draw-page pointer publication ($C2F558).
 * render_page.c owns the readable selector. This bridge preserves the two
 * pointer writes at their original instruction boundaries. */
#include "glue_step.h"

int glue_C2F558_step(void) {
    uint32_t pc = REG_PC, value;
    uint16_t opcode;
    if (pc < 0xC2F558u || pc >= 0xC2F582u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC2F558: A(0) = m68ki_read_imm_32(); break;
    case 0xC2F55E: A(1) = m68ki_read_imm_32(); break;
    case 0xC2F564:
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2F56A: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F56C: A(0) += (int16_t)m68ki_read_imm_16(); break;
    case 0xC2F570: A(1) += (int16_t)m68ki_read_imm_16(); break;
    case 0xC2F574: step_write_long(m68ki_read_imm_32(), A(0)); flags_logic_l(A(0)); break;
    case 0xC2F57A: step_write_long(m68ki_read_imm_32(), A(1)); flags_logic_l(A(1)); break;
    case 0xC2F580: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
