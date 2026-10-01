/* View/record matrix routing, aim and rotation builders; matrix_route.c, view.c and matrix.c.
 * CPU effects and source instruction/bus/event boundaries stay in glue. */
#include "glue_renderer_step_math.h"

int glue_C2D99C_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC258C8: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC258CE: /* beq     $c2597e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC258D2: /* tst.b   $c457ad.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC258D8: /* bne     $c2597e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC258DC: /* tst.b   $c457b4.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC258E2: /* bne     $c2597e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC258E6: /* tst.b   $c458ae.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC258EC: /* bne     $c2597e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC258F0: /* move.w  $c45a94.l, D4 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC258F6: /* move.w  $c45a96.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC258FC: /* move.b  $c4582e.l, D2 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(2), value); flags_logic_b(value); break;
    case 0xC25902: /* beq     $c25950 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC25904: /* andi.b  #$20, D2 */
        value = m68ki_read_imm_16(); result = D(2) & value; SET_B(D(2), result); flags_logic_b(result); break;
    case 0xC25908: /* bne     $c2592c */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2590A: /* cmpi.w  #$3840, D4 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(4)); break;
    case 0xC2590E: /* blt     $c25920 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC25910: /* addi.w  #$1e0, D4 */
        value = m68ki_read_imm_16(); step_add_word(&D(4), value); break;
    case 0xC25914: /* cmpi.w  #$7080, D4 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(4)); break;
    case 0xC25918: /* blt     $c25948 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2591A: /* subi.w  #$7080, D4 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(4), value); break;
    case 0xC2591E: /* bra     $c25948 */
        step_branch(pc, opcode, 1); break;
    case 0xC25920: /* addi.w  #$1e0, D4 */
        value = m68ki_read_imm_16(); step_add_word(&D(4), value); break;
    case 0xC25924: /* cmpi.w  #$1c20, D4 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(4)); break;
    case 0xC25928: /* blt     $c25948 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2592A: /* bra     $c2597e */
        step_branch(pc, opcode, 1); break;
    case 0xC2592C: /* cmpi.w  #$3840, D4 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(4)); break;
    case 0xC25930: /* blt     $c2593e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC25932: /* subi.w  #$1e0, D4 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(4), value); break;
    case 0xC25936: /* cmpi.w  #$5460, D4 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(4)); break;
    case 0xC2593A: /* bgt     $c25948 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2593C: /* bra     $c2597e */
        step_branch(pc, opcode, 1); break;
    case 0xC2593E: /* subi.w  #$1e0, D4 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(4), value); break;
    case 0xC25942: /* bge     $c25948 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC25944: /* addi.w  #$7080, D4 */
        value = m68ki_read_imm_16(); step_add_word(&D(4), value); break;
    case 0xC25948: /* move.w  D4, $c45a94.l */
        value = D(4); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2594E: /* bra     $c2597e */
        step_branch(pc, opcode, 1); break;
    case 0xC25950: /* move.b  $c45830.l, D2 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(2), value); flags_logic_b(value); break;
    case 0xC25956: /* beq     $c2597e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC25958: /* andi.b  #$8, D2 */
        value = m68ki_read_imm_16(); result = D(2) & value; SET_B(D(2), result); flags_logic_b(result); break;
    case 0xC2595C: /* bne     $c2596e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2595E: /* addi.w  #$1e0, D1 */
        value = m68ki_read_imm_16(); step_add_word(&D(1), value); break;
    case 0xC25962: /* cmpi.w  #$7080, D1 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(1)); break;
    case 0xC25966: /* blt     $c25978 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC25968: /* subi.w  #$7080, D1 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(1), value); break;
    case 0xC2596C: /* bra     $c25978 */
        step_branch(pc, opcode, 1); break;
    case 0xC2596E: /* subi.w  #$1e0, D1 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(1), value); break;
    case 0xC25972: /* bge     $c25978 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC25974: /* addi.w  #$7080, D1 */
        value = m68ki_read_imm_16(); step_add_word(&D(1), value); break;
    case 0xC25978: /* move.w  D1, $c45a96.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2597E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2D970: /* adda.w  #$92, A1 */
        value = m68ki_read_imm_16(); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2D974: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2D976: /* moveq   #$0, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC2D978: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC2D97A: /* move.w  #$7080, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2D97E: /* tst.w   D5 */
        value = D(5); flags_logic_w(value); break;
    case 0xC2D980: /* beq     $c2d986 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2D982: /* move.w  D1, D0 */
        value = D(1); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2D984: /* sub.w   D5, D0 */
        value = D(5); step_subtract_word(&D(0), value); break;
    case 0xC2D986: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2D988: /* beq     $c2d98e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2D98A: /* move.w  D1, D2 */
        value = D(1); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2D98C: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2D98E: /* tst.w   D7 */
        value = D(7); flags_logic_w(value); break;
    case 0xC2D990: /* beq     $c2d996 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2D992: /* move.w  D1, D4 */
        value = D(1); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2D994: /* sub.w   D7, D4 */
        value = D(7); step_subtract_word(&D(4), value); break;
    case 0xC2D996: /* bsr     $c2e514 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2D99A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2D99C: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC2D9A2: /* beq     $c2d9aa */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2D9A4: /* bsr     $c2d9ba */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2D9A8: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2D9AA: /* bsr     $c2db18 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2D9AE: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2D9B0: /* jsr     $c258c8.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2D9B6: /* bra     $c2dab0 */
        step_branch(pc, opcode, 1); break;
    case 0xC2D9BA: /* tst.b   $c457b4.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC2D9C0: /* beq     $c2d9b0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2D9C2: /* lea     $c46184.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2D9C8: /* move.w  $c458de.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2D9CE: /* bne     $c2d9e6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2D9D0: /* move.w  $c459c2.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2D9D6: /* btst    #$6, ($1,A1,D2.w) */
        value = m68ki_read_imm_16(); address = step_indexed(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC2D9DC: /* beq     $c2d9e8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2D9DE: /* cmpi.b  #$30, ($62,A1,D2.w) */
        value = m68ki_read_imm_16(); address = step_indexed(A(1)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC2D9E4: /* bne     $c2d9e8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2D9E6: /* adda.w  D2, A1 */
        value = D(2); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2D9E8: /* move.b  ($62,A1), D0 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC2D9EC: /* andi.b  #$f0, D0 */
        value = m68ki_read_imm_16(); result = D(0) & value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC2D9F0: /* cmpi.b  #$30, D0 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(0)); break;
    case 0xC2D9F4: /* beq     $c2da0a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2D9F6: /* moveq   #$0, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC2D9F8: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC2D9FA: /* moveq   #$1, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC2D9FC: /* cmpi.b  #$6, $c458ae.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC2DA04: /* bne     $c2da10 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2DA06: /* moveq   #$5, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC2DA08: /* bra     $c2da10 */
        step_branch(pc, opcode, 1); break;
    case 0xC2DA0A: /* moveq   #$0, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC2DA0C: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC2DA0E: /* moveq   #-$4, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC2DA10: /* jsr     $c091e0.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2DA16: /* move.l  D2, D4 */
        value = D(2); D(4) = value; flags_logic_l(value); break;
    case 0xC2DA18: /* move.l  D1, D3 */
        value = D(1); D(3) = value; flags_logic_l(value); break;
    case 0xC2DA1A: /* move.l  D0, D2 */
        value = D(0); D(2) = value; flags_logic_l(value); break;
    case 0xC2DA1C: /* sub.l   $c45c3e.l, D2 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_subtract_long(&D(2), value); break;
    case 0xC2DA22: /* sub.l   $c45c42.l, D3 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_subtract_long(&D(3), value); break;
    case 0xC2DA28: /* sub.l   $c45c46.l, D4 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_subtract_long(&D(4), value); break;
    case 0xC2DA2E: /* tst.b   $c457b4.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC2DA34: /* blt     $c2da62 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2DA36: /* tst.b   $c458ae.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC2DA3C: /* beq     $c2da46 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2DA3E: /* move.l  #$230, D5 */
        value = m68ki_read_imm_32(); D(5) = value; flags_logic_l(value); break;
    case 0xC2DA44: /* bra     $c2da6c */
        step_branch(pc, opcode, 1); break;
    case 0xC2DA46: /* moveq   #$0, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC2DA48: /* move.w  $c458de.l, D5 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2DA4E: /* or.b    $c458b2.l, D5 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); result = D(5) | value; SET_B(D(5), result); flags_logic_b(result); break;
    case 0xC2DA54: /* cmp.w   $c45a60.l, D5 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_compare_word(value, D(5)); break;
    case 0xC2DA5A: /* beq     $c2da66 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2DA5C: /* move.w  D5, $c45a60.l */
        value = D(5); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2DA62: /* moveq   #-$1, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC2DA64: /* bra     $c2da6c */
        step_branch(pc, opcode, 1); break;
    case 0xC2DA66: /* move.l  #$7d0, D5 */
        value = m68ki_read_imm_32(); D(5) = value; flags_logic_l(value); break;
    case 0xC2DA6C: /* tst.b   $c457ae.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC2DA72: /* bne     $c2dab0 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2DA74: /* move.w  $c45a94.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DA7A: /* move.w  $c45a96.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DA80: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC2DA82: /* ext.l   D1 */
        D(1) = (uint32_t)(int32_t)(int16_t)D(1); flags_logic_l(D(1)); break;
    case 0xC2DA84: /* tst.b   $c457b5.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC2DA8A: /* bne     $c2da8e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2DA8C: /* moveq   #-$1, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC2DA8E: /* movem.l D0-D5, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2DA92: /* jsr     $c123fa.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2DA98: /* adda.w  #$18, A7 */
        value = m68ki_read_imm_16(); A(7) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2DA9C: /* move.w  $c45ac0.l, $c45a94.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2DAA6: /* move.w  $c45ac2.l, $c45a96.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2DAB0: /* move.w  $c45a94.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DAB6: /* move.w  $c45a96.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DABC: /* lea     $c45bd8.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DAC2: /* bsr     $c2e38e */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DAC6: /* lea     $c45bd8.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DACC: /* bsr     $c2e5ac */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DAD0: /* move.w  $c45a96.l, D4 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2DAD6: /* lea     $c45bfc.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DADC: /* bsr     $c2e346 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DAE0: /* movem.w $c461ea.l, D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC2DAE8: /* movem.l D0-D2, $c45a88.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 4, -1); break;
    case 0xC2DAF0: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2DAF2: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC2DAF8: /* adda.w  $c458de.l, A0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2DAFE: /* move.w  #$7080, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DB02: /* move.w  ($68,A0), D4 */
        value = m68k_read_memory_16(step_displacement(A(0))); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2DB06: /* beq     $c2db0c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2DB08: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC2DB0A: /* move.w  D1, D4 */
        value = D(1); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2DB0C: /* lea     $c45c0e.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DB12: /* bsr     $c2e370 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DB16: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2DB18: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC2DB1E: /* adda.w  $c458de.l, A0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2DB24: /* move.w  ($68,A0), D4 */
        value = m68k_read_memory_16(step_displacement(A(0))); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2DB28: /* lea     $c45bfc.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DB2E: /* bsr     $c2e346 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DB32: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC2DB38: /* adda.w  $c458de.l, A0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2DB3E: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC2DB44: /* bne     $c2db5a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2DB46: /* movem.w ($66,A0), D0/D2/D4 */
        mask = m68ki_read_imm_16(); renderer_load(step_displacement(A(0)), mask, 2, -1); break;
    case 0xC2DB4C: /* lea     $c45bea.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DB52: /* move.l  A0, -(A7) */
        value = A(0); A(7) -= 4; m68k_write_memory_16(A(7) + 2, value); m68k_write_memory_16(A(7), value >> 16); flags_logic_l(value); break;
    case 0xC2DB54: /* bsr     $c2e3de */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DB58: /* movea.l (A7)+, A0 */
        address = A(7); A(7) += 4; value = m68k_read_memory_32(address); A(0) = value; break;
    case 0xC2DB5A: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2DB5C: /* moveq   #$0, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC2DB5E: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC2DB60: /* move.b  ($62,A0), D3 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(3), value); flags_logic_b(value); break;
    case 0xC2DB64: /* andi.b  #$f0, D3 */
        value = m68ki_read_imm_16(); result = D(3) & value; SET_B(D(3), result); flags_logic_b(result); break;
    case 0xC2DB68: /* move.b  $c457a7.l, D1 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC2DB6E: /* beq     $c2db86 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2DB70: /* cmpi.b  #$b, D1 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(1)); break;
    case 0xC2DB74: /* ble     $c2dc42 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2DB78: /* subi.b  #$b, D1 */
        value = m68ki_read_imm_16(); step_subtract_byte(&D(1), value); break;
    case 0xC2DB7C: /* cmpi.b  #$30, D3 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(3)); break;
    case 0xC2DB80: /* bne     $c2dbba */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2DB82: /* bra     $c2dbfa */
        step_branch(pc, opcode, 1); break;
    case 0xC2DB86: /* cmpi.b  #$30, D3 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(3)); break;
    case 0xC2DB8A: /* bne     $c2db92 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2DB8C: /* moveq   #$c, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC2DB8E: /* bra     $c2dc42 */
        step_branch(pc, opcode, 1); break;
    case 0xC2DB92: /* move.b  $c457b1.l, D1 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC2DB98: /* cmpi.b  #$1, D1 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(1)); break;
    case 0xC2DB9C: /* bgt     $c2dbb0 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2DB9E: /* movem.w ($66,A0), D0/D2/D4 */
        mask = m68ki_read_imm_16(); renderer_load(step_displacement(A(0)), mask, 2, -1); break;
    case 0xC2DBA4: /* movem.l D0/D2/D4, $c45a88.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 4, -1); break;
    case 0xC2DBAC: /* bra     $c2dc9a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DBB0: /* subq.b  #2, D1 */
        value = 2u; step_subtract_byte(&D(1), value); break;
    case 0xC2DBB2: /* beq     $c2dc3c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2DBB6: /* bra     $c2dc36 */
        step_branch(pc, opcode, 1); break;
    case 0xC2DBBA: /* subq.b  #1, D1 */
        value = 1u; step_subtract_byte(&D(1), value); break;
    case 0xC2DBBC: /* beq     $c2dbde */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2DBBE: /* move.w  ($66,A0), D0 */
        value = m68k_read_memory_16(step_displacement(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DBC2: /* cmpi.w  #$230, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2DBC6: /* blt     $c2dbce */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2DBC8: /* cmpi.w  #$6e50, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2DBCC: /* blt     $c2dbd6 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2DBCE: /* move.w  #$19f0, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DBD2: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DBD6: /* move.w  #$1c20, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DBDA: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DBDE: /* move.w  ($66,A0), D0 */
        value = m68k_read_memory_16(step_displacement(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DBE2: /* cmpi.w  #$230, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2DBE6: /* blt     $c2dbee */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2DBE8: /* cmpi.w  #$6e50, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2DBEC: /* blt     $c2dbf4 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2DBEE: /* move.w  #$5690, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DBF2: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DBF4: /* move.w  #$5460, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DBF8: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DBFA: /* subq.b  #1, D1 */
        value = 1u; step_subtract_byte(&D(1), value); break;
    case 0xC2DBFC: /* beq     $c2dc1a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2DBFE: /* move.w  ($66,A0), D0 */
        value = m68k_read_memory_16(step_displacement(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DC02: /* cmpi.w  #$1e50, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2DC06: /* bge     $c2dc14 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2DC08: /* cmpi.w  #$19f0, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2DC0C: /* ble     $c2dc14 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2DC0E: /* move.w  #$230, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DC12: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DC14: /* move.w  #$0, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DC18: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DC1A: /* move.w  ($66,A0), D0 */
        value = m68k_read_memory_16(step_displacement(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DC1E: /* cmpi.w  #$1e50, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2DC22: /* bge     $c2dc30 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2DC24: /* cmpi.w  #$19f0, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2DC28: /* ble     $c2dc30 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2DC2A: /* move.w  #$3610, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DC2E: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DC30: /* move.w  #$3840, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DC34: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DC36: /* move.w  #$280, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DC3A: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DC3C: /* move.w  #$6e00, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DC40: /* bra     $c2dc6a */
        step_branch(pc, opcode, 1); break;
    case 0xC2DC42: /* cmpi.b  #$30, D3 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(3)); break;
    case 0xC2DC46: /* bne     $c2dc50 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2DC48: /* lea     $c2dd04.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DC4E: /* bra     $c2dc56 */
        step_branch(pc, opcode, 1); break;
    case 0xC2DC50: /* lea     $c2dcc2.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DC56: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC2DC58: /* ext.w   D1 */
        SET_W(D(1), (int16_t)(int8_t)D(1)); flags_logic_w(D(1)); break;
    case 0xC2DC5A: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC2DC5C: /* move.w  D1, D0 */
        value = D(1); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DC5E: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC2DC60: /* add.w   D0, D1 */
        value = D(0); step_add_word(&D(1), value); break;
    case 0xC2DC62: /* adda.w  D1, A1 */
        value = D(1); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2DC64: /* move.w  (A1)+, D0 */
        address = A(1); A(1) += 2; value = m68k_read_memory_16(address); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DC66: /* move.w  (A1)+, D2 */
        address = A(1); A(1) += 2; value = m68k_read_memory_16(address); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DC68: /* move.w  (A1), D4 */
        value = m68k_read_memory_16(A(1)); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2DC6A: /* move.l  A0, -(A7) */
        value = A(0); A(7) -= 4; m68k_write_memory_16(A(7) + 2, value); m68k_write_memory_16(A(7), value >> 16); flags_logic_l(value); break;
    case 0xC2DC6C: /* movem.w D0/D2/D4, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 2, 7); break;
    case 0xC2DC70: /* lea     $c45c20.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DC76: /* bsr     $c2e3de */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DC7A: /* lea     $c45c20.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DC80: /* bsr     $c2e5ac */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DC84: /* movem.w (A7)+, D0/D2/D4 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 2, 7); break;
    case 0xC2DC88: /* movea.l (A7)+, A0 */
        address = A(7); A(7) += 4; value = m68k_read_memory_32(address); A(0) = value; break;
    case 0xC2DC8A: /* lea     ($80,A0), A4 */
        A(4) = step_displacement(A(0)); break;
    case 0xC2DC8E: /* bsr     $c2dee0 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DC92: /* movem.l D4-D6, $c45a88.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 4, -1); break;
    case 0xC2DC9A: /* move.w  $c45a8a.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DCA0: /* move.w  $c45a8e.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DCA6: /* move.w  $c45a92.l, D4 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2DCAC: /* lea     $c45bd8.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DCB2: /* bsr     $c2e3de */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DCB6: /* lea     $c45bd8.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DCBC: /* bsr     $c2e5ac */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DCC0: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2DEE0: /* movem.l D1/A0-A1, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2DEE4: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2DEE6: /* bge     $c2deec */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2DEE8: /* addi.w  #$7080, D0 */
        value = m68ki_read_imm_16(); step_add_word(&D(0), value); break;
    case 0xC2DEEC: /* tst.w   D2 */
        value = D(2); flags_logic_w(value); break;
    case 0xC2DEEE: /* bge     $c2def4 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2DEF0: /* addi.w  #$7080, D2 */
        value = m68ki_read_imm_16(); step_add_word(&D(2), value); break;
    case 0xC2DEF4: /* tst.w   D4 */
        value = D(4); flags_logic_w(value); break;
    case 0xC2DEF6: /* bge     $c2defc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2DEF8: /* addi.w  #$7080, D4 */
        value = m68ki_read_imm_16(); step_add_word(&D(4), value); break;
    case 0xC2DEFC: /* lea     $c45b90.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2DF02: /* bsr     $c2e47a */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2DF06: /* lea     (A4), A1 */
        A(1) = A(4); break;
    case 0xC2DF08: /* lea     $c45b90.l, A2 */
        A(2) = m68ki_read_imm_32(); break;
    case 0xC2DF0E: /* lea     $c45ba2.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC2DF14: /* move.w  (A1), D0 */
        value = m68k_read_memory_16(A(1)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DF16: /* muls.w  (A2), D0 */
        value = m68k_read_memory_16(A(2)); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2DF18: /* move.w  ($6,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DF1C: /* muls.w  ($2,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2DF20: /* move.w  ($c,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DF24: /* muls.w  ($4,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC2DF28: /* add.l   D1, D0 */
        value = D(1); step_add_long(&D(0), value); break;
    case 0xC2DF2A: /* add.l   D2, D0 */
        value = D(2); step_add_long(&D(0), value); break;
    case 0xC2DF2C: /* move.l  D0, (A3)+ */
        value = D(0); address = A(3); A(3) += 4; step_write_long(address, value); flags_logic_l(value); break;
    case 0xC2DF2E: /* move.w  ($2,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DF32: /* muls.w  (A2), D0 */
        value = m68k_read_memory_16(A(2)); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2DF34: /* move.w  ($8,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DF38: /* muls.w  ($2,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2DF3C: /* move.w  ($e,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DF40: /* muls.w  ($4,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC2DF44: /* add.l   D1, D0 */
        value = D(1); step_add_long(&D(0), value); break;
    case 0xC2DF46: /* add.l   D2, D0 */
        value = D(2); step_add_long(&D(0), value); break;
    case 0xC2DF48: /* move.l  D0, (A3)+ */
        value = D(0); address = A(3); A(3) += 4; step_write_long(address, value); flags_logic_l(value); break;
    case 0xC2DF4A: /* move.w  ($4,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DF4E: /* muls.w  (A2), D0 */
        value = m68k_read_memory_16(A(2)); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2DF50: /* move.w  ($a,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DF54: /* muls.w  ($2,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2DF58: /* move.w  ($10,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DF5C: /* muls.w  ($4,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC2DF60: /* add.l   D1, D0 */
        value = D(1); step_add_long(&D(0), value); break;
    case 0xC2DF62: /* add.l   D2, D0 */
        value = D(2); step_add_long(&D(0), value); break;
    case 0xC2DF64: /* move.l  D0, (A3)+ */
        value = D(0); address = A(3); A(3) += 4; step_write_long(address, value); flags_logic_l(value); break;
    case 0xC2DF66: /* move.w  (A1), D0 */
        value = m68k_read_memory_16(A(1)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DF68: /* muls.w  ($6,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2DF6C: /* move.w  ($6,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DF70: /* muls.w  ($8,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2DF74: /* move.w  ($c,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DF78: /* muls.w  ($a,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC2DF7C: /* add.l   D1, D0 */
        value = D(1); step_add_long(&D(0), value); break;
    case 0xC2DF7E: /* add.l   D2, D0 */
        value = D(2); step_add_long(&D(0), value); break;
    case 0xC2DF80: /* move.l  D0, (A3)+ */
        value = D(0); address = A(3); A(3) += 4; step_write_long(address, value); flags_logic_l(value); break;
    case 0xC2DF82: /* move.w  ($2,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DF86: /* muls.w  ($6,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2DF8A: /* move.w  ($8,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DF8E: /* muls.w  ($8,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2DF92: /* move.w  ($e,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DF96: /* muls.w  ($a,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC2DF9A: /* add.l   D1, D0 */
        value = D(1); step_add_long(&D(0), value); break;
    case 0xC2DF9C: /* add.l   D2, D0 */
        value = D(2); step_add_long(&D(0), value); break;
    case 0xC2DF9E: /* move.l  D0, (A3)+ */
        value = D(0); address = A(3); A(3) += 4; step_write_long(address, value); flags_logic_l(value); break;
    case 0xC2DFA0: /* move.w  ($4,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DFA4: /* muls.w  ($6,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2DFA8: /* move.w  ($a,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DFAC: /* muls.w  ($8,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2DFB0: /* move.w  ($10,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DFB4: /* muls.w  ($a,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC2DFB8: /* add.l   D1, D0 */
        value = D(1); step_add_long(&D(0), value); break;
    case 0xC2DFBA: /* add.l   D2, D0 */
        value = D(2); step_add_long(&D(0), value); break;
    case 0xC2DFBC: /* move.l  D0, (A3)+ */
        value = D(0); address = A(3); A(3) += 4; step_write_long(address, value); flags_logic_l(value); break;
    case 0xC2DFBE: /* move.w  (A1), D0 */
        value = m68k_read_memory_16(A(1)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DFC0: /* muls.w  ($c,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2DFC4: /* move.w  ($6,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DFC8: /* muls.w  ($e,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2DFCC: /* move.w  ($c,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DFD0: /* muls.w  ($10,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC2DFD4: /* add.l   D1, D0 */
        value = D(1); step_add_long(&D(0), value); break;
    case 0xC2DFD6: /* add.l   D2, D0 */
        value = D(2); step_add_long(&D(0), value); break;
    case 0xC2DFD8: /* move.l  D0, (A3)+ */
        value = D(0); address = A(3); A(3) += 4; step_write_long(address, value); flags_logic_l(value); break;
    case 0xC2DFDA: /* move.w  ($2,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DFDE: /* muls.w  ($c,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2DFE2: /* move.w  ($8,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2DFE6: /* muls.w  ($e,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2DFEA: /* move.w  ($e,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2DFEE: /* muls.w  ($10,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC2DFF2: /* add.l   D1, D0 */
        value = D(1); step_add_long(&D(0), value); break;
    case 0xC2DFF4: /* add.l   D2, D0 */
        value = D(2); step_add_long(&D(0), value); break;
    case 0xC2DFF6: /* move.l  D0, (A3)+ */
        value = D(0); address = A(3); A(3) += 4; step_write_long(address, value); flags_logic_l(value); break;
    case 0xC2DFF8: /* move.w  ($4,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2DFFC: /* muls.w  ($c,A2), D0 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2E000: /* move.w  ($a,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E004: /* muls.w  ($e,A2), D1 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2E008: /* move.w  ($10,A1), D2 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E00C: /* muls.w  ($10,A2), D2 */
        value = m68k_read_memory_16(step_displacement(A(2))); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC2E010: /* add.l   D1, D0 */
        value = D(1); step_add_long(&D(0), value); break;
    case 0xC2E012: /* add.l   D2, D0 */
        value = D(2); step_add_long(&D(0), value); break;
    case 0xC2E014: /* move.l  D0, (A3) */
        value = D(0); step_write_long(A(3), value); flags_logic_l(value); break;
    case 0xC2E016: /* move.l  $c45bbe.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC2E01C: /* neg.l   D0 */
        renderer_negate(&D(0), 4); break;
    case 0xC2E01E: /* move.l  D0, D1 */
        value = D(0); D(1) = value; flags_logic_l(value); break;
    case 0xC2E020: /* bge     $c2e024 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E022: /* neg.l   D1 */
        renderer_negate(&D(1), 4); break;
    case 0xC2E024: /* cmpi.l  #$e210000, D1 */
        value = m68ki_read_imm_32(); step_compare_long(value, D(1)); break;
    case 0xC2E02A: /* blt     $c2e07c */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E02C: /* cmpi.l  #$ff60000, D1 */
        value = m68ki_read_imm_32(); step_compare_long(value, D(1)); break;
    case 0xC2E032: /* blt     $c2e056 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E034: /* lea     $c3e3e8.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2E03A: /* moveq   #$c, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC2E03C: /* asr.l   D2, D1 */
        step_asr_long(&D(1), D(2)); break;
    case 0xC2E03E: /* bcc     $c2e042 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2E040: /* addq.l  #1, D1 */
        value = 1u; step_add_long(&D(1), value); break;
    case 0xC2E042: /* subi.l  #$ff08, D1 */
        value = m68ki_read_imm_32(); step_subtract_long(&D(1), value); break;
    case 0xC2E048: /* cmpi.l  #$f8, D1 */
        value = m68ki_read_imm_32(); step_compare_long(value, D(1)); break;
    case 0xC2E04E: /* ble     $c2e072 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E050: /* move.w  #$f8, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E054: /* bra     $c2e072 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E056: /* lea     $c3df98.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2E05C: /* moveq   #$10, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC2E05E: /* asr.l   D2, D1 */
        step_asr_long(&D(1), D(2)); break;
    case 0xC2E060: /* bcc     $c2e064 */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2E062: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC2E064: /* subi.w  #$ddb, D1 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(1), value); break;
    case 0xC2E068: /* cmpi.w  #$225, D1 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(1)); break;
    case 0xC2E06C: /* ble     $c2e072 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E06E: /* move.w  #$225, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E072: /* tst.l   D0 */
        value = D(0); flags_logic_l(value); break;
    case 0xC2E074: /* bge     $c2e078 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E076: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2E078: /* move.w  D1, D0 */
        value = D(1); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2E07A: /* bra     $c2e08a */
        step_branch(pc, opcode, 1); break;
    case 0xC2E07C: /* lea     $c3dd92.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2E082: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E084: /* asr.w   #4, D0 */
        renderer_asr_word(&D(0), 4); break;
    case 0xC2E086: /* bcc     $c2e08a */
        step_branch(pc, opcode, COND_CC()); break;
    case 0xC2E088: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC2E08A: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2E08C: /* move.w  D0, D7 */
        value = D(0); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E08E: /* blt     $c2e09e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E090: /* move.w  (A1,D0.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E094: /* move.w  D2, D4 */
        value = D(2); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2E096: /* move.w  #$384, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E09A: /* sub.w   D2, D3 */
        value = D(2); step_subtract_word(&D(3), value); break;
    case 0xC2E09C: /* bra     $c2e0b2 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E09E: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E0A0: /* move.w  (A1,D0.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E0A4: /* move.w  #$e10, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E0A8: /* sub.w   D1, D2 */
        value = D(1); step_subtract_word(&D(2), value); break;
    case 0xC2E0AA: /* move.w  D2, D4 */
        value = D(2); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2E0AC: /* subi.w  #$a8c, D2 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(2), value); break;
    case 0xC2E0B0: /* move.w  D2, D3 */
        value = D(2); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E0B2: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC2E0B4: /* lea     $c3e5e8.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC2E0BA: /* lea     $c3dd92.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC2E0C0: /* move.w  (A0,D3.w), D3 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E0C4: /* cmpi.w  #$8f, D3 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(3)); break;
    case 0xC2E0C8: /* bge     $c2e0dc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E0CA: /* tst.w   D7 */
        value = D(7); flags_logic_w(value); break;
    case 0xC2E0CC: /* bge     $c2e0d4 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E0CE: /* move.w  #$a82, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2E0D2: /* bra     $c2e0d8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E0D4: /* move.w  #$38e, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2E0D8: /* move.w  #$fee2, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E0DC: /* move.l  $c45bba.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC2E0E2: /* neg.l   D0 */
        renderer_negate(&D(0), 4); break;
    case 0xC2E0E4: /* cmpi.w  #$147, D3 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(3)); break;
    case 0xC2E0E8: /* bgt     $c2e0f0 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E0EA: /* asl.w   #6, D3 */
        renderer_asl_word(&D(3), 6); break;
    case 0xC2E0EC: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E0EE: /* bra     $c2e118 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E0F0: /* asr.l   #6, D0 */
        step_asr_long(&D(0), 6); break;
    case 0xC2E0F2: /* move.w  D3, D2 */
        value = D(3); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E0F4: /* bge     $c2e0f8 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E0F6: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E0F8: /* asr.w   #1, D2 */
        renderer_asr_word(&D(2), 1); break;
    case 0xC2E0FA: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E0FC: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E0FE: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E100: /* bge     $c2e104 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E102: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E104: /* cmp.w   D0, D2 */
        value = D(0); step_compare_word(value, D(2)); break;
    case 0xC2E106: /* bgt     $c2e116 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E108: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E10A: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E10C: /* bge     $c2e112 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E10E: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC2E110: /* bra     $c2e118 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E112: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC2E114: /* bra     $c2e118 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E116: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E118: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2E11A: /* blt     $c2e174 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E11C: /* cmpi.w  #$180, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2E120: /* ble     $c2e16e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E122: /* move.l  $c45bc2.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC2E128: /* cmpi.w  #$147, D3 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(3)); break;
    case 0xC2E12C: /* bgt     $c2e134 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E12E: /* asl.w   #6, D3 */
        renderer_asl_word(&D(3), 6); break;
    case 0xC2E130: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E132: /* bra     $c2e15c */
        step_branch(pc, opcode, 1); break;
    case 0xC2E134: /* asr.l   #6, D0 */
        step_asr_long(&D(0), 6); break;
    case 0xC2E136: /* move.w  D3, D2 */
        value = D(3); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E138: /* bge     $c2e13c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E13A: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E13C: /* asr.w   #1, D2 */
        renderer_asr_word(&D(2), 1); break;
    case 0xC2E13E: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E140: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E142: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E144: /* bge     $c2e148 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E146: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E148: /* cmp.w   D0, D2 */
        value = D(0); step_compare_word(value, D(2)); break;
    case 0xC2E14A: /* bgt     $c2e15a */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E14C: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E14E: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E150: /* bge     $c2e156 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E152: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC2E154: /* bra     $c2e15c */
        step_branch(pc, opcode, 1); break;
    case 0xC2E156: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC2E158: /* bra     $c2e15c */
        step_branch(pc, opcode, 1); break;
    case 0xC2E15A: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E15C: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2E15E: /* bge     $c2e162 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E160: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E162: /* move.w  (A1,D0.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E166: /* move.w  #$384, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E16A: /* sub.w   D1, D2 */
        value = D(1); step_subtract_word(&D(2), value); break;
    case 0xC2E16C: /* bra     $c2e1d4 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E16E: /* move.w  (A1,D0.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E172: /* bra     $c2e1d4 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E174: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E176: /* cmpi.w  #$180, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2E17A: /* ble     $c2e1ca */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E17C: /* move.l  $c45bc2.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC2E182: /* cmpi.w  #$147, D3 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(3)); break;
    case 0xC2E186: /* bgt     $c2e18e */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E188: /* asl.w   #6, D3 */
        renderer_asl_word(&D(3), 6); break;
    case 0xC2E18A: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E18C: /* bra     $c2e1b6 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E18E: /* asr.l   #6, D0 */
        step_asr_long(&D(0), 6); break;
    case 0xC2E190: /* move.w  D3, D2 */
        value = D(3); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E192: /* bge     $c2e196 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E194: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E196: /* asr.w   #1, D2 */
        renderer_asr_word(&D(2), 1); break;
    case 0xC2E198: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E19A: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E19C: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E19E: /* bge     $c2e1a2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E1A0: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E1A2: /* cmp.w   D0, D2 */
        value = D(0); step_compare_word(value, D(2)); break;
    case 0xC2E1A4: /* bgt     $c2e1b4 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E1A6: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E1A8: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E1AA: /* bge     $c2e1b0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E1AC: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC2E1AE: /* bra     $c2e1b6 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E1B0: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC2E1B2: /* bra     $c2e1b6 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E1B4: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E1B6: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2E1B8: /* bge     $c2e1bc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E1BA: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E1BC: /* move.w  (A1,D0.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E1C0: /* move.w  #$384, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E1C4: /* sub.w   D1, D2 */
        value = D(1); step_subtract_word(&D(2), value); break;
    case 0xC2E1C6: /* move.w  D2, D1 */
        value = D(2); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E1C8: /* bra     $c2e1ce */
        step_branch(pc, opcode, 1); break;
    case 0xC2E1CA: /* move.w  (A1,D0.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E1CE: /* move.w  #$e10, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E1D2: /* sub.w   D1, D2 */
        value = D(1); step_subtract_word(&D(2), value); break;
    case 0xC2E1D4: /* tst.l   $c45bba.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC2E1DA: /* blt     $c2e1f2 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E1DC: /* tst.l   $c45bc2.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC2E1E2: /* bge     $c2e202 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E1E4: /* move.w  D2, D1 */
        value = D(2); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E1E6: /* cmpi.w  #$708, D1 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(1)); break;
    case 0xC2E1EA: /* ble     $c2e1fc */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E1EC: /* move.w  #$1518, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E1F0: /* bra     $c2e200 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E1F2: /* tst.l   $c45bc2.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC2E1F8: /* bge     $c2e202 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E1FA: /* move.w  D2, D1 */
        value = D(2); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E1FC: /* move.w  #$708, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E200: /* sub.w   D1, D2 */
        value = D(1); step_subtract_word(&D(2), value); break;
    case 0xC2E202: /* move.w  D2, D5 */
        value = D(2); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2E204: /* bge     $c2e208 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E206: /* neg.w   D5 */
        renderer_negate(&D(5), 2); break;
    case 0xC2E208: /* move.l  $c45ba6.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC2E20E: /* neg.l   D0 */
        renderer_negate(&D(0), 4); break;
    case 0xC2E210: /* cmpi.w  #$147, D3 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(3)); break;
    case 0xC2E214: /* bgt     $c2e21c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E216: /* asl.w   #6, D3 */
        renderer_asl_word(&D(3), 6); break;
    case 0xC2E218: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E21A: /* bra     $c2e244 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E21C: /* asr.l   #6, D0 */
        step_asr_long(&D(0), 6); break;
    case 0xC2E21E: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E220: /* bge     $c2e224 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E222: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E224: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC2E226: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E228: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E22A: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E22C: /* bge     $c2e230 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E22E: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E230: /* cmp.w   D0, D6 */
        value = D(0); step_compare_word(value, D(6)); break;
    case 0xC2E232: /* bgt     $c2e242 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E234: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E236: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E238: /* bge     $c2e23e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E23A: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC2E23C: /* bra     $c2e244 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E23E: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC2E240: /* bra     $c2e244 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E242: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E244: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2E246: /* blt     $c2e2a0 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E248: /* cmpi.w  #$180, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2E24C: /* ble     $c2e29a */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E24E: /* move.l  $c45bb2.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC2E254: /* cmpi.w  #$147, D3 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(3)); break;
    case 0xC2E258: /* bgt     $c2e260 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E25A: /* asl.w   #6, D3 */
        renderer_asl_word(&D(3), 6); break;
    case 0xC2E25C: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E25E: /* bra     $c2e288 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E260: /* asr.l   #6, D0 */
        step_asr_long(&D(0), 6); break;
    case 0xC2E262: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E264: /* bge     $c2e268 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E266: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E268: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC2E26A: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E26C: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E26E: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E270: /* bge     $c2e274 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E272: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E274: /* cmp.w   D0, D6 */
        value = D(0); step_compare_word(value, D(6)); break;
    case 0xC2E276: /* bgt     $c2e286 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E278: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E27A: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E27C: /* bge     $c2e282 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E27E: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC2E280: /* bra     $c2e288 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E282: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC2E284: /* bra     $c2e288 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E286: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E288: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2E28A: /* bge     $c2e28e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E28C: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E28E: /* move.w  (A1,D0.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E292: /* move.w  #$384, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E296: /* sub.w   D1, D6 */
        value = D(1); step_subtract_word(&D(6), value); break;
    case 0xC2E298: /* bra     $c2e300 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E29A: /* move.w  (A1,D0.w), D6 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E29E: /* bra     $c2e300 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E2A0: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E2A2: /* cmpi.w  #$180, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2E2A6: /* ble     $c2e2f6 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E2A8: /* move.l  $c45bb2.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC2E2AE: /* cmpi.w  #$147, D3 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(3)); break;
    case 0xC2E2B2: /* bgt     $c2e2ba */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E2B4: /* asl.w   #6, D3 */
        renderer_asl_word(&D(3), 6); break;
    case 0xC2E2B6: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E2B8: /* bra     $c2e2e2 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E2BA: /* asr.l   #6, D0 */
        step_asr_long(&D(0), 6); break;
    case 0xC2E2BC: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E2BE: /* bge     $c2e2c2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E2C0: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E2C2: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC2E2C4: /* divs.w  D3, D0 */
        value = D(3); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2E2C6: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E2C8: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E2CA: /* bge     $c2e2ce */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E2CC: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E2CE: /* cmp.w   D0, D6 */
        value = D(0); step_compare_word(value, D(6)); break;
    case 0xC2E2D0: /* bgt     $c2e2e0 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2E2D2: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E2D4: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2E2D6: /* bge     $c2e2dc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E2D8: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC2E2DA: /* bra     $c2e2e2 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E2DC: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC2E2DE: /* bra     $c2e2e2 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E2E0: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2E2E2: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2E2E4: /* bge     $c2e2e8 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E2E6: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E2E8: /* move.w  (A1,D0.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E2EC: /* move.w  #$384, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E2F0: /* sub.w   D1, D6 */
        value = D(1); step_subtract_word(&D(6), value); break;
    case 0xC2E2F2: /* move.w  D6, D1 */
        value = D(6); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E2F4: /* bra     $c2e2fa */
        step_branch(pc, opcode, 1); break;
    case 0xC2E2F6: /* move.w  (A1,D0.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E2FA: /* move.w  #$e10, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E2FE: /* sub.w   D1, D6 */
        value = D(1); step_subtract_word(&D(6), value); break;
    case 0xC2E300: /* tst.l   $c45ba6.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC2E306: /* blt     $c2e31e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2E308: /* tst.l   $c45bb2.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC2E30E: /* bge     $c2e32e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E310: /* move.w  D6, D1 */
        value = D(6); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E312: /* cmpi.w  #$708, D1 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(1)); break;
    case 0xC2E316: /* ble     $c2e328 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2E318: /* move.w  #$1518, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E31C: /* bra     $c2e32c */
        step_branch(pc, opcode, 1); break;
    case 0xC2E31E: /* tst.l   $c45bb2.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC2E324: /* bge     $c2e32e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E326: /* move.w  D6, D1 */
        value = D(6); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E328: /* move.w  #$708, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E32C: /* sub.w   D1, D6 */
        value = D(1); step_subtract_word(&D(6), value); break;
    case 0xC2E32E: /* tst.w   D6 */
        value = D(6); flags_logic_w(value); break;
    case 0xC2E330: /* bge     $c2e334 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E332: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E334: /* ext.l   D4 */
        D(4) = (uint32_t)(int32_t)(int16_t)D(4); flags_logic_l(D(4)); break;
    case 0xC2E336: /* asl.l   #3, D4 */
        step_asl_long(&D(4), 3); break;
    case 0xC2E338: /* ext.l   D5 */
        D(5) = (uint32_t)(int32_t)(int16_t)D(5); flags_logic_l(D(5)); break;
    case 0xC2E33A: /* asl.l   #3, D5 */
        step_asl_long(&D(5), 3); break;
    case 0xC2E33C: /* ext.l   D6 */
        D(6) = (uint32_t)(int32_t)(int16_t)D(6); flags_logic_l(D(6)); break;
    case 0xC2E33E: /* asl.l   #3, D6 */
        step_asl_long(&D(6), 3); break;
    case 0xC2E340: /* movem.l (A7)+, D1/A0-A1 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC2E344: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2E346: /* asr.w   #3, D4 */
        renderer_asr_word(&D(4), 3); break;
    case 0xC2E348: /* bsr     $c2e6da */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E34C: /* move.w  D5, D6 */
        value = D(5); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E34E: /* asr.w   #6, D6 */
        renderer_asr_word(&D(6), 6); break;
    case 0xC2E350: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E352: /* clr.w   (A1)+ */
        address = A(1); A(1) += 2; step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC2E354: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E356: /* asr.w   #6, D6 */
        renderer_asr_word(&D(6), 6); break;
    case 0xC2E358: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E35A: /* clr.w   (A1)+ */
        address = A(1); A(1) += 2; step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC2E35C: /* move.w  #$100, (A1)+ */
        value = m68ki_read_imm_16(); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E360: /* clr.w   (A1)+ */
        address = A(1); A(1) += 2; step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC2E362: /* asr.w   #6, D4 */
        renderer_asr_word(&D(4), 6); break;
    case 0xC2E364: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2E366: /* move.w  D4, (A1)+ */
        value = D(4); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E368: /* clr.w   (A1)+ */
        address = A(1); A(1) += 2; step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC2E36A: /* asr.w   #6, D5 */
        renderer_asr_word(&D(5), 6); break;
    case 0xC2E36C: /* move.w  D5, (A1)+ */
        value = D(5); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E36E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2E370: /* asr.w   #3, D4 */
        renderer_asr_word(&D(4), 3); break;
    case 0xC2E372: /* bsr     $c2e6da */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E376: /* move.w  D5, (A1)+ */
        value = D(5); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E378: /* clr.w   (A1)+ */
        address = A(1); A(1) += 2; step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC2E37A: /* move.w  D4, (A1)+ */
        value = D(4); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E37C: /* clr.w   (A1)+ */
        address = A(1); A(1) += 2; step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC2E37E: /* move.w  #$4000, (A1)+ */
        value = m68ki_read_imm_16(); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E382: /* clr.w   (A1)+ */
        address = A(1); A(1) += 2; step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC2E384: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2E386: /* move.w  D4, (A1)+ */
        value = D(4); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E388: /* clr.w   (A1)+ */
        address = A(1); A(1) += 2; step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC2E38A: /* move.w  D5, (A1)+ */
        value = D(5); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E38C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2E38E: /* lsr.w   #3, D0 */
        SET_W(D(0), step_lsr_word_value((uint16_t)D(0), 3)); break;
    case 0xC2E390: /* lsr.w   #3, D2 */
        SET_W(D(2), step_lsr_word_value((uint16_t)D(2), 3)); break;
    case 0xC2E392: /* bsr     $c2e5f6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E396: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E398: /* asr.w   #6, D6 */
        renderer_asr_word(&D(6), 6); break;
    case 0xC2E39A: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E39C: /* clr.w   (A1)+ */
        address = A(1); A(1) += 2; step_write_word(address, 0); flags_logic_w(0); break;
    case 0xC2E39E: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E3A0: /* asr.w   #6, D6 */
        renderer_asr_word(&D(6), 6); break;
    case 0xC2E3A2: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E3A4: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E3A6: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E3A8: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2E3AA: /* asr.w   #4, D6 */
        renderer_asr_word(&D(6), 4); break;
    case 0xC2E3AC: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E3AE: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E3B0: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E3B2: /* asr.w   #6, D6 */
        renderer_asr_word(&D(6), 6); break;
    case 0xC2E3B4: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E3B6: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E3B8: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E3BA: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2E3BC: /* asr.w   #4, D6 */
        renderer_asr_word(&D(6), 4); break;
    case 0xC2E3BE: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E3C0: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E3C2: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E3C4: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2E3C6: /* asr.w   #4, D6 */
        renderer_asr_word(&D(6), 4); break;
    case 0xC2E3C8: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E3CA: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E3CC: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E3CE: /* asr.w   #6, D0 */
        renderer_asr_word(&D(0), 6); break;
    case 0xC2E3D0: /* move.w  D0, (A1)+ */
        value = D(0); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E3D2: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E3D4: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E3D6: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2E3D8: /* asr.w   #4, D6 */
        renderer_asr_word(&D(6), 4); break;
    case 0xC2E3DA: /* move.w  D6, (A1) */
        value = D(6); step_write_word(A(1), value); flags_logic_w(value); break;
    case 0xC2E3DC: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2E3DE: /* lsr.w   #3, D0 */
        SET_W(D(0), step_lsr_word_value((uint16_t)D(0), 3)); break;
    case 0xC2E3E0: /* lsr.w   #3, D2 */
        SET_W(D(2), step_lsr_word_value((uint16_t)D(2), 3)); break;
    case 0xC2E3E2: /* lsr.w   #3, D4 */
        SET_W(D(4), step_lsr_word_value((uint16_t)D(4), 3)); break;
    case 0xC2E3E4: /* bsr     $c2e5f6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E3E8: /* bsr     $c2e6da */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E3EC: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E3EE: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E3F0: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E3F2: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E3F4: /* muls.w  D4, D6 */
        value = D(4); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E3F6: /* move.w  D3, D7 */
        value = D(3); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E3F8: /* muls.w  D5, D7 */
        value = D(5); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E3FA: /* add.l   D6, D7 */
        value = D(6); step_add_long(&D(7), value); break;
    case 0xC2E3FC: /* swap    D7 */
        step_swap(&D(7)); break;
    case 0xC2E3FE: /* asr.w   #4, D7 */
        renderer_asr_word(&D(7), 4); break;
    case 0xC2E400: /* move.w  D7, (A1)+ */
        value = D(7); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E402: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E404: /* muls.w  D4, D6 */
        value = D(4); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E406: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2E408: /* asr.w   #4, D6 */
        renderer_asr_word(&D(6), 4); break;
    case 0xC2E40A: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E40C: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E40E: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E410: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E412: /* swap    D7 */
        step_swap(&D(7)); break;
    case 0xC2E414: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E416: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E418: /* muls.w  D4, D6 */
        value = D(4); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E41A: /* move.w  D2, D7 */
        value = D(2); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E41C: /* muls.w  D5, D7 */
        value = D(5); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E41E: /* sub.l   D6, D7 */
        value = D(6); step_subtract_long(&D(7), value); break;
    case 0xC2E420: /* swap    D7 */
        step_swap(&D(7)); break;
    case 0xC2E422: /* asr.w   #4, D7 */
        renderer_asr_word(&D(7), 4); break;
    case 0xC2E424: /* move.w  D7, (A1)+ */
        value = D(7); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E426: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E428: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E42A: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E42C: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E42E: /* muls.w  D5, D6 */
        value = D(5); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E430: /* move.w  D3, D7 */
        value = D(3); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E432: /* muls.w  D4, D7 */
        value = D(4); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E434: /* sub.l   D6, D7 */
        value = D(6); step_subtract_long(&D(7), value); break;
    case 0xC2E436: /* swap    D7 */
        step_swap(&D(7)); break;
    case 0xC2E438: /* asr.w   #4, D7 */
        renderer_asr_word(&D(7), 4); break;
    case 0xC2E43A: /* move.w  D7, (A1)+ */
        value = D(7); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E43C: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E43E: /* muls.w  D5, D6 */
        value = D(5); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E440: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2E442: /* asr.w   #4, D6 */
        renderer_asr_word(&D(6), 4); break;
    case 0xC2E444: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E446: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E448: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E44A: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E44C: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E44E: /* muls.w  D5, D6 */
        value = D(5); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E450: /* move.w  D2, D7 */
        value = D(2); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E452: /* muls.w  D4, D7 */
        value = D(4); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E454: /* add.l   D6, D7 */
        value = D(6); step_add_long(&D(7), value); break;
    case 0xC2E456: /* swap    D7 */
        step_swap(&D(7)); break;
    case 0xC2E458: /* asr.w   #4, D7 */
        renderer_asr_word(&D(7), 4); break;
    case 0xC2E45A: /* move.w  D7, (A1)+ */
        value = D(7); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E45C: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E45E: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E460: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2E462: /* asr.w   #4, D6 */
        renderer_asr_word(&D(6), 4); break;
    case 0xC2E464: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E466: /* move.w  D6, (A1)+ */
        value = D(6); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E468: /* asr.w   #6, D0 */
        renderer_asr_word(&D(0), 6); break;
    case 0xC2E46A: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E46C: /* move.w  D0, (A1)+ */
        value = D(0); address = A(1); A(1) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2E46E: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E470: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E472: /* swap    D6 */
        step_swap(&D(6)); break;
    case 0xC2E474: /* asr.w   #4, D6 */
        renderer_asr_word(&D(6), 4); break;
    case 0xC2E476: /* move.w  D6, (A1) */
        value = D(6); step_write_word(A(1), value); flags_logic_w(value); break;
    case 0xC2E478: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2E5AC: /* movem.w $c45a3e.l, D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC2E5B4: /* movem.w (A1), D3-D5 */
        mask = m68ki_read_imm_16(); renderer_load(A(1), mask, 2, -1); break;
    case 0xC2E5B8: /* muls.w  D0, D3 */
        value = D(0); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC2E5BA: /* muls.w  D0, D4 */
        value = D(0); renderer_multiply(&D(4), (uint16_t)value); break;
    case 0xC2E5BC: /* muls.w  D0, D5 */
        value = D(0); renderer_multiply(&D(5), (uint16_t)value); break;
    case 0xC2E5BE: /* asr.l   #8, D3 */
        step_asr_long(&D(3), 8); break;
    case 0xC2E5C0: /* asr.l   #8, D4 */
        step_asr_long(&D(4), 8); break;
    case 0xC2E5C2: /* asr.l   #8, D5 */
        step_asr_long(&D(5), 8); break;
    case 0xC2E5C4: /* movem.w D3-D5, (A1) */
        mask = m68ki_read_imm_16(); renderer_store(A(1), mask, 2, -1); break;
    case 0xC2E5C8: /* addq.w  #6, A1 */
        value = 6u; A(1) += value; break;
    case 0xC2E5CA: /* movem.w (A1), D3-D5 */
        mask = m68ki_read_imm_16(); renderer_load(A(1), mask, 2, -1); break;
    case 0xC2E5CE: /* muls.w  D1, D3 */
        value = D(1); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC2E5D0: /* muls.w  D1, D4 */
        value = D(1); renderer_multiply(&D(4), (uint16_t)value); break;
    case 0xC2E5D2: /* muls.w  D1, D5 */
        value = D(1); renderer_multiply(&D(5), (uint16_t)value); break;
    case 0xC2E5D4: /* asr.l   #8, D3 */
        step_asr_long(&D(3), 8); break;
    case 0xC2E5D6: /* asr.l   #8, D4 */
        step_asr_long(&D(4), 8); break;
    case 0xC2E5D8: /* asr.l   #8, D5 */
        step_asr_long(&D(5), 8); break;
    case 0xC2E5DA: /* movem.w D3-D5, (A1) */
        mask = m68ki_read_imm_16(); renderer_store(A(1), mask, 2, -1); break;
    case 0xC2E5DE: /* addq.w  #6, A1 */
        value = 6u; A(1) += value; break;
    case 0xC2E5E0: /* movem.w (A1), D3-D5 */
        mask = m68ki_read_imm_16(); renderer_load(A(1), mask, 2, -1); break;
    case 0xC2E5E4: /* muls.w  D2, D3 */
        value = D(2); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC2E5E6: /* muls.w  D2, D4 */
        value = D(2); renderer_multiply(&D(4), (uint16_t)value); break;
    case 0xC2E5E8: /* muls.w  D2, D5 */
        value = D(2); renderer_multiply(&D(5), (uint16_t)value); break;
    case 0xC2E5EA: /* asr.l   #8, D3 */
        step_asr_long(&D(3), 8); break;
    case 0xC2E5EC: /* asr.l   #8, D4 */
        step_asr_long(&D(4), 8); break;
    case 0xC2E5EE: /* asr.l   #8, D5 */
        step_asr_long(&D(5), 8); break;
    case 0xC2E5F0: /* movem.w D3-D5, (A1) */
        mask = m68ki_read_imm_16(); renderer_store(A(1), mask, 2, -1); break;
    case 0xC2E5F4: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C2D9BA_step(void) { return glue_C2D99C_step(); }

int glue_C2DB18_step(void) { return glue_C2D99C_step(); }

int glue_C2DEE0_step(void) { return glue_C2D99C_step(); }

int glue_C2DAF2_step(void) { return glue_C2D99C_step(); }

int glue_C2E370_step(void) { return glue_C2D99C_step(); }

int glue_C2E346_step(void) { return glue_C2D99C_step(); }

int glue_C2E38E_step(void) { return glue_C2D99C_step(); }

int glue_C2E3DE_step(void) { return glue_C2D99C_step(); }

int glue_C2E5AC_step(void) { return glue_C2D99C_step(); }

int glue_C2D970_step(void) { return glue_C2D99C_step(); }

int glue_C258C8_step(void) { return glue_C2D99C_step(); }
