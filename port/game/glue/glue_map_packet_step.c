/* Source timing for the sibling normal/wide map packet passes. map_packet.c
 * owns the readable selector, directory walk and projection operations.
 * These steps suspend at the polygon child and at chipset service events. */
#include "glue_unsigned_division_step.h"

static void map_negate_long(uint32_t *reg) {
    uint32_t old = *reg; *reg = 0; step_subtract_long(reg, old);
}
static void map_negate_word(uint32_t *reg) {
    uint16_t old = (uint16_t)*reg; SET_W(*reg, 0); step_subtract_word(reg, old);
}
static void map_shift_word(uint32_t *reg, unsigned count, int arithmetic) {
    uint16_t old = (uint16_t)*reg, result;
    uint32_t mask;
    count &= 63u;
    result = count < 16 ? (uint16_t)(old << count) : 0;
    SET_W(*reg, result); flags_logic_w(result); FLAG_C = FLAG_V = 0;
    if (count) {
        FLAG_X = FLAG_C = count <= 16 ? ((old >> (16 - count)) & 1u) << 8 : 0;
        if (arithmetic) {
            if (count < 16) {
                mask = (0xFFFFu << (15 - count)) & 0xFFFFu;
                old &= mask; FLAG_V = (old != 0 && old != mask) << 7;
            } else if (count == 16) FLAG_V = (old != 0 && old != 0xFFFFu) << 7;
            else FLAG_V = (old != 0) << 7;
        }
    }
    USE_CYCLES(count << CYC_SHIFT);
}
static void map_rotate_long(uint32_t *reg) {
    uint32_t old = *reg;
    *reg = (old << 4) | (old >> 28); flags_logic_l(*reg);
    FLAG_C = (*reg & 1u) << 8; USE_CYCLES(4 << CYC_SHIFT);
}
static void map_multiply(uint32_t *reg, uint16_t source, int signed_words) {
    uint32_t bits = signed_words ? (uint32_t)source << 1 : source;
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i, bits >>= 1)
        if (signed_words ? ((bits & 3u) == 1u || (bits & 3u) == 2u) : (bits & 1u)) ++count;
    USE_CYCLES(2 * count);
    *reg = signed_words ? (uint32_t)((int32_t)(int16_t)*reg * (int32_t)(int16_t)source) :
                         (uint32_t)(uint16_t)*reg * source;
    flags_logic_l(*reg);
}
static void map_load_words(uint32_t address, uint16_t mask) {
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
        REG_DA[i] = (uint32_t)(int32_t)(int16_t)m68k_read_memory_16(address);
        address += 2; ++count;
    }
    USE_CYCLES(count << CYC_MOVEM_W);
}
static void map_store_words(uint32_t address, uint16_t mask) {
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
        m68k_write_memory_16(address, REG_DA[i]); address += 2; ++count;
    }
    USE_CYCLES(count << CYC_MOVEM_W);
}
static void map_add_memory_word(uint32_t address) {
    uint32_t old = m68k_read_memory_16(address), result = old + 1;
    m68k_write_memory_16(address, result);
    FLAG_N = NFLAG_16(result); FLAG_Z = result & 0xFFFFu;
    FLAG_V = VFLAG_ADD_16(1, old, result); FLAG_X = FLAG_C = CFLAG_16(result);
}

