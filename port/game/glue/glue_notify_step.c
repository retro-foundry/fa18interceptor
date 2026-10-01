/* Source-timed notification cadence ($C11B44).
 * notify.c owns the readable state transition. This bridge preserves its
 * countdown/code writes and each branch boundary in the main update. */
#include "glue_step.h"

int glue_C11B44_step(void) {
    uint32_t pc = REG_PC, address, value;
    uint16_t opcode;
    if (pc < 0xC11B44u || pc >= 0xC11BB0u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC11B44: case 0xC11B68: case 0xC11B7C: case 0xC11B94:
        value = m68k_read_memory_8(m68ki_read_imm_32());
        SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC11B4A: step_subtract_byte(&D(0), 1); break;
    case 0xC11B6E: step_subtract_byte(&D(0), 4); break;
    case 0xC11B86: step_subtract_byte(&D(0), 2); break;
    case 0xC11B4C:
        m68k_write_memory_8(m68ki_read_imm_32(), D(0)); flags_logic_b(D(0)); break;
    case 0xC11B52: flags_logic_b(D(0)); break;
    case 0xC11B54: step_branch(pc, opcode, COND_GT()); break;
    case 0xC11B70: case 0xC11B88: step_branch(pc, opcode, COND_NE()); break;
    case 0xC11B9E: step_branch(pc, opcode, COND_LE()); break;
    case 0xC11B66: case 0xC11B7A: case 0xC11B92: step_branch(pc, opcode, 1); break;
    case 0xC11B56: case 0xC11B5E: case 0xC11B72: case 0xC11B8A: case 0xC11BA0:
        value = m68ki_read_imm_16() & 0xFFu; address = m68ki_read_imm_32();
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC11B82:
        value = m68ki_read_imm_16() & 0xFFu;
        SET_B(D(0), D(0) & value); flags_logic_b(D(0)); break;
    case 0xC11B9A:
        value = m68ki_read_imm_16() & 0xFFu; step_compare_byte(value, D(0)); break;
    case 0xC11BA8:
        address = m68ki_read_imm_32(); m68k_write_memory_8(address, 0); flags_logic_b(0); break;
    case 0xC11BAE: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
