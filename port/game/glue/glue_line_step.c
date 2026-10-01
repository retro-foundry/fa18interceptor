/* Four-plane line submission; readable behavior is in render_line.c.
 * Source calls, arithmetic, bus accesses and instruction boundaries stay
 * in this CPU bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

static void line_rotate_word(uint32_t *reg, unsigned count) {
    uint16_t before = (uint16_t)*reg;
    uint16_t result = (uint16_t)((before >> count) | (before << (16 - count)));
    SET_W(*reg, result); flags_logic_w(result);
    FLAG_C = ((before >> (count - 1)) & 1u) << 8;
    USE_CYCLES(count << CYC_SHIFT);
}

int glue_C2FA7E_step(void) {
    uint32_t pc = REG_PC, value, address, result, temporary;
    uint16_t opcode = step_begin(pc), mask, word;
    switch (pc) {
    case 0xC2FA70: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC2FA72: /* addq.w  #1, D3 */
        value = 1u; step_add_word(&D(3), value); break;
    case 0xC2FA74: /* clr.w   D5 */
        value = 0; SET_W(D(5), value); flags_logic_w(0); break;
    case 0xC2FA76: /* bra     $c2fa9c */
        step_branch(pc, opcode, 1); break;
    case 0xC2FA78: /* movea.w #$c7, A2 */
        value = m68ki_read_imm_16(); A(2) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FA7C: /* bra     $c2fa84 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FA7E: /* movea.w $c45984.l, A2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(2) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FA84: /* move.w  $c45954.l, $c45956.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2FA8E: /* cmp.w   D1, D3 */
        value = D(1); step_compare_word(value, D(3)); break;
    case 0xC2FA90: /* beq     $c2fa70 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FA92: /* bls     $c2faba */
        step_branch(pc, opcode, COND_LS()); break;
    case 0xC2FA94: /* move.w  D3, D5 */
        value = D(3); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2FA96: /* sub.w   D1, D5 */
        value = D(1); step_subtract_word(&D(5), value); break;
    case 0xC2FA98: /* subq.w  #1, D5 */
        value = 1u; step_subtract_word(&D(5), value); break;
    case 0xC2FA9A: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC2FA9C: /* cmp.w   A2, D1 */
        value = A(2); step_compare_word(value, D(1)); break;
    case 0xC2FA9E: /* bgt     $c2fab8 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2FAA0: /* movea.w D1, A1 */
        value = D(1); A(1) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FAA2: /* asl.w   #3, D1 */
        renderer_asl_word(&D(1), 3); break;
    case 0xC2FAA4: /* move.w  D1, D7 */
        value = D(1); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2FAA6: /* add.w   D7, D7 */
        value = D(7); step_add_word(&D(7), value); break;
    case 0xC2FAA8: /* add.w   D7, D7 */
        value = D(7); step_add_word(&D(7), value); break;
    case 0xC2FAAA: /* add.w   D1, D7 */
        value = D(1); step_add_word(&D(7), value); break;
    case 0xC2FAAC: /* move.w  D2, D4 */
        value = D(2); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2FAAE: /* sub.w   D0, D4 */
        value = D(0); step_subtract_word(&D(4), value); break;
    case 0xC2FAB0: /* move.w  D0, D6 */
        value = D(0); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2FAB2: /* lsr.w   #3, D0 */
        SET_W(D(0), step_lsr_word_value(D(0), 3)); break;
    case 0xC2FAB4: /* add.w   D0, D7 */
        value = D(0); step_add_word(&D(7), value); break;
    case 0xC2FAB6: /* bra     $c2fadc */
        step_branch(pc, opcode, 1); break;
    case 0xC2FAB8: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2FABA: /* move.w  D1, D5 */
        value = D(1); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2FABC: /* sub.w   D3, D5 */
        value = D(3); step_subtract_word(&D(5), value); break;
    case 0xC2FABE: /* subq.w  #1, D5 */
        value = 1u; step_subtract_word(&D(5), value); break;
    case 0xC2FAC0: /* move.w  D0, D4 */
        value = D(0); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2FAC2: /* sub.w   D2, D4 */
        value = D(2); step_subtract_word(&D(4), value); break;
    case 0xC2FAC4: /* addq.w  #1, D3 */
        value = 1u; step_add_word(&D(3), value); break;
    case 0xC2FAC6: /* cmp.w   A2, D3 */
        value = A(2); step_compare_word(value, D(3)); break;
    case 0xC2FAC8: /* bgt     $c2fab8 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2FACA: /* movea.w D3, A1 */
        value = D(3); A(1) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FACC: /* asl.w   #3, D3 */
        renderer_asl_word(&D(3), 3); break;
    case 0xC2FACE: /* move.w  D3, D7 */
        value = D(3); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2FAD0: /* add.w   D7, D7 */
        value = D(7); step_add_word(&D(7), value); break;
    case 0xC2FAD2: /* add.w   D7, D7 */
        value = D(7); step_add_word(&D(7), value); break;
    case 0xC2FAD4: /* add.w   D3, D7 */
        value = D(3); step_add_word(&D(7), value); break;
    case 0xC2FAD6: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2FAD8: /* lsr.w   #3, D2 */
        SET_W(D(2), step_lsr_word_value(D(2), 3)); break;
    case 0xC2FADA: /* add.w   D2, D7 */
        value = D(2); step_add_word(&D(7), value); break;
    case 0xC2FADC: /* ext.l   D7 */
        D(7) = (uint32_t)(int32_t)(int16_t)D(7); flags_logic_l(D(7)); break;
    case 0xC2FADE: /* moveq   #$1, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC2FAE0: /* andi.w  #$f, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), D(6) & value); flags_logic_w(D(6)); break;
    case 0xC2FAE4: /* ror.w   #4, D6 */
        line_rotate_word(&D(6), 4); break;
    case 0xC2FAE6: /* addi.w  #$b00, D6 */
        value = m68ki_read_imm_16(); step_add_word(&D(6), value); break;
    case 0xC2FAEA: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2FAEC: /* bmi     $c2faf8 */
        step_branch(pc, opcode, COND_MI()); break;
    case 0xC2FAEE: /* cmp.w   D5, D4 */
        value = D(5); step_compare_word(value, D(4)); break;
    case 0xC2FAF0: /* bcs     $c2fb22 */
        step_branch(pc, opcode, COND_CS()); break;
    case 0xC2FAF2: /* addi.w  #$10, D1 */
        value = m68ki_read_imm_16(); step_add_word(&D(1), value); break;
    case 0xC2FAF6: /* bra     $c2fb02 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FAF8: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2FAFA: /* cmp.w   D5, D4 */
        value = D(5); step_compare_word(value, D(4)); break;
    case 0xC2FAFC: /* bcs     $c2fb20 */
        step_branch(pc, opcode, COND_CS()); break;
    case 0xC2FAFE: /* addi.w  #$14, D1 */
        value = m68ki_read_imm_16(); step_add_word(&D(1), value); break;
    case 0xC2FB02: /* suba.w  A1, A2 */
        value = A(1); A(2) -= (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FB04: /* cmp.w   A2, D5 */
        value = A(2); step_compare_word(value, D(5)); break;
    case 0xC2FB06: /* ble     $c2fb1c */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2FB08: /* move.w  A2, D2 */
        value = A(2); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2FB0A: /* move.w  D4, D3 */
        value = D(4); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2FB0C: /* muls.w  D2, D3 */
        value = D(2); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC2FB0E: /* add.l   D3, D3 */
        value = D(3); step_add_long(&D(3), value); break;
    case 0xC2FB10: /* divs.w  D5, D3 */
        value = D(5); renderer_divide(&D(3), (int16_t)value); break;
    case 0xC2FB12: /* asr.w   #1, D3 */
        renderer_asr_word(&D(3), 1); break;
    case 0xC2FB14: /* bcc     $c2fb18 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2FB16: /* addq.w  #1, D3 */
        value = 1u; step_add_word(&D(3), value); break;
    case 0xC2FB18: /* movea.w D3, A2 */
        value = D(3); A(2) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FB1A: /* bra     $c2fb2c */
        step_branch(pc, opcode, 1); break;
    case 0xC2FB1C: /* movea.w D4, A2 */
        value = D(4); A(2) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FB1E: /* bra     $c2fb2c */
        step_branch(pc, opcode, 1); break;
    case 0xC2FB20: /* addq.w  #8, D1 */
        value = 8u; step_add_word(&D(1), value); break;
    case 0xC2FB22: /* exg     D4, D5 */
        temporary = D(4); D(4) = D(5); D(5) = temporary; break;
    case 0xC2FB24: /* suba.w  A1, A2 */
        value = A(1); A(2) -= (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FB26: /* cmp.w   A2, D4 */
        value = A(2); step_compare_word(value, D(4)); break;
    case 0xC2FB28: /* bgt     $c2fb2c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2FB2A: /* movea.w D4, A2 */
        value = D(4); A(2) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FB2C: /* add.w   D5, D5 */
        value = D(5); step_add_word(&D(5), value); break;
    case 0xC2FB2E: /* add.w   D5, D5 */
        value = D(5); step_add_word(&D(5), value); break;
    case 0xC2FB30: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC2FB32: /* move.w  D5, D2 */
        value = D(5); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2FB34: /* sub.w   D4, D2 */
        value = D(4); step_subtract_word(&D(2), value); break;
    case 0xC2FB36: /* bge     $c2fb3c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2FB38: /* bset    #$6, D1 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(1) & (1u << value); D(1) |= 1u << value; USE_CYCLES(-(1 << CYC_SHIFT)); break;
    case 0xC2FB3C: /* move.w  D5, D3 */
        value = D(5); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2FB3E: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC2FB40: /* sub.w   D4, D5 */
        value = D(4); step_subtract_word(&D(5), value); break;
    case 0xC2FB42: /* move.w  A2, D4 */
        value = A(2); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2FB44: /* asl.w   #6, D4 */
        renderer_asl_word(&D(4), 6); break;
    case 0xC2FB46: /* addi.w  #$42, D4 */
        value = m68ki_read_imm_16(); step_add_word(&D(4), value); break;
    case 0xC2FB4A: /* moveq   #-$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2FB4C: /* movea.w D5, A3 */
        value = D(5); A(3) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2FB4E: /* lea     $dff000.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC2FB54: /* btst    #$6, ($2,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FB5A: /* beq     $c2fb62 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FB5C: /* nop */
        break;
    case 0xC2FB5E: /* nop */
        break;
    case 0xC2FB60: /* bra     $c2fb54 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FB62: /* move.w  A3, ($64,A0) */
        value = A(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FB66: /* move.w  #$28, ($66,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FB6C: /* move.w  #$28, ($60,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FB72: /* move.l  D0, ($44,A0) */
        value = D(0); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC2FB76: /* move.w  D0, ($72,A0) */
        value = D(0); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FB7A: /* movea.l $c456b6.l, A2 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(2) = value; break;
    case 0xC2FB80: /* movea.l D7, A1 */
        value = D(7); A(1) = value; break;
    case 0xC2FB82: /* btst    #$0, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FB8A: /* beq     $c2fbea */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FB8C: /* move.w  D6, D5 */
        value = D(6); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2FB8E: /* tst.w   $c456e8.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2FB94: /* blt     $c2fba0 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2FB96: /* btst    #$0, $c456e9.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FB9E: /* bra     $c2fba8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FBA0: /* btst    #$0, $c45957.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FBA8: /* beq     $c2fbb0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FBAA: /* addi.w  #$fa, D5 */
        value = m68ki_read_imm_16(); step_add_word(&D(5), value); break;
    case 0xC2FBAE: /* bra     $c2fbb4 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FBB0: /* addi.w  #$a, D5 */
        value = m68ki_read_imm_16(); step_add_word(&D(5), value); break;
    case 0xC2FBB4: /* move.l  ($c,A2), D7 */
        value = m68k_read_memory_32(step_displacement(A(2))); D(7) = value; flags_logic_l(value); break;
    case 0xC2FBB8: /* add.l   A1, D7 */
        value = A(1); step_add_long(&D(7), value); break;
    case 0xC2FBBA: /* btst    #$6, ($2,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FBC0: /* beq     $c2fbc8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FBC2: /* nop */
        break;
    case 0xC2FBC4: /* nop */
        break;
    case 0xC2FBC6: /* bra     $c2fbba */
        step_branch(pc, opcode, 1); break;
    case 0xC2FBC8: /* move.w  D5, ($40,A0) */
        value = D(5); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FBCC: /* move.w  D1, ($42,A0) */
        value = D(1); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FBD0: /* move.w  D2, ($52,A0) */
        value = D(2); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FBD4: /* move.l  D7, ($48,A0) */
        value = D(7); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC2FBD8: /* move.l  D7, ($54,A0) */
        value = D(7); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC2FBDC: /* move.w  #$8000, ($74,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FBE2: /* move.w  D3, ($62,A0) */
        value = D(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FBE6: /* move.w  D4, ($58,A0) */
        value = D(4); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FBEA: /* btst    #$1, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FBF2: /* beq     $c2fc52 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FBF4: /* move.w  D6, D5 */
        value = D(6); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2FBF6: /* tst.w   $c456e8.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2FBFC: /* blt     $c2fc08 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2FBFE: /* btst    #$1, $c456e9.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FC06: /* bra     $c2fc10 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FC08: /* btst    #$1, $c45957.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FC10: /* beq     $c2fc18 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FC12: /* addi.w  #$fa, D5 */
        value = m68ki_read_imm_16(); step_add_word(&D(5), value); break;
    case 0xC2FC16: /* bra     $c2fc1c */
        step_branch(pc, opcode, 1); break;
    case 0xC2FC18: /* addi.w  #$a, D5 */
        value = m68ki_read_imm_16(); step_add_word(&D(5), value); break;
    case 0xC2FC1C: /* move.l  ($8,A2), D7 */
        value = m68k_read_memory_32(step_displacement(A(2))); D(7) = value; flags_logic_l(value); break;
    case 0xC2FC20: /* add.l   A1, D7 */
        value = A(1); step_add_long(&D(7), value); break;
    case 0xC2FC22: /* btst    #$6, ($2,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FC28: /* beq     $c2fc30 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FC2A: /* nop */
        break;
    case 0xC2FC2C: /* nop */
        break;
    case 0xC2FC2E: /* bra     $c2fc22 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FC30: /* move.w  D5, ($40,A0) */
        value = D(5); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FC34: /* move.w  D1, ($42,A0) */
        value = D(1); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FC38: /* move.w  D2, ($52,A0) */
        value = D(2); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FC3C: /* move.l  D7, ($48,A0) */
        value = D(7); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC2FC40: /* move.l  D7, ($54,A0) */
        value = D(7); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC2FC44: /* move.w  #$8000, ($74,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FC4A: /* move.w  D3, ($62,A0) */
        value = D(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FC4E: /* move.w  D4, ($58,A0) */
        value = D(4); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FC52: /* btst    #$2, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FC5A: /* beq     $c2fcba */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FC5C: /* move.w  D6, D5 */
        value = D(6); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2FC5E: /* tst.w   $c456e8.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2FC64: /* blt     $c2fc70 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2FC66: /* btst    #$2, $c456e9.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FC6E: /* bra     $c2fc78 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FC70: /* btst    #$2, $c45957.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FC78: /* beq     $c2fc80 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FC7A: /* addi.w  #$fa, D5 */
        value = m68ki_read_imm_16(); step_add_word(&D(5), value); break;
    case 0xC2FC7E: /* bra     $c2fc84 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FC80: /* addi.w  #$a, D5 */
        value = m68ki_read_imm_16(); step_add_word(&D(5), value); break;
    case 0xC2FC84: /* move.l  ($4,A2), D7 */
        value = m68k_read_memory_32(step_displacement(A(2))); D(7) = value; flags_logic_l(value); break;
    case 0xC2FC88: /* add.l   A1, D7 */
        value = A(1); step_add_long(&D(7), value); break;
    case 0xC2FC8A: /* btst    #$6, ($2,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FC90: /* beq     $c2fc98 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FC92: /* nop */
        break;
    case 0xC2FC94: /* nop */
        break;
    case 0xC2FC96: /* bra     $c2fc8a */
        step_branch(pc, opcode, 1); break;
    case 0xC2FC98: /* move.w  D5, ($40,A0) */
        value = D(5); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FC9C: /* move.w  D1, ($42,A0) */
        value = D(1); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FCA0: /* move.w  D2, ($52,A0) */
        value = D(2); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FCA4: /* move.l  D7, ($48,A0) */
        value = D(7); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC2FCA8: /* move.l  D7, ($54,A0) */
        value = D(7); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC2FCAC: /* move.w  #$8000, ($74,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FCB2: /* move.w  D3, ($62,A0) */
        value = D(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FCB6: /* move.w  D4, ($58,A0) */
        value = D(4); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FCBA: /* btst    #$3, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FCC2: /* beq     $c2fd20 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FCC4: /* move.w  D6, D5 */
        value = D(6); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2FCC6: /* tst.w   $c456e8.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2FCCC: /* blt     $c2fcd8 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2FCCE: /* btst    #$3, $c456e9.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FCD6: /* bra     $c2fce0 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FCD8: /* btst    #$3, $c45957.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FCE0: /* beq     $c2fce8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FCE2: /* addi.w  #$fa, D5 */
        value = m68ki_read_imm_16(); step_add_word(&D(5), value); break;
    case 0xC2FCE6: /* bra     $c2fcec */
        step_branch(pc, opcode, 1); break;
    case 0xC2FCE8: /* addi.w  #$a, D5 */
        value = m68ki_read_imm_16(); step_add_word(&D(5), value); break;
    case 0xC2FCEC: /* move.l  (A2), D7 */
        value = m68k_read_memory_32(A(2)); D(7) = value; flags_logic_l(value); break;
    case 0xC2FCEE: /* add.l   A1, D7 */
        value = A(1); step_add_long(&D(7), value); break;
    case 0xC2FCF0: /* btst    #$6, ($2,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FCF6: /* beq     $c2fcfe */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FCF8: /* nop */
        break;
    case 0xC2FCFA: /* nop */
        break;
    case 0xC2FCFC: /* bra     $c2fcf0 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FCFE: /* move.w  D5, ($40,A0) */
        value = D(5); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FD02: /* move.w  D1, ($42,A0) */
        value = D(1); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FD06: /* move.w  D2, ($52,A0) */
        value = D(2); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FD0A: /* move.l  D7, ($48,A0) */
        value = D(7); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC2FD0E: /* move.l  D7, ($54,A0) */
        value = D(7); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC2FD12: /* move.w  #$8000, ($74,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FD18: /* move.w  D3, ($62,A0) */
        value = D(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FD1C: /* move.w  D4, ($58,A0) */
        value = D(4); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2FD20: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C2FA78_step(void) { return glue_C2FA7E_step(); }
