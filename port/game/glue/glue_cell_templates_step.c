/* Cell template expansion and record filing from control_records.c, with
 * the sorted-word lookup from stages.c. Saved register halves and byte
 * list markers retain their original widths throughout the shared body.
 * Source calls, arithmetic, bus accesses and instruction boundaries stay
 * in this CPU bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

int glue_C1D3F4_step(void) {
    uint32_t pc = REG_PC, value, address, result, temporary;
    uint16_t opcode = step_begin(pc), mask, word;
    switch (pc) {
    case 0xC1D3F4: /* movem.l D2-D5/A0-A5, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC1D3F8: /* move.w  D0, D2 */
        value = D(0); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1D3FA: /* move.w  D1, D3 */
        value = D(1); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC1D3FC: /* movea.l (-$4,A6), A0 */
        value = m68k_read_memory_32(step_displacement(A(6))); A(0) = value; break;
    case 0xC1D400: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC1D402: /* adda.w  (A0,D0.w), A0 */
        value = m68k_read_memory_16(step_indexed(A(0))); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1D406: /* tst.w   (A0) */
        value = m68k_read_memory_16(A(0)); flags_logic_w(value); break;
    case 0xC1D408: /* blt     $c1d4da */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D40C: /* asl.w   #3, D0 */
        renderer_asl_word(&D(0), 3); break;
    case 0xC1D40E: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1D410: /* move.w  D1, D4 */
        value = D(1); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC1D412: /* asr.w   #5, D6 */
        renderer_asr_word(&D(6), 5); break;
    case 0xC1D414: /* add.w   D6, D6 */
        value = D(6); step_add_word(&D(6), value); break;
    case 0xC1D416: /* add.w   D6, D6 */
        value = D(6); step_add_word(&D(6), value); break;
    case 0xC1D418: /* add.w   D0, D6 */
        value = D(0); step_add_word(&D(6), value); break;
    case 0xC1D41A: /* andi.w  #$1f, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), D(4) & value); flags_logic_w(D(4)); break;
    case 0xC1D41E: /* movea.l (-$8,A6), A5 */
        value = m68k_read_memory_32(step_displacement(A(6))); A(5) = value; break;
    case 0xC1D422: /* move.l  (A5,D6.w), D5 */
        value = m68k_read_memory_32(step_indexed(A(5))); D(5) = value; flags_logic_l(value); break;
    case 0xC1D426: /* btst    D4, D5 */
        value = D(4); value &= 31u; FLAG_Z = D(5) & (1u << value); break;
    case 0xC1D428: /* beq     $c1d4da */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D42C: /* lea     (A0), A5 */
        A(5) = A(0); break;
    case 0xC1D42E: /* adda.w  (A0), A5 */
        value = m68k_read_memory_16(A(0)); A(5) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1D430: /* addq.l  #2, A5 */
        value = 2u; A(5) += value; break;
    case 0xC1D432: /* bsr     $c1d4e4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC1D436: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC1D438: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC1D43A: /* movea.l (A5,D0.w), A5 */
        value = m68k_read_memory_32(step_indexed(A(5))); A(5) = value; break;
    case 0xC1D43E: /* move.b  #$ff, D1 */
        value = m68ki_read_imm_16(); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC1D442: /* move.b  (A5)+, D0 */
        value = m68k_read_memory_8(A(5)); A(5) += 1; SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC1D444: /* cmpi.b  #-$1, D0 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(0)); break;
    case 0xC1D448: /* beq     $c1d4d6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D44C: /* move.b  D0, D6 */
        value = D(0); SET_B(D(6), value); flags_logic_b(value); break;
    case 0xC1D44E: /* andi.b  #$f, D0 */
        value = m68ki_read_imm_16(); SET_B(D(0), D(0) & value); flags_logic_b(D(0)); break;
    case 0xC1D452: /* cmp.b   D0, D1 */
        value = D(0); step_compare_byte(value, D(1)); break;
    case 0xC1D454: /* beq     $c1d47c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D456: /* bsr     $c1d520 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC1D45A: /* moveq   #$10, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC1D45C: /* move.b  D0, D1 */
        value = D(0); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC1D45E: /* ext.w   D0 */
        SET_W(D(0), (int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC1D460: /* asl.w   #5, D0 */
        renderer_asl_word(&D(0), 5); break;
    case 0xC1D462: /* move.w  D0, D7 */
        value = D(0); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1D464: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC1D466: /* add.w   D7, D0 */
        value = D(7); step_add_word(&D(0), value); break;
    case 0xC1D468: /* lea     (A1,D0.w), A2 */
        A(2) = step_indexed(A(1)); break;
    case 0xC1D46C: /* move.l  A2, $c45a2e.l */
        value = A(2); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1D472: /* addi.l  #$5d, $c45a2e.l */
        value = m68ki_read_imm_32(); address = m68ki_read_imm_32(); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC1D47C: /* subq.b  #1, D5 */
        value = 1u; step_subtract_byte(&D(5), value); break;
    case 0xC1D47E: /* blt     $c1d4c6 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D480: /* cmpa.l  $c45a2e.l, A2 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_compare_long(value, A(2)); break;
    case 0xC1D486: /* bge     $c1d4d6 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1D488: /* move.b  (A5)+, D7 */
        value = m68k_read_memory_8(A(5)); A(5) += 1; SET_B(D(7), value); flags_logic_b(value); break;
    case 0xC1D48A: /* move.b  D7, D0 */
        value = D(7); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC1D48C: /* andi.b  #$80, D7 */
        value = m68ki_read_imm_16(); SET_B(D(7), D(7) & value); flags_logic_b(D(7)); break;
    case 0xC1D490: /* move.b  D7, (A2)+ */
        value = D(7); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D492: /* andi.b  #$7f, D0 */
        value = m68ki_read_imm_16(); SET_B(D(0), D(0) & value); flags_logic_b(D(0)); break;
    case 0xC1D496: /* move.b  D0, (A2)+ */
        value = D(0); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D498: /* btst    #$7, D7 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(7) & (1u << value); break;
    case 0xC1D49C: /* beq     $c1d4bc */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D49E: /* lsr.b   #4, D6 */
        value = (uint8_t)D(6); SET_B(D(6), value >> 4); flags_logic_b(D(6)); FLAG_X = FLAG_C = ((value >> (4 - 1)) & 1u) << 8; USE_CYCLES(4 << CYC_SHIFT); break;
    case 0xC1D4A0: /* andi.w  #$f, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), D(6) & value); flags_logic_w(D(6)); break;
    case 0xC1D4A4: /* add.w   D6, D6 */
        value = D(6); step_add_word(&D(6), value); break;
    case 0xC1D4A6: /* lea     $c1d8b6.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC1D4AC: /* move.b  (A3,D6.w), D0 */
        value = m68k_read_memory_8(step_indexed(A(3))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC1D4B0: /* ext.w   D0 */
        SET_W(D(0), (int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC1D4B2: /* move.w  D0, (A2)+ */
        value = D(0); step_write_word(A(2), value); A(2) += 2; flags_logic_w(value); break;
    case 0xC1D4B4: /* move.b  ($1,A3,D6.w), D0 */
        value = m68k_read_memory_8(step_indexed(A(3))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC1D4B8: /* move.w  D0, (A2)+ */
        value = D(0); step_write_word(A(2), value); A(2) += 2; flags_logic_w(value); break;
    case 0xC1D4BA: /* bra     $c1d4be */
        step_branch(pc, opcode, 1); break;
    case 0xC1D4BC: /* move.l  (A5)+, (A2)+ */
        value = m68k_read_memory_32(A(5)); A(5) += 4; step_write_long(A(2), value); A(2) += 4; flags_logic_l(value); break;
    case 0xC1D4BE: /* move.b  #$ff, (A2) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(2), value); flags_logic_b(value); break;
    case 0xC1D4C2: /* bra     $c1d442 */
        step_branch(pc, opcode, 1); break;
    case 0xC1D4C6: /* move.w  #$38, $c4599e.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1D4CE: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1D4D4: /* bra     $c1d4c6 */
        step_branch(pc, opcode, 1); break;
    case 0xC1D4D6: /* bsr     $c1d520 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC1D4DA: /* bsr     $c1d5d8 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC1D4DE: /* movem.l (A7)+, D2-D5/A0-A5 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC1D4E2: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1D4E4: /* moveq   #$0, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC1D4E6: /* move.w  (A0)+, D7 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1D4E8: /* asr.w   #1, D7 */
        renderer_asr_word(&D(7), 1); break;
    case 0xC1D4EA: /* move.w  D7, D0 */
        value = D(7); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1D4EC: /* sub.w   D6, D0 */
        value = D(6); step_subtract_word(&D(0), value); break;
    case 0xC1D4EE: /* blt     $c1d50e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D4F0: /* asr.w   #1, D0 */
        renderer_asr_word(&D(0), 1); break;
    case 0xC1D4F2: /* add.w   D6, D0 */
        value = D(6); step_add_word(&D(0), value); break;
    case 0xC1D4F4: /* move.w  D0, D4 */
        value = D(0); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC1D4F6: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC1D4F8: /* cmp.w   (A0,D4.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(0))); step_compare_word(value, D(1)); break;
    case 0xC1D4FC: /* beq     $c1d50c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D4FE: /* blt     $c1d506 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D500: /* move.w  D0, D6 */
        value = D(0); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1D502: /* addq.w  #1, D6 */
        value = 1u; step_add_word(&D(6), value); break;
    case 0xC1D504: /* bra     $c1d4ea */
        step_branch(pc, opcode, 1); break;
    case 0xC1D506: /* move.w  D0, D7 */
        value = D(0); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1D508: /* subq.w  #1, D7 */
        value = 1u; step_subtract_word(&D(7), value); break;
    case 0xC1D50A: /* bra     $c1d4ea */
        step_branch(pc, opcode, 1); break;
    case 0xC1D50C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1D50E: /* move.w  #$1c, $c4599e.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1D516: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1D51C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1D51E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1D520: /* tst.b   D1 */
        value = D(1); flags_logic_b(value); break;
    case 0xC1D522: /* blt     $c1d51e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D524: /* tst.b   $c45864.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC1D52A: /* beq     $c1d51e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D52C: /* move.w  #$50, $c45ad4.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1D534: /* move.l  D6, -(A7) */
        value = D(6); A(7) -= 4; m68k_write_memory_16(A(7) + 2, value); m68k_write_memory_16(A(7), value >> 16); flags_logic_l(value); break;
    case 0xC1D536: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1D53C: /* moveq   #$0, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC1D53E: /* btst    #$6, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC1D544: /* beq     $c1d570 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D546: /* btst    #$4, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC1D54C: /* beq     $c1d570 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D54E: /* cmp.w   ($6,A0), D3 */
        value = m68k_read_memory_16(step_displacement(A(0))); step_compare_word(value, D(3)); break;
    case 0xC1D552: /* bne     $c1d570 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D554: /* cmp.w   ($8,A0), D2 */
        value = m68k_read_memory_16(step_displacement(A(0))); step_compare_word(value, D(2)); break;
    case 0xC1D558: /* bne     $c1d570 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D55A: /* cmp.b   ($a,A0), D1 */
        value = m68k_read_memory_8(step_displacement(A(0))); step_compare_byte(value, D(1)); break;
    case 0xC1D55E: /* bne     $c1d570 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D560: /* bclr    #$4, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~value); break;
    case 0xC1D566: /* move.b  #$10, (A2)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D56A: /* move.b  D6, (A2)+ */
        value = D(6); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D56C: /* move.b  #$ff, (A2) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(2), value); flags_logic_b(value); break;
    case 0xC1D570: /* adda.w  #$200, A0 */
        value = m68ki_read_imm_16(); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1D574: /* addq.b  #1, D6 */
        value = 1u; renderer_add_byte(&D(6), value); break;
    case 0xC1D576: /* cmpi.b  #$10, D6 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(6)); break;
    case 0xC1D57A: /* blt     $c1d53e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D57C: /* lea     $c48184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1D582: /* moveq   #$0, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC1D584: /* btst    #$6, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC1D58A: /* beq     $c1d5b6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D58C: /* btst    #$4, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC1D592: /* beq     $c1d5b6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D594: /* cmp.w   ($6,A0), D3 */
        value = m68k_read_memory_16(step_displacement(A(0))); step_compare_word(value, D(3)); break;
    case 0xC1D598: /* bne     $c1d5b6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D59A: /* cmp.w   ($8,A0), D2 */
        value = m68k_read_memory_16(step_displacement(A(0))); step_compare_word(value, D(2)); break;
    case 0xC1D59E: /* bne     $c1d5b6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D5A0: /* cmp.b   ($a,A0), D1 */
        value = m68k_read_memory_8(step_displacement(A(0))); step_compare_byte(value, D(1)); break;
    case 0xC1D5A4: /* bne     $c1d5b6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D5A6: /* bclr    #$4, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~value); break;
    case 0xC1D5AC: /* move.b  #$40, (A2)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D5B0: /* move.b  D6, (A2)+ */
        value = D(6); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D5B2: /* move.b  #$ff, (A2) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(2), value); flags_logic_b(value); break;
    case 0xC1D5B6: /* adda.w  #$20, A0 */
        value = m68ki_read_imm_16(); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1D5BA: /* addq.b  #1, D6 */
        value = 1u; renderer_add_byte(&D(6), value); break;
    case 0xC1D5BC: /* cmpi.b  #$10, D6 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(6)); break;
    case 0xC1D5C0: /* blt     $c1d584 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D5C2: /* move.l  (A7)+, D6 */
        value = m68k_read_memory_32(A(7)); A(7) += 4; D(6) = value; flags_logic_l(value); break;
    case 0xC1D5C4: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1D5C6: /* move.w  #$e, $c4599e.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1D5CE: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1D5D4: /* bra     $c1d5c6 */
        step_branch(pc, opcode, 1); break;
    case 0xC1D5D6: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1D5D8: /* tst.b   $c45864.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC1D5DE: /* beq     $c1d5d6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D5E0: /* move.w  #$51, $c45ad4.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1D5E8: /* move.b  #$ff, D1 */
        value = m68ki_read_imm_16(); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC1D5EC: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1D5F2: /* moveq   #$0, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC1D5F4: /* btst    #$6, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC1D5FA: /* beq     $c1d66e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D5FC: /* btst    #$4, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC1D602: /* beq     $c1d66e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D604: /* cmp.w   ($6,A0), D3 */
        value = m68k_read_memory_16(step_displacement(A(0))); step_compare_word(value, D(3)); break;
    case 0xC1D608: /* bne     $c1d66e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D60A: /* cmp.w   ($8,A0), D2 */
        value = m68k_read_memory_16(step_displacement(A(0))); step_compare_word(value, D(2)); break;
    case 0xC1D60E: /* bne     $c1d66e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D610: /* move.b  ($a,A0), D4 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1D614: /* cmp.b   D4, D1 */
        value = D(4); step_compare_byte(value, D(1)); break;
    case 0xC1D616: /* beq     $c1d65e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D618: /* move.b  D4, D1 */
        value = D(4); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC1D61A: /* blt     $c1d5c6 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D61C: /* ext.w   D4 */
        SET_W(D(4), (int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case 0xC1D61E: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC1D620: /* asl.w   #5, D4 */
        renderer_asl_word(&D(4), 5); break;
    case 0xC1D622: /* move.w  D4, D1 */
        value = D(4); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1D624: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC1D626: /* add.w   D1, D4 */
        value = D(1); step_add_word(&D(4), value); break;
    case 0xC1D628: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC1D62A: /* lea     (A1,D4.w), A2 */
        A(2) = step_indexed(A(1)); break;
    case 0xC1D62E: /* move.l  A2, $c45a2e.l */
        value = A(2); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1D634: /* addi.l  #$5d, $c45a2e.l */
        value = m68ki_read_imm_32(); address = m68ki_read_imm_32(); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC1D63E: /* cmpa.l  $c45a2e.l, A2 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_compare_long(value, A(2)); break;
    case 0xC1D644: /* bge     $c1d5d6 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1D646: /* move.b  (A2), D4 */
        value = m68k_read_memory_8(A(2)); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1D648: /* blt     $c1d65e */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D64A: /* btst    #$4, D4 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(4) & (1u << value); break;
    case 0xC1D64E: /* bne     $c1d65a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D650: /* btst    #$6, D4 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(4) & (1u << value); break;
    case 0xC1D654: /* bne     $c1d65a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D656: /* addq.w  #4, A2 */
        value = 4u; A(2) += value; break;
    case 0xC1D658: /* bra     $c1d63e */
        step_branch(pc, opcode, 1); break;
    case 0xC1D65A: /* addq.w  #2, A2 */
        value = 2u; A(2) += value; break;
    case 0xC1D65C: /* bra     $c1d63e */
        step_branch(pc, opcode, 1); break;
    case 0xC1D65E: /* bclr    #$4, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~value); break;
    case 0xC1D664: /* move.b  #$10, (A2)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D668: /* move.b  D6, (A2)+ */
        value = D(6); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D66A: /* move.b  #$ff, (A2) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(2), value); flags_logic_b(value); break;
    case 0xC1D66E: /* adda.w  #$200, A0 */
        value = m68ki_read_imm_16(); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1D672: /* addq.b  #1, D6 */
        value = 1u; renderer_add_byte(&D(6), value); break;
    case 0xC1D674: /* cmpi.b  #$10, D6 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(6)); break;
    case 0xC1D678: /* blt     $c1d5f4 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D67C: /* move.b  #$ff, D1 */
        value = m68ki_read_imm_16(); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC1D680: /* lea     $c48184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1D686: /* moveq   #$0, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC1D688: /* btst    #$6, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC1D68E: /* beq     $c1d702 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D690: /* btst    #$4, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC1D696: /* beq     $c1d702 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D698: /* cmp.w   ($6,A0), D3 */
        value = m68k_read_memory_16(step_displacement(A(0))); step_compare_word(value, D(3)); break;
    case 0xC1D69C: /* bne     $c1d702 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D69E: /* cmp.w   ($8,A0), D2 */
        value = m68k_read_memory_16(step_displacement(A(0))); step_compare_word(value, D(2)); break;
    case 0xC1D6A2: /* bne     $c1d702 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D6A4: /* move.b  ($a,A0), D4 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1D6A8: /* cmp.b   D4, D1 */
        value = D(4); step_compare_byte(value, D(1)); break;
    case 0xC1D6AA: /* beq     $c1d6f2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D6AC: /* move.b  D4, D1 */
        value = D(4); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC1D6AE: /* blt     $c1d712 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D6B0: /* ext.w   D4 */
        SET_W(D(4), (int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case 0xC1D6B2: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC1D6B4: /* asl.w   #5, D4 */
        renderer_asl_word(&D(4), 5); break;
    case 0xC1D6B6: /* move.w  D4, D1 */
        value = D(4); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1D6B8: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC1D6BA: /* add.w   D1, D4 */
        value = D(1); step_add_word(&D(4), value); break;
    case 0xC1D6BC: /* swap    D1 */
        step_swap(&D(1)); break;
    case 0xC1D6BE: /* lea     (A1,D4.w), A2 */
        A(2) = step_indexed(A(1)); break;
    case 0xC1D6C2: /* move.l  A2, $c45a2e.l */
        value = A(2); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1D6C8: /* addi.l  #$5d, $c45a2e.l */
        value = m68ki_read_imm_32(); address = m68ki_read_imm_32(); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC1D6D2: /* cmpa.l  $c45a2e.l, A2 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_compare_long(value, A(2)); break;
    case 0xC1D6D8: /* bge     $c1d710 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1D6DA: /* move.b  (A2), D4 */
        value = m68k_read_memory_8(A(2)); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1D6DC: /* blt     $c1d6f2 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D6DE: /* btst    #$4, D4 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(4) & (1u << value); break;
    case 0xC1D6E2: /* bne     $c1d6ee */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D6E4: /* btst    #$6, D4 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(4) & (1u << value); break;
    case 0xC1D6E8: /* bne     $c1d6ee */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D6EA: /* addq.w  #4, A2 */
        value = 4u; A(2) += value; break;
    case 0xC1D6EC: /* bra     $c1d6d2 */
        step_branch(pc, opcode, 1); break;
    case 0xC1D6EE: /* addq.w  #2, A2 */
        value = 2u; A(2) += value; break;
    case 0xC1D6F0: /* bra     $c1d6d2 */
        step_branch(pc, opcode, 1); break;
    case 0xC1D6F2: /* bclr    #$4, ($1,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result & ~value); break;
    case 0xC1D6F8: /* move.b  #$40, (A2)+ */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D6FC: /* move.b  D6, (A2)+ */
        value = D(6); m68k_write_memory_8(A(2), value); A(2) += 1; flags_logic_b(value); break;
    case 0xC1D6FE: /* move.b  #$ff, (A2) */
        value = m68ki_read_imm_16(); m68k_write_memory_8(A(2), value); flags_logic_b(value); break;
    case 0xC1D702: /* adda.w  #$20, A0 */
        value = m68ki_read_imm_16(); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1D706: /* addq.b  #1, D6 */
        value = 1u; renderer_add_byte(&D(6), value); break;
    case 0xC1D708: /* cmpi.b  #$10, D6 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(6)); break;
    case 0xC1D70C: /* blt     $c1d688 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1D710: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1D712: /* move.w  #$36, $c4599e.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1D71A: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1D720: /* bra     $c1d712 */
        step_branch(pc, opcode, 1); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C1D4E4_step(void) { return glue_C1D3F4_step(); }

int glue_C1D520_step(void) { return glue_C1D3F4_step(); }

int glue_C1D5D8_step(void) { return glue_C1D3F4_step(); }
