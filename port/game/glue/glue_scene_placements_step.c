/* Complete C1CB14/C1CB26 source timing: 99 unique source instructions.
 * Domain traversal is in scene_placements.c; runtime dispatch owns children. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"

int glue_C1CB14_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode, mask;
    unsigned mode, reg, destination, width, count;
    if (pc < 0xc1cb14u || pc >= 0xc1ccbcu) return 0;
    opcode = step_begin(pc); mode = (opcode >> 3) & 7u;
    reg = opcode & 7u; destination = (opcode >> 9) & 7u;
    switch (pc) {
    case 0xC1CB14: case 0xC1CB38: case 0xC1CB3E:
        cache_step_write(mode, reg, 1, 0); flags_logic_b(0); break;
    case 0xC1CB1A: case 0xC1CB26: case 0xC1CB2E: case 0xC1CB44:
    case 0xC1CB66: case 0xC1CB6E: case 0xC1CB8A: case 0xC1CB92:
    case 0xC1CB9C: case 0xC1CBA2: case 0xC1CBA8: case 0xC1CBAE:
    case 0xC1CBEE: case 0xC1CC06: case 0xC1CC0C: case 0xC1CC34:
    case 0xC1CC38: case 0xC1CC48: case 0xC1CC54: case 0xC1CC5C:
    case 0xC1CC60: case 0xC1CC6C: case 0xC1CC6E: case 0xC1CC70:
    case 0xC1CC76: case 0xC1CC80: case 0xC1CC9E: case 0xC1CCAA:
        width = (opcode >> 12) == 1 ? 1 : (opcode >> 12) == 3 ? 2 : 4;
        value = cache_step_read(mode, reg, width); mode = (opcode >> 6) & 7u;
        cache_step_write(mode, destination, width, value);
        if (mode != 1) cache_step_logic(value, width); break;
    case 0xC1CB54:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        step_write_word(address, value); flags_logic_w(value); break;
    case 0xC1CB24: case 0xC1CB5C: case 0xC1CB82: case 0xC1CC1A:
    case 0xC1CC96: case 0xC1CCB6:
        step_branch(pc, opcode, 1); break;
    case 0xC1CB52:
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1CB7A: case 0xC1CBBC: case 0xC1CC12: case 0xC1CC24:
    case 0xC1CC8E:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1CB98: case 0xC1CBEC: case 0xC1CC18:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1CBB2: case 0xC1CBC8: case 0xC1CC04:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1CBFC: case 0xC1CC40: case 0xC1CC46:
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1CCA6:
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1CB4A: renderer_negate(&D(reg), 4); break;
    case 0xC1CB4C: case 0xC1CBB6: case 0xC1CBBE: case 0xC1CBE6:
    case 0xC1CBF6: case 0xC1CBFE:
        value = m68ki_read_imm_32(); step_compare_long(value, cache_step_read(mode, reg, 4)); break;
    case 0xC1CB94: case 0xC1CC20:
        value = m68ki_read_imm_16(); step_compare_word(value, cache_step_read(mode, reg, 2)); break;
    case 0xC1CB5E:
        renderer_asr_word(&D(reg), destination ? destination : 8); break;
    case 0xC1CB6C:
        renderer_asl_word(&D(reg), destination ? destination : 8); break;
    case 0xC1CBD8: case 0xC1CBDA: case 0xC1CBDC: case 0xC1CBF4:
        count = opcode & 0x20u ? D(destination) & 63u : destination ? destination : 8;
        step_asl_long(&D(reg), count); break;
    case 0xC1CB60: case 0xC1CB7C: case 0xC1CB84: case 0xC1CC90:
    case 0xC1CC98:
        A(destination) = cache_step_address(mode, reg, 4); break;
    case 0xC1CB6A: case 0xC1CC7E:
        SET_W(D(reg), (int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC1CBF2:
        D(reg) = (uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC1CB74: case 0xC1CC88:
        flags_logic_b(cache_step_read(mode, reg, 1)); break;
    case 0xC1CBB0: case 0xC1CCA4:
        flags_logic_w(cache_step_read(mode, reg, 2)); break;
    case 0xC1CB90:
        A(destination) += (uint32_t)(int32_t)(int16_t)D(reg); break;
    case 0xC1CBA4: case 0xC1CC14: case 0xC1CC1C:
        value = m68ki_read_imm_16(); SET_W(D(reg), D(reg) & value);
        flags_logic_w(D(reg)); break;
    case 0xC1CBCC: case 0xC1CBD0: case 0xC1CBDE: case 0xC1CC26:
        mask = m68ki_read_imm_16(); width = opcode & 0x40u ? 4 : 2;
        address = mode == 3 ? A(reg) : cache_step_address(mode, reg, width);
        if (opcode & 0x0400u) renderer_load(address, mask, width, mode == 3 ? (int)reg : -1);
        else renderer_store(address, mask, width, -1);
        break;
    case 0xC1CC0E:
        count = m68ki_read_imm_16() & 31u; FLAG_Z = D(reg) & (1u << count); break;
    case 0xC1CC2E:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1CC42: case 0xC1CC50:
        address = cache_step_address(mode, reg, 1); value = m68k_read_memory_8(address);
        step_subtract_byte(&value, 1); m68k_write_memory_8(address, value); break;
    case 0xC1CC66:
        value = cache_step_read(mode, reg, 2); step_compare_word(value, D(destination)); break;
    case 0xC1CC7C:
        count = 8; value = (uint16_t)D(reg);
        SET_W(D(reg), value >> count); flags_logic_w(D(reg));
        FLAG_X = FLAG_C = ((value >> (count - 1)) & 1u) << 8;
        USE_CYCLES(count << CYC_SHIFT); break;
    case 0xC1CC86:
        address = A(reg); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1CCA8:
        D(destination) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC1CCAE:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        { uint32_t old = m68k_read_memory_16(address);
          step_add_word(&old, value); step_write_word(address, old); }
        break;
    case 0xC1CCBA: REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
int glue_C1CB26_step(void) { return glue_C1CB14_step(); }
