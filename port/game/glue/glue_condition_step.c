/* Terrain condition-table key/value matching and result bytes; fixed_math.c.
 * Source CPU effects and instruction/bus/event boundaries stay in glue. */
#include "glue_renderer_step_math.h"

int glue_C09A78_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC09A78: /* lea     $c09b48.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC09A7E: /* bsr     $c09ab8 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC09A82: /* beq     $c09a8e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC09A84: /* move.b  #$1, $c4589b.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC09A8C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC09A8E: /* move.b  #$0, $c4589b.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC09A96: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC09A98: /* lea     $c09d78.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC09A9E: /* bsr     $c09ab8 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC09AA2: /* beq     $c09aae */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC09AA4: /* move.b  #$1, $c4589c.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC09AAC: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC09AAE: /* move.b  #$0, $c4589c.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC09AB6: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC09AB8: /* move.w  $c45948.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC09ABE: /* move.w  $c4594a.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC09AC4: /* lea     (A0), A1 */
        A(1) = A(0); break;
    case 0xC09AC6: /* moveq   #-$2, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC09AC8: /* addq.w  #2, D3 */
        value = 2u; step_add_word(&D(3), value); break;
    case 0xC09ACA: /* move.w  (A0)+, D2 */
        address = A(0); A(0) += 2; value = m68k_read_memory_16(address); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC09ACC: /* blt     $c09b44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC09ACE: /* cmp.w   D2, D1 */
        value = D(2); step_compare_word(value, D(1)); break;
    case 0xC09AD0: /* bne     $c09ac8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC09AD2: /* tst.w   (A0)+ */
        address = A(0); A(0) += 2; value = m68k_read_memory_16(address); flags_logic_w(value); break;
    case 0xC09AD4: /* bge     $c09ad2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC09AD6: /* move.w  (A0,D3.w), D3 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC09ADA: /* lea     (A1,D3.w), A0 */
        A(0) = step_indexed(A(1)); break;
    case 0xC09ADE: /* moveq   #-$2, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC09AE0: /* addq.w  #2, D3 */
        value = 2u; step_add_word(&D(3), value); break;
    case 0xC09AE2: /* move.w  (A0)+, D2 */
        address = A(0); A(0) += 2; value = m68k_read_memory_16(address); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC09AE4: /* blt     $c09b44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC09AE6: /* cmp.w   D2, D0 */
        value = D(2); step_compare_word(value, D(0)); break;
    case 0xC09AE8: /* bne     $c09ae0 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC09AEA: /* tst.w   (A0)+ */
        address = A(0); A(0) += 2; value = m68k_read_memory_16(address); flags_logic_w(value); break;
    case 0xC09AEC: /* bge     $c09aea */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC09AEE: /* move.w  (A0,D3.w), D3 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC09AF2: /* lea     (A1,D3.w), A0 */
        A(0) = step_indexed(A(1)); break;
    case 0xC09AF6: /* move.l  $c45a78.l, D4 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(4) = value; flags_logic_l(value); break;
    case 0xC09AFC: /* neg.l   D4 */
        renderer_negate(&D(4), 4); break;
    case 0xC09AFE: /* tst.w   (A0) */
        value = m68k_read_memory_16(A(0)); flags_logic_w(value); break;
    case 0xC09B00: /* blt     $c09b44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC09B02: /* cmpi.w  #$0, (A0)+ */
        value = m68ki_read_imm_16(); address = A(0); A(0) += 2; result = m68k_read_memory_16(address); step_compare_word(value, result); break;
    case 0xC09B06: /* beq     $c09b0e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC09B08: /* cmp.l   (A0)+, D4 */
        address = A(0); A(0) += 4; value = m68k_read_memory_32(address); step_compare_long(value, D(4)); break;
    case 0xC09B0A: /* blt     $c09b44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC09B0C: /* bra     $c09b12 */
        step_branch(pc, opcode, 1); break;
    case 0xC09B0E: /* cmp.l   (A0)+, D4 */
        address = A(0); A(0) += 4; value = m68k_read_memory_32(address); step_compare_long(value, D(4)); break;
    case 0xC09B10: /* bgt     $c09b44 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC09B12: /* move.b  $c45850.l, D6 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(6), value); flags_logic_b(value); break;
    case 0xC09B18: /* move.b  (A0)+, D5 */
        address = A(0); A(0) += 1; value = m68k_read_memory_8(address); SET_B(D(5), value); flags_logic_b(value); break;
    case 0xC09B1A: /* blt     $c09b26 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC09B1C: /* cmp.b   D5, D6 */
        value = D(5); step_compare_byte(value, D(6)); break;
    case 0xC09B1E: /* beq     $c09b32 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC09B20: /* tst.b   (A0)+ */
        address = A(0); A(0) += 1; value = m68k_read_memory_8(address); flags_logic_b(value); break;
    case 0xC09B22: /* bge     $c09b20 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC09B24: /* bra     $c09b18 */
        step_branch(pc, opcode, 1); break;
    case 0xC09B26: /* move.w  A0, D5 */
        value = A(0); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC09B28: /* btst    #$0, D5 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(5) & (1u << value); break;
    case 0xC09B2C: /* beq     $c09afe */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC09B2E: /* addq.w  #1, A0 */
        value = 1u; A(0) += value; break;
    case 0xC09B30: /* bra     $c09afe */
        step_branch(pc, opcode, 1); break;
    case 0xC09B32: /* move.b  $c45854.l, D6 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(6), value); flags_logic_b(value); break;
    case 0xC09B38: /* move.b  (A0)+, D5 */
        address = A(0); A(0) += 1; value = m68k_read_memory_8(address); SET_B(D(5), value); flags_logic_b(value); break;
    case 0xC09B3A: /* blt     $c09b18 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC09B3C: /* cmp.b   D5, D6 */
        value = D(5); step_compare_byte(value, D(6)); break;
    case 0xC09B3E: /* bne     $c09b38 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC09B40: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC09B42: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC09B44: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC09B46: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C09A98_step(void) { return glue_C09A78_step(); }

int glue_C09AB8_step(void) { return glue_C09A78_step(); }
