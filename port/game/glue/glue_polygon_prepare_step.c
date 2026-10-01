/* Polygon bounds, thin primitives, outline/fill and mask-between-planes.
 * Readable behavior is in render_polygon.c; C301F0 shares C301F6's body.
 * Source calls, arithmetic, bus accesses and instruction boundaries stay
 * in this CPU bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

int glue_C301F6_step(void) {
    uint32_t pc = REG_PC, value, address, result, temporary;
    uint16_t opcode = step_begin(pc), mask, word;
    switch (pc) {
    case 0xC301F0: /* movea.w #$c7, A4 */
        value = m68ki_read_imm_16(); A(4) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC301F4: /* bra     $c301fc */
        step_branch(pc, opcode, 1); break;
    case 0xC301F6: /* movea.w $c45984.l, A4 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(4) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC301FC: /* lea     $c4b390.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC30202: /* move.w  (A0)+, D6 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC30204: /* subq.w  #3, D6 */
        value = 3u; step_subtract_word(&D(6), value); break;
    case 0xC30206: /* movem.w (A0)+, D0-D5 */
        mask = m68ki_read_imm_16(); renderer_load(A(0), mask, 2, 0); break;
    case 0xC3020A: /* cmp.w   D0, D2 */
        value = D(0); step_compare_word(value, D(2)); break;
    case 0xC3020C: /* bge     $c30210 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC3020E: /* exg     D0, D2 */
        temporary = D(0); D(0) = D(2); D(2) = temporary; break;
    case 0xC30210: /* cmp.w   D0, D4 */
        value = D(0); step_compare_word(value, D(4)); break;
    case 0xC30212: /* bge     $c30218 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC30214: /* move.w  D4, D0 */
        value = D(4); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC30216: /* bra     $c3021e */
        step_branch(pc, opcode, 1); break;
    case 0xC30218: /* cmp.w   D2, D4 */
        value = D(2); step_compare_word(value, D(4)); break;
    case 0xC3021A: /* ble     $c3021e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC3021C: /* move.w  D4, D2 */
        value = D(4); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC3021E: /* cmp.w   D1, D3 */
        value = D(1); step_compare_word(value, D(3)); break;
    case 0xC30220: /* bge     $c30224 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC30222: /* exg     D1, D3 */
        temporary = D(1); D(1) = D(3); D(3) = temporary; break;
    case 0xC30224: /* cmp.w   D1, D5 */
        value = D(1); step_compare_word(value, D(5)); break;
    case 0xC30226: /* bge     $c3022c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC30228: /* move.w  D5, D1 */
        value = D(5); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC3022A: /* bra     $c30232 */
        step_branch(pc, opcode, 1); break;
    case 0xC3022C: /* cmp.w   D3, D5 */
        value = D(3); step_compare_word(value, D(5)); break;
    case 0xC3022E: /* ble     $c30232 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC30230: /* move.w  D5, D3 */
        value = D(5); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC30232: /* subq.w  #1, D6 */
        value = 1u; step_subtract_word(&D(6), value); break;
    case 0xC30234: /* blt     $c3025c */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC30236: /* move.w  (A0)+, D4 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC30238: /* move.w  (A0)+, D5 */
        value = m68k_read_memory_16(A(0)); A(0) += 2; SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC3023A: /* cmp.w   D0, D4 */
        value = D(0); step_compare_word(value, D(4)); break;
    case 0xC3023C: /* bgt     $c30242 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC3023E: /* move.w  D4, D0 */
        value = D(4); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC30240: /* bra     $c30248 */
        step_branch(pc, opcode, 1); break;
    case 0xC30242: /* cmp.w   D4, D2 */
        value = D(4); step_compare_word(value, D(2)); break;
    case 0xC30244: /* bge     $c30248 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC30246: /* move.w  D4, D2 */
        value = D(4); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC30248: /* cmp.w   D1, D5 */
        value = D(1); step_compare_word(value, D(5)); break;
    case 0xC3024A: /* bgt     $c30250 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC3024C: /* move.w  D5, D1 */
        value = D(5); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC3024E: /* bra     $c30232 */
        step_branch(pc, opcode, 1); break;
    case 0xC30250: /* cmp.w   D5, D3 */
        value = D(5); step_compare_word(value, D(3)); break;
    case 0xC30252: /* bge     $c30232 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC30254: /* move.w  D5, D3 */
        value = D(5); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC30256: /* bra     $c30232 */
        step_branch(pc, opcode, 1); break;
    case 0xC30258: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC3025A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC3025C: /* cmp.w   A4, D1 */
        value = A(4); step_compare_word(value, D(1)); break;
    case 0xC3025E: /* bgt     $c30258 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC30260: /* move.w  D3, D7 */
        value = D(3); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC30262: /* sub.w   D1, D7 */
        value = D(1); step_subtract_word(&D(7), value); break;
    case 0xC30264: /* bge     $c30268 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC30266: /* neg.w   D7 */
        renderer_negate(&D(7), 2); break;
    case 0xC30268: /* cmpi.w  #$2, D7 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(7)); break;
    case 0xC3026C: /* bgt     $c302de */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC3026E: /* cmpi.w  #$1, D7 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(7)); break;
    case 0xC30272: /* ble     $c30290 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC30274: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC30276: /* sub.w   D0, D6 */
        value = D(0); step_subtract_word(&D(6), value); break;
    case 0xC30278: /* bge     $c3027c */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC3027A: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC3027C: /* cmpi.w  #$2, D6 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(6)); break;
    case 0xC30280: /* bgt     $c302ec */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC30282: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC30284: /* cmp.w   A4, D1 */
        value = A(4); step_compare_word(value, D(1)); break;
    case 0xC30286: /* bgt     $c302da */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC30288: /* move.w  D2, D0 */
        value = D(2); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC3028A: /* bsr     $c2f66e */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC3028E: /* bra     $c302da */
        step_branch(pc, opcode, 1); break;
    case 0xC30290: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC30292: /* sub.w   D0, D6 */
        value = D(0); step_subtract_word(&D(6), value); break;
    case 0xC30294: /* bge     $c30298 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC30296: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC30298: /* cmpi.w  #$1, D6 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(6)); break;
    case 0xC3029C: /* ble     $c302c4 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC3029E: /* move.l  $c456e6.l, -(A7) */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(7) -= 4; m68k_write_memory_16(A(7) + 2, value); m68k_write_memory_16(A(7), value >> 16); flags_logic_l(value); break;
    case 0xC302A4: /* tst.b   $c457a2.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC302AA: /* bne     $c302b6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC302AC: /* move.l  #$fffff, $c456e6.l */
        value = m68ki_read_imm_32(); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC302B6: /* bsr     $c2fa7e */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC302BA: /* move.l  (A7)+, $c456e6.l */
        value = m68k_read_memory_32(A(7)); A(7) += 4; step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC302C0: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC302C2: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC302C4: /* addq.w  #1, D1 */
        value = 1u; step_add_word(&D(1), value); break;
    case 0xC302C6: /* cmp.w   A4, D1 */
        value = A(4); step_compare_word(value, D(1)); break;
    case 0xC302C8: /* bgt     $c302da */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC302CA: /* subq.w  #1, D6 */
        value = 1u; step_subtract_word(&D(6), value); break;
    case 0xC302CC: /* bge     $c302d4 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC302CE: /* bsr     $c2f5f4 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC302D2: /* bra     $c302da */
        step_branch(pc, opcode, 1); break;
    case 0xC302D4: /* move.w  D2, D0 */
        value = D(2); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC302D6: /* bsr     $c2f60a */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC302DA: /* moveq   #$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC302DC: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC302DE: /* move.w  D2, D6 */
        value = D(2); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC302E0: /* sub.w   D0, D6 */
        value = D(0); step_subtract_word(&D(6), value); break;
    case 0xC302E2: /* bge     $c302e6 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC302E4: /* neg.w   D6 */
        renderer_negate(&D(6), 2); break;
    case 0xC302E6: /* cmpi.w  #$1, D6 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(6)); break;
    case 0xC302EA: /* ble     $c3029e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC302EC: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC302EE: /* bge     $c302f2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC302F0: /* clr.w   D0 */
        value = 0; SET_W(D(0), value); flags_logic_w(0); break;
    case 0xC302F2: /* exg     D1, D2 */
        temporary = D(1); D(1) = D(2); D(2) = temporary; break;
    case 0xC302F4: /* movem.w D0-D3, $c4597c.l */
        mask = m68ki_read_imm_16(); renderer_store(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC302FC: /* move.w  $c45954.l, $c45956.l */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC30306: /* lea     $c4b390.l, A2 */
        A(2) = m68ki_read_imm_32(); break;
    case 0xC3030C: /* lea     $c45970.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC30312: /* move.w  (A2)+, (A3) */
        value = m68k_read_memory_16(A(2)); A(2) += 2; step_write_word(A(3), value); flags_logic_w(value); break;
    case 0xC30314: /* subq.w  #1, (A3) */
        value = 1u; address = A(3); result = m68k_read_memory_16(address); step_subtract_word(&result, value); step_write_word(address, result); break;
    case 0xC30316: /* lea     $dff000.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC3031C: /* movem.w (A2), D0-D3 */
        mask = m68ki_read_imm_16(); renderer_load(A(2), mask, 2, -1); break;
    case 0xC30320: /* addq.w  #4, A2 */
        value = 4u; A(2) += value; break;
    case 0xC30322: /* move.w  A4, -(A7) */
        value = A(4); A(7) -= 2; step_write_word(A(7), value); flags_logic_w(value); break;
    case 0xC30324: /* bsr     $c305aa */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC30328: /* movea.w (A7)+, A4 */
        value = m68k_read_memory_16(A(7)); A(7) += 2; A(4) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC3032A: /* subq.w  #1, (A3) */
        value = 1u; address = A(3); result = m68k_read_memory_16(address); step_subtract_word(&result, value); step_write_word(address, result); break;
    case 0xC3032C: /* bgt     $c3031c */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC3032E: /* move.w  (A2)+, D0 */
        value = m68k_read_memory_16(A(2)); A(2) += 2; SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC30330: /* move.w  (A2), D1 */
        value = m68k_read_memory_16(A(2)); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC30332: /* movem.w $c4b392.l, D2-D3 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC3033A: /* move.w  A4, -(A7) */
        value = A(4); A(7) -= 2; step_write_word(A(7), value); flags_logic_w(value); break;
    case 0xC3033C: /* bsr     $c305aa */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC30340: /* movea.w (A7)+, A4 */
        value = m68k_read_memory_16(A(7)); A(7) += 2; A(4) = (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC30342: /* movem.w $c4597c.l, D1/D3 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 2, -1); break;
    case 0xC3034A: /* asr.w   #4, D3 */
        renderer_asr_word(&D(3), 4); break;
    case 0xC3034C: /* move.w  D3, D2 */
        value = D(3); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC3034E: /* asr.w   #4, D1 */
        renderer_asr_word(&D(1), 4); break;
    case 0xC30350: /* sub.w   D1, D3 */
        value = D(1); step_subtract_word(&D(3), value); break;
    case 0xC30352: /* move.w  D3, D6 */
        value = D(3); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC30354: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC30356: /* neg.w   D3 */
        renderer_negate(&D(3), 2); break;
    case 0xC30358: /* addi.w  #$27, D3 */
        value = m68ki_read_imm_16(); step_add_word(&D(3), value); break;
    case 0xC3035C: /* move.w  $c45982.l, D5 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC30362: /* move.w  D5, D1 */
        value = D(5); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC30364: /* move.w  D5, D7 */
        value = D(5); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC30366: /* asl.w   #3, D1 */
        renderer_asl_word(&D(1), 3); break;
    case 0xC30368: /* move.w  D1, D0 */
        value = D(1); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC3036A: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC3036C: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC3036E: /* add.w   D0, D1 */
        value = D(0); step_add_word(&D(1), value); break;
    case 0xC30370: /* add.w   D2, D2 */
        value = D(2); step_add_word(&D(2), value); break;
    case 0xC30372: /* ext.l   D2 */
        D(2) = (uint32_t)(int32_t)(int16_t)D(2); flags_logic_l(D(2)); break;
    case 0xC30374: /* add.l   D2, D1 */
        value = D(2); step_add_long(&D(1), value); break;
    case 0xC30376: /* move.l  D1, D2 */
        value = D(1); D(2) = value; flags_logic_l(value); break;
    case 0xC30378: /* cmp.w   A4, D7 */
        value = A(4); step_compare_word(value, D(7)); break;
    case 0xC3037A: /* ble     $c30390 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC3037C: /* sub.w   A4, D7 */
        value = A(4); step_subtract_word(&D(7), value); break;
    case 0xC3037E: /* move.w  D7, D0 */
        value = D(7); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC30380: /* asl.w   #3, D7 */
        renderer_asl_word(&D(7), 3); break;
    case 0xC30382: /* move.w  D7, D4 */
        value = D(7); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC30384: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC30386: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC30388: /* add.w   D4, D7 */
        value = D(4); step_add_word(&D(7), value); break;
    case 0xC3038A: /* ext.l   D7 */
        D(7) = (uint32_t)(int32_t)(int16_t)D(7); flags_logic_l(D(7)); break;
    case 0xC3038C: /* sub.l   D7, D1 */
        value = D(7); step_subtract_long(&D(1), value); break;
    case 0xC3038E: /* bra     $c30394 */
        step_branch(pc, opcode, 1); break;
    case 0xC30390: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC30392: /* moveq   #$0, D7 */
        D(7) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(7)); break;
    case 0xC30394: /* move.l  D1, $c45968.l */
        value = D(1); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC3039A: /* move.l  D2, D1 */
        value = D(2); D(1) = value; flags_logic_l(value); break;
    case 0xC3039C: /* add.l   $c456e2.l, D1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_add_long(&D(1), value); break;
    case 0xC303A2: /* sub.l   D7, D1 */
        value = D(7); step_subtract_long(&D(1), value); break;
    case 0xC303A4: /* move.l  D1, $c45960.l */
        value = D(1); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC303AA: /* move.l  D1, D2 */
        value = D(1); D(2) = value; flags_logic_l(value); break;
    case 0xC303AC: /* move.l  D1, $c45964.l */
        value = D(1); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC303B2: /* sub.w   $c45980.l, D5 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_subtract_word(&D(5), value); break;
    case 0xC303B8: /* addq.w  #1, D5 */
        value = 1u; step_add_word(&D(5), value); break;
    case 0xC303BA: /* move.w  D5, D7 */
        value = D(5); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC303BC: /* addq.w  #1, D6 */
        value = 1u; step_add_word(&D(6), value); break;
    case 0xC303BE: /* sub.w   D0, D7 */
        value = D(0); step_subtract_word(&D(7), value); break;
    case 0xC303C0: /* lsl.w   #6, D7 */
        renderer_asl_word(&D(7), 6); FLAG_V = 0; break;
    case 0xC303C2: /* add.w   D6, D7 */
        value = D(6); step_add_word(&D(7), value); break;
    case 0xC303C4: /* move.w  D7, $c4596e.l */
        value = D(7); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC303CA: /* moveq   #-$1, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC303CC: /* lea     $dff000.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC303D2: /* btst    #$6, ($2,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC303D8: /* beq     $c303e0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC303DA: /* nop */
        break;
    case 0xC303DC: /* nop */
        break;
    case 0xC303DE: /* bra     $c303d2 */
        step_branch(pc, opcode, 1); break;
    case 0xC303E0: /* move.w  #$9f0, ($40,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC303E6: /* move.w  #$a, ($42,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC303EC: /* move.l  D2, ($50,A0) */
        value = D(2); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC303F0: /* move.l  D2, ($54,A0) */
        value = D(2); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC303F4: /* move.l  D0, ($44,A0) */
        value = D(0); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC303F8: /* move.w  D3, ($64,A0) */
        value = D(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC303FC: /* move.w  D3, ($62,A0) */
        value = D(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC30400: /* move.w  D3, ($66,A0) */
        value = D(3); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC30404: /* move.w  D7, ($58,A0) */
        value = D(7); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC30408: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC3040A: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC3040C: /* movea.l $c456b6.l, A2 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(2) = value; break;
    case 0xC30412: /* move.l  ($4,A2), D2 */
        value = m68k_read_memory_32(step_displacement(A(2))); D(2) = value; flags_logic_l(value); break;
    case 0xC30416: /* move.l  (A2), D3 */
        value = m68k_read_memory_32(A(2)); D(3) = value; flags_logic_l(value); break;
    case 0xC30418: /* move.w  $c4596e.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC3041E: /* move.l  $c45968.l, D1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(1) = value; flags_logic_l(value); break;
    case 0xC30424: /* add.l   D1, D2 */
        value = D(1); step_add_long(&D(2), value); break;
    case 0xC30426: /* add.l   D1, D3 */
        value = D(1); step_add_long(&D(3), value); break;
    case 0xC30428: /* move.l  $c45964.l, D1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(1) = value; flags_logic_l(value); break;
    case 0xC3042E: /* lea     $dff000.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC30434: /* move.w  #$fca, D4 */
        value = m68ki_read_imm_16(); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC30438: /* btst    #$6, ($2,A0) */
        value = m68ki_read_imm_16(); address = step_displacement(A(0)); FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7u)); break;
    case 0xC3043E: /* beq     $c30446 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC30440: /* nop */
        break;
    case 0xC30442: /* nop */
        break;
    case 0xC30444: /* bra     $c30438 */
        step_branch(pc, opcode, 1); break;
    case 0xC30446: /* move.w  D4, ($40,A0) */
        value = D(4); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC3044A: /* move.w  #$2, ($42,A0) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC30450: /* move.l  D1, ($50,A0) */
        value = D(1); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC30454: /* move.l  D3, ($4c,A0) */
        value = D(3); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC30458: /* move.l  D2, ($48,A0) */
        value = D(2); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC3045C: /* move.l  D2, ($54,A0) */
        value = D(2); step_write_long(step_displacement(A(0)), value); flags_logic_l(value); break;
    case 0xC30460: /* move.w  D0, ($58,A0) */
        value = D(0); step_write_word(step_displacement(A(0)), value); flags_logic_w(value); break;
    case 0xC30464: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C301F0_step(void) { return glue_C301F6_step(); }

int glue_C3040C_step(void) { return glue_C301F6_step(); }
