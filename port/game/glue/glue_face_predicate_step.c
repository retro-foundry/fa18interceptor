/* Face orientation, component bounds and scan-pass line-style timing.
 * Readable game behavior remains in its domain modules; CPU effects stay here. */
#include "glue_renderer_step_math.h"

int glue_C1FB82_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC1FB82: /* move.w  D7, D1 */
        value = D(7); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1FB84: /* andi.w  #$c00, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_W(D(1), result); flags_logic_w(result); break;
    case 0xC1FB88: /* bne     $c1fc3a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1FB8C: /* andi.w  #$3000, D7 */
        value = m68ki_read_imm_16(); result = D(7); result &= value; SET_W(D(7), result); flags_logic_w(result); break;
    case 0xC1FB90: /* beq     $c1fbd4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1FB92: /* movea.l (-$2c,A6), A0 */
        value = m68k_read_memory_32(step_displacement(A(6))); A(0) = value; break;
    case 0xC1FB96: /* adda.w  (A2)+, A0 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1FB98: /* movem.w (A0), D0-D5 */
        mask = m68ki_read_imm_16(); renderer_load(A(0), mask, 2, -1); break;
    case 0xC1FB9C: /* move.w  $c45ab8.l, D7 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1FBA2: /* asr.w   D7, D0 */
        renderer_asr_word(&D(0), D(7)); break;
    case 0xC1FBA4: /* asr.w   D7, D1 */
        renderer_asr_word(&D(1), D(7)); break;
    case 0xC1FBA6: /* asr.w   D7, D2 */
        renderer_asr_word(&D(2), D(7)); break;
    case 0xC1FBA8: /* add.w   $c45b2a.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_add_word(&D(0), value); break;
    case 0xC1FBAE: /* add.w   $c45b2e.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_add_word(&D(2), value); break;
    case 0xC1FBB4: /* sub.w   (-$26,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); step_subtract_word(&D(0), value); break;
    case 0xC1FBB8: /* sub.w   (-$24,A6), D1 */
        value = m68k_read_memory_16(step_displacement(A(6))); step_subtract_word(&D(1), value); break;
    case 0xC1FBBC: /* sub.w   (-$22,A6), D2 */
        value = m68k_read_memory_16(step_displacement(A(6))); step_subtract_word(&D(2), value); break;
    case 0xC1FBC0: /* muls.w  D3, D0 */
        value = D(3); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC1FBC2: /* muls.w  D4, D1 */
        value = D(4); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC1FBC4: /* muls.w  D5, D2 */
        value = D(5); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC1FBC6: /* add.l   D0, D2 */
        value = D(0); step_add_long(&D(2), value); break;
    case 0xC1FBC8: /* add.l   D1, D2 */
        value = D(1); step_add_long(&D(2), value); break;
    case 0xC1FBCA: /* blt     $c1fbd0 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1FBCC: /* moveq   #$1, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC1FBCE: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1FBD0: /* clr.w   D7 */
        SET_W(D(7), 0); flags_logic_w(0); break;
    case 0xC1FBD2: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1FBD4: /* lea     $c4bf94.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1FBDA: /* movem.w (A0), D0-D5 */
        mask = m68ki_read_imm_16(); renderer_load(A(0), mask, 2, -1); break;
    case 0xC1FBDE: /* sub.w   D0, D3 */
        value = D(0); step_subtract_word(&D(3), value); break;
    case 0xC1FBE0: /* sub.w   D1, D4 */
        value = D(1); step_subtract_word(&D(4), value); break;
    case 0xC1FBE2: /* sub.w   D2, D5 */
        value = D(2); step_subtract_word(&D(5), value); break;
    case 0xC1FBE4: /* movem.w ($c,A0), D6-D7 */
        mask = m68ki_read_imm_16(); renderer_load(step_displacement(A(0)), mask, 2, -1); break;
    case 0xC1FBEA: /* sub.w   D0, D6 */
        value = D(0); step_subtract_word(&D(6), value); break;
    case 0xC1FBEC: /* sub.w   D1, D7 */
        value = D(1); step_subtract_word(&D(7), value); break;
    case 0xC1FBEE: /* move.w  ($10,A0), D0 */
        value = m68k_read_memory_16(step_displacement(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1FBF2: /* sub.w   D2, D0 */
        value = D(2); step_subtract_word(&D(0), value); break;
    case 0xC1FBF4: /* move.w  A3, D1 */
        value = A(3); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1FBF6: /* asr.w   #7, D1 */
        renderer_asr_word(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1FBF8: /* andi.w  #$7, D1 */
        value = m68ki_read_imm_16(); result = D(1); result &= value; SET_W(D(1), result); flags_logic_w(result); break;
    case 0xC1FBFC: /* beq     $c1fc0a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1FBFE: /* asl.w   D1, D3 */
        renderer_asl_word(&D(3), D(1)); break;
    case 0xC1FC00: /* asl.w   D1, D4 */
        renderer_asl_word(&D(4), D(1)); break;
    case 0xC1FC02: /* asl.w   D1, D5 */
        renderer_asl_word(&D(5), D(1)); break;
    case 0xC1FC04: /* asl.w   D1, D6 */
        renderer_asl_word(&D(6), D(1)); break;
    case 0xC1FC06: /* asl.w   D1, D7 */
        renderer_asl_word(&D(7), D(1)); break;
    case 0xC1FC08: /* asl.w   D1, D0 */
        renderer_asl_word(&D(0), D(1)); break;
    case 0xC1FC0A: /* move.w  D5, D1 */
        value = D(5); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1FC0C: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1FC0E: /* muls.w  D4, D0 */
        value = D(4); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC1FC10: /* muls.w  D7, D5 */
        value = D(7); renderer_multiply(&D(5), (uint16_t)value); break;
    case 0xC1FC12: /* sub.l   D5, D0 */
        value = D(5); step_subtract_long(&D(0), value); break;
    case 0xC1FC14: /* asr.l   #8, D0 */
        step_asr_long(&D(0), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1FC16: /* muls.w  D3, D2 */
        value = D(3); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC1FC18: /* muls.w  D6, D1 */
        value = D(6); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC1FC1A: /* sub.l   D2, D1 */
        value = D(2); step_subtract_long(&D(1), value); break;
    case 0xC1FC1C: /* asr.l   #8, D1 */
        step_asr_long(&D(1), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1FC1E: /* muls.w  D3, D7 */
        value = D(3); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC1FC20: /* muls.w  D4, D6 */
        value = D(4); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC1FC22: /* sub.l   D6, D7 */
        value = D(6); step_subtract_long(&D(7), value); break;
    case 0xC1FC24: /* asr.l   #8, D7 */
        step_asr_long(&D(7), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1FC26: /* muls.w  (A0)+, D0 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC1FC28: /* muls.w  (A0)+, D1 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC1FC2A: /* muls.w  (A0), D7 */
        value = m68k_read_memory_16(A(0)); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC1FC2C: /* add.l   D0, D7 */
        value = D(0); step_add_long(&D(7), value); break;
    case 0xC1FC2E: /* add.l   D1, D7 */
        value = D(1); step_add_long(&D(7), value); break;
    case 0xC1FC30: /* blt     $c1fc36 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1FC32: /* moveq   #$1, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC1FC34: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1FC36: /* clr.w   D7 */
        SET_W(D(7), 0); flags_logic_w(0); break;
    case 0xC1FC38: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1FC3A: /* move.w  (-$4,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1FC3E: /* andi.w  #$3fff, D0 */
        value = m68ki_read_imm_16(); result = D(0); result &= value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC1FC42: /* movea.l $c45a32.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC1FC48: /* move.w  D7, D1 */
        value = D(7); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1FC4A: /* andi.w  #$c00, D7 */
        value = m68ki_read_imm_16(); result = D(7); result &= value; SET_W(D(7), result); flags_logic_w(result); break;
    case 0xC1FC4E: /* asr.w   #8, D7 */
        renderer_asr_word(&D(7), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1FC50: /* asr.w   #2, D7 */
        renderer_asr_word(&D(7), ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u); break;
    case 0xC1FC52: /* subq.w  #1, D7 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(7), value); break;
    case 0xC1FC54: /* beq     $c1fcae */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1FC56: /* subq.w  #1, D7 */
        value = ((opcode >> 9) & 7u) ? ((opcode >> 9) & 7u) : 8u; step_subtract_word(&D(7), value); break;
    case 0xC1FC58: /* beq     $c1fc84 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1FC5A: /* move.b  ($6,A0), D5 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(5), value); flags_logic_b(value); break;
    case 0xC1FC5E: /* andi.w  #$f, D5 */
        value = m68ki_read_imm_16(); result = D(5); result &= value; SET_W(D(5), result); flags_logic_w(result); break;
    case 0xC1FC62: /* move.w  ($e,A0,D0.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1FC66: /* asr.w   D5, D2 */
        renderer_asr_word(&D(2), D(5)); break;
    case 0xC1FC68: /* move.w  $c45a76.l, D7 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1FC6E: /* neg.w   D7 */
        renderer_negate(&D(7), 2); break;
    case 0xC1FC70: /* move.w  $c45b2e.l, D6 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1FC76: /* move.w  $c45ab8.l, D5 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC1FC7C: /* asl.w   D5, D6 */
        renderer_asl_word(&D(6), D(5)); break;
    case 0xC1FC7E: /* add.w   D6, D2 */
        value = D(6); step_add_word(&D(2), value); break;
    case 0xC1FC80: /* cmp.w   D2, D7 */
        value = D(2); result = D(7); step_compare_word(value, result); break;
    case 0xC1FC82: /* bra     $c1fcc8 */
        step_branch(pc, opcode, 1); break;
    case 0xC1FC84: /* move.b  ($6,A0), D5 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(5), value); flags_logic_b(value); break;
    case 0xC1FC88: /* andi.w  #$f, D5 */
        value = m68ki_read_imm_16(); result = D(5); result &= value; SET_W(D(5), result); flags_logic_w(result); break;
    case 0xC1FC8C: /* move.w  ($a,A0,D0.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1FC90: /* asr.w   D5, D2 */
        renderer_asr_word(&D(2), D(5)); break;
    case 0xC1FC92: /* move.w  $c45a72.l, D7 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1FC98: /* neg.w   D7 */
        renderer_negate(&D(7), 2); break;
    case 0xC1FC9A: /* move.w  $c45b2a.l, D6 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1FCA0: /* move.w  $c45ab8.l, D5 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC1FCA6: /* asl.w   D5, D6 */
        renderer_asl_word(&D(6), D(5)); break;
    case 0xC1FCA8: /* add.w   D6, D2 */
        value = D(6); step_add_word(&D(2), value); break;
    case 0xC1FCAA: /* cmp.w   D2, D7 */
        value = D(2); result = D(7); step_compare_word(value, result); break;
    case 0xC1FCAC: /* bra     $c1fcc8 */
        step_branch(pc, opcode, 1); break;
    case 0xC1FCAE: /* move.b  ($6,A0), D5 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(5), value); flags_logic_b(value); break;
    case 0xC1FCB2: /* andi.w  #$f, D5 */
        value = m68ki_read_imm_16(); result = D(5); result &= value; SET_W(D(5), result); flags_logic_w(result); break;
    case 0xC1FCB6: /* move.w  ($c,A0,D0.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1FCBA: /* asr.w   D5, D2 */
        renderer_asr_word(&D(2), D(5)); break;
    case 0xC1FCBC: /* move.l  $c45a78.l, D7 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(7) = value; flags_logic_l(value); break;
    case 0xC1FCC2: /* neg.l   D7 */
        renderer_negate(&D(7), 4); break;
    case 0xC1FCC4: /* ext.l   D2 */
        D(2) = (uint32_t)(int32_t)(int16_t)D(2); flags_logic_l(D(2)); break;
    case 0xC1FCC6: /* cmp.l   D2, D7 */
        value = D(2); result = D(7); step_compare_long(value, result); break;
    case 0xC1FCC8: /* blt     $c1fcd0 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1FCCA: /* btst    #$c, D1 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(1); FLAG_Z = result & value; break;
    case 0xC1FCCE: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1FCD0: /* btst    #$c, D1 */
        value = m68ki_read_imm_16(); value = 1u << (value & 31u); result = D(1); FLAG_Z = result & value; break;
    case 0xC1FCD4: /* bne     $c1fcda */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1FCD6: /* moveq   #$1, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC1FCD8: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1FCDA: /* clr.w   D7 */
        SET_W(D(7), 0); flags_logic_w(0); break;
    case 0xC1FCDC: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F490: /* move.l #$fffff, $c456e6.l */
        value = m68ki_read_imm_32(); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC2F49A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C1FB8C_step(void) { return glue_C1FB82_step(); }

int glue_C1FB9C_step(void) { return glue_C1FB82_step(); }

int glue_C1FC42_step(void) { return glue_C1FB82_step(); }

int glue_C2F490_step(void) { return glue_C1FB82_step(); }
