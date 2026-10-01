/* Polygon compositing parent; readable behavior is in render_polygon.c.
 * Source calls, arithmetic, bus accesses and instruction boundaries stay
 * in this CPU bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

int glue_C2FF48_step(void) {
    uint32_t pc = REG_PC, value, address, result, temporary;
    uint16_t opcode = step_begin(pc), mask, word;
    switch (pc) {
    case 0xC2FF46: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2FF48: /* nop */
        break;
    case 0xC2FF4A: /* move.w  #$8400, $dff096.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2FF52: /* bsr     $c301f6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2FF56: /* bne     $c2ff46 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2FF58: /* tst.w   $c456e8.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); flags_logic_w(value); break;
    case 0xC2FF5E: /* blt     $c2ff70 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2FF60: /* move.w  $c456ec.l, D5 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC2FF66: /* beq     $c2ff70 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FF68: /* bsr     $c3040c */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2FF6C: /* bra     $c3002a */
        step_branch(pc, opcode, 1); break;
    case 0xC2FF70: /* btst    #$0, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FF78: /* beq     $c2ff96 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FF7A: /* moveq   #$c, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2FF7C: /* move.w  $c456ea.l, D4 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2FF82: /* move.w  $c456e8.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2FF88: /* bge     $c2ff90 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC2FF8A: /* move.w  $c45956.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2FF90: /* bsr     $c30466 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2FF94: /* bra     $c2ff9c */
        step_branch(pc, opcode, 1); break;
    case 0xC2FF96: /* lsr.w   $c45956.l */
        address = m68ki_read_imm_32(); word = m68k_read_memory_16(address); word = step_lsr_word_value(word, 1); USE_CYCLES(-(1 << CYC_SHIFT)); step_write_word(address, word); break;
    case 0xC2FF9C: /* btst    #$1, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FFA4: /* beq     $c2ffc8 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FFA6: /* moveq   #$8, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2FFA8: /* move.w  $c456e8.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2FFAE: /* blt     $c2ffbc */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2FFB0: /* move.w  $c456ea.l, D4 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2FFB6: /* asr.w   #1, D3 */
        renderer_asr_word(&D(3), 1); break;
    case 0xC2FFB8: /* asr.w   #1, D4 */
        renderer_asr_word(&D(4), 1); break;
    case 0xC2FFBA: /* bra     $c2ffc2 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FFBC: /* move.w  $c45956.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2FFC2: /* bsr     $c30466 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2FFC6: /* bra     $c2ffce */
        step_branch(pc, opcode, 1); break;
    case 0xC2FFC8: /* lsr.w   $c45956.l */
        address = m68ki_read_imm_32(); word = m68k_read_memory_16(address); word = step_lsr_word_value(word, 1); USE_CYCLES(-(1 << CYC_SHIFT)); step_write_word(address, word); break;
    case 0xC2FFCE: /* btst    #$2, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC2FFD6: /* beq     $c2fffa */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FFD8: /* moveq   #$4, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC2FFDA: /* move.w  $c456e8.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2FFE0: /* blt     $c2ffee */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC2FFE2: /* move.w  $c456ea.l, D4 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC2FFE8: /* asr.w   #2, D3 */
        renderer_asr_word(&D(3), 2); break;
    case 0xC2FFEA: /* asr.w   #2, D4 */
        renderer_asr_word(&D(4), 2); break;
    case 0xC2FFEC: /* bra     $c2fff4 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FFEE: /* move.w  $c45956.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC2FFF4: /* bsr     $c30466 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC2FFF8: /* bra     $c30000 */
        step_branch(pc, opcode, 1); break;
    case 0xC2FFFA: /* lsr.w   $c45956.l */
        address = m68ki_read_imm_32(); word = m68k_read_memory_16(address); word = step_lsr_word_value(word, 1); USE_CYCLES(-(1 << CYC_SHIFT)); step_write_word(address, word); break;
    case 0xC30000: /* btst    #$3, $c456e7.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC30008: /* beq     $c3002a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC3000A: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC3000C: /* move.w  $c456e8.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC30012: /* blt     $c30020 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC30014: /* move.w  $c456ea.l, D4 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC3001A: /* asr.w   #3, D3 */
        renderer_asr_word(&D(3), 3); break;
    case 0xC3001C: /* asr.w   #3, D4 */
        renderer_asr_word(&D(4), 3); break;
    case 0xC3001E: /* bra     $c30026 */
        step_branch(pc, opcode, 1); break;
    case 0xC30020: /* move.w  $c45956.l, D3 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC30026: /* bsr     $c30466 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC3002A: /* bsr     $c304b2 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC3002E: /* move.w  #$400, $dff096.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC30036: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
