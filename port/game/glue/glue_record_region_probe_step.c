/* Resumable source timing for $C2B042-$C2B3B3. The readable directory
 * crossing and placed-polygon operations live in record_region_probe.c.
 * Keep the source's unusual D2/D4-D6 restore and fault-call prefix. */
#include "glue_step.h"

static void region_load_words(uint32_t address, uint16_t mask) {
    unsigned i, count = 0;
    for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
        REG_DA[i] = (uint32_t)(int32_t)(int16_t)m68k_read_memory_16(address);
        address += 2; ++count;
    }
    USE_CYCLES(count << CYC_MOVEM_W);
}

static void region_negate_long(uint32_t *reg) {
    uint32_t old = *reg;
    *reg = 0; step_subtract_long(reg, old);
}

static void region_negate_word(uint32_t *reg) {
    uint16_t old = (uint16_t)*reg;
    SET_W(*reg, 0); step_subtract_word(reg, old);
}

static void region_lsl_word(uint32_t *reg, unsigned count) {
    uint16_t old = (uint16_t)*reg, result = (uint16_t)(old << count);
    SET_W(*reg, result); flags_logic_w(result);
    FLAG_X = FLAG_C = ((old >> (16 - count)) & 1u) << 8;
    USE_CYCLES(count << CYC_SHIFT);
}

static void region_multiply_signed(uint32_t *reg, uint16_t source) {
    uint32_t bits = (uint32_t)source << 1;
    unsigned i, transitions = 0;
    for (i = 0; i < 16; ++i, bits >>= 1)
        if ((bits & 3u) == 1u || (bits & 3u) == 2u) ++transitions;
    USE_CYCLES(2 * transitions);
    *reg = (uint32_t)((int32_t)(int16_t)*reg * (int32_t)(int16_t)source);
    flags_logic_l(*reg);
}

static void region_divide_signed(uint32_t *reg, int16_t divisor) {
    int32_t dividend = (int32_t)*reg, quotient, remainder;
    uint32_t absolute_dividend, absolute_divisor, magnitude;
    int cycles = 6, i;
    if (!divisor) { m68ki_exception_trap(EXCEPTION_ZERO_DIVIDE); return; }
    absolute_dividend = dividend < 0 ? 0u - (uint32_t)dividend : (uint32_t)dividend;
    absolute_divisor = divisor < 0 ? (uint32_t)-divisor : (uint32_t)divisor;
    if (dividend < 0) ++cycles;
    if ((absolute_dividend >> 16) >= absolute_divisor) cycles = (cycles + 2) * 2;
    else {
        magnitude = absolute_dividend / absolute_divisor;
        cycles += 55;
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
        FLAG_Z = (uint16_t)quotient; FLAG_N = NFLAG_16(quotient);
        FLAG_V = FLAG_C = 0;
        *reg = ((uint32_t)(uint16_t)remainder << 16) | (uint16_t)quotient;
    } else FLAG_V = VFLAG_SET;
}

