/* Source timing for the $C23CA6 record-view walk and its shared exits.
 * Readable domain behavior remains in control_records.c. Every case performs
 * one source instruction, retaining its original bus and event boundary. */
#include "glue_renderer_step_math.h"

int glue_C23CA6_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode, mask;
    if (pc < 0xC23CA6u || pc >= 0xC24368u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC23CA6: /* tst.b   $c457ae.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC23CAC: /* bne     $c23d7a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC23CB0: /* cmpi.b  #$8, ($5,A1) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(step_displacement(A(1))); step_compare_byte(value, result); break;
    case 0xC23CB6: /* bne     $c23d3c */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC23CBA: /* btst    #$0, ($2,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC23CC0: /* bne     $c23d3c */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC23CC2: /* cmpi.w  #$480, ($4a,A1) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_16(step_displacement(A(1))); step_compare_word(value, result); break;
    case 0xC23CC8: /* bgt     $c23d3c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC23CCA: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC23CCC: /* move.b  ($3a,A1), D0 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC23CD0: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC23CD2: /* lea     $c295e0.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC23CD8: /* adda.w  (A4,D0.w), A4 */
        value = m68k_read_memory_16(step_indexed(A(4))); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC23CDC: /* move.l  (A4), D0 */
        value = m68k_read_memory_32(A(4)); D(0) = value; flags_logic_l(value); break;
    case 0xC23CDE: /* cmp.l   ($2c,A1), D0 */
        value = m68k_read_memory_32(step_displacement(A(1))); result = D(0); step_compare_long(value, result); break;
    case 0xC23CE2: /* bne     $c23cee */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC23CE4: /* move.l  ($4,A4), D0 */
        value = m68k_read_memory_32(step_displacement(A(4))); D(0) = value; flags_logic_l(value); break;
    case 0xC23CE8: /* cmp.l   ($30,A1), D0 */
        value = m68k_read_memory_32(step_displacement(A(1))); result = D(0); step_compare_long(value, result); break;
    case 0xC23CEC: /* beq     $c23d06 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC23CEE: /* adda.w  #$a, A4 */
        value = m68ki_read_imm_16(); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC23CF2: /* tst.w   (A4) */
        value = m68k_read_memory_16(A(4)); flags_logic_w(value); break;
    case 0xC23CF4: /* bge     $c23cdc */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC23CF6: /* move.w  #$35, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC23CFE: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC23D04: /* bra     $c23d3c */
        step_branch(pc, opcode, 1); break;
    case 0xC23D06: /* tst.w   ($a,A4) */
        value = m68k_read_memory_16(step_displacement(A(4))); flags_logic_w(value); break;
    case 0xC23D0A: /* bge     $c23d24 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC23D0C: /* bclr    #$7, ($1,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~value); break;
    case 0xC23D12: /* beq     $c23d3c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC23D14: /* cmpi.b  #$15, ($62,A1) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(step_displacement(A(1))); step_compare_byte(value, result); break;
    case 0xC23D1A: /* bne     $c23d3c */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC23D1C: /* ori.w   #$200, ($0,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); value |= m68k_read_memory_16(address); m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC23D22: /* bra     $c23d3c */
        step_branch(pc, opcode, 1); break;
    case 0xC23D24: /* movem.w ($a,A4), D0-D4 */
        mask = m68ki_read_imm_16(); address = step_displacement(A(4)); renderer_load(address, mask, 2, -1); break;
    case 0xC23D2A: /* movem.w D0-D3, ($2c,A1) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(1)); renderer_store(address, mask, 2, -1); break;
    case 0xC23D30: /* ext.l   D4 */
        D(4) = (uint32_t)(int32_t)(int16_t)D(4); flags_logic_l(D(4)); break;
    case 0xC23D32: /* move.l  D4, ($34,A1) */
        value = D(4); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC23D36: /* move.w  #$7fff, ($4a,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC23D3C: /* cmpi.b  #$8, ($5,A1) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(step_displacement(A(1))); step_compare_byte(value, result); break;
    case 0xC23D42: /* beq     $c241a6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC23D46: /* move.w  ($0,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23D4A: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC23D4C: /* andi.w  #$8, D0 */
        value = m68ki_read_imm_16(); value &= D(0); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23D50: /* bne     $c23d72 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC23D52: /* lea     $c46184.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC23D58: /* move.b  ($38,A1), D0 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC23D5C: /* cmpi.b  #-$1, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC23D60: /* beq     $c2407e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC23D64: /* andi.w  #$7f, D0 */
        value = m68ki_read_imm_16(); value &= D(0); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23D68: /* asl.w   #8, D0 */
        renderer_asl_word(&D(0), 8); break;
    case 0xC23D6A: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC23D6C: /* adda.w  D0, A3 */
        value = D(0); A(3) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC23D6E: /* bra     $c24056 */
        step_branch(pc, opcode, 1); break;
    case 0xC23D72: /* andi.w  #$2, D1 */
        value = m68ki_read_imm_16(); value &= D(1); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC23D76: /* bne     $c23ff8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC23D7A: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC23D7C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC23F8E: /* move.w  #$4200, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC23F92: /* move.l  #$0, ($34,A1) */
        value = m68ki_read_imm_32(); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC23F9A: /* move.w  ($6c,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23F9E: /* cmp.w   D1, D0 */
        value = D(1); result = D(0); step_compare_word(value, result); break;
    case 0xC23FA0: /* bgt     $c23fac */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC23FA2: /* addi.w  #$240, D0 */
        value = m68ki_read_imm_16(); step_add_word(&D(0), value); break;
    case 0xC23FA6: /* cmp.w   D1, D0 */
        value = D(1); result = D(0); step_compare_word(value, result); break;
    case 0xC23FA8: /* ble     $c23fb6 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC23FAA: /* bra     $c23fb4 */
        step_branch(pc, opcode, 1); break;
    case 0xC23FAC: /* subi.w  #$240, D0 */
        value = m68ki_read_imm_16(); step_subtract_word(&D(0), value); break;
    case 0xC23FB0: /* cmp.w   D1, D0 */
        value = D(1); result = D(0); step_compare_word(value, result); break;
    case 0xC23FB2: /* bge     $c23fb6 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC23FB4: /* move.w  D1, D0 */
        value = D(1); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC23FB6: /* move.w  D0, ($6c,A1) */
        value = D(0); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC23FBA: /* move.w  D0, ($6e,A1) */
        value = D(0); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC23FBE: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC23FC0: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC23FF8: /* andi.w  #$fffe, ($0,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); value &= m68k_read_memory_16(address); m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC23FFE: /* btst    #$3, ($1,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC24004: /* beq     $c24052 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24006: /* move.w  $c459c0.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2400C: /* cmpi.w  #-$1, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC24010: /* beq     $c23f8e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24014: /* cmp.w   $c459b6.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); result = D(0); step_compare_word(value, result); break;
    case 0xC2401A: /* beq     $c23f8e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2401E: /* lea     $c46184.l, A2 */
        A(2) = m68ki_read_imm_32(); break;
    case 0xC24024: /* adda.w  D0, A2 */
        value = D(0); A(2) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24026: /* move.w  ($6,A2), ($2c,A1) */
        value = m68k_read_memory_16(step_displacement(A(2))); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC2402C: /* move.w  ($8,A2), ($2e,A1) */
        value = m68k_read_memory_16(step_displacement(A(2))); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC24032: /* movem.w ($c,A2), D2-D3 */
        mask = m68ki_read_imm_16(); address = step_displacement(A(2)); renderer_load(address, mask, 2, -1); break;
    case 0xC24038: /* move.l  ($10,A2), D4 */
        value = m68k_read_memory_32(step_displacement(A(2))); D(4) = value; flags_logic_l(value); break;
    case 0xC2403C: /* movem.w D2-D3, ($30,A1) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(1)); renderer_store(address, mask, 2, -1); break;
    case 0xC24042: /* move.l  D4, ($34,A1) */
        value = D(4); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC24046: /* lsr.w   #8, D0 */
        SET_W(D(0), step_lsr_word_value((uint16_t)D(0), 8)); break;
    case 0xC24048: /* lsr.w   #1, D0 */
        SET_W(D(0), step_lsr_word_value((uint16_t)D(0), 1)); break;
    case 0xC2404A: /* ori.b   #$80, D0 */
        value = m68ki_read_imm_16(); value |= D(0); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC2404E: /* move.b  D0, ($38,A1) */
        value = D(0); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC24052: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24054: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24056: /* cmpi.b  #$5, ($7a,A1) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(step_displacement(A(1))); step_compare_byte(value, result); break;
    case 0xC2405C: /* bne     $c24064 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2405E: /* move.b  #$3, ($7a,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC24064: /* btst    #$6, ($1,A3) */
        value = m68ki_read_imm_16(); address = step_displacement(A(3)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC2406A: /* beq     $c242de */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2406E: /* ori.w   #$1, ($0,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); value |= m68k_read_memory_16(address); m68k_write_memory_16(address, value); flags_logic_w(value); break;
    case 0xC24074: /* move.l  #$c46184, D0 */
        value = m68ki_read_imm_32(); D(0) = value; flags_logic_l(value); break;
    case 0xC2407A: /* cmp.l   A3, D0 */
        value = A(3); result = D(0); step_compare_long(value, result); break;
    case 0xC2407C: /* beq     $c240e2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2407E: /* cmpi.b  #$8, ($5,A1) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(step_displacement(A(1))); step_compare_byte(value, result); break;
    case 0xC24084: /* beq     $c240e2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24086: /* movem.l ($14,A1), D0-D2 */
        mask = m68ki_read_imm_16(); address = step_displacement(A(1)); renderer_load(address, mask, 4, -1); break;
    case 0xC2408C: /* movem.l $c46198.l, D3-D5 */
        mask = m68ki_read_imm_16(); address = m68ki_read_imm_32(); renderer_load(address, mask, 4, -1); break;
    case 0xC24094: /* cmpi.b  #$3, $c458a7.l */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(m68ki_read_imm_32()); step_compare_byte(value, result); break;
    case 0xC2409C: /* bge     $c240b8 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2409E: /* cmpi.b  #$2, $c458a7.l */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(m68ki_read_imm_32()); step_compare_byte(value, result); break;
    case 0xC240A6: /* bge     $c240b0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC240A8: /* move.l  #$180000, D6 */
        value = m68ki_read_imm_32(); D(6) = value; flags_logic_l(value); break;
    case 0xC240AE: /* bra     $c240be */
        step_branch(pc, opcode, 1); break;
    case 0xC240B0: /* move.l  #$240000, D6 */
        value = m68ki_read_imm_32(); D(6) = value; flags_logic_l(value); break;
    case 0xC240B6: /* bra     $c240be */
        step_branch(pc, opcode, 1); break;
    case 0xC240B8: /* move.l  #$300000, D6 */
        value = m68ki_read_imm_32(); D(6) = value; flags_logic_l(value); break;
    case 0xC240BE: /* sub.l   D0, D3 */
        value = D(0); step_subtract_long(&D(3), value); break;
    case 0xC240C0: /* bge     $c240c4 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC240C2: /* neg.l   D3 */
        renderer_negate(&D(3), 4); break;
    case 0xC240C4: /* cmp.l   D6, D3 */
        value = D(6); result = D(3); step_compare_long(value, result); break;
    case 0xC240C6: /* bgt     $c240e2 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC240C8: /* sub.l   D1, D4 */
        value = D(1); step_subtract_long(&D(4), value); break;
    case 0xC240CA: /* bge     $c240ce */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC240CC: /* neg.l   D4 */
        renderer_negate(&D(4), 4); break;
    case 0xC240CE: /* cmp.l   D6, D4 */
        value = D(6); result = D(4); step_compare_long(value, result); break;
    case 0xC240D0: /* bgt     $c240e2 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC240D2: /* sub.l   D2, D5 */
        value = D(2); step_subtract_long(&D(5), value); break;
    case 0xC240D4: /* bge     $c240d8 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC240D6: /* neg.l   D5 */
        renderer_negate(&D(5), 4); break;
    case 0xC240D8: /* cmp.l   D6, D5 */
        value = D(6); result = D(5); step_compare_long(value, result); break;
    case 0xC240DA: /* bgt     $c240e2 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC240DC: /* move.b  #$80, ($38,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC240E2: /* move.w  ($2,A1), D0 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC240E6: /* andi.w  #$1, D0 */
        value = m68ki_read_imm_16(); value &= D(0); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC240EA: /* beq     $c24128 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC240EC: /* btst    #$8, ($64,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC240F2: /* bne     $c2410e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC240F4: /* btst    #$1, ($64,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC240FA: /* bne     $c24102 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC240FC: /* move.w  #$ffa0, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC24100: /* bra     $c24106 */
        step_branch(pc, opcode, 1); break;
    case 0xC24102: /* move.w  #$a8, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC24106: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC24108: /* move.w  #$ffd0, D5 */
        value = m68ki_read_imm_16(); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2410C: /* bra     $c24178 */
        step_branch(pc, opcode, 1); break;
    case 0xC2410E: /* btst    #$1, ($64,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC24114: /* bne     $c2411c */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC24116: /* move.w  #$ffa0, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2411A: /* bra     $c24120 */
        step_branch(pc, opcode, 1); break;
    case 0xC2411C: /* move.w  #$60, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC24120: /* moveq   #$0, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC24122: /* move.w  #$ffd0, D5 */
        value = m68ki_read_imm_16(); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC24126: /* bra     $c24178 */
        step_branch(pc, opcode, 1); break;
    case 0xC24128: /* move.w  $c458da.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2412E: /* andi.w  #$ff, D3 */
        value = m68ki_read_imm_16(); value &= D(3); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC24132: /* bne     $c2415e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC24134: /* move.b  #$14, D3 */
        value = m68ki_read_imm_16(); SET_B(D(3), value); flags_logic_b(value); break;
    case 0xC24138: /* move.b  ($28,A3), D4 */
        value = m68k_read_memory_8(step_displacement(A(3))); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC2413C: /* bge     $c24140 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2413E: /* neg.b   D4 */
        renderer_negate(&D(4), 1); break;
    case 0xC24140: /* cmp.w   D3, D4 */
        value = D(3); result = D(4); step_compare_word(value, result); break;
    case 0xC24142: /* bgt     $c24150 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24144: /* move.b  ($2a,A3), D4 */
        value = m68k_read_memory_8(step_displacement(A(3))); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC24148: /* bge     $c2414c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2414A: /* neg.b   D4 */
        renderer_negate(&D(4), 1); break;
    case 0xC2414C: /* cmp.b   D3, D4 */
        value = D(3); result = D(4); step_compare_byte(value, result); break;
    case 0xC2414E: /* ble     $c2415e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24150: /* move.w  ($16,A3), D4 */
        value = m68k_read_memory_16(step_displacement(A(3))); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC24154: /* andi.w  #$4, D4 */
        value = m68ki_read_imm_16(); value &= D(4); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC24158: /* move.b  D4, $c4578a.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC2415E: /* tst.b   $c4578a.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC24164: /* bra     $c24170 */
        step_branch(pc, opcode, 1); break;
    case 0xC24170: /* moveq   #$0, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC24172: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC24174: /* move.w  #$ffdc, D5 */
        value = m68ki_read_imm_16(); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC24178: /* exg     A1, A3 */
        value = A(1); A(1) = A(3); A(3) = value; break;
    case 0xC2417A: /* jsr     $c091e0.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24180: /* exg     A1, A3 */
        value = A(1); A(1) = A(3); A(3) = value; break;
    case 0xC24182: /* move.l  D0, D3 */
        value = D(0); D(3) = value; flags_logic_l(value); break;
    case 0xC24184: /* move.l  D2, D4 */
        value = D(2); D(4) = value; flags_logic_l(value); break;
    case 0xC24186: /* asr.l   #8, D3 */
        step_asr_long(&D(3), 8); break;
    case 0xC24188: /* asr.l   #8, D4 */
        step_asr_long(&D(4), 8); break;
    case 0xC2418A: /* andi.w  #$3fff, D3 */
        value = m68ki_read_imm_16(); value &= D(3); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2418E: /* andi.w  #$3fff, D4 */
        value = m68ki_read_imm_16(); value &= D(4); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC24192: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC24194: /* swap    D2 */
        step_swap(&D(2)); break;
    case 0xC24196: /* asr.w   #6, D0 */
        renderer_asr_word(&D(0), 6); break;
    case 0xC24198: /* asr.w   #6, D2 */
        renderer_asr_word(&D(2), 6); break;
    case 0xC2419A: /* movem.w D0/D2-D4, ($2c,A1) */
        mask = m68ki_read_imm_16(); address = step_displacement(A(1)); renderer_store(address, mask, 2, -1); break;
    case 0xC241A0: /* asr.l   #8, D1 */
        step_asr_long(&D(1), 8); break;
    case 0xC241A2: /* move.l  D1, ($34,A1) */
        value = D(1); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC241A6: /* btst    #$1, ($20,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC241AC: /* bne     $c242d4 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC241B0: /* bsr     $c2436a */
        value = (uint32_t)(int32_t)(int8_t)opcode; if (!(opcode & 0xffu)) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC241B4: /* move.b  ($5,A1), D0 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC241B8: /* beq     $c241e4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC241BA: /* cmpi.b  #$1, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC241BE: /* beq     $c241e4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC241C0: /* cmpi.b  #$6, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC241C4: /* beq     $c242d4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC241C8: /* cmpi.b  #$8, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC241CC: /* bne     $c241e4 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC241CE: /* cmpi.b  #$5, $c458a6.l */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(m68ki_read_imm_32()); step_compare_byte(value, result); break;
    case 0xC241D6: /* bne     $c242d4 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC241DA: /* tst.b   $c457b7.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC241E0: /* beq     $c242d4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC241E4: /* btst    #$5, ($4,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC241EA: /* beq     $c242d4 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC241EE: /* lea     $c2bb98.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC241F4: /* move.w  $c458c6.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC241FA: /* moveq   #$0, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC241FC: /* cmpi.w  #$300, ($4a,A3) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_16(step_displacement(A(3))); step_compare_word(value, result); break;
    case 0xC24202: /* bgt     $c24230 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC24204: /* move.w  ($6c,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC24208: /* sub.w   ($6c,A3), D1 */
        value = m68k_read_memory_16(step_displacement(A(3))); step_subtract_word(&D(1), value); break;
    case 0xC2420C: /* bge     $c24212 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2420E: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC24210: /* moveq   #$1, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC24212: /* cmpi.w  #$c0, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC24216: /* blt     $c24224 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC24218: /* adda.w  #$20, A4 */
        value = m68ki_read_imm_16(); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2421C: /* tst.b   D2 */
        value = D(2); flags_logic_b(value); break;
    case 0xC2421E: /* bne     $c24224 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC24220: /* adda.w  #$20, A4 */
        value = m68ki_read_imm_16(); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24224: /* andi.w  #$8, D0 */
        value = m68ki_read_imm_16(); value &= D(0); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC24228: /* bra     $c24290 */
        step_branch(pc, opcode, 1); break;
    case 0xC24230: /* adda.w  #$60, A4 */
        value = m68ki_read_imm_16(); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24234: /* cmpi.w  #$c00, ($4a,A3) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_16(step_displacement(A(3))); step_compare_word(value, result); break;
    case 0xC2423A: /* ble     $c2424e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2423C: /* adda.w  #$60, A4 */
        value = m68ki_read_imm_16(); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24240: /* cmpi.w  #$1e00, ($4a,A3) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_16(step_displacement(A(3))); step_compare_word(value, result); break;
    case 0xC24246: /* ble     $c2424e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24248: /* adda.w  #$60, A4 */
        value = m68ki_read_imm_16(); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2424C: /* bra     $c24290 */
        step_branch(pc, opcode, 1); break;
    case 0xC2424E: /* move.w  ($6c,A1), D1 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC24252: /* sub.w   ($6c,A3), D1 */
        value = m68k_read_memory_16(step_displacement(A(3))); step_subtract_word(&D(1), value); break;
    case 0xC24256: /* bge     $c2425c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC24258: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2425A: /* moveq   #$1, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC2425C: /* cmpi.w  #$c0, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC24260: /* blt     $c2426e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC24262: /* adda.w  #$20, A4 */
        value = m68ki_read_imm_16(); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24266: /* tst.b   D2 */
        value = D(2); flags_logic_b(value); break;
    case 0xC24268: /* bne     $c2426e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2426A: /* adda.w  #$20, A4 */
        value = m68ki_read_imm_16(); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC2426E: /* move.w  $c459c0.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC24274: /* blt     $c24290 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC24276: /* cmp.w   $c459b6.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); result = D(0); step_compare_word(value, result); break;
    case 0xC2427C: /* bne     $c24290 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2427E: /* tst.b   $c457b7.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC24284: /* beq     $c24290 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24286: /* clr.b   $c457b7.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC2428C: /* adda.w  #$10, A4 */
        value = m68ki_read_imm_16(); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24290: /* move.b  $c458a7.l, D1 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC24296: /* ext.w   D1 */
        SET_W(D(1), (uint32_t)(int32_t)(int8_t)D(1)); flags_logic_w(D(1)); break;
    case 0xC24298: /* cmpi.w  #$3, D1 */
        value = m68ki_read_imm_16(); result = D(1); step_compare_word(value, result); break;
    case 0xC2429C: /* ble     $c242a0 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2429E: /* moveq   #$3, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC242A0: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC242A2: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC242A4: /* move.w  (A4,D1.w), ($4c,A1) */
        value = m68k_read_memory_16(step_indexed(A(4))); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC242AA: /* move.w  ($2,A4,D1.w), D0 */
        value = m68k_read_memory_16(step_indexed(A(4))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC242AE: /* cmpi.b  #$8, ($5,A1) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(step_displacement(A(1))); step_compare_byte(value, result); break;
    case 0xC242B4: /* bne     $c242d0 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC242B6: /* btst    #$3, ($1,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC242BC: /* beq     $c242d0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC242BE: /* bclr    #$3, ($1,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~value); break;
    case 0xC242C4: /* addq.b  #1, $c458a9.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC242CA: /* addq.b  #1, $c458aa.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC242D0: /* move.b  D0, ($5,A1) */
        value = D(0); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC242D4: /* clr.b   $c457b7.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC242DA: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC242DC: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC242DE: /* btst    #$4, ($0,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC242E4: /* beq     $c242da */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC242E6: /* movea.l $c4573a.l, A3 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(3) = value; break;
    case 0xC242EC: /* tst.w   (A3) */
        value = m68k_read_memory_16(A(3)); flags_logic_w(value); break;
    case 0xC242EE: /* blt     $c24306 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC242F0: /* move.w  ($4,A3), D0 */
        value = m68k_read_memory_16(step_displacement(A(3))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC242F4: /* andi.w  #$ff, D0 */
        value = m68ki_read_imm_16(); value &= D(0); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC242F8: /* cmp.w   $c459b4.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); result = D(0); step_compare_word(value, result); break;
    case 0xC242FE: /* beq     $c24332 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24300: /* adda.w  #$a, A3 */
        value = m68ki_read_imm_16(); A(3) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24304: /* bra     $c242ec */
        step_branch(pc, opcode, 1); break;
    case 0xC24306: /* move.b  ($5d,A1), D0 */
        value = m68k_read_memory_8(step_displacement(A(1))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC2430A: /* ble     $c24322 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2430C: /* lea     $c29720.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC24312: /* subq.b  #1, D0 */
        value = 1u; step_subtract_byte(&D(0), value); break;
    case 0xC24314: /* ext.w   D0 */
        SET_W(D(0), (uint32_t)(int32_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC24316: /* asl.w   #2, D0 */
        renderer_asl_word(&D(0), 2); break;
    case 0xC24318: /* movea.l (A3,D0.w), A3 */
        value = m68k_read_memory_32(step_indexed(A(3))); A(3) = value; break;
    case 0xC2431C: /* adda.w  #$a, A3 */
        value = m68ki_read_imm_16(); A(3) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24320: /* bra     $c242ec */
        step_branch(pc, opcode, 1); break;
    case 0xC24322: /* move.w  #$34, $c4599e.l */
        value = m68ki_read_imm_16(); m68k_write_memory_16(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2432A: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24330: /* bra     $c242d4 */
        step_branch(pc, opcode, 1); break;
    case 0xC24332: /* move.w  ($6,A3), D2 */
        value = m68k_read_memory_16(step_displacement(A(3))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC24336: /* lea     $c295e0.l, A4 */
        A(4) = m68ki_read_imm_32(); break;
    case 0xC2433C: /* adda.w  (A4,D2.w), A4 */
        value = m68k_read_memory_16(step_indexed(A(4))); A(4) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24340: /* movem.w (A4)+, D2-D6 */
        mask = m68ki_read_imm_16(); address = A(4); renderer_load(address, mask, 2, 4); break;
    case 0xC24344: /* move.w  D2, ($2c,A1) */
        value = D(2); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC24348: /* move.w  D3, ($2e,A1) */
        value = D(3); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC2434C: /* move.w  D4, ($30,A1) */
        value = D(4); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC24350: /* move.w  D5, ($32,A1) */
        value = D(5); m68k_write_memory_16(step_displacement(A(1)), value); flags_logic_w(value); break;
    case 0xC24354: /* move.l  D6, ($34,A1) */
        value = D(6); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC24358: /* move.b  #$ff, ($38,A1) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(step_displacement(A(1)), value); flags_logic_b(value); break;
    case 0xC2435E: /* bclr    #$0, ($1,A1) */
        value = m68ki_read_imm_16(); address = step_displacement(A(1)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~value); break;
    case 0xC24364: /* bra     $c242d4 */
        step_branch(pc, opcode, 1); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
