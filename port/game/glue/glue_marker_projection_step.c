/* Fixed matrix marker, view-point projection and filled circle timing.
 * Readable game behavior remains in its domain modules; CPU effects stay here. */
#include "glue_renderer_step_math.h"

static void marker_rotate_word(uint32_t *reg, unsigned count) {
    uint16_t old = (uint16_t)*reg, result;
    unsigned shift;
    count &= 63u; shift = count & 15u;
    result = shift ? (uint16_t)((old >> shift) | (old << (16 - shift))) : old;
    SET_W(*reg, result); flags_logic_w(result);
    FLAG_C = count ? ((old >> ((shift - 1u) & 15u)) & 1u) << 8 : 0;
    USE_CYCLES(count << CYC_SHIFT);
}

int glue_C0DAEE_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC06C02: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC0DAEE: /* move.w  #$e000, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC0DAF2: /* move.w  #$3800, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC0DAF6: /* move.w  #$e000, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC0DAFA: /* lea     $c45bd8.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC0DB00: /* move.w  D2, D5 */
        value = D(2); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC0DB02: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0DB04: /* move.w  D4, D0 */
        value = D(4); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0DB06: /* muls.w  (A0)+, D5 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(5), (uint16_t)value); break;
    case 0xC0DB08: /* muls.w  (A0)+, D6 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC0DB0A: /* muls.w  (A0)+, D0 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC0DB0C: /* add.l   D6, D0 */
        value = D(6); step_add_long(&D(0), value); break;
    case 0xC0DB0E: /* add.l   D5, D0 */
        value = D(5); step_add_long(&D(0), value); break;
    case 0xC0DB10: /* asr.l   #8, D0 */
        step_asr_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC0DB12: /* move.w  D2, D5 */
        value = D(2); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC0DB14: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0DB16: /* move.w  D4, D1 */
        value = D(4); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC0DB18: /* muls.w  (A0)+, D5 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(5), (uint16_t)value); break;
    case 0xC0DB1A: /* muls.w  (A0)+, D6 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC0DB1C: /* muls.w  (A0)+, D1 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC0DB1E: /* add.l   D6, D1 */
        value = D(6); step_add_long(&D(1), value); break;
    case 0xC0DB20: /* add.l   D5, D1 */
        value = D(5); step_add_long(&D(1), value); break;
    case 0xC0DB22: /* asr.l   #8, D1 */
        step_asr_long(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC0DB24: /* muls.w  (A0)+, D2 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC0DB26: /* muls.w  (A0)+, D3 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC0DB28: /* muls.w  (A0)+, D4 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(4), (uint16_t)value); break;
    case 0xC0DB2A: /* add.l   D3, D2 */
        value = D(3); step_add_long(&D(2), value); break;
    case 0xC0DB2C: /* add.l   D4, D2 */
        value = D(4); step_add_long(&D(2), value); break;
    case 0xC0DB2E: /* asr.l   #8, D2 */
        step_asr_long(&D(2), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC0DB30: /* moveq   #$8, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC0DB32: /* move.w  #$9, $c45954.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC0DB3A: /* jsr     $c2ec9c.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC0DB40: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2EC70: /* move.w  #$1b, $c4599e.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2EC78: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2EC7E: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2EC80: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2EC82: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2EC84: /* move.l  #$ffffffff, $c45958.l */
        value = m68ki_read_imm_32(); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC2EC8E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2EC90: /* moveq   #-$5, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2EC92: /* bra     $c2ecaa */
        step_branch(pc, opcode, 1); break;
    case 0xC2EC94: /* move.w  $c45ab8.l, D7 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2EC9A: /* bra     $c2ecaa */
        step_branch(pc, opcode, 1); break;
    case 0xC2EC9C: /* moveq   #-$4, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2EC9E: /* bra     $c2ecaa */
        step_branch(pc, opcode, 1); break;
    case 0xC2ECA4: /* moveq   #-$2, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2ECA6: /* bra     $c2ecaa */
        step_branch(pc, opcode, 1); break;
    case 0xC2ECAA: /* cmp.w   D2, D0 */
        value = D(2); result = D(0); step_compare_word(value, result); break;
    case 0xC2ECAC: /* bge     $c2ec82 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ECAE: /* cmp.w   D2, D1 */
        value = D(2); result = D(1); step_compare_word(value, result); break;
    case 0xC2ECB0: /* bge     $c2ec82 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ECB2: /* move.w  D0, D3 */
        value = D(0); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2ECB4: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2ECB6: /* cmp.w   D2, D3 */
        value = D(2); result = D(3); step_compare_word(value, result); break;
    case 0xC2ECB8: /* bge     $c2ec82 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ECBA: /* move.w  D1, D3 */
        value = D(1); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2ECBC: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2ECBE: /* cmp.w   D2, D3 */
        value = D(2); result = D(3); step_compare_word(value, result); break;
    case 0xC2ECC0: /* bge     $c2ec82 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ECC2: /* tst.w   D2 */
        value = D(2); flags_logic_w(value); break;
    case 0xC2ECC4: /* ble     $c2ec70 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2ECC6: /* muls.w  #$a0, D0 */
        value = m68ki_read_imm_16(); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2ECCA: /* divs.w  D2, D0 */
        value = D(2); renderer_divide(&D(0), (uint16_t)value); break;
    case 0xC2ECCC: /* addi.w  #$a0, D0 */
        value = m68ki_read_imm_16(); step_add_word(&D(0), value); break;
    case 0xC2ECD0: /* blt     $c2ed44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2ECD2: /* cmpi.w  #$140, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2ECD6: /* bge     $c2ed4c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ECD8: /* muls.w  #$5a, D1 */
        value = m68ki_read_imm_16(); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2ECDC: /* divs.w  D2, D1 */
        value = D(2); renderer_divide(&D(1), (uint16_t)value); break;
    case 0xC2ECDE: /* addi.w  #$5a, D1 */
        value = m68ki_read_imm_16(); step_add_word(&D(1), value); break;
    case 0xC2ECE2: /* blt     $c2ed48 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2ECE4: /* cmpi.w  #$b4, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC2ECE8: /* bge     $c2ed52 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ECEA: /* subi.w  #$13f, D0 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(0), value); break;
    case 0xC2ECEE: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2ECF0: /* subi.w  #$b3, D1 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(1), value); break;
    case 0xC2ECF4: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2ECF6: /* addq.w  #1, D1 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(1), value); break;
    case 0xC2ECF8: /* cmp.w   $c45984.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); result = D(1); step_compare_word(value, result); break;
    case 0xC2ECFE: /* bgt     $c2ec82 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2ED00: /* movem.w D0-D1, $c45958.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC2ED08: /* tst.w   D7 */
        value = D(7); flags_logic_w(value); break;
    case 0xC2ED0A: /* blt     $c2ed58 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2ED0C: /* move.w  #$30, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2ED10: /* asr.w   D7, D2 */
        renderer_asr_word(&D(2), D(7)); break;
    case 0xC2ED12: /* cmp.w   (-$28,A6), D2 */
        value = m68k_read_memory_16(step_displacement(A(6))); result = D(2); step_compare_word(value, result); break;
    case 0xC2ED16: /* bge     $c2ed34 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ED18: /* move.w  #$50, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2ED1C: /* asr.w   D7, D2 */
        renderer_asr_word(&D(2), D(7)); break;
    case 0xC2ED1E: /* cmp.w   (-$28,A6), D2 */
        value = m68k_read_memory_16(step_displacement(A(6))); result = D(2); step_compare_word(value, result); break;
    case 0xC2ED22: /* bge     $c2ed2c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ED24: /* jsr     $c2f5f4.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2ED2A: /* bra     $c2ed40 */
        step_branch(pc, opcode, 1); break;
    case 0xC2ED2C: /* jsr     $c2f60a.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2ED32: /* bra     $c2ed40 */
        step_branch(pc, opcode, 1); break;
    case 0xC2ED34: /* jsr     $c2f66e.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2ED3A: /* bra     $c2ed40 */
        step_branch(pc, opcode, 1); break;
    case 0xC2ED3C: /* bsr     $c2f1c0 */
        value = opcode & 0xffu ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2ED40: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2ED42: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2ED44: /* clr.w   D0 */
        SET_W(D(0), 0); flags_logic_w(0); break;
    case 0xC2ED46: /* bra     $c2ecd8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2ED48: /* clr.w   D1 */
        SET_W(D(1), 0); flags_logic_w(0); break;
    case 0xC2ED4A: /* bra     $c2ecea */
        step_branch(pc, opcode, 1); break;
    case 0xC2ED4C: /* move.w  #$13f, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2ED50: /* bra     $c2ecd8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2ED52: /* move.w  #$b3, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2ED56: /* bra     $c2ecea */
        step_branch(pc, opcode, 1); break;
    case 0xC2ED58: /* addq.w  #1, D7 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(7), value); break;
    case 0xC2ED5A: /* bge     $c2ed24 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ED5C: /* addq.w  #1, D7 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(7), value); break;
    case 0xC2ED5E: /* bge     $c2ed2c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ED60: /* addq.w  #1, D7 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(7), value); break;
    case 0xC2ED62: /* bge     $c2ed34 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ED64: /* addq.w  #1, D7 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(7), value); break;
    case 0xC2ED66: /* bge     $c2ed3c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2ED68: /* move.w  D0, D0 */
        value = D(0); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2ED6A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F1B8: /* jsr     $c2f60a.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2F1BE: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F1C0: /* move.w  D6, D5 */
        value = D(6); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2F1C2: /* subq.w  #1, D5 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(5), value); break;
    case 0xC2F1C4: /* blt     $c2f1b8 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F1C6: /* link    A6, #-$8 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC2F1CA: /* movea.l $c4fe1c.l, A2 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(2) = value; break;
    case 0xC2F1D0: /* subq.w  #1, D5 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(5), value); break;
    case 0xC2F1D2: /* bge     $c2f1e0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F1D4: /* move.w  D6, (A2)+ */
        value = D(6); step_write_word(A(2), value); A(2) += 2; flags_logic_w(value); break;
    case 0xC2F1D6: /* move.l  #$ffff0001, (A2) */
        value = m68ki_read_imm_32(); step_write_long(A(2), value); flags_logic_l(value); break;
    case 0xC2F1DC: /* bra     $c2f250 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F1E0: /* cmpi.w  #$7f, D6 */
        value = m68ki_read_imm_16(); result = D(6); step_compare_word(value, result); break;
    case 0xC2F1E4: /* ble     $c2f1ea */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2F1E6: /* move.w  #$7f, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F1EA: /* move.w  D6, (A2)+ */
        value = D(6); step_write_word(A(2), value); A(2) += 2; flags_logic_w(value); break;
    case 0xC2F1EC: /* move.w  D6, D2 */
        value = D(6); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2F1EE: /* move.w  D2, D5 */
        value = D(2); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2F1F0: /* add.w   D5, D5 */
        value = D(5); step_add_word(&D(5), value); break;
    case 0xC2F1F2: /* add.w   D5, D5 */
        value = D(5); step_add_word(&D(5), value); break;
    case 0xC2F1F4: /* lea     (A2,D5.w), A3 */
        A(3) = step_indexed(A(2)); break;
    case 0xC2F1F8: /* clr.w   D3 */
        SET_W(D(3), 0); flags_logic_w(0); break;
    case 0xC2F1FA: /* moveq   #$3, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC2F1FC: /* sub.w   D2, D4 */
        value = D(2); step_subtract_word(&D(4), value); break;
    case 0xC2F1FE: /* sub.w   D2, D4 */
        value = D(2); step_subtract_word(&D(4), value); break;
    case 0xC2F200: /* cmp.w   D2, D3 */
        value = D(2); result = D(3); step_compare_word(value, result); break;
    case 0xC2F202: /* bge     $c2f242 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F204: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2F206: /* move.w  D3, (A2) */
        value = D(3); step_write_word(A(2), value); flags_logic_w(value); break;
    case 0xC2F208: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2F20A: /* move.w  D3, ($2,A2) */
        value = D(3); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F20E: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2F210: /* move.w  D2, (A3) */
        value = D(2); step_write_word(A(3), value); flags_logic_w(value); break;
    case 0xC2F212: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2F214: /* move.w  D2, ($2,A3) */
        value = D(2); step_write_word(step_displacement(A(3)), value); flags_logic_w(value); break;
    case 0xC2F218: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2F21A: /* bge     $c2f228 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F21C: /* add.w   D3, D4 */
        value = D(3); step_add_word(&D(4), value); break;
    case 0xC2F21E: /* add.w   D3, D4 */
        value = D(3); step_add_word(&D(4), value); break;
    case 0xC2F220: /* add.w   D3, D4 */
        value = D(3); step_add_word(&D(4), value); break;
    case 0xC2F222: /* add.w   D3, D4 */
        value = D(3); step_add_word(&D(4), value); break;
    case 0xC2F224: /* addq.w  #6, D4 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(4), value); break;
    case 0xC2F226: /* bra     $c2f23a */
        step_branch(pc, opcode, 1); break;
    case 0xC2F228: /* move.w  D3, D5 */
        value = D(3); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2F22A: /* sub.w   D2, D5 */
        value = D(2); step_subtract_word(&D(5), value); break;
    case 0xC2F22C: /* add.w   D5, D5 */
        value = D(5); step_add_word(&D(5), value); break;
    case 0xC2F22E: /* add.w   D5, D5 */
        value = D(5); step_add_word(&D(5), value); break;
    case 0xC2F230: /* addi.w  #$a, D5 */
        value = m68ki_read_imm_16(); step_add_word(&D(5), value); break;
    case 0xC2F234: /* add.w   D5, D4 */
        value = D(5); step_add_word(&D(4), value); break;
    case 0xC2F236: /* subq.w  #1, D2 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(2), value); break;
    case 0xC2F238: /* addq.w  #4, A2 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; A(2) += value; break;
    case 0xC2F23A: /* subq.w  #4, A3 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; A(3) -= value; break;
    case 0xC2F23C: /* addq.w  #1, D3 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(3), value); break;
    case 0xC2F23E: /* cmp.w   D2, D3 */
        value = D(2); result = D(3); step_compare_word(value, result); break;
    case 0xC2F240: /* blt     $c2f204 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F242: /* cmp.w   D2, D3 */
        value = D(2); result = D(3); step_compare_word(value, result); break;
    case 0xC2F244: /* bne     $c2f250 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2F246: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2F248: /* move.w  D3, (A2) */
        value = D(3); step_write_word(A(2), value); flags_logic_w(value); break;
    case 0xC2F24A: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2F24C: /* move.w  D3, ($2,A2) */
        value = D(3); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F250: /* move.w  D0, (-$6,A6) */
        value = D(0); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC2F254: /* movea.l $c4fe1c.l, A4 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(4) = value; break;
    case 0xC2F25A: /* move.w  (A4)+, D4 */
        value = m68k_read_memory_16(A(4)); A(4) += 2; SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2F25C: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F25E: /* subq.w  #1, D4 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(4), value); break;
    case 0xC2F260: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC2F262: /* cmpi.w  #$1, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC2F266: /* bge     $c2f29c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F268: /* subq.w  #1, D1 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(1), value); break;
    case 0xC2F26A: /* add.w   D1, D4 */
        value = D(1); step_add_word(&D(4), value); break;
    case 0xC2F26C: /* bge     $c2f274 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F26E: /* add.w   D4, D6 */
        value = D(4); step_add_word(&D(6), value); break;
    case 0xC2F270: /* ble     $c2f47e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2F274: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2F276: /* asl.w   #2, D1 */
        renderer_asl_word(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC2F278: /* adda.w  D1, A4 */
        value = D(1); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2F27A: /* moveq   #$1, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC2F27C: /* bra     $c2f29c */
        step_branch(pc, opcode, 1); break;
    case 0xC2F27E: /* move.w  D1, D5 */
        value = D(1); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2F280: /* sub.w   $c45984.l, D5 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_subtract_word(&D(5), value); break;
    case 0xC2F286: /* sub.w   D6, D1 */
        value = D(6); step_subtract_word(&D(1), value); break;
    case 0xC2F288: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC2F28A: /* cmp.w   $c45984.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); result = D(1); step_compare_word(value, result); break;
    case 0xC2F290: /* bgt     $c2f47e */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2F294: /* sub.w   D5, D6 */
        value = D(5); step_subtract_word(&D(6), value); break;
    case 0xC2F296: /* bge     $c2f2ac */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F298: /* add.w   D6, D4 */
        value = D(6); step_add_word(&D(4), value); break;
    case 0xC2F29A: /* bra     $c2f2ac */
        step_branch(pc, opcode, 1); break;
    case 0xC2F29C: /* add.w   D4, D1 */
        value = D(4); step_add_word(&D(1), value); break;
    case 0xC2F29E: /* add.w   D6, D1 */
        value = D(6); step_add_word(&D(1), value); break;
    case 0xC2F2A0: /* cmp.w   $c45984.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); result = D(1); step_compare_word(value, result); break;
    case 0xC2F2A6: /* bge     $c2f27e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F2A8: /* sub.w   D6, D1 */
        value = D(6); step_subtract_word(&D(1), value); break;
    case 0xC2F2AA: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC2F2AC: /* move.w  D1, D5 */
        value = D(1); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2F2AE: /* asl.w   #3, D5 */
        renderer_asl_word(&D(5), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC2F2B0: /* move.w  D5, D1 */
        value = D(5); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2F2B2: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC2F2B4: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC2F2B6: /* add.w   D5, D1 */
        value = D(5); step_add_word(&D(1), value); break;
    case 0xC2F2B8: /* movea.l $c456b6.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC2F2BE: /* movea.l ($c,A1), A1 */
        value = m68k_read_memory_32(step_displacement(A(1))); A(1) = value; break;
    case 0xC2F2C2: /* adda.w  D1, A1 */
        value = D(1); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2F2C4: /* lea     $dff000.l, A2 */
        A(2) = m68ki_read_imm_32(); break;
    case 0xC2F2CA: /* btst    #$6, ($2,A2) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(2)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC2F2D0: /* beq     $c2f2d8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F2D2: /* nop */
        break;
    case 0xC2F2D4: /* nop */
        break;
    case 0xC2F2D6: /* bra     $c2f2ca */
        step_branch(pc, opcode, 1); break;
    case 0xC2F2D8: /* move.w  #$0, ($42,A2) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F2DE: /* move.w  #$ffff, ($74,A2) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F2E4: /* move.w  $c45954.l, (-$2,A6) */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC2F2EC: /* move.w  (-$2,A6), (-$4,A6) */
        value = m68k_read_memory_16(step_displacement(A(6))); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC2F2F2: /* move.w  (A4), D0 */
        value = m68k_read_memory_16(A(4)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F2F4: /* add.w   (-$6,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); step_add_word(&D(0), value); break;
    case 0xC2F2F8: /* bge     $c2f2fc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F2FA: /* clr.w   D0 */
        SET_W(D(0), 0); flags_logic_w(0); break;
    case 0xC2F2FC: /* move.w  ($2,A4), D7 */
        value = m68k_read_memory_16(step_displacement(A(4))); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2F300: /* add.w   (-$6,A6), D7 */
        value = m68k_read_memory_16(step_displacement(A(6))); step_add_word(&D(7), value); break;
    case 0xC2F304: /* cmpi.w  #$140, D7 */
        value = m68ki_read_imm_16(); result = D(7); step_compare_word(value, result); break;
    case 0xC2F308: /* blt     $c2f30e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F30A: /* move.w  #$13f, D7 */
        value = m68ki_read_imm_16(); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2F30E: /* sub.w   D0, D7 */
        value = D(0); step_subtract_word(&D(7), value); break;
    case 0xC2F310: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2F312: /* andi.w  #$f, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC2F316: /* andi.w  #$fff0, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_W(D(1), result); flags_logic_w(result); break;
    case 0xC2F31A: /* asr.w   #3, D1 */
        renderer_asr_word(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC2F31C: /* lea     (A1,D1.w), A0 */
        A(0) = step_indexed(A(1)); break;
    case 0xC2F320: /* move.w  D7, D1 */
        value = D(7); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2F322: /* move.w  D0, D7 */
        value = D(0); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2F324: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2F326: /* addi.w  #$10, D0 */
        value = m68ki_read_imm_16(); step_add_word(&D(0), value); break;
    case 0xC2F32A: /* cmp.w   D1, D0 */
        value = D(1); result = D(0); step_compare_word(value, result); break;
    case 0xC2F32C: /* ble     $c2f332 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2F32E: /* move.w  D1, D0 */
        value = D(1); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F330: /* addq.w  #1, D0 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(0), value); break;
    case 0xC2F332: /* sub.w   D0, D1 */
        value = D(0); step_subtract_word(&D(1), value); break;
    case 0xC2F334: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2F336: /* move.w  ($a,PC,D0.w), D0 */
        value = m68k_read_memory_16(step_indexed(pc + 2)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F33A: /* ror.w   D7, D0 */
        marker_rotate_word(&D(0), D(7)); break;
    case 0xC2F33C: /* move.w  D0, ($44,A2) */
        value = D(0); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F340: /* bra     $c2f364 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F364: /* move.w  #$41, D7 */
        value = m68ki_read_imm_16(); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2F368: /* cmpi.w  #$f, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC2F36C: /* blt     $c2f37c */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F36E: /* move.w  D1, D0 */
        value = D(1); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F370: /* addq.w  #1, D0 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(0), value); break;
    case 0xC2F372: /* andi.w  #$fff0, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC2F376: /* sub.w   D0, D1 */
        value = D(0); step_subtract_word(&D(1), value); break;
    case 0xC2F378: /* asr.w   #4, D0 */
        renderer_asr_word(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC2F37A: /* add.w   D0, D7 */
        value = D(0); step_add_word(&D(7), value); break;
    case 0xC2F37C: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC2F37E: /* blt     $c2f3aa */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F380: /* addq.w  #1, D7 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_add_word(&D(7), value); break;
    case 0xC2F382: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC2F384: /* move.w  ($4,PC,D1.w), D0 */
        value = m68k_read_memory_16(step_indexed(pc + 2)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F388: /* bra     $c2f3ac */
        step_branch(pc, opcode, 1); break;
    case 0xC2F3AA: /* moveq   #-$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2F3AC: /* move.w  D0, ($46,A2) */
        value = D(0); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F3B0: /* lsr.w   (-$4,A6) */
        address = step_displacement(A(6)); result = m68k_read_memory_16(address); value = result >> 1; flags_logic_w(value); FLAG_X = FLAG_C = (result & 1u) << 8; step_write_word(address, value); break;
    case 0xC2F3B4: /* bcc     $c2f3bc */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2F3B6: /* move.w  #$3fa, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F3BA: /* bra     $c2f3c0 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F3BC: /* move.w  #$30a, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F3C0: /* move.w  D0, ($40,A2) */
        value = D(0); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F3C4: /* move.l  A0, ($48,A2) */
        value = A(0); step_write_long(step_displacement(A(2)), value); flags_logic_l(value); break;
    case 0xC2F3C8: /* move.l  A0, ($54,A2) */
        value = A(0); step_write_long(step_displacement(A(2)), value); flags_logic_l(value); break;
    case 0xC2F3CC: /* move.w  D7, ($58,A2) */
        value = D(7); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F3D0: /* lea     ($1f40,A0), A0 */
        A(0) = step_displacement(A(0)); break;
    case 0xC2F3D4: /* lsr.w   (-$4,A6) */
        address = step_displacement(A(6)); result = m68k_read_memory_16(address); value = result >> 1; flags_logic_w(value); FLAG_X = FLAG_C = (result & 1u) << 8; step_write_word(address, value); break;
    case 0xC2F3D8: /* bcc     $c2f3e0 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2F3DA: /* move.w  #$3fa, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F3DE: /* bra     $c2f3e4 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F3E0: /* move.w  #$30a, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F3E4: /* btst    #$6, ($2,A2) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(2)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC2F3EA: /* beq     $c2f3f2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F3EC: /* nop */
        break;
    case 0xC2F3EE: /* nop */
        break;
    case 0xC2F3F0: /* bra     $c2f3e4 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F3F2: /* move.w  D0, ($40,A2) */
        value = D(0); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F3F6: /* move.l  A0, ($48,A2) */
        value = A(0); step_write_long(step_displacement(A(2)), value); flags_logic_l(value); break;
    case 0xC2F3FA: /* move.l  A0, ($54,A2) */
        value = A(0); step_write_long(step_displacement(A(2)), value); flags_logic_l(value); break;
    case 0xC2F3FE: /* move.w  D7, ($58,A2) */
        value = D(7); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F402: /* lea     ($1f40,A0), A0 */
        A(0) = step_displacement(A(0)); break;
    case 0xC2F406: /* lsr.w   (-$4,A6) */
        address = step_displacement(A(6)); result = m68k_read_memory_16(address); value = result >> 1; flags_logic_w(value); FLAG_X = FLAG_C = (result & 1u) << 8; step_write_word(address, value); break;
    case 0xC2F40A: /* bcc     $c2f412 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2F40C: /* move.w  #$3fa, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F410: /* bra     $c2f416 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F412: /* move.w  #$30a, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F416: /* btst    #$6, ($2,A2) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(2)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC2F41C: /* beq     $c2f424 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F41E: /* nop */
        break;
    case 0xC2F420: /* nop */
        break;
    case 0xC2F422: /* bra     $c2f416 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F424: /* move.w  D0, ($40,A2) */
        value = D(0); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F428: /* move.l  A0, ($48,A2) */
        value = A(0); step_write_long(step_displacement(A(2)), value); flags_logic_l(value); break;
    case 0xC2F42C: /* move.l  A0, ($54,A2) */
        value = A(0); step_write_long(step_displacement(A(2)), value); flags_logic_l(value); break;
    case 0xC2F430: /* move.w  D7, ($58,A2) */
        value = D(7); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F434: /* lea     ($1f40,A0), A0 */
        A(0) = step_displacement(A(0)); break;
    case 0xC2F438: /* lsr.w   (-$4,A6) */
        address = step_displacement(A(6)); result = m68k_read_memory_16(address); value = result >> 1; flags_logic_w(value); FLAG_X = FLAG_C = (result & 1u) << 8; step_write_word(address, value); break;
    case 0xC2F43C: /* bcc     $c2f444 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2F43E: /* move.w  #$3fa, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F442: /* bra     $c2f448 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F444: /* move.w  #$30a, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F448: /* btst    #$6, ($2,A2) */
        value = m68ki_read_imm_16(); value = 1u << (value & 7u); address = step_displacement(A(2)); result = m68k_read_memory_8(address); FLAG_Z = result & value; break;
    case 0xC2F44E: /* beq     $c2f456 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F450: /* nop */
        break;
    case 0xC2F452: /* nop */
        break;
    case 0xC2F454: /* bra     $c2f448 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F456: /* move.w  D0, ($40,A2) */
        value = D(0); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F45A: /* move.l  A0, ($48,A2) */
        value = A(0); step_write_long(step_displacement(A(2)), value); flags_logic_l(value); break;
    case 0xC2F45E: /* move.l  A0, ($54,A2) */
        value = A(0); step_write_long(step_displacement(A(2)), value); flags_logic_l(value); break;
    case 0xC2F462: /* move.w  D7, ($58,A2) */
        value = D(7); step_write_word(step_displacement(A(2)), value); flags_logic_w(value); break;
    case 0xC2F466: /* adda.w  #$28, A1 */
        value = m68ki_read_imm_16(); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2F46A: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2F46C: /* blt     $c2f476 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F46E: /* addq.w  #4, A4 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; A(4) += value; break;
    case 0xC2F470: /* subq.w  #1, D4 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(4), value); break;
    case 0xC2F472: /* bge     $c2f2ec */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F476: /* subq.w  #4, A4 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; A(4) -= value; break;
    case 0xC2F478: /* subq.w  #1, D6 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(6), value); break;
    case 0xC2F47A: /* bge     $c2f2ec */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F47E: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC2F480: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C2EC90_step(void) { return glue_C0DAEE_step(); }

int glue_C2EC94_step(void) { return glue_C0DAEE_step(); }

int glue_C2EC9C_step(void) { return glue_C0DAEE_step(); }

int glue_C2ECA4_step(void) { return glue_C0DAEE_step(); }

int glue_C2F1C0_step(void) { return glue_C0DAEE_step(); }

int glue_C06C02_step(void) { return glue_C0DAEE_step(); }