int glue_C2B05A_step(void) {
    uint32_t pc = REG_PC, address, value, temporary;
    uint16_t opcode, mask;
    if (pc < 0xC2B042u || pc >= 0xC2B3B4u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC2B042:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC2B04A:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC2B050: case 0xC2B244: case 0xC2B3A4: case 0xC2B3AC:
        value = m68ki_read_imm_16() & 7u; address = step_displacement(A(1));
        temporary = m68k_read_memory_8(address); FLAG_Z = temporary & (1u << value);
        if (pc == 0xC2B050 || pc == 0xC2B3AC) temporary &= ~(1u << value);
        else temporary |= 1u << value;
        m68k_write_memory_8(address, temporary); break;
    case 0xC2B05A: case 0xC2B07E: case 0xC2B24A: case 0xC2B250: case 0xC2B27C:
        A((opcode >> 9) & 7u) = m68ki_read_imm_32(); break;
    case 0xC2B060: A(1) += (uint32_t)(int32_t)(int16_t)m68k_read_memory_16(m68ki_read_imm_32()); break;
    case 0xC2B066: case 0xC2B06A: case 0xC2B0A8: case 0xC2B1CE:
    case 0xC2B1E8: case 0xC2B20C:
        value = m68k_read_memory_32(step_displacement(A(1)));
        D((opcode >> 9) & 7u) = value; flags_logic_l(value); break;
    case 0xC2B06E: case 0xC2B0DE: case 0xC2B106: case 0xC2B10C:
    case 0xC2B11A: case 0xC2B11E: case 0xC2B12E: case 0xC2B132:
    case 0xC2B142: case 0xC2B146: case 0xC2B154: case 0xC2B158:
    case 0xC2B166: case 0xC2B16A: case 0xC2B178: case 0xC2B17C:
    case 0xC2B18A: case 0xC2B18E: case 0xC2B196: case 0xC2B198:
    case 0xC2B1A0: case 0xC2B2A0: case 0xC2B2B2:
        value = REG_DA[opcode & 15u]; D((opcode >> 9) & 7u) = value; flags_logic_l(value); break;
    case 0xC2B070: case 0xC2B0AC: case 0xC2B1D2: case 0xC2B1EC: case 0xC2B210:
        value = m68ki_read_imm_32(); D(opcode & 7u) &= value; flags_logic_l(D(opcode & 7u)); break;
    case 0xC2B076: case 0xC2B078: case 0xC2B270: case 0xC2B274:
        step_swap(&D(opcode & 7u)); break;
    case 0xC2B07A: case 0xC2B07C:
        SET_W(D(opcode & 7u), step_lsr_word_value(D(opcode & 7u), 8)); break;
    case 0xC2B084: case 0xC2B088: case 0xC2B278: case 0xC2B27A:
        step_add_word(&D((opcode >> 9) & 7u), D(opcode & 7u)); break;
    case 0xC2B086: region_lsl_word(&D(7), 6); break;
    case 0xC2B08A:
        value = m68k_read_memory_16(step_indexed(A(3))); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2B090: A(3) += (uint32_t)(int32_t)(int16_t)D(7); break;
    case 0xC2B092: flags_logic_w(m68k_read_memory_16(A(3))); break;
    case 0xC2B096: A(3) += 4; break;
    case 0xC2B098: case 0xC2B0A2:
        value = m68k_read_memory_16(A(3)); A(3) += 2; SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2B09C: case 0xC2B0C0: case 0xC2B264:
        value = m68ki_read_imm_16(); step_compare_word(value, D(opcode & 7u)); break;
    case 0xC2B0A4: A(4) = A(3); break;
    case 0xC2B0A6: A(5) = 0; break;
    case 0xC2B0B2: case 0xC2B0C6: case 0xC2B2E6: case 0xC2B36C:
        mask = m68ki_read_imm_16(); region_load_words(A(opcode & 7u), mask); break;
    case 0xC2B25E: case 0xC2B282:
        mask = m68ki_read_imm_16(); region_load_words(step_indexed(A(opcode & 7u)), mask); break;
    case 0xC2B2D8: case 0xC2B308: case 0xC2B318: case 0xC2B33A: case 0xC2B34A: case 0xC2B37A:
        mask = m68ki_read_imm_16(); region_load_words(step_displacement(A(4)), mask); break;
    case 0xC2B0B6: A(3) += 4; break;
    case 0xC2B0B8: case 0xC2B0BA: case 0xC2B0BC: case 0xC2B0BE:
    case 0xC2B0CA: case 0xC2B0CC: case 0xC2B0CE: case 0xC2B0D0:
    case 0xC2B11C: case 0xC2B130: case 0xC2B144: case 0xC2B156:
    case 0xC2B168: case 0xC2B17A: case 0xC2B18C: case 0xC2B272:
    case 0xC2B276: case 0xC2B288: case 0xC2B28A: case 0xC2B28C:
    case 0xC2B28E: case 0xC2B294: case 0xC2B296: case 0xC2B298:
    case 0xC2B29A: case 0xC2B2EA: case 0xC2B2EC: case 0xC2B31E:
    case 0xC2B320: case 0xC2B350: case 0xC2B352: case 0xC2B380: case 0xC2B382:
        value = (opcode >> 9) & 7u; step_asl_long(&D(opcode & 7u), value ? value : 8); break;
    case 0xC2B0D2: step_save_registers(); break;
    case 0xC2B1CA: step_restore_registers(); break;
    case 0xC2B0D6: case 0xC2B0D8: case 0xC2B19E: case 0xC2B1D8: case 0xC2B1F2:
        step_subtract_long(&D((opcode >> 9) & 7u), D(opcode & 7u)); break;
    case 0xC2B0DA: A(0) = D(4); break;
    case 0xC2B2D0: A(4) = D(6); break;
    case 0xC2B0DC: case 0xC2B0F0: case 0xC2B0F6: case 0xC2B0F8:
    case 0xC2B0FC: case 0xC2B120: case 0xC2B122: case 0xC2B128:
    case 0xC2B134: case 0xC2B136: case 0xC2B13C: case 0xC2B148:
    case 0xC2B15A: case 0xC2B16C: case 0xC2B17E: case 0xC2B190:
    case 0xC2B19A: case 0xC2B1AE: case 0xC2B1B0: case 0xC2B2B8:
    case 0xC2B2FA: case 0xC2B2FC: case 0xC2B32E: case 0xC2B330:
    case 0xC2B360: case 0xC2B362: case 0xC2B390: case 0xC2B392:
        value = (opcode >> 9) & 7u; step_asr_long(&D(opcode & 7u), value ? value : 8); break;
    case 0xC2B0E6: case 0xC2B10A: case 0xC2B110: case 0xC2B1A4:
    case 0xC2B1DC: case 0xC2B1F6: case 0xC2B2A8: case 0xC2B2BC:
        region_negate_long(&D(opcode & 7u)); break;
    case 0xC2B0E8: case 0xC2B1A6: case 0xC2B1DE: case 0xC2B1F8: case 0xC2B2AA: case 0xC2B2BE:
        value = m68ki_read_imm_32(); step_compare_long(value, D(opcode & 7u)); break;
    case 0xC2B0FA: case 0xC2B0FE: case 0xC2B104:
        address = 8u + (opcode & 7u); value = (opcode >> 9) & 7u;
        temporary = D(value); D(value) = REG_DA[address]; REG_DA[address] = temporary; break;
    case 0xC2B20A: case 0xC2B222:
        value = (opcode >> 9) & 7u; address = opcode & 7u;
        temporary = D(value); D(value) = D(address); D(address) = temporary; break;
    case 0xC2B102: case 0xC2B256:
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)opcode;
        flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC2B112: case 0xC2B114: case 0xC2B14E: case 0xC2B160:
    case 0xC2B172: case 0xC2B184: case 0xC2B1C4: case 0xC2B290:
    case 0xC2B292: case 0xC2B29C: case 0xC2B29E: case 0xC2B2EE:
    case 0xC2B2F0: case 0xC2B302: case 0xC2B322: case 0xC2B324:
    case 0xC2B336: case 0xC2B354: case 0xC2B356: case 0xC2B368:
    case 0xC2B384: case 0xC2B386: case 0xC2B398:
        step_add_long(&D((opcode >> 9) & 7u), D(opcode & 7u)); break;
    case 0xC2B116: case 0xC2B12A: case 0xC2B13E: case 0xC2B150:
    case 0xC2B162: case 0xC2B174: case 0xC2B186: case 0xC2B1C6:
    case 0xC2B202: case 0xC2B206: case 0xC2B216: case 0xC2B21A:
    case 0xC2B224: case 0xC2B228:
        step_compare_long(D(opcode & 7u), D((opcode >> 9) & 7u)); break;
    case 0xC2B124: case 0xC2B138: case 0xC2B14A: case 0xC2B15C:
    case 0xC2B16E: case 0xC2B180: case 0xC2B192: case 0xC2B234:
        value = (opcode >> 9) & 7u; step_subtract_word(&D(opcode & 7u), value ? value : 8); break;
    case 0xC2B1B2: step_add_word(&D(6), 1); break;
    case 0xC2B19C: region_divide_signed(&D(5), (int16_t)D(4)); break;
    case 0xC2B1B6: case 0xC2B2FE: case 0xC2B300: case 0xC2B332:
    case 0xC2B334: case 0xC2B364: case 0xC2B366: case 0xC2B394: case 0xC2B396:
        region_multiply_signed(&D((opcode >> 9) & 7u), D(opcode & 7u)); break;
    case 0xC2B1B8: case 0xC2B26C: flags_logic_w(D(opcode & 7u)); break;
    case 0xC2B1BC: case 0xC2B2E4: case 0xC2B316: case 0xC2B348: case 0xC2B378:
        region_negate_word(&D(opcode & 7u)); break;
    case 0xC2B1BE: step_asr_long(&D(0), D(6)); break;
    case 0xC2B1C2: step_asl_long(&D(0), D(6)); break;
    case 0xC2B22C: A(5) += 1; break;
    case 0xC2B230: case 0xC2B39C:
        A(opcode >> 9 & 7u) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC2B23A: SET_W(D(0), A(5)); flags_logic_w(D(0)); break;
    case 0xC2B23C: value = m68ki_read_imm_16() & 31u; FLAG_Z = D(0) & (1u << value); break;
    case 0xC2B25A: step_add_word(&D(0), m68ki_read_imm_16()); break;
    case 0xC2B2A2: case 0xC2B2B4: case 0xC2B2F2: case 0xC2B2F6:
    case 0xC2B326: case 0xC2B32A: case 0xC2B358: case 0xC2B35C:
    case 0xC2B388: case 0xC2B38C:
        value = m68k_read_memory_32(step_displacement(A(1)));
        step_subtract_long(&D((opcode >> 9) & 7u), value); break;
    case 0xC2B2C6:
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2B2CA:
        D(6) = m68k_read_memory_32(step_indexed(A(3))); flags_logic_l(D(6)); break;
    case 0xC2B2D2: value = m68ki_read_imm_16(); step_compare_word(value, m68k_read_memory_16(A(4))); break;
    case 0xC2B2DE: step_subtract_word(&D(5), m68k_read_memory_16(A(4))); break;
    case 0xC2B2E0: case 0xC2B30E: case 0xC2B312: case 0xC2B340:
    case 0xC2B344: case 0xC2B370: case 0xC2B374:
        value = m68k_read_memory_16(step_displacement(A(4)));
        step_subtract_word(&D((opcode >> 9) & 7u), value); break;
    case 0xC2B3AA: case 0xC2B3B2: m68ki_jump(m68ki_pull_32()); break;
    case 0xC2B056: case 0xC2B100: case 0xC2B126: case 0xC2B13A:
    case 0xC2B14C: case 0xC2B15E: case 0xC2B170: case 0xC2B182:
    case 0xC2B194: case 0xC2B1B4: case 0xC2B1C0: case 0xC2B21E:
    case 0xC2B22E: case 0xC2B258: case 0xC2B3A0: step_branch(pc, opcode, 1); break;
    case 0xC2B08E: case 0xC2B0EE: case 0xC2B1AC: case 0xC2B218:
    case 0xC2B226: case 0xC2B2CE: step_branch(pc, opcode, COND_LE()); break;
    case 0xC2B094: case 0xC2B118: case 0xC2B12C: case 0xC2B140:
    case 0xC2B152: case 0xC2B164: case 0xC2B176: case 0xC2B188:
    case 0xC2B1C8: case 0xC2B1E4: case 0xC2B1FE: case 0xC2B26E:
    case 0xC2B304: case 0xC2B338: case 0xC2B36A: step_branch(pc, opcode, COND_LT()); break;
    case 0xC2B09A: case 0xC2B0C4: case 0xC2B0E4: case 0xC2B220:
    case 0xC2B236: case 0xC2B2B0: case 0xC2B2C4: step_branch(pc, opcode, COND_GT()); break;
    case 0xC2B0A0: case 0xC2B0E0: case 0xC2B0F2: case 0xC2B240:
    case 0xC2B268: case 0xC2B2D6: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2B108: case 0xC2B10E: case 0xC2B1A2: case 0xC2B1BA:
    case 0xC2B1DA: case 0xC2B1F4: case 0xC2B208: case 0xC2B21C:
    case 0xC2B22A: case 0xC2B2A6: case 0xC2B2BA: case 0xC2B39A:
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2B204: step_branch(pc, opcode, COND_NE()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
