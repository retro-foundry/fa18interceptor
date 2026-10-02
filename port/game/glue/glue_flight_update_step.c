/* Flight-update selection, firing, target/projection and attitude timing.
 * Readable game behavior remains in its domain modules; CPU effects stay here. */
#include "glue_renderer_step_math.h"
#include "glue_unsigned_division_step.h"

static void flight_multiply_unsigned(uint32_t *reg, uint16_t source) {
    uint16_t bits = source;
    unsigned count = 0;
    while (bits) { count += bits & 1u; bits >>= 1; }
    USE_CYCLES(2 * count);
    *reg = (uint32_t)(uint16_t)*reg * source; flags_logic_l(*reg);
}
static void flight_lsr_byte(uint32_t *reg, unsigned count) {
    uint8_t old = (uint8_t)*reg, result;
    count &= 63u; result = count < 8 ? (uint8_t)(old >> count) : 0;
    SET_B(*reg, result); flags_logic_b(result); FLAG_C = 0;
    if (count) FLAG_X = FLAG_C = count <= 8 ? ((old >> (count - 1)) & 1u) << 8 : 0;
    USE_CYCLES(count << CYC_SHIFT);
}

int glue_C230B0_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC122A2: /* link    A6, #-$6 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC122A6: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC122AC: /* beq     $c122c0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC122AE: /* move.w  $c45a94.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC122B4: /* asr.w   #3, D0 */
        renderer_asr_word(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC122B6: /* clr.w   (-$4,A6) */
        step_write_word(step_displacement(A(6)), 0); flags_logic_w(0); break;
    case 0xC122BA: /* move.w  D0, (-$2,A6) */
        value = D(0); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC122BE: /* bra     $c122d8 */
        step_branch(pc, opcode, 1); break;
    case 0xC122C0: /* move.l  $c45a88.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC122C6: /* asr.l   #3, D0 */
        step_asr_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC122C8: /* move.l  $c45a90.l, D1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(1) = value; flags_logic_l(value); break;
    case 0xC122CE: /* asr.l   #3, D1 */
        step_asr_long(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC122D0: /* move.w  D0, (-$2,A6) */
        value = D(0); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC122D4: /* move.w  D1, (-$4,A6) */
        value = D(1); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC122D8: /* move.w  (-$2,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC122DC: /* cmpi.w  #$258, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC122E0: /* ble     $c12316 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC122E2: /* cmpi.w  #$c4e, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC122E6: /* bge     $c12316 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC122E8: /* move.b  $c45786.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC122EE: /* tst.b   D0 */
        value = D(0); flags_logic_b(value); break;
    case 0xC122F0: /* bne     $c1238e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC122F4: /* addq.b  #1, D0 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; renderer_add_byte(&D(0), value); break;
    case 0xC122F6: /* move.b  D0, $c45786.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC122FC: /* move.b  $c45858.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC12302: /* ori.b   #$b, D0 */
        value = m68ki_read_imm_16(); result = D(0); result |= value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC12306: /* move.b  D0, $c45858.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1230C: /* move.b  #$3, $c4586b.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC12314: /* bra     $c1238e */
        step_branch(pc, opcode, 1); break;
    case 0xC12316: /* tst.b   $c45786.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC1231C: /* beq     $c12334 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1231E: /* clr.b   $c45786.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC12324: /* move.b  $c45858.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC1232A: /* ori.b   #$b, D0 */
        value = m68ki_read_imm_16(); result = D(0); result |= value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC1232E: /* move.b  D0, $c45858.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC12334: /* move.w  (-$2,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12338: /* cmpi.w  #$1c2, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC1233C: /* ble     $c1234e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1233E: /* cmpi.w  #$c80, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC12342: /* bge     $c1234e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC12344: /* move.b  #$1, $c4586c.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1234C: /* bra     $c1238e */
        step_branch(pc, opcode, 1); break;
    case 0xC1234E: /* clr.b   $c4586c.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC12354: /* move.w  (-$2,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12358: /* cmpi.w  #$c80, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC1235C: /* bge     $c12388 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1235E: /* cmpi.w  #$12c, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC12362: /* ble     $c1236e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC12364: /* move.b  #$2, $c4586b.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1236C: /* bra     $c1238e */
        step_branch(pc, opcode, 1); break;
    case 0xC1236E: /* cmpi.w  #$64, (-$2,A6) */
        value = m68ki_read_imm_16(); address = step_displacement(A(6)); result = m68k_read_memory_16(address); step_compare_word(value, result); break;
    case 0xC12374: /* ble     $c12380 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC12376: /* move.b  #$1, $c4586b.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1237E: /* bra     $c1238e */
        step_branch(pc, opcode, 1); break;
    case 0xC12380: /* clr.b   $c4586b.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC12386: /* bra     $c1238e */
        step_branch(pc, opcode, 1); break;
    case 0xC12388: /* clr.b   $c4586b.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC1238E: /* move.w  $c458ca.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12394: /* andi.w  #$fffd, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC12398: /* move.w  D0, $c458ca.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1239E: /* clr.b   (-$5,A6) */
        m68k_write_memory_8(step_displacement(A(6)), 0); flags_logic_b(0); break;
    case 0xC123A2: /* move.w  (-$2,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC123A6: /* cmpi.w  #$384, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC123AA: /* blt     $c123b2 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC123AC: /* cmpi.w  #$a8c, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC123B0: /* blt     $c123ca */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC123B2: /* move.w  (-$4,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC123B6: /* cmpi.w  #$384, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC123BA: /* blt     $c123e0 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC123BC: /* cmpi.w  #$a8c, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC123C0: /* bgt     $c123e0 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC123C2: /* move.b  #$1, (-$5,A6) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(value); break;
    case 0xC123C8: /* bra     $c123e0 */
        step_branch(pc, opcode, 1); break;
    case 0xC123CA: /* move.w  (-$4,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC123CE: /* cmpi.w  #$384, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC123D2: /* blt     $c123da */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC123D4: /* cmpi.w  #$a8c, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC123D8: /* blt     $c123e0 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC123DA: /* move.b  #$1, (-$5,A6) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(value); break;
    case 0xC123E0: /* tst.b   (-$5,A6) */
        value = m68k_read_memory_8(step_displacement(A(6))); flags_logic_b(value); break;
    case 0xC123E4: /* beq     $c123f6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC123E6: /* move.w  $c458ca.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC123EC: /* ori.w   #$2, D0 */
        value = m68ki_read_imm_16(); result = D(0); result |= value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC123F0: /* move.w  D0, $c458ca.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC123F6: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC123F8: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1C2C8: /* movem.l D0-D6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC1C2CC: /* tst.b   $c457ac.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC1C2D2: /* beq     $c1c340 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1C2D4: /* clr.w   D0 */
        SET_W(D(0), 0); flags_logic_w(0); break;
    case 0xC1C2D6: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1C2DC: /* adda.w  $c458de.l, A0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1C2E2: /* move.l  ($14,A0), D2 */
        value = m68k_read_memory_32(step_displacement(A(0))); D(2) = value; flags_logic_l(value); break;
    case 0xC1C2E6: /* andi.l  #$3fffff, D2 */
        value = m68ki_read_imm_32(); result = D(2); result &= value; D(2) = result; flags_logic_l(result); break;
    case 0xC1C2EC: /* add.l   $c45c32.l, D2 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_add_long(&D(2), value); break;
    case 0xC1C2F2: /* bge     $c1c2fa */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1C2F4: /* neg.l   D2 */
        renderer_negate(&D(2), 4); break;
    case 0xC1C2F6: /* bset    #$0, D0 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(0); FLAG_Z = result & value; D(0) = result | value; if (value < 0x10000u) USE_CYCLES(-2); break;
    case 0xC1C2FA: /* asr.l   #8, D2 */
        step_asr_long(&D(2), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C2FC: /* move.l  ($18,A0), D3 */
        value = m68k_read_memory_32(step_displacement(A(0))); D(3) = value; flags_logic_l(value); break;
    case 0xC1C300: /* add.l   $c45c36.l, D3 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_add_long(&D(3), value); break;
    case 0xC1C306: /* bge     $c1c30e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1C308: /* neg.l   D3 */
        renderer_negate(&D(3), 4); break;
    case 0xC1C30A: /* bset    #$1, D0 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(0); FLAG_Z = result & value; D(0) = result | value; if (value < 0x10000u) USE_CYCLES(-2); break;
    case 0xC1C30E: /* asr.l   #8, D3 */
        step_asr_long(&D(3), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C310: /* move.l  ($1c,A0), D4 */
        value = m68k_read_memory_32(step_displacement(A(0))); D(4) = value; flags_logic_l(value); break;
    case 0xC1C314: /* andi.l  #$3fffff, D4 */
        value = m68ki_read_imm_32(); result = D(4); result &= value; D(4) = result; flags_logic_l(result); break;
    case 0xC1C31A: /* add.l   $c45c3a.l, D4 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_add_long(&D(4), value); break;
    case 0xC1C320: /* bge     $c1c328 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1C322: /* neg.l   D4 */
        renderer_negate(&D(4), 4); break;
    case 0xC1C324: /* bset    #$2, D0 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(0); FLAG_Z = result & value; D(0) = result | value; if (value < 0x10000u) USE_CYCLES(-2); break;
    case 0xC1C328: /* asr.l   #8, D4 */
        step_asr_long(&D(4), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C32A: /* movem.w D2-D4, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 2, 7); break;
    case 0xC1C32E: /* jsr     $c1d974.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1C334: /* movem.w (A7)+, D2-D4 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 2, 7); break;
    case 0xC1C338: /* move.w  D1, D5 */
        value = D(1); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC1C33A: /* subi.w  #$200, D1 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(1), value); break;
    case 0xC1C33E: /* bgt     $c1c34a */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1C340: /* clr.l   D2 */
        D(2) = 0; flags_logic_l(0); break;
    case 0xC1C342: /* clr.l   D3 */
        D(3) = 0; flags_logic_l(0); break;
    case 0xC1C344: /* clr.l   D4 */
        D(4) = 0; flags_logic_l(0); break;
    case 0xC1C346: /* bra     $c1c3dc */
        step_branch(pc, opcode, 1); break;
    case 0xC1C34A: /* move.w  D5, D6 */
        value = D(5); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1C34C: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C34E: /* cmp.w   D6, D1 */
        value = D(6); result = D(1); step_compare_word(value, result); break;
    case 0xC1C350: /* blt     $c1c3b8 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1C352: /* moveq   #$0, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC1C354: /* move.w  D5, D6 */
        value = D(5); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1C356: /* sub.w   D1, D6 */
        value = D(1); step_subtract_word(&D(6), value); break;
    case 0xC1C358: /* ext.l   D6 */
        D(6) = (uint32_t)(int32_t)(int16_t)D(6); flags_logic_l(D(6)); break;
    case 0xC1C35A: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC1C35C: /* divu.w  D5, D6 */
        value = D(5); step_divide_unsigned(&D(6), (uint16_t)value); break;
    case 0xC1C35E: /* mulu.w  D6, D2 */
        value = D(6); flight_multiply_unsigned(&D(2), (uint16_t)value); break;
    case 0xC1C360: /* mulu.w  D6, D3 */
        value = D(6); flight_multiply_unsigned(&D(3), (uint16_t)value); break;
    case 0xC1C362: /* mulu.w  D6, D4 */
        value = D(6); flight_multiply_unsigned(&D(4), (uint16_t)value); break;
    case 0xC1C364: /* asr.l   #8, D2 */
        step_asr_long(&D(2), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C366: /* bcc     $c1c36a */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC1C368: /* addq.l  #1, D2 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_long(&D(2), value); break;
    case 0xC1C36A: /* asr.l   #8, D3 */
        step_asr_long(&D(3), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C36C: /* bcc     $c1c370 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC1C36E: /* addq.l  #1, D3 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_long(&D(3), value); break;
    case 0xC1C370: /* asr.l   #8, D4 */
        step_asr_long(&D(4), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C372: /* bcc     $c1c376 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC1C374: /* addq.l  #1, D4 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_long(&D(4), value); break;
    case 0xC1C376: /* btst    #$0, D0 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(0); FLAG_Z = result & value; break;
    case 0xC1C37A: /* beq     $c1c37e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1C37C: /* neg.l   D2 */
        renderer_negate(&D(2), 4); break;
    case 0xC1C37E: /* btst    #$1, D0 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(0); FLAG_Z = result & value; break;
    case 0xC1C382: /* beq     $c1c386 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1C384: /* neg.l   D3 */
        renderer_negate(&D(3), 4); break;
    case 0xC1C386: /* btst    #$2, D0 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(0); FLAG_Z = result & value; break;
    case 0xC1C38A: /* beq     $c1c38e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1C38C: /* neg.l   D4 */
        renderer_negate(&D(4), 4); break;
    case 0xC1C38E: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1C394: /* adda.w  $c458de.l, A0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1C39A: /* move.l  ($14,A0), D0 */
        value = m68k_read_memory_32(step_displacement(A(0))); D(0) = value; flags_logic_l(value); break;
    case 0xC1C39E: /* andi.l  #$3fffff, D0 */
        value = m68ki_read_imm_32(); result = D(0); result &= value; D(0) = result; flags_logic_l(result); break;
    case 0xC1C3A4: /* sub.l   D0, D2 */
        value = D(0); step_subtract_long(&D(2), value); break;
    case 0xC1C3A6: /* sub.l   ($18,A0), D3 */
        value = m68k_read_memory_32(step_displacement(A(0))); step_subtract_long(&D(3), value); break;
    case 0xC1C3AA: /* move.l  ($1c,A0), D0 */
        value = m68k_read_memory_32(step_displacement(A(0))); D(0) = value; flags_logic_l(value); break;
    case 0xC1C3AE: /* andi.l  #$3fffff, D0 */
        value = m68ki_read_imm_32(); result = D(0); result &= value; D(0) = result; flags_logic_l(result); break;
    case 0xC1C3B4: /* sub.l   D0, D4 */
        value = D(0); step_subtract_long(&D(4), value); break;
    case 0xC1C3B6: /* bra     $c1c3f4 */
        step_branch(pc, opcode, 1); break;
    case 0xC1C3B8: /* ext.l   D1 */
        D(1) = (uint32_t)(int32_t)(int16_t)D(1); flags_logic_l(D(1)); break;
    case 0xC1C3BA: /* asl.l   #8, D1 */
        step_asl_long(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C3BC: /* divu.w  D5, D1 */
        value = D(5); step_divide_unsigned(&D(1), (uint16_t)value); break;
    case 0xC1C3BE: /* mulu.w  D1, D2 */
        value = D(1); flight_multiply_unsigned(&D(2), (uint16_t)value); break;
    case 0xC1C3C0: /* mulu.w  D1, D3 */
        value = D(1); flight_multiply_unsigned(&D(3), (uint16_t)value); break;
    case 0xC1C3C2: /* mulu.w  D1, D4 */
        value = D(1); flight_multiply_unsigned(&D(4), (uint16_t)value); break;
    case 0xC1C3C4: /* btst    #$0, D0 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(0); FLAG_Z = result & value; break;
    case 0xC1C3C8: /* bne     $c1c3cc */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1C3CA: /* neg.l   D2 */
        renderer_negate(&D(2), 4); break;
    case 0xC1C3CC: /* btst    #$1, D0 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(0); FLAG_Z = result & value; break;
    case 0xC1C3D0: /* bne     $c1c3d4 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1C3D2: /* neg.l   D3 */
        renderer_negate(&D(3), 4); break;
    case 0xC1C3D4: /* btst    #$2, D0 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(0); FLAG_Z = result & value; break;
    case 0xC1C3D8: /* bne     $c1c3dc */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1C3DA: /* neg.l   D4 */
        renderer_negate(&D(4), 4); break;
    case 0xC1C3DC: /* move.l  $c45c32.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC1C3E2: /* move.l  $c45c36.l, D1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(1) = value; flags_logic_l(value); break;
    case 0xC1C3E8: /* move.l  $c45c3a.l, D5 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(5) = value; flags_logic_l(value); break;
    case 0xC1C3EE: /* add.l   D0, D2 */
        value = D(0); step_add_long(&D(2), value); break;
    case 0xC1C3F0: /* add.l   D1, D3 */
        value = D(1); step_add_long(&D(3), value); break;
    case 0xC1C3F2: /* add.l   D5, D4 */
        value = D(5); step_add_long(&D(4), value); break;
    case 0xC1C3F4: /* move.l  D2, $c45a62.l */
        value = D(2); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1C3FA: /* move.l  D3, $c45a66.l */
        value = D(3); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1C400: /* move.l  D4, $c45a6a.l */
        value = D(4); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1C406: /* movem.l (A7)+, D0-D6 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC1C40A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1C54E: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC1C554: /* bne     $c1c60c */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1C558: /* lea     $c46184.l, A2 */
        A(2) = m68ki_read_imm_32(); break;
    case 0xC1C55E: /* adda.w  $c458de.l, A2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(2) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1C564: /* cmpi.b  #$30, ($62,A2) */
        value = m68ki_read_imm_16(); address = step_displacement(A(2)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC1C56A: /* bne     $c1c574 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1C56C: /* moveq   #$0, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC1C56E: /* moveq   #$1, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC1C570: /* moveq   #-$5, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC1C572: /* bra     $c1c588 */
        step_branch(pc, opcode, 1); break;
    case 0xC1C574: /* moveq   #$0, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC1C576: /* cmpi.b  #$11, ($62,A2) */
        value = m68ki_read_imm_16(); address = step_displacement(A(2)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC1C57C: /* beq     $c1c584 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1C57E: /* moveq   #$4, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC1C580: /* moveq   #$12, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC1C582: /* bra     $c1c588 */
        step_branch(pc, opcode, 1); break;
    case 0xC1C584: /* moveq   #$5, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC1C586: /* moveq   #$14, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC1C588: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1C58A: /* move.w  D4, D0 */
        value = D(4); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1C58C: /* move.w  D5, D7 */
        value = D(5); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1C58E: /* muls.w  ($92,A2), D6 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC1C592: /* muls.w  ($94,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC1C596: /* muls.w  ($96,A2), D7 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC1C59A: /* add.l   D6, D0 */
        value = D(6); step_add_long(&D(0), value); break;
    case 0xC1C59C: /* add.l   D7, D0 */
        value = D(7); step_add_long(&D(0), value); break;
    case 0xC1C59E: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1C5A0: /* move.w  D4, D1 */
        value = D(4); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1C5A2: /* move.w  D5, D7 */
        value = D(5); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1C5A4: /* muls.w  ($98,A2), D6 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC1C5A8: /* muls.w  ($9a,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC1C5AC: /* muls.w  ($9c,A2), D7 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC1C5B0: /* add.l   D6, D1 */
        value = D(6); step_add_long(&D(1), value); break;
    case 0xC1C5B2: /* add.l   D7, D1 */
        value = D(7); step_add_long(&D(1), value); break;
    case 0xC1C5B4: /* move.w  D4, D2 */
        value = D(4); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1C5B6: /* muls.w  ($9e,A2), D3 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC1C5BA: /* muls.w  ($a0,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC1C5BE: /* muls.w  ($a2,A2), D5 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(5), (uint16_t)value); break;
    case 0xC1C5C2: /* add.l   D3, D2 */
        value = D(3); step_add_long(&D(2), value); break;
    case 0xC1C5C4: /* add.l   D5, D2 */
        value = D(5); step_add_long(&D(2), value); break;
    case 0xC1C5C6: /* asr.l   #6, D0 */
        step_asr_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C5C8: /* asr.l   #6, D1 */
        step_asr_long(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C5CA: /* asr.l   #6, D2 */
        step_asr_long(&D(2), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C5CC: /* movem.l ($14,A2), D3-D5 */
        mask = m68ki_read_imm_16(); renderer_load(step_displacement(A(2)), mask, 4, -1); break;
    case 0xC1C5D2: /* add.l   D0, D3 */
        value = D(0); step_add_long(&D(3), value); break;
    case 0xC1C5D4: /* add.l   D1, D4 */
        value = D(1); step_add_long(&D(4), value); break;
    case 0xC1C5D6: /* add.l   D2, D5 */
        value = D(2); step_add_long(&D(5), value); break;
    case 0xC1C5D8: /* movem.l D3-D5, $c45a7c.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 4, -1); break;
    case 0xC1C5E0: /* move.l  ($14,A2), D3 */
        value = m68k_read_memory_32(step_displacement(A(2))); D(3) = value; flags_logic_l(value); break;
    case 0xC1C5E4: /* andi.l  #$3fffff, D3 */
        value = m68ki_read_imm_32(); result = D(3); result &= value; D(3) = result; flags_logic_l(result); break;
    case 0xC1C5EA: /* add.l   D3, D0 */
        value = D(3); step_add_long(&D(0), value); break;
    case 0xC1C5EC: /* add.l   ($18,A2), D1 */
        value = m68k_read_memory_32(step_displacement(A(2))); step_add_long(&D(1), value); break;
    case 0xC1C5F0: /* move.l  ($1c,A2), D3 */
        value = m68k_read_memory_32(step_displacement(A(2))); D(3) = value; flags_logic_l(value); break;
    case 0xC1C5F4: /* andi.l  #$3fffff, D3 */
        value = m68ki_read_imm_32(); result = D(3); result &= value; D(3) = result; flags_logic_l(result); break;
    case 0xC1C5FA: /* add.l   D3, D2 */
        value = D(3); step_add_long(&D(2), value); break;
    case 0xC1C5FC: /* neg.l   D0 */
        renderer_negate(&D(0), 4); break;
    case 0xC1C5FE: /* neg.l   D1 */
        renderer_negate(&D(1), 4); break;
    case 0xC1C600: /* neg.l   D2 */
        renderer_negate(&D(2), 4); break;
    case 0xC1C602: /* movem.l D0-D2, $c45a62.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 4, -1); break;
    case 0xC1C60A: /* bra     $c1c628 */
        step_branch(pc, opcode, 1); break;
    case 0xC1C60C: /* bsr     $c1c2c8 */
        value = opcode & 0xffu ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC1C610: /* movem.l $c45c3e.l, D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 4, -1); break;
    case 0xC1C618: /* movem.l D0-D2, $c45a7c.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 4, -1); break;
    case 0xC1C620: /* movem.l $c45a62.l, D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 4, -1); break;
    case 0xC1C628: /* asr.l   #8, D0 */
        step_asr_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C62A: /* asr.l   #8, D1 */
        step_asr_long(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C62C: /* asr.l   #8, D2 */
        step_asr_long(&D(2), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1C62E: /* movem.w D0-D2, $c45a72.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC1C636: /* move.l  D1, $c45a78.l */
        value = D(1); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1C63C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC230B0: /* move.w  $c459c0.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC230B6: /* blt     $c230e6 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC230B8: /* lea     $c46184.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC230BE: /* adda.w  D0, A1 */
        value = D(0); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC230C0: /* btst    #$6, ($1,A1) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(1)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC230C6: /* beq     $c230d0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC230C8: /* btst    #$1, ($20,A1) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(1)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC230CE: /* beq     $c230e6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC230D0: /* move.w  #$ffff, $c459c0.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC230D8: /* clr.b   $c45868.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC230DE: /* move.w  #$ffff, $c4593a.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC230E6: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC231A2: /* move.w  ($0,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC231A6: /* andi.w  #$8700, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC231AA: /* bne     $c2321a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC231AC: /* btst    #$1, ($20,A2) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(2)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC231B2: /* bne     $c2321a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC231B4: /* tst.b   $c457bc.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC231BA: /* bne     $c2321e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC231BC: /* btst    #$6, ($1,A2) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(2)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC231C2: /* beq     $c2321a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC231C4: /* move.b  ($38,A2), D0 */
        value = m68k_read_memory_8(step_displacement(A(2))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC231C8: /* bge     $c231f0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC231CA: /* andi.w  #$7f, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC231CE: /* asl.w   #8, D0 */
        renderer_asl_word(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC231D0: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC231D2: /* lea     $c46184.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC231D8: /* btst    #$6, ($1,A3,D0.w) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_indexed(A(3)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC231DE: /* beq     $c2321a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC231E0: /* btst    #$1, ($20,A3,D0.w) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_indexed(A(3)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC231E6: /* bne     $c2321a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC231E8: /* btst    #$0, ($1,A3,D0.w) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_indexed(A(3)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC231EE: /* bne     $c2321a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC231F0: /* move.b  ($64,A2), D0 */
        value = m68k_read_memory_8(step_displacement(A(2))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC231F4: /* andi.b  #$60, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC231F8: /* cmpi.b  #$60, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC231FC: /* bne     $c2321a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC231FE: /* btst    #$0, ($1,A2) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(2)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC23204: /* beq     $c2321a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC23206: /* move.b  ($63,A2), D0 */
        value = m68k_read_memory_8(step_displacement(A(2))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC2320A: /* andi.b  #$f0, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC2320E: /* cmpi.b  #$20, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC23212: /* beq     $c2321e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC23214: /* cmpi.b  #$30, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC23218: /* beq     $c2321e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2321A: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2321C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2321E: /* clr.b   $c457bc.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC23224: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC23226: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC23744: /* bclr    #$6, ($1,A1) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(1)); result = m68k_read_memory_8(address); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~value); break;
    case 0xC2374A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2374C: /* clr.b   $c457ba.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC23752: /* move.b  #$1, $c457b7.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC2375A: /* move.l  $c45b50.l, D6 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(6) = value; flags_logic_l(value); break;
    case 0xC23760: /* andi.l  #$4000, D6 */
        value = m68ki_read_imm_32(); result = D(6); result &= value; D(6) = result; flags_logic_l(result); break;
    case 0xC23766: /* beq     $c237b8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC23768: /* andi.l  #$ffffbfff, $c45b50.l */
        value = m68ki_read_imm_32(); address = m68ki_read_imm_32(); result = m68k_read_memory_32(address); result &= value; step_write_long(address, result); flags_logic_l(result); break;
    case 0xC23772: /* ori.l   #$8, $c45b54.l */
        value = m68ki_read_imm_32(); address = m68ki_read_imm_32(); result = m68k_read_memory_32(address); result |= value; step_write_long(address, result); flags_logic_l(result); break;
    case 0xC2377C: /* bra     $c237b8 */
        step_branch(pc, opcode, 1); break;
    case 0xC237B8: /* move.b  ($5f,A2), D1 */
        value = m68k_read_memory_8(step_displacement(A(2))); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC237BC: /* move.b  ($63,A2), D0 */
        value = m68k_read_memory_8(step_displacement(A(2))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC237C0: /* andi.b  #$f0, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC237C4: /* cmpi.b  #$30, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC237C8: /* beq     $c237d0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC237CA: /* andi.b  #$f0, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_B(D(1), result); flags_logic_b(result); break;
    case 0xC237CE: /* bra     $c237d4 */
        step_branch(pc, opcode, 1); break;
    case 0xC237D0: /* andi.b  #$f, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_B(D(1), result); flags_logic_b(result); break;
    case 0xC237D4: /* beq     $c23744 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC237D8: /* move.l  A2, D0 */
        value = A(2); D(0) = value; flags_logic_l(value); break;
    case 0xC237DA: /* subi.l  #$c46184, D0 */
        value = m68ki_read_imm_32(); step_subtract_long(&D(0), value); break;
    case 0xC237E0: /* cmp.w   $c458de.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); result = D(0); step_compare_word(value, result); break;
    case 0xC237E6: /* bne     $c237f8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC237E8: /* move.b  #$8, $c45797.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC237F0: /* move.b  #$fb, $c458b0.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC237F8: /* lea     (A2), A4 */
        A(4) = A(2); break;
    case 0xC237FA: /* lea     (A1), A5 */
        A(5) = A(1); break;
    case 0xC237FC: /* moveq   #$28, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC237FE: /* move.l  (A4)+, (A5)+ */
        value = m68k_read_memory_32(A(4)); A(4) += 4; step_write_long(A(5), value); A(5) += 4; flags_logic_l(value); break;
    case 0xC23800: /* dbra    D0, $c237fe */
        step_dbf(pc, &D(0)); break;
    case 0xC23804: /* clr.b   ($5,A1) */
        m68k_write_memory_8(step_displacement(A(1)), 0); flags_logic_b(0); break;
    case 0xC23808: /* movea.l $c1ab74.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC2380E: /* move.b  ($63,A1), D0 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC23812: /* andi.b  #$f0, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC23816: /* cmpi.b  #$30, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC2381A: /* bne     $c2383c */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2381C: /* move.b  #$1, ($62,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC23822: /* move.w  #$14, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23826: /* cmpa.l  #$c46184, A2 */
        value = m68ki_read_imm_32(); result = A(2); step_compare_long(value, result); break;
    case 0xC2382C: /* bne     $c2385a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2382E: /* addq.w  #1, ($3e,A0) */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; address = step_displacement(A(0)); result = m68k_read_memory_16(address); step_add_word(&result, value); step_write_word(address, result); break;
    case 0xC23832: /* move.b  #$1, $c457c5.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC2383A: /* bra     $c2385a */
        step_branch(pc, opcode, 1); break;
    case 0xC2383C: /* move.b  #$0, ($62,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC23842: /* move.w  #$0, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23846: /* cmpa.l  #$c46184, A2 */
        value = m68ki_read_imm_32(); result = A(2); step_compare_long(value, result); break;
    case 0xC2384C: /* bne     $c2385a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2384E: /* addq.w  #1, ($42,A0) */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; address = step_displacement(A(0)); result = m68k_read_memory_16(address); step_add_word(&result, value); step_write_word(address, result); break;
    case 0xC23852: /* move.b  #$1, $c457c5.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC2385A: /* lea     $c22048.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC23860: /* adda.w  D0, A4 */
        value = D(0); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC23862: /* lea     $c22188.l, A5 */
        A(5) = m68ki_read_imm_32(); break;
    case 0xC23868: /* move.w  $c459b4.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2386E: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC23870: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC23872: /* move.w  D3, D4 */
        value = D(3); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC23874: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC23876: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC23878: /* add.w   D4, D3 */
        value = D(4); step_add_word(&D(3), value); break;
    case 0xC2387A: /* adda.w  D3, A5 */
        value = D(3); A(5) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2387C: /* movem.l (A4), D2-D6 */
        mask = m68ki_read_imm_16(); renderer_load(A(4), mask, 4, -1); break;
    case 0xC23880: /* movem.l D2-D6, (A5) */
        mask = m68ki_read_imm_16(); renderer_store(A(5), mask, 4, -1); break;
    case 0xC23884: /* clr.w   ($56,A1) */
        step_write_word(step_displacement(A(1)), 0); flags_logic_w(0); break;
    case 0xC23888: /* clr.w   ($58,A1) */
        step_write_word(step_displacement(A(1)), 0); flags_logic_w(0); break;
    case 0xC2388C: /* clr.w   ($5a,A1) */
        step_write_word(step_displacement(A(1)), 0); flags_logic_w(0); break;
    case 0xC23890: /* clr.b   ($64,A1) */
        m68k_write_memory_8(step_displacement(A(1)), 0); flags_logic_b(0); break;
    case 0xC23894: /* bclr    #$2, ($3,A1) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(1)); result = m68k_read_memory_8(address); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~value); break;
    case 0xC2389A: /* move.b  ($62,A1), D1 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC2389E: /* andi.b  #$f0, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_B(D(1), result); flags_logic_b(result); break;
    case 0xC238A2: /* cmpi.b  #$30, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_byte(value, result); break;
    case 0xC238A6: /* beq     $c2390a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC238A8: /* move.b  ($5f,A2), D1 */
        value = m68k_read_memory_8(step_displacement(A(2))); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC238AC: /* move.b  D1, D2 */
        value = D(1); SET_B(D(2), value); flags_logic_b(value); break;
    case 0xC238AE: /* move.b  ($63,A2), D0 */
        value = m68k_read_memory_8(step_displacement(A(2))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC238B2: /* andi.b  #$f0, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC238B6: /* cmpi.b  #$30, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC238BA: /* beq     $c238ce */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC238BC: /* andi.b  #$f0, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_B(D(1), result); flags_logic_b(result); break;
    case 0xC238C0: /* andi.b  #$f, D2 */
        value = m68ki_read_imm_16(); result = D(2); result &= value; SET_B(D(2), result); flags_logic_b(result); break;
    case 0xC238C4: /* subi.b  #$10, D1 */
        value = m68ki_read_imm_16(); step_subtract_byte(&D(1), value); break;
    case 0xC238C8: /* move.b  D1, D0 */
        value = D(1); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC238CA: /* lsr.b   #4, D0 */
        flight_lsr_byte(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC238CC: /* bra     $c238dc */
        step_branch(pc, opcode, 1); break;
    case 0xC238CE: /* andi.b  #$f, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_B(D(1), result); flags_logic_b(result); break;
    case 0xC238D2: /* andi.b  #$f0, D2 */
        value = m68ki_read_imm_16(); result = D(2); result &= value; SET_B(D(2), result); flags_logic_b(result); break;
    case 0xC238D6: /* subq.b  #1, D1 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_byte(&D(1), value); break;
    case 0xC238D8: /* move.b  D1, D0 */
        value = D(1); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC238DA: /* addq.b  #2, D0 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; renderer_add_byte(&D(0), value); break;
    case 0xC238DC: /* or.b    D2, D1 */
        value = D(2); result = D(1); result |= value; SET_B(D(1), result); flags_logic_b(result); break;
    case 0xC238DE: /* move.b  D1, ($5f,A2) */
        value = D(1); m68k_write_memory_8(step_displacement(A(2)), value); flags_logic_b(value); break;
    case 0xC238E2: /* move.b  #$3, $c45843.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC238EA: /* move.b  #$3, $c45844.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC238F2: /* cmpi.b  #$10, ($62,A2) */
        value = m68ki_read_imm_16(); address = step_displacement(A(2)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC238F8: /* beq     $c23902 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC238FA: /* lea     $c23a56.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC23900: /* bra     $c2391c */
        step_branch(pc, opcode, 1); break;
    case 0xC23902: /* lea     $c23a32.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC23908: /* bra     $c2391c */
        step_branch(pc, opcode, 1); break;
    case 0xC2390A: /* lea     $c23a26.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC23910: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC23912: /* cmpi.b  #$30, ($62,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC23918: /* beq     $c2391c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2391A: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2391C: /* ext.w   D0 */
        SET_W(D(0), (uint16_t)(int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC2391E: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC23920: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC23922: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC23924: /* add.w   D1, D0 */
        value = D(1); step_add_word(&D(0), value); break;
    case 0xC23926: /* movem.w (A4,D0.w), D3-D5 */
        mask = m68ki_read_imm_16(); renderer_load(step_indexed(A(4)), mask, 2, -1); break;
    case 0xC2392C: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2392E: /* move.w  D4, D0 */
        value = D(4); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23930: /* move.w  D5, D7 */
        value = D(5); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC23932: /* muls.w  ($92,A2), D6 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC23936: /* muls.w  ($94,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2393A: /* muls.w  ($96,A2), D7 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2393E: /* add.l   D6, D0 */
        value = D(6); step_add_long(&D(0), value); break;
    case 0xC23940: /* add.l   D7, D0 */
        value = D(7); step_add_long(&D(0), value); break;
    case 0xC23942: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC23944: /* move.w  D4, D1 */
        value = D(4); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC23946: /* move.w  D5, D7 */
        value = D(5); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC23948: /* muls.w  ($98,A2), D6 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2394C: /* muls.w  ($9a,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC23950: /* muls.w  ($9c,A2), D7 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC23954: /* add.l   D6, D1 */
        value = D(6); step_add_long(&D(1), value); break;
    case 0xC23956: /* add.l   D7, D1 */
        value = D(7); step_add_long(&D(1), value); break;
    case 0xC23958: /* move.w  D4, D2 */
        value = D(4); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2395A: /* muls.w  ($9e,A2), D3 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC2395E: /* muls.w  ($a0,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC23962: /* muls.w  ($a2,A2), D5 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(5), (uint16_t)value); break;
    case 0xC23966: /* add.l   D3, D2 */
        value = D(3); step_add_long(&D(2), value); break;
    case 0xC23968: /* add.l   D5, D2 */
        value = D(5); step_add_long(&D(2), value); break;
    case 0xC2396A: /* asr.l   #6, D0 */
        step_asr_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC2396C: /* asr.l   #6, D1 */
        step_asr_long(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC2396E: /* asr.l   #6, D2 */
        step_asr_long(&D(2), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC23970: /* add.l   D0, ($14,A1) */
        value = D(0); address = step_displacement(A(1)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC23974: /* add.l   D1, ($18,A1) */
        value = D(1); address = step_displacement(A(1)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC23978: /* add.l   D2, ($1c,A1) */
        value = D(2); address = step_displacement(A(1)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC2397C: /* move.w  #$96, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23980: /* cmpi.b  #$0, ($62,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC23986: /* beq     $c2398c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC23988: /* move.w  #$c8, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2398C: /* move.w  D0, ($4c,A1) */
        value = D(0); step_write_word(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC23990: /* move.w  #$14, ($26,A1) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC23996: /* ori.w   #$10c0, ($0,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result |= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC2399C: /* ori.w   #$100, ($0,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result |= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC239A2: /* ori.w   #$2, ($0,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result |= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC239A8: /* clr.w   ($76,A1) */
        step_write_word(step_displacement(A(1)), 0); flags_logic_w(0); break;
    case 0xC239AC: /* move.w  ($2,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC239B0: /* andi.w  #$80, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC239B4: /* beq     $c239ba */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC239B6: /* clr.l   ($42,A1) */
        step_write_long(step_displacement(A(1)), 0); flags_logic_l(0); break;
    case 0xC239BA: /* andi.w  #$ff7f, ($2,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result &= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC239C0: /* move.w  #$ffff, ($2c,A1) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC239C6: /* move.b  #$8c, $c45858.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC239CE: /* move.b  ($38,A2), ($38,A1) */
        value = m68k_read_memory_8(step_displacement(A(2))); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC239D4: /* move.b  ($62,A1), D0 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC239D8: /* andi.b  #$f0, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC239DC: /* cmpi.b  #$30, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC239E0: /* bne     $c23a24 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC239E2: /* move.w  ($94,A2), D5 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC239E6: /* move.w  ($9a,A2), D6 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC239EA: /* move.w  ($a0,A2), D7 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC239EE: /* asr.w   #2, D5 */
        renderer_asr_word(&D(5), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC239F0: /* asr.w   #2, D6 */
        renderer_asr_word(&D(6), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC239F2: /* asr.w   #2, D7 */
        renderer_asr_word(&D(7), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC239F4: /* cmpi.b  #$30, ($62,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC239FA: /* beq     $c23a02 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC239FC: /* neg.w   D5 */
        renderer_negate(&D(5), 2); break;
    case 0xC239FE: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC23A00: /* neg.w   D7 */
        renderer_negate(&D(7), 2); break;
    case 0xC23A02: /* move.w  #$3c0, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23A06: /* jsr     $c2574a.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC23A0C: /* movem.l ($3e,A1), D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(step_displacement(A(1)), mask, 4, -1); break;
    case 0xC23A12: /* add.l   D0, D5 */
        value = D(0); step_add_long(&D(5), value); break;
    case 0xC23A14: /* add.l   D1, D6 */
        value = D(1); step_add_long(&D(6), value); break;
    case 0xC23A16: /* add.l   D2, D7 */
        value = D(2); step_add_long(&D(7), value); break;
    case 0xC23A18: /* movem.l D5-D7, ($3e,A1) */
        mask = m68ki_read_imm_16(); renderer_store(step_displacement(A(1)), mask, 4, -1); break;
    case 0xC23A1E: /* move.w  #$ffec, ($4c,A1) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC23A24: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC244E2: /* cmpi.w  #$1200, ($6c,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_16(address); step_compare_word(value, result); break;
    case 0xC244E8: /* blt     $c2450e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC244EA: /* move.b  ($7c,A1), D1 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC244EE: /* andi.b  #$70, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_B(D(1), result); flags_logic_b(result); break;
    case 0xC244F2: /* bne     $c2450e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC244F4: /* tst.b   $c45846.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC244FA: /* bne     $c24514 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC244FC: /* move.b  #$1, $c45846.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC24504: /* moveq   #$4, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24506: /* jsr     $c3316e.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2450C: /* bra     $c24514 */
        step_branch(pc, opcode, 1); break;
    case 0xC2450E: /* clr.b   $c45846.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC24514: /* move.w  $c459c0.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2451A: /* ble     $c24566 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2451C: /* lea     $c46184.l, A2 */
        A(2) = m68ki_read_imm_32(); break;
    case 0xC24522: /* adda.w  D0, A2 */
        value = D(0); A(2) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24524: /* addi.b  #$10, ($39,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC2452A: /* cmpi.w  #$480, ($4a,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_16(address); step_compare_word(value, result); break;
    case 0xC24530: /* blt     $c2454a */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC24532: /* moveq   #$20, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC24534: /* cmpi.w  #$900, ($4a,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_16(address); step_compare_word(value, result); break;
    case 0xC2453A: /* blt     $c2453e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2453C: /* moveq   #$50, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC2453E: /* move.b  ($39,A1), D0 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC24542: /* andi.b  #$f0, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC24546: /* cmp.b   D0, D1 */
        value = D(0); result = D(1); step_compare_byte(value, result); break;
    case 0xC24548: /* bge     $c24566 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2454A: /* andi.b  #$f, ($39,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); result &= value; m68k_write_memory_8(address, result); flags_logic_b(result); break;
    case 0xC24550: /* move.w  ($c,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC24554: /* move.w  ($e,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC24558: /* move.w  ($6,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2455C: /* move.w  ($8,A2), D4 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC24560: /* move.l  ($10,A2), D3 */
        value = m68k_read_memory_32(step_displacement(A(2))); D(3) = value; flags_logic_l(value); break;
    case 0xC24564: /* bra     $c245aa */
        step_branch(pc, opcode, 1); break;
    case 0xC24566: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC245AA: /* ext.l   D2 */
        D(2) = (uint32_t)(int32_t)(int16_t)D(2); flags_logic_l(D(2)); break;
    case 0xC245AC: /* ext.l   D4 */
        D(4) = (uint32_t)(int32_t)(int16_t)D(4); flags_logic_l(D(4)); break;
    case 0xC245AE: /* sub.w   ($6,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); step_subtract_word(&D(2), value); break;
    case 0xC245B2: /* swap    D2 */
        step_swap(&D(2)); break;
    case 0xC245B4: /* asr.l   #2, D2 */
        step_asr_long(&D(2), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC245B6: /* sub.w   ($c,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); step_subtract_word(&D(0), value); break;
    case 0xC245BA: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC245BC: /* add.l   D0, D2 */
        value = D(0); step_add_long(&D(2), value); break;
    case 0xC245BE: /* bge     $c245c2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC245C0: /* neg.l   D2 */
        renderer_negate(&D(2), 4); break;
    case 0xC245C2: /* sub.w   ($8,A1), D4 */
        value = m68k_read_memory_16(step_displacement(A(1))); step_subtract_word(&D(4), value); break;
    case 0xC245C6: /* swap    D4 */
        step_swap(&D(4)); break;
    case 0xC245C8: /* asr.l   #2, D4 */
        step_asr_long(&D(4), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC245CA: /* sub.w   ($e,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); step_subtract_word(&D(1), value); break;
    case 0xC245CE: /* ext.l   D1 */
        D(1) = (uint32_t)(int32_t)(int16_t)D(1); flags_logic_l(D(1)); break;
    case 0xC245D0: /* add.l   D1, D4 */
        value = D(1); step_add_long(&D(4), value); break;
    case 0xC245D2: /* bge     $c245d6 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC245D4: /* neg.l   D4 */
        renderer_negate(&D(4), 4); break;
    case 0xC245D6: /* sub.l   ($10,A1), D3 */
        value = m68k_read_memory_32(step_displacement(A(1))); step_subtract_long(&D(3), value); break;
    case 0xC245DA: /* bge     $c245de */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC245DC: /* neg.l   D3 */
        renderer_negate(&D(3), 4); break;
    case 0xC245DE: /* move.l  #$7f00, D0 */
        value = m68ki_read_imm_32(); D(0) = value; flags_logic_l(value); break;
    case 0xC245E4: /* cmp.l   D0, D2 */
        value = D(0); result = D(2); step_compare_long(value, result); break;
    case 0xC245E6: /* bgt     $c2464c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC245E8: /* cmp.l   D0, D3 */
        value = D(0); result = D(3); step_compare_long(value, result); break;
    case 0xC245EA: /* bgt     $c2464c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC245EC: /* cmp.l   D0, D4 */
        value = D(0); result = D(4); step_compare_long(value, result); break;
    case 0xC245EE: /* bgt     $c2464c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC245F0: /* jsr     $c1d974.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC245F6: /* move.w  D1, ($4a,A1) */
        value = D(1); step_write_word(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC245FA: /* bset    #$0, ($4,A1) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(1)); result = m68k_read_memory_8(address); FLAG_Z = result & value; m68k_write_memory_8(address, result | value); break;
    case 0xC24600: /* cmpi.w  #$36c0, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC24604: /* bgt     $c24652 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24606: /* cmpi.w  #$1e00, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC2460A: /* bgt     $c2461c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2460C: /* cmpi.b  #$3, ($7a,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC24612: /* bne     $c2462a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC24614: /* move.b  #$4, ($7a,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC2461A: /* bra     $c2462a */
        step_branch(pc, opcode, 1); break;
    case 0xC2461C: /* cmpi.b  #$4, ($7a,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC24622: /* bne     $c2462a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC24624: /* move.b  #$3, ($7a,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC2462A: /* tst.w   $c459b6.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC24630: /* beq     $c24660 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24632: /* cmpi.w  #$1800, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC24636: /* bgt     $c2464c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24638: /* cmpi.w  #$300, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC2463C: /* bgt     $c24662 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2463E: /* andi.b  #$f, ($63,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); result &= value; m68k_write_memory_8(address, result); flags_logic_b(result); break;
    case 0xC24644: /* ori.b   #$10, ($63,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); result |= value; m68k_write_memory_8(address, result); flags_logic_b(result); break;
    case 0xC2464A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2464C: /* move.w  #$7fff, ($4a,A1) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC24652: /* tst.w   $c459b6.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC24658: /* beq     $c24660 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2465A: /* ori.b   #$f0, ($63,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); result |= value; m68k_write_memory_8(address, result); flags_logic_b(result); break;
    case 0xC24660: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24662: /* andi.b  #$f, ($63,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); result &= value; m68k_write_memory_8(address, result); flags_logic_b(result); break;
    case 0xC24668: /* cmpi.w  #$c00, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC2466C: /* bgt     $c24676 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2466E: /* ori.b   #$20, ($63,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); result |= value; m68k_write_memory_8(address, result); flags_logic_b(result); break;
    case 0xC24674: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24676: /* ori.b   #$30, ($63,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); result |= value; m68k_write_memory_8(address, result); flags_logic_b(result); break;
    case 0xC2467C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC254E8: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC254EE: /* beq     $c254f8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC254F0: /* move.w  $c45a96.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC254F6: /* bra     $c254fe */
        step_branch(pc, opcode, 1); break;
    case 0xC254F8: /* move.l  $c45a8c.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC254FE: /* cmpi.w  #$3840, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC25502: /* bge     $c25526 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC25504: /* cmpi.w  #$1c20, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC25508: /* bge     $c25518 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2550A: /* cmpi.w  #$e10, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2550E: /* bge     $c25514 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC25510: /* moveq   #$0, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC25512: /* bra     $c25546 */
        step_branch(pc, opcode, 1); break;
    case 0xC25514: /* moveq   #$1, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC25516: /* bra     $c25546 */
        step_branch(pc, opcode, 1); break;
    case 0xC25518: /* cmpi.w  #$2a30, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2551C: /* bge     $c25522 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2551E: /* moveq   #$2, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC25520: /* bra     $c25546 */
        step_branch(pc, opcode, 1); break;
    case 0xC25522: /* moveq   #$3, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC25524: /* bra     $c25546 */
        step_branch(pc, opcode, 1); break;
    case 0xC25526: /* cmpi.w  #$5460, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2552A: /* bge     $c2553a */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2552C: /* cmpi.w  #$4650, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC25530: /* bge     $c25536 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC25532: /* moveq   #$4, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC25534: /* bra     $c25546 */
        step_branch(pc, opcode, 1); break;
    case 0xC25536: /* moveq   #$5, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC25538: /* bra     $c25546 */
        step_branch(pc, opcode, 1); break;
    case 0xC2553A: /* cmpi.w  #$6270, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2553E: /* bge     $c25544 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC25540: /* moveq   #$6, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC25542: /* bra     $c25546 */
        step_branch(pc, opcode, 1); break;
    case 0xC25544: /* moveq   #$7, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC25546: /* move.b  D1, $c45854.l */
        value = D(1); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC2554C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC25592: /* clr.b   $c457c0.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC25598: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2559A: /* move.b  $c457c0.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC255A0: /* beq     $c25598 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC255A2: /* move.w  $c45986.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC255A8: /* bge     $c255ac */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC255AA: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC255AC: /* cmpi.w  #$e, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC255B0: /* bgt     $c25592 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC255B2: /* lea     (-$42,PC), A0; ($c25572) */
        A(0) = step_displacement(pc + 2); break;
    case 0xC255B6: /* ext.w   D0 */
        SET_W(D(0), (uint16_t)(int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC255B8: /* subq.w  #1, D0 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(0), value); break;
    case 0xC255BA: /* asl.w   #3, D0 */
        renderer_asl_word(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC255BC: /* movea.l (A0,D0.w), A1 */
        value = m68k_read_memory_32(step_indexed(A(0))); A(1) = value; break;
    case 0xC255C0: /* move.b  (A1), D1 */
        value = m68k_read_memory_8(A(1)); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC255C2: /* cmp.b   $c4588f.l, D1 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); result = D(1); step_compare_byte(value, result); break;
    case 0xC255C8: /* ble     $c25624 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC255CA: /* move.w  $c458cc.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC255D0: /* andi.w  #$400, D2 */
        value = m68ki_read_imm_16(); result = D(2); result &= value; SET_W(D(2), result); flags_logic_w(result); break;
    case 0xC255D4: /* bne     $c255e4 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC255D6: /* clr.b   $c4588f.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC255DC: /* ori.w   #$400, $c458cc.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); result |= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC255E4: /* addq.w  #4, D0 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(0), value); break;
    case 0xC255E6: /* movea.l (A0,D0.w), A1 */
        value = m68k_read_memory_32(step_indexed(A(0))); A(1) = value; break;
    case 0xC255EA: /* move.b  $c4588f.l, D2 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(2), value); flags_logic_b(value); break;
    case 0xC255F0: /* ext.w   D2 */
        SET_W(D(2), (uint16_t)(int16_t)(int8_t)D(2)); flags_logic_w(D(2)); break;
    case 0xC255F2: /* move.b  (A1,D2.w), D0 */
        value = m68k_read_memory_8(step_indexed(A(1))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC255F6: /* addq.b  #1, $c4588f.l */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC255FC: /* ext.w   D0 */
        SET_W(D(0), (uint16_t)(int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC255FE: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC25600: /* addi.w  #$90, D1 */
        value = m68ki_read_imm_16(); step_add_word(&D(1), value); break;
    case 0xC25604: /* move.w  D1, $c45984.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2560A: /* move.w  D0, $c458d8.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC25610: /* asl.w   #3, D0 */
        renderer_asl_word(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC25612: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC25614: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC25616: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC25618: /* add.w   D1, D0 */
        value = D(1); step_add_word(&D(0), value); break;
    case 0xC2561A: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC2561C: /* move.l  D0, $c45918.l */
        value = D(0); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC25622: /* bra     $c25646 */
        step_branch(pc, opcode, 1); break;
    case 0xC25624: /* clr.b   $c457c0.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC2562A: /* clr.b   $c4588f.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC25630: /* andi.w  #$fbff, $c458cc.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); result &= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC25638: /* clr.l   $c45918.l */
        step_write_long(m68ki_read_imm_32(), 0); flags_logic_l(0); break;
    case 0xC2563E: /* move.w  #$90, $c45984.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC25646: /* jsr     $c082b8.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2564C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC25704: /* move.w  D1, -(A7) */
        value = D(1); A(7) -= 2; step_write_word(A(7), value); flags_logic_w(value); break;
    case 0xC25706: /* move.w  D0, $c45ae0.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2570C: /* ori.w   #$1, $c458cc.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); result |= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC25714: /* andi.w  #$ff00, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC25718: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2571A: /* andi.w  #$2000, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_W(D(1), result); flags_logic_w(result); break;
    case 0xC2571E: /* beq     $c25732 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC25720: /* andi.w  #$7fff, $c458ce.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); result &= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC25728: /* ori.w   #$2000, $c458ce.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); result |= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC25730: /* bra     $c25746 */
        step_branch(pc, opcode, 1); break;
    case 0xC25732: /* cmpi.w  #$4000, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC25736: /* beq     $c2573e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC25738: /* cmpi.w  #$4800, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2573C: /* bne     $c25746 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2573E: /* andi.w  #$dfff, $c458ce.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); result &= value; step_write_word(address, result); flags_logic_w(result); break;
    case 0xC25746: /* move.w  (A7)+, D1 */
        value = m68k_read_memory_16(A(7)); A(7) += 2; SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC25748: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2574A: /* link    A6, #-$4 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC2574E: /* move.w  D0, (-$2,A6) */
        value = D(0); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC25752: /* bra     $c25764 */
        step_branch(pc, opcode, 1); break;
    case 0xC25764: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC25766: /* beq     $c257d4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC25768: /* bgt     $c2576c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2576A: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2576C: /* move.w  D5, D2 */
        value = D(5); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2576E: /* bge     $c25772 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC25770: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC25772: /* move.w  D6, D3 */
        value = D(6); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC25774: /* bge     $c25778 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC25776: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC25778: /* move.w  D7, D4 */
        value = D(7); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2577A: /* bge     $c2577e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2577C: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2577E: /* jsr     $c1d974.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC25784: /* beq     $c257d4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC25786: /* moveq   #$8, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC25788: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC2578A: /* cmp.l   D1, D0 */
        value = D(1); result = D(0); step_compare_long(value, result); break;
    case 0xC2578C: /* bgt     $c25798 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2578E: /* asl.l   #2, D0 */
        step_asl_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC25790: /* addq.w  #2, D2 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(2), value); break;
    case 0xC25792: /* bra     $c2578a */
        step_branch(pc, opcode, 1); break;
    case 0xC25794: /* cmp.l   D1, D0 */
        value = D(1); result = D(0); step_compare_long(value, result); break;
    case 0xC25796: /* ble     $c257a2 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC25798: /* asr.l   #2, D0 */
        step_asr_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC2579A: /* subq.w  #2, D2 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(2), value); break;
    case 0xC2579C: /* cmpi.w  #$1, D2 */
        value = m68ki_read_imm_16(); result = D(2); step_compare_word(value, result); break;
    case 0xC257A0: /* bgt     $c25794 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC257A2: /* asl.l   #2, D0 */
        step_asl_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC257A4: /* addq.w  #2, D2 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(2), value); break;
    case 0xC257A6: /* asl.l   #8, D0 */
        step_asl_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC257A8: /* divu.w  D1, D0 */
        value = D(1); step_divide_unsigned(&D(0), (uint16_t)value); break;
    case 0xC257AA: /* muls.w  D0, D5 */
        value = D(0); renderer_multiply(&D(5), (uint16_t)value); break;
    case 0xC257AC: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC257AE: /* muls.w  D0, D7 */
        value = D(0); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC257B0: /* asr.l   D2, D5 */
        step_asr_long(&D(5), D(2)); break;
    case 0xC257B2: /* asr.l   D2, D6 */
        step_asr_long(&D(6), D(2)); break;
    case 0xC257B4: /* asr.l   D2, D7 */
        step_asr_long(&D(7), D(2)); break;
    case 0xC257B6: /* tst.w   (-$2,A6) */
        value = m68k_read_memory_16(step_displacement(A(6))); flags_logic_w(value); break;
    case 0xC257BA: /* bge     $c257c2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC257BC: /* neg.w   D5 */
        renderer_negate(&D(5), 2); break;
    case 0xC257BE: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC257C0: /* neg.w   D7 */
        renderer_negate(&D(7), 2); break;
    case 0xC257C2: /* movem.w D5-D7, $c45a4c.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC257CA: /* ext.l   D5 */
        D(5) = (uint32_t)(int32_t)(int16_t)D(5); flags_logic_l(D(5)); break;
    case 0xC257CC: /* ext.l   D6 */
        D(6) = (uint32_t)(int32_t)(int16_t)D(6); flags_logic_l(D(6)); break;
    case 0xC257CE: /* ext.l   D7 */
        D(7) = (uint32_t)(int32_t)(int16_t)D(7); flags_logic_l(D(7)); break;
    case 0xC257D0: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC257D2: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC257D4: /* clr.w   D5 */
        SET_W(D(5), 0); flags_logic_w(0); break;
    case 0xC257D6: /* clr.w   D6 */
        SET_W(D(6), 0); flags_logic_w(0); break;
    case 0xC257D8: /* clr.w   D7 */
        SET_W(D(7), 0); flags_logic_w(0); break;
    case 0xC257DA: /* bra     $c257c2 */
        step_branch(pc, opcode, 1); break;
    case 0xC25864: /* clr.w   $c46182.l */
        step_write_word(m68ki_read_imm_32(), 0); flags_logic_w(0); break;
    case 0xC2586A: /* move.l  #$c4e2bc, $c459ca.l */
        value = m68ki_read_imm_32(); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC25874: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C244E2_step(void) { return glue_C230B0_step(); }

int glue_C1C54E_step(void) { return glue_C230B0_step(); }

int glue_C254E8_step(void) { return glue_C230B0_step(); }

int glue_C122A2_step(void) { return glue_C230B0_step(); }

int glue_C1C2C8_step(void) { return glue_C230B0_step(); }

int glue_C2374C_step(void) { return glue_C230B0_step(); }

int glue_C2574A_step(void) { return glue_C230B0_step(); }

int glue_C25704_step(void) { return glue_C230B0_step(); }

int glue_C231A2_step(void) { return glue_C230B0_step(); }

int glue_C2559A_step(void) { return glue_C230B0_step(); }

int glue_C25864_step(void) { return glue_C230B0_step(); }
