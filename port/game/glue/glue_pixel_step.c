/* Pixel/square masks, palette dispatch and planar lane writes; plot.c and planar_lane_masks.c.
 * Source CPU effects and instruction/bus/event boundaries stay in glue. */
#include "glue_renderer_step_math.h"

int glue_C2F5C0_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC2F5C0: /* add.w   $c45988.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_add_word(&D(0), value); break;
    case 0xC2F5C6: /* blt     $c2f622 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F5C8: /* cmpi.w  #$140, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2F5CC: /* bge     $c2f622 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F5CE: /* add.w   $c458d8.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_add_word(&D(1), value); break;
    case 0xC2F5D4: /* movea.l $c456b6.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC2F5DA: /* lea     $c2f766.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC2F5E0: /* lea     $c2f786.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC2F5E6: /* move.w  D0, -(A7) */
        value = D(0); A(7) -= 2; step_write_word(A(7), value); flags_logic_w(value); break;
    case 0xC2F5E8: /* move.w  D1, -(A7) */
        value = D(1); A(7) -= 2; step_write_word(A(7), value); flags_logic_w(value); break;
    case 0xC2F5EA: /* bsr     $c2f688 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2F5EE: /* move.w  (A7)+, D1 */
        address = A(7); A(7) += 2; value = m68k_read_memory_16(address); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2F5F0: /* move.w  (A7)+, D0 */
        address = A(7); A(7) += 2; value = m68k_read_memory_16(address); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F5F2: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F5F4: /* movea.l $c456b6.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC2F5FA: /* lea     $c2f766.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC2F600: /* lea     $c2f786.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC2F606: /* bra     $c2f688 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F60A: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2F60C: /* andi.w  #$f, D2 */
        value = m68ki_read_imm_16(); result = D(2) & value; SET_W(D(2), result); flags_logic_w(result); break;
    case 0xC2F610: /* bne     $c2f626 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2F612: /* movem.w D0-D1, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 2, 7); break;
    case 0xC2F616: /* bsr     $c2f5f4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2F618: /* movem.w (A7)+, D0-D1 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 2, 7); break;
    case 0xC2F61C: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC2F61E: /* bsr     $c2f5f4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2F620: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F622: /* moveq   #-$1, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC2F624: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F626: /* movea.l $c456b6.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC2F62C: /* lea     $c2f7c6.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC2F632: /* lea     $c2f786.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC2F638: /* bra     $c2f688 */
        step_branch(pc, opcode, 1); break;
    case 0xC2F63A: /* add.w   $c45988.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_add_word(&D(0), value); break;
    case 0xC2F640: /* blt     $c2f622 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F642: /* cmpi.w  #$13f, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC2F646: /* bge     $c2f622 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F648: /* add.w   $c458d8.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_add_word(&D(1), value); break;
    case 0xC2F64E: /* move.w  D0, -(A7) */
        value = D(0); A(7) -= 2; step_write_word(A(7), value); flags_logic_w(value); break;
    case 0xC2F650: /* move.w  D1, -(A7) */
        value = D(1); A(7) -= 2; step_write_word(A(7), value); flags_logic_w(value); break;
    case 0xC2F652: /* movea.l $c456b6.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC2F658: /* lea     $c2f7c6.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC2F65E: /* lea     $c2f7e6.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC2F664: /* bsr     $c2f688 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2F668: /* move.w  (A7)+, D1 */
        address = A(7); A(7) += 2; value = m68k_read_memory_16(address); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2F66A: /* move.w  (A7)+, D0 */
        address = A(7); A(7) += 2; value = m68k_read_memory_16(address); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F66C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F66E: /* cmp.w   $c45984.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_compare_word(value, D(1)); break;
    case 0xC2F674: /* bge     $c2f60a */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2F676: /* movea.l $c456b6.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC2F67C: /* lea     $c2f7c6.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC2F682: /* lea     $c2f7e6.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC2F688: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC2F68A: /* ble     $c2f622 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2F68C: /* move.w  $c45954.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2F692: /* andi.w  #$f, D2 */
        value = m68ki_read_imm_16(); result = D(2) & value; SET_W(D(2), result); flags_logic_w(result); break;
    case 0xC2F696: /* add.w   D2, D2 */
        value = D(2); step_add_word(&D(2), value); break;
    case 0xC2F698: /* add.w   D2, D2 */
        value = D(2); step_add_word(&D(2), value); break;
    case 0xC2F69A: /* movea.l (A4,D2.w), A4 */
        value = m68k_read_memory_32(step_indexed(A(4))); A(4) = value; break;
    case 0xC2F69E: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2F6A0: /* andi.w  #$f, D0 */
        value = m68ki_read_imm_16(); result = D(0) & value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC2F6A4: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2F6A6: /* move.w  (A3,D0.w), D0 */
        value = m68k_read_memory_16(step_indexed(A(3))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F6AA: /* move.w  D0, D7 */
        value = D(0); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2F6AC: /* not.w   D0 */
        value = ~D(0); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F6AE: /* asl.w   #3, D1 */
        renderer_asl_word(&D(1), 3); break;
    case 0xC2F6B0: /* move.w  D1, D3 */
        value = D(1); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2F6B2: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC2F6B4: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC2F6B6: /* add.w   D3, D1 */
        value = D(3); step_add_word(&D(1), value); break;
    case 0xC2F6B8: /* andi.w  #$fff0, D2 */
        value = m68ki_read_imm_16(); result = D(2) & value; SET_W(D(2), result); flags_logic_w(result); break;
    case 0xC2F6BC: /* asr.w   #3, D2 */
        renderer_asr_word(&D(2), 3); break;
    case 0xC2F6BE: /* add.w   D1, D2 */
        value = D(1); step_add_word(&D(2), value); break;
    case 0xC2F6C0: /* movem.l (A1), A0-A3 */
        mask = m68ki_read_imm_16(); renderer_load(A(1), mask, 4, -1); break;
    case 0xC2F6C4: /* adda.w  D2, A0 */
        value = D(2); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2F6C6: /* adda.w  D2, A1 */
        value = D(2); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2F6C8: /* adda.w  D2, A2 */
        value = D(2); A(2) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2F6CA: /* adda.w  D2, A3 */
        value = D(2); A(3) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2F6CC: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2F6CE: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2F6D0: /* move.w  D0, D3 */
        value = D(0); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2F6D2: /* move.w  D7, D4 */
        value = D(7); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2F6D4: /* move.w  D7, D5 */
        value = D(7); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2F6D6: /* move.w  D7, D6 */
        value = D(7); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2F6D8: /* btst    #$0, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2F6E0: /* bne     $c2f6e8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2F6E2: /* move.w  #$ffff, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2F6E6: /* clr.w   D4 */
        SET_W(D(4), 0); flags_logic_w(0); break;
    case 0xC2F6E8: /* btst    #$1, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2F6F0: /* bne     $c2f6f8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2F6F2: /* move.w  #$ffff, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2F6F6: /* clr.w   D5 */
        SET_W(D(5), 0); flags_logic_w(0); break;
    case 0xC2F6F8: /* btst    #$2, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2F700: /* bne     $c2f708 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2F702: /* move.w  #$ffff, D2 */
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2F706: /* clr.w   D6 */
        SET_W(D(6), 0); flags_logic_w(0); break;
    case 0xC2F708: /* btst    #$3, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2F710: /* bne     $c2f718 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2F712: /* move.w  #$ffff, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2F716: /* clr.w   D7 */
        SET_W(D(7), 0); flags_logic_w(0); break;
    case 0xC2F718: /* tst.w   $c456e8.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2F71E: /* blt     $c2f764 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2F720: /* andi.l  #$ffff, D0 */
        value = m68ki_read_imm_32(); result = D(0) & value; D(0) = result; flags_logic_l(result); break;
    case 0xC2F726: /* btst    #$0, $c456eb.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2F72E: /* beq     $c2f734 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F730: /* eor.w   D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result ^= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F732: /* moveq   #-$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2F734: /* btst    #$1, $c456eb.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2F73C: /* beq     $c2f742 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F73E: /* eor.w   D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result ^= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F740: /* moveq   #-$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2F742: /* btst    #$2, $c456eb.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2F74A: /* beq     $c2f750 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F74C: /* eor.w   D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result ^= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F74E: /* moveq   #-$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2F750: /* btst    #$3, $c456eb.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2F758: /* beq     $c2f75e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2F75A: /* eor.w   D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result ^= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F75C: /* moveq   #-$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2F75E: /* tst.l   D0 */
        value = D(0); flags_logic_l(value); break;
    case 0xC2F760: /* bpl     $c2f764 */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC2F762: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F764: /* jmp     (A4) */
        REG_PC = A(4); break;
    case 0xC2F826: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F828: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F82A: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F82C: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F82E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F83A: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F83C: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F83E: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F840: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F842: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F844: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F846: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F848: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F84A: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F84C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F84E: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F850: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F852: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F854: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F856: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F858: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F85A: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F85C: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F85E: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F860: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F862: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F864: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F866: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F868: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F86A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F86C: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F86E: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F870: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F872: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F874: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F876: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F878: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F87A: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F87C: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F87E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F880: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F882: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F884: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F886: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F888: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F88A: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F88C: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F88E: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F890: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F892: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F894: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F896: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F898: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F89A: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F89C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F89E: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8A0: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8A2: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8A4: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8A6: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F8A8: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8AA: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8AC: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8AE: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8B0: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F8B2: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8B4: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8B6: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8B8: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8BA: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F8EA: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8EC: /* or.w    D4, ($28,A3) */
        value = D(4); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8F0: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8F2: /* and.w   D1, ($28,A2) */
        value = D(1); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8F6: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8F8: /* and.w   D2, ($28,A1) */
        value = D(2); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8FC: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F8FE: /* and.w   D3, ($28,A0) */
        value = D(3); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F902: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F904: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F906: /* and.w   D0, ($28,A3) */
        value = D(0); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F90A: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F90C: /* or.w    D5, ($28,A2) */
        value = D(5); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F910: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F912: /* and.w   D2, ($28,A1) */
        value = D(2); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F916: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F918: /* and.w   D3, ($28,A0) */
        value = D(3); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F91C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F91E: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F920: /* or.w    D4, ($28,A3) */
        value = D(4); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F924: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F926: /* or.w    D5, ($28,A2) */
        value = D(5); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F92A: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F92C: /* and.w   D2, ($28,A1) */
        value = D(2); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F930: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F932: /* and.w   D3, ($28,A0) */
        value = D(3); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F936: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F96C: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F96E: /* and.w   D0, ($28,A3) */
        value = D(0); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F972: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F974: /* or.w    D5, ($28,A2) */
        value = D(5); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F978: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F97A: /* or.w    D6, ($28,A1) */
        value = D(6); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F97E: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F980: /* and.w   D3, ($28,A0) */
        value = D(3); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F984: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F986: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F988: /* or.w    D4, ($28,A3) */
        value = D(4); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F98C: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F98E: /* or.w    D5, ($28,A2) */
        value = D(5); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F992: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F994: /* or.w    D6, ($28,A1) */
        value = D(6); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F998: /* and.w   D3, (A0) */
        value = D(3); address = A(0); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F99A: /* and.w   D3, ($28,A0) */
        value = D(3); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F99E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F9A0: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9A2: /* and.w   D0, ($28,A3) */
        value = D(0); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9A6: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9A8: /* and.w   D1, ($28,A2) */
        value = D(1); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9AC: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9AE: /* and.w   D2, ($28,A1) */
        value = D(2); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9B2: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9B4: /* or.w    D7, ($28,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9B8: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F9BA: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9BC: /* or.w    D4, ($28,A3) */
        value = D(4); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9C0: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9C2: /* and.w   D1, ($28,A2) */
        value = D(1); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9C6: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9C8: /* and.w   D2, ($28,A1) */
        value = D(2); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9CC: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9CE: /* or.w    D7, ($28,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9D2: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F9D4: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9D6: /* and.w   D0, ($28,A3) */
        value = D(0); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9DA: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9DC: /* or.w    D5, ($28,A2) */
        value = D(5); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9E0: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9E2: /* and.w   D2, ($28,A1) */
        value = D(2); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9E6: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9E8: /* or.w    D7, ($28,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9EC: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2F9EE: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9F0: /* or.w    D4, ($28,A3) */
        value = D(4); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9F4: /* or.w    D5, (A2) */
        value = D(5); address = A(2); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9F6: /* or.w    D5, ($28,A2) */
        value = D(5); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9FA: /* and.w   D2, (A1) */
        value = D(2); address = A(1); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2F9FC: /* and.w   D2, ($28,A1) */
        value = D(2); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA00: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA02: /* or.w    D7, ($28,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA06: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2FA08: /* and.w   D0, (A3) */
        value = D(0); address = A(3); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA0A: /* and.w   D0, ($28,A3) */
        value = D(0); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA0E: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA10: /* and.w   D1, ($28,A2) */
        value = D(1); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA14: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA16: /* or.w    D6, ($28,A1) */
        value = D(6); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA1A: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA1C: /* or.w    D7, ($28,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA20: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2FA22: /* or.w    D4, (A3) */
        value = D(4); address = A(3); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA24: /* or.w    D4, ($28,A3) */
        value = D(4); address = step_displacement(A(3)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA28: /* and.w   D1, (A2) */
        value = D(1); address = A(2); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA2A: /* and.w   D1, ($28,A2) */
        value = D(1); address = step_displacement(A(2)); result = m68k_read_memory_16(address); result &= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA2E: /* or.w    D6, (A1) */
        value = D(6); address = A(1); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA30: /* or.w    D6, ($28,A1) */
        value = D(6); address = step_displacement(A(1)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA34: /* or.w    D7, (A0) */
        value = D(7); address = A(0); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA36: /* or.w    D7, ($28,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_16(address); result |= value; flags_logic_w(result); step_write_word(address, result); break;
    case 0xC2FA3A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C2F5D4_step(void) { return glue_C2F5C0_step(); }

int glue_C2F5F4_step(void) { return glue_C2F5C0_step(); }

int glue_C2F60A_step(void) { return glue_C2F5C0_step(); }

int glue_C2F63A_step(void) { return glue_C2F5C0_step(); }

int glue_C2F64E_step(void) { return glue_C2F5C0_step(); }

int glue_C2F66E_step(void) { return glue_C2F5C0_step(); }

int glue_C2F826_step(void) { return glue_C2F5C0_step(); }

int glue_C2F83A_step(void) { return glue_C2F5C0_step(); }

int glue_C2F844_step(void) { return glue_C2F5C0_step(); }

int glue_C2F84E_step(void) { return glue_C2F5C0_step(); }

int glue_C2F858_step(void) { return glue_C2F5C0_step(); }

int glue_C2F862_step(void) { return glue_C2F5C0_step(); }

int glue_C2F86C_step(void) { return glue_C2F5C0_step(); }

int glue_C2F876_step(void) { return glue_C2F5C0_step(); }

int glue_C2F880_step(void) { return glue_C2F5C0_step(); }

int glue_C2F88A_step(void) { return glue_C2F5C0_step(); }

int glue_C2F894_step(void) { return glue_C2F5C0_step(); }

int glue_C2F89E_step(void) { return glue_C2F5C0_step(); }

int glue_C2F8A8_step(void) { return glue_C2F5C0_step(); }

int glue_C2F8B2_step(void) { return glue_C2F5C0_step(); }

int glue_C2F8EA_step(void) { return glue_C2F5C0_step(); }

int glue_C2F904_step(void) { return glue_C2F5C0_step(); }

int glue_C2F91E_step(void) { return glue_C2F5C0_step(); }

int glue_C2F96C_step(void) { return glue_C2F5C0_step(); }

int glue_C2F986_step(void) { return glue_C2F5C0_step(); }

int glue_C2F9A0_step(void) { return glue_C2F5C0_step(); }

int glue_C2F9BA_step(void) { return glue_C2F5C0_step(); }

int glue_C2F9D4_step(void) { return glue_C2F5C0_step(); }

int glue_C2F9EE_step(void) { return glue_C2F5C0_step(); }

int glue_C2FA08_step(void) { return glue_C2F5C0_step(); }

int glue_C2FA22_step(void) { return glue_C2F5C0_step(); }
