/* Source timing for engine/noise/alert starts and their random-bit helpers.
 * Readable sound and random operations remain in audio.c and fixed_math.c.
 * Every case retains one original instruction and its child/event boundary. */
#include "glue_step.h"

int glue_C17E4A_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC17E4Au || pc >= 0xC17EF2u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC17E4A: /* link    A6, #-$4 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC17E4E: /* btst    #$4, $c45b5b.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC17E56: /* beq     $c17edc */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17E5A: /* tst.l   $c0a45c.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC17E60: /* beq     $c17eee */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17E64: /* movea.l $c0a458.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC17E6A: /* moveq   #$6, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17E6C: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17E6E: /* move.l  A0, ($4,A7) */
        value = A(0); m68k_write_memory_32(step_displacement(A(7)), value); flags_logic_l(value); break;
    case 0xC17E72: /* jsr     $c50b02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC17E78: /* addq.l  #4, A7 */
        A(7) += 4u; break;
    case 0xC17E7A: /* addi.l  #$168, D0 */
        value = m68ki_read_imm_32(); step_add_long(&D(0), value); break;
    case 0xC17E80: /* moveq   #$10, D1 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17E82: /* asl.l   D1, D0 */
        step_asl_long(&D(0), D(1)); break;
    case 0xC17E84: /* movea.l ($0,A7), A0 */
        value = m68k_read_memory_32(step_displacement(A(7))); A(0) = value; break;
    case 0xC17E88: /* move.l  D0, ($8,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17E8C: /* movea.l $c0a45c.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC17E92: /* moveq   #$6, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17E94: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17E96: /* move.l  A0, ($4,A7) */
        value = A(0); m68k_write_memory_32(step_displacement(A(7)), value); flags_logic_l(value); break;
    case 0xC17E9A: /* jsr     $c50b02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC17EA0: /* addq.l  #4, A7 */
        A(7) += 4u; break;
    case 0xC17EA2: /* addi.l  #$168, D0 */
        value = m68ki_read_imm_32(); step_add_long(&D(0), value); break;
    case 0xC17EA8: /* moveq   #$10, D1 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17EAA: /* asl.l   D1, D0 */
        step_asl_long(&D(0), D(1)); break;
    case 0xC17EAC: /* movea.l ($0,A7), A0 */
        value = m68k_read_memory_32(step_displacement(A(7))); A(0) = value; break;
    case 0xC17EB0: /* move.l  D0, ($8,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17EB4: /* move.l  ($8,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17EB8: /* clr.l   -(A7) */
        value = 0; step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17EBA: /* moveq   #$8, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17EBC: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17EBE: /* bsr     $c17b2c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17EC2: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC17EC6: /* move.l  ($8,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17ECA: /* moveq   #$1, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17ECC: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17ECE: /* moveq   #$9, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17ED0: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17ED2: /* bsr     $c17b2c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17ED6: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC17EDA: /* bra     $c17eee */
        step_branch(pc, opcode, 1); break;
    case 0xC17EDC: /* clr.l   -(A7) */
        value = 0; step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17EDE: /* bsr     $c17b08 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17EE2: /* addq.l  #4, A7 */
        A(7) += 4u; break;
    case 0xC17EE4: /* moveq   #$1, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17EE6: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17EE8: /* bsr     $c17b08 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17EEC: /* addq.l  #4, A7 */
        A(7) += 4u; break;
    case 0xC17EEE: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC17EF0: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C17CF6_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC17CF6u || pc >= 0xC17D6Eu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC17CF6: /* link    A6, #$0 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC17CFA: /* btst    #$1, $c45b5b.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC17D02: /* beq     $c17d6a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17D04: /* tst.l   $c0a444.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC17D0A: /* beq     $c17d6a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17D0C: /* movea.l $c0a440.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC17D12: /* movea.l $c0a444.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC17D18: /* moveq   #$0, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17D1A: /* move.l  D0, ($1c,A1) */
        value = D(0); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17D1E: /* move.l  D0, ($1c,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17D22: /* move.l  D0, ($18,A1) */
        value = D(0); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17D26: /* move.l  D0, ($18,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17D2A: /* moveq   #$10, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17D2C: /* move.l  ($8,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC17D30: /* asl.l   D0, D1 */
        step_asl_long(&D(1), D(0)); break;
    case 0xC17D32: /* move.l  D1, ($8,A0) */
        value = D(1); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17D36: /* move.l  ($8,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC17D3A: /* addq.l  #2, D0 */
        value = 2u; step_add_long(&D(0), value); break;
    case 0xC17D3C: /* moveq   #$10, D1 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17D3E: /* asl.l   D1, D0 */
        step_asl_long(&D(0), D(1)); break;
    case 0xC17D40: /* move.l  D0, ($8,A1) */
        value = D(0); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17D44: /* move.l  ($c,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D48: /* clr.l   -(A7) */
        value = 0; step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D4A: /* moveq   #$2, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17D4C: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D4E: /* bsr     $c17b2c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17D52: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC17D56: /* move.l  ($c,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D5A: /* moveq   #$1, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17D5C: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D5E: /* moveq   #$3, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17D60: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D62: /* bsr     $c17b2c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17D66: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC17D6A: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC17D6C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C17DAA_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC17DAAu || pc >= 0xC17E4Au) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC17DAA: /* link    A6, #$0 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC17DAE: /* movem.l D2, -(A7) */
        step_save_registers(); break;
    case 0xC17DB2: /* btst    #$1, $c45b5b.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC17DBA: /* beq     $c17e42 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17DBE: /* tst.l   $c4fe3c.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC17DC4: /* beq     $c17e42 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17DC6: /* movea.l $c4fe3c.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC17DCC: /* moveq   #$10, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17DCE: /* move.l  ($8,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC17DD2: /* asl.l   D0, D1 */
        step_asl_long(&D(1), D(0)); break;
    case 0xC17DD4: /* movea.l $c4fe38.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC17DDA: /* sub.l   ($8,A1), D1 */
        value = m68k_read_memory_32(step_displacement(A(1))); step_subtract_long(&D(1), value); break;
    case 0xC17DDE: /* move.l  D1, D0 */
        value = D(1); D(0) = value; flags_logic_l(value); break;
    case 0xC17DE0: /* move.l  ($10,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC17DE4: /* jsr     $c52ec8.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC17DEA: /* move.l  D0, ($18,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17DEE: /* movea.l $c4fe38.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC17DF4: /* move.l  D0, ($18,A1) */
        value = D(0); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17DF8: /* move.l  ($10,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC17DFC: /* move.l  D0, ($38,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17E00: /* movea.l $c4fe38.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC17E06: /* move.l  D0, ($38,A1) */
        value = D(0); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17E0A: /* moveq   #$10, D1 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17E0C: /* move.l  ($c,A6), D2 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(2) = value; flags_logic_l(value); break;
    case 0xC17E10: /* asl.l   D1, D2 */
        step_asl_long(&D(2), D(1)); break;
    case 0xC17E12: /* sub.l   ($c,A1), D2 */
        value = m68k_read_memory_32(step_displacement(A(1))); step_subtract_long(&D(2), value); break;
    case 0xC17E16: /* move.l  D2, D0 */
        value = D(2); D(0) = value; flags_logic_l(value); break;
    case 0xC17E18: /* move.l  ($10,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC17E1C: /* jsr     $c52ec8.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC17E22: /* move.l  D0, ($1c,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17E26: /* movea.l $c4fe38.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC17E2C: /* move.l  D0, ($1c,A1) */
        value = D(0); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17E30: /* move.l  ($10,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC17E34: /* move.l  D0, ($3c,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17E38: /* movea.l $c4fe38.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC17E3E: /* move.l  D0, ($3c,A1) */
        value = D(0); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17E42: /* movem.l (A7)+, D2 */
        step_restore_registers(); break;
    case 0xC17E46: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC17E48: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C17C62_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC17C62u || pc >= 0xC17CF6u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC17C62: /* link    A6, #$0 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC17C66: /* btst    #$0, $c45b5b.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC17C6E: /* beq     $c17ce2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17C70: /* tst.l   $c0a43c.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC17C76: /* beq     $c17cf2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17C78: /* movea.l $c0a43c.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC17C7E: /* moveq   #$0, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17C80: /* move.l  D0, ($1c,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17C84: /* movea.l $c0a438.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC17C8A: /* move.l  D0, ($1c,A1) */
        value = D(0); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17C8E: /* move.l  D0, ($18,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17C92: /* movea.l $c0a438.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC17C98: /* move.l  D0, ($18,A1) */
        value = D(0); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17C9C: /* moveq   #$10, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17C9E: /* move.l  ($8,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC17CA2: /* asl.l   D0, D1 */
        step_asl_long(&D(1), D(0)); break;
    case 0xC17CA4: /* movea.l $c0a438.l, A1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(1) = value; break;
    case 0xC17CAA: /* move.l  D1, ($8,A1) */
        value = D(1); m68k_write_memory_32(step_displacement(A(1)), value); flags_logic_l(value); break;
    case 0xC17CAE: /* move.l  ($8,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC17CB2: /* addq.l  #2, D0 */
        value = 2u; step_add_long(&D(0), value); break;
    case 0xC17CB4: /* moveq   #$10, D1 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17CB6: /* asl.l   D1, D0 */
        step_asl_long(&D(0), D(1)); break;
    case 0xC17CB8: /* move.l  D0, ($8,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC17CBC: /* move.l  ($c,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17CC0: /* moveq   #$0, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17CC2: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17CC4: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17CC6: /* bsr     $c17b2c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17CCA: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC17CCE: /* move.l  ($c,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17CD2: /* moveq   #$1, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC17CD4: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17CD6: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17CD8: /* bsr     $c17b2c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17CDC: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC17CE0: /* bra     $c17cf2 */
        step_branch(pc, opcode, 1); break;
    case 0xC17CE2: /* move.l  ($c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC17CE6: /* asr.l   #2, D0 */
        step_asr_long(&D(0), 2u); break;
    case 0xC17CE8: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17CEA: /* move.l  ($8,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17CEE: /* bsr     $c17cf6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17CF0: /* addq.l  #8, A7 */
        A(7) += 8u; break;
    case 0xC17CF2: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC17CF4: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C17D6E_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC17D6Eu || pc >= 0xC17DAAu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC17D6E: /* link    A6, #$0 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC17D72: /* btst    #$0, $c45b5b.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC17D7A: /* beq     $c17d90 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC17D7C: /* move.l  ($10,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D80: /* move.l  ($c,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D84: /* move.l  ($8,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D88: /* bsr     $c17daa */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17D8A: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC17D8E: /* bra     $c17da6 */
        step_branch(pc, opcode, 1); break;
    case 0xC17D90: /* move.l  ($c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC17D94: /* asr.l   #2, D0 */
        step_asr_long(&D(0), 2u); break;
    case 0xC17D96: /* move.l  ($10,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D9A: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17D9C: /* move.l  ($8,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC17DA0: /* bsr     $c17daa */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC17DA2: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC17DA6: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC17DA8: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C18096_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC18096u || pc >= 0xC180FCu) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC18096: /* link    A6, #$0 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC1809A: /* btst    #$6, $c45b5b.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC180A2: /* beq     $c180f8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC180A4: /* move.w  $c459b6.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC180AA: /* move.w  $c458de.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC180B0: /* cmp.w   D1, D0 */
        step_compare_word(D(1), D(0)); break;
    case 0xC180B2: /* bne     $c180f8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC180B4: /* tst.l   $c0a464.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC180BA: /* beq     $c180f8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC180BC: /* moveq   #$2, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC180BE: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC180C0: /* bsr     $c17b08 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC180C4: /* addq.l  #4, A7 */
        A(7) += 4u; break;
    case 0xC180C6: /* movea.l $c0a464.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC180CC: /* moveq   #$0, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC180CE: /* move.l  D0, ($2c,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC180D2: /* move.l  #$c50c00, ($30,A0) */
        value = m68ki_read_imm_32(); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC180DA: /* move.l  D0, ($34,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC180DE: /* moveq   #$1, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC180E0: /* move.l  D0, ($2c,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC180E4: /* move.l  ($8,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC180E8: /* moveq   #$2, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC180EA: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC180EC: /* moveq   #$b, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC180EE: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC180F0: /* bsr     $c17b2c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC180F4: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC180F8: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC180FA: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C1803C_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC1803Cu || pc >= 0xC18096u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC1803C: /* link    A6, #$0 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC18040: /* btst    #$2, $c45b5b.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC18048: /* beq     $c18092 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1804A: /* move.w  $c459b6.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC18050: /* move.w  $c458de.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC18056: /* cmp.w   D1, D0 */
        step_compare_word(D(1), D(0)); break;
    case 0xC18058: /* bne     $c18092 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1805A: /* tst.l   $c0a44c.l */
        value = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(value); break;
    case 0xC18060: /* beq     $c18092 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC18062: /* movea.l $c0a44c.l, A0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(0) = value; break;
    case 0xC18068: /* move.l  #$1360000, ($8,A0) */
        value = m68ki_read_imm_32(); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC18070: /* move.l  #$fffe0000, ($18,A0) */
        value = m68ki_read_imm_32(); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC18078: /* moveq   #$1, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC1807A: /* move.l  D0, ($10,A0) */
        value = D(0); m68k_write_memory_32(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC1807E: /* move.l  ($8,A6), -(A7) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC18082: /* moveq   #$2, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC18084: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC18086: /* moveq   #$5, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC18088: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC1808A: /* bsr     $c17b2c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC1808E: /* lea     ($c,A7), A7 */
        A(7) = step_displacement(A(7)); break;
    case 0xC18092: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC18094: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C180FC_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC180FCu || pc >= 0xC18108u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC180FC: /* moveq   #$2, D0 */
        D((opcode >> 9) & 7u) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D((opcode >> 9) & 7u)); break;
    case 0xC180FE: /* move.l  D0, -(A7) */
        value = D(0); step_predecrement_long(value); flags_logic_l(value); break;
    case 0xC18100: /* bsr     $c17b08 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC18104: /* addq.l  #4, A7 */
        A(7) += 4u; break;
    case 0xC18106: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C50AB4_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC50AB4u || pc >= 0xC50B02u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC50AB4: /* link    A6, #-$4 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC50AB8: /* move.l  $c07288.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC50ABE: /* andi.l  #$1, D0 */
        value = m68ki_read_imm_32(); D(0) &= value; flags_logic_l(D(0)); break;
    case 0xC50AC4: /* move.l  $c07288.l, D1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(1) = value; flags_logic_l(value); break;
    case 0xC50ACA: /* asr.l   #3, D1 */
        step_asr_long(&D(1), 3u); break;
    case 0xC50ACC: /* andi.l  #$1, D1 */
        value = m68ki_read_imm_32(); D(1) &= value; flags_logic_l(D(1)); break;
    case 0xC50AD2: /* eor.l   D1, D0 */
        value = D(1); D(0) ^= value; flags_logic_l(D(0)); break;
    case 0xC50AD4: /* move.l  $c07288.l, D1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(1) = value; flags_logic_l(value); break;
    case 0xC50ADA: /* asr.l   #1, D1 */
        step_asr_long(&D(1), 1u); break;
    case 0xC50ADC: /* andi.l  #$7fffffff, D1 */
        value = m68ki_read_imm_32(); D(1) &= value; flags_logic_l(D(1)); break;
    case 0xC50AE2: /* move.l  D1, $c07288.l */
        value = D(1); m68k_write_memory_32(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC50AE8: /* move.l  D0, (-$4,A6) */
        value = D(0); m68k_write_memory_32(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC50AEC: /* tst.l   D0 */
        value = D(0); flags_logic_l(value); break;
    case 0xC50AEE: /* beq     $c50afa */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC50AF0: /* subi.l  #-$80000000, $c07288.l */
        value = m68ki_read_imm_32(); address = m68ki_read_imm_32(); result = m68k_read_memory_32(address); step_subtract_long(&result, value); m68k_write_memory_32(address, result); break;
    case 0xC50AFA: /* move.l  (-$4,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC50AFE: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC50B00: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C50B02_step(void) {
    uint32_t pc = REG_PC, value, result, address;
    uint16_t opcode;
    if (pc < 0xC50B02u || pc >= 0xC50B36u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC50B02: /* link    A6, #-$8 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC50B06: /* clr.l   (-$4,A6) */
        value = 0; m68k_write_memory_32(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC50B0A: /* move.l  ($8,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC50B0E: /* subq.l  #1, ($8,A6) */
        value = 1u; address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_subtract_long(&result, value); m68k_write_memory_32(address, result); break;
    case 0xC50B12: /* tst.l   D0 */
        value = D(0); flags_logic_l(value); break;
    case 0xC50B14: /* ble     $c50b2e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC50B16: /* move.l  (-$4,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC50B1A: /* asl.l   #1, D0 */
        step_asl_long(&D(0), 1u); break;
    case 0xC50B1C: /* move.l  D0, ($0,A7) */
        value = D(0); m68k_write_memory_32(step_displacement(A(7)), value); flags_logic_l(value); break;
    case 0xC50B20: /* bsr     $c50ab4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC50B22: /* move.l  ($0,A7), D1 */
        value = m68k_read_memory_32(step_displacement(A(7))); D(1) = value; flags_logic_l(value); break;
    case 0xC50B26: /* add.l   D0, D1 */
        value = D(0); step_add_long(&D(1), value); break;
    case 0xC50B28: /* move.l  D1, (-$4,A6) */
        value = D(1); m68k_write_memory_32(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC50B2C: /* bra     $c50b0a */
        step_branch(pc, opcode, 1); break;
    case 0xC50B2E: /* move.l  (-$4,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC50B32: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC50B34: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
