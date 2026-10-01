/* Source-timed polygon lane blits ($C30466 and $C304B2).  The readable
 * renderer owns the operation; these steps retain BBUSY polling and the
 * exact custom-register publication order. */
#include "glue_step.h"

static int polygon_lane_instruction(void) {
    uint32_t pc = REG_PC, address, value;
    uint16_t opcode = step_begin(pc), word;
    switch (pc) {
    case 0xC30466:
        address = m68ki_read_imm_32(); word = m68k_read_memory_16(address);
        word = step_lsr_word_value(word, 1); USE_CYCLES(-(1 << CYC_SHIFT));
        m68k_write_memory_16(address, word); break;
    case 0xC3046C: A(2) = m68k_read_memory_32(m68ki_read_imm_32()); break;
    case 0xC30472: D(2) = m68k_read_memory_32(step_indexed(A(2))); flags_logic_l(D(2)); break;
    case 0xC30476: case 0xC304B2:
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC3047C: D(1) = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(D(1)); break;
    case 0xC30482: step_add_long(&D(1), D(2)); break;
    case 0xC30484: case 0xC304B8:
        D(2) = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(D(2)); break;
    case 0xC3048A: case 0xC304C0: A(0) = m68ki_read_imm_32(); break;
    case 0xC304BE: D(1) = D(2); flags_logic_l(D(1)); break;
    case 0xC30490: case 0xC304C6:
        value = m68ki_read_imm_16(); address = step_displacement(A(0));
        FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC30496: case 0xC304CC: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC30498: case 0xC3049A: case 0xC304CE: case 0xC304D0: break;
    case 0xC3049C: case 0xC304B0: case 0xC304D2: case 0xC304DA:
        step_branch(pc, opcode, 1); break;
    case 0xC3049E:
        value = m68ki_read_imm_16() & 31u; FLAG_Z = D(4) & (1u << value); break;
    case 0xC304A2: step_branch(pc, opcode, COND_NE()); break;
    case 0xC304A4:
        value = m68ki_read_imm_16() & 31u; FLAG_Z = D(3) & (1u << value); break;
    case 0xC304A8: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC304AA: case 0xC304D4: case 0xC304DC:
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC304E2:
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC304E8: step_write_long(step_displacement(A(0)), D(2)); flags_logic_l(D(2)); break;
    case 0xC304EC: case 0xC304F0:
        step_write_long(step_displacement(A(0)), D(1)); flags_logic_l(D(1)); break;
    case 0xC304F4: step_write_word(step_displacement(A(0)), D(0)); flags_logic_w(D(0)); break;
    case 0xC304F8: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}

int glue_C30466_step(void) { return REG_PC >= 0xC30466 && REG_PC < 0xC304FA ? polygon_lane_instruction() : 0; }
int glue_C304B2_step(void) { return REG_PC >= 0xC304B2 && REG_PC < 0xC304FA ? polygon_lane_instruction() : 0; }
