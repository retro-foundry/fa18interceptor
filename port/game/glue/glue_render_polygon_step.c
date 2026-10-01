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

static void polygon_asl_word(uint32_t *reg, unsigned count) {
    uint16_t before = (uint16_t)*reg;
    uint16_t result = (uint16_t)(before << count);
    uint16_t mask = (uint16_t)(0xFFFFu << (15 - count));
    SET_W(*reg, result); flags_logic_w(result);
    FLAG_X = FLAG_C = ((before >> (16 - count)) & 1u) << 8;
    before &= mask;
    FLAG_V = (before != 0 && before != mask) << 7;
    USE_CYCLES(count << CYC_SHIFT);
}

static void polygon_asr_word(uint32_t *reg, unsigned count) {
    uint16_t before = (uint16_t)*reg;
    uint16_t result = (uint16_t)((int16_t)before >> count);
    SET_W(*reg, result); flags_logic_w(result);
    FLAG_X = FLAG_C = ((before >> (count - 1)) & 1u) << 8;
    USE_CYCLES(count << CYC_SHIFT);
}

static void polygon_ror_word(uint32_t *reg, unsigned count) {
    uint16_t before = (uint16_t)*reg;
    uint16_t result = (uint16_t)((before >> count) | (before << (16 - count)));
    SET_W(*reg, result); flags_logic_w(result);
    FLAG_C = ((before >> (count - 1)) & 1u) << 8;
    USE_CYCLES(count << CYC_SHIFT);
}

static void polygon_negate_word(uint32_t *reg) {
    uint16_t before = (uint16_t)*reg;
    uint16_t result = (uint16_t)(0u - before);
    SET_W(*reg, result);
    FLAG_N = NFLAG_16(result); FLAG_Z = result;
    FLAG_V = before == 0x8000u ? VFLAG_SET : VFLAG_CLEAR;
    FLAG_X = FLAG_C = before ? CFLAG_SET : CFLAG_CLEAR;
}

static int polygon_muls_cycles(uint16_t source) {
    uint32_t bits = (uint32_t)source << 1;
    int count = 0, i;
    for (i = 0; i < 16 && bits; ++i, bits >>= 1)
        if ((bits & 3u) == 1u || (bits & 3u) == 2u) ++count;
    return 2 * count;
}

static int polygon_divs_cycles(int32_t dividend, int16_t divisor) {
    int cycles = 6, i;
    uint32_t absolute_dividend = dividend < 0 ? 0u - (uint32_t)dividend : (uint32_t)dividend;
    uint32_t absolute_divisor = (uint32_t)(divisor < 0 ? -divisor : divisor) & 0xFFFFu;
    uint32_t quotient;
    if (dividend < 0) ++cycles;
    if ((absolute_dividend >> 16) >= absolute_divisor) return (cycles + 2) * 2;
    quotient = absolute_dividend / absolute_divisor;
    cycles += 55;
    if (divisor >= 0) cycles += dividend >= 0 ? -1 : 1;
    for (i = 0; i < 15; ++i) {
        if ((int16_t)quotient >= 0) ++cycles;
        quotient <<= 1;
    }
    return cycles * 2;
}

static void polygon_multiply_signed(uint32_t *destination, uint16_t source) {
    uint32_t result;
    USE_CYCLES(polygon_muls_cycles(source));
    result = (uint32_t)((int32_t)(int16_t)source * (int32_t)(int16_t)*destination);
    *destination = result;
    FLAG_N = NFLAG_32(result); FLAG_Z = result; FLAG_V = FLAG_C = 0;
}

