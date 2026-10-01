/* Resumable source timing for the timer-gated post-input callback ($C0FA04).
 * post_input_followup.c remains the readable operation; child calls continue
 * through the translated dispatcher at their original instruction boundary. */
#include "glue_step.h"

static void followup_jsr(uint32_t address) {
    m68ki_push_32(REG_PC);
    m68ki_jump(address);
}

static void followup_bsr(uint32_t pc) {
    int16_t displacement = (int16_t)m68ki_read_imm_16();
    m68ki_push_32(REG_PC);
    REG_PC = pc + 2 + displacement;
}

int glue_C0FA04_step(void) {
    uint32_t pc = REG_PC, address, value;
    uint16_t opcode;
    if (pc < 0xC0FA04u || pc >= 0xC0FA4Cu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC0FA04:
        value = m68k_read_memory_16(m68ki_read_imm_32());
        SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0FA0A: flags_logic_w(D(0)); break;
    case 0xC0FA0C: step_branch(pc, opcode, COND_PL()); break;
    case 0xC0FA0E: followup_bsr(pc); break;
    case 0xC0FA12: case 0xC0FA2A:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC0FA1A: D(0) = 0; flags_logic_l(D(0)); break;
    case 0xC0FA1C: case 0xC0FA32:
        address = m68ki_read_imm_32(); m68k_write_memory_8(address, D(0));
        flags_logic_b(D(0)); break;
    case 0xC0FA22:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC0FA38: A(0) = step_displacement(REG_PC); break;
    case 0xC0FA3C:
        address = m68ki_read_imm_32(); m68k_write_memory_32(address, A(0));
        flags_logic_l(A(0)); break;
    case 0xC0FA42: step_branch(pc, opcode, 1); break;
    case 0xC0FA44: followup_jsr(m68ki_read_imm_32()); break;
    case 0xC0FA4A: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
