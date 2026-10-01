/* Original instruction boundaries for segment submissions and postflight HUD.
 * Domain operations remain in draw_stream.c and postflight_hud.c. */
#include "glue_step.h"

int glue_C212B0_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode;
    if (pc < 0xc212b0u || pc >= 0xc2131cu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC212B0: step_save_registers(); break;
    case 0xC212B4:
        value = m68ki_read_imm_32(); address = m68ki_read_imm_32();
        m68k_write_memory_32(address, value); flags_logic_l(value); break;
    case 0xC212BE:
        value = m68k_read_memory_16(A(2)); A(2) += 2;
        m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC212C4: case 0xC212C8:
        m68k_write_memory_16(step_displacement(A(6)), 0); flags_logic_w(0); break;
    case 0xC212CC: flags_logic_w(m68k_read_memory_16(step_displacement(A(6)))); break;
    case 0xC212D0: step_branch(pc, opcode, COND_NE()); break;
    case 0xC212D2: case 0xC212D4:
        value = m68k_read_memory_16(A(2)); A(2) += 2;
        SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC212D6: step_branch(pc, opcode, COND_GE()); break;
    case 0xC212D8:
        address = step_displacement(A(6)); value = m68k_read_memory_16(address);
        step_add_word(&value, 1); m68k_write_memory_16(address, value); break;
    case 0xC212DC:
        SET_W(D(2), D(2) & m68ki_read_imm_16()); flags_logic_w(D(2)); break;
    case 0xC212E0: case 0xC212E6: A((opcode >> 9) & 7u) = m68ki_read_imm_32(); break;
    case 0xC212EC: case 0xC212F6: A(4) = step_indexed(A(3)); break;
    case 0xC212F0: case 0xC212FA:
        value = m68k_read_memory_32(A(4)); A(4) += 4;
        m68k_write_memory_32(A(0), value); A(0) += 4; flags_logic_l(value); break;
    case 0xC212F2: SET_W(D(6), m68k_read_memory_16(A(4))); flags_logic_w(D(6)); break;
    case 0xC212F4:
        m68k_write_memory_16(A(0), D(6)); A(0) += 2; flags_logic_w(D(6)); break;
    case 0xC212FC:
        SET_W(D(6), D(6) & m68k_read_memory_16(A(4))); flags_logic_w(D(6)); break;
    case 0xC212FE: step_branch(pc, opcode, COND_LT()); break;
    case 0xC21300:
        value = m68k_read_memory_16(A(4)); m68k_write_memory_16(A(0), value); flags_logic_w(value); break;
    case 0xC21302: step_predecrement_long(A(2)); flags_logic_l(A(2)); break;
    case 0xC21304:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2130A:
        address = step_displacement(A(6)); value = m68k_read_memory_16(address) | (uint16_t)D(0);
        m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC2130E: A(2) = m68ki_pull_32(); break;
    case 0xC21310: step_branch(pc, opcode, 1); break;
    case 0xC21312: step_restore_registers(); break;
    case 0xC21316:
        SET_W(D(0), m68k_read_memory_16(step_displacement(A(6)))); flags_logic_w(D(0)); break;
    case 0xC2131A: REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C332BC_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode;
    if (pc < 0xc332b4u || pc >= 0xc332fcu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC332B4: m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC332BA: case 0xC332FA: REG_PC = m68ki_pull_32(); break;
    case 0xC332BC:
        value = m68ki_read_imm_32(); address = m68ki_read_imm_32();
        m68k_write_memory_32(address, value); flags_logic_l(value); break;
    case 0xC332C6: case 0xC332E4: flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC332CC: step_branch(pc, opcode, COND_NE()); break;
    case 0xC332EA: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC332CE: case 0xC332D2: case 0xC332D6: case 0xC332DA: case 0xC332F2: case 0xC332F6:
        value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16();
        m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC332DE: case 0xC332EC:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
