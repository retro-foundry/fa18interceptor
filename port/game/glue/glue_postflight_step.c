/* Resumable source timing for the postflight dispatcher and both variants.
 *
 * postflight_variants.c remains the readable description of the operation.
 * This bridge preserves each original instruction boundary across the shared
 * $C31392 tail, child drawing calls, interrupts and frame ends. */
#include "glue_step.h"

static void postflight_jsr(uint32_t address) {
    m68ki_push_32(REG_PC);
    m68ki_jump(address);
}

static void postflight_bsr(uint32_t pc) {
    int16_t displacement = (int16_t)m68ki_read_imm_16();
    m68ki_push_32(REG_PC);
    REG_PC = pc + 2 + displacement;
}

static void postflight_bit_register(uint32_t *reg, unsigned bit, int set) {
    uint32_t mask = 1u << (bit & 31u);
    FLAG_Z = *reg & mask;
    if (set > 0) *reg |= mask;
    else if (set < 0) *reg &= ~mask;
    if (set) USE_CYCLES(-(1 << CYC_SHIFT));
}

static void postflight_bit_memory(uint32_t address, unsigned bit, int set) {
    uint8_t value = m68k_read_memory_8(address);
    uint8_t mask = (uint8_t)(1u << (bit & 7u));
    FLAG_Z = value & mask;
    if (set > 0) value |= mask;
    else if (set < 0) value &= (uint8_t)~mask;
    if (set) m68k_write_memory_8(address, value);
}

static void postflight_negate_long(uint32_t *reg) {
    uint32_t old = *reg, result = 0u - old;
    *reg = result;
    FLAG_N = NFLAG_32(result); FLAG_Z = result;
    FLAG_V = old == 0x80000000u ? VFLAG_SET : VFLAG_CLEAR;
    FLAG_X = FLAG_C = old ? CFLAG_SET : CFLAG_CLEAR;
}

static void postflight_add_byte_memory(uint32_t address, uint8_t amount) {
    uint8_t old = m68k_read_memory_8(address);
    uint32_t result = (uint32_t)old + amount;
    m68k_write_memory_8(address, result);
    FLAG_N = NFLAG_8(result); FLAG_Z = result & 0xFFu;
    FLAG_V = VFLAG_ADD_8(amount, old, result);
    FLAG_X = FLAG_C = CFLAG_8(result);
}

static void postflight_sub_byte_memory(uint32_t address, uint8_t amount) {
    uint8_t old = m68k_read_memory_8(address);
    uint32_t result = (uint32_t)old - amount;
    m68k_write_memory_8(address, result);
    step_compare_byte(amount, old); FLAG_X = FLAG_C;
}

static void postflight_or_long_memory(uint32_t address, uint32_t value) {
    value |= m68k_read_memory_32(address);
    step_write_long(address, value); flags_logic_l(value);
}

static void postflight_asr_word(uint32_t *reg, unsigned count) {
    uint16_t old = (uint16_t)*reg;
    uint16_t result = (uint16_t)((int16_t)old >> count);
    SET_W(*reg, result); flags_logic_w(result);
    FLAG_X = FLAG_C = ((old >> (count - 1)) & 1u) << 8;
    USE_CYCLES(count << CYC_SHIFT);
}

static void postflight_movem_words_a0(void) {
    unsigned i;
    (void)m68ki_read_imm_16();
    for (i = 0; i < 4; ++i) {
        D(i) = (uint32_t)(int32_t)(int16_t)m68k_read_memory_16(A(0));
        A(0) += 2;
    }
    USE_CYCLES(4 * 4);
}

static void postflight_save_draw_registers(void) {
    (void)m68ki_read_imm_16();
    step_predecrement_long(A(2));
    step_predecrement_long(A(1));
    step_predecrement_long(A(0));
    step_predecrement_long(D(6));
    USE_CYCLES(4 << CYC_MOVEM_L);
}

static void postflight_restore_draw_registers(void) {
    (void)m68ki_read_imm_16();
    D(6) = m68k_read_memory_32(A(7)); A(7) += 4;
    A(0) = m68k_read_memory_32(A(7)); A(7) += 4;
    A(1) = m68k_read_memory_32(A(7)); A(7) += 4;
    A(2) = m68k_read_memory_32(A(7)); A(7) += 4;
    USE_CYCLES(4 << CYC_MOVEM_L);
}

