/* Scene stream selection and record initialization; scene_dispatch.c and control_records.c.
 * CPU register/flag effects, bus accesses and source instruction boundaries
 * stay in this bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

int glue_C28722_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC28720: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC28722: /* jsr     $c24e2c.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC28728: /* clr.b   $c458a9.l */
        value = 0; m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(0); break;
    case 0xC2872E: /* clr.b   $c458ab.l */
        value = 0; m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(0); break;
    case 0xC28734: /* clr.w   $c45782.l */
        value = 0; step_write_word(m68ki_read_imm_32(), value); flags_logic_w(0); break;
    case 0xC2873A: /* move.b  $c458a6.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC28740: /* ext.w   D0 */
        SET_W(D(0), (int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC28742: /* cmpi.b  #$7d, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC28746: /* beq     $c288c6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2874A: /* cmpi.b  #$7e, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC2874E: /* beq     $c28756 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28750: /* cmpi.b  #$7f, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC28754: /* bne     $c2875a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC28756: /* moveq   #$a, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC28758: /* bra     $c28760 */
        step_branch(pc, opcode, 1); break;
    case 0xC2875A: /* cmpi.w  #$9, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2875E: /* bgt     $c28720 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC28760: /* lea     $c297d2.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC28766: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC28768: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2876A: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2876C: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2876E: /* add.w   D1, D0 */
        value = D(1); step_add_word(&D(0), value); break;
    case 0xC28770: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC28772: /* tst.b   $c4582b.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC28778: /* beq     $c28782 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2877A: /* move.w  $c45b1c.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28780: /* bra     $c28796 */
        step_branch(pc, opcode, 1); break;
    case 0xC28782: /* move.w  $c45af8.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28788: /* andi.w  #$3, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC2878C: /* beq     $c28796 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2878E: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC28790: /* move.w  D1, $c45b1c.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC28796: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC28798: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC2879A: /* add.w   D1, D0 */
        value = D(1); step_add_word(&D(0), value); break;
    case 0xC2879C: /* lea     (A0), A2 */
        A(2) = A(0); break;
    case 0xC2879E: /* adda.w  (A0,D0.w), A2 */
        value = m68k_read_memory_16(step_indexed(A(0))); A(2) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC287A2: /* move.w  ($2,A0,D0.w), D0 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC287A6: /* move.b  D0, $c458aa.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC287AC: /* bsr     $c287da */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC287B0: /* adda.w  (A2,D0.w), A2 */
        value = m68k_read_memory_16(step_indexed(A(2))); A(2) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC287B4: /* move.l  A2, $c4573a.l */
        value = A(2); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC287BA: /* bsr     $c28afe */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC287BE: /* adda.w  #$a, A2 */
        value = m68ki_read_imm_16(); A(2) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC287C2: /* tst.w   (A2) */
        value = m68k_read_memory_16(A(2)); flags_logic_w(value); break;
    case 0xC287C4: /* blt     $c287d8 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC287C6: /* move.w  ($8,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC287CA: /* andi.w  #$7f, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), D(0) & value); flags_logic_w(D(0)); break;
    case 0xC287CE: /* cmp.b   $c458a7.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); result = D(0); step_compare_byte(value, result); break;
    case 0xC287D4: /* ble     $c287ba */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC287D6: /* bra     $c287be */
        step_branch(pc, opcode, 1); break;
    case 0xC287D8: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC287DA: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC287DC: /* move.b  $c458a6.l, D1 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC287E2: /* cmpi.b  #$7e, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_byte(value, result); break;
    case 0xC287E6: /* beq     $c287fe */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC287E8: /* cmpi.b  #$7f, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_byte(value, result); break;
    case 0xC287EC: /* beq     $c287fe */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC287EE: /* ext.w   D1 */
        SET_W(D(1), (int16_t)(int8_t)D(1)); flags_logic_w(D(1)); break;
    case 0xC287F0: /* movea.l $c1ab74.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC287F6: /* move.b  ($12,A0,D1.w), D0 */
        value = m68k_read_memory_8(step_indexed(A(0))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC287FA: /* ext.w   D0 */
        SET_W(D(0), (int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC287FC: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC287FE: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC28800: /* move.l  A2, -(A7) */
        value = A(2); A(7) -= 4; m68k_write_memory_16(A(7) + 2, value); m68k_write_memory_16(A(7), value >> 16); flags_logic_l(value); break;
    case 0xC28802: /* move.w  ($4,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28806: /* bge     $c2883e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC28808: /* andi.w  #$7f00, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC2880C: /* beq     $c288c2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28810: /* asr.w   #7, D1 */
        renderer_asr_word(&D(1), 7); break;
    case 0xC28812: /* lea     $c295e0.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC28818: /* adda.w  (A4,D1.w), A4 */
        value = m68k_read_memory_16(step_indexed(A(4))); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2881C: /* movem.w (A4)+, D2-D6 */
        mask = m68ki_read_imm_16(); renderer_load(A(4), mask, 2, 4); break;
    case 0xC28820: /* bsr     $c28f16 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC28824: /* move.b  #$ff, ($38,A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC2882A: /* swap    D2 */
        step_swap(&D(2)); break;
    case 0xC2882C: /* swap    D3 */
        step_swap(&D(3)); break;
    case 0xC2882E: /* asl.l   #6, D2 */
        step_asl_long(&D(2), 6); break;
    case 0xC28830: /* asl.l   #6, D3 */
        step_asl_long(&D(3), 6); break;
    case 0xC28832: /* asl.l   #8, D4 */
        step_asl_long(&D(4), 8); break;
    case 0xC28834: /* asl.l   #8, D5 */
        step_asl_long(&D(5), 8); break;
    case 0xC28836: /* add.l   D4, D2 */
        value = D(4); step_add_long(&D(2), value); break;
    case 0xC28838: /* add.l   D5, D3 */
        value = D(5); step_add_long(&D(3), value); break;
    case 0xC2883A: /* move.l  D3, D4 */
        value = D(3); D(4) = value; flags_logic_l(value); break;
    case 0xC2883C: /* bra     $c28888 */
        step_branch(pc, opcode, 1); break;
    case 0xC2883E: /* lea     $c46184.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC28844: /* andi.w  #$ff00, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC28848: /* move.w  D1, D2 */
        value = D(1); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2884A: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC2884C: /* adda.w  D1, A1 */
        value = D(1); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2884E: /* move.w  ($0,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28852: /* andi.w  #$40, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC28856: /* beq     $c288c2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28858: /* move.w  ($6,A1), ($2c,A0) */
        value = m68k_read_memory_16(step_displacement(A(1))); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2885E: /* move.w  ($8,A1), ($2e,A0) */
        value = m68k_read_memory_16(step_displacement(A(1))); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28864: /* move.w  ($c,A1), ($30,A0) */
        value = m68k_read_memory_16(step_displacement(A(1))); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC2886A: /* move.w  ($e,A1), ($32,A0) */
        value = m68k_read_memory_16(step_displacement(A(1))); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28870: /* move.l  ($10,A1), ($34,A0) */
        value = m68k_read_memory_32(step_displacement(A(1))); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC28876: /* lsr.w   #8, D2 */
        SET_W(D(2), step_lsr_word_value(D(2), 8)); break;
    case 0xC28878: /* ori.b   #$80, D2 */
        value = m68ki_read_imm_16(); SET_B(D(2), D(2) | value); flags_logic_b(D(2)); break;
    case 0xC2887C: /* move.b  D2, ($38,A0) */
        value = D(2); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28880: /* move.l  ($14,A1), D2 */
        value = m68k_read_memory_32(step_displacement(A(1))); D(2) = value; flags_logic_l(value); break;
    case 0xC28884: /* move.l  ($1c,A1), D4 */
        value = m68k_read_memory_32(step_displacement(A(1))); D(4) = value; flags_logic_l(value); break;
    case 0xC28888: /* sub.l   ($14,A0), D2 */
        value = m68k_read_memory_32(step_displacement(A(0))); step_subtract_long(&D(2), value); break;
    case 0xC2888C: /* sub.l   ($1c,A0), D4 */
        value = m68k_read_memory_32(step_displacement(A(0))); step_subtract_long(&D(4), value); break;
    case 0xC28890: /* moveq   #$0, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC28892: /* moveq   #-$1, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC28894: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC28896: /* moveq   #$0, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC28898: /* move.l  A0, -(A7) */
        value = A(0); A(7) -= 4; m68k_write_memory_16(A(7) + 2, value); m68k_write_memory_16(A(7), value >> 16); flags_logic_l(value); break;
    case 0xC2889A: /* movem.l D0-D5, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2889E: /* jsr     $c123fa.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC288A4: /* adda.w  #$18, A7 */
        value = m68ki_read_imm_16(); A(7) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC288A8: /* movea.l (A7)+, A0 */
        value = m68k_read_memory_32(A(7)); A(7) += 4; A(0) = value; break;
    case 0xC288AA: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC288AC: /* move.w  $c45ac2.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC288B2: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC288B4: /* move.w  D0, D4 */
        value = D(0); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC288B6: /* move.w  D2, D5 */
        value = D(2); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC288B8: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC288BA: /* lea     (A0), A1 */
        A(1) = A(0); break;
    case 0xC288BC: /* jsr     $c2d954.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC288C2: /* movea.l (A7)+, A2 */
        value = m68k_read_memory_32(A(7)); A(7) += 4; A(2) = value; break;
    case 0xC288C4: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC288C6: /* move.w  #$4, $c459b4.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC288CE: /* lea     $c46184.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC288D4: /* lea     (A4), A5 */
        A(5) = A(4); break;
    case 0xC288D6: /* move.w  $c459b4.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC288DC: /* asl.w   #8, D0 */
        renderer_asl_word(&D(0), 8); break;
    case 0xC288DE: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC288E0: /* move.w  D0, $c459b6.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC288E6: /* adda.w  D0, A5 */
        value = D(0); A(5) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC288E8: /* moveq   #$3f, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC288EA: /* move.l  (A4)+, (A5)+ */
        value = m68k_read_memory_32(A(4)); A(4) += 4; step_write_long(A(5), value); A(5) += 4; flags_logic_l(value); break;
    case 0xC288EC: /* dbra    D0, $c288ea */
        step_dbf(pc, &D(0)); break;
    case 0xC288F0: /* lea     $c22048.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC288F6: /* move.w  #$3c, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC288FA: /* adda.w  D0, A0 */
        value = D(0); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC288FC: /* movem.l (A0), D0-D4 */
        mask = m68ki_read_imm_16(); renderer_load(A(0), mask, 4, -1); break;
    case 0xC28900: /* lea     $c22188.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC28906: /* move.w  $c459b4.l, D6 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2890C: /* add.w   D6, D6 */
        value = D(6); step_add_word(&D(6), value); break;
    case 0xC2890E: /* add.w   D6, D6 */
        value = D(6); step_add_word(&D(6), value); break;
    case 0xC28910: /* move.w  D6, D7 */
        value = D(6); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC28912: /* add.w   D6, D6 */
        value = D(6); step_add_word(&D(6), value); break;
    case 0xC28914: /* add.w   D6, D6 */
        value = D(6); step_add_word(&D(6), value); break;
    case 0xC28916: /* add.w   D7, D6 */
        value = D(7); step_add_word(&D(6), value); break;
    case 0xC28918: /* movem.l D0-D4, (A4,D6.w) */
        mask = m68ki_read_imm_16(); renderer_store(step_indexed(A(4)), mask, 4, -1); break;
    case 0xC2891E: /* moveq   #$0, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC28920: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC28922: /* move.w  #$48, D5 */
        value = m68ki_read_imm_16(); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC28926: /* jsr     $c091a8.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2892C: /* lea     $c46184.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC28932: /* adda.w  $c459b6.l, A1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC28938: /* move.b  #$10, ($62,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC2893E: /* movem.l D0-D2, ($14,A1) */
        mask = m68ki_read_imm_16(); renderer_store(step_displacement(A(1)), mask, 4, -1); break;
    case 0xC28944: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC28946: /* swap    D2 */
        step_swap(&D(2)); break;
    case 0xC28948: /* lsr.w   #6, D0 */
        SET_W(D(0), step_lsr_word_value(D(0), 6)); break;
    case 0xC2894A: /* lsr.w   #6, D2 */
        SET_W(D(2), step_lsr_word_value(D(2), 6)); break;
    case 0xC2894C: /* move.w  D0, ($6,A1) */
        value = D(0); step_write_word(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC28950: /* move.w  D2, ($8,A1) */
        value = D(2); step_write_word(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC28954: /* andi.w  #$3, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), D(0) & value); flags_logic_w(D(0)); break;
    case 0xC28958: /* subq.w  #3, D0 */
        value = 3u; step_subtract_word(&D(0), value); break;
    case 0xC2895A: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2895C: /* andi.w  #$3, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), D(2) & value); flags_logic_w(D(2)); break;
    case 0xC28960: /* subq.w  #3, D2 */
        value = 3u; step_subtract_word(&D(2), value); break;
    case 0xC28962: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC28964: /* asl.w   #2, D2 */
        renderer_asl_word(&D(2), 2); break;
    case 0xC28966: /* add.w   D0, D2 */
        value = D(0); step_add_word(&D(2), value); break;
    case 0xC28968: /* move.b  D2, ($a,A1) */
        value = D(2); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC2896C: /* move.b  #$1, ($5d,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC28972: /* move.w  #$ffff, ($2c,A1) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC28978: /* clr.b   ($5,A1) */
        value = 0; m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(0); break;
    case 0xC2897C: /* move.w  $c459b4.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC28982: /* move.b  D0, ($5e,A1) */
        value = D(0); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC28986: /* clr.b   $c4fdd0.l */
        value = 0; m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(0); break;
    case 0xC2898C: /* clr.b   $c4fdd1.l */
        value = 0; m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(0); break;
    case 0xC28992: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC28AFE: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC28B00: /* moveq   #-$2, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC28B02: /* move.l  A2, -(A7) */
        value = A(2); A(7) -= 4; m68k_write_memory_16(A(7) + 2, value); m68k_write_memory_16(A(7), value >> 16); flags_logic_l(value); break;
    case 0xC28B04: /* bsr     $c28b34 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC28B08: /* movea.l (A7)+, A2 */
        value = m68k_read_memory_32(A(7)); A(7) += 4; A(2) = value; break;
    case 0xC28B0A: /* tst.w   D7 */
        value = D(7); flags_logic_w(value); break;
    case 0xC28B0C: /* blt     $c28b12 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC28B0E: /* bsr     $c28800 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC28B12: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC28B34: /* lea     $c22048.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC28B3A: /* adda.w  (A2)+, A1 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC28B3C: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC28B3E: /* move.w  (A2)+, D0 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC28B40: /* move.w  ($4,A2), D3 */
        value = m68k_read_memory_16(step_displacement(A(2))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC28B44: /* andi.w  #$f00, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), D(3) & value); flags_logic_w(D(3)); break;
    case 0xC28B48: /* beq     $c28b56 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28B4A: /* lsr.w   #8, D3 */
        SET_W(D(3), step_lsr_word_value(D(3), 8)); break;
    case 0xC28B4C: /* cmp.b   $c458a6.l, D3 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); result = D(3); step_compare_byte(value, result); break;
    case 0xC28B52: /* bne     $c28be6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC28B56: /* move.w  (A2), D3 */
        value = m68k_read_memory_16(A(2)); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC28B58: /* andi.w  #$7f, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), D(3) & value); flags_logic_w(D(3)); break;
    case 0xC28B5C: /* move.w  D3, D1 */
        value = D(3); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28B5E: /* lea     $c46184.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC28B64: /* asl.w   #8, D3 */
        renderer_asl_word(&D(3), 8); break;
    case 0xC28B66: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC28B68: /* adda.w  D3, A4 */
        value = D(3); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC28B6A: /* btst    #$6, ($1,A4) */
        value = m68ki_read_imm_16(); address = step_displacement(A(4)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC28B70: /* bne     $c28be6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC28B72: /* move.w  D1, D3 */
        value = D(1); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC28B74: /* lea     $c22188.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC28B7A: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC28B7C: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC28B7E: /* move.w  D3, D4 */
        value = D(3); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC28B80: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC28B82: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC28B84: /* add.w   D4, D3 */
        value = D(4); step_add_word(&D(3), value); break;
    case 0xC28B86: /* adda.w  D3, A0 */
        value = D(3); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC28B88: /* movem.l (A1), D2-D6 */
        mask = m68ki_read_imm_16(); renderer_load(A(1), mask, 4, -1); break;
    case 0xC28B8C: /* movem.l D2-D6, (A0) */
        mask = m68ki_read_imm_16(); renderer_store(A(0), mask, 4, -1); break;
    case 0xC28B90: /* movea.l ($4,A0), A1 */
        value = m68k_read_memory_32(step_displacement(A(0))); A(1) = value; break;
    case 0xC28B94: /* move.w  (A1), D2 */
        value = m68k_read_memory_16(A(1)); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC28B96: /* blt     $c28ba8 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC28B98: /* andi.w  #$4000, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), D(2) & value); flags_logic_w(D(2)); break;
    case 0xC28B9C: /* beq     $c28ba4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28B9E: /* move.w  ($2,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC28BA2: /* bra     $c28ba8 */
        step_branch(pc, opcode, 1); break;
    case 0xC28BA4: /* move.w  ($4,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC28BA8: /* andi.w  #$fff, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), D(2) & value); flags_logic_w(D(2)); break;
    case 0xC28BAC: /* move.b  ($6,A1,D2.w), D4 */
        value = m68k_read_memory_8(step_indexed(A(1))); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC28BB0: /* andi.b  #$f, D4 */
        value = m68ki_read_imm_16(); SET_B(D(4), D(4) & value); flags_logic_b(D(4)); break;
    case 0xC28BB4: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC28BBA: /* move.w  D1, D3 */
        value = D(1); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC28BBC: /* asl.w   #8, D3 */
        renderer_asl_word(&D(3), 8); break;
    case 0xC28BBE: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC28BC0: /* adda.w  D3, A0 */
        value = D(3); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC28BC2: /* move.b  $c458aa.l, D3 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(3), value); flags_logic_b(value); break;
    case 0xC28BC8: /* cmp.b   $c458a9.l, D3 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); result = D(3); step_compare_byte(value, result); break;
    case 0xC28BCE: /* ble     $c28be6 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC28BD0: /* cmpi.b  #$15, ($62,A0) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(step_displacement(A(0))); step_compare_byte(value, result); break;
    case 0xC28BD6: /* bne     $c28bde */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC28BD8: /* tst.w   ($6,A0) */
        value = m68k_read_memory_16(step_displacement(A(0))); flags_logic_w(value); break;
    case 0xC28BDC: /* bne     $c28be6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC28BDE: /* btst    #$6, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC28BE4: /* beq     $c28bee */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28BE6: /* addq.w  #6, A2 */
        value = 6u; A(2) += value; break;
    case 0xC28BE8: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC28BEA: /* bra     $c28e0a */
        step_branch(pc, opcode, 1); break;
    case 0xC28BEE: /* lea     (A4), A0 */
        A(0) = A(4); break;
    case 0xC28BF0: /* moveq   #$28, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC28BF2: /* lea     (A0), A1 */
        A(1) = A(0); break;
    case 0xC28BF4: /* clr.l   (A1)+ */
        value = 0; step_write_long(A(1), value); A(1) += 4; flags_logic_l(0); break;
    case 0xC28BF6: /* dbra    D3, $c28bf4 */
        step_dbf(pc, &D(3)); break;
    case 0xC28BFA: /* move.b  ($7d,A0), D2 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(2), value); flags_logic_b(value); break;
    case 0xC28BFE: /* andi.b  #$f0, D2 */
        value = m68ki_read_imm_16(); SET_B(D(2), D(2) & value); flags_logic_b(D(2)); break;
    case 0xC28C02: /* or.b    D4, D2 */
        value = D(4); SET_B(D(2), D(2) | value); flags_logic_b(D(2)); break;
    case 0xC28C04: /* move.b  D2, ($7d,A0) */
        value = D(2); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28C08: /* move.b  D0, ($62,A0) */
        value = D(0); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28C0C: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC28C0E: /* move.b  D1, ($5e,A0) */
        value = D(1); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28C12: /* move.w  (A2)+, D1 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28C14: /* move.w  D1, D2 */
        value = D(1); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC28C16: /* andi.w  #$7f00, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), D(2) & value); flags_logic_w(D(2)); break;
    case 0xC28C1A: /* lsr.w   #8, D2 */
        SET_W(D(2), step_lsr_word_value(D(2), 8)); break;
    case 0xC28C1C: /* move.b  D2, ($3a,A0) */
        value = D(2); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28C20: /* move.w  (A2)+, D2 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC28C22: /* lea     $c295e0.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC28C28: /* adda.w  (A4,D2.w), A4 */
        value = m68k_read_memory_16(step_indexed(A(4))); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC28C2C: /* move.w  (A2)+, D2 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC28C2E: /* move.w  D2, D3 */
        value = D(2); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC28C30: /* andi.w  #$8000, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), D(2) & value); flags_logic_w(D(2)); break;
    case 0xC28C34: /* bne     $c28c52 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC28C36: /* bclr    #$3, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~ value); break;
    case 0xC28C3C: /* move.b  ($62,A0), D2 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(2), value); flags_logic_b(value); break;
    case 0xC28C40: /* andi.b  #$f0, D2 */
        value = m68ki_read_imm_16(); SET_B(D(2), D(2) & value); flags_logic_b(D(2)); break;
    case 0xC28C44: /* cmpi.b  #$10, D2 */
        value = m68ki_read_imm_16(); result = D(2); step_compare_byte(value, result); break;
    case 0xC28C48: /* bne     $c28c58 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC28C4A: /* addq.b  #1, $c458a9.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC28C50: /* bra     $c28c58 */
        step_branch(pc, opcode, 1); break;
    case 0xC28C52: /* bset    #$3, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result | value); break;
    case 0xC28C58: /* move.w  D3, D2 */
        value = D(3); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC28C5A: /* andi.w  #$4000, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), D(3) & value); flags_logic_w(D(3)); break;
    case 0xC28C5E: /* beq     $c28c66 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28C60: /* move.b  #$8, ($5,A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28C66: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC28C68: /* move.w  D2, D1 */
        value = D(2); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28C6A: /* andi.w  #$2000, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), D(2) & value); flags_logic_w(D(2)); break;
    case 0xC28C6E: /* beq     $c28c76 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28C70: /* bset    #$0, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result | value); break;
    case 0xC28C76: /* movem.w (A4)+, D2-D6 */
        mask = m68ki_read_imm_16(); renderer_load(A(4), mask, 2, 4); break;
    case 0xC28C7A: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC28C7C: /* move.w  D1, D0 */
        value = D(1); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC28C7E: /* andi.w  #$1000, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC28C82: /* beq     $c28cfc */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28C84: /* tst.b   $c4582b.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC28C8A: /* beq     $c28cae */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28C8C: /* swap    D4 */
        step_swap(&D(4)); break;
    case 0xC28C8E: /* move.w  D0, D4 */
        value = D(0); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC28C90: /* move.w  $c45b18.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC28C96: /* move.w  $c45b1a.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28C9C: /* andi.w  #$80, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), D(4) & value); flags_logic_w(D(4)); break;
    case 0xC28CA0: /* beq     $c28ca6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28CA2: /* asr.w   #1, D0 */
        renderer_asr_word(&D(0), 1); break;
    case 0xC28CA4: /* asr.w   #1, D1 */
        renderer_asr_word(&D(1), 1); break;
    case 0xC28CA6: /* add.w   D0, D2 */
        value = D(0); step_add_word(&D(2), value); break;
    case 0xC28CA8: /* add.w   D1, D3 */
        value = D(1); step_add_word(&D(3), value); break;
    case 0xC28CAA: /* swap    D4 */
        step_swap(&D(4)); break;
    case 0xC28CAC: /* bra     $c28cfc */
        step_branch(pc, opcode, 1); break;
    case 0xC28CAE: /* move.w  $c45af8.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28CB4: /* lsr.w   #1, D1 */
        SET_W(D(1), step_lsr_word_value(D(1), 1)); break;
    case 0xC28CB6: /* andi.w  #$3, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC28CBA: /* btst    #$4, $c45af9.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC28CC2: /* beq     $c28cc6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28CC4: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC28CC6: /* move.w  D1, $c45b18.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC28CCC: /* andi.w  #$80, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), D(0) & value); flags_logic_w(D(0)); break;
    case 0xC28CD0: /* beq     $c28cd4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28CD2: /* asr.w   #1, D1 */
        renderer_asr_word(&D(1), 1); break;
    case 0xC28CD4: /* add.w   D1, D2 */
        value = D(1); step_add_word(&D(2), value); break;
    case 0xC28CD6: /* move.w  $c45af8.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28CDC: /* lsr.w   #2, D1 */
        SET_W(D(1), step_lsr_word_value(D(1), 2)); break;
    case 0xC28CDE: /* andi.w  #$3, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC28CE2: /* btst    #$5, $c45af9.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC28CEA: /* beq     $c28cee */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28CEC: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC28CEE: /* move.w  D1, $c45b1a.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC28CF4: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC28CF6: /* beq     $c28cfa */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28CF8: /* asr.w   #1, D1 */
        renderer_asr_word(&D(1), 1); break;
    case 0xC28CFA: /* add.w   D1, D3 */
        value = D(1); step_add_word(&D(3), value); break;
    case 0xC28CFC: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC28CFE: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC28D00: /* addq.b  #1, D7 */
        value = 1u; renderer_add_byte(&D(7), value); break;
    case 0xC28D02: /* move.b  D7, ($5d,A0) */
        value = D(7); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28D06: /* move.b  #$3, ($7a,A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28D0C: /* move.b  ($62,A0), D7 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(7), value); flags_logic_b(value); break;
    case 0xC28D10: /* andi.b  #$f0, D7 */
        value = m68ki_read_imm_16(); SET_B(D(7), D(7) & value); flags_logic_b(D(7)); break;
    case 0xC28D14: /* cmpi.b  #$20, D7 */
        value = m68ki_read_imm_16(); result = D(7); step_compare_byte(value, result); break;
    case 0xC28D18: /* beq     $c28d20 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28D1A: /* ori.w   #$1080, ($0,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_16(address); SET_W(result, result | value); flags_logic_w(result); step_write_word(address, result); break;
    case 0xC28D20: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC28D22: /* bge     $c28d54 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC28D24: /* andi.w  #$7f00, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC28D28: /* beq     $c28d4e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28D2A: /* asr.w   #7, D1 */
        renderer_asr_word(&D(1), 7); break;
    case 0xC28D2C: /* lea     $c295e0.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC28D32: /* adda.w  (A4,D1.w), A4 */
        value = m68k_read_memory_16(step_indexed(A(4))); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC28D36: /* move.w  (A4)+, ($2c,A0) */
        value = m68k_read_memory_16(A(4)); A(4) += 2; step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D3A: /* move.w  (A4)+, ($2e,A0) */
        value = m68k_read_memory_16(A(4)); A(4) += 2; step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D3E: /* move.w  (A4)+, ($30,A0) */
        value = m68k_read_memory_16(A(4)); A(4) += 2; step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D42: /* move.w  (A4)+, ($32,A0) */
        value = m68k_read_memory_16(A(4)); A(4) += 2; step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D46: /* move.w  (A4), D1 */
        value = m68k_read_memory_16(A(4)); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC28D48: /* ext.l   D1 */
        D(1) = (uint32_t)(int32_t)(int16_t)D(1); flags_logic_l(D(1)); break;
    case 0xC28D4A: /* move.l  D1, ($34,A0) */
        value = D(1); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC28D4E: /* move.b  #$ff, D1 */
        value = m68ki_read_imm_16(); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC28D52: /* bra     $c28d5e */
        step_branch(pc, opcode, 1); break;
    case 0xC28D54: /* bsr     $c28f16 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC28D58: /* lsr.w   #8, D1 */
        SET_W(D(1), step_lsr_word_value(D(1), 8)); break;
    case 0xC28D5A: /* ori.b   #$80, D1 */
        value = m68ki_read_imm_16(); SET_B(D(1), D(1) | value); flags_logic_b(D(1)); break;
    case 0xC28D5E: /* move.b  D1, ($38,A0) */
        value = D(1); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28D62: /* move.w  D2, ($6,A0) */
        value = D(2); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D66: /* move.w  D3, ($8,A0) */
        value = D(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D6A: /* move.w  D4, ($c,A0) */
        value = D(4); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D6E: /* move.w  D5, ($e,A0) */
        value = D(5); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D72: /* move.l  D6, ($10,A0) */
        value = D(6); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC28D76: /* ext.l   D4 */
        D(4) = (uint32_t)(int32_t)(int16_t)D(4); flags_logic_l(D(4)); break;
    case 0xC28D78: /* ext.l   D5 */
        D(5) = (uint32_t)(int32_t)(int16_t)D(5); flags_logic_l(D(5)); break;
    case 0xC28D7A: /* asl.l   #8, D4 */
        step_asl_long(&D(4), 8); break;
    case 0xC28D7C: /* asl.l   #8, D6 */
        step_asl_long(&D(6), 8); break;
    case 0xC28D7E: /* asl.l   #8, D5 */
        step_asl_long(&D(5), 8); break;
    case 0xC28D80: /* tst.l   D6 */
        value = D(6); flags_logic_l(value); break;
    case 0xC28D82: /* beq     $c28d9c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC28D84: /* bset    #$7, ($7c,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result | value); break;
    case 0xC28D8A: /* ori.b   #$60, ($7c,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); SET_B(result, result | value); flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC28D90: /* move.w  #$2000, ($6c,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D96: /* move.w  #$2000, ($6e,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28D9C: /* ori.w   #$140, ($0,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_16(address); SET_W(result, result | value); flags_logic_w(result); step_write_word(address, result); break;
    case 0xC28DA2: /* andi.w  #$7fff, ($0,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_16(address); SET_W(result, result & value); flags_logic_w(result); step_write_word(address, result); break;
    case 0xC28DA8: /* move.b  #$44, ($5f,A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28DAE: /* move.w  #$1f4, ($60,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28DB4: /* move.l  #$61a800, ($72,A0) */
        value = m68ki_read_imm_32(); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC28DBC: /* move.b  #$3d, ($63,A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28DC2: /* move.b  #$ff, ($71,A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28DC8: /* move.b  #$6, ($64,A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28DCE: /* move.b  #$10, ($64,A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(0)), value); flags_logic_b(value); break;
    case 0xC28DD4: /* ext.l   D2 */
        D(2) = (uint32_t)(int32_t)(int16_t)D(2); flags_logic_l(D(2)); break;
    case 0xC28DD6: /* ext.l   D3 */
        D(3) = (uint32_t)(int32_t)(int16_t)D(3); flags_logic_l(D(3)); break;
    case 0xC28DD8: /* swap    D2 */
        step_swap(&D(2)); break;
    case 0xC28DDA: /* swap    D3 */
        step_swap(&D(3)); break;
    case 0xC28DDC: /* asl.l   #6, D2 */
        step_asl_long(&D(2), 6); break;
    case 0xC28DDE: /* asl.l   #6, D3 */
        step_asl_long(&D(3), 6); break;
    case 0xC28DE0: /* add.l   D4, D2 */
        value = D(4); step_add_long(&D(2), value); break;
    case 0xC28DE2: /* add.l   D5, D3 */
        value = D(5); step_add_long(&D(3), value); break;
    case 0xC28DE4: /* move.l  D2, ($14,A0) */
        value = D(2); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC28DE8: /* move.l  D6, ($18,A0) */
        value = D(6); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC28DEC: /* move.l  D3, ($1c,A0) */
        value = D(3); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC28DF0: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC28DF2: /* moveq   #$0, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC28DF4: /* moveq   #$0, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC28DF6: /* lea     (A0), A1 */
        A(1) = A(0); break;
    case 0xC28DF8: /* movem.l D0/A0/A2, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC28DFC: /* jsr     $c2d954.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC28E02: /* movem.l (A7)+, D0/A0/A2 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC28E06: /* moveq   #$0, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC28E08: /* bra     $c28e0c */
        step_branch(pc, opcode, 1); break;
    case 0xC28E0A: /* moveq   #-$1, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC28E0C: /* dbra    D0, $c28b34 */
        step_dbf(pc, &D(0)); break;
    case 0xC28E10: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC28F16: /* move.w  D2, ($2c,A0) */
        value = D(2); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28F1A: /* move.w  D3, ($2e,A0) */
        value = D(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28F1E: /* move.w  D4, ($30,A0) */
        value = D(4); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28F22: /* move.w  D5, ($32,A0) */
        value = D(5); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC28F26: /* move.l  D6, ($34,A0) */
        value = D(6); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC28F2A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C287DA_step(void) { return glue_C28722_step(); }

int glue_C28800_step(void) { return glue_C28722_step(); }

int glue_C28AFE_step(void) { return glue_C28722_step(); }

int glue_C28B34_step(void) { return glue_C28722_step(); }

int glue_C28F16_step(void) { return glue_C28722_step(); }
