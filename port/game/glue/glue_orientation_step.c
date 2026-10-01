/* Record orientation and forward/inverse matrices; matrix.c and fixed_math.c.
 * CPU register/flag effects, bus accesses and source instruction boundaries
 * stay in this bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

int glue_C2D954_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC2D954: /* movem.w D4-D6, ($66,A1) */
        mask = m68ki_read_imm_16(); renderer_store(step_displacement(A(1)), mask, 2, -1); break;
    case 0xC2D95A: /* movem.l D1/D4-D6/A1, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC2D95E: /* move.w  D4, D0 */
        value = D(4); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2D960: /* move.w  D5, D2 */
        value = D(5); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2D962: /* move.w  D6, D4 */
        value = D(6); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2D964: /* lea     ($80,A1), A1 */
        A(1) = step_displacement(A(1)); break;
    case 0xC2D968: /* bsr     $c2e47a */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2D96C: /* movem.l (A7)+, D1/D5-D7/A1 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
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
    case 0xC2E47A: /* lsr.w   #3, D0 */
        SET_W(D(0), step_lsr_word_value(D(0), 3)); break;
    case 0xC2E47C: /* lsr.w   #3, D2 */
        SET_W(D(2), step_lsr_word_value(D(2), 3)); break;
    case 0xC2E47E: /* lsr.w   #3, D4 */
        SET_W(D(4), step_lsr_word_value(D(4), 3)); break;
    case 0xC2E480: /* bsr     $c2e5f6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E484: /* bsr     $c2e6da */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E488: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E48A: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E48C: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E48E: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E490: /* muls.w  D4, D6 */
        value = D(4); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E492: /* move.w  D3, D7 */
        value = D(3); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E494: /* muls.w  D5, D7 */
        value = D(5); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E496: /* add.l   D6, D7 */
        value = D(6); step_add_long(&D(7), value); break;
    case 0xC2E498: /* moveq   #$e, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC2E49A: /* asr.l   D6, D7 */
        step_asr_long(&D(7), D(6)); break;
    case 0xC2E49C: /* move.w  D7, (A1)+ */
        value = D(7); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E49E: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E4A0: /* muls.w  D4, D6 */
        value = D(4); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E4A2: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E4A4: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E4A6: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E4A8: /* move.w  D6, (A1)+ */
        value = D(6); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E4AA: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E4AC: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E4AE: /* swap    D7 */
        step_swap(&D(7)); break;
    case 0xC2E4B0: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E4B2: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E4B4: /* muls.w  D4, D6 */
        value = D(4); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E4B6: /* move.w  D2, D7 */
        value = D(2); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E4B8: /* muls.w  D5, D7 */
        value = D(5); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E4BA: /* sub.l   D6, D7 */
        value = D(6); step_subtract_long(&D(7), value); break;
    case 0xC2E4BC: /* moveq   #$e, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC2E4BE: /* asr.l   D6, D7 */
        step_asr_long(&D(7), D(6)); break;
    case 0xC2E4C0: /* move.w  D7, (A1)+ */
        value = D(7); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E4C2: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E4C4: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E4C6: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E4C8: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E4CA: /* muls.w  D5, D6 */
        value = D(5); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E4CC: /* move.w  D3, D7 */
        value = D(3); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E4CE: /* muls.w  D4, D7 */
        value = D(4); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E4D0: /* sub.l   D6, D7 */
        value = D(6); step_subtract_long(&D(7), value); break;
    case 0xC2E4D2: /* moveq   #$e, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC2E4D4: /* asr.l   D6, D7 */
        step_asr_long(&D(7), D(6)); break;
    case 0xC2E4D6: /* move.w  D7, (A1)+ */
        value = D(7); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E4D8: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E4DA: /* muls.w  D5, D6 */
        value = D(5); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E4DC: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E4DE: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E4E0: /* move.w  D6, (A1)+ */
        value = D(6); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E4E2: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E4E4: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E4E6: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E4E8: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E4EA: /* muls.w  D5, D6 */
        value = D(5); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E4EC: /* move.w  D2, D7 */
        value = D(2); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E4EE: /* muls.w  D4, D7 */
        value = D(4); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E4F0: /* add.l   D6, D7 */
        value = D(6); step_add_long(&D(7), value); break;
    case 0xC2E4F2: /* moveq   #$e, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC2E4F4: /* asr.l   D6, D7 */
        step_asr_long(&D(7), D(6)); break;
    case 0xC2E4F6: /* move.w  D7, (A1)+ */
        value = D(7); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E4F8: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E4FA: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E4FC: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E4FE: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E500: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC2E502: /* move.w  D6, (A1)+ */
        value = D(6); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E504: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E506: /* move.w  D0, (A1)+ */
        value = D(0); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E508: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E50A: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E50C: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E50E: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E510: /* move.w  D6, (A1) */
        value = D(6); step_write_word(A(1), value); flags_logic_w(value); break;
    case 0xC2E512: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2E514: /* lsr.w   #3, D0 */
        SET_W(D(0), step_lsr_word_value(D(0), 3)); break;
    case 0xC2E516: /* lsr.w   #3, D2 */
        SET_W(D(2), step_lsr_word_value(D(2), 3)); break;
    case 0xC2E518: /* lsr.w   #3, D4 */
        SET_W(D(4), step_lsr_word_value(D(4), 3)); break;
    case 0xC2E51A: /* bsr     $c2e5f6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E51E: /* bsr     $c2e6da */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2E522: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E524: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E526: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E528: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E52A: /* muls.w  D2, D6 */
        value = D(2); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E52C: /* move.w  D5, D7 */
        value = D(5); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E52E: /* muls.w  D3, D7 */
        value = D(3); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E530: /* sub.l   D6, D7 */
        value = D(6); step_subtract_long(&D(7), value); break;
    case 0xC2E532: /* moveq   #$e, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC2E534: /* asr.l   D6, D7 */
        step_asr_long(&D(7), D(6)); break;
    case 0xC2E536: /* move.w  D7, (A1)+ */
        value = D(7); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E538: /* move.w  D5, D6 */
        value = D(5); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E53A: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E53C: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E53E: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E540: /* muls.w  D2, D6 */
        value = D(2); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E542: /* move.w  D4, D7 */
        value = D(4); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E544: /* muls.w  D3, D7 */
        value = D(3); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E546: /* add.l   D6, D7 */
        value = D(6); step_add_long(&D(7), value); break;
    case 0xC2E548: /* moveq   #$e, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC2E54A: /* asr.l   D6, D7 */
        step_asr_long(&D(7), D(6)); break;
    case 0xC2E54C: /* neg.w   D7 */
        renderer_negate(&D(7), 2); break;
    case 0xC2E54E: /* move.w  D7, (A1)+ */
        value = D(7); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E550: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E552: /* muls.w  D2, D6 */
        value = D(2); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E554: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E556: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E558: /* move.w  D6, (A1)+ */
        value = D(6); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E55A: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E55C: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E55E: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E560: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E562: /* move.w  D6, (A1)+ */
        value = D(6); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E564: /* move.w  D5, D6 */
        value = D(5); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E566: /* muls.w  D1, D6 */
        value = D(1); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E568: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E56A: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E56C: /* move.w  D6, (A1)+ */
        value = D(6); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E56E: /* move.w  D0, D6 */
        value = D(0); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E570: /* move.w  D6, (A1)+ */
        value = D(6); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E572: /* move.w  D4, D6 */
        value = D(4); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E574: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E576: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E578: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E57A: /* muls.w  D3, D6 */
        value = D(3); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E57C: /* move.w  D5, D7 */
        value = D(5); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E57E: /* muls.w  D2, D7 */
        value = D(2); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E580: /* add.l   D6, D7 */
        value = D(6); step_add_long(&D(7), value); break;
    case 0xC2E582: /* moveq   #$e, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC2E584: /* asr.l   D6, D7 */
        step_asr_long(&D(7), D(6)); break;
    case 0xC2E586: /* neg.w   D7 */
        renderer_negate(&D(7), 2); break;
    case 0xC2E588: /* move.w  D7, (A1)+ */
        value = D(7); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E58A: /* move.w  D5, D6 */
        value = D(5); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E58C: /* muls.w  D0, D6 */
        value = D(0); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E58E: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E590: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E592: /* muls.w  D3, D6 */
        value = D(3); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E594: /* move.w  D4, D7 */
        value = D(4); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E596: /* muls.w  D2, D7 */
        value = D(2); renderer_multiply(&D(7), (uint16_t)value); break;
    case 0xC2E598: /* sub.l   D6, D7 */
        value = D(6); step_subtract_long(&D(7), value); break;
    case 0xC2E59A: /* moveq   #$e, D6 */
        D(6) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(6)); break;
    case 0xC2E59C: /* asr.l   D6, D7 */
        step_asr_long(&D(7), D(6)); break;
    case 0xC2E59E: /* move.w  D7, (A1)+ */
        value = D(7); step_write_word(A(1), value); A(1) += 2; flags_logic_w(value); break;
    case 0xC2E5A0: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E5A2: /* muls.w  D3, D6 */
        value = D(3); renderer_multiply(&D(6), (uint16_t)value); break;
    case 0xC2E5A4: /* moveq   #$e, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC2E5A6: /* asr.l   D7, D6 */
        step_asr_long(&D(6), D(7)); break;
    case 0xC2E5A8: /* move.w  D6, (A1) */
        value = D(6); step_write_word(A(1), value); flags_logic_w(value); break;
    case 0xC2E5AA: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2E5F6: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC2E5F8: /* add.w   D2, D2 */
        value = D(2); step_add_word(&D(2), value); break;
    case 0xC2E5FA: /* lea     $c3e5e8.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC2E600: /* move.w  D0, D7 */
        value = D(0); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E602: /* cmpi.w  #$708, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2E606: /* bge     $c2e618 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E608: /* move.w  (A0,D0.w), D0 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2E60C: /* move.w  #$708, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E610: /* sub.w   D7, D6 */
        value = D(7); step_subtract_word(&D(6), value); break;
    case 0xC2E612: /* move.w  (A0,D6.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E616: /* bra     $c2e66c */
        step_branch(pc, opcode, 1); break;
    case 0xC2E618: /* cmpi.w  #$e10, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2E61C: /* bge     $c2e636 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E61E: /* move.w  #$e10, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E622: /* sub.w   D0, D6 */
        value = D(0); step_subtract_word(&D(6), value); break;
    case 0xC2E624: /* move.w  (A0,D6.w), D0 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2E628: /* move.w  #$708, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E62C: /* sub.w   D6, D7 */
        value = D(6); step_subtract_word(&D(7), value); break;
    case 0xC2E62E: /* move.w  (A0,D7.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E632: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2E634: /* bra     $c2e66c */
        step_branch(pc, opcode, 1); break;
    case 0xC2E636: /* cmpi.w  #$1518, D0 */
        value = m68ki_read_imm_16(); result = D(0); step_compare_word(value, result); break;
    case 0xC2E63A: /* bge     $c2e656 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E63C: /* move.w  #$e10, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E640: /* sub.w   D6, D0 */
        value = D(6); step_subtract_word(&D(0), value); break;
    case 0xC2E642: /* move.w  (A0,D0.w), D0 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2E646: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E648: /* move.w  #$1518, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E64C: /* sub.w   D7, D6 */
        value = D(7); step_subtract_word(&D(6), value); break;
    case 0xC2E64E: /* move.w  (A0,D6.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E652: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC2E654: /* bra     $c2e66c */
        step_branch(pc, opcode, 1); break;
    case 0xC2E656: /* move.w  #$1c20, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E65A: /* sub.w   D0, D6 */
        value = D(0); step_subtract_word(&D(6), value); break;
    case 0xC2E65C: /* move.w  (A0,D6.w), D0 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC2E660: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC2E662: /* move.w  #$1518, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E666: /* sub.w   D6, D7 */
        value = D(6); step_subtract_word(&D(7), value); break;
    case 0xC2E668: /* move.w  (A0,D7.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC2E66C: /* move.w  D2, D7 */
        value = D(2); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E66E: /* cmpi.w  #$708, D2 */
        value = m68ki_read_imm_16(); result = D(2); step_compare_word(value, result); break;
    case 0xC2E672: /* bge     $c2e684 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E674: /* move.w  (A0,D2.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E678: /* move.w  #$708, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E67C: /* sub.w   D7, D6 */
        value = D(7); step_subtract_word(&D(6), value); break;
    case 0xC2E67E: /* move.w  (A0,D6.w), D3 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E682: /* bra     $c2e6d8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E684: /* cmpi.w  #$e10, D2 */
        value = m68ki_read_imm_16(); result = D(2); step_compare_word(value, result); break;
    case 0xC2E688: /* bge     $c2e6a2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E68A: /* move.w  #$e10, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E68E: /* sub.w   D2, D6 */
        value = D(2); step_subtract_word(&D(6), value); break;
    case 0xC2E690: /* move.w  (A0,D6.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E694: /* move.w  #$708, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E698: /* sub.w   D6, D7 */
        value = D(6); step_subtract_word(&D(7), value); break;
    case 0xC2E69A: /* move.w  (A0,D7.w), D3 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E69E: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2E6A0: /* bra     $c2e6d8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E6A2: /* cmpi.w  #$1518, D2 */
        value = m68ki_read_imm_16(); result = D(2); step_compare_word(value, result); break;
    case 0xC2E6A6: /* bge     $c2e6c2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E6A8: /* move.w  #$e10, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E6AC: /* sub.w   D6, D2 */
        value = D(6); step_subtract_word(&D(2), value); break;
    case 0xC2E6AE: /* move.w  (A0,D2.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E6B2: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E6B4: /* move.w  #$1518, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E6B8: /* sub.w   D7, D6 */
        value = D(7); step_subtract_word(&D(6), value); break;
    case 0xC2E6BA: /* move.w  (A0,D6.w), D3 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E6BE: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC2E6C0: /* bra     $c2e6d8 */
        step_branch(pc, opcode, 1); break;
    case 0xC2E6C2: /* move.w  #$1c20, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E6C6: /* sub.w   D2, D6 */
        value = D(2); step_subtract_word(&D(6), value); break;
    case 0xC2E6C8: /* move.w  (A0,D6.w), D2 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2E6CC: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC2E6CE: /* move.w  #$1518, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E6D2: /* sub.w   D6, D7 */
        value = D(6); step_subtract_word(&D(7), value); break;
    case 0xC2E6D4: /* move.w  (A0,D7.w), D3 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2E6D8: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2E6DA: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC2E6DC: /* lea     $c3e5e8.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC2E6E2: /* move.w  D4, D7 */
        value = D(4); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC2E6E4: /* cmpi.w  #$708, D4 */
        value = m68ki_read_imm_16(); result = D(4); step_compare_word(value, result); break;
    case 0xC2E6E8: /* bge     $c2e6fa */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E6EA: /* move.w  (A0,D4.w), D4 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2E6EE: /* move.w  #$708, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E6F2: /* sub.w   D7, D6 */
        value = D(7); step_subtract_word(&D(6), value); break;
    case 0xC2E6F4: /* move.w  (A0,D6.w), D5 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2E6F8: /* bra     $c2e74e */
        step_branch(pc, opcode, 1); break;
    case 0xC2E6FA: /* cmpi.w  #$e10, D4 */
        value = m68ki_read_imm_16(); result = D(4); step_compare_word(value, result); break;
    case 0xC2E6FE: /* bge     $c2e718 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E700: /* move.w  #$e10, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E704: /* sub.w   D4, D6 */
        value = D(4); step_subtract_word(&D(6), value); break;
    case 0xC2E706: /* move.w  (A0,D6.w), D4 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2E70A: /* move.w  #$708, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E70E: /* sub.w   D6, D7 */
        value = D(6); step_subtract_word(&D(7), value); break;
    case 0xC2E710: /* move.w  (A0,D7.w), D5 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2E714: /* neg.w   D5 */
        renderer_negate(&D(5), 2); break;
    case 0xC2E716: /* bra     $c2e74e */
        step_branch(pc, opcode, 1); break;
    case 0xC2E718: /* cmpi.w  #$1518, D4 */
        value = m68ki_read_imm_16(); result = D(4); step_compare_word(value, result); break;
    case 0xC2E71C: /* bge     $c2e738 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2E71E: /* move.w  #$e10, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E722: /* sub.w   D6, D4 */
        value = D(6); step_subtract_word(&D(4), value); break;
    case 0xC2E724: /* move.w  (A0,D4.w), D4 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2E728: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2E72A: /* move.w  #$1518, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E72E: /* sub.w   D7, D6 */
        value = D(7); step_subtract_word(&D(6), value); break;
    case 0xC2E730: /* move.w  (A0,D6.w), D5 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2E734: /* neg.w   D5 */
        renderer_negate(&D(5), 2); break;
    case 0xC2E736: /* bra     $c2e74e */
        step_branch(pc, opcode, 1); break;
    case 0xC2E738: /* move.w  #$1c20, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E73C: /* sub.w   D4, D6 */
        value = D(4); step_subtract_word(&D(6), value); break;
    case 0xC2E73E: /* move.w  (A0,D6.w), D4 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2E742: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC2E744: /* move.w  #$1518, D6 */
        value = m68ki_read_imm_16(); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2E748: /* sub.w   D6, D7 */
        value = D(6); step_subtract_word(&D(7), value); break;
    case 0xC2E74A: /* move.w  (A0,D7.w), D5 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2E74E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C2E47A_step(void) { return glue_C2D954_step(); }

int glue_C2E514_step(void) { return glue_C2D954_step(); }

int glue_C2E5F6_step(void) { return glue_C2D954_step(); }

int glue_C2E6DA_step(void) { return glue_C2D954_step(); }
