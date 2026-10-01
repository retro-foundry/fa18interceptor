/* Message sequence reset and long-table clear; stages.c.
 * CPU register/flag effects, bus accesses and source instruction boundaries
 * stay in this bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

int glue_C11312_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC11312: /* link    A6, #-$4 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC11316: /* lea     $c4574a.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1131C: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC1131E: /* move.w  D0, (A0) */
        value = D(0); step_write_word(A(0), value); flags_logic_w(value); break;
    case 0xC11320: /* addq.l  #2, A0 */
        value = 2u; A(0) += value; break;
    case 0xC11322: /* move.w  D0, (A0) */
        value = D(0); step_write_word(A(0), value); flags_logic_w(value); break;
    case 0xC11324: /* addq.l  #2, A0 */
        value = 2u; A(0) += value; break;
    case 0xC11326: /* move.l  #$1b8, $c4573e.l */
        value = m68ki_read_imm_32(); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC11330: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC11332: /* move.b  D0, $c457c6.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC11338: /* move.b  D0, $c457c3.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1133E: /* move.b  D0, $c457e0.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC11344: /* move.b  D0, $c45871.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1134A: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC1134C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC11B0E: /* link    A6, #-$6 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC11B12: /* move.b  #$2, $c458a4.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC11B1A: /* move.l  $c45660.l, (-$6,A6) */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC11B22: /* clr.w   (-$2,A6) */
        value = 0; step_write_word(step_displacement(A(6)), value); flags_logic_w(0); break;
    case 0xC11B26: /* cmpi.w  #$10, (-$2,A6) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_16(step_displacement(A(6))); step_compare_word(value, result); break;
    case 0xC11B2C: /* bge     $c11b3e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC11B2E: /* movea.l (-$6,A6), A0 */
        value = m68k_read_memory_32(step_displacement(A(6))); A(0) = value; break;
    case 0xC11B32: /* clr.l   (A0) */
        value = 0; step_write_long(A(0), value); flags_logic_l(0); break;
    case 0xC11B34: /* addq.l  #4, (-$6,A6) */
        value = 4u; address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC11B38: /* addq.w  #1, (-$2,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_16(address); step_add_word(&result, value); step_write_word(address, result); break;
    case 0xC11B3C: /* bra     $c11b26 */
        step_branch(pc, opcode, 1); break;
    case 0xC11B3E: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC11B40: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C11B0E_step(void) { return glue_C11312_step(); }
