/* Source timing for the stores centre mark and two icon streams, C309FE-
 * C30B5A. The readable drawing behavior is hud_stores.c. These adapters retain
 * original child/event boundaries instead of drawing a whole stream at one
 * fixed charge. Source: recomp_003.c's C30A00/C30AE2 instruction listings. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "ports_glue.h"

static int stores_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode = step_begin(pc), mask;
    unsigned mode = (opcode >> 3) & 7u, reg = opcode & 7u;
    unsigned destination = (opcode >> 9) & 7u, width;
    switch (pc) {
    case 0xC309FE: case 0xC30AE0: case 0xC30B5A:
        REG_PC = m68ki_pull_32(); break;
    case 0xC30A00: case 0xC30AF4:
        flags_logic_b(cache_step_read(mode, reg, 1)); break;
    case 0xC30AF0:
        flags_logic_l(D(4)); break;
    case 0xC30A06: case 0xC30B16:
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC30A26: case 0xC30AB0: case 0xC30AD0:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC30A3C: case 0xC30B0C:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC30A42: case 0xC30AF2: case 0xC30AF6:
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC30A5A:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC30AE4: case 0xC30B1C:
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC30A72: case 0xC30AB6: case 0xC30AD6: case 0xC30AEE:
    case 0xC30B00: case 0xC30B58:
        step_branch(pc, opcode, 1); break;
    case 0xC30A08:
        address = cache_step_address(mode, reg, 1);
        value = cache_step_read_memory(address, 1);
        step_subtract_byte(&value, 1);
        cache_step_write_memory(address, value, 1, 0); break;
    case 0xC30AE2:
        step_subtract_byte(&D(4), 1); break;
    case 0xC30B3C:
        step_subtract_word(&D(0), 1); break;
    case 0xC30A96: case 0xC30B28: case 0xC30B4C:
        step_add_word(&D(reg), destination); break;
    case 0xC30A0E: case 0xC30AA2: case 0xC30AC0:
        A(destination) = cache_step_address(mode, reg, 4); break;
    case 0xC30A14:
        A(0) += (uint32_t)(int32_t)(int16_t)cache_step_read(mode, reg, 2); break;
    case 0xC30A1A: case 0xC30A4E: case 0xC30AA6: case 0xC30AC4:
        width = 1; goto move;
    case 0xC30A28:
        width = 4; goto move;
    case 0xC30A32: case 0xC30A44: case 0xC30A5C: case 0xC30A74:
    case 0xC30A8A: case 0xC30AE6: case 0xC30AF8: case 0xC30B02:
    case 0xC30B0A: case 0xC30B0E: case 0xC30B1E: case 0xC30B26:
        width = 2; goto move;
    case 0xC30A36: case 0xC30A48: case 0xC30B10: case 0xC30B20:
        step_add_word(&D(destination), (uint16_t)cache_step_read(mode, reg, 2)); break;
    case 0xC30A1E: case 0xC30A52: case 0xC30AC8:
        value = m68ki_read_imm_16();
        SET_B(D(reg), D(reg) & value); flags_logic_b(D(reg)); break;
    case 0xC30A22: case 0xC30A56: case 0xC30AAC: case 0xC30ACC:
        value = m68ki_read_imm_16(); step_compare_byte((uint8_t)value, (uint8_t)D(reg)); break;
    case 0xC30A3E: case 0xC30B18:
        value = m68ki_read_imm_16(); step_compare_word((uint16_t)value, (uint16_t)D(reg)); break;
    case 0xC30A64: case 0xC30A7C: case 0xC30A92: case 0xC30B2A:
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC30B2E: case 0xC30B3E:
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 2, 7); break;
    case 0xC30A6E: case 0xC30A86: case 0xC30A9E: case 0xC30B54:
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC30B38: case 0xC30B48:
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 2, 7); break;
    case 0xC30A68: case 0xC30A80: case 0xC30A98:
    case 0xC30B32: case 0xC30B42: case 0xC30B4E:
        address = cache_step_address(mode, reg, 4);
        m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC30ABC: case 0xC30ADC:
        value = m68ki_read_imm_16();
        m68ki_push_32(REG_PC); REG_PC = pc + 2 + (int16_t)value; break;
    case 0xC30AAA:
        value = (uint8_t)D(4);
        SET_B(D(4), value >> 4); flags_logic_b(D(4));
        FLAG_X = FLAG_C = ((value >> 3) & 1u) << 8;
        USE_CYCLES(4 << CYC_SHIFT); break;
    case 0xC30AB2: case 0xC30AD2: case 0xC30AB8: case 0xC30AD8:
        value = 1u << (m68ki_read_imm_16() & 31u);
        FLAG_Z = D(4) & value;
        if (pc == 0xC30AB2 || pc == 0xC30AD2) D(4) &= ~value;
        else D(4) |= value;
        break;
    default: return 0;
    }
    goto finish;
move:
    value = cache_step_read(mode, reg, width);
    cache_step_write((opcode >> 6) & 7u, destination, width, value);
    cache_step_logic(value, width);
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}

int glue_C30A00_step(void) { return stores_step(); }
int glue_C30AE2_step(void) { return stores_step(); }