static void polygon_divide_signed(uint32_t *destination, int16_t divisor) {
    int32_t dividend = (int32_t)*destination;
    int32_t quotient, remainder;
    if (!divisor) {
        m68ki_exception_trap(EXCEPTION_ZERO_DIVIDE);
        return;
    }
    USE_CYCLES(polygon_divs_cycles(dividend, divisor) - 158);
    if ((uint32_t)dividend == 0x80000000u && divisor == -1) {
        FLAG_Z = 0; FLAG_N = FLAG_V = FLAG_C = 0; *destination = 0; return;
    }
    quotient = dividend / divisor;
    remainder = dividend % divisor;
    if (quotient == (int16_t)quotient) {
        FLAG_Z = (uint16_t)quotient; FLAG_N = NFLAG_16(quotient);
        FLAG_V = FLAG_C = 0;
        *destination = ((uint32_t)(uint16_t)remainder << 16) | (uint16_t)quotient;
    } else FLAG_V = VFLAG_SET;
}

static int polygon_edge_instruction(void) {
    uint32_t pc = REG_PC, address, value, temporary;
    uint16_t opcode = step_begin(pc), word;
    switch (pc) {
    case 0xC305AA: step_compare_word(D(1), D(3)); break;
    case 0xC305AC: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC305AE: step_branch(pc, opcode, COND_LS()); break;
    case 0xC305B0: case 0xC305C0: case 0xC305C8: case 0xC305CC:
    case 0xC305D6: case 0xC305DC: case 0xC305EA: case 0xC305F2:
    case 0xC3062A: case 0xC3062C: case 0xC30650: case 0xC3065A:
    case 0xC30660:
        SET_W(D((opcode >> 9) & 7u), REG_DA[opcode & 15u]);
        flags_logic_w(D((opcode >> 9) & 7u)); break;
    case 0xC305B2: step_subtract_word(&D(5), D(1)); break;
    case 0xC305B4: case 0xC305DA: step_subtract_word(&D(5), 1); break;
    case 0xC305B6: step_add_word(&D(1), 1); break;
    case 0xC305B8: step_compare_word(A(4), D(1)); break;
    case 0xC305BA: case 0xC305E4: step_branch(pc, opcode, COND_GT()); break;
    case 0xC305BC: A(1) = (uint32_t)(int32_t)(int16_t)D(1); break;
    case 0xC305BE: polygon_asl_word(&D(1), 3); break;
    case 0xC305C2: case 0xC305C4: case 0xC305C6: case 0xC305D0:
    case 0xC305EC: case 0xC305EE: case 0xC305F0: case 0xC305F6:
    case 0xC3064A: case 0xC3064C: case 0xC3064E:
    case 0xC3065C:
        step_add_word(&D((opcode >> 9) & 7u), REG_DA[opcode & 15u]); break;
    case 0xC305CA: step_subtract_word(&D(4), D(0)); break;
    case 0xC305CE: D(0) = (D(0) & 0xFFFF0000u) | step_lsr_word_value(D(0), 3); break;
    case 0xC305D2: case 0xC30618: case 0xC3063C: case 0xC30676:
        step_branch(pc, opcode, 1); break;
    case 0xC305D4: case 0xC306B2: m68ki_jump(m68ki_pull_32()); break;
    case 0xC305D8: step_subtract_word(&D(5), D(3)); break;
    case 0xC305DE: step_subtract_word(&D(4), D(2)); break;
    case 0xC305E0: step_add_word(&D(3), 1); break;
    case 0xC305E2: step_compare_word(A(4), D(3)); break;
    case 0xC305E6: A(1) = (uint32_t)(int32_t)(int16_t)D(3); break;
    case 0xC305E8: polygon_asl_word(&D(3), 3); break;
    case 0xC305F4: D(2) = (D(2) & 0xFFFF0000u) | step_lsr_word_value(D(2), 3); break;
    case 0xC305F8: D(7) = (uint32_t)(int32_t)(int16_t)D(7); flags_logic_l(D(7)); break;
    case 0xC305FA: step_add_long(&D(7), m68k_read_memory_32(m68ki_read_imm_32())); break;
    case 0xC30600: D(1) = 3; flags_logic_l(D(1)); break;
    case 0xC30602: value = m68ki_read_imm_16(); SET_W(D(6), D(6) & value); flags_logic_w(D(6)); break;
    case 0xC30606: polygon_ror_word(&D(6), 4); break;
    case 0xC30608: step_add_word(&D(6), m68ki_read_imm_16()); break;
    case 0xC3060C: flags_logic_w(D(4)); break;
    case 0xC3060E: step_branch(pc, opcode, COND_MI()); break;
    case 0xC30610: step_compare_word(D(5), D(4)); break;
    case 0xC30612: case 0xC3061E: step_branch(pc, opcode, COND_CS()); break;
    case 0xC30614: step_add_word(&D(1), m68ki_read_imm_16()); break;
    case 0xC3061A: polygon_negate_word(&D(4)); break;
    case 0xC3061C: step_compare_word(D(5), D(4)); break;
    case 0xC30620: step_add_word(&D(1), m68ki_read_imm_16()); break;
    case 0xC30624: A(4) -= (uint32_t)(int32_t)(int16_t)A(1); break;
    case 0xC30626: step_compare_word(A(4), D(5)); break;
    case 0xC30628: step_branch(pc, opcode, COND_LE()); break;
    case 0xC3062E: polygon_multiply_signed(&D(3), D(2)); break;
    case 0xC30630: step_add_long(&D(3), D(3)); break;
    case 0xC30632: polygon_divide_signed(&D(3), (int16_t)D(5)); break;
    case 0xC30634: polygon_asr_word(&D(3), 1); break;
    case 0xC30636: step_branch(pc, opcode, COND_CC()); break;
    case 0xC30638: step_add_word(&D(3), 1); break;
    case 0xC3063A: A(4) = (uint32_t)(int32_t)(int16_t)D(3); break;
    case 0xC3063E: step_add_word(&D(1), 8); break;
    case 0xC30640: temporary = D(4); D(4) = D(5); D(5) = temporary; break;
    case 0xC30642: A(4) -= (uint32_t)(int32_t)(int16_t)A(1); break;
    case 0xC30644: step_compare_word(A(4), D(4)); break;
    case 0xC30646: step_branch(pc, opcode, COND_GT()); break;
    case 0xC30648: A(4) = (uint32_t)(int32_t)(int16_t)D(4); break;
    case 0xC30652: step_subtract_word(&D(2), D(4)); break;
    case 0xC30654: step_branch(pc, opcode, COND_GE()); break;
    case 0xC30656:
        value = m68ki_read_imm_16() & 31u; FLAG_Z = D(1) & (1u << value);
        D(1) |= 1u << value; USE_CYCLES(-(1 << CYC_SHIFT)); break;
    case 0xC3065E: step_subtract_word(&D(5), D(4)); break;
    case 0xC30662: polygon_asl_word(&D(4), 6); break;
    case 0xC30664: step_add_word(&D(4), m68ki_read_imm_16()); break;
    case 0xC30668: D(0) = 0xFFFFFFFFu; flags_logic_l(D(0)); break;
    case 0xC3066A:
        value = m68ki_read_imm_16(); address = step_displacement(A(0));
        FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC30670: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC30672: case 0xC30674: break;
    case 0xC30678: case 0xC3067C: case 0xC30680: case 0xC30690:
    case 0xC306A0: case 0xC306AA: case 0xC306AE:
        step_write_word(step_displacement(A(0)), D(opcode & 7u));
        flags_logic_w(D(opcode & 7u)); break;
    case 0xC30684: case 0xC3068A: case 0xC306A4:
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC30694: case 0xC30698: case 0xC3069C:
        step_write_long(step_displacement(A(0)), D(opcode & 7u));
        flags_logic_l(D(opcode & 7u)); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C30466_step(void) { return REG_PC >= 0xC30466 && REG_PC < 0xC304FA ? polygon_lane_instruction() : 0; }
int glue_C304B2_step(void) { return REG_PC >= 0xC304B2 && REG_PC < 0xC304FA ? polygon_lane_instruction() : 0; }
int glue_C305AA_step(void) { return REG_PC >= 0xC305AA && REG_PC < 0xC306B4 ? polygon_edge_instruction() : 0; }
