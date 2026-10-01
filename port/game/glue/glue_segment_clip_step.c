/* Projected segment clipping and projection from render_line.c, with the
 * four truncated plane helpers from clip.c and their shared source exits.
 * Source calls, arithmetic, bus accesses and instruction boundaries stay
 * in this CPU bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

int glue_C2EE4A_step(void) {
    uint32_t pc = REG_PC, value, address, result, temporary;
    uint16_t opcode = step_begin(pc), mask, word;
    switch (pc) {
    case 0xC2EE44: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC2EE46: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2EE48: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2EE4A: /* link    A6, #-$4 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC2EE4E: /* lea     $c4c592.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2EE54: /* lea     $c4b390.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC2EE5A: /* move.w  #$1, (-$2,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC2EE60: /* movem.w (A1), D3-D5 */
        mask = m68ki_read_imm_16(); renderer_load(A(1), mask, 2, -1); break;
    case 0xC2EE64: /* cmp.w   D5, D3 */
        value = D(5); step_compare_word(value, D(3)); break;
    case 0xC2EE66: /* blt     $c2ee94 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EE68: /* move.w  ($6,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2EE6C: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EE70: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EE72: /* ble     $c2ee44 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EE74: /* bsr     $c2f0c6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EE78: /* beq     $c2eff6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EE7C: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2EE82: /* bge     $c2eeca */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EE84: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EE86: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EE88: /* ble     $c2eeca */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EE8A: /* bsr     $c2f0f4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EE8E: /* bne     $c2eeca */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2EE90: /* bra     $c2f03a */
        step_branch(pc, opcode, 1); break;
    case 0xC2EE94: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EE96: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2EE98: /* cmp.w   D5, D6 */
        value = D(5); step_compare_word(value, D(6)); break;
    case 0xC2EE9A: /* blt     $c2ef58 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EE9E: /* move.w  ($6,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2EEA2: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EEA6: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EEA8: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EEAA: /* ble     $c2ee44 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EEAC: /* bsr     $c2f0f4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EEB0: /* beq     $c2f02e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EEB4: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2EEBA: /* bge     $c2eeca */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EEBC: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EEBE: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EEC0: /* ble     $c2eeca */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EEC2: /* bsr     $c2f0c6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EEC6: /* beq     $c2eff6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EECA: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2EECC: /* bge     $c2ef12 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EECE: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EED0: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2EED2: /* cmp.w   D5, D6 */
        value = D(5); step_compare_word(value, D(6)); break;
    case 0xC2EED4: /* blt     $c2ee44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EED8: /* move.w  ($8,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2EEDC: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EEE0: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EEE2: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EEE4: /* ble     $c2ee44 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EEE8: /* bsr     $c2f156 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EEEC: /* beq     $c2ef54 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EEEE: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2EEF4: /* bge     $c2ee44 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EEF8: /* cmp.w   D5, D4 */
        value = D(5); step_compare_word(value, D(4)); break;
    case 0xC2EEFA: /* blt     $c2ee44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EEFE: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EF00: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EF02: /* ble     $c2ee44 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EF06: /* bsr     $c2f128 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EF0A: /* bne     $c2ee44 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2EF0E: /* bra     $c2f03a */
        step_branch(pc, opcode, 1); break;
    case 0xC2EF12: /* cmp.w   D5, D4 */
        value = D(5); step_compare_word(value, D(4)); break;
    case 0xC2EF14: /* blt     $c2ee44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EF18: /* move.w  ($8,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2EF1C: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EF20: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EF22: /* ble     $c2ee44 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EF26: /* bsr     $c2f128 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EF2A: /* beq     $c2ef0e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EF2C: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2EF32: /* bge     $c2ee44 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EF36: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EF38: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2EF3A: /* cmp.w   D5, D6 */
        value = D(5); step_compare_word(value, D(6)); break;
    case 0xC2EF3C: /* blt     $c2ee44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EF40: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EF44: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EF46: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EF48: /* ble     $c2ee44 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EF4C: /* bsr     $c2f156 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EF50: /* bne     $c2ee44 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2EF54: /* bra     $c2f03a */
        step_branch(pc, opcode, 1); break;
    case 0xC2EF58: /* cmp.w   D5, D4 */
        value = D(5); step_compare_word(value, D(4)); break;
    case 0xC2EF5A: /* blt     $c2ef88 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EF5C: /* move.w  ($8,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2EF60: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EF64: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EF66: /* ble     $c2ee44 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EF6A: /* bsr     $c2f128 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EF6E: /* beq     $c2ef0e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EF70: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2EF76: /* bge     $c2efbe */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EF78: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EF7A: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EF7C: /* ble     $c2efbe */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EF7E: /* bsr     $c2f156 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EF82: /* bne     $c2efbe */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2EF84: /* bra     $c2f03a */
        step_branch(pc, opcode, 1); break;
    case 0xC2EF88: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EF8A: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2EF8C: /* cmp.w   D5, D6 */
        value = D(5); step_compare_word(value, D(6)); break;
    case 0xC2EF8E: /* blt     $c2f030 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EF92: /* move.w  ($8,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2EF96: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EF9A: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EF9C: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EF9E: /* ble     $c2ee44 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EFA2: /* bsr     $c2f156 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EFA6: /* beq     $c2ef54 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EFA8: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2EFAE: /* bge     $c2efbe */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EFB0: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EFB2: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EFB4: /* ble     $c2efbe */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EFB6: /* bsr     $c2f128 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EFBA: /* beq     $c2ef0e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EFBE: /* tst.w   D3 */
        value = D(3); flags_logic_w(value); break;
    case 0xC2EFC0: /* bge     $c2eff8 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EFC2: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EFC4: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2EFC6: /* cmp.w   D5, D6 */
        value = D(5); step_compare_word(value, D(6)); break;
    case 0xC2EFC8: /* blt     $c2f034 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EFCA: /* move.w  ($6,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2EFCE: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EFD2: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EFD4: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EFD6: /* ble     $c2f034 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EFD8: /* bsr     $c2f0f4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EFDC: /* beq     $c2f02e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EFDE: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2EFE4: /* bge     $c2f034 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EFE6: /* cmp.w   D5, D3 */
        value = D(5); step_compare_word(value, D(3)); break;
    case 0xC2EFE8: /* blt     $c2f034 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EFEA: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2EFEC: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2EFEE: /* ble     $c2f034 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EFF0: /* bsr     $c2f0c6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2EFF4: /* bne     $c2f034 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2EFF6: /* bra     $c2f03a */
        step_branch(pc, opcode, 1); break;
    case 0xC2EFF8: /* cmp.w   D5, D3 */
        value = D(5); step_compare_word(value, D(3)); break;
    case 0xC2EFFA: /* blt     $c2f034 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EFFC: /* move.w  ($6,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2F000: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F004: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2F006: /* ble     $c2f034 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2F008: /* bsr     $c2f0c6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2F00C: /* beq     $c2eff6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F00E: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2F014: /* bge     $c2f034 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F016: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F018: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2F01A: /* cmp.w   D5, D6 */
        value = D(5); step_compare_word(value, D(6)); break;
    case 0xC2F01C: /* blt     $c2f034 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F01E: /* move.w  ($a,A1), D6 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F022: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2F024: /* cmp.w   D2, D6 */
        value = D(2); step_compare_word(value, D(6)); break;
    case 0xC2F026: /* ble     $c2f034 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2F028: /* bsr     $c2f0f4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2F02C: /* bne     $c2f034 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2F02E: /* bra     $c2f03a */
        step_branch(pc, opcode, 1); break;
    case 0xC2F030: /* tst.w   D5 */
        value = D(5); flags_logic_w(value); break;
    case 0xC2F032: /* bge     $c2f042 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F034: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC2F036: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2F038: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F03A: /* movem.w $c45ac6.l, D3-D5 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC2F042: /* tst.w   D5 */
        value = D(5); flags_logic_w(value); break;
    case 0xC2F044: /* ble     $c2f094 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2F046: /* muls.w  #$a0, D3 */
        value = m68ki_read_imm_16(); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC2F04A: /* divs.w  D5, D3 */
        value = D(5); renderer_divide(&D(3), (int16_t)value); break;
    case 0xC2F04C: /* addi.w  #$a0, D3 */
        value = m68ki_read_imm_16(); step_add_word(&D(3), value); break;
    case 0xC2F050: /* blt     $c2f0b2 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F052: /* cmpi.w  #$140, D3 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(3)); break;
    case 0xC2F056: /* bge     $c2f0ba */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F058: /* muls.w  #$5a, D4 */
        value = m68ki_read_imm_16(); renderer_multiply(&D(4), (uint16_t)value); break;
    case 0xC2F05C: /* divs.w  D5, D4 */
        value = D(5); renderer_divide(&D(4), (int16_t)value); break;
    case 0xC2F05E: /* addi.w  #$5a, D4 */
        value = m68ki_read_imm_16(); step_add_word(&D(4), value); break;
    case 0xC2F062: /* blt     $c2f0b6 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F064: /* cmpi.w  #$b4, D4 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(4)); break;
    case 0xC2F068: /* bge     $c2f0c0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F06A: /* subi.w  #$13f, D3 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(3), value); break;
    case 0xC2F06E: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2F070: /* subi.w  #$b3, D4 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(4), value); break;
    case 0xC2F074: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2F076: /* move.w  D3, (A0)+ */
        value = D(3); step_write_word(A(0), value); A(0) += 2; flags_logic_w(value); break;
    case 0xC2F078: /* move.w  D4, (A0)+ */
        value = D(4); step_write_word(A(0), value); A(0) += 2; flags_logic_w(value); break;
    case 0xC2F07A: /* subq.w  #1, (-$2,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_16(address); step_subtract_word(&result, value); step_write_word(address, result); break;
    case 0xC2F07E: /* beq     $c2f0a0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F080: /* movem.w $c4b390.l, D0-D3 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC2F088: /* jsr     $c2fa7e.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2F08E: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC2F090: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2F092: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F094: /* move.w  #$16, $c4599e.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2F09C: /* bra     $c2ee44 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F0A0: /* movem.w (A1), D0-D5 */
        mask = m68ki_read_imm_16(); renderer_load(A(1), mask, 2, -1); break;
    case 0xC2F0A4: /* movem.w D3-D5, (A1) */
        mask = m68ki_read_imm_16(); renderer_store(A(1), mask, 2, -1); break;
    case 0xC2F0A8: /* movem.w D0-D2, ($6,A1) */
        mask = m68ki_read_imm_16(); renderer_store(step_displacement(A(1)), mask, 2, -1); break;
    case 0xC2F0AE: /* bra     $c2ee60 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F0B2: /* clr.w   D3 */
        value = 0; SET_W(D(3), value); flags_logic_w(0); break;
    case 0xC2F0B4: /* bra     $c2f058 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F0B6: /* clr.w   D4 */
        value = 0; SET_W(D(4), value); flags_logic_w(0); break;
    case 0xC2F0B8: /* bra     $c2f06a */
        step_branch(pc, opcode, 1); break;
    case 0xC2F0BA: /* move.w  #$13f, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2F0BE: /* bra     $c2f058 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F0C0: /* move.w  #$b3, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2F0C4: /* bra     $c2f06a */
        step_branch(pc, opcode, 1); break;
    case 0xC2F0C6: /* movem.l D0-D6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2F0CA: /* movem.w ($6,A1), D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(step_displacement(A(1)), mask, 2, -1); break;
    case 0xC2F0D0: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F0D2: /* sub.w   D5, D6 */
        value = D(5); step_subtract_word(&D(6), value); break;
    case 0xC2F0D4: /* sub.w   D2, D0 */
        value = D(2); step_subtract_word(&D(0), value); break;
    case 0xC2F0D6: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2F0D8: /* sub.w   D5, D3 */
        value = D(5); step_subtract_word(&D(3), value); break;
    case 0xC2F0DA: /* add.w   D0, D3 */
        value = D(0); step_add_word(&D(3), value); break;
    case 0xC2F0DC: /* beq     $c2f0dc */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F0DE: /* sub.w   D1, D4 */
        value = D(1); step_subtract_word(&D(4), value); break;
    case 0xC2F0E0: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2F0E2: /* muls.w  D0, D4 */
        value = D(0); renderer_multiply(&D(4), (uint16_t)value); break;
    case 0xC2F0E4: /* divs.w  D3, D4 */
        value = D(3); renderer_divide(&D(4), (int16_t)value); break;
    case 0xC2F0E6: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC2F0E8: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2F0EA: /* divs.w  D3, D6 */
        value = D(3); renderer_divide(&D(6), (int16_t)value); break;
    case 0xC2F0EC: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2F0EE: /* move.w  D2, D0 */
        value = D(2); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F0F0: /* bra     $c2f186 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F0F4: /* movem.l D0-D6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2F0F8: /* movem.w ($6,A1), D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(step_displacement(A(1)), mask, 2, -1); break;
    case 0xC2F0FE: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2F100: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2F102: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F104: /* sub.w   D5, D6 */
        value = D(5); step_subtract_word(&D(6), value); break;
    case 0xC2F106: /* sub.w   D2, D0 */
        value = D(2); step_subtract_word(&D(0), value); break;
    case 0xC2F108: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2F10A: /* sub.w   D5, D3 */
        value = D(5); step_subtract_word(&D(3), value); break;
    case 0xC2F10C: /* add.w   D0, D3 */
        value = D(0); step_add_word(&D(3), value); break;
    case 0xC2F10E: /* beq     $c2f10e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F110: /* sub.w   D1, D4 */
        value = D(1); step_subtract_word(&D(4), value); break;
    case 0xC2F112: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2F114: /* muls.w  D0, D4 */
        value = D(0); renderer_multiply(&D(4), (uint16_t)value); break;
    case 0xC2F116: /* divs.w  D3, D4 */
        value = D(3); renderer_divide(&D(4), (int16_t)value); break;
    case 0xC2F118: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC2F11A: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2F11C: /* divs.w  D3, D6 */
        value = D(3); renderer_divide(&D(6), (int16_t)value); break;
    case 0xC2F11E: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2F120: /* move.w  D2, D0 */
        value = D(2); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F122: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2F124: /* bra     $c2f186 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F128: /* movem.l D0-D6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2F12C: /* movem.w ($6,A1), D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(step_displacement(A(1)), mask, 2, -1); break;
    case 0xC2F132: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F134: /* sub.w   D5, D6 */
        value = D(5); step_subtract_word(&D(6), value); break;
    case 0xC2F136: /* sub.w   D2, D1 */
        value = D(2); step_subtract_word(&D(1), value); break;
    case 0xC2F138: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2F13A: /* sub.w   D5, D4 */
        value = D(5); step_subtract_word(&D(4), value); break;
    case 0xC2F13C: /* add.w   D1, D4 */
        value = D(1); step_add_word(&D(4), value); break;
    case 0xC2F13E: /* beq     $c2f13e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F140: /* sub.w   D0, D3 */
        value = D(0); step_subtract_word(&D(3), value); break;
    case 0xC2F142: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2F144: /* muls.w  D1, D3 */
        value = D(1); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC2F146: /* divs.w  D4, D3 */
        value = D(4); renderer_divide(&D(3), (int16_t)value); break;
    case 0xC2F148: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC2F14A: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2F14C: /* divs.w  D4, D6 */
        value = D(4); renderer_divide(&D(6), (int16_t)value); break;
    case 0xC2F14E: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2F150: /* move.w  D2, D1 */
        value = D(2); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2F152: /* bra     $c2f186 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F156: /* movem.l D0-D6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2F15A: /* movem.w ($6,A1), D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(step_displacement(A(1)), mask, 2, -1); break;
    case 0xC2F160: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2F162: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2F164: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F166: /* sub.w   D5, D6 */
        value = D(5); step_subtract_word(&D(6), value); break;
    case 0xC2F168: /* sub.w   D2, D1 */
        value = D(2); step_subtract_word(&D(1), value); break;
    case 0xC2F16A: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2F16C: /* sub.w   D5, D4 */
        value = D(5); step_subtract_word(&D(4), value); break;
    case 0xC2F16E: /* add.w   D1, D4 */
        value = D(1); step_add_word(&D(4), value); break;
    case 0xC2F170: /* beq     $c2f170 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F172: /* sub.w   D0, D3 */
        value = D(0); step_subtract_word(&D(3), value); break;
    case 0xC2F174: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2F176: /* muls.w  D1, D3 */
        value = D(1); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC2F178: /* divs.w  D4, D3 */
        value = D(4); renderer_divide(&D(3), (int16_t)value); break;
    case 0xC2F17A: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC2F17C: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2F17E: /* divs.w  D4, D6 */
        value = D(4); renderer_divide(&D(6), (int16_t)value); break;
    case 0xC2F180: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2F182: /* move.w  D2, D1 */
        value = D(2); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2F184: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2F186: /* movem.w D0-D2, $c45ac6.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC2F18E: /* move.w  D2, D5 */
        value = D(2); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2F190: /* blt     $c2f1a8 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F192: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F194: /* cmp.w   D5, D0 */
        value = D(5); step_compare_word(value, D(0)); break;
    case 0xC2F196: /* bgt     $c2f1a8 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2F198: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2F19A: /* cmp.w   D5, D0 */
        value = D(5); step_compare_word(value, D(0)); break;
    case 0xC2F19C: /* bgt     $c2f1a8 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2F19E: /* cmp.w   D6, D1 */
        value = D(6); step_compare_word(value, D(1)); break;
    case 0xC2F1A0: /* bgt     $c2f1a8 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2F1A2: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2F1A4: /* cmp.w   D6, D1 */
        value = D(6); step_compare_word(value, D(1)); break;
    case 0xC2F1A6: /* ble     $c2f1b0 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2F1A8: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2F1AA: /* movem.l (A7)+, D0-D6 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC2F1AE: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F1B0: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2F1B2: /* movem.l (A7)+, D0-D6 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC2F1B6: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C2F0C6_step(void) { return glue_C2EE4A_step(); }

int glue_C2F0F4_step(void) { return glue_C2EE4A_step(); }

int glue_C2F128_step(void) { return glue_C2EE4A_step(); }

int glue_C2F156_step(void) { return glue_C2EE4A_step(); }