static int map_packet_instruction(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC2AB34: case 0xC2AB46: case 0xC2AB4C: case 0xC2AB52:
    case 0xC2AB6A: case 0xC2AB70: case 0xC2AB76: case 0xC2AD50:
    case 0xC2AD62: case 0xC2AD74: case 0xC2AD7A:
        value = m68ki_read_imm_16(); address = step_displacement(A(6));
        m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC2AB3A: case 0xC2AB5A: case 0xC2AB5E: case 0xC2AD1E: case 0xC2AD22:
        address = step_displacement(A(6));
        m68k_write_memory_16(address, 0); flags_logic_w(0); break;
    case 0xC2AD26:
        address = step_displacement(A(6));
        m68k_write_memory_8(address, 0); flags_logic_b(0); break;
    case 0xC2AB3E: case 0xC2AB62:
        value = m68ki_read_imm_32(); address = step_displacement(A(6));
        m68k_write_memory_32(address, value); flags_logic_l(value); break;
    case 0xC2AB7C: case 0xC2AD30: case 0xC2ADDA: case 0xC2AE92:
        flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC2AB84: case 0xC2AB8C: case 0xC2AC98: case 0xC2AF20:
    case 0xC2AF7E: case 0xC2AF86:
        A((opcode >> 9) & 7u) = m68ki_read_imm_32(); break;
    case 0xC2AB92: A(4) += (uint32_t)(int32_t)(int16_t)m68k_read_memory_16(m68ki_read_imm_32()); break;
    case 0xC2AB98: A(4) = step_displacement(A(4)); break;
    case 0xC2AB9C: case 0xC2ABB0: case 0xC2ABB4: case 0xC2AE1C: case 0xC2AE20: case 0xC2AF06:
        value = m68k_read_memory_32(step_displacement(A(4)));
        D((opcode >> 9) & 7u) = value; flags_logic_l(value); break;
    case 0xC2ABA0: case 0xC2AEBE: case 0xC2AECE: case 0xC2AF10:
        step_add_long(&D(opcode & 7u), m68ki_read_imm_32()); break;
    case 0xC2ABA6: case 0xC2ABB8: case 0xC2ABBA: case 0xC2AE54:
    case 0xC2AE56: case 0xC2AEBA: case 0xC2AEEA: case 0xC2AEEC:
    case 0xC2AEF4: case 0xC2AEF6: case 0xC2AEF8: case 0xC2AF16:
    case 0xC2AF1C: case 0xC2AF98: step_swap(&D(opcode & 7u)); break;
    case 0xC2ABA8: case 0xC2AEEE: case 0xC2AEF0: case 0xC2AF18:
        map_rotate_long(&D(opcode & 7u)); break;
    case 0xC2ABAA: case 0xC2AE44: case 0xC2AE46: case 0xC2AEC6: case 0xC2AED6:
        map_negate_long(&D(opcode & 7u)); break;
    case 0xC2ABAC:
        m68k_write_memory_16(step_displacement(A(6)), D(2)); flags_logic_w(D(2)); break;
    case 0xC2ABBC: case 0xC2ABBE: case 0xC2ACC0: case 0xC2ACEA:
    case 0xC2AEFA: case 0xC2AF26: case 0xC2AF28: case 0xC2AF9A:
    case 0xC2AFA2: case 0xC2AFA4: case 0xC2AFB6: case 0xC2AFB8:
        value = D(opcode & 7u); SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC2ABC0: case 0xC2AD8E: case 0xC2ADA4: case 0xC2AF92:
        value = m68k_read_memory_16(step_displacement(A(6)));
        SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC2ABC4: case 0xC2ABC6: case 0xC2ABE0: case 0xC2ABE2:
        value = D((opcode >> 9) & 7u);
        SET_W(D(opcode & 7u), step_lsr_word_value(D(opcode & 7u), value)); break;
    case 0xC2ABC8: case 0xC2ABD8: case 0xC2AD00: case 0xC2AD2A:
    case 0xC2ADBA: case 0xC2AE24: case 0xC2AE5C: case 0xC2AE70:
    case 0xC2AEE4: case 0xC2AF0A:
        flags_logic_w(m68k_read_memory_16(step_displacement(A(6)))); break;
    case 0xC2ABCE: case 0xC2ABD0: step_add_word(&D(opcode & 7u), 4); break;
    case 0xC2ABD2: case 0xC2AF3A:
        mask = m68ki_read_imm_16(); map_store_words(step_displacement(A(6)), mask); break;
    case 0xC2ABDE: case 0xC2AE76:
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)opcode;
        flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC2ABE4: case 0xC2ABE8: case 0xC2AD1A:
        value = m68ki_read_imm_16(); SET_W(D(opcode & 7u), D(opcode & 7u) & value);
        flags_logic_w(D(opcode & 7u)); break;
    case 0xC2ABEC: case 0xC2ABF0: step_subtract_word(&D(opcode & 7u), 3); break;
    case 0xC2ABEE: case 0xC2ABF2: case 0xC2AF1E: map_negate_word(&D(opcode & 7u)); break;
    case 0xC2ABF4: case 0xC2ABF6: case 0xC2ABF8: case 0xC2AC08:
    case 0xC2AC0A: case 0xC2AC7E: case 0xC2AC80: case 0xC2ACC2:
    case 0xC2ACC6: case 0xC2ACCA: case 0xC2ACE2: case 0xC2ACE4:
    case 0xC2ACEC: case 0xC2ACEE: case 0xC2ACF0: case 0xC2ACF2:
    case 0xC2AD84: case 0xC2AD92: case 0xC2ADA8: case 0xC2ADC0:
    case 0xC2ADC6: case 0xC2ADCA: case 0xC2AE8C: case 0xC2AF56:
    case 0xC2AF58: case 0xC2AFB0: case 0xC2AFC4: case 0xC2AFD4:
        step_add_word(&D((opcode >> 9) & 7u), REG_DA[opcode & 15u]); break;
    case 0xC2ABFA: case 0xC2AC1A: case 0xC2AC24: case 0xC2AC46:
    case 0xC2AC52: case 0xC2AC5C: case 0xC2ACCE: case 0xC2ACD8:
    case 0xC2AD46: case 0xC2AD58: case 0xC2AD6A:
        value = m68ki_read_imm_32(); step_compare_long(value, m68k_read_memory_32(step_displacement(A(6)))); break;
    case 0xC2AC04: case 0xC2AC0E: case 0xC2AC2E: case 0xC2AC36:
    case 0xC2AC66: case 0xC2AC6C: case 0xC2AC72: case 0xC2ACAA:
    case 0xC2ACAE: case 0xC2ACB2: case 0xC2AD80: case 0xC2AE88:
        address = REG_PC; A((opcode >> 9) & 7u) = step_displacement(address); break;
    case 0xC2AC12: case 0xC2ACBE: case 0xC2ACFC: case 0xC2AE50: case 0xC2AE52:
        value = (opcode >> 9) & 7u; map_shift_word(&D(opcode & 7u), value ? value : 8, 1); break;
    case 0xC2ADC2: case 0xC2ADC8:
        map_shift_word(&D(1), (opcode >> 9) & 7u, 0); break;
    case 0xC2AC14: case 0xC2AC82: case 0xC2ACC4: case 0xC2ACC8:
    case 0xC2ACCC: case 0xC2ACE6: case 0xC2ACF6: case 0xC2ACFE:
    case 0xC2AD86: case 0xC2ADD4:
        A((opcode >> 9) & 7u) += (uint32_t)(int32_t)(int16_t)D(opcode & 7u); break;
    case 0xC2AC3E: case 0xC2AC76: case 0xC2ACB6:
        value = m68k_read_memory_8(m68ki_read_imm_32());
        SET_B(D((opcode >> 9) & 7u), value); flags_logic_b(value); break;
    case 0xC2AC44: case 0xC2AC7C: case 0xC2ACBC: case 0xC2AD0A:
    case 0xC2AD8C: case 0xC2ADA2: case 0xC2AE4A: case 0xC2AE4E:
        SET_W(D(opcode & 7u), (int16_t)(int8_t)D(opcode & 7u)); flags_logic_w(D(opcode & 7u)); break;
    case 0xC2AC84: case 0xC2AD08: case 0xC2AD8A: case 0xC2AE48:
        value = m68k_read_memory_8(A(opcode & 7u)); A(opcode & 7u) += 1;
        SET_B(D((opcode >> 9) & 7u), value); flags_logic_b(value); break;
    case 0xC2ADA0: case 0xC2AE4C:
        value = m68k_read_memory_8(A(opcode & 7u));
        SET_B(D((opcode >> 9) & 7u), value); flags_logic_b(value); break;
    case 0xC2AC88: step_compare_byte(D(4), D(2)); break;
    case 0xC2AC8C: case 0xC2AC90:
        value = m68k_read_memory_8(A(0)); A(0) += 1; step_compare_byte(value, D(2)); break;
    case 0xC2AC94: step_compare_byte(m68k_read_memory_8(A(0)), D(2)); break;
    case 0xC2AC9E:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC2ACF4: case 0xC2ACFA: case 0xC2AD88: case 0xC2ADCC: case 0xC2AFA0:
        A((opcode >> 9) & 7u) = A(opcode & 7u); break;
    case 0xC2AD0C: value = m68ki_read_imm_16(); step_compare_byte(value, D(2)); break;
    case 0xC2AD16: map_add_memory_word(step_displacement(A(6))); break;
    case 0xC2AD38: case 0xC2AE7E: case 0xC2AF4A: case 0xC2AF66:
        value = m68ki_read_imm_16(); step_compare_word(value, D(opcode & 7u)); break;
    case 0xC2AD3E:
        value = m68k_read_memory_8(m68ki_read_imm_32()); address = step_displacement(A(6));
        m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC2AD98: case 0xC2ADAE:
        value = m68k_read_memory_16(step_displacement(A(6)));
        step_compare_word(value, D((opcode >> 9) & 7u)); break;
    case 0xC2ADB6: A(1) = m68k_read_memory_32(step_displacement(A(6))); break;
    case 0xC2ADCE:
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2ADD6: flags_logic_w(m68k_read_memory_16(A(3))); break;
    case 0xC2ADE6: case 0xC2AF6C:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC2ADEE: case 0xC2AF74: case 0xC2AFE2:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC2AE2A: case 0xC2AE30: case 0xC2AE38: case 0xC2AE3E:
        D(opcode & 7u) &= m68ki_read_imm_32(); flags_logic_l(D(opcode & 7u)); break;
    case 0xC2AE58: case 0xC2AE5A: case 0xC2AEE2:
        SET_W(D(opcode & 7u), 0); flags_logic_w(0); break;
    case 0xC2AE62: case 0xC2AE64: step_asl_long(&D(opcode & 7u), 4); break;
    case 0xC2AE66: case 0xC2AE68: case 0xC2AFAC: case 0xC2AFC0: case 0xC2AFD0:
        step_add_long(&D((opcode >> 9) & 7u), D(opcode & 7u)); break;
    case 0xC2AE6A: flags_logic_b(m68k_read_memory_8(step_displacement(A(6)))); break;
    case 0xC2AE78: D(3) = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(D(3)); break;
    case 0xC2AE7C: case 0xC2AEB8: case 0xC2AF2C: case 0xC2AF32:
    case 0xC2AF38: case 0xC2AFAE: case 0xC2AFC2: case 0xC2AFD2:
        step_asr_long(&D(opcode & 7u), 8); break;
    case 0xC2AE84: case 0xC2AEDC:
        value = m68ki_read_imm_16(); SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC2AE8E:
        value = m68k_read_memory_16(step_indexed(A(2))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2AE9A: D(7) = m68ki_read_imm_32(); flags_logic_l(D(7)); break;
    case 0xC2AEA0:
        value = m68ki_read_imm_16(); step_compare_word(value, m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC2AEAA: step_divide_unsigned(&D(7), m68ki_read_imm_16()); break;
    case 0xC2AEB0: step_divide_unsigned(&D(7), m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC2AEB6: map_multiply(&D(6), D(7), 0); break;
    case 0xC2AEBC: case 0xC2AECC: case 0xC2AF96:
        D((opcode >> 9) & 7u) = D(opcode & 7u); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC2AEC8: case 0xC2AED8: step_compare_long(D(6), D(7)); break;
    case 0xC2AEFC: case 0xC2AFDE: step_save_registers(); break;
    case 0xC2AFE8: case 0xC2AFF0: step_restore_registers(); break;
    case 0xC2AF00:
        D(5) = m68k_read_memory_32(A(3)); A(3) += 4; flags_logic_l(D(5)); break;
    case 0xC2AF2A: map_multiply(&D(1), m68k_read_memory_16(A(0)), 1); break;
    case 0xC2AF2E: case 0xC2AF34:
        value = m68k_read_memory_16(step_displacement(A(0))); map_multiply(&D((opcode >> 9) & 7u), value, 1); break;
    case 0xC2AF40: flags_logic_w(D(7)); break;
    case 0xC2AF44: A(3) = D(5); break;
    case 0xC2AF46: case 0xC2AF64:
        value = m68k_read_memory_16(A(3)); A(3) += 2; SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2AF52:
        value = m68ki_read_imm_16() & 31u; FLAG_Z = D(1) & (1u << value);
        D(1) &= ~(1u << value); USE_CYCLES(-(1 << CYC_SHIFT)); break;
    case 0xC2AF5A: D(1) = (uint32_t)(int32_t)(int16_t)D(1); flags_logic_l(D(1)); break;
    case 0xC2AF5C: step_compare_long(m68k_read_memory_32(step_displacement(A(6))), D(1)); break;
    case 0xC2AF84: case 0xC2AFB4: case 0xC2AFC8: case 0xC2AFD8:
        value = D(opcode & 7u); m68k_write_memory_16(A(5), value); A(5) += 2; flags_logic_w(value); break;
    case 0xC2AF8C:
        mask = m68ki_read_imm_16(); map_load_words(step_displacement(A(6)), mask); break;
    case 0xC2AF9C: case 0xC2AF9E:
        value = m68k_read_memory_16(A(3)); A(3) += 2;
        step_add_word(&D((opcode >> 9) & 7u), value); break;
    case 0xC2AFA6: case 0xC2AFAA: case 0xC2AFBA: case 0xC2AFBE: case 0xC2AFCA:
        value = m68k_read_memory_16(A(1)); A(1) += 2;
        map_multiply(&D((opcode >> 9) & 7u), value, 1); break;
    case 0xC2AFA8: case 0xC2AFBC: A(1) += 2; break;
    case 0xC2AFB2: case 0xC2AFC6: case 0xC2AFD6:
        map_shift_word(&D(opcode & 7u), D(3), 1); break;
    case 0xC2AFCC: map_multiply(&D(4), m68k_read_memory_16(step_displacement(A(1))), 1); break;
    case 0xC2AFDA: step_subtract_word(&D(1), 1); break;
    case 0xC2AFF8: m68ki_jump(m68ki_pull_32()); break;
    case 0xC2AB58: case 0xC2AB8A: case 0xC2AC0C: case 0xC2AC16:
    case 0xC2AC32: case 0xC2AC3A: case 0xC2AC6A: case 0xC2AC70:
    case 0xC2ACA6: case 0xC2ACE8: case 0xC2ACF8: case 0xC2AD56:
    case 0xC2AD68: case 0xC2ADC4: case 0xC2ADE2: case 0xC2ADF4:
    case 0xC2AE36: case 0xC2AEAE: case 0xC2AEE0: case 0xC2AEF2:
    case 0xC2AF1A: case 0xC2AF7A: case 0xC2AFEC: case 0xC2AFF4:
        step_branch(pc, opcode, 1); break;
    case 0xC2AB82: case 0xC2AC8A: case 0xC2AC8E: case 0xC2AC92:
    case 0xC2AD10: case 0xC2AD2E: case 0xC2AE74: case 0xC2AEE8:
    case 0xC2AF0E: case 0xC2AF42: case 0xC2AF4E: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2ABCC: case 0xC2ABDC: case 0xC2AC96: case 0xC2AD04:
    case 0xC2AD36: case 0xC2AD3C: case 0xC2ADBE: case 0xC2ADE0:
    case 0xC2AE28: case 0xC2AE60: case 0xC2AE6E: case 0xC2AE98:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2AC02: case 0xC2AC2C: case 0xC2AC4E: case 0xC2AC5A:
    case 0xC2AC64: case 0xC2ACD6: case 0xC2ACE0: case 0xC2AD4E:
    case 0xC2AD60: case 0xC2AD72: case 0xC2AD9C: case 0xC2ADB2:
    case 0xC2AECA: case 0xC2AF48: case 0xC2AF60: case 0xC2AFDC:
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2AC22: case 0xC2ADD2: case 0xC2AE82: case 0xC2AEDA: case 0xC2AF6A:
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2AC86: case 0xC2AD94: case 0xC2ADAA: case 0xC2AF02:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2AD14: case 0xC2ADD8: case 0xC2AEA8: case 0xC2AEC4: case 0xC2AED4:
        step_branch(pc, opcode, COND_GE()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}

int glue_C2AB34_step(void) {
    return REG_PC >= 0xC2AB34u && REG_PC < 0xC2AFFAu ? map_packet_instruction() : 0;
}
int glue_C2AB5A_step(void) {
    return REG_PC >= 0xC2AB5Au && REG_PC < 0xC2AFFAu ? map_packet_instruction() : 0;
}

int glue_C2AA9C_step(void) {
    uint32_t pc = REG_PC, value, address, displacement;
    uint16_t opcode, mask;
    if (pc < 0xC2AA9Cu || pc >= 0xC2AB34u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC2AA9C:
        m68ki_push_32(A(6)); A(6) = A(7);
        displacement = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16();
        A(7) += displacement; break;
    case 0xC2AAA0:
        address = m68ki_read_imm_32();
        m68k_write_memory_8(address, 0); flags_logic_b(0); break;
    case 0xC2AAA6:
        address = m68ki_read_imm_32();
        m68k_write_memory_16(address, 0); flags_logic_w(0); break;
    case 0xC2AAAC: case 0xC2AADA:
        flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC2AAB4: case 0xC2AAB8: case 0xC2AABE: case 0xC2AAC2:
        value = m68ki_read_imm_16(); SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC2AAC6: case 0xC2AAC8:
        SET_W(D(opcode & 7u), 0); flags_logic_w(0); break;
    case 0xC2AACA:
        mask = m68ki_read_imm_16(); map_store_words(m68ki_read_imm_32(), mask); break;
    case 0xC2AAD2: D(0) = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(D(0)); break;
    case 0xC2AAD8: map_negate_long(&D(0)); break;
    case 0xC2AAE2: case 0xC2AB10:
        step_compare_long(m68ki_read_imm_32(), D(0)); break;
    case 0xC2AAEA: case 0xC2AB0A: step_asr_long(&D(0), 4); break;
    case 0xC2AAEC: D(7) = m68ki_read_imm_32(); flags_logic_l(D(7)); break;
    case 0xC2AAF2:
        value = m68ki_read_imm_16(); step_compare_word(value, m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC2AAFC: step_divide_unsigned(&D(7), m68ki_read_imm_16()); break;
    case 0xC2AB02: step_divide_unsigned(&D(7), m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC2AB08: map_multiply(&D(0), D(7), 0); break;
    case 0xC2AB0C:
        m68k_write_memory_32(step_displacement(A(6)), D(0)); flags_logic_l(D(0)); break;
    case 0xC2AB18: case 0xC2AB24:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC2AB20: case 0xC2AB2C:
        displacement = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16();
        m68ki_push_32(REG_PC); REG_PC = pc + 2 + displacement; break;
    case 0xC2AB30: A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC2AB32: m68ki_jump(m68ki_pull_32()); break;
    case 0xC2AABC: case 0xC2AB00: step_branch(pc, opcode, 1); break;
    case 0xC2AAB2: case 0xC2AAE0: step_branch(pc, opcode, COND_NE()); break;
    case 0xC2AAE8: step_branch(pc, opcode, COND_GT()); break;
    case 0xC2AAFA: step_branch(pc, opcode, COND_GE()); break;
    case 0xC2AB16: step_branch(pc, opcode, COND_LE()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
