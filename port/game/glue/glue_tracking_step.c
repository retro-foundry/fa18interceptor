/* Target aim, rounded division and integer square root; tracking.c and fixed_math.c.
 * CPU register/flag effects, bus accesses and source instruction boundaries
 * stay in this bridge; no interpreter opcode handler is called. */
#include "glue_renderer_step_math.h"

#include "glue_unsigned_division_step.h"

int glue_C123FA_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC123FA: /* link    A6, #-$1c */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC123FE: /* movem.l D2, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC12402: /* move.w  #$2, (-$1c,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC12408: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC1240A: /* move.b  D0, (-$9,A6) */
        value = D(0); m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(value); break;
    case 0xC1240E: /* tst.l   ($10,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(value); break;
    case 0xC12412: /* bpl     $c12420 */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC12414: /* neg.l   ($10,A6) */
        address = step_displacement(A(6)); result = m68k_read_memory_32(address); renderer_negate(&result, 4); step_write_long(address, result); break;
    case 0xC12418: /* bset    #$0, D0 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(0) & (1u << value); D(0) |= 1u << value; USE_CYCLES(-(1 << CYC_SHIFT)); break;
    case 0xC1241C: /* move.b  D0, (-$9,A6) */
        value = D(0); m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(value); break;
    case 0xC12420: /* tst.l   ($14,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(value); break;
    case 0xC12424: /* bpl     $c12430 */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC12426: /* neg.l   ($14,A6) */
        address = step_displacement(A(6)); result = m68k_read_memory_32(address); renderer_negate(&result, 4); step_write_long(address, result); break;
    case 0xC1242A: /* bset    #$1, (-$9,A6) */
        value = m68ki_read_imm_16(); address = step_displacement(A(6)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result | value); break;
    case 0xC12430: /* tst.l   ($18,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(value); break;
    case 0xC12434: /* bpl     $c12440 */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC12436: /* neg.l   ($18,A6) */
        address = step_displacement(A(6)); result = m68k_read_memory_32(address); renderer_negate(&result, 4); step_write_long(address, result); break;
    case 0xC1243A: /* bset    #$2, (-$9,A6) */
        value = m68ki_read_imm_16(); address = step_displacement(A(6)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result | value); break;
    case 0xC12440: /* move.l  ($10,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12444: /* cmp.l   ($14,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); result = D(0); step_compare_long(value, result); break;
    case 0xC12448: /* ble     $c1245e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1244A: /* cmp.l   ($18,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); result = D(0); step_compare_long(value, result); break;
    case 0xC1244E: /* ble     $c12456 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC12450: /* move.l  D0, (-$16,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12454: /* bra     $c12474 */
        step_branch(pc, opcode, 1); break;
    case 0xC12456: /* move.l  ($18,A6), (-$16,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1245C: /* bra     $c12474 */
        step_branch(pc, opcode, 1); break;
    case 0xC1245E: /* move.l  ($14,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12462: /* cmp.l   ($18,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); result = D(0); step_compare_long(value, result); break;
    case 0xC12466: /* ble     $c1246e */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC12468: /* move.l  D0, (-$16,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1246C: /* bra     $c12474 */
        step_branch(pc, opcode, 1); break;
    case 0xC1246E: /* move.l  ($18,A6), (-$16,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12474: /* cmpi.l  #$10000000, (-$16,A6) */
        value = m68ki_read_imm_32(); result = m68k_read_memory_32(step_displacement(A(6))); step_compare_long(value, result); break;
    case 0xC1247C: /* ble     $c124a8 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1247E: /* move.l  ($10,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12482: /* asr.l   #3, D0 */
        step_asr_long(&D(0), 3); break;
    case 0xC12484: /* move.l  ($14,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC12488: /* asr.l   #3, D1 */
        step_asr_long(&D(1), 3); break;
    case 0xC1248A: /* move.l  ($18,A6), D2 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(2) = value; flags_logic_l(value); break;
    case 0xC1248E: /* asr.l   #3, D2 */
        step_asr_long(&D(2), 3); break;
    case 0xC12490: /* move.w  #$e, (-$18,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC12496: /* clr.w   (-$1a,A6) */
        value = 0; step_write_word(step_displacement(A(6)), value); flags_logic_w(0); break;
    case 0xC1249A: /* move.l  D0, ($10,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1249E: /* move.l  D1, ($14,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC124A2: /* move.l  D2, ($18,A6) */
        value = D(2); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC124A6: /* bra     $c124fa */
        step_branch(pc, opcode, 1); break;
    case 0xC124A8: /* cmpi.l  #$2000000, (-$16,A6) */
        value = m68ki_read_imm_32(); result = m68k_read_memory_32(step_displacement(A(6))); step_compare_long(value, result); break;
    case 0xC124B0: /* ble     $c124be */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC124B2: /* move.w  #$e, (-$18,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC124B8: /* clr.w   (-$1a,A6) */
        value = 0; step_write_word(step_displacement(A(6)), value); flags_logic_w(0); break;
    case 0xC124BC: /* bra     $c124fa */
        step_branch(pc, opcode, 1); break;
    case 0xC124BE: /* cmpi.l  #$800000, (-$16,A6) */
        value = m68ki_read_imm_32(); result = m68k_read_memory_32(step_displacement(A(6))); step_compare_long(value, result); break;
    case 0xC124C6: /* ble     $c124d6 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC124C8: /* move.w  #$c, (-$18,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC124CE: /* move.w  #$2, (-$1a,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC124D4: /* bra     $c124fa */
        step_branch(pc, opcode, 1); break;
    case 0xC124D6: /* cmpi.l  #$200000, (-$16,A6) */
        value = m68ki_read_imm_32(); result = m68k_read_memory_32(step_displacement(A(6))); step_compare_long(value, result); break;
    case 0xC124DE: /* ble     $c124ee */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC124E0: /* move.w  #$a, (-$18,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC124E6: /* move.w  #$4, (-$1a,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC124EC: /* bra     $c124fa */
        step_branch(pc, opcode, 1); break;
    case 0xC124EE: /* move.w  #$8, (-$18,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC124F4: /* move.w  #$6, (-$1a,A6) */
        value = m68ki_read_imm_16(); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC124FA: /* move.w  (-$18,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC124FE: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC12500: /* move.l  ($10,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC12504: /* asr.l   D0, D1 */
        step_asr_long(&D(1), D(0)); break;
    case 0xC12506: /* move.l  ($18,A6), D2 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(2) = value; flags_logic_l(value); break;
    case 0xC1250A: /* asr.l   D0, D2 */
        step_asr_long(&D(2), D(0)); break;
    case 0xC1250C: /* move.w  D1, (-$4,A6) */
        value = D(1); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC12510: /* move.w  D2, (-$6,A6) */
        value = D(2); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC12514: /* cmp.w   D2, D1 */
        value = D(2); result = D(1); step_compare_word(value, result); break;
    case 0xC12516: /* bgt     $c1256a */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC12518: /* move.w  (-$1a,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1251C: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC1251E: /* move.l  ($10,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC12522: /* asl.l   D0, D1 */
        step_asl_long(&D(1), D(0)); break;
    case 0xC12524: /* move.l  D1, $c45acc.l */
        value = D(1); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1252A: /* move.w  D2, $c45ad0.l */
        value = D(2); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC12530: /* tst.w   D2 */
        value = D(2); flags_logic_w(value); break;
    case 0xC12532: /* beq     $c12560 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC12534: /* jsr     $c25980.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1253A: /* move.w  $c45ad2.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12540: /* asr.w   #6, D0 */
        renderer_asr_word(&D(0), 6); break;
    case 0xC12542: /* move.w  D0, $c45ad2.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC12548: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC1254A: /* asl.l   #1, D0 */
        step_asl_long(&D(0), 1); break;
    case 0xC1254C: /* movea.l D0, A0 */
        value = D(0); A(0) = value; break;
    case 0xC1254E: /* adda.l  #$c3db00, A0 */
        value = m68ki_read_imm_32(); A(0) += value; break;
    case 0xC12554: /* move.w  (A0), D0 */
        value = m68k_read_memory_16(A(0)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12556: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC12558: /* asl.l   #3, D0 */
        step_asl_long(&D(0), 3); break;
    case 0xC1255A: /* move.l  D0, (-$12,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1255E: /* bra     $c125b6 */
        step_branch(pc, opcode, 1); break;
    case 0xC12560: /* move.l  #$e10, (-$12,A6) */
        value = m68ki_read_imm_32(); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12568: /* bra     $c125b6 */
        step_branch(pc, opcode, 1); break;
    case 0xC1256A: /* move.w  (-$1a,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1256E: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC12570: /* move.l  ($18,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC12574: /* asl.l   D0, D1 */
        step_asl_long(&D(1), D(0)); break;
    case 0xC12576: /* move.l  D1, $c45acc.l */
        value = D(1); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1257C: /* move.w  (-$4,A6), $c45ad0.l */
        value = m68k_read_memory_16(step_displacement(A(6))); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC12584: /* jsr     $c25980.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1258A: /* move.w  $c45ad2.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12590: /* asr.w   #6, D0 */
        renderer_asr_word(&D(0), 6); break;
    case 0xC12592: /* move.w  D0, $c45ad2.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC12598: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC1259A: /* asl.l   #1, D0 */
        step_asl_long(&D(0), 1); break;
    case 0xC1259C: /* movea.l D0, A0 */
        value = D(0); A(0) = value; break;
    case 0xC1259E: /* adda.l  #$c3db00, A0 */
        value = m68ki_read_imm_32(); A(0) += value; break;
    case 0xC125A4: /* move.w  (A0), D0 */
        value = m68k_read_memory_16(A(0)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC125A6: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC125A8: /* move.l  #$384, D1 */
        value = m68ki_read_imm_32(); D(1) = value; flags_logic_l(value); break;
    case 0xC125AE: /* sub.l   D0, D1 */
        value = D(0); step_subtract_long(&D(1), value); break;
    case 0xC125B0: /* asl.l   #3, D1 */
        step_asl_long(&D(1), 3); break;
    case 0xC125B2: /* move.l  D1, (-$12,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC125B6: /* move.b  (-$9,A6), D0 */
        value = m68k_read_memory_8(step_displacement(A(6))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC125BA: /* andi.b  #$5, D0 */
        value = m68ki_read_imm_16(); SET_B(D(0), D(0) & value); flags_logic_b(D(0)); break;
    case 0xC125BE: /* move.b  D0, (-$a,A6) */
        value = D(0); m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(value); break;
    case 0xC125C2: /* subq.b  #5, D0 */
        value = 5u; step_subtract_byte(&D(0), value); break;
    case 0xC125C4: /* bne     $c125d6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC125C6: /* move.l  #$3840, D0 */
        value = m68ki_read_imm_32(); D(0) = value; flags_logic_l(value); break;
    case 0xC125CC: /* sub.l   (-$12,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); step_subtract_long(&D(0), value); break;
    case 0xC125D0: /* move.l  D0, (-$12,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC125D4: /* bra     $c125fc */
        step_branch(pc, opcode, 1); break;
    case 0xC125D6: /* cmpi.b  #$4, (-$a,A6) */
        value = m68ki_read_imm_16(); result = m68k_read_memory_8(step_displacement(A(6))); step_compare_byte(value, result); break;
    case 0xC125DC: /* bne     $c125e8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC125DE: /* addi.l  #$3840, (-$12,A6) */
        value = m68ki_read_imm_32(); address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC125E6: /* bra     $c125fc */
        step_branch(pc, opcode, 1); break;
    case 0xC125E8: /* tst.b   (-$a,A6) */
        value = m68k_read_memory_8(step_displacement(A(6))); flags_logic_b(value); break;
    case 0xC125EC: /* bne     $c125fc */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC125EE: /* move.l  #$7080, D0 */
        value = m68ki_read_imm_32(); D(0) = value; flags_logic_l(value); break;
    case 0xC125F4: /* sub.l   (-$12,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); step_subtract_long(&D(0), value); break;
    case 0xC125F8: /* move.l  D0, (-$12,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC125FC: /* move.w  (-$18,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12600: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC12602: /* move.l  ($18,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC12606: /* asr.l   D0, D1 */
        step_asr_long(&D(1), D(0)); break;
    case 0xC12608: /* move.w  D1, (-$8,A6) */
        value = D(1); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC1260C: /* muls.w  D1, D1 */
        value = D(1); renderer_multiply(&D(1), (uint16_t)value); break;
    case 0xC1260E: /* move.l  ($10,A6), D2 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(2) = value; flags_logic_l(value); break;
    case 0xC12612: /* asr.l   D0, D2 */
        step_asr_long(&D(2), D(0)); break;
    case 0xC12614: /* move.w  D2, (-$8,A6) */
        value = D(2); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC12618: /* muls.w  D2, D2 */
        value = D(2); renderer_multiply(&D(2), (uint16_t)value); break;
    case 0xC1261A: /* move.l  D1, (-$16,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1261E: /* add.l   D2, D1 */
        value = D(2); step_add_long(&D(1), value); break;
    case 0xC12620: /* move.l  D1, $c45b64.l */
        value = D(1); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC12626: /* jsr     $c2564e.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1262C: /* move.w  (-$18,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12630: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC12632: /* move.l  ($14,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC12636: /* asr.l   D0, D1 */
        step_asr_long(&D(1), D(0)); break;
    case 0xC12638: /* move.w  D1, (-$2,A6) */
        value = D(1); step_write_word(step_displacement(A(6)), value); flags_logic_w(value); break;
    case 0xC1263C: /* ext.l   D1 */
        D(1) = (uint32_t)(int32_t)(int16_t)D(1); flags_logic_l(D(1)); break;
    case 0xC1263E: /* move.w  $c45b68.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12644: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC12646: /* cmp.l   D0, D1 */
        value = D(0); result = D(1); step_compare_long(value, result); break;
    case 0xC12648: /* bgt     $c12690 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC1264A: /* asl.l   #8, D1 */
        step_asl_long(&D(1), 8); break;
    case 0xC1264C: /* move.l  D1, $c45acc.l */
        value = D(1); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC12652: /* move.w  $c45b68.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12658: /* move.w  D0, $c45ad0.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1265E: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC12660: /* beq     $c12686 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC12662: /* jsr     $c25980.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC12668: /* move.w  $c45ad2.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1266E: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC12670: /* asl.l   #1, D0 */
        step_asl_long(&D(0), 1); break;
    case 0xC12672: /* movea.l D0, A0 */
        value = D(0); A(0) = value; break;
    case 0xC12674: /* adda.l  #$c3db00, A0 */
        value = m68ki_read_imm_32(); A(0) += value; break;
    case 0xC1267A: /* move.w  (A0), D0 */
        value = m68k_read_memory_16(A(0)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1267C: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC1267E: /* asl.l   #3, D0 */
        step_asl_long(&D(0), 3); break;
    case 0xC12680: /* move.l  D0, (-$e,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12684: /* bra     $c126d2 */
        step_branch(pc, opcode, 1); break;
    case 0xC12686: /* move.l  #$e10, (-$e,A6) */
        value = m68ki_read_imm_32(); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1268E: /* bra     $c126d2 */
        step_branch(pc, opcode, 1); break;
    case 0xC12690: /* move.w  $c45b68.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12696: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC12698: /* asl.l   #8, D0 */
        step_asl_long(&D(0), 8); break;
    case 0xC1269A: /* move.l  D0, $c45acc.l */
        value = D(0); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC126A0: /* move.w  (-$2,A6), $c45ad0.l */
        value = m68k_read_memory_16(step_displacement(A(6))); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC126A8: /* jsr     $c25980.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC126AE: /* move.w  $c45ad2.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC126B4: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC126B6: /* asl.l   #1, D0 */
        step_asl_long(&D(0), 1); break;
    case 0xC126B8: /* movea.l D0, A0 */
        value = D(0); A(0) = value; break;
    case 0xC126BA: /* adda.l  #$c3db00, A0 */
        value = m68ki_read_imm_32(); A(0) += value; break;
    case 0xC126C0: /* move.w  (A0), D0 */
        value = m68k_read_memory_16(A(0)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC126C2: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC126C4: /* move.l  #$384, D1 */
        value = m68ki_read_imm_32(); D(1) = value; flags_logic_l(value); break;
    case 0xC126CA: /* sub.l   D0, D1 */
        value = D(0); step_subtract_long(&D(1), value); break;
    case 0xC126CC: /* asl.l   #3, D1 */
        step_asl_long(&D(1), 3); break;
    case 0xC126CE: /* move.l  D1, (-$e,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC126D2: /* btst    #$1, (-$9,A6) */
        value = m68ki_read_imm_16(); address = step_displacement(A(6)); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; break;
    case 0xC126D8: /* bne     $c126e8 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC126DA: /* move.l  #$7080, D0 */
        value = m68ki_read_imm_32(); D(0) = value; flags_logic_l(value); break;
    case 0xC126E0: /* sub.l   (-$e,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); step_subtract_long(&D(0), value); break;
    case 0xC126E4: /* move.l  D0, (-$e,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC126E8: /* cmpi.l  #$7080, (-$e,A6) */
        value = m68ki_read_imm_32(); result = m68k_read_memory_32(step_displacement(A(6))); step_compare_long(value, result); break;
    case 0xC126F0: /* blt     $c126f6 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC126F2: /* clr.l   (-$e,A6) */
        value = 0; step_write_long(step_displacement(A(6)), value); flags_logic_l(0); break;
    case 0xC126F6: /* cmpi.l  #$7080, (-$12,A6) */
        value = m68ki_read_imm_32(); result = m68k_read_memory_32(step_displacement(A(6))); step_compare_long(value, result); break;
    case 0xC126FE: /* blt     $c12704 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC12700: /* clr.l   (-$12,A6) */
        value = 0; step_write_long(step_displacement(A(6)), value); flags_logic_l(0); break;
    case 0xC12704: /* tst.l   ($1c,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); flags_logic_l(value); break;
    case 0xC12708: /* bmi     $c1271c */
        step_branch(pc, opcode, COND_MI()); break;
    case 0xC1270A: /* move.b  $c458ae.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC12710: /* move.b  $c457a6.l, D1 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(1), value); flags_logic_b(value); break;
    case 0xC12716: /* or.b    D1, D0 */
        value = D(1); SET_B(D(0), D(0) | value); flags_logic_b(D(0)); break;
    case 0xC12718: /* tst.b   D0 */
        value = D(0); flags_logic_b(value); break;
    case 0xC1271A: /* bne     $c12734 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1271C: /* move.b  #$1, $c457a6.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC12724: /* move.l  (-$e,A6), ($8,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1272A: /* move.l  (-$12,A6), ($c,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12730: /* bra     $c12934 */
        step_branch(pc, opcode, 1); break;
    case 0xC12734: /* move.l  (-$e,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12738: /* sub.l   ($8,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); step_subtract_long(&D(0), value); break;
    case 0xC1273C: /* move.l  D0, (-$e,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12740: /* tst.l   D0 */
        value = D(0); flags_logic_l(value); break;
    case 0xC12742: /* bpl     $c127be */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC12744: /* cmpi.l  #-$3840, D0 */
        value = m68ki_read_imm_32(); result = D(0); step_compare_long(value, result); break;
    case 0xC1274A: /* bge     $c12796 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1274C: /* addi.l  #$7080, D0 */
        value = m68ki_read_imm_32(); step_add_long(&D(0), value); break;
    case 0xC12752: /* moveq   #$0, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC12754: /* move.w  (-$1c,A6), D1 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC12758: /* asr.l   D1, D0 */
        step_asr_long(&D(0), D(1)); break;
    case 0xC1275A: /* move.l  D0, (-$e,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1275E: /* move.l  ($1c,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC12762: /* cmp.l   D1, D0 */
        value = D(1); result = D(0); step_compare_long(value, result); break;
    case 0xC12764: /* ble     $c1276a */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC12766: /* move.l  D1, (-$e,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1276A: /* move.l  (-$e,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC1276E: /* add.l   ($8,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); step_add_long(&D(0), value); break;
    case 0xC12772: /* move.l  D0, (-$e,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12776: /* cmpi.l  #$7080, D0 */
        value = m68ki_read_imm_32(); result = D(0); step_compare_long(value, result); break;
    case 0xC1277C: /* blt     $c1278c */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1277E: /* subi.l  #$7080, D0 */
        value = m68ki_read_imm_32(); step_subtract_long(&D(0), value); break;
    case 0xC12784: /* move.l  D0, ($8,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12788: /* bra     $c12834 */
        step_branch(pc, opcode, 1); break;
    case 0xC1278C: /* move.l  (-$e,A6), ($8,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12792: /* bra     $c12834 */
        step_branch(pc, opcode, 1); break;
    case 0xC12796: /* move.l  (-$e,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC1279A: /* neg.l   D0 */
        renderer_negate(&D(0), 4); break;
    case 0xC1279C: /* moveq   #$0, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC1279E: /* move.w  (-$1c,A6), D1 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC127A2: /* asr.l   D1, D0 */
        step_asr_long(&D(0), D(1)); break;
    case 0xC127A4: /* move.l  D0, (-$e,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC127A8: /* move.l  ($1c,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC127AC: /* cmp.l   D1, D0 */
        value = D(1); result = D(0); step_compare_long(value, result); break;
    case 0xC127AE: /* ble     $c127b4 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC127B0: /* move.l  D1, (-$e,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC127B4: /* move.l  (-$e,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC127B8: /* sub.l   D0, ($8,A6) */
        value = D(0); address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_subtract_long(&result, value); step_write_long(address, result); break;
    case 0xC127BC: /* bra     $c12834 */
        step_branch(pc, opcode, 1); break;
    case 0xC127BE: /* move.l  (-$e,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC127C2: /* cmpi.l  #$3840, D0 */
        value = m68ki_read_imm_32(); result = D(0); step_compare_long(value, result); break;
    case 0xC127C8: /* ble     $c12810 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC127CA: /* move.l  #$7080, D1 */
        value = m68ki_read_imm_32(); D(1) = value; flags_logic_l(value); break;
    case 0xC127D0: /* sub.l   D0, D1 */
        value = D(0); step_subtract_long(&D(1), value); break;
    case 0xC127D2: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC127D4: /* move.w  (-$1c,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC127D8: /* asr.l   D0, D1 */
        step_asr_long(&D(1), D(0)); break;
    case 0xC127DA: /* move.l  D1, (-$e,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC127DE: /* move.l  ($1c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC127E2: /* cmp.l   D0, D1 */
        value = D(0); result = D(1); step_compare_long(value, result); break;
    case 0xC127E4: /* ble     $c127ea */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC127E6: /* move.l  D0, (-$e,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC127EA: /* move.l  (-$e,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC127EE: /* move.l  ($8,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC127F2: /* sub.l   D0, D1 */
        value = D(0); step_subtract_long(&D(1), value); break;
    case 0xC127F4: /* move.l  D1, (-$e,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC127F8: /* tst.l   D1 */
        value = D(1); flags_logic_l(value); break;
    case 0xC127FA: /* bpl     $c12808 */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC127FC: /* addi.l  #$7080, D1 */
        value = m68ki_read_imm_32(); step_add_long(&D(1), value); break;
    case 0xC12802: /* move.l  D1, ($8,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12806: /* bra     $c12834 */
        step_branch(pc, opcode, 1); break;
    case 0xC12808: /* move.l  (-$e,A6), ($8,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1280E: /* bra     $c12834 */
        step_branch(pc, opcode, 1); break;
    case 0xC12810: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC12812: /* move.w  (-$1c,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12816: /* move.l  (-$e,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC1281A: /* asr.l   D0, D1 */
        step_asr_long(&D(1), D(0)); break;
    case 0xC1281C: /* move.l  D1, (-$e,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12820: /* move.l  ($1c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12824: /* cmp.l   D0, D1 */
        value = D(0); result = D(1); step_compare_long(value, result); break;
    case 0xC12826: /* ble     $c1282c */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC12828: /* move.l  D0, (-$e,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1282C: /* move.l  (-$e,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12830: /* add.l   D0, ($8,A6) */
        value = D(0); address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC12834: /* move.l  (-$12,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12838: /* sub.l   ($c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); step_subtract_long(&D(0), value); break;
    case 0xC1283C: /* move.l  D0, (-$12,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12840: /* tst.l   D0 */
        value = D(0); flags_logic_l(value); break;
    case 0xC12842: /* bpl     $c128be */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC12844: /* cmpi.l  #-$3840, D0 */
        value = m68ki_read_imm_32(); result = D(0); step_compare_long(value, result); break;
    case 0xC1284A: /* bge     $c12896 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1284C: /* addi.l  #$7080, D0 */
        value = m68ki_read_imm_32(); step_add_long(&D(0), value); break;
    case 0xC12852: /* moveq   #$0, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC12854: /* move.w  (-$1c,A6), D1 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC12858: /* asr.l   D1, D0 */
        step_asr_long(&D(0), D(1)); break;
    case 0xC1285A: /* move.l  D0, (-$12,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1285E: /* move.l  ($1c,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC12862: /* cmp.l   D1, D0 */
        value = D(1); result = D(0); step_compare_long(value, result); break;
    case 0xC12864: /* ble     $c1286a */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC12866: /* move.l  D1, (-$12,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1286A: /* move.l  (-$12,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC1286E: /* add.l   ($c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); step_add_long(&D(0), value); break;
    case 0xC12872: /* move.l  D0, (-$12,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12876: /* cmpi.l  #$7080, D0 */
        value = m68ki_read_imm_32(); result = D(0); step_compare_long(value, result); break;
    case 0xC1287C: /* blt     $c1288c */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1287E: /* subi.l  #$7080, D0 */
        value = m68ki_read_imm_32(); step_subtract_long(&D(0), value); break;
    case 0xC12884: /* move.l  D0, ($c,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12888: /* bra     $c12934 */
        step_branch(pc, opcode, 1); break;
    case 0xC1288C: /* move.l  (-$12,A6), ($c,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12892: /* bra     $c12934 */
        step_branch(pc, opcode, 1); break;
    case 0xC12896: /* move.l  (-$12,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC1289A: /* neg.l   D0 */
        renderer_negate(&D(0), 4); break;
    case 0xC1289C: /* moveq   #$0, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC1289E: /* move.w  (-$1c,A6), D1 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC128A2: /* asr.l   D1, D0 */
        step_asr_long(&D(0), D(1)); break;
    case 0xC128A4: /* move.l  D0, (-$12,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC128A8: /* move.l  ($1c,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC128AC: /* cmp.l   D1, D0 */
        value = D(1); result = D(0); step_compare_long(value, result); break;
    case 0xC128AE: /* ble     $c128b4 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC128B0: /* move.l  D1, (-$12,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC128B4: /* move.l  (-$12,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC128B8: /* sub.l   D0, ($c,A6) */
        value = D(0); address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_subtract_long(&result, value); step_write_long(address, result); break;
    case 0xC128BC: /* bra     $c12934 */
        step_branch(pc, opcode, 1); break;
    case 0xC128BE: /* move.l  (-$12,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC128C2: /* cmpi.l  #$3840, D0 */
        value = m68ki_read_imm_32(); result = D(0); step_compare_long(value, result); break;
    case 0xC128C8: /* ble     $c12910 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC128CA: /* move.l  #$7080, D1 */
        value = m68ki_read_imm_32(); D(1) = value; flags_logic_l(value); break;
    case 0xC128D0: /* sub.l   D0, D1 */
        value = D(0); step_subtract_long(&D(1), value); break;
    case 0xC128D2: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC128D4: /* move.w  (-$1c,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC128D8: /* asr.l   D0, D1 */
        step_asr_long(&D(1), D(0)); break;
    case 0xC128DA: /* move.l  D1, (-$12,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC128DE: /* move.l  ($1c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC128E2: /* cmp.l   D0, D1 */
        value = D(0); result = D(1); step_compare_long(value, result); break;
    case 0xC128E4: /* ble     $c128ea */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC128E6: /* move.l  D0, (-$12,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC128EA: /* move.l  (-$12,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC128EE: /* move.l  ($c,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC128F2: /* sub.l   D0, D1 */
        value = D(0); step_subtract_long(&D(1), value); break;
    case 0xC128F4: /* move.l  D1, (-$12,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC128F8: /* tst.l   D1 */
        value = D(1); flags_logic_l(value); break;
    case 0xC128FA: /* bpl     $c12908 */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC128FC: /* addi.l  #$7080, D1 */
        value = m68ki_read_imm_32(); step_add_long(&D(1), value); break;
    case 0xC12902: /* move.l  D1, ($c,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12906: /* bra     $c12934 */
        step_branch(pc, opcode, 1); break;
    case 0xC12908: /* move.l  (-$12,A6), ($c,A6) */
        value = m68k_read_memory_32(step_displacement(A(6))); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1290E: /* bra     $c12934 */
        step_branch(pc, opcode, 1); break;
    case 0xC12910: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC12912: /* move.w  (-$1c,A6), D0 */
        value = m68k_read_memory_16(step_displacement(A(6))); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12916: /* move.l  (-$12,A6), D1 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(1) = value; flags_logic_l(value); break;
    case 0xC1291A: /* asr.l   D0, D1 */
        step_asr_long(&D(1), D(0)); break;
    case 0xC1291C: /* move.l  D1, (-$12,A6) */
        value = D(1); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC12920: /* move.l  ($1c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12924: /* cmp.l   D0, D1 */
        value = D(0); result = D(1); step_compare_long(value, result); break;
    case 0xC12926: /* ble     $c1292c */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC12928: /* move.l  D0, (-$12,A6) */
        value = D(0); step_write_long(step_displacement(A(6)), value); flags_logic_l(value); break;
    case 0xC1292C: /* move.l  (-$12,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12930: /* add.l   D0, ($c,A6) */
        value = D(0); address = step_displacement(A(6)); result = m68k_read_memory_32(address); step_add_long(&result, value); step_write_long(address, result); break;
    case 0xC12934: /* move.l  ($8,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12938: /* move.w  D0, $c45ac0.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1293E: /* move.l  ($c,A6), D0 */
        value = m68k_read_memory_32(step_displacement(A(6))); D(0) = value; flags_logic_l(value); break;
    case 0xC12942: /* move.w  D0, $c45ac2.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC12948: /* movem.l (A7)+, D2 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC1294C: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC1294E: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC2564E: /* movem.l D0-D3, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC25652: /* move.l  $c45b64.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC25658: /* cmpi.l  #$63f000, D0 */
        value = m68ki_read_imm_32(); result = D(0); step_compare_long(value, result); break;
    case 0xC2565E: /* bgt     $c25692 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC25662: /* move.l  D0, D2 */
        value = D(0); D(2) = value; flags_logic_l(value); break;
    case 0xC25664: /* divu.w  #$c8, D2 */
        value = m68ki_read_imm_16(); step_divide_unsigned(&D(2), (uint16_t)value); break;
    case 0xC25668: /* addq.w  #2, D2 */
        value = 2u; step_add_word(&D(2), value); break;
    case 0xC2566A: /* move.l  D0, D1 */
        value = D(0); D(1) = value; flags_logic_l(value); break;
    case 0xC2566C: /* divu.w  D2, D1 */
        value = D(2); step_divide_unsigned(&D(1), (uint16_t)value); break;
    case 0xC2566E: /* move.w  D1, D3 */
        value = D(1); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC25670: /* sub.w   D2, D3 */
        value = D(2); step_subtract_word(&D(3), value); break;
    case 0xC25672: /* beq     $c25686 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC25674: /* cmpi.w  #$1, D3 */
        value = m68ki_read_imm_16(); result = D(3); step_compare_word(value, result); break;
    case 0xC25678: /* beq     $c25686 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2567A: /* cmpi.w  #-$1, D3 */
        value = m68ki_read_imm_16(); result = D(3); step_compare_word(value, result); break;
    case 0xC2567E: /* beq     $c25686 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC25680: /* add.w   D1, D2 */
        value = D(1); step_add_word(&D(2), value); break;
    case 0xC25682: /* lsr.w   #1, D2 */
        SET_W(D(2), step_lsr_word_value(D(2), 1)); break;
    case 0xC25684: /* bra     $c2566a */
        step_branch(pc, opcode, 1); break;
    case 0xC25686: /* move.w  D1, $c45b68.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC2568C: /* movem.l (A7)+, D0-D3 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC25690: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC25692: /* cmpi.l  #$63f0000, D0 */
        value = m68ki_read_imm_32(); result = D(0); step_compare_long(value, result); break;
    case 0xC25698: /* bgt     $c256d0 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC2569C: /* lsr.l   #4, D0 */
        step_lsr_long(&D(0), 4); break;
    case 0xC2569E: /* move.l  D0, D2 */
        value = D(0); D(2) = value; flags_logic_l(value); break;
    case 0xC256A0: /* divu.w  #$c8, D2 */
        value = m68ki_read_imm_16(); step_divide_unsigned(&D(2), (uint16_t)value); break;
    case 0xC256A4: /* addq.w  #2, D2 */
        value = 2u; step_add_word(&D(2), value); break;
    case 0xC256A6: /* move.l  D0, D1 */
        value = D(0); D(1) = value; flags_logic_l(value); break;
    case 0xC256A8: /* divu.w  D2, D1 */
        value = D(2); step_divide_unsigned(&D(1), (uint16_t)value); break;
    case 0xC256AA: /* move.w  D1, D3 */
        value = D(1); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC256AC: /* sub.w   D2, D3 */
        value = D(2); step_subtract_word(&D(3), value); break;
    case 0xC256AE: /* beq     $c256c2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC256B0: /* cmpi.w  #$1, D3 */
        value = m68ki_read_imm_16(); result = D(3); step_compare_word(value, result); break;
    case 0xC256B4: /* beq     $c256c2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC256B6: /* cmpi.w  #-$1, D3 */
        value = m68ki_read_imm_16(); result = D(3); step_compare_word(value, result); break;
    case 0xC256BA: /* beq     $c256c2 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC256BC: /* add.w   D1, D2 */
        value = D(1); step_add_word(&D(2), value); break;
    case 0xC256BE: /* lsr.w   #1, D2 */
        SET_W(D(2), step_lsr_word_value(D(2), 1)); break;
    case 0xC256C0: /* bra     $c256a6 */
        step_branch(pc, opcode, 1); break;
    case 0xC256C2: /* lsl.w   #2, D1 */
        renderer_asl_word(&D(1), 2); FLAG_V = 0; break;
    case 0xC256C4: /* move.w  D1, $c45b68.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC256CA: /* movem.l (A7)+, D0-D3 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC256CE: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC256D0: /* lsr.l   #8, D0 */
        step_lsr_long(&D(0), 8); break;
    case 0xC256D2: /* move.l  D0, D2 */
        value = D(0); D(2) = value; flags_logic_l(value); break;
    case 0xC256D4: /* divu.w  #$c8, D2 */
        value = m68ki_read_imm_16(); step_divide_unsigned(&D(2), (uint16_t)value); break;
    case 0xC256D8: /* addq.w  #2, D2 */
        value = 2u; step_add_word(&D(2), value); break;
    case 0xC256DA: /* move.l  D0, D1 */
        value = D(0); D(1) = value; flags_logic_l(value); break;
    case 0xC256DC: /* divu.w  D2, D1 */
        value = D(2); step_divide_unsigned(&D(1), (uint16_t)value); break;
    case 0xC256DE: /* move.w  D1, D3 */
        value = D(1); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC256E0: /* sub.w   D2, D3 */
        value = D(2); step_subtract_word(&D(3), value); break;
    case 0xC256E2: /* beq     $c256f6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC256E4: /* cmpi.w  #$1, D3 */
        value = m68ki_read_imm_16(); result = D(3); step_compare_word(value, result); break;
    case 0xC256E8: /* beq     $c256f6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC256EA: /* cmpi.w  #-$1, D3 */
        value = m68ki_read_imm_16(); result = D(3); step_compare_word(value, result); break;
    case 0xC256EE: /* beq     $c256f6 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC256F0: /* add.w   D1, D2 */
        value = D(1); step_add_word(&D(2), value); break;
    case 0xC256F2: /* lsr.w   #1, D2 */
        SET_W(D(2), step_lsr_word_value(D(2), 1)); break;
    case 0xC256F4: /* bra     $c256da */
        step_branch(pc, opcode, 1); break;
    case 0xC256F6: /* lsl.w   #4, D1 */
        renderer_asl_word(&D(1), 4); FLAG_V = 0; break;
    case 0xC256F8: /* move.w  D1, $c45b68.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC256FE: /* movem.l (A7)+, D0-D3 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC25702: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC25980: /* movem.l D0-D2, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC25984: /* move.l  $c45acc.l, D0 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(0) = value; flags_logic_l(value); break;
    case 0xC2598A: /* move.w  $c45ad0.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC25990: /* move.w  D1, D2 */
        value = D(1); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC25992: /* bge     $c25996 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC25994: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC25996: /* asr.w   #1, D2 */
        renderer_asr_word(&D(2), 1); break;
    case 0xC25998: /* divs.w  D1, D0 */
        value = D(1); renderer_divide(&D(0), (int16_t)value); break;
    case 0xC2599A: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC2599C: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC2599E: /* bge     $c259a2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC259A0: /* neg.w   D0 */
        renderer_negate(&D(0), 2); break;
    case 0xC259A2: /* cmp.w   D0, D2 */
        value = D(0); result = D(2); step_compare_word(value, result); break;
    case 0xC259A4: /* bgt     $c259b4 */
        step_branch(pc, opcode, COND_GT()); break;
    case 0xC259A6: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC259A8: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC259AA: /* bge     $c259b0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC259AC: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC259AE: /* bra     $c259b6 */
        step_branch(pc, opcode, 1); break;
    case 0xC259B0: /* addq.w  #1, D0 */
        value = 1u; step_add_word(&D(0), value); break;
    case 0xC259B2: /* bra     $c259b6 */
        step_branch(pc, opcode, 1); break;
    case 0xC259B4: /* swap    D0 */
        step_swap(&D(0)); break;
    case 0xC259B6: /* move.w  D0, $c45ad2.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC259BC: /* movem.l (A7)+, D0-D2 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC259C0: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C25980_step(void) { return glue_C123FA_step(); }

int glue_C2564E_step(void) { return glue_C123FA_step(); }
