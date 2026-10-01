/* Record rate classification; control_records.c.
 * CPU effects and source instruction/bus/event boundaries stay in glue. */
#include "glue_renderer_step_math.h"

int glue_C1C7F6_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc);
    switch (pc) {
    case 0xC1C7F6: /* move.w  ($56,A3), D0 */
        value = m68k_read_memory_16(step_displacement(A(3))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1C7FA: /* bge     $c1c7fe */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1C7FC: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC1C7FE: /* move.w  ($58,A3), D1 */
        value = m68k_read_memory_16(step_displacement(A(3))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1C802: /* bge     $c1c806 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1C804: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC1C806: /* move.w  ($5a,A3), D2 */
        value = m68k_read_memory_16(step_displacement(A(3))); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1C80A: /* bge     $c1c80e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1C80C: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC1C80E: /* asr.w   #2, D2 */
        renderer_asr_word(&D(2), 2); break;
    case 0xC1C810: /* cmp.w   D0, D1 */
        value = D(0); step_compare_word(value, D(1)); break;
    case 0xC1C812: /* bgt     $c1c81c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1C814: /* cmp.w   D0, D2 */
        value = D(0); step_compare_word(value, D(2)); break;
    case 0xC1C816: /* ble     $c1c826 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1C818: /* move.w  D2, D0 */
        value = D(2); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1C81A: /* bra     $c1c826 */
        step_branch(pc, opcode, 1); break;
    case 0xC1C81C: /* cmp.w   D2, D1 */
        value = D(2); step_compare_word(value, D(1)); break;
    case 0xC1C81E: /* bgt     $c1c824 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1C820: /* move.w  D2, D0 */
        value = D(2); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1C822: /* bra     $c1c826 */
        step_branch(pc, opcode, 1); break;
    case 0xC1C824: /* move.w  D1, D0 */
        value = D(1); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1C826: /* cmpi.w  #$60, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC1C82A: /* bgt     $c1c844 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1C82C: /* move.w  ($6c,A3), D1 */
        value = m68k_read_memory_16(step_displacement(A(3))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1C830: /* bge     $c1c834 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1C832: /* neg.w   D1 */
        renderer_negate(&D(1), 2); break;
    case 0xC1C834: /* cmpi.w  #$1000, D1 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(1)); break;
    case 0xC1C838: /* bgt     $c1c84a */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1C83A: /* move.b  #$5, $c458bc.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1C842: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1C844: /* cmpi.w  #$c0, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC1C848: /* bgt     $c1c854 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1C84A: /* move.b  #$3, $c458bc.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1C852: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1C854: /* move.b  #$1, $c458bc.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1C85C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