static int postflight_instruction(void) {
    uint32_t pc = REG_PC, address, value, old;
    uint16_t opcode = step_begin(pc), word;
    switch (pc) {
    case 0xC31224: m68ki_jump(m68ki_pull_32()); break;
    case 0xC31226: value = m68ki_read_imm_32(); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC31230: D(1) = m68ki_read_imm_32(); flags_logic_l(D(1)); break;
    case 0xC31236: A(4) = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC3123A: value = m68ki_read_imm_16(); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC3123E: step_add_long(&D(1), m68k_read_memory_32(m68ki_read_imm_32())); break;
    case 0xC31244: postflight_jsr(m68ki_read_imm_32()); break;
    case 0xC3124A: step_branch(pc, opcode, COND_LT()); break;
    case 0xC3124C: value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); step_compare_byte(value, m68k_read_memory_8(address)); break;
    case 0xC31254: step_branch(pc, opcode, COND_GT()); break;
    case 0xC31256: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC31258: value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC3125E: value = m68ki_read_imm_16(); SET_W(D(0), D(0) & value); flags_logic_w(D(0)); break;
    case 0xC31262: case 0xC31268: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC31264: step_compare_word(m68ki_read_imm_16(), D(0)); break;
    case 0xC3126C: case 0xC3130E: step_branch(pc, opcode, 1); break;
    case 0xC31270: case 0xC31278:
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC31280: case 0xC31284: postflight_bsr(pc); break;
    case 0xC31288: case 0xC318F4: m68ki_jump(m68ki_pull_32()); break;

    /* Tuple and fixed drawing heads. */
    case 0xC3129A: value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC312A2: A(0) = step_displacement(REG_PC); break;
    case 0xC312A6: case 0xC312DC: postflight_movem_words_a0(); break;
    case 0xC312AA: case 0xC312B8: case 0xC312C6: case 0xC312CC:
    case 0xC312E0: case 0xC312EE: case 0xC312FC: case 0xC31302:
    case 0xC31316: case 0xC31328: case 0xC31340: case 0xC31352:
    case 0xC31362: case 0xC3136C: case 0xC3137C: case 0xC31386:
        step_add_word(&D((opcode >> 9) & 7u), m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC312B0: case 0xC312BE: case 0xC312E6: case 0xC312F4:
    case 0xC3131C: case 0xC31346: step_branch(pc, opcode, COND_LT()); break;
    case 0xC312B2: case 0xC312C0: case 0xC312E8: case 0xC312F6:
    case 0xC3131E: case 0xC31348: step_compare_word(m68ki_read_imm_16(), D(opcode & 7u)); break;
    case 0xC312B6: case 0xC312C4: case 0xC312EC: case 0xC312FA:
    case 0xC31322: case 0xC3134C: step_branch(pc, opcode, COND_GE()); break;
    case 0xC312D2: step_predecrement_long(A(0)); flags_logic_l(A(0)); break;
    case 0xC312D4: case 0xC31308: case 0xC31336: case 0xC31358:
    case 0xC31372: case 0xC3138C: postflight_jsr(m68ki_read_imm_32()); break;
    case 0xC312DA: A(0) = m68ki_pull_32(); break;
    case 0xC31312: case 0xC31324: case 0xC3133C: case 0xC3134E:
    case 0xC3135E: case 0xC31368: case 0xC31378: case 0xC31382:
        value = m68ki_read_imm_16(); SET_W(D((opcode >> 9) & 7u), value); flags_logic_w(value); break;
    case 0xC3132E: value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;

    /* Shared prefix and vector fetch. */
    case 0xC31392: flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC31398: case 0xC313A8: case 0xC3140A: case 0xC3141A:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC3139C: case 0xC313F6: case 0xC313FE: A((opcode >> 9) & 7u) = m68ki_read_imm_32(); break;
    case 0xC313A2: case 0xC31404: flags_logic_w(m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC313AA: case 0xC3140C: A(2) += (int16_t)m68ki_read_imm_16(); break;
    case 0xC313AE: value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC313B6: case 0xC313E8: case 0xC313FC:
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)opcode;
        flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC313B8: word = m68k_read_memory_16(A(2)); A(2) += 2; SET_W(D(0), word); flags_logic_w(word); break;
    case 0xC313BA: step_branch(pc, opcode, COND_GE()); break;
    case 0xC313BC: step_compare_word(m68ki_read_imm_16(), D(0)); break;
    case 0xC313C0: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC313C2: value = m68ki_read_imm_16(); postflight_bit_register(&D(0), value, -1); break;
    case 0xC313C6: case 0xC313D4: word = m68k_read_memory_16(A(2)); A(2) += 2; SET_W(D(1), word); flags_logic_w(word); break;
    case 0xC313C8: case 0xC313D6: step_predecrement_long(A(2)); flags_logic_l(A(2)); break;
    case 0xC313CA: case 0xC313D8: A(7) -= 2; step_write_word(A(7), D(2)); flags_logic_w(D(2)); break;
    case 0xC313CC: case 0xC313DA: postflight_jsr(m68ki_read_imm_32()); break;
    case 0xC313D2: step_branch(pc, opcode, 1); break;
    case 0xC313E0: word = m68k_read_memory_16(A(7)); A(7) += 2; SET_W(D(2), word); flags_logic_w(word); break;
    case 0xC313E2: A(2) = m68ki_pull_32(); break;
    case 0xC313E4: step_dbf(pc, &D(2)); break;
    case 0xC313EA: postflight_add_byte_memory(m68ki_read_imm_32(), 1); break;
    case 0xC313F0: address = m68ki_read_imm_32(); m68k_write_memory_8(address, 0); flags_logic_b(0); break;
    case 0xC31410:
        (void)m68ki_read_imm_16();
        D(0) = m68k_read_memory_32(A(0)); A(0) += 4;
        D(1) = m68k_read_memory_32(A(0)); A(0) += 4;
        D(2) = m68k_read_memory_32(A(0)); A(0) += 4;
        USE_CYCLES(3 << CYC_MOVEM_L); break;
    case 0xC31414: D(3) = D(0); flags_logic_l(D(3)); break;
    case 0xC31416: D(3) |= D(1); flags_logic_l(D(3)); break;
    case 0xC31418: D(3) |= D(2); flags_logic_l(D(3)); break;

    /* Record selection and classification. */
    case 0xC3141E: value = D(1); D(1) = D(2); D(2) = value; break;
    case 0xC31420: D(5) = D(0); flags_logic_l(D(5)); break;
    case 0xC31422: A(5) = D(1); break;
    case 0xC31424: value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC31428: step_asr_long(&D(0), D(4)); break;
    case 0xC3142A: case 0xC31430: case 0xC315DC: case 0xC315E2:
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC3142C: case 0xC31432: case 0xC315DE: case 0xC315E4:
        step_add_long(&D(opcode & 7u), 1); break;
    case 0xC3142E: step_asr_long(&D(1), D(4)); break;
    case 0xC31434: value = m68k_read_memory_16(A(0)); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC31436: case 0xC3148A: case 0xC315B0: case 0xC31518:
    case 0xC3152E: case 0xC31546: case 0xC31550:
        value = m68ki_read_imm_16();
        if (pc == 0xC31436 || pc == 0xC3148A || pc == 0xC315B0)
            postflight_bit_register(&D(3), value, 0);
        else postflight_bit_memory(step_displacement(A(1)), value, 0);
        break;
    case 0xC3143A: case 0xC3145C: case 0xC3148E: case 0xC3154C:
    case 0xC31556: case 0xC315B4: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC3143E: SET_W(D(4), D(3)); flags_logic_w(D(4)); break;
    case 0xC31440: value = m68ki_read_imm_16(); SET_W(D(4), D(4) & value); flags_logic_w(D(4)); break;
    case 0xC31444: step_add_word(&D(4), D(4)); break;
    case 0xC31446: A(1) = m68ki_read_imm_32(); break;
    case 0xC3144C: A(1) += (int32_t)(int16_t)D(4); break;
    case 0xC3144E: step_compare_word(m68k_read_memory_16(m68ki_read_imm_32()), D(4)); break;
    case 0xC31454: case 0xC314A6: case 0xC314AC: case 0xC314E2:
    case 0xC31502: case 0xC3153C: case 0xC31568:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC31456: case 0xC3145E:
        value = m68ki_read_imm_16(); postflight_bit_memory(step_displacement(A(1)), value, 0); break;
    case 0xC31464: case 0xC3151E: case 0xC3152A: case 0xC31534:
    case 0xC31544: case 0xC3158C: case 0xC31594:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC31466: case 0xC31478: case 0xC315E6: case 0xC315FC:
        postflight_negate_long(&D(opcode & 7u)); break;
    case 0xC31468: case 0xC31470: case 0xC3147A: case 0xC31482:
    case 0xC314B6: case 0xC314C6: case 0xC315E8: case 0xC315F2:
    case 0xC315FE: case 0xC31608:
        step_compare_long(m68ki_read_imm_32(), D(opcode & 7u)); break;
    case 0xC3146E: case 0xC314BC: case 0xC314CC: case 0xC315EE:
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC31476: case 0xC315F8: case 0xC3160E:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC31480: case 0xC31604: step_branch(pc, opcode, COND_GE()); break;
    case 0xC31488: step_branch(pc, opcode, COND_GE()); break;
    case 0xC31492:
        value = m68ki_read_imm_16(); postflight_bit_memory(step_displacement(A(1)), value, -1); break;
    case 0xC31498: case 0xC3150E: case 0xC3154E: case 0xC31560:
    case 0xC31572: case 0xC3157C: case 0xC3159E: case 0xC315A8:
        step_branch(pc, opcode, 1); break;
    case 0xC3149C: value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC314A2: step_compare_byte(m68ki_read_imm_16(), D(1)); break;
    case 0xC314AA: step_subtract_byte(&D(1), 2); break;
    case 0xC314B0: D(1) = D(5); flags_logic_l(D(1)); break;
    case 0xC314B2: case 0xC314C2: step_branch(pc, opcode, COND_GE()); break;
    case 0xC314B4: case 0xC314C4: postflight_negate_long(&D(1)); break;
    case 0xC314C0: D(1) = A(5); flags_logic_l(D(1)); break;
    case 0xC314D0: case 0xC31558: case 0xC3156A: case 0xC31574:
    case 0xC31596: case 0xC315A0: case 0xC315B6:
        value = m68ki_read_imm_16();
        if (pc == 0xC31558 || pc == 0xC3156A || pc == 0xC31574 ||
            pc == 0xC31596 || pc == 0xC315A0)
            postflight_bit_memory(m68ki_read_imm_32(), value, 1);
        else postflight_bit_memory(step_displacement(A(1)), value, 1);
        break;
    case 0xC314D6: case 0xC3157E:
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC314DA:
        value = m68ki_read_imm_16(); SET_B(D(1), D(1) & value); flags_logic_b(D(1)); break;
    case 0xC314DE: case 0xC314EE: case 0xC31538: case 0xC31540:
        step_compare_byte(m68ki_read_imm_16(), D(1)); break;
    case 0xC314E6: case 0xC314F4:
        flags_logic_w(m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC314EC: case 0xC31504: step_branch(pc, opcode, COND_LT()); break;
    case 0xC314F2: case 0xC314FA: step_branch(pc, opcode, COND_NE()); break;
    case 0xC314FC: flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC31506: postflight_sub_byte_memory(m68ki_read_imm_32(), 1); break;
    case 0xC3150C: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC31510: value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC31522: value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC31526: value = m68ki_read_imm_16(); SET_W(D(0), D(0) & value); flags_logic_w(D(0)); break;
    case 0xC31562: case 0xC3158E:
        value = m68ki_read_imm_16(); address = step_displacement(A(1));
        step_compare_byte(value, m68k_read_memory_8(address)); break;
    case 0xC31582: value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC31586: step_compare_word(m68k_read_memory_16(m68ki_read_imm_32()), D(1)); break;
    case 0xC315AA: value = m68ki_read_imm_16(); postflight_bit_memory(step_displacement(A(1)), value, -1); break;
    case 0xC315BC: D(0) = D(5); flags_logic_l(D(0)); break;
    case 0xC315BE: D(1) = A(5); flags_logic_l(D(1)); break;

    /* Viewed-record normalization and optional point submission. */
    case 0xC315C0: case 0xC31656: step_predecrement_long(A(0)); flags_logic_l(A(0)); break;
    case 0xC315C2: case 0xC31658: A(0) = m68ki_read_imm_32(); break;
    case 0xC315C8: A(0) += (int32_t)(int16_t)m68k_read_memory_16(m68ki_read_imm_32()); break;
    case 0xC315CE: value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC315D2: case 0xC316BE: A(0) = m68ki_pull_32(); break;
    case 0xC315D4: value = m68ki_read_imm_16(); SET_B(D(4), D(4) & value); flags_logic_b(D(4)); break;
    case 0xC315D8: SET_W(D(4), (uint16_t)(int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case 0xC315DA: step_asr_long(&D(0), D(4)); break;
    case 0xC315E0: step_asr_long(&D(1), D(4)); break;
    case 0xC31612: step_add_word(&D(0), m68ki_read_imm_16()); break;
    case 0xC31616: step_add_word(&D(0), m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC3161C: step_branch(pc, opcode, COND_LT()); break;
    case 0xC31620: step_compare_word(m68ki_read_imm_16(), D(0)); break;
    case 0xC31624: step_branch(pc, opcode, COND_GE()); break;
    case 0xC31628: step_add_word(&D(1), m68ki_read_imm_16()); break;
    case 0xC3162C: value = m68ki_read_imm_16(); postflight_bit_register(&D(3), value, 0); break;
    case 0xC31630: case 0xC31652: case 0xC3168A: case 0xC316A0:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC31634: D(5) = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(D(5)); break;
    case 0xC3163A: SET_W(D(4), D(3)); flags_logic_w(D(4)); break;
    case 0xC3163C: value = m68ki_read_imm_16(); SET_W(D(4), D(4) & value); flags_logic_w(D(4)); break;
    case 0xC31640: step_add_word(&D(4), D(4)); break;
    case 0xC31642: step_compare_word(m68k_read_memory_16(m68ki_read_imm_32()), D(4)); break;
    case 0xC31648: case 0xC31692: case 0xC3169A:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC3164A: value = m68ki_read_imm_16(); postflight_bit_memory(m68ki_read_imm_32(), value, 0); break;
    case 0xC3165E: A(0) += (int32_t)(int16_t)D(4); break;
    case 0xC31660: postflight_negate_long(&D(5)); break;
    case 0xC31662: case 0xC3166C:
        value = m68ki_read_imm_16(); postflight_bit_register(&D(7), value, pc == 0xC3166C ? 1 : -1); break;
    case 0xC31666: step_compare_long(m68k_read_memory_32(step_displacement(A(0))), D(5)); break;
    case 0xC3166A: step_branch(pc, opcode, COND_GT()); break;
    case 0xC31670: value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(5), value); flags_logic_b(value); break;
    case 0xC31674: value = m68ki_read_imm_16(); SET_B(D(5), D(5) & value); flags_logic_b(D(5)); break;
    case 0xC31678: case 0xC3167E: case 0xC3169C:
        step_compare_byte(m68ki_read_imm_16(), D(5)); break;
    case 0xC3167C: case 0xC31682: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC31684: case 0xC3168C: case 0xC31694:
        value = m68ki_read_imm_16(); postflight_bit_memory(step_displacement(A(0)), value, 0); break;
    case 0xC316A2: case 0xC316A6: case 0xC316B6:
        D(2) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(2)); break;
    case 0xC316A4: case 0xC316A8: case 0xC316AE: case 0xC316B4:
        step_branch(pc, opcode, 1); break;
    case 0xC316AA: case 0xC316B0:
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC316B8: step_write_word(m68ki_read_imm_32(), D(2)); flags_logic_w(D(2)); break;

    /* Point-table capacity, drawing call and vector advance. */
    case 0xC316C0: flags_logic_w(m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC316C6: case 0xC316E4: step_branch(pc, opcode, COND_NE()); break;
    case 0xC316C8: case 0xC316D0: step_compare_long(m68ki_read_imm_32(), A(2)); break;
    case 0xC316CE: step_branch(pc, opcode, 1); break;
    case 0xC316D6: step_branch(pc, opcode, COND_GE()); break;
    case 0xC316D8: A(7) -= 2; step_write_word(A(7), D(3)); flags_logic_w(D(3)); break;
    case 0xC316DA: step_add_word(&D(1), m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC316E0: value = m68ki_read_imm_16(); postflight_bit_register(&D(7), value, 0); break;
    case 0xC316E6: step_write_word(A(2), D(0)); A(2) += 2; flags_logic_w(D(0)); break;
    case 0xC316E8: case 0xC316FC: step_write_word(A(2), D(1)); A(2) += 2; flags_logic_w(D(1)); break;
    case 0xC316EA: case 0xC316FE: postflight_save_draw_registers(); break;
    case 0xC316EE: case 0xC31702: postflight_jsr(m68ki_read_imm_32()); break;
    case 0xC316F4: step_branch(pc, opcode, 1); break;
    case 0xC316F6: step_write_word(A(2), D(0)); flags_logic_w(D(0)); break;
    case 0xC316F8:
        word = m68k_read_memory_16(A(2)); word |= m68ki_read_imm_16();
        step_write_word(A(2), word); A(2) += 2; flags_logic_w(word); break;
    case 0xC31708: postflight_restore_draw_registers(); break;
    case 0xC3170C: word = m68k_read_memory_16(A(7)); A(7) += 2; SET_W(D(3), word); flags_logic_w(word); break;
    case 0xC3170E: case 0xC31714:
        value = m68ki_read_imm_16(); postflight_bit_register(&D(3), value, pc == 0xC3170E ? 1 : -1); break;
    case 0xC31712: case 0xC3171E: step_branch(pc, opcode, 1); break;
    case 0xC31718: step_write_word(A(0), D(3)); A(0) += 2; flags_logic_w(D(3)); break;
    case 0xC3171A: A(0) += 2; break;
    case 0xC3171C: step_add_word(&D(6), 1); break;

    /* Status transition events. */
    case 0xC31722: value = m68ki_read_imm_16(); step_write_word(A(2), value); flags_logic_w(value); break;
    case 0xC31726: value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC3172C: case 0xC31740: case 0xC31772: case 0xC3178A:
    case 0xC31790: case 0xC3179A: case 0xC317BA: case 0xC317D2:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC31730: value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC31736: case 0xC3173C: case 0xC31742: case 0xC31758:
    case 0xC3176E: case 0xC31774: case 0xC31786: case 0xC3178C:
    case 0xC31796: case 0xC3179C: case 0xC317B6: case 0xC317BC:
    case 0xC317CE: case 0xC317DE:
        value = m68ki_read_imm_16(); postflight_bit_register(&D(opcode & 7u), value, 0); break;
    case 0xC3173A: case 0xC31746: case 0xC3175C: case 0xC31778:
    case 0xC317A0: case 0xC317C0: case 0xC317E2:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC3174A: case 0xC31760: case 0xC3177A: case 0xC317A2:
    case 0xC317E4:
        value = m68ki_read_imm_32(); postflight_or_long_memory(m68ki_read_imm_32(), value); break;
    case 0xC31754: case 0xC3176A: case 0xC31784: case 0xC31792:
    case 0xC317B4: case 0xC317CC: case 0xC317EE: step_branch(pc, opcode, 1); break;
    case 0xC317AC: value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC317C2: case 0xC317D4:
        value = m68ki_read_imm_16(); step_compare_byte(value, m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC317CA: case 0xC317DC: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC317F0: value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC317F6: value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC317FA: step_compare_word(m68ki_read_imm_16(), D(1)); break;
    case 0xC317FE: step_branch(pc, opcode, COND_NE()); break;
    case 0xC31800: address = m68ki_read_imm_32(); step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC31806: address = m68ki_read_imm_32(); m68k_write_memory_8(address, D(0)); flags_logic_b(D(0)); break;

    /* Optional selected-record scan. */
    case 0xC3180C: flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC31812: case 0xC31874: case 0xC3187A: case 0xC31894:
    case 0xC3189A: case 0xC318A0: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC31816: address = m68ki_read_imm_32(); m68k_write_memory_8(address, 0); flags_logic_b(0); break;
    case 0xC3181C: case 0xC3187C: A((opcode >> 9) & 7u) = m68ki_read_imm_32(); break;
    case 0xC31822: case 0xC31856: D(0) = 0; flags_logic_l(0); break;
    case 0xC31824: flags_logic_w(m68k_read_memory_16(m68ki_read_imm_32())); break;
    case 0xC3182A: step_branch(pc, opcode, COND_LT()); break;
    case 0xC3182C: case 0xC3186C:
        value = m68k_read_memory_16(step_indexed(A(0)));
        SET_W(D(pc == 0xC3182C ? 1 : 2), value); flags_logic_w(value); break;
    case 0xC31830: value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC31834: step_add_word(&D(1), D(1)); break;
    case 0xC31836: step_compare_word(m68k_read_memory_16(m68ki_read_imm_32()), D(1)); break;
    case 0xC3183C: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC3183E: case 0xC3185A: case 0xC318DC: step_add_word(&D(0), m68ki_read_imm_16()); break;
    case 0xC31842: step_compare_word(m68ki_read_imm_16(), D(0)); break;
    case 0xC31846: step_branch(pc, opcode, COND_LT()); break;
    case 0xC31848: value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC31850: postflight_jsr(m68ki_read_imm_32()); break;
    case 0xC31858: case 0xC318DA: case 0xC318E0: step_branch(pc, opcode, 1); break;
    case 0xC3185E: D(5) = m68k_read_memory_32(step_indexed(A(0))); flags_logic_l(D(5)); break;
    case 0xC31862: case 0xC31866: D(5) |= m68k_read_memory_32(step_indexed(A(0))); flags_logic_l(D(5)); break;
    case 0xC3186A: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC31870: case 0xC31876:
        value = m68ki_read_imm_16();
        if (pc == 0xC31870 || pc == 0xC31876) postflight_bit_register(&D(2), value, 0);
        else postflight_bit_memory(step_indexed(A(2)), value, 0);
        break;
    case 0xC31882: value = m68ki_read_imm_16(); SET_W(D(2), D(2) & value); flags_logic_w(D(2)); break;
    case 0xC31886: step_add_word(&D(2), D(2)); break;
    case 0xC31888: value = m68k_read_memory_8(step_indexed(A(2))); SET_B(D(5), value); flags_logic_b(value); break;
    case 0xC3188C: value = m68ki_read_imm_16(); SET_B(D(5), D(5) & value); flags_logic_b(D(5)); break;
    case 0xC31890: case 0xC31896: case 0xC3189C: step_compare_byte(m68ki_read_imm_16(), D(5)); break;
    case 0xC318A2: value = m68ki_read_imm_16(); postflight_bit_memory(step_indexed(A(2)), value, 0); break;
    case 0xC318A8: step_branch(pc, opcode, COND_NE()); break;
    case 0xC318AA: step_write_word(m68ki_read_imm_32(), D(2)); flags_logic_w(D(2)); break;
    case 0xC318B0: value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC318B8: case 0xC318C0: case 0xC318E4:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); m68k_write_memory_8(address, value); flags_logic_b(value); break;
    case 0xC318C8: postflight_asr_word(&D(0), 4); break;
    case 0xC318CA: {
        uint8_t prior = (uint8_t)D(0); uint32_t result = (uint32_t)prior + 1;
        SET_B(D(0), result); FLAG_N = NFLAG_8(result); FLAG_Z = result & 0xFFu;
        FLAG_V = VFLAG_ADD_8(1u, prior, result); FLAG_X = FLAG_C = CFLAG_8(result); break;
    }
    case 0xC318CC: address = m68ki_read_imm_32(); m68k_write_memory_8(address, D(0)); flags_logic_b(D(0)); break;
    case 0xC318D2:
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        word = m68k_read_memory_16(address) & value; step_write_word(address, word); flags_logic_w(word); break;
    case 0xC318EC: value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;

    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C31226_step(void) { return REG_PC >= 0xC31224 && REG_PC < 0xC318F6 ? postflight_instruction() : 0; }
int glue_C3129A_step(void) { return glue_C31226_step(); }
int glue_C31312_step(void) { return glue_C31226_step(); }
