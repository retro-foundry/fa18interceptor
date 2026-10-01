/* Source timing for $C2469E/$C246A0 and their shared clipping children.
 * Readable domain behavior remains in polygon_clip.c. Every case performs
 * one source instruction, retaining its original bus and event boundary. */
#include "glue_renderer_step_math.h"

int glue_C246A0_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode, mask;
    if (pc < 0xC24688u || pc >= 0xC24DA8u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC24688: /* bra     $c24d8c */
        step_branch(pc, opcode, 1); break;
    case 0xC2468C: /* move.w  #$1f, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC24694: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC2469A: /* bra     $c24da0 */
        step_branch(pc, opcode, 1); break;
    case 0xC2469E: /* nop */
        break;
    case 0xC246A0: /* link    A6, #-$6 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC246A4: /* clr.w   D7 */
        SET_W(D(7), 0); flags_logic_w(0); break;
    case 0xC246A6: /* lea     $c4bf90.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC246AC: /* move.w  (A0)+, (-$2,A6) */
        value = m68k_read_memory_16(A(0)); A(0) += 2; m68k_write_memory_16(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC246B0: /* move.w  (A0)+, D0 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC246B2: /* cmpi.w  #$3, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC246B6: /* blt     $c2468c */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC246B8: /* move.w  D0, (-$4,A6) */
        value = D(0); m68k_write_memory_16(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC246BC: /* lea     $c4b990.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC246C2: /* lea     $c4e91a.l, A2 */
        A(2) = m68ki_read_imm_32(); break;
    case 0xC246C8: /* lea     $c4e910.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC246CE: /* lea     $c4e874.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC246D4: /* clr.l   (A4) */
        m68k_write_memory_32(A(4), 0); flags_logic_l(0); break;
    case 0xC246D6: /* clr.l   ($4,A4) */
        m68k_write_memory_32(step_displacement(A(4)), 0); flags_logic_l(0); break;
    case 0xC246DA: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC246DC: /* beq     $c24688 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC246DE: /* subq.w  #1, (-$4,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_16(address); step_subtract_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC246E2: /* blt     $c24a94 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC246E6: /* movem.w (A0)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(0); renderer_load(address, mask, 2, 0); break;
    case 0xC246EA: /* move.w  (-$2,A6), D3 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC246EE: /* asl.l   D3, D0 */
        step_asl_long(&D(0), D(3) & 63u); break;
    case 0xC246F0: /* asl.l   D3, D1 */
        step_asl_long(&D(1), D(3) & 63u); break;
    case 0xC246F2: /* asl.l   D3, D2 */
        step_asl_long(&D(2), D(3) & 63u); break;
    case 0xC246F4: /* tst.b   (A4) */
        value = m68k_read_memory_8(A(4)); flags_logic_b(value); break;
    case 0xC246F6: /* bne     $c24704 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC246F8: /* movem.w D0-D2, ($6,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC246FE: /* addq.b  #1, (A4) */
        value = 1u; address = A(4); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC24700: /* bra     $c24792 */
        step_branch(pc, opcode, 1); break;
    case 0xC24704: /* movem.w (A2), D3-D5 */
        mask = m68ki_read_imm_16(); address = A(2); renderer_load(address, mask, 2, -1); break;
    case 0xC24708: /* cmp.w   D2, D1 */
        value = D(2); result = D(1); step_compare_word(value, result); break;
    case 0xC2470A: /* bgt     $c24714 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2470C: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC2470E: /* ble     $c24792 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24712: /* bra     $c2471a */
        step_branch(pc, opcode, 1); break;
    case 0xC24714: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC24716: /* bgt     $c24792 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2471A: /* movem.w D0-D2, -(A7) */
        mask = m68ki_read_imm_16(); address = A(7); renderer_store(address, mask, 2, 7); break;
    case 0xC2471E: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC24720: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC24722: /* add.w   D5, D2 */
        value = D(5); step_add_word(&D(2), value); break;
    case 0xC24724: /* add.w   D1, D2 */
        value = D(1); step_add_word(&D(2), value); break;
    case 0xC24726: /* beq     $c24790 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24728: /* sub.w   D4, D5 */
        value = D(4); step_subtract_word(&D(5), value); break;
    case 0xC2472A: /* muls.w  D5, D1 */
        value = D(5); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2472C: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2472E: /* bge     $c24732 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24730: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC24732: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC24734: /* divs.w  D2, D1 */
        value = D(2); renderer_divide(&D(1), (int16_t)value); break;
    case 0xC24736: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24738: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC2473A: /* bge     $c2473e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2473C: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2473E: /* cmp.w   D1, D6 */
        value = D(1); result = D(6); step_compare_word(value, result); break;
    case 0xC24740: /* bgt     $c24750 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24742: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24744: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24746: /* bge     $c2474c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24748: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC2474A: /* bra     $c24752 */
        step_branch(pc, opcode, 1); break;
    case 0xC2474C: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC2474E: /* bra     $c24752 */
        step_branch(pc, opcode, 1); break;
    case 0xC24750: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24752: /* add.w   D4, D1 */
        value = D(4); step_add_word(&D(1), value); break;
    case 0xC24754: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC24756: /* muls.w  D5, D0 */
        value = D(5); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC24758: /* divs.w  D2, D0 */
        value = D(2); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2475A: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2475C: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2475E: /* bge     $c24762 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24760: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24762: /* cmp.w   D0, D6 */
        value = D(0); result = D(6); step_compare_word(value, result); break;
    case 0xC24764: /* bgt     $c24774 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24766: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24768: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2476A: /* bge     $c24770 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2476C: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC2476E: /* bra     $c24776 */
        step_branch(pc, opcode, 1); break;
    case 0xC24770: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC24772: /* bra     $c24776 */
        step_branch(pc, opcode, 1); break;
    case 0xC24774: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24776: /* add.w   D3, D0 */
        value = D(3); step_add_word(&D(0), value); break;
    case 0xC24778: /* move.w  D1, D2 */
        value = D(1); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2477A: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC2477E: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC24782: /* movem.w D0-D2, (A2) */
        mask = m68ki_read_imm_16(); address = A(2); renderer_store(address, mask, 2, -1); break;
    case 0xC24786: /* bsr     $c247c0 */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2478A: /* addq.b  #1, ($4,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC2478E: /* bra     $c24796 */
        step_branch(pc, opcode, 1); break;
    case 0xC24790: /* bra     $c247ae */
        step_branch(pc, opcode, 1); break;
    case 0xC24792: /* movem.w D0-D2, (A2) */
        mask = m68ki_read_imm_16(); address = A(2); renderer_store(address, mask, 2, -1); break;
    case 0xC24796: /* cmp.w   D2, D1 */
        value = D(2); result = D(1); step_compare_word(value, result); break;
    case 0xC24798: /* bgt     $c247a8 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2479A: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC2479E: /* bsr     $c247c0 */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC247A2: /* beq     $c247ae */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC247A4: /* addq.b  #1, ($4,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC247A8: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC247AA: /* bra     $c246dc */
        step_branch(pc, opcode, 1); break;
    case 0xC247AE: /* move.w  #$1, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC247B6: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC247BC: /* bra     $c246dc */
        step_branch(pc, opcode, 1); break;
    case 0xC247C0: /* movem.w D0-D2, -(A7) */
        mask = m68ki_read_imm_16(); address = A(7); renderer_store(address, mask, 2, 7); break;
    case 0xC247C4: /* movem.w (A3), D0-D2 */
        mask = m68ki_read_imm_16(); address = A(3); renderer_load(address, mask, 2, -1); break;
    case 0xC247C8: /* tst.b   ($1,A4) */
        value = m68k_read_memory_8(step_displacement(A(4))); flags_logic_b(value); break;
    case 0xC247CC: /* bne     $c247dc */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC247CE: /* movem.w D0-D2, ($16,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC247D4: /* addq.b  #1, ($1,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC247D8: /* bra     $c2487a */
        step_branch(pc, opcode, 1); break;
    case 0xC247DC: /* movem.w ($10,A2), D3-D5 */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_load(address, mask, 2, -1); break;
    case 0xC247E2: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC247E4: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC247E6: /* cmp.w   D2, D1 */
        value = D(2); result = D(1); step_compare_word(value, result); break;
    case 0xC247E8: /* bgt     $c247f2 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC247EA: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC247EC: /* ble     $c24878 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC247F0: /* bra     $c247f8 */
        step_branch(pc, opcode, 1); break;
    case 0xC247F2: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC247F4: /* bgt     $c24878 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC247F8: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC247FA: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC247FC: /* movem.w D0-D2, -(A7) */
        mask = m68ki_read_imm_16(); address = A(7); renderer_store(address, mask, 2, 7); break;
    case 0xC24800: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC24802: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC24804: /* add.w   D5, D2 */
        value = D(5); step_add_word(&D(2), value); break;
    case 0xC24806: /* sub.w   D1, D2 */
        value = D(1); step_subtract_word(&D(2), value); break;
    case 0xC24808: /* beq     $c24876 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2480A: /* add.w   D4, D5 */
        value = D(4); step_add_word(&D(5), value); break;
    case 0xC2480C: /* muls.w  D5, D1 */
        value = D(5); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC2480E: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC24810: /* bge     $c24814 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24812: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC24814: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC24816: /* divs.w  D2, D1 */
        value = D(2); renderer_divide(&D(1), (int16_t)value); break;
    case 0xC24818: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC2481A: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC2481C: /* bge     $c24820 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2481E: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24820: /* cmp.w   D1, D6 */
        value = D(1); result = D(6); step_compare_word(value, result); break;
    case 0xC24822: /* bgt     $c24832 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24824: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24826: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24828: /* bge     $c2482e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2482A: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC2482C: /* bra     $c24834 */
        step_branch(pc, opcode, 1); break;
    case 0xC2482E: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC24830: /* bra     $c24834 */
        step_branch(pc, opcode, 1); break;
    case 0xC24832: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24834: /* add.w   D4, D1 */
        value = D(4); step_add_word(&D(1), value); break;
    case 0xC24836: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC24838: /* muls.w  D5, D0 */
        value = D(5); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC2483A: /* divs.w  D2, D0 */
        value = D(2); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2483C: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2483E: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24840: /* bge     $c24844 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24842: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24844: /* cmp.w   D0, D6 */
        value = D(0); result = D(6); step_compare_word(value, result); break;
    case 0xC24846: /* bgt     $c24856 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24848: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2484A: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2484C: /* bge     $c24852 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2484E: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC24850: /* bra     $c24858 */
        step_branch(pc, opcode, 1); break;
    case 0xC24852: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC24854: /* bra     $c24858 */
        step_branch(pc, opcode, 1); break;
    case 0xC24856: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24858: /* add.w   D3, D0 */
        value = D(3); step_add_word(&D(0), value); break;
    case 0xC2485A: /* move.w  D1, D2 */
        value = D(1); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2485C: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2485E: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC24862: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC24866: /* movem.w D0-D2, ($10,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC2486C: /* bsr     $c248b2 */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC24870: /* addq.b  #1, ($5,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC24874: /* bra     $c24880 */
        step_branch(pc, opcode, 1); break;
    case 0xC24876: /* bra     $c2489e */
        step_branch(pc, opcode, 1); break;
    case 0xC24878: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2487A: /* movem.w D0-D2, ($10,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC24880: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24882: /* cmp.w   D2, D1 */
        value = D(2); result = D(1); step_compare_word(value, result); break;
    case 0xC24884: /* bgt     $c24896 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24886: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24888: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC2488C: /* bsr     $c248b2 */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC24890: /* beq     $c2489e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24892: /* addq.b  #1, ($5,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC24896: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24898: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC2489C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2489E: /* move.w  #$3, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC248A6: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC248AC: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC248B0: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC248B2: /* movem.w D0-D2, -(A7) */
        mask = m68ki_read_imm_16(); address = A(7); renderer_store(address, mask, 2, 7); break;
    case 0xC248B6: /* movem.w (A3), D0-D2 */
        mask = m68ki_read_imm_16(); address = A(3); renderer_load(address, mask, 2, -1); break;
    case 0xC248BA: /* tst.b   ($2,A4) */
        value = m68k_read_memory_8(step_displacement(A(4))); flags_logic_b(value); break;
    case 0xC248BE: /* bne     $c248ce */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC248C0: /* movem.w D0-D2, ($26,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC248C6: /* addq.b  #1, ($2,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC248CA: /* bra     $c24962 */
        step_branch(pc, opcode, 1); break;
    case 0xC248CE: /* movem.w ($20,A2), D3-D5 */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_load(address, mask, 2, -1); break;
    case 0xC248D4: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC248D6: /* cmp.w   D2, D0 */
        value = D(2); result = D(0); step_compare_word(value, result); break;
    case 0xC248D8: /* bgt     $c248e2 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC248DA: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC248DC: /* ble     $c24962 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC248E0: /* bra     $c248e8 */
        step_branch(pc, opcode, 1); break;
    case 0xC248E2: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC248E4: /* bgt     $c24962 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC248E8: /* movem.w D0-D2, -(A7) */
        mask = m68ki_read_imm_16(); address = A(7); renderer_store(address, mask, 2, 7); break;
    case 0xC248EC: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC248EE: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC248F0: /* add.w   D5, D2 */
        value = D(5); step_add_word(&D(2), value); break;
    case 0xC248F2: /* add.w   D0, D2 */
        value = D(0); step_add_word(&D(2), value); break;
    case 0xC248F4: /* beq     $c24960 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC248F6: /* sub.w   D3, D5 */
        value = D(3); step_subtract_word(&D(5), value); break;
    case 0xC248F8: /* muls.w  D5, D0 */
        value = D(5); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC248FA: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC248FC: /* bge     $c24900 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC248FE: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC24900: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC24902: /* divs.w  D2, D0 */
        value = D(2); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC24904: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24906: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24908: /* bge     $c2490c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2490A: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2490C: /* cmp.w   D0, D6 */
        value = D(0); result = D(6); step_compare_word(value, result); break;
    case 0xC2490E: /* bgt     $c2491e */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24910: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24912: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24914: /* bge     $c2491a */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24916: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC24918: /* bra     $c24920 */
        step_branch(pc, opcode, 1); break;
    case 0xC2491A: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC2491C: /* bra     $c24920 */
        step_branch(pc, opcode, 1); break;
    case 0xC2491E: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24920: /* add.w   D3, D0 */
        value = D(3); step_add_word(&D(0), value); break;
    case 0xC24922: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC24924: /* muls.w  D5, D1 */
        value = D(5); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC24926: /* divs.w  D2, D1 */
        value = D(2); renderer_divide(&D(1), (int16_t)value); break;
    case 0xC24928: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC2492A: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC2492C: /* bge     $c24930 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2492E: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24930: /* cmp.w   D1, D6 */
        value = D(1); result = D(6); step_compare_word(value, result); break;
    case 0xC24932: /* bgt     $c24942 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24934: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24936: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24938: /* bge     $c2493e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2493A: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC2493C: /* bra     $c24944 */
        step_branch(pc, opcode, 1); break;
    case 0xC2493E: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC24940: /* bra     $c24944 */
        step_branch(pc, opcode, 1); break;
    case 0xC24942: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24944: /* add.w   D4, D1 */
        value = D(4); step_add_word(&D(1), value); break;
    case 0xC24946: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC24948: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC2494C: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC24950: /* movem.w D0-D2, ($20,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC24956: /* bsr     $c24996 */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2495A: /* addq.b  #1, ($6,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC2495E: /* bra     $c24968 */
        step_branch(pc, opcode, 1); break;
    case 0xC24960: /* bra     $c24982 */
        step_branch(pc, opcode, 1); break;
    case 0xC24962: /* movem.w D0-D2, ($20,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC24968: /* cmp.w   D2, D0 */
        value = D(2); result = D(0); step_compare_word(value, result); break;
    case 0xC2496A: /* bgt     $c2497a */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2496C: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC24970: /* bsr     $c24996 */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC24974: /* beq     $c24982 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24976: /* addq.b  #1, ($6,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC2497A: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2497C: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC24980: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24982: /* move.w  #$4, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2498A: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24990: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC24994: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24996: /* movem.w D0-D2, -(A7) */
        mask = m68ki_read_imm_16(); address = A(7); renderer_store(address, mask, 2, 7); break;
    case 0xC2499A: /* movem.w (A3), D0-D2 */
        mask = m68ki_read_imm_16(); address = A(3); renderer_load(address, mask, 2, -1); break;
    case 0xC2499E: /* tst.b   ($3,A4) */
        value = m68k_read_memory_8(step_displacement(A(4))); flags_logic_b(value); break;
    case 0xC249A2: /* bne     $c249b2 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC249A4: /* movem.w D0-D2, ($36,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC249AA: /* addq.b  #1, ($3,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC249AE: /* bra     $c24a5e */
        step_branch(pc, opcode, 1); break;
    case 0xC249B2: /* movem.w ($30,A2), D3-D5 */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_load(address, mask, 2, -1); break;
    case 0xC249B8: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC249BA: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC249BC: /* cmp.w   D2, D0 */
        value = D(2); result = D(0); step_compare_word(value, result); break;
    case 0xC249BE: /* bgt     $c249cc */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC249C0: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC249C2: /* ble     $c24a5c */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC249C6: /* bra     $c249d2 */
        step_branch(pc, opcode, 1); break;
    case 0xC249C8: /* bra     $c24a80 */
        step_branch(pc, opcode, 1); break;
    case 0xC249CC: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC249CE: /* bgt     $c24a5c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC249D2: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC249D4: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC249D6: /* movem.w D0-D2, -(A7) */
        mask = m68ki_read_imm_16(); address = A(7); renderer_store(address, mask, 2, 7); break;
    case 0xC249DA: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC249DC: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC249DE: /* add.w   D5, D2 */
        value = D(5); step_add_word(&D(2), value); break;
    case 0xC249E0: /* sub.w   D0, D2 */
        value = D(0); step_subtract_word(&D(2), value); break;
    case 0xC249E2: /* beq     $c249c8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC249E4: /* add.w   D3, D5 */
        value = D(3); step_add_word(&D(5), value); break;
    case 0xC249E6: /* muls.w  D5, D0 */
        value = D(5); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC249E8: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC249EA: /* bge     $c249ee */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC249EC: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC249EE: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC249F0: /* divs.w  D2, D0 */
        value = D(2); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC249F2: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC249F4: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC249F6: /* bge     $c249fa */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC249F8: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC249FA: /* cmp.w   D0, D6 */
        value = D(0); result = D(6); step_compare_word(value, result); break;
    case 0xC249FC: /* bgt     $c24a0c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC249FE: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24A00: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24A02: /* bge     $c24a08 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24A04: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC24A06: /* bra     $c24a0e */
        step_branch(pc, opcode, 1); break;
    case 0xC24A08: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC24A0A: /* bra     $c24a0e */
        step_branch(pc, opcode, 1); break;
    case 0xC24A0C: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24A0E: /* add.w   D3, D0 */
        value = D(3); step_add_word(&D(0), value); break;
    case 0xC24A10: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC24A12: /* muls.w  D5, D1 */
        value = D(5); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC24A14: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC24A16: /* bge     $c24a1a */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24A18: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC24A1A: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC24A1C: /* divs.w  D2, D1 */
        value = D(2); renderer_divide(&D(1), (int16_t)value); break;
    case 0xC24A1E: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24A20: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24A22: /* bge     $c24a26 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24A24: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24A26: /* cmp.w   D1, D6 */
        value = D(1); result = D(6); step_compare_word(value, result); break;
    case 0xC24A28: /* bgt     $c24a38 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24A2A: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24A2C: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24A2E: /* bge     $c24a34 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24A30: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC24A32: /* bra     $c24a3a */
        step_branch(pc, opcode, 1); break;
    case 0xC24A34: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC24A36: /* bra     $c24a3a */
        step_branch(pc, opcode, 1); break;
    case 0xC24A38: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24A3A: /* add.w   D4, D1 */
        value = D(4); step_add_word(&D(1), value); break;
    case 0xC24A3C: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC24A3E: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC24A40: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC24A44: /* move.l  (A3), (A1)+ */
        value = m68k_read_memory_32(A(3)); m68k_write_memory_32(A(1), value); A(1) += 4; flags_logic_l(value); break;
    case 0xC24A46: /* move.w  ($4,A3), (A1)+ */
        value = m68k_read_memory_16(step_displacement(A(3))); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC24A4A: /* addq.w  #1, D7 */
        value = 1u; step_add_word(&D(7), value); break;
    case 0xC24A4C: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC24A50: /* movem.w D0-D2, ($30,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC24A56: /* addq.b  #1, ($7,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC24A5A: /* bra     $c24a64 */
        step_branch(pc, opcode, 1); break;
    case 0xC24A5C: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24A5E: /* movem.w D0-D2, ($30,A2) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_store(address, mask, 2, -1); break;
    case 0xC24A64: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24A66: /* cmp.w   D2, D0 */
        value = D(2); result = D(0); step_compare_word(value, result); break;
    case 0xC24A68: /* bgt     $c24a78 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24A6A: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24A6C: /* move.w  D0, (A1)+ */
        value = D(0); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC24A6E: /* move.w  D1, (A1)+ */
        value = D(1); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC24A70: /* move.w  D2, (A1)+ */
        value = D(2); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC24A72: /* addq.w  #1, D7 */
        value = 1u; step_add_word(&D(7), value); break;
    case 0xC24A74: /* addq.b  #1, ($7,A4) */
        value = 1u; address = step_displacement(A(4)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC24A78: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24A7A: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC24A7E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24A80: /* move.w  #$5, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC24A88: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24A8E: /* movem.w (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); address = A(7); renderer_load(address, mask, 2, 7); break;
    case 0xC24A92: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24A94: /* tst.b   ($4,A4) */
        value = m68k_read_memory_8(step_displacement(A(4))); flags_logic_b(value); break;
    case 0xC24A98: /* beq     $c24b28 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24A9C: /* movem.w (A2), D0-D5 */
        mask = m68ki_read_imm_16(); address = A(2); renderer_load(address, mask, 2, -1); break;
    case 0xC24AA0: /* cmp.w   D2, D1 */
        value = D(2); result = D(1); step_compare_word(value, result); break;
    case 0xC24AA2: /* bgt     $c24abe */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24AA4: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC24AA6: /* ble     $c24b28 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24AAA: /* bra     $c24ac4 */
        step_branch(pc, opcode, 1); break;
    case 0xC24AAC: /* move.w  #$6, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC24AB4: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24ABA: /* bra     $c24d8c */
        step_branch(pc, opcode, 1); break;
    case 0xC24ABE: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC24AC0: /* bgt     $c24b28 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24AC4: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC24AC6: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC24AC8: /* add.w   D5, D2 */
        value = D(5); step_add_word(&D(2), value); break;
    case 0xC24ACA: /* add.w   D1, D2 */
        value = D(1); step_add_word(&D(2), value); break;
    case 0xC24ACC: /* beq     $c24aac */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24ACE: /* sub.w   D4, D5 */
        value = D(4); step_subtract_word(&D(5), value); break;
    case 0xC24AD0: /* muls.w  D5, D1 */
        value = D(5); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC24AD2: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC24AD4: /* bge     $c24ad8 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24AD6: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC24AD8: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC24ADA: /* divs.w  D2, D1 */
        value = D(2); renderer_divide(&D(1), (int16_t)value); break;
    case 0xC24ADC: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24ADE: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24AE0: /* bge     $c24ae4 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24AE2: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24AE4: /* cmp.w   D1, D6 */
        value = D(1); result = D(6); step_compare_word(value, result); break;
    case 0xC24AE6: /* bgt     $c24af6 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24AE8: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24AEA: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24AEC: /* bge     $c24af2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24AEE: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC24AF0: /* bra     $c24af8 */
        step_branch(pc, opcode, 1); break;
    case 0xC24AF2: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC24AF4: /* bra     $c24af8 */
        step_branch(pc, opcode, 1); break;
    case 0xC24AF6: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24AF8: /* add.w   D4, D1 */
        value = D(4); step_add_word(&D(1), value); break;
    case 0xC24AFA: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC24AFC: /* muls.w  D5, D0 */
        value = D(5); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC24AFE: /* divs.w  D2, D0 */
        value = D(2); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC24B00: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24B02: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24B04: /* bge     $c24b08 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24B06: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24B08: /* cmp.w   D0, D6 */
        value = D(0); result = D(6); step_compare_word(value, result); break;
    case 0xC24B0A: /* bgt     $c24b1a */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24B0C: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24B0E: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24B10: /* bge     $c24b16 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24B12: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC24B14: /* bra     $c24b1c */
        step_branch(pc, opcode, 1); break;
    case 0xC24B16: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC24B18: /* bra     $c24b1c */
        step_branch(pc, opcode, 1); break;
    case 0xC24B1A: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24B1C: /* add.w   D3, D0 */
        value = D(3); step_add_word(&D(0), value); break;
    case 0xC24B1E: /* move.w  D1, D2 */
        value = D(1); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC24B20: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC24B24: /* bsr     $c247c0 */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC24B28: /* tst.b   ($5,A4) */
        value = m68k_read_memory_8(step_displacement(A(4))); flags_logic_b(value); break;
    case 0xC24B2C: /* beq     $c24bc8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24B30: /* movem.w ($10,A2), D0-D5 */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_load(address, mask, 2, -1); break;
    case 0xC24B36: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24B38: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC24B3A: /* cmp.w   D2, D1 */
        value = D(2); result = D(1); step_compare_word(value, result); break;
    case 0xC24B3C: /* bgt     $c24b58 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24B3E: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC24B40: /* ble     $c24bc8 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24B44: /* bra     $c24b5e */
        step_branch(pc, opcode, 1); break;
    case 0xC24B46: /* move.w  #$7, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC24B4E: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24B54: /* bra     $c24d8c */
        step_branch(pc, opcode, 1); break;
    case 0xC24B58: /* cmp.w   D5, D4 */
        value = D(5); result = D(4); step_compare_word(value, result); break;
    case 0xC24B5A: /* bgt     $c24bc8 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24B5E: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24B60: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC24B62: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC24B64: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC24B66: /* add.w   D5, D2 */
        value = D(5); step_add_word(&D(2), value); break;
    case 0xC24B68: /* sub.w   D1, D2 */
        value = D(1); step_subtract_word(&D(2), value); break;
    case 0xC24B6A: /* beq     $c24b46 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24B6C: /* add.w   D4, D5 */
        value = D(4); step_add_word(&D(5), value); break;
    case 0xC24B6E: /* muls.w  D5, D1 */
        value = D(5); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC24B70: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC24B72: /* bge     $c24b76 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24B74: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC24B76: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC24B78: /* divs.w  D2, D1 */
        value = D(2); renderer_divide(&D(1), (int16_t)value); break;
    case 0xC24B7A: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24B7C: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24B7E: /* bge     $c24b82 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24B80: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24B82: /* cmp.w   D1, D6 */
        value = D(1); result = D(6); step_compare_word(value, result); break;
    case 0xC24B84: /* bgt     $c24b94 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24B86: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24B88: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24B8A: /* bge     $c24b90 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24B8C: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC24B8E: /* bra     $c24b96 */
        step_branch(pc, opcode, 1); break;
    case 0xC24B90: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC24B92: /* bra     $c24b96 */
        step_branch(pc, opcode, 1); break;
    case 0xC24B94: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24B96: /* add.w   D4, D1 */
        value = D(4); step_add_word(&D(1), value); break;
    case 0xC24B98: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC24B9A: /* muls.w  D5, D0 */
        value = D(5); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC24B9C: /* divs.w  D2, D0 */
        value = D(2); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC24B9E: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24BA0: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24BA2: /* bge     $c24ba6 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24BA4: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24BA6: /* cmp.w   D0, D6 */
        value = D(0); result = D(6); step_compare_word(value, result); break;
    case 0xC24BA8: /* bgt     $c24bb8 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24BAA: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24BAC: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24BAE: /* bge     $c24bb4 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24BB0: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC24BB2: /* bra     $c24bba */
        step_branch(pc, opcode, 1); break;
    case 0xC24BB4: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC24BB6: /* bra     $c24bba */
        step_branch(pc, opcode, 1); break;
    case 0xC24BB8: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24BBA: /* add.w   D3, D0 */
        value = D(3); step_add_word(&D(0), value); break;
    case 0xC24BBC: /* move.w  D1, D2 */
        value = D(1); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC24BBE: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC24BC0: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC24BC4: /* bsr     $c248b2 */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC24BC8: /* tst.b   ($6,A4) */
        value = m68k_read_memory_8(step_displacement(A(4))); flags_logic_b(value); break;
    case 0xC24BCC: /* beq     $c24c5e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24BD0: /* movem.w ($20,A2), D0-D5 */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_load(address, mask, 2, -1); break;
    case 0xC24BD6: /* cmp.w   D2, D0 */
        value = D(2); result = D(0); step_compare_word(value, result); break;
    case 0xC24BD8: /* bgt     $c24bf4 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24BDA: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC24BDC: /* ble     $c24c5e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24BE0: /* bra     $c24bfa */
        step_branch(pc, opcode, 1); break;
    case 0xC24BE2: /* move.w  #$8, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC24BEA: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24BF0: /* bra     $c24d8c */
        step_branch(pc, opcode, 1); break;
    case 0xC24BF4: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC24BF6: /* bgt     $c24c5e */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24BFA: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC24BFC: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC24BFE: /* add.w   D5, D2 */
        value = D(5); step_add_word(&D(2), value); break;
    case 0xC24C00: /* add.w   D0, D2 */
        value = D(0); step_add_word(&D(2), value); break;
    case 0xC24C02: /* beq     $c24be2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24C04: /* sub.w   D3, D5 */
        value = D(3); step_subtract_word(&D(5), value); break;
    case 0xC24C06: /* muls.w  D5, D0 */
        value = D(5); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC24C08: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC24C0A: /* bge     $c24c0e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24C0C: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC24C0E: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC24C10: /* divs.w  D2, D0 */
        value = D(2); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC24C12: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24C14: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24C16: /* bge     $c24c1a */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24C18: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24C1A: /* cmp.w   D0, D6 */
        value = D(0); result = D(6); step_compare_word(value, result); break;
    case 0xC24C1C: /* bgt     $c24c2c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24C1E: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24C20: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24C22: /* bge     $c24c28 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24C24: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC24C26: /* bra     $c24c2e */
        step_branch(pc, opcode, 1); break;
    case 0xC24C28: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC24C2A: /* bra     $c24c2e */
        step_branch(pc, opcode, 1); break;
    case 0xC24C2C: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24C2E: /* add.w   D3, D0 */
        value = D(3); step_add_word(&D(0), value); break;
    case 0xC24C30: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC24C32: /* muls.w  D5, D1 */
        value = D(5); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC24C34: /* divs.w  D2, D1 */
        value = D(2); renderer_divide(&D(1), (int16_t)value); break;
    case 0xC24C36: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24C38: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24C3A: /* bge     $c24c3e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24C3C: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24C3E: /* cmp.w   D1, D6 */
        value = D(1); result = D(6); step_compare_word(value, result); break;
    case 0xC24C40: /* bgt     $c24c50 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24C42: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24C44: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24C46: /* bge     $c24c4c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24C48: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC24C4A: /* bra     $c24c52 */
        step_branch(pc, opcode, 1); break;
    case 0xC24C4C: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC24C4E: /* bra     $c24c52 */
        step_branch(pc, opcode, 1); break;
    case 0xC24C50: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24C52: /* add.w   D4, D1 */
        value = D(4); step_add_word(&D(1), value); break;
    case 0xC24C54: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC24C56: /* movem.w D0-D2, (A3) */
        mask = m68ki_read_imm_16(); address = A(3); renderer_store(address, mask, 2, -1); break;
    case 0xC24C5A: /* bsr     $c24996 */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC24C5E: /* tst.b   ($7,A4) */
        value = m68k_read_memory_8(step_displacement(A(4))); flags_logic_b(value); break;
    case 0xC24C62: /* beq     $c24cfe */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24C66: /* movem.w ($30,A2), D0-D5 */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_load(address, mask, 2, -1); break;
    case 0xC24C6C: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24C6E: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC24C70: /* cmp.w   D2, D0 */
        value = D(2); result = D(0); step_compare_word(value, result); break;
    case 0xC24C72: /* bgt     $c24c8e */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24C74: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC24C76: /* ble     $c24cfe */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24C7A: /* bra     $c24c94 */
        step_branch(pc, opcode, 1); break;
    case 0xC24C7C: /* move.w  #$9, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC24C84: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24C8A: /* bra     $c24d8c */
        step_branch(pc, opcode, 1); break;
    case 0xC24C8E: /* cmp.w   D5, D3 */
        value = D(5); result = D(3); step_compare_word(value, result); break;
    case 0xC24C90: /* bgt     $c24cfe */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24C94: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24C96: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC24C98: /* sub.w   D3, D0 */
        value = D(3); step_subtract_word(&D(0), value); break;
    case 0xC24C9A: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC24C9C: /* add.w   D5, D2 */
        value = D(5); step_add_word(&D(2), value); break;
    case 0xC24C9E: /* sub.w   D0, D2 */
        value = D(0); step_subtract_word(&D(2), value); break;
    case 0xC24CA0: /* beq     $c24c7c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24CA2: /* add.w   D3, D5 */
        value = D(3); step_add_word(&D(5), value); break;
    case 0xC24CA4: /* muls.w  D5, D0 */
        value = D(5); renderer_multiply(&D(0), (uint16_t)value); break;
    case 0xC24CA6: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC24CA8: /* bge     $c24cac */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24CAA: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC24CAC: /* asr.w   #1, D6 */
        renderer_asr_word(&D(6), 1); break;
    case 0xC24CAE: /* divs.w  D2, D0 */
        value = D(2); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC24CB0: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24CB2: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24CB4: /* bge     $c24cb8 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24CB6: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC24CB8: /* cmp.w   D0, D6 */
        value = D(0); result = D(6); step_compare_word(value, result); break;
    case 0xC24CBA: /* bgt     $c24cca */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24CBC: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24CBE: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC24CC0: /* bge     $c24cc6 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24CC2: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC24CC4: /* bra     $c24ccc */
        step_branch(pc, opcode, 1); break;
    case 0xC24CC6: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC24CC8: /* bra     $c24ccc */
        step_branch(pc, opcode, 1); break;
    case 0xC24CCA: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24CCC: /* add.w   D3, D0 */
        value = D(3); step_add_word(&D(0), value); break;
    case 0xC24CCE: /* sub.w   D4, D1 */
        value = D(4); step_subtract_word(&D(1), value); break;
    case 0xC24CD0: /* muls.w  D5, D1 */
        value = D(5); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC24CD2: /* divs.w  D2, D1 */
        value = D(2); renderer_divide(&D(1), (int16_t)value); break;
    case 0xC24CD4: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24CD6: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24CD8: /* bge     $c24cdc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24CDA: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24CDC: /* cmp.w   D1, D6 */
        value = D(1); result = D(6); step_compare_word(value, result); break;
    case 0xC24CDE: /* bgt     $c24cee */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24CE0: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24CE2: /* tst.w   D1 */
        value = D(1); flags_logic_w(value); break;
    case 0xC24CE4: /* bge     $c24cea */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24CE6: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC24CE8: /* bra     $c24cf0 */
        step_branch(pc, opcode, 1); break;
    case 0xC24CEA: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC24CEC: /* bra     $c24cf0 */
        step_branch(pc, opcode, 1); break;
    case 0xC24CEE: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC24CF0: /* add.w   D4, D1 */
        value = D(4); step_add_word(&D(1), value); break;
    case 0xC24CF2: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC24CF4: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC24CF6: /* move.w  D0, (A1)+ */
        value = D(0); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC24CF8: /* move.w  D1, (A1)+ */
        value = D(1); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC24CFA: /* move.w  D2, (A1)+ */
        value = D(2); m68k_write_memory_16(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC24CFC: /* addq.w  #1, D7 */
        value = 1u; step_add_word(&D(7), value); break;
    case 0xC24CFE: /* tst.w   D7 */
        value = D(7); flags_logic_w(value); break;
    case 0xC24D00: /* beq     $c24d72 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24D04: /* cmpi.w  #$2, D7 */
        value = m68ki_read_imm_16(); result = D(7); step_compare_word(value, result); break;
    case 0xC24D08: /* ble     $c24d28 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24D0A: /* lea     $c4b990.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC24D10: /* lea     $c4b390.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC24D16: /* move.w  #$13f, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC24D1A: /* move.w  #$b3, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC24D1E: /* move.w  D7, (A0)+ */
        value = D(7); m68k_write_memory_16(A(0), value); A(0) += 2; flags_logic_w(value); break;
    case 0xC24D20: /* subq.w  #1, D7 */
        value = 1u; step_subtract_word(&D(7), value); break;
    case 0xC24D22: /* movem.w (A1)+, D3-D5 */
        mask = m68ki_read_imm_16(); address = A(1); renderer_load(address, mask, 2, 1); break;
    case 0xC24D26: /* tst.w   D5 */
        value = D(5); flags_logic_w(value); break;
    case 0xC24D28: /* ble     $c24da0 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24D2C: /* muls.w  #$a0, D3 */
        value = m68ki_read_imm_16(); renderer_multiply(&D(3), (uint16_t)value); break;
    case 0xC24D30: /* divs.w  D5, D3 */
        value = D(5); renderer_divide(&D(3), (int16_t)value); break;
    case 0xC24D32: /* addi.w  #$a0, D3 */
        value = m68ki_read_imm_16(); step_add_word(&D(3), value); break;
    case 0xC24D36: /* blt     $c24d78 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC24D38: /* cmpi.w  #$140, D3 */
        value = m68ki_read_imm_16(); result = D(3); step_compare_word(value, result); break;
    case 0xC24D3C: /* bge     $c24d80 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24D3E: /* muls.w  #$5a, D4 */
        value = m68ki_read_imm_16(); renderer_multiply(&D(4), (uint16_t)value); break;
    case 0xC24D42: /* divs.w  D5, D4 */
        value = D(5); renderer_divide(&D(4), (int16_t)value); break;
    case 0xC24D44: /* addi.w  #$5a, D4 */
        value = m68ki_read_imm_16(); step_add_word(&D(4), value); break;
    case 0xC24D48: /* blt     $c24d7c */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC24D4A: /* cmpi.w  #$b4, D4 */
        value = m68ki_read_imm_16(); result = D(4); step_compare_word(value, result); break;
    case 0xC24D4E: /* bge     $c24d86 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24D50: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC24D52: /* move.w  D1, D5 */
        value = D(1); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC24D54: /* sub.w   D3, D2 */
        value = D(3); step_subtract_word(&D(2), value); break;
    case 0xC24D56: /* sub.w   D4, D5 */
        value = D(4); step_subtract_word(&D(5), value); break;
    case 0xC24D58: /* move.w  D2, (A0)+ */
        value = D(2); m68k_write_memory_16(A(0), value); A(0) += 2; flags_logic_w(value); break;
    case 0xC24D5A: /* move.w  D5, (A0)+ */
        value = D(5); m68k_write_memory_16(A(0), value); A(0) += 2; flags_logic_w(value); break;
    case 0xC24D5C: /* dbra    D7, $c24d22 */
        step_dbf(pc, &D(7)); break;
    case 0xC24D60: /* jsr     $c2ff48.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24D66: /* addq.w  #1, $c46182.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); step_add_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC24D6C: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC24D6E: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24D70: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24D72: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC24D74: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24D76: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24D78: /* clr.w   D3 */
        SET_W(D(3), 0); flags_logic_w(0); break;
    case 0xC24D7A: /* bra     $c24d3e */
        step_branch(pc, opcode, 1); break;
    case 0xC24D7C: /* clr.w   D4 */
        SET_W(D(4), 0); flags_logic_w(0); break;
    case 0xC24D7E: /* bra     $c24d50 */
        step_branch(pc, opcode, 1); break;
    case 0xC24D80: /* move.w  #$13f, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC24D84: /* bra     $c24d3e */
        step_branch(pc, opcode, 1); break;
    case 0xC24D86: /* move.w  #$b3, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC24D8A: /* bra     $c24d50 */
        step_branch(pc, opcode, 1); break;
    case 0xC24D8C: /* addq.w  #1, $c458ec.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_16(address); step_add_word(&result, value); m68k_write_memory_16(address, result); break;
    case 0xC24D92: /* move.w  #$a, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC24D9A: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24DA0: /* clr.w   D7 */
        SET_W(D(7), 0); flags_logic_w(0); break;
    case 0xC24DA2: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC24DA4: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24DA6: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C2469E_step(void) { return glue_C246A0_step(); }

int glue_C247C0_step(void) { return glue_C246A0_step(); }

int glue_C248B2_step(void) { return glue_C246A0_step(); }

int glue_C24996_step(void) { return glue_C246A0_step(); }
