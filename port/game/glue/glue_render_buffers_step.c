/* Source-timed renderer work-buffer clear ($C2FD22).  render_buffers.c owns
 * the readable operation; this bridge preserves the interleaved long writes
 * and both 2,000-iteration DBRA loops. */
#include "glue_step.h"

int glue_C2FD22_step(void) {
    uint32_t pc = REG_PC, value;
    uint16_t opcode;
    unsigned reg;
    if (pc < 0xC2FD22u || pc >= 0xC2FD8Cu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC2FD22: case 0xC2FD28: case 0xC2FD2E: case 0xC2FD34: case 0xC2FD3A:
    case 0xC2FD5A: case 0xC2FD60: case 0xC2FD66: case 0xC2FD6C: case 0xC2FD72:
        reg = opcode >> 9 & 7u; A(reg) = m68k_read_memory_32(m68ki_read_imm_32()); break;
    case 0xC2FD40: case 0xC2FD78:
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2FD44: case 0xC2FD46: case 0xC2FD48: case 0xC2FD4A: case 0xC2FD54:
    case 0xC2FD7C: case 0xC2FD7E: case 0xC2FD80: case 0xC2FD82: case 0xC2FD84:
        reg = opcode & 7u; step_write_long(A(reg), 0); A(reg) += 4; flags_logic_l(0); break;
    case 0xC2FD4C: flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC2FD52: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FD56: case 0xC2FD86: step_dbf(pc, &D(0)); break;
    case 0xC2FD8A: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
