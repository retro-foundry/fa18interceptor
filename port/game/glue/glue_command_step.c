/* Source instruction boundaries for indexed sound/command state updates.
 * The readable operations remain free_voice() and play_sound(); this bridge
 * preserves their original stack, register, call and Copper-trigger timing. */
#include "glue_step.h"

static void step_link_a6(void) {
    step_predecrement_long(A(6));
    A(6) = A(7);
    A(7) += (int16_t)m68ki_read_imm_16();
}

static void step_unlink_a6(void) {
    A(7) = A(6);
    A(6) = m68k_read_memory_32(A(7));
    A(7) += 4;
}

static int command_instruction(void) {
    uint32_t pc = REG_PC, address, value;
    uint16_t opcode = step_begin(pc);
    switch (pc) {
    /* $C17B08: clear one command slot and clear its Paula interrupt. */
    case 0xC17B08: step_link_a6(); break;
    case 0xC17B0C: D(0) = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(D(0)); break;
    case 0xC17B10: step_asl_long(&D(0), 2); break;
    case 0xC17B12: A(0) = D(0); break;
    case 0xC17B14: A(0) += m68ki_read_imm_32(); break;
    case 0xC17B1A: step_write_long(A(0), 0); flags_logic_l(0); break;
    case 0xC17B1C:
        value = m68k_read_memory_32(step_displacement(A(6)));
        step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17B20:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC17B26: A(7) += 4; break;
    case 0xC17B28: step_unlink_a6(); break;
    case 0xC17B2A: m68ki_jump(m68ki_pull_32()); break;

    /* $C17B2C: publish a sound record and trigger its selected voice. */
    case 0xC17B2C: step_link_a6(); break;
    case 0xC17B30: step_save_registers(); break;
    case 0xC17B34: case 0xC17B4E:
        D(0) = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(D(0)); break;
    case 0xC17B38: case 0xC17B52: step_asl_long(&D(0), 2); break;
    case 0xC17B3A: case 0xC17B54: A(0) = D(0); break;
    case 0xC17B3C: case 0xC17B56: A(0) += m68ki_read_imm_32(); break;
    case 0xC17B42: flags_logic_l(m68k_read_memory_32(A(0))); break;
    case 0xC17B44: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17B46: case 0xC17B82:
        value = m68k_read_memory_32(step_displacement(A(6)));
        step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17B4A:
        address = pc + 2 + (int8_t)opcode;
        m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC17B4C: case 0xC17B8C: A(7) += 4; break;
    case 0xC17B5C: A(1) = m68k_read_memory_32(A(0)); break;
    case 0xC17B5E: D(1) = 0x10; flags_logic_l(D(1)); break;
    case 0xC17B60: D(2) = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(D(2)); break;
    case 0xC17B64: step_asl_long(&D(2), D(1)); break;
    case 0xC17B66: step_write_long(step_displacement(A(1)), D(2)); flags_logic_l(D(2)); break;
    case 0xC17B6A: D(1) = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(D(1)); break;
    case 0xC17B6E: step_asl_long(&D(1), 2); break;
    case 0xC17B70: A(0) = D(1); break;
    case 0xC17B72: A(0) += m68ki_read_imm_32(); break;
    case 0xC17B78: A(1) = D(0); break;
    case 0xC17B7A: A(1) += m68ki_read_imm_32(); break;
    case 0xC17B80:
        value = m68k_read_memory_32(A(1)); step_write_long(A(0), value); flags_logic_l(value); break;
    case 0xC17B86:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC17B8E: step_restore_registers(); break;
    case 0xC17B92: step_unlink_a6(); break;
    case 0xC17B94: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}

static int programmed_sound_instruction(void) {
    uint32_t pc = REG_PC, address, value;
    uint16_t opcode = step_begin(pc);
    switch (pc) {
    case 0xC17EF2: step_link_a6(); break;
    case 0xC17EF6: flags_logic_l(m68k_read_memory_32(m68ki_read_imm_32())); break;
    case 0xC17EFC: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17F00: case 0xC17F1A: case 0xC17F6A: case 0xC17F70:
    case 0xC17F78: case 0xC17F7C:
        D(0) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(0)); break;
    case 0xC17F02: case 0xC17F7A: case 0xC17F7E:
        step_predecrement_long(D(0)); flags_logic_l(D(0)); break;
    case 0xC17F04: case 0xC17F80:
        address = pc + 2;
        if (opcode & 0xFFu) address += (int8_t)opcode;
        else address += (int16_t)m68ki_read_imm_16();
        m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC17F08: A(7) += 4; break;
    case 0xC17F0A: case 0xC17F12: case 0xC17F34: case 0xC17F54: case 0xC17F5C:
        value = m68k_read_memory_32(step_displacement(A(6)));
        step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC17F1C: case 0xC17F28: case 0xC17F3C: case 0xC17F48:
        D(1) = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(D(1)); break;
    case 0xC17F20: case 0xC17F2C: case 0xC17F40: case 0xC17F4C:
        step_asl_long(&D(1), D(0)); break;
    case 0xC17F22: case 0xC17F2E: case 0xC17F42: case 0xC17F4E:
        step_write_long(m68ki_read_imm_32(), D(1)); flags_logic_l(D(1)); break;
    case 0xC17F64: A(0) = m68k_read_memory_32(m68ki_read_imm_32()); break;
    case 0xC17F6C: case 0xC17F72:
        step_write_long(step_displacement(A(0)), D(0)); flags_logic_l(D(0)); break;
    case 0xC17F76:
        A(7) -= 4; step_write_long(A(7), 0); flags_logic_l(0); break;
    case 0xC17F84: A(7) = step_displacement(A(7)); break;
    case 0xC17F88: step_unlink_a6(); break;
    case 0xC17F8A: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}

static int tone_route_instruction(void) {
    uint32_t pc = REG_PC, address, value;
    uint16_t opcode = step_begin(pc);
    switch (pc) {
    case 0xC3316A: case 0xC33180: case 0xC33182: case 0xC3317C:
    case 0xC33196: case 0xC33198: case 0xC331A6: case 0xC331B2:
        value = (uint32_t)(int32_t)(int8_t)opcode;
        if ((opcode >> 9 & 7u) == 0) D(0) = value;
        else if ((opcode >> 9 & 7u) == 1) D(1) = value;
        else if ((opcode >> 9 & 7u) == 2) D(2) = value;
        else D(6) = value;
        flags_logic_l(value); break;
    case 0xC3316C: case 0xC3317E: case 0xC33184:
        step_branch(pc, opcode, 1); break;
    case 0xC3316E: case 0xC33186: case 0xC3318E: case 0xC3319A:
        flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC33174: case 0xC3318C: case 0xC33194:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC331A0: step_branch(pc, opcode, COND_GT()); break;
    case 0xC33176: case 0xC331A2: case 0xC331BA: step_save_registers(); break;
    case 0xC3317A: D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC331A8: D(7) = D(0); flags_logic_l(D(7)); break;
    case 0xC331AA: A(0) = D(7); break;
    case 0xC331AC: D(0) = m68ki_read_imm_32(); flags_logic_l(D(0)); break;
    case 0xC331B4: D(3) = D(0); flags_logic_l(D(3)); break;
    case 0xC331B6: D(4) = D(1); flags_logic_l(D(4)); break;
    case 0xC331B8: D(5) = D(2); flags_logic_l(D(5)); break;
    case 0xC331BE:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC331C4: A(7) += (int16_t)m68ki_read_imm_16(); break;
    case 0xC331C8: step_restore_registers(); break;
    case 0xC331CC: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}

int glue_C17B08_step(void) { return REG_PC >= 0xC17B08 && REG_PC < 0xC17B2C ? command_instruction() : 0; }
int glue_C17B2C_step(void) { return REG_PC >= 0xC17B08 && REG_PC < 0xC17B96 ? command_instruction() : 0; }
int glue_C17EF2_step(void) { return REG_PC >= 0xC17EF2 && REG_PC < 0xC17F8C ? programmed_sound_instruction() : 0; }
int glue_C3316A_step(void) { return REG_PC >= 0xC3316A && REG_PC < 0xC331CE ? tone_route_instruction() : 0; }
int glue_C3316E_step(void) { return REG_PC >= 0xC3316E && REG_PC < 0xC331CE ? tone_route_instruction() : 0; }
int glue_C33180_step(void) { return REG_PC >= 0xC33180 && REG_PC < 0xC331CE ? tone_route_instruction() : 0; }
int glue_C3318E_step(void) { return REG_PC >= 0xC33180 && REG_PC < 0xC331CE ? tone_route_instruction() : 0; }
int glue_C33186_step(void) { return REG_PC >= 0xC33180 && REG_PC < 0xC331CE ? tone_route_instruction() : 0; }
