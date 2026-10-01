/* Source timing for the screen-frame preparation family.
 * Readable domain behavior remains in screen_frame.c and clip.c.
 * Shared entries retain source arithmetic, children and bus boundaries. */
#include "glue_renderer_step_math.h"

int glue_C0D74A_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode, mask;
    if (pc < 0xC0D74Au || pc >= 0xC0DAEEu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC0D74A: /* lea     $c45bea.l, A2 */
        A(2) = m68ki_read_imm_32(); break;
    case 0xC0D750: /* bra     $c0d758 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D752: /* lea     $c45bd8.l, A2 */
        A(2) = m68ki_read_imm_32(); break;
    case 0xC0D758: /* lea     (A2), A4 */
        A(4) = A(2); break;
    case 0xC0D75A: /* link    A6, #-$2 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0D75E: /* lea     (-$40,PC), A1; ($c0d720) */
        A(1) = pc + 2 + (int16_t)m68ki_read_imm_16(); break;
    case 0xC0D762: /* lea     $c4b390.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC0D768: /* lea     (-$2,A6), A0 */
        A(0) = step_displacement(A(6)); break;
    case 0xC0D76C: /* move.w  #$4, (A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(0), value); flags_logic_w(value); break;
    case 0xC0D770: /* move.w  (A1)+, D2 */
        value = m68k_read_memory_16(A(1)); A(1) += 2; SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC0D772: /* move.l  $c45a66.l, D3 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(3) = value; flags_logic_l(value); break;
    case 0xC0D778: /* swap    D3 */
        step_swap(&D(3)); break;
    case 0xC0D77A: /* asr.w   #2, D3 */
        renderer_asr_word(&D(3), 2); break;
    case 0xC0D77C: /* move.w  (A1)+, D4 */
        value = m68k_read_memory_16(A(1)); A(1) += 2; SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC0D77E: /* lea     (A4), A2 */
        A(2) = A(4); break;
    case 0xC0D780: /* move.w  D2, D5 */
        value = D(2); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC0D782: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D784: /* move.w  D4, D7 */
        value = D(4); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC0D786: /* muls.w  (A2)+, D5 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; renderer_multiply(&D(5), value); break;
    case 0xC0D788: /* muls.w  (A2)+, D6 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; renderer_multiply(&D(6), value); break;
    case 0xC0D78A: /* muls.w  (A2)+, D7 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; renderer_multiply(&D(7), value); break;
    case 0xC0D78C: /* add.l   D6, D7 */
        value = D(6); step_add_long(&D(7), value); break;
    case 0xC0D78E: /* add.l   D5, D7 */
        value = D(5); step_add_long(&D(7), value); break;
    case 0xC0D790: /* asr.l   #8, D7 */
        step_asr_long(&D(7), 8); break;
    case 0xC0D792: /* bcc     $c0d796 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC0D794: /* addq.w  #1, D7 */
        value = 1u; step_add_word(&D(7), value); break;
    case 0xC0D796: /* move.w  D7, (A3)+ */
        value = D(7); m68k_write_memory_16(A(3), value); A(3) += 2; flags_logic_w(value); break;
    case 0xC0D798: /* move.w  D2, D5 */
        value = D(2); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC0D79A: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D79C: /* move.w  D4, D7 */
        value = D(4); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC0D79E: /* muls.w  (A2)+, D5 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; renderer_multiply(&D(5), value); break;
    case 0xC0D7A0: /* muls.w  (A2)+, D6 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; renderer_multiply(&D(6), value); break;
    case 0xC0D7A2: /* muls.w  (A2)+, D7 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; renderer_multiply(&D(7), value); break;
    case 0xC0D7A4: /* add.l   D6, D7 */
        value = D(6); step_add_long(&D(7), value); break;
    case 0xC0D7A6: /* add.l   D5, D7 */
        value = D(5); step_add_long(&D(7), value); break;
    case 0xC0D7A8: /* asr.l   #8, D7 */
        step_asr_long(&D(7), 8); break;
    case 0xC0D7AA: /* bcc     $c0d7ae */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC0D7AC: /* addq.w  #1, D7 */
        value = 1u; step_add_word(&D(7), value); break;
    case 0xC0D7AE: /* move.w  D7, (A3)+ */
        value = D(7); m68k_write_memory_16(A(3), value); A(3) += 2; flags_logic_w(value); break;
    case 0xC0D7B0: /* muls.w  (A2)+, D2 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; renderer_multiply(&D(2), value); break;
    case 0xC0D7B2: /* muls.w  (A2)+, D3 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; renderer_multiply(&D(3), value); break;
    case 0xC0D7B4: /* muls.w  (A2)+, D4 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; renderer_multiply(&D(4), value); break;
    case 0xC0D7B6: /* add.l   D3, D4 */
        value = D(3); step_add_long(&D(4), value); break;
    case 0xC0D7B8: /* add.l   D2, D4 */
        value = D(2); step_add_long(&D(4), value); break;
    case 0xC0D7BA: /* asr.l   #8, D4 */
        step_asr_long(&D(4), 8); break;
    case 0xC0D7BC: /* bcc     $c0d7c0 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC0D7BE: /* addq.w  #1, D4 */
        value = 1u; step_add_word(&D(4), value); break;
    case 0xC0D7C0: /* move.w  D4, (A3)+ */
        value = D(4); m68k_write_memory_16(A(3), value); A(3) += 2; flags_logic_w(value); break;
    case 0xC0D7C2: /* move.w  D4, D3 */
        value = D(4); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC0D7C4: /* adda.w  #$1a, A3 */
        value = m68ki_read_imm_16(); A(3) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC0D7C8: /* subq.w  #1, (A0) */
        value = 1u; address = A(0); result = m68k_read_memory_16(address); step_subtract_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC0D7CA: /* bgt     $c0d770 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC0D7CC: /* lea     $c4e854.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC0D7D2: /* clr.l   (A4)+ */
        value = 0; m68k_write_memory_32(A(4), value); A(4) += 4; flags_logic_l(0); break;
    case 0xC0D7D4: /* clr.l   (A4)+ */
        value = 0; m68k_write_memory_32(A(4), value); A(4) += 4; flags_logic_l(0); break;
    case 0xC0D7D6: /* clr.l   (A4)+ */
        value = 0; m68k_write_memory_32(A(4), value); A(4) += 4; flags_logic_l(0); break;
    case 0xC0D7D8: /* clr.l   (A4) */
        value = 0; m68k_write_memory_32(A(4), value); flags_logic_l(0); break;
    case 0xC0D7DA: /* jsr     $c2e758.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC0D7E0: /* lea     $c4e854.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D7E6: /* tst.w   ($2,A1) */
        value = m68k_read_memory_16(step_displacement(A(1))); flags_logic_w(value); break;
    case 0xC0D7EA: /* beq     $c0d826 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0D7EC: /* tst.w   ($6,A1) */
        value = m68k_read_memory_16(step_displacement(A(1))); flags_logic_w(value); break;
    case 0xC0D7F0: /* beq     $c0d7fe */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0D7F2: /* move.w  ($a,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0D7F6: /* move.w  ($e,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC0D7FA: /* bra     $c0d872 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D7FE: /* tst.w   ($0,A1) */
        value = m68k_read_memory_16(step_displacement(A(1))); flags_logic_w(value); break;
    case 0xC0D802: /* beq     $c0d810 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0D804: /* move.w  ($a,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0D808: /* move.w  ($8,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC0D80C: /* bra     $c0d8be */
        step_branch(pc, opcode, 1); break;
    case 0xC0D810: /* tst.w   ($4,A1) */
        value = m68k_read_memory_16(step_displacement(A(1))); flags_logic_w(value); break;
    case 0xC0D814: /* beq     $c0d822 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0D816: /* move.w  ($a,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0D81A: /* move.w  ($c,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC0D81E: /* bra     $c0d90c */
        step_branch(pc, opcode, 1); break;
    case 0xC0D822: /* bra     $c0da94 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D826: /* tst.w   ($0,A1) */
        value = m68k_read_memory_16(step_displacement(A(1))); flags_logic_w(value); break;
    case 0xC0D82A: /* beq     $c0d854 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0D82C: /* tst.w   ($6,A1) */
        value = m68k_read_memory_16(step_displacement(A(1))); flags_logic_w(value); break;
    case 0xC0D830: /* beq     $c0d83e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0D832: /* move.w  ($8,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0D836: /* move.w  ($e,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC0D83A: /* bra     $c0d95a */
        step_branch(pc, opcode, 1); break;
    case 0xC0D83E: /* tst.w   ($4,A1) */
        value = m68k_read_memory_16(step_displacement(A(1))); flags_logic_w(value); break;
    case 0xC0D842: /* beq     $c0d850 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0D844: /* move.w  ($8,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0D848: /* move.w  ($c,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC0D84C: /* bra     $c0d9a8 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D850: /* bra     $c0da94 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D854: /* tst.w   ($4,A1) */
        value = m68k_read_memory_16(step_displacement(A(1))); flags_logic_w(value); break;
    case 0xC0D858: /* beq     $c0da38 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0D85C: /* tst.w   ($6,A1) */
        value = m68k_read_memory_16(step_displacement(A(1))); flags_logic_w(value); break;
    case 0xC0D860: /* beq     $c0d86e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC0D862: /* move.w  ($c,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0D866: /* move.w  ($e,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC0D86A: /* bra     $c0d9ea */
        step_branch(pc, opcode, 1); break;
    case 0xC0D86E: /* bra     $c0da94 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D872: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D878: /* move.w  #$4, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0D87C: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC0D882: /* bne     $c0d890 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC0D884: /* move.w  $c458ca.l, D6 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D88A: /* andi.w  #$2, D6 */
        value = m68ki_read_imm_16(); value &= D(6); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D88E: /* bne     $c0d8ae */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC0D890: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D894: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D898: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D89C: /* lea     $c4b39a.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D8A2: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8A6: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8AA: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D8AE: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8B2: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8B6: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8BA: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D8BE: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D8C4: /* move.w  #$5, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0D8C8: /* move.w  $c458ca.l, D6 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D8CE: /* andi.w  #$2, D6 */
        value = m68ki_read_imm_16(); value &= D(6); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D8D2: /* bne     $c0d8f8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC0D8D4: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8D8: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8DC: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8E0: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8E4: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D8EA: /* move.w  #$3, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0D8EE: /* addq.w  #8, A1 */
        value = 8u; A(1) += value; break;
    case 0xC0D8F0: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8F4: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D8F8: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D8FC: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D900: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D904: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D908: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D90C: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D912: /* move.w  #$5, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0D916: /* move.w  $c458ca.l, D6 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D91C: /* andi.w  #$2, D6 */
        value = m68ki_read_imm_16(); value &= D(6); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D920: /* bne     $c0d936 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC0D922: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D926: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D92A: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D92E: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D932: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D936: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D93A: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D93E: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D942: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D946: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D94C: /* move.w  #$3, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0D950: /* addq.w  #8, A1 */
        value = 8u; A(1) += value; break;
    case 0xC0D952: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D956: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D95A: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D960: /* move.w  #$5, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0D964: /* move.w  $c458ca.l, D6 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D96A: /* andi.w  #$2, D6 */
        value = m68ki_read_imm_16(); value &= D(6); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D96E: /* bne     $c0d994 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC0D970: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D974: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D978: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D97C: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D980: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D986: /* move.w  #$3, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0D98A: /* addq.w  #8, A1 */
        value = 8u; A(1) += value; break;
    case 0xC0D98C: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D990: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D994: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D998: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D99C: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9A0: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9A4: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D9A8: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D9AE: /* move.w  #$4, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0D9B2: /* cmpi.w  #$3840, $c45a92.l */
        value = m68ki_read_imm_16(); result = m68k_read_memory_16(m68ki_read_imm_32()); step_compare_word(value, result); break;
    case 0xC0D9BA: /* bge     $c0d9da */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC0D9BC: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9C0: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9C4: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9C8: /* lea     $c4b39a.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D9CE: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9D2: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9D6: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D9DA: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9DE: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9E2: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0D9E6: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0D9EA: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0D9F0: /* move.w  #$5, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0D9F4: /* move.w  $c458ca.l, D6 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D9FA: /* andi.w  #$2, D6 */
        value = m68ki_read_imm_16(); value &= D(6); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC0D9FE: /* bne     $c0da14 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC0DA00: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA04: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA08: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA0C: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA10: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0DA14: /* bsr     $c0daa0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA18: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA1C: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA20: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA24: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0DA2A: /* move.w  #$3, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DA2E: /* addq.w  #8, A1 */
        value = 8u; A(1) += value; break;
    case 0xC0DA30: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA34: /* bra     $c0da70 */
        step_branch(pc, opcode, 1); break;
    case 0xC0DA38: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC0DA3E: /* move.w  #$4, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DA42: /* bsr     $c0dad0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA46: /* bsr     $c0dad4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA4A: /* bsr     $c0dadc */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA4E: /* bsr     $c0dae6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC0DA52: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC0DA58: /* bne     $c0da62 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC0DA5A: /* move.w  $c45a8a.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0DA60: /* bra     $c0da68 */
        step_branch(pc, opcode, 1); break;
    case 0xC0DA62: /* move.w  $c45a94.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC0DA68: /* cmpi.w  #$3840, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC0DA6C: /* bgt     $c0da70 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC0DA6E: /* bra     $c0da94 */
        step_branch(pc, opcode, 1); break;
    case 0xC0DA70: /* move.w  #$1, $c456e6.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC0DA78: /* move.w  #$1, $c456e8.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC0DA80: /* clr.l   $c456ea.l */
        value = 0; m68k_write_memory_32(m68ki_read_imm_32(), value); flags_logic_l(0); break;
    case 0xC0DA86: /* move.b  #$1, $c4589e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC0DA8E: /* moveq   #$0, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC0DA90: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC0DA92: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC0DA94: /* clr.b   $c4589e.l */
        value = 0; m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(0); break;
    case 0xC0DA9A: /* moveq   #$1, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC0DA9C: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC0DA9E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC0DAA0: /* asl.w   #3, D0 */
        renderer_asl_word(&D(0), 3); break;
    case 0xC0DAA2: /* asl.w   #3, D1 */
        renderer_asl_word(&D(1), 3); break;
    case 0xC0DAA4: /* lea     $c4b990.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC0DAAA: /* move.w  #$13f, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC0DAAE: /* move.w  D2, D4 */
        value = D(2); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC0DAB0: /* sub.w   (A0,D0.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(0))); step_subtract_word(&D(2), value); break;
    case 0xC0DAB4: /* move.w  #$b3, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC0DAB8: /* move.w  D3, D5 */
        value = D(3); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC0DABA: /* sub.w   ($2,A0,D0.w), D3 */
        value = m68k_read_memory_16(step_indexed(A(0))); step_subtract_word(&D(3), value); break;
    case 0xC0DABE: /* move.w  D2, (A1)+ */
        value = D(2); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DAC0: /* move.w  D3, (A1)+ */
        value = D(3); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DAC2: /* sub.w   (A0,D1.w), D4 */
        value = m68k_read_memory_16(step_indexed(A(0))); step_subtract_word(&D(4), value); break;
    case 0xC0DAC6: /* sub.w   ($2,A0,D1.w), D5 */
        value = m68k_read_memory_16(step_indexed(A(0))); step_subtract_word(&D(5), value); break;
    case 0xC0DACA: /* move.w  D4, (A1)+ */
        value = D(4); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DACC: /* move.w  D5, (A1)+ */
        value = D(5); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DACE: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC0DAD0: /* clr.l   (A1)+ */
        value = 0; m68k_write_memory_32(A(1), value); A(1) += 4; flags_logic_l(0); break;
    case 0xC0DAD2: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC0DAD4: /* move.w  #$13f, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DAD8: /* clr.w   (A1)+ */
        value = 0; m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(0); break;
    case 0xC0DADA: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC0DADC: /* move.w  #$13f, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DAE0: /* move.w  #$b3, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DAE4: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC0DAE6: /* clr.w   (A1)+ */
        value = 0; m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(0); break;
    case 0xC0DAE8: /* move.w  #$b3, (A1)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC0DAEC: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C0D752_step(void) { return glue_C0D74A_step(); }

int glue_C0DAA0_step(void) { return glue_C0D74A_step(); }

int glue_C0DAD0_step(void) { return glue_C0D74A_step(); }

int glue_C0DAD4_step(void) { return glue_C0D74A_step(); }

int glue_C0DADC_step(void) { return glue_C0D74A_step(); }

int glue_C0DAE6_step(void) { return glue_C0D74A_step(); }
