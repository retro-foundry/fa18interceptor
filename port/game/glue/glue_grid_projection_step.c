/* One-instruction timing bridge for $C279D0-$C27D23. The full grid operation
 * and mathematical observers are in grid_projection_packet.c. */
#include "glue_step.h"

static void grid_load_words(uint32_t address, uint16_t mask, int postincrement,
                             unsigned address_register) {
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
        REG_DA[i] = (uint32_t)(int32_t)(int16_t)m68k_read_memory_16(address);
        address += 2; ++count;
    }
    if (postincrement) A(address_register) = address;
    USE_CYCLES(count << CYC_MOVEM_W);
}
static void grid_store_words(uint32_t address, uint16_t mask) {
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
        m68k_write_memory_16(address, REG_DA[i]); address += 2; ++count;
    }
    USE_CYCLES(count << CYC_MOVEM_W);
}
static void grid_shift_word(uint32_t *reg, unsigned count, int arithmetic) {
    uint16_t old = (uint16_t)*reg, result;
    uint32_t mask;
    count &= 63u;
    result = count < 16 ? (uint16_t)(old << count) : 0;
    SET_W(*reg, result); flags_logic_w(result); FLAG_C = FLAG_V = 0;
    if (count) {
        FLAG_X = FLAG_C = count <= 16 ? ((old >> (16 - count)) & 1u) << 8 : 0;
        if (arithmetic) {
            if (count < 16) {
                mask = (0xffffu << (15 - count)) & 0xffffu;
                old &= mask; FLAG_V = (old != 0 && old != mask) << 7;
            } else if (count == 16) FLAG_V = (old != 0 && old != 0xffffu) << 7;
            else FLAG_V = (old != 0) << 7;
        }
    }
    USE_CYCLES(count << CYC_SHIFT);
}
static void grid_negate_word(uint32_t *reg) {
    uint16_t old = (uint16_t)*reg; SET_W(*reg, 0); step_subtract_word(reg, old);
}
static void grid_multiply(uint32_t *reg, uint16_t source) {
    uint32_t bits = (uint32_t)source << 1;
    unsigned i, transitions = 0;
    for (i = 0; i < 16; ++i, bits >>= 1)
        if ((bits & 3u) == 1u || (bits & 3u) == 2u) ++transitions;
    USE_CYCLES(2 * transitions);
    *reg = (uint32_t)((int32_t)(int16_t)*reg * (int32_t)(int16_t)source);
    flags_logic_l(*reg);
}
static void grid_divide(uint32_t *reg, int16_t divisor) {
    int32_t dividend = (int32_t)*reg, quotient, remainder;
    uint32_t absolute_dividend, absolute_divisor, magnitude;
    int cycles = 6, i;
    if (!divisor) { m68ki_exception_trap(EXCEPTION_ZERO_DIVIDE); return; }
    absolute_dividend = dividend < 0 ? 0u - (uint32_t)dividend : (uint32_t)dividend;
    absolute_divisor = divisor < 0 ? (uint32_t)-divisor : (uint32_t)divisor;
    if (dividend < 0) ++cycles;
    if ((absolute_dividend >> 16) >= absolute_divisor) cycles = (cycles + 2) * 2;
    else {
        magnitude = absolute_dividend / absolute_divisor; cycles += 55;
        if (divisor >= 0) cycles += dividend >= 0 ? -1 : 1;
        for (i = 0; i < 15; ++i, magnitude <<= 1)
            if ((int16_t)magnitude >= 0) ++cycles;
        cycles *= 2;
    }
    USE_CYCLES(cycles - 158);
    if ((uint32_t)dividend == 0x80000000u && divisor == -1) {
        FLAG_Z = FLAG_N = FLAG_V = FLAG_C = 0; *reg = 0; return;
    }
    quotient = dividend / divisor; remainder = dividend % divisor;
    if (quotient == (int16_t)quotient) {
        FLAG_Z = (uint16_t)quotient; FLAG_N = NFLAG_16(quotient); FLAG_V = FLAG_C = 0;
        *reg = ((uint32_t)(uint16_t)remainder << 16) | (uint16_t)quotient;
    } else FLAG_V = VFLAG_SET;
}
static void grid_change_frame_word(uint32_t address, int subtract) {
    uint32_t value = m68k_read_memory_16(address);
    if (subtract) step_subtract_word(&value, 1); else step_add_word(&value, 1);
    m68k_write_memory_16(address, value);
}

