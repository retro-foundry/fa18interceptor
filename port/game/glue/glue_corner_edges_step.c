/* Source timing for the corner projection and four plane-clipping helpers.
 * Readable domain behavior remains in clip.c.
 * Shared entries retain source arithmetic, children and bus boundaries. */
#include "glue_renderer_step_math.h"

int glue_C2E758_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode, mask;
    if (pc < 0xC2E758u || pc >= 0xC2EC68u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC2E758: /* link    A6, #-$4 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC2E75C: /* clr.b   (-$2,A6) */
        value = 0; m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(0); break;
    case 0xC2E760: /* lea     $c4b990.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC2E766: /* lea     $c4b390.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2E76C: /* clr.w   D0 */
        value = 0; SET_W(D(0), value); flags_logic_w(0); break;
    case 0xC2E76E: /* move.w  #$7, ($8,A6) */
        value = m68ki_read_imm_16(); m68k_write_memory_16(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC2E774: /* btst    #$0, D0 */
        value = m68ki_read_imm_16(); FLAG_Z = D(0) & (1u << (value & 31u)); break;
    case 0xC2E778: /* beq     $c2e78c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E77A: /* move.w  D0, D3 */
        value = D(0); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E77C: /* addq.w  #1, D3 */
        value = 1u; step_add_word(&D(3), value); break;
    case 0xC2E77E: /* cmp.w   ($8,A6), D3 */
        value = m68k_read_memory_16(step_displacement(A(6))); result = D(3); step_compare_word(value, result); break;
    case 0xC2E782: /* ble     $c2e786 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E784: /* clr.w   D3 */
        value = 0; SET_W(D(3), value); flags_logic_w(0); break;
    case 0xC2E786: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E788: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC2E78A: /* bra     $c2e79a */
        step_branch(pc, opcode, 1); break;
    case 0xC2E78C: /* move.w  D0, D3 */
        value = D(0); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E78E: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E790: /* addq.w  #2, D1 */
        value = 2u; step_add_word(&D(1), value); break;
    case 0xC2E792: /* cmp.w   ($8,A6), D1 */
        value = m68k_read_memory_16(step_displacement(A(6))); result = D(1); step_compare_word(value, result); break;
    case 0xC2E796: /* ble     $c2e79a */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E798: /* clr.w   D1 */
        value = 0; SET_W(D(1), value); flags_logic_w(0); break;
    case 0xC2E79A: /* asl.w   #4, D1 */
        renderer_asl_word(&D(1), 4); break;
    case 0xC2E79C: /* move.w  D0, D6 */
        value = D(0); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E79E: /* asl.w   #3, D6 */
        renderer_asl_word(&D(6), 3); break;
    case 0xC2E7A0: /* lea     (A0,D6.w), A3 */
        A(3) = step_indexed(A(0)); break;
    case 0xC2E7A4: /* tst.b   (-$2,A6) */
        value = m68k_read_memory_8(step_displacement(A(6))); flags_logic_b(value); break;
    case 0xC2E7A8: /* beq     $c2e7b2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E7AA: /* clr.b   (-$2,A6) */
        value = 0; m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(0); break;
    case 0xC2E7AE: /* bra     $c2ea38 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E7B2: /* asl.w   #4, D3 */
        renderer_asl_word(&D(3), 4); break;
    case 0xC2E7B4: /* movem.w (A1,D3.w), D3-D5 */
        mask = m68ki_read_imm_16(); renderer_load(step_indexed(A(1)), mask, 2, -1); break;
    case 0xC2E7BA: /* addq.w  #1, D3 */
        value = 1u; step_add_word(&D(3), value); break;
    case 0xC2E7BC: /* addq.w  #1, D4 */
        value = 1u; step_add_word(&D(4), value); break;
    case 0xC2E7BE: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC2E7C0: /* blt     $c2e7fc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E7C2: /* move.w  (A1,D1.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E7C6: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E7CA: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E7CC: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E7D0: /* bsr     $c2ea5a */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E7D4: /* beq     $c2e986 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E7D8: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2E7DE: /* bge     $c2e834 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E7E0: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E7E2: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E7E4: /* ble     $c2e834 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E7E6: /* bsr     $c2ead0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E7EA: /* bne     $c2e834 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2E7EC: /* addq.w  #1, $c4e85a.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); step_add_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC2E7F2: /* move.w  D0, $c4e862.l */
        value = D(0); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2E7F8: /* bra     $c2e9f8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E7FC: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E7FE: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E800: /* cmp.w   D5, D6 */
        value = D(5); result = D(6); step_compare_word(value, result); break;
    case 0xC2E802: /* blt     $c2e8da */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E806: /* move.w  (A1,D1.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E80A: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E80E: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E810: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E812: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E816: /* bsr     $c2ead0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E81A: /* beq     $c2e9ca */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E81E: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2E824: /* bge     $c2e834 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E826: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E828: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E82A: /* ble     $c2e834 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E82C: /* bsr     $c2ea5a */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E830: /* beq     $c2e986 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E834: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2E836: /* bge     $c2e888 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E838: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E83A: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E83C: /* cmp.w   D5, D6 */
        value = D(5); result = D(6); step_compare_word(value, result); break;
    case 0xC2E83E: /* blt     $c2e9dc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E842: /* move.w  ($2,A1,D1.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E846: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E84A: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E84C: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E84E: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E852: /* bsr     $c2ebc2 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E856: /* beq     $c2e8ca */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E858: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2E85E: /* bge     $c2e9dc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E862: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC2E864: /* blt     $c2e9dc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E868: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E86A: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E86C: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E870: /* bsr     $c2eb4c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E874: /* bne     $c2e9dc */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2E878: /* addq.w  #1, $c4e854.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); step_add_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC2E87E: /* move.w  D0, $c4e85c.l */
        value = D(0); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2E884: /* bra     $c2e9f8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E888: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC2E88A: /* blt     $c2e9dc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E88E: /* move.w  ($2,A1,D1.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E892: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E896: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E898: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E89C: /* bsr     $c2eb4c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E8A0: /* beq     $c2e878 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E8A2: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2E8A8: /* bge     $c2e9dc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E8AC: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E8AE: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E8B0: /* cmp.w   D5, D6 */
        value = D(5); result = D(6); step_compare_word(value, result); break;
    case 0xC2E8B2: /* blt     $c2e9dc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E8B6: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E8BA: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E8BC: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E8BE: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E8C2: /* bsr     $c2ebc2 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E8C6: /* bne     $c2e9dc */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2E8CA: /* addq.w  #1, $c4e858.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); step_add_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC2E8D0: /* move.w  D0, $c4e860.l */
        value = D(0); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2E8D6: /* bra     $c2e9f8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E8DA: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC2E8DC: /* blt     $c2e916 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E8DE: /* move.w  ($2,A1,D1.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E8E2: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E8E6: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E8E8: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E8EC: /* bsr     $c2eb4c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E8F0: /* beq     $c2e878 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E8F2: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2E8F8: /* bge     $c2e94c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E8FA: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E8FC: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E8FE: /* ble     $c2e94c */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E900: /* bsr     $c2ebc2 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E904: /* bne     $c2e94c */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2E906: /* addq.w  #1, $c4e858.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); step_add_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC2E90C: /* move.w  D0, $c4e860.l */
        value = D(0); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2E912: /* bra     $c2e9f8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E916: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E918: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E91A: /* cmp.w   D5, D6 */
        value = D(5); result = D(6); step_compare_word(value, result); break;
    case 0xC2E91C: /* blt     $c2e9d8 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E920: /* move.w  ($2,A1,D1.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E924: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E928: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E92A: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E92C: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E930: /* bsr     $c2ebc2 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E934: /* beq     $c2e8ca */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E936: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2E93C: /* bge     $c2e94c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E93E: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E940: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E942: /* ble     $c2e94c */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E944: /* bsr     $c2eb4c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E948: /* beq     $c2e878 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E94C: /* tst.w   D3 */
        value = D(3); flags_logic_w(value); break;
    case 0xC2E94E: /* bge     $c2e994 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E950: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E952: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E954: /* cmp.w   D5, D6 */
        value = D(5); result = D(6); step_compare_word(value, result); break;
    case 0xC2E956: /* blt     $c2e9dc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E95A: /* move.w  (A1,D1.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E95E: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E962: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E964: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E966: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E968: /* bsr     $c2ead0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E96C: /* beq     $c2e9ca */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E96E: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2E974: /* bge     $c2e9dc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E976: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC2E978: /* blt     $c2e9dc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E97A: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E97C: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E97E: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E980: /* bsr     $c2ea5a */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E984: /* bne     $c2e9dc */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2E986: /* addq.w  #1, $c4e856.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); step_add_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC2E98C: /* move.w  D0, $c4e85e.l */
        value = D(0); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2E992: /* bra     $c2e9f8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E994: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC2E996: /* blt     $c2e9dc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E998: /* move.w  (A1,D1.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E99C: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E9A0: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E9A2: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E9A4: /* bsr     $c2ea5a */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E9A8: /* beq     $c2e986 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E9AA: /* tst.w   $c45aca.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2E9B0: /* bge     $c2e9dc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E9B2: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E9B4: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E9B6: /* cmp.w   D5, D6 */
        value = D(5); result = D(6); step_compare_word(value, result); break;
    case 0xC2E9B8: /* blt     $c2e9dc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E9BA: /* move.w  ($4,A1,D1.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E9BE: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E9C0: /* cmp.w   D2, D6 */
        value = D(2); result = D(6); step_compare_word(value, result); break;
    case 0xC2E9C2: /* ble     $c2e9dc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E9C4: /* bsr     $c2ead0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E9C8: /* bne     $c2e9dc */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2E9CA: /* addq.w  #1, $c4e85a.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); step_add_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC2E9D0: /* move.w  D0, $c4e862.l */
        value = D(0); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2E9D6: /* bra     $c2e9f8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E9D8: /* tst.w   D5 */
        value = D(5); flags_logic_w(value); break;
    case 0xC2E9DA: /* bge     $c2e9ea */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E9DC: /* clr.l   (A3) */
        value = 0; m68k_write_memory_32(A(3), value); flags_logic_l(0); break;
    case 0xC2E9DE: /* move.w  D0, D6 */
        value = D(0); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E9E0: /* asl.w   #4, D6 */
        renderer_asl_word(&D(6), 4); break;
    case 0xC2E9E2: /* clr.w   ($e,A1,D6.w) */
        value = 0; m68k_write_memory_16(step_indexed(A(1)), value); flags_logic_w(0); break;
    case 0xC2E9E6: /* bra     $c2ea38 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E9EA: /* btst    #$0, D0 */
        value = m68ki_read_imm_16(); FLAG_Z = D(0) & (1u << (value & 31u)); break;
    case 0xC2E9EE: /* beq     $c2ea38 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2E9F0: /* move.b  #$1, (-$2,A6) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(value); break;
    case 0xC2E9F6: /* bra     $c2ea38 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E9F8: /* movem.w $c45ac6.l, D3-D5 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC2EA00: /* tst.w   D5 */
        value = D(5); flags_logic_w(value); break;
    case 0xC2EA02: /* ble     $c2ea02 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EA04: /* muls.w  #$140, D3 */
        value = m68ki_read_imm_16(); renderer_multiply(&D(3), value); break;
    case 0xC2EA08: /* divs.w  D5, D3 */
        value = D(5); renderer_divide(&D(3), (int16_t)value); break;
    case 0xC2EA0A: /* asr.w   #1, D3 */
        renderer_asr_word(&D(3), 1); break;
    case 0xC2EA0C: /* bcc     $c2ea10 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2EA0E: /* addq.w  #1, D3 */
        value = 1u; step_add_word(&D(3), value); break;
    case 0xC2EA10: /* addi.w  #$a0, D3 */
        value = m68ki_read_imm_16(); step_add_word(&D(3), value); break;
    case 0xC2EA14: /* blt     $c2ea46 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EA16: /* cmpi.w  #$140, D3 */
        value = m68ki_read_imm_16(); result = D(3); step_compare_word(value, result); break;
    case 0xC2EA1A: /* bge     $c2ea4e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EA1C: /* muls.w  #$b4, D4 */
        value = m68ki_read_imm_16(); renderer_multiply(&D(4), value); break;
    case 0xC2EA20: /* divs.w  D5, D4 */
        value = D(5); renderer_divide(&D(4), (int16_t)value); break;
    case 0xC2EA22: /* asr.w   #1, D4 */
        renderer_asr_word(&D(4), 1); break;
    case 0xC2EA24: /* bcc     $c2ea28 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2EA26: /* addq.w  #1, D4 */
        value = 1u; step_add_word(&D(4), value); break;
    case 0xC2EA28: /* addi.w  #$5a, D4 */
        value = m68ki_read_imm_16(); step_add_word(&D(4), value); break;
    case 0xC2EA2C: /* blt     $c2ea4a */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EA2E: /* cmpi.w  #$b4, D4 */
        value = m68ki_read_imm_16(); result = D(4); step_compare_word(value, result); break;
    case 0xC2EA32: /* bge     $c2ea54 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EA34: /* movem.w D3-D4, (A3) */
        mask = m68ki_read_imm_16(); renderer_store(A(3), mask, 2, -1); break;
    case 0xC2EA38: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC2EA3A: /* cmp.w   ($8,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); result = D(0); step_compare_word(value, result); break;
    case 0xC2EA3E: /* ble     $c2e774 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EA42: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC2EA44: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2EA46: /* clr.w   D3 */
        value = 0; SET_W(D(3), value); flags_logic_w(0); break;
    case 0xC2EA48: /* bra     $c2ea1c */
        step_branch(pc, opcode, 1); break;
    case 0xC2EA4A: /* clr.w   D4 */
        value = 0; SET_W(D(4), value); flags_logic_w(0); break;
    case 0xC2EA4C: /* bra     $c2ea34 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EA4E: /* move.w  #$13f, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2EA52: /* bra     $c2ea1c */
        step_branch(pc, opcode, 1); break;
    case 0xC2EA54: /* move.w  #$b3, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2EA58: /* bra     $c2ea34 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EA5A: /* movem.l D0-D6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2EA5E: /* movem.w (A1,D1.w), D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(step_indexed(A(1)), mask, 2, -1); break;
    case 0xC2EA64: /* subq.w  #1, D3 */
        value = 1u; step_subtract_word(&D(3), value); break;
    case 0xC2EA66: /* subq.w  #1, D4 */
        value = 1u; step_subtract_word(&D(4), value); break;
    case 0xC2EA68: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EA6A: /* sub.w   D5, D6 */
        value = D(5); step_subtract_word(&D(6), value); break;
    case 0xC2EA6C: /* sub.w   D2, D0 */
        value = D(2); step_subtract_word(&D(0), value); break;
    case 0xC2EA6E: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2EA70: /* sub.w   D5, D3 */
        value = D(5); step_subtract_word(&D(3), value); break;
    case 0xC2EA72: /* add.w   D0, D3 */
        value = D(0); step_add_word(&D(3), value); break;
    case 0xC2EA74: /* beq     $c2eacc */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EA76: /* sub.w   D1, D4 */
        value = D(1); step_subtract_word(&D(4), value); break;
    case 0xC2EA78: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2EA7A: /* muls.w  D0, D4 */
        value = D(0); renderer_multiply(&D(4), value); break;
    case 0xC2EA7C: /* move.w  D3, D5 */
        value = D(3); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2EA7E: /* bge     $c2ea82 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EA80: /* neg.w   D5 */
        renderer_negate(&D(5), 2); break;
    case 0xC2EA82: /* asr.w   #1, D5 */
        renderer_asr_word(&D(5), 1); break;
    case 0xC2EA84: /* divs.w  D3, D4 */
        value = D(3); renderer_divide(&D(4), (int16_t)value); break;
    case 0xC2EA86: /* swap    D4 */
        step_swap(&D(4)); break;
    case 0xC2EA88: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2EA8A: /* bge     $c2ea8e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EA8C: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2EA8E: /* cmp.w   D4, D5 */
        value = D(4); result = D(5); step_compare_word(value, result); break;
    case 0xC2EA90: /* bgt     $c2eaa0 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EA92: /* swap    D4 */
        step_swap(&D(4)); break;
    case 0xC2EA94: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2EA96: /* bge     $c2ea9c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EA98: /* subq.w  #1, D4 */
        value = 1u; step_subtract_word(&D(4), value); break;
    case 0xC2EA9A: /* bra     $c2eaa2 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EA9C: /* addq.w  #1, D4 */
        value = 1u; step_add_word(&D(4), value); break;
    case 0xC2EA9E: /* bra     $c2eaa2 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EAA0: /* swap    D4 */
        step_swap(&D(4)); break;
    case 0xC2EAA2: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC2EAA4: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), value); break;
    case 0xC2EAA6: /* divs.w  D3, D6 */
        value = D(3); renderer_divide(&D(6), (int16_t)value); break;
    case 0xC2EAA8: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EAAA: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2EAAC: /* bge     $c2eab0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EAAE: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2EAB0: /* cmp.w   D6, D5 */
        value = D(6); result = D(5); step_compare_word(value, result); break;
    case 0xC2EAB2: /* bgt     $c2eac2 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EAB4: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EAB6: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2EAB8: /* bge     $c2eabe */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EABA: /* subq.w  #1, D6 */
        value = 1u; step_subtract_word(&D(6), value); break;
    case 0xC2EABC: /* bra     $c2eac4 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EABE: /* addq.w  #1, D6 */
        value = 1u; step_add_word(&D(6), value); break;
    case 0xC2EAC0: /* bra     $c2eac4 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EAC2: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EAC4: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2EAC6: /* move.w  D2, D0 */
        value = D(2); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2EAC8: /* bra     $c2ec36 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EACC: /* bra     $c2ec58 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EAD0: /* movem.l D0-D6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2EAD4: /* movem.w (A1,D1.w), D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(step_indexed(A(1)), mask, 2, -1); break;
    case 0xC2EADA: /* subq.w  #1, D3 */
        value = 1u; step_subtract_word(&D(3), value); break;
    case 0xC2EADC: /* subq.w  #1, D4 */
        value = 1u; step_subtract_word(&D(4), value); break;
    case 0xC2EADE: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2EAE0: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2EAE2: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EAE4: /* sub.w   D5, D6 */
        value = D(5); step_subtract_word(&D(6), value); break;
    case 0xC2EAE6: /* sub.w   D2, D0 */
        value = D(2); step_subtract_word(&D(0), value); break;
    case 0xC2EAE8: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2EAEA: /* sub.w   D5, D3 */
        value = D(5); step_subtract_word(&D(3), value); break;
    case 0xC2EAEC: /* add.w   D0, D3 */
        value = D(0); step_add_word(&D(3), value); break;
    case 0xC2EAEE: /* beq     $c2eb48 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EAF0: /* sub.w   D1, D4 */
        value = D(1); step_subtract_word(&D(4), value); break;
    case 0xC2EAF2: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2EAF4: /* muls.w  D0, D4 */
        value = D(0); renderer_multiply(&D(4), value); break;
    case 0xC2EAF6: /* move.w  D3, D5 */
        value = D(3); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2EAF8: /* bge     $c2eafc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EAFA: /* neg.w   D5 */
        renderer_negate(&D(5), 2); break;
    case 0xC2EAFC: /* asr.w   #1, D5 */
        renderer_asr_word(&D(5), 1); break;
    case 0xC2EAFE: /* divs.w  D3, D4 */
        value = D(3); renderer_divide(&D(4), (int16_t)value); break;
    case 0xC2EB00: /* swap    D4 */
        step_swap(&D(4)); break;
    case 0xC2EB02: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2EB04: /* bge     $c2eb08 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EB06: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2EB08: /* cmp.w   D4, D5 */
        value = D(4); result = D(5); step_compare_word(value, result); break;
    case 0xC2EB0A: /* bgt     $c2eb1a */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EB0C: /* swap    D4 */
        step_swap(&D(4)); break;
    case 0xC2EB0E: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2EB10: /* bge     $c2eb16 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EB12: /* subq.w  #1, D4 */
        value = 1u; step_subtract_word(&D(4), value); break;
    case 0xC2EB14: /* bra     $c2eb1c */
        step_branch(pc, opcode, 1); break;
    case 0xC2EB16: /* addq.w  #1, D4 */
        value = 1u; step_add_word(&D(4), value); break;
    case 0xC2EB18: /* bra     $c2eb1c */
        step_branch(pc, opcode, 1); break;
    case 0xC2EB1A: /* swap    D4 */
        step_swap(&D(4)); break;
    case 0xC2EB1C: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC2EB1E: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), value); break;
    case 0xC2EB20: /* divs.w  D3, D6 */
        value = D(3); renderer_divide(&D(6), (int16_t)value); break;
    case 0xC2EB22: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EB24: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2EB26: /* bge     $c2eb2a */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EB28: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2EB2A: /* cmp.w   D6, D5 */
        value = D(6); result = D(5); step_compare_word(value, result); break;
    case 0xC2EB2C: /* bgt     $c2eb3c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EB2E: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EB30: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2EB32: /* bge     $c2eb38 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EB34: /* subq.w  #1, D6 */
        value = 1u; step_subtract_word(&D(6), value); break;
    case 0xC2EB36: /* bra     $c2eb3e */
        step_branch(pc, opcode, 1); break;
    case 0xC2EB38: /* addq.w  #1, D6 */
        value = 1u; step_add_word(&D(6), value); break;
    case 0xC2EB3A: /* bra     $c2eb3e */
        step_branch(pc, opcode, 1); break;
    case 0xC2EB3C: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EB3E: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2EB40: /* move.w  D2, D0 */
        value = D(2); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2EB42: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2EB44: /* bra     $c2ec36 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EB48: /* bra     $c2ec58 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EB4C: /* movem.l D0-D6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2EB50: /* movem.w (A1,D1.w), D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(step_indexed(A(1)), mask, 2, -1); break;
    case 0xC2EB56: /* subq.w  #1, D3 */
        value = 1u; step_subtract_word(&D(3), value); break;
    case 0xC2EB58: /* subq.w  #1, D4 */
        value = 1u; step_subtract_word(&D(4), value); break;
    case 0xC2EB5A: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EB5C: /* sub.w   D5, D6 */
        value = D(5); step_subtract_word(&D(6), value); break;
    case 0xC2EB5E: /* sub.w   D2, D1 */
        value = D(2); step_subtract_word(&D(1), value); break;
    case 0xC2EB60: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2EB62: /* sub.w   D5, D4 */
        value = D(5); step_subtract_word(&D(4), value); break;
    case 0xC2EB64: /* add.w   D1, D4 */
        value = D(1); step_add_word(&D(4), value); break;
    case 0xC2EB66: /* beq     $c2ebbe */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EB68: /* sub.w   D0, D3 */
        value = D(0); step_subtract_word(&D(3), value); break;
    case 0xC2EB6A: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2EB6C: /* muls.w  D1, D3 */
        value = D(1); renderer_multiply(&D(3), value); break;
    case 0xC2EB6E: /* move.w  D4, D5 */
        value = D(4); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2EB70: /* bge     $c2eb74 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EB72: /* neg.w   D5 */
        renderer_negate(&D(5), 2); break;
    case 0xC2EB74: /* asr.w   #1, D5 */
        renderer_asr_word(&D(5), 1); break;
    case 0xC2EB76: /* divs.w  D4, D3 */
        value = D(4); renderer_divide(&D(3), (int16_t)value); break;
    case 0xC2EB78: /* swap    D3 */
        step_swap(&D(3)); break;
    case 0xC2EB7A: /* tst.w   D3 */
        value = D(3); flags_logic_w(value); break;
    case 0xC2EB7C: /* bge     $c2eb80 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EB7E: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2EB80: /* cmp.w   D3, D5 */
        value = D(3); result = D(5); step_compare_word(value, result); break;
    case 0xC2EB82: /* bgt     $c2eb92 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EB84: /* swap    D3 */
        step_swap(&D(3)); break;
    case 0xC2EB86: /* tst.w   D3 */
        value = D(3); flags_logic_w(value); break;
    case 0xC2EB88: /* bge     $c2eb8e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EB8A: /* subq.w  #1, D3 */
        value = 1u; step_subtract_word(&D(3), value); break;
    case 0xC2EB8C: /* bra     $c2eb94 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EB8E: /* addq.w  #1, D3 */
        value = 1u; step_add_word(&D(3), value); break;
    case 0xC2EB90: /* bra     $c2eb94 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EB92: /* swap    D3 */
        step_swap(&D(3)); break;
    case 0xC2EB94: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC2EB96: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), value); break;
    case 0xC2EB98: /* divs.w  D4, D6 */
        value = D(4); renderer_divide(&D(6), (int16_t)value); break;
    case 0xC2EB9A: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EB9C: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2EB9E: /* bge     $c2eba2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EBA0: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2EBA2: /* cmp.w   D6, D5 */
        value = D(6); result = D(5); step_compare_word(value, result); break;
    case 0xC2EBA4: /* bgt     $c2ebb4 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EBA6: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EBA8: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2EBAA: /* bge     $c2ebb0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EBAC: /* subq.w  #1, D6 */
        value = 1u; step_subtract_word(&D(6), value); break;
    case 0xC2EBAE: /* bra     $c2ebb6 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EBB0: /* addq.w  #1, D6 */
        value = 1u; step_add_word(&D(6), value); break;
    case 0xC2EBB2: /* bra     $c2ebb6 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EBB4: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EBB6: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2EBB8: /* move.w  D2, D1 */
        value = D(2); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2EBBA: /* bra     $c2ec36 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EBBE: /* bra     $c2ec58 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EBC2: /* movem.l D0-D6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2EBC6: /* movem.w (A1,D1.w), D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(step_indexed(A(1)), mask, 2, -1); break;
    case 0xC2EBCC: /* subq.w  #1, D3 */
        value = 1u; step_subtract_word(&D(3), value); break;
    case 0xC2EBCE: /* subq.w  #1, D4 */
        value = 1u; step_subtract_word(&D(4), value); break;
    case 0xC2EBD0: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2EBD2: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2EBD4: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EBD6: /* sub.w   D5, D6 */
        value = D(5); step_subtract_word(&D(6), value); break;
    case 0xC2EBD8: /* sub.w   D2, D1 */
        value = D(2); step_subtract_word(&D(1), value); break;
    case 0xC2EBDA: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2EBDC: /* sub.w   D5, D4 */
        value = D(5); step_subtract_word(&D(4), value); break;
    case 0xC2EBDE: /* add.w   D1, D4 */
        value = D(1); step_add_word(&D(4), value); break;
    case 0xC2EBE0: /* beq     $c2ec58 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2EBE2: /* sub.w   D0, D3 */
        value = D(0); step_subtract_word(&D(3), value); break;
    case 0xC2EBE4: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2EBE6: /* muls.w  D1, D3 */
        value = D(1); renderer_multiply(&D(3), value); break;
    case 0xC2EBE8: /* move.w  D4, D5 */
        value = D(4); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2EBEA: /* bge     $c2ebee */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EBEC: /* neg.w   D5 */
        renderer_negate(&D(5), 2); break;
    case 0xC2EBEE: /* asr.w   #1, D5 */
        renderer_asr_word(&D(5), 1); break;
    case 0xC2EBF0: /* divs.w  D4, D3 */
        value = D(4); renderer_divide(&D(3), (int16_t)value); break;
    case 0xC2EBF2: /* swap    D3 */
        step_swap(&D(3)); break;
    case 0xC2EBF4: /* tst.w   D3 */
        value = D(3); flags_logic_w(value); break;
    case 0xC2EBF6: /* bge     $c2ebfa */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EBF8: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2EBFA: /* cmp.w   D3, D5 */
        value = D(3); result = D(5); step_compare_word(value, result); break;
    case 0xC2EBFC: /* bgt     $c2ec0c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EBFE: /* swap    D3 */
        step_swap(&D(3)); break;
    case 0xC2EC00: /* tst.w   D3 */
        value = D(3); flags_logic_w(value); break;
    case 0xC2EC02: /* bge     $c2ec08 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EC04: /* subq.w  #1, D3 */
        value = 1u; step_subtract_word(&D(3), value); break;
    case 0xC2EC06: /* bra     $c2ec0e */
        step_branch(pc, opcode, 1); break;
    case 0xC2EC08: /* addq.w  #1, D3 */
        value = 1u; step_add_word(&D(3), value); break;
    case 0xC2EC0A: /* bra     $c2ec0e */
        step_branch(pc, opcode, 1); break;
    case 0xC2EC0C: /* swap    D3 */
        step_swap(&D(3)); break;
    case 0xC2EC0E: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC2EC10: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), value); break;
    case 0xC2EC12: /* divs.w  D4, D6 */
        value = D(4); renderer_divide(&D(6), (int16_t)value); break;
    case 0xC2EC14: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EC16: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2EC18: /* bge     $c2ec1c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EC1A: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2EC1C: /* cmp.w   D6, D5 */
        value = D(6); result = D(5); step_compare_word(value, result); break;
    case 0xC2EC1E: /* bgt     $c2ec2e */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EC20: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EC22: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2EC24: /* bge     $c2ec2a */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2EC26: /* subq.w  #1, D6 */
        value = 1u; step_subtract_word(&D(6), value); break;
    case 0xC2EC28: /* bra     $c2ec30 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EC2A: /* addq.w  #1, D6 */
        value = 1u; step_add_word(&D(6), value); break;
    case 0xC2EC2C: /* bra     $c2ec30 */
        step_branch(pc, opcode, 1); break;
    case 0xC2EC2E: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2EC30: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2EC32: /* move.w  D2, D1 */
        value = D(2); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2EC34: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2EC36: /* movem.w D0-D2, $c45ac6.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC2EC3E: /* move.w  D2, D5 */
        value = D(2); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2EC40: /* blt     $c2ec58 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2EC42: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2EC44: /* cmp.w   D5, D0 */
        value = D(5); result = D(0); step_compare_word(value, result); break;
    case 0xC2EC46: /* bgt     $c2ec58 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EC48: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2EC4A: /* cmp.w   D5, D0 */
        value = D(5); result = D(0); step_compare_word(value, result); break;
    case 0xC2EC4C: /* bgt     $c2ec58 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EC4E: /* cmp.w   D6, D1 */
        value = D(6); result = D(1); step_compare_word(value, result); break;
    case 0xC2EC50: /* bgt     $c2ec58 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2EC52: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2EC54: /* cmp.w   D6, D1 */
        value = D(6); result = D(1); step_compare_word(value, result); break;
    case 0xC2EC56: /* ble     $c2ec60 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2EC58: /* moveq   #$1, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC2EC5A: /* movem.l (A7)+, D0-D6 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC2EC5E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2EC60: /* moveq   #$0, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC2EC62: /* movem.l (A7)+, D0-D6 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC2EC66: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C2EA5A_step(void) { return glue_C2E758_step(); }

int glue_C2EAD0_step(void) { return glue_C2E758_step(); }

int glue_C2EB4C_step(void) { return glue_C2E758_step(); }

int glue_C2EBC2_step(void) { return glue_C2E758_step(); }
