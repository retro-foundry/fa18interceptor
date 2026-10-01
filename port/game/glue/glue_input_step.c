/* Source-timed mouse-button read ($C1715C).
 * player_input.c owns the readable input operation. This bridge retains the
 * CIA-copy and POTINP read positions in the caller's instruction timeline. */
#include "glue_step.h"

static int joystick_instruction(void) {
    uint32_t pc = REG_PC, address, value, bit;
    uint16_t opcode = step_begin(pc);
    switch (pc) {
    case 0xC16F1C:
        step_predecrement_long(A(6)); A(6) = A(7);
        A(7) += (int16_t)m68ki_read_imm_16(); break;
    case 0xC16F20:
        value = m68ki_read_imm_32(); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC16F28: A(0) = m68k_read_memory_32(step_displacement(A(6))); break;
    case 0xC16F2C: value = m68k_read_memory_16(A(0)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC16F2E:
        address = m68ki_read_imm_32(); step_write_word(address, D(0)); flags_logic_w(D(0)); break;
    case 0xC16F34: step_write_word(step_displacement(A(6)), D(0)); flags_logic_w(D(0)); break;
    case 0xC16F38: D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC16F3A: case 0xC16F6C: step_write_long(step_displacement(A(7)), D(0)); flags_logic_l(D(0)); break;
    case 0xC16F3E: value = m68ki_read_imm_32(); D(0) &= value; flags_logic_l(D(0)); break;
    case 0xC16F44: case 0xC16F76: step_asr_long(&D(0), 1); break;
    case 0xC16F46: case 0xC16F78: D(1) = m68k_read_memory_32(step_displacement(A(7))); flags_logic_l(D(1)); break;
    case 0xC16F4A: case 0xC16F7C: value = m68ki_read_imm_32(); D(1) &= value; flags_logic_l(D(1)); break;
    case 0xC16F50: case 0xC16F82: D(0) ^= D(1); flags_logic_l(D(0)); break;
    case 0xC16F52: case 0xC16F84: flags_logic_l(D(0)); break;
    case 0xC16F54: case 0xC16F86: case 0xC16F9E: case 0xC16FB2: case 0xC16FCA: case 0xC16FE2:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC16F56: case 0xC16F88: case 0xC16FA0: case 0xC16FB4: case 0xC16FCC: case 0xC16FE4:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC16F5C: case 0xC16F8E: case 0xC16FBA: case 0xC16FD2:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC16F64: case 0xC16F96: case 0xC16FC2: case 0xC16FDA:
        step_branch(pc, opcode, 1); break;
    case 0xC16F66: D(0) = (D(0) & 0xFFFF0000u) | m68k_read_memory_16(step_displacement(A(6))); flags_logic_w(D(0)); break;
    case 0xC16F6A: D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC16F70: value = m68ki_read_imm_32(); D(0) &= value; flags_logic_l(D(0)); break;
    case 0xC16F98: flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC16FA6: case 0xC16FEA:
        address = m68ki_read_imm_32(); m68k_write_memory_8(address, 0); flags_logic_b(0); break;
    case 0xC16FAC: case 0xC16FC4:
        bit = m68ki_read_imm_16() & 7u;
        FLAG_Z = m68k_read_memory_8(step_displacement(A(6))) & (1u << bit); break;
    case 0xC16FDC: flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC16FF0: A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC16FF2: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}

int glue_C16F1C_step(void) {
    return REG_PC >= 0xC16F1C && REG_PC < 0xC16FF4 ? joystick_instruction() : 0;
}

int glue_C1715C_step(void) {
    uint32_t pc = REG_PC, value, bit;
    uint16_t opcode;
    if (pc < 0xC1715Cu || pc >= 0xC1718Eu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC1715C:
        value = A(6); m68ki_push_32(value); A(6) = A(7);
        A(7) += (int16_t)m68ki_read_imm_16(); break;
    case 0xC17160: step_save_registers(); break;
    case 0xC17164: D(7) = 0; flags_logic_l(0); break;
    case 0xC17166:
        value = m68k_read_memory_8(m68ki_read_imm_32());
        SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC1716C: case 0xC1717A:
        bit = m68ki_read_imm_16() & 31u;
        FLAG_Z = D(0) & (1u << bit); break;
    case 0xC17170: case 0xC1717E: step_branch(pc, opcode, COND_NE()); break;
    case 0xC17172: D(7) = 1; flags_logic_l(D(7)); break;
    case 0xC17174:
        value = m68k_read_memory_16(m68ki_read_imm_32());
        SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC17180:
        bit = m68ki_read_imm_16() & 31u;
        FLAG_Z = D(7) & (1u << bit); D(7) |= 1u << bit;
        if (bit < 16) USE_CYCLES(-2); break;
    case 0xC17184: D(0) = D(7); flags_logic_l(D(0)); break;
    case 0xC17186: step_restore_registers(); break;
    case 0xC1718A: A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC1718C: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