int glue_C279D0_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode, mask;
    if (pc < 0xc279d0u || pc >= 0xc27d24u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC279D0:
        m68ki_push_32(A(6)); A(6) = A(7);
        A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC279D4: case 0xC279D8: case 0xC279DE: case 0xC27A48:
    case 0xC27A4C: case 0xC27A50: case 0xC27A88: case 0xC27B6C:
    case 0xC27B72: case 0xC27C56: case 0xC27C5C: case 0xC27D18: case 0xC27D1E:
        value = m68ki_read_imm_16(); SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC279DC: case 0xC27C4E: case 0xC27C52: case 0xC27D10: case 0xC27D14:
        SET_W(D(opcode & 7u), 0); flags_logic_w(0); break;
    case 0xC279E2:
        mask = m68ki_read_imm_16(); grid_store_words(m68ki_read_imm_32(), mask); break;
    case 0xC279EA: case 0xC27B18:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC279F2:
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(3), value); flags_logic_b(value); break;
    case 0xC279FA: case 0xC27A04: case 0xC27A0C:
        D(2) = m68ki_read_imm_32(); flags_logic_l(D(2)); break;
    case 0xC27A00: step_subtract_byte(&D(3), 1); break;
    case 0xC27A12: D(1) = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(D(1)); break;
    case 0xC27A18: step_compare_long(D(2), D(1)); break;
    case 0xC27A1E:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC27A26: case 0xC27A58:
        m68k_write_memory_16(step_displacement(A(6)), 0); flags_logic_w(0); break;
    case 0xC27A2A: step_compare_long(m68ki_read_imm_32(), D(1)); break;
    case 0xC27A32: case 0xC27C3C:
        grid_change_frame_word(step_displacement(A(6)), pc == 0xC27C3C); break;
    case 0xC27A36: case 0xC27A3C:
        value = m68k_read_memory_16(m68ki_read_imm_32());
        SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC27A42: case 0xC27B12:
        mask = m68ki_read_imm_16(); grid_store_words(step_displacement(A(6)), mask); break;
    case 0xC27A54: case 0xC27AB2: case 0xC27AB8: case 0xC27AFA: case 0xC27B7A:
        value = m68k_read_memory_16(step_displacement(A(6)));
        SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC27A5C: case 0xC27A6E: case 0xC27BDC: case 0xC27BE4:
    case 0xC27BE8: case 0xC27BF0: case 0xC27C9C: case 0xC27CA4:
    case 0xC27CA8: case 0xC27CB0:
        step_compare_word(D(opcode & 7u), D((opcode >> 9) & 7u)); break;
    case 0xC27A60: case 0xC27A72: case 0xC27A7A: case 0xC27AF4:
    case 0xC27B44: case 0xC27B92: case 0xC27BA4: case 0xC27C66:
        A((opcode >> 9) & 7u) = m68ki_read_imm_32(); break;
    case 0xC27A66:
        value = m68ki_read_imm_16(); address = step_displacement(A(6));
        m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC27A80: case 0xC27A84:
        value = m68k_read_memory_16(A(3)); A(3) += 2;
        m68k_write_memory_16(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC27A8C: case 0xC27A90: case 0xC27AFE: case 0xC27B00:
    case 0xC27B30: case 0xC27B36: case 0xC27BBA: case 0xC27BE0:
    case 0xC27BEC: case 0xC27C7C: case 0xC27CA0: case 0xC27CAC:
    case 0xC27CEE: case 0xC27CF0:
        value = D(opcode & 7u); SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC27A8E: SET_W(D(1), step_lsr_word_value(D(1), 1)); break;
    case 0xC27A92: case 0xC27A96:
        value = m68k_read_memory_16(step_displacement(A(6)));
        step_subtract_word(&D((opcode >> 9) & 7u), value); break;
    case 0xC27A9A: case 0xC27B34: case 0xC27B3A: case 0xC27BE2:
    case 0xC27BEE: case 0xC27C1C: case 0xC27C22: case 0xC27CA2:
    case 0xC27CAE: case 0xC27CDC: case 0xC27CE2:
        grid_negate_word(&D(opcode & 7u)); break;
    case 0xC27A9C: case 0xC27AA6:
        SET_W(D((opcode >> 9) & 7u), D((opcode >> 9) & 7u) & D(opcode & 7u));
        flags_logic_w(D((opcode >> 9) & 7u)); break;
    case 0xC27A9E: case 0xC27AA8: case 0xC27B28: case 0xC27B2C:
        value = m68k_read_memory_16(step_displacement(A(6)));
        step_add_word(&D((opcode >> 9) & 7u), value); break;
    case 0xC27AA2: case 0xC27AAC: case 0xC27ABE: case 0xC27B24: case 0xC27B76:
        value = D(opcode & 7u); m68k_write_memory_16(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC27AB0: D(0) = A(3); flags_logic_l(D(0)); break;
    case 0xC27ABC: case 0xC27B7E: case 0xC27B80:
        grid_shift_word(&D(opcode & 7u), D((opcode >> 9) & 7u), 1); break;
    case 0xC27AC2: case 0xC27ACA: case 0xC27AD2: case 0xC27ADC: case 0xC27AE4: case 0xC27AEC:
        value = m68ki_read_imm_32(); m68k_write_memory_32(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC27B02: grid_multiply(&D(1), m68k_read_memory_16(A(0))); break;
    case 0xC27B06: case 0xC27B0C: case 0xC27BCE: case 0xC27C90:
        value = m68k_read_memory_16(step_displacement(A(0)));
        grid_multiply(&D((opcode >> 9) & 7u), value); break;
    case 0xC27B04: case 0xC27B0A: case 0xC27B10: case 0xC27BB6:
    case 0xC27BC8: case 0xC27BD4: case 0xC27C78: case 0xC27C8A: case 0xC27C96:
        step_asr_long(&D(opcode & 7u), 8); break;
    case 0xC27B20: case 0xC27B9C:
        mask = m68ki_read_imm_16(); grid_load_words(A(opcode & 7u), mask, 1, opcode & 7u); break;
    case 0xC27B3C: case 0xC27B3E:
        value = D(opcode & 7u); SET_W(D(opcode & 7u), (int16_t)value >> 8);
        flags_logic_w(D(opcode & 7u)); FLAG_X = FLAG_C = ((value >> 7) & 1u) << 8;
        USE_CYCLES(8 << CYC_SHIFT); break;
    case 0xC27B40: grid_shift_word(&D(4), 5, 0); break;
    case 0xC27B42: case 0xC27BA0: case 0xC27BA2: case 0xC27BB8:
    case 0xC27BCA: case 0xC27BD6: case 0xC27C7A: case 0xC27C8C: case 0xC27C98:
        step_add_word(&D((opcode >> 9) & 7u), REG_DA[opcode & 15u]); break;
    case 0xC27B4A:
        value = m68k_read_memory_8(step_indexed(A(4))); SET_B(D(3), value); flags_logic_b(value); break;
    case 0xC27B4E:
        SET_W(D(3), (int16_t)(int8_t)D(3)); flags_logic_w(D(3)); break;
    case 0xC27B50:
        value = m68k_read_memory_16(step_displacement(A(6))); step_compare_word(value, D(3)); break;
    case 0xC27B58: case 0xC27B8C: flags_logic_w(D(2)); break;
    case 0xC27B5C: flags_logic_w(m68k_read_memory_16(step_displacement(A(6)))); break;
    case 0xC27B62: step_subtract_word(&D(3), 1); break;
    case 0xC27B66: step_compare_word(m68ki_read_imm_16(), D(2)); break;
    case 0xC27B82: case 0xC27B84:
        A((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int16_t)D(opcode & 7u); break;
    case 0xC27B86:
        mask = m68ki_read_imm_16(); grid_load_words(step_displacement(A(6)), mask, 0, 0); break;
    case 0xC27B98: A(2) = m68k_read_memory_32(step_indexed(A(6))); break;
    case 0xC27BAA: case 0xC27BAE: case 0xC27BBC: case 0xC27BC0:
    case 0xC27C6C: case 0xC27C70: case 0xC27C7E: case 0xC27C82:
        value = m68k_read_memory_16(A(0)); A(0) += 2;
        SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC27BAC: case 0xC27BBE: case 0xC27C6E: case 0xC27C80: A(0) += 2; break;
    case 0xC27BB0: case 0xC27BB2: case 0xC27BC2: case 0xC27BC4:
    case 0xC27C72: case 0xC27C74: case 0xC27C84: case 0xC27C86:
        grid_multiply(&D((opcode >> 9) & 7u), D(opcode & 7u)); break;
    case 0xC27BB4: case 0xC27BC6: case 0xC27BD2: case 0xC27C76: case 0xC27C88: case 0xC27C94:
        step_add_long(&D((opcode >> 9) & 7u), D(opcode & 7u)); break;
    case 0xC27BCC: case 0xC27C8E:
        value = m68k_read_memory_16(A(0)); A(0) += 2;
        grid_multiply(&D(3), value); break;
    case 0xC27BF4: case 0xC27C06: case 0xC27CB4: case 0xC27CC6:
        grid_multiply(&D((opcode >> 9) & 7u), m68ki_read_imm_16()); break;
    case 0xC27BF8: case 0xC27C0A: case 0xC27CB8: case 0xC27CCA:
        grid_divide(&D((opcode >> 9) & 7u), (int16_t)D(4)); break;
    case 0xC27BFA: case 0xC27C0C: case 0xC27CBA: case 0xC27CCC:
        step_add_word(&D(opcode & 7u), m68ki_read_imm_16()); break;
    case 0xC27C00: case 0xC27C12: case 0xC27CC0: case 0xC27CD2:
        step_compare_word(m68ki_read_imm_16(), D(opcode & 7u)); break;
    case 0xC27C18: case 0xC27C1E: case 0xC27CD8: case 0xC27CDE:
        step_subtract_word(&D(opcode & 7u), m68ki_read_imm_16()); break;
    case 0xC27C24: case 0xC27C26:
        value = D(opcode & 7u); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC27C28: step_compare_long(m68ki_read_imm_32(), A(1)); break;
    case 0xC27C32: case 0xC27CF2:
        step_predecrement_long(A(3)); flags_logic_l(A(3)); break;
    case 0xC27C34: case 0xC27CFC: case 0xC27D04:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC27C3A: case 0xC27D0A: A(3) = m68ki_pull_32(); break;
    case 0xC27C44:
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC27C4A: A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC27C4C: m68ki_jump(m68ki_pull_32()); break;
    case 0xC27C62: case 0xC27C64:
        value = A(opcode & 7u); SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC27CE4: step_compare_word(m68k_read_memory_16(m68ki_read_imm_32()), D(2)); break;
    case 0xC27CF4:
        value = m68ki_read_imm_16(); step_compare_word(value, m68k_read_memory_16(step_displacement(A(6)))); break;
    case 0xC27A0A: case 0xC27A6C: case 0xC27A78: case 0xC27ADA:
    case 0xC27B70: case 0xC27C50: case 0xC27C54: case 0xC27C5A:
    case 0xC27C60: case 0xC27D02: case 0xC27D0C: case 0xC27D12:
    case 0xC27D16: case 0xC27D1C: case 0xC27D22: step_branch(pc, opcode, 1); break;
    case 0xC279F8: case 0xC27A02: case 0xC27AB6: case 0xC27CFA:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC27A1A: case 0xC27A5E: case 0xC27A70: case 0xC27C2E:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC27A30: case 0xC27B32: case 0xC27B38: case 0xC27B5A:
    case 0xC27B8E: case 0xC27C04: case 0xC27C16: case 0xC27CC4: case 0xC27CD6:
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC27B54: case 0xC27BDE: case 0xC27BE6: case 0xC27BEA:
    case 0xC27BF2: case 0xC27C40: case 0xC27C9E: case 0xC27CA6:
    case 0xC27CAA: case 0xC27CB2: case 0xC27CEA:
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC27B60: case 0xC27B6A: step_branch(pc, opcode, COND_NE()); break;
    case 0xC27B64: case 0xC27BD8: case 0xC27C9A: step_branch(pc, opcode, COND_LE()); break;
    case 0xC27BFE: case 0xC27C10: case 0xC27CBE: case 0xC27CD0:
        step_branch(pc, opcode, COND_LT()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
