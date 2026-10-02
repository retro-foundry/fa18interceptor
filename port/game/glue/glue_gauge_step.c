/* Source timing for the cached 22-row gauge ($C30918).
 * view_marks.c owns the readable renderer; CPU/event effects stay here. */
#include "glue_renderer_step_math.h"

int glue_C30918_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode, mask;
    if (pc < 0xC30916u || pc >= 0xC309A2u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC30916: case 0xC309A0: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC30918: /* move.w $c458f6.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32());
        SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC3091E: /* andi.w #$7fff, D2 */
        value = m68ki_read_imm_16();
        SET_W(D(2), D(2) & value); flags_logic_w(D(2)); break;
    case 0xC30922: /* lsr.w #8, D2 */
        SET_W(D(2), step_lsr_word_value((uint16_t)D(2), 8)); break;
    case 0xC30924: /* lsr.w #2, D2 */
        SET_W(D(2), step_lsr_word_value((uint16_t)D(2), 2)); break;
    case 0xC30926: /* tst.b $c45837.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC3092C: case 0xC3095A: case 0xC30968: /* bgt */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC3092E: /* cmp.w $c459a4.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32());
        step_compare_word(value, D(2)); break;
    case 0xC30934: case 0xC3093E: /* beq */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC30936: /* btst #0, $c458db.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32();
        FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC30940: /* move.w D2, $c459a4.l */
        step_write_word(m68ki_read_imm_32(), D(2)); flags_logic_w(D(2)); break;
    case 0xC30946: /* move.w #$15, D3 */
        value = m68ki_read_imm_16(); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC3094A: /* move.w #$70, D0 */
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC3094E: /* add.w $c45988.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_add_word(&D(0), value); break;
    case 0xC30954: /* ble */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC30956: /* cmpi.w #$13c, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC3095C: /* move.w #$b4, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC30960: /* add.w $c458d8.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_add_word(&D(1), value); break;
    case 0xC30966: /* subq.w #1, D2 */
        step_subtract_word(&D(2), 1); break;
    case 0xC3096A: case 0xC30974: /* move.w #0/#4, $c45954.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value);
        flags_logic_w(value); break;
    case 0xC30972: /* bra */
        step_branch(pc, opcode, 1); break;
    case 0xC3097C: case 0xC3098A: /* movem.w D0-D3, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 2, 7); break;
    case 0xC30980: case 0xC30990: /* jsr $c2f60a.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC30986: case 0xC30996: /* movem.w (A7)+, D0-D3 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 2, 7); break;
    case 0xC3098E: /* addq.w #2, D0 */
        step_add_word(&D(0), 2); break;
    case 0xC3099A: /* subq.w #1, D1 */
        step_subtract_word(&D(1), 1); break;
    case 0xC3099C: /* dbra D3, $c30966 */
        step_dbf(pc, &D(3)); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
