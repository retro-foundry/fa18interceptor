/* Date field, packed BCD and fixed-width hex formatting; numbers.c and text.c.
 * CPU register/flag effects, bus accesses and source instruction boundaries
 * stay in this bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

#include "glue_unsigned_division_step.h"

int glue_C24E2C_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC0F56A: /* link    A6, #-$2 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0F56E: /* move.b  ($13,A6), D0 */
        value = m68k_read_memory_8(step_displacement(A(6))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC0F572: /* ext.w   D0 */
        SET_W(D(0), (int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC0F574: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC0F576: /* add.l   D0, ($8,A6) */
        value = D(0); address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC0F57A: /* clr.b   (-$1,A6) */
        value = 0; m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(0); break;
    case 0xC0F57E: /* move.b  (-$1,A6), D0 */
        value = m68k_read_memory_8(step_displacement(A(6))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC0F582: /* cmp.b   ($13,A6), D0 */
        value = m68k_read_memory_8(step_displacement(A(6))); result = D(0); step_compare_byte(value, result); break;
    case 0xC0F586: /* bge     $c0f5c2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC0F588: /* move.l  ($c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC0F58C: /* andi.l  #$f, D0 */
        value = m68ki_read_imm_32(); D(0) = D(0) & value; flags_logic_l(D(0)); break;
    case 0xC0F592: /* addi.l  #$30, D0 */
        value = m68ki_read_imm_32(); step_add_long(&D(0), value); break;
    case 0xC0F598: /* move.b  D0, (-$2,A6) */
        value = D(0); m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(value); break;
    case 0xC0F59C: /* cmpi.b  #$39, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC0F5A0: /* ble     $c0f5a6 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC0F5A2: /* addq.b  #7, (-$2,A6) */
        value = 7u; address = step_displacement(A(6)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC0F5A6: /* movea.l ($8,A6), A0 */
        value = m68k_read_memory_32(step_displacement(A(6))); A(0) = value; break;
    case 0xC0F5AA: /* move.b  (-$2,A6), (A0) */
        value = m68k_read_memory_8(step_displacement(A(6))); m68k_write_memory_8(A(0), value); flags_logic_b(value); break;
    case 0xC0F5AE: /* subq.l  #1, ($8,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_subtract_long(&result, value); step_write_long(address, result); break;
    case 0xC0F5B2: /* addq.b  #1, (-$1,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC0F5B6: /* move.l  ($c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC0F5BA: /* lsr.l   #4, D0 */
        step_lsr_long(&D(0), 4); break;
    case 0xC0F5BC: /* move.l  D0, ($c,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC0F5C0: /* bra     $c0f57e */
        step_branch(pc, opcode, 1); break;
    case 0xC0F5C2: /* subq.b  #1, ($13,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_8(address); step_subtract_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC0F5C6: /* addq.l  #1, ($8,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC0F5CA: /* clr.b   (-$1,A6) */
        value = 0; m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(0); break;
    case 0xC0F5CE: /* move.b  (-$1,A6), D0 */
        value = m68k_read_memory_8(step_displacement(A(6))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC0F5D2: /* cmp.b   ($13,A6), D0 */
        value = m68k_read_memory_8(step_displacement(A(6))); result = D(0); step_compare_byte(value, result); break;
    case 0xC0F5D6: /* bge     $c0f5f4 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC0F5D8: /* movea.l ($8,A6), A0 */
        value = m68k_read_memory_32(step_displacement(A(6))); A(0) = value; break;
    case 0xC0F5DC: /* move.b  (A0), D0 */
        value = m68k_read_memory_8(A(0)); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC0F5DE: /* cmpi.b  #$30, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_byte(value, result); break;
    case 0xC0F5E2: /* bne     $c0f5f4 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC0F5E4: /* move.b  #$20, (A0) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(0), value); flags_logic_b(value); break;
    case 0xC0F5E8: /* addq.l  #1, ($8,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC0F5EC: /* nop */
        break;
    case 0xC0F5EE: /* addq.b  #1, (-$1,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC0F5F2: /* bra     $c0f5ce */
        step_branch(pc, opcode, 1); break;
    case 0xC0F5F4: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC0F5F6: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24E2C: /* movea.l $c1ab74.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC24E32: /* move.l  ($8,A0), D0 */
        value = m68k_read_memory_32(step_displacement(A(0))); D(0) = value; flags_logic_l(value); break;
    case 0xC24E36: /* divu.w  #$e10, D0 */
        value = m68ki_read_imm_16(); step_divide_unsigned(&D(0), (uint16_t)value); break;
    case 0xC24E3A: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC24E3C: /* lea     (-$36,PC), A0; ($c24e08) */
        A(0) = pc + 2 + (int16_t)m68ki_read_imm_16(); break;
    case 0xC24E40: /* moveq   #$9, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC24E42: /* cmpi.w  #$7f, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC24E46: /* ble     $c24e4a */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24E48: /* moveq   #$7f, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24E4A: /* lsr.w   #5, D0 */
        SET_W(D(0), step_lsr_word_value(D(0), 5)); break;
    case 0xC24E4C: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC24E4E: /* blt     $c24e56 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC24E50: /* adda.w  D1, A0 */
        value = D(1); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24E52: /* dbra    D0, $c24e50 */
        step_dbf(pc, &D(0)); break;
    case 0xC24E56: /* lea     $c3fd96.l, A1 */
        A(1) = m68ki_read_imm_32(); break;
    case 0xC24E5C: /* adda.w  #$15, A1 */
        value = m68ki_read_imm_16(); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24E60: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC24E62: /* move.b  (A0)+, -(A1) */
        value = m68k_read_memory_8(A(0)); A(0) += 1; A(1) -= 1; m68k_write_memory_8(A(1), value); flags_logic_b(value); break;
    case 0xC24E64: /* dbra    D1, $c24e62 */
        step_dbf(pc, &D(1)); break;
    case 0xC24E68: /* move.w  D2, D0 */
        value = D(2); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC24E6A: /* andi.w  #$1f, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), D(0) & value); flags_logic_w(D(0)); break;
    case 0xC24E6E: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC24E70: /* cmpi.w  #$1e, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC24E74: /* ble     $c24e78 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC24E76: /* moveq   #$1e, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24E78: /* lea     $c3fd96.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC24E7E: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC24E80: /* moveq   #$15, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC24E82: /* moveq   #$2, D2 */
        D(2) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(2)); break;
    case 0xC24E84: /* bsr     $c24f76 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC24E88: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC24F76: /* move.l  D0, $c45b1e.l */
        value = D(0); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC24F7C: /* bsr     $c25a08 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC24F80: /* move.l  $c45b22.l, D1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(1) = value; flags_logic_l(value); break;
    case 0xC24F86: /* adda.w  D3, A0 */
        value = D(3); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC24F88: /* move.l  A0, D0 */
        value = A(0); D(0) = value; flags_logic_l(value); break;
    case 0xC24F8A: /* movem.l D0-D2, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC24F8E: /* jsr     $c0f56a.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC24F94: /* adda.l  #$c, A7 */
        value = m68ki_read_imm_32(); A(7) += value; break;
    case 0xC24F9A: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC24F9C: /* movea.l $c1ab74.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC24FA2: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC25A08: /* movem.l D5-D7/A6, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC25A0C: /* lea     $c45b1e.l, A6 */
        A(6) = m68ki_read_imm_32(); break;
    case 0xC25A12: /* move.l  (A6), D5 */
        value = m68k_read_memory_32(A(6)); D(5) = value; flags_logic_l(value); break;
    case 0xC25A14: /* lea     $c25a3e.l, A6 */
        A(6) = m68ki_read_imm_32(); break;
    case 0xC25A1A: /* moveq   #$0, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC25A1C: /* move.l  #$10000000, D6 */
        value = m68ki_read_imm_32(); D(6) = value; flags_logic_l(value); break;
    case 0xC25A22: /* cmp.l   (A6), D5 */
        value = m68k_read_memory_32(A(6)); result = D(5); step_compare_long(value, result); break;
    case 0xC25A24: /* bcs     $c25a2c */
        step_branch(pc, opcode, COND_CS()); break;
    case 0xC25A26: /* sub.l   (A6), D5 */
        value = m68k_read_memory_32(A(6)); step_subtract_long(&D(5), value); break;
    case 0xC25A28: /* add.l   D6, D7 */
        value = D(6); step_add_long(&D(7), value); break;
    case 0xC25A2A: /* bra     $c25a22 */
        step_branch(pc, opcode, 1); break;
    case 0xC25A2C: /* addq.l  #4, A6 */
        value = 4u; A(6) += value; break;
    case 0xC25A2E: /* lsr.l   #4, D6 */
        step_lsr_long(&D(6), 4); break;
    case 0xC25A30: /* bne     $c25a22 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC25A32: /* move.l  D7, $c45b22.l */
        value = D(7); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC25A38: /* movem.l (A7)+, D5-D7/A6 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC25A3C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C24F76_step(void) { return glue_C24E2C_step(); }

int glue_C25A08_step(void) { return glue_C24E2C_step(); }
