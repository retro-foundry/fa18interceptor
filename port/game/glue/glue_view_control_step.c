/* View controls, view-key queue and cockpit redraw; stages.c, player_input.c and view.c.
 * CPU effects and source instruction/bus/event boundaries stay in glue. */
#include "glue_renderer_step_math.h"

int glue_C12098_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc);
    switch (pc) {
    case 0xC082B0: /* move.w  #$90, $c45984.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC082B8: /* moveq   #$3, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC082BA: /* move.b  D4, $c45837.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082C0: /* move.b  D4, $c45836.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082C6: /* move.b  D4, $c45839.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082CC: /* move.b  D4, $c4583a.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082D2: /* move.b  D4, $c4583e.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082D8: /* move.b  D4, $c4583f.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082DE: /* move.b  D4, $c45840.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082E4: /* move.b  D4, $c45841.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082EA: /* move.b  D4, $c4583b.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082F0: /* move.b  D4, $c4583d.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082F6: /* move.b  D4, $c45843.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC082FC: /* move.b  D4, $c45844.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC08302: /* move.b  D4, $c45845.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC08308: /* move.b  D4, $c4583c.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC0830E: /* tst.b   $c457c0.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC08314: /* bne     $c08322 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC08316: /* clr.w   $c458d8.l */
        step_write_word(m68ki_read_imm_32(), 0); flags_logic_w(0); break;
    case 0xC0831C: /* clr.l   $c45918.l */
        step_write_long(m68ki_read_imm_32(), 0); flags_logic_l(0); break;
    case 0xC08322: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC08324: /* bset    #$7, $c457dd.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); value = 1u << (value & 7u); FLAG_Z = result & value; m68k_write_memory_8(address, result | value); break;
    case 0xC0832C: /* move.w  #$80, $c45a42.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC08334: /* move.b  #$3, $c4583d.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC0833C: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC12098: /* link    A6, #-$2 */
        m68ki_push_32(A(6)); A(6) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC1209C: /* move.b  $c45785.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC120A2: /* tst.b   D0 */
        value = D(0); flags_logic_b(value); break;
    case 0xC120A4: /* bne     $c120b6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC120A6: /* move.b  $c45891.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC120AC: /* tst.b   D0 */
        value = D(0); flags_logic_b(value); break;
    case 0xC120AE: /* bne     $c120b6 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC120B0: /* jsr     $c1b906.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC120B6: /* move.w  $c458dc.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC120BC: /* moveq   #$9, D1 */
        D(1) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(1)); break;
    case 0xC120BE: /* ext.l   D0 */
        D(0) = (uint32_t)(int32_t)(int16_t)D(0); flags_logic_l(D(0)); break;
    case 0xC120C0: /* asl.l   D1, D0 */
        step_asl_long(&D(0), D(1)); break;
    case 0xC120C2: /* movea.l D0, A0 */
        value = D(0); A(0) = value; break;
    case 0xC120C4: /* adda.l  #$c46184, A0 */
        value = m68ki_read_imm_32(); A(0) += value; break;
    case 0xC120CA: /* move.b  ($62,A0), D0 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC120CE: /* andi.b  #$f0, D0 */
        value = m68ki_read_imm_16(); result = D(0) & value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC120D2: /* move.w  $c458c6.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC120D8: /* move.b  D0, (-$1,A6) */
        value = D(0); m68k_write_memory_8(step_displacement(A(6)), value); flags_logic_b(value); break;
    case 0xC120DC: /* btst    #$1, D1 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(1) & (1u << value); break;
    case 0xC120E0: /* beq     $c1219c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC120E4: /* andi.w  #$fffd, D1 */
        value = m68ki_read_imm_16(); result = D(1) & value; SET_W(D(1), result); flags_logic_w(result); break;
    case 0xC120E8: /* move.w  D1, $c458c6.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC120EE: /* move.b  #$fe, $c458b0.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC120F6: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC120FC: /* beq     $c1211c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC120FE: /* moveq   #$0, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC12100: /* move.b  D0, $c45785.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC12106: /* clr.w   $c45986.l */
        step_write_word(m68ki_read_imm_32(), 0); flags_logic_w(0); break;
    case 0xC1210C: /* move.b  D0, $c457a6.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC12112: /* move.b  #$1, $c45835.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1211A: /* bra     $c1212e */
        step_branch(pc, opcode, 1); break;
    case 0xC1211C: /* move.b  $c45833.l, $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC12126: /* move.w  #$32, $c45986.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1212E: /* moveq   #$3, D0 */
        D(0) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(0)); break;
    case 0xC12130: /* move.l  D0, -(A7) */
        value = D(0); A(7) -= 4; m68k_write_memory_16(A(7) + 2, value); m68k_write_memory_16(A(7), value >> 16); flags_logic_l(value); break;
    case 0xC12132: /* jsr     $c17b08.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC12138: /* addq.l  #4, A7 */
        value = 4u; A(7) += value; break;
    case 0xC1213A: /* cmpi.b  #$30, (-$1,A6) */
        value = m68ki_read_imm_16(); address = step_displacement(A(6)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC12140: /* bne     $c1214a */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC12142: /* move.w  #$32, $c45986.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1214A: /* move.w  $c45986.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC12150: /* asl.w   #4, D0 */
        renderer_asl_word(&D(0), 4); break;
    case 0xC12152: /* move.w  D0, $c45988.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC12158: /* jsr     $c08324.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1215E: /* clr.b   $c457a7.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC12164: /* move.b  #$ff, $c45858.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1216C: /* jsr     $c1ba86.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC12172: /* cmpi.b  #$30, (-$1,A6) */
        value = m68ki_read_imm_16(); address = step_displacement(A(6)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC12178: /* beq     $c12182 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1217A: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC12180: /* beq     $c1219c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC12182: /* tst.b   $c457ad.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC12188: /* beq     $c12194 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1218A: /* move.w  #$b3, $c45984.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC12192: /* bra     $c1219c */
        step_branch(pc, opcode, 1); break;
    case 0xC12194: /* move.w  #$a7, $c45984.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1219C: /* cmpi.b  #$30, (-$1,A6) */
        value = m68ki_read_imm_16(); address = step_displacement(A(6)); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC121A2: /* bne     $c121e0 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC121A4: /* move.w  $c45986.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC121AA: /* tst.w   D0 */
        value = D(0); flags_logic_w(value); break;
    case 0xC121AC: /* bne     $c121e0 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC121AE: /* move.w  #$32, $c45986.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC121B6: /* move.w  #$320, $c45988.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC121BE: /* jsr     $c08324.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC121C4: /* clr.b   $c457a7.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC121CA: /* move.b  #$ff, $c45858.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC121D2: /* jsr     $c1ba86.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC121D8: /* move.w  #$a7, $c45984.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC121E0: /* tst.b   $c45785.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC121E6: /* beq     $c1223e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC121E8: /* move.w  $c458c6.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC121EE: /* btst    #$4, D0 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(0) & (1u << value); break;
    case 0xC121F2: /* beq     $c1220e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC121F4: /* andi.w  #$ffef, D0 */
        value = m68ki_read_imm_16(); result = D(0) & value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC121F8: /* move.w  D0, $c458c6.l */
        value = D(0); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC121FE: /* move.b  $c457ac.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC12204: /* eori.b  #$1, D0 */
        value = m68ki_read_imm_16(); result = D(0) ^ value; SET_B(D(0), result); flags_logic_b(result); break;
    case 0xC12208: /* move.b  D0, $c457ac.l */
        value = D(0); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1220E: /* move.b  $c458ae.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC12214: /* subq.b  #6, D0 */
        value = 6u; step_subtract_byte(&D(0), value); break;
    case 0xC12216: /* bne     $c1223e */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC12218: /* cmpi.l  #$24000, $c45c42.l */
        value = m68ki_read_imm_32(); address = m68ki_read_imm_32(); result = m68k_read_memory_32(address); step_compare_long(value, result); break;
    case 0xC12222: /* bge     $c1223e */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC12224: /* move.w  $c45984.l, D0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1222A: /* cmpi.w  #$a7, D0 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(0)); break;
    case 0xC1222E: /* beq     $c1223e */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC12230: /* move.w  #$a7, $c45984.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC12238: /* jsr     $c082b8.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1223E: /* unlk    A6 */
        A(7) = A(6); A(6) = m68ki_pull_32(); break;
    case 0xC12240: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1B906: /* clr.b   D7 */
        SET_B(D(7), 0); flags_logic_b(0); break;
    case 0xC1B908: /* move.b  #$0, $c457a8.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1B910: /* bra     $c1b9cc */
        step_branch(pc, opcode, 1); break;
    case 0xC1B9CC: /* move.b  D7, $c457a7.l */
        value = D(7); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1B9D2: /* move.b  #$3, $c45836.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1B9DA: /* jsr     $c08324.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1B9E0: /* move.w  #$ffff, $c45936.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1B9E8: /* move.b  #$ff, $c45858.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1B9F0: /* move.b  #$0, $c457a9.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1B9F8: /* move.b  #$ff, $c45891.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1BA00: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1BA06: /* adda.w  $c458de.l, A0 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1BA0C: /* move.b  ($62,A0), D4 */
        value = m68k_read_memory_8(step_displacement(A(0))); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1BA10: /* andi.b  #$f0, D4 */
        value = m68ki_read_imm_16(); result = D(4) & value; SET_B(D(4), result); flags_logic_b(result); break;
    case 0xC1BA14: /* cmpi.b  #$30, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BA18: /* beq     $c1c23c */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1BA1C: /* move.b  $c457a7.l, D4 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1BA22: /* cmpi.b  #$3, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BA26: /* blt     $c1ba64 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1BA28: /* cmpi.b  #$9, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BA2C: /* ble     $c1ba34 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1BA2E: /* cmpi.b  #$c, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BA32: /* blt     $c1ba64 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1BA34: /* move.w  #$b3, D7 */
        value = m68ki_read_imm_16(); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1BA38: /* cmpi.b  #$5, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BA3C: /* blt     $c1ba44 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1BA3E: /* cmpi.b  #$7, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BA42: /* ble     $c1ba48 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1BA44: /* move.w  #$a7, D7 */
        value = m68ki_read_imm_16(); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1BA48: /* cmp.w   $c45984.l, D7 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); step_compare_word(value, D(7)); break;
    case 0xC1BA4E: /* bne     $c1ba64 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1BA50: /* move.w  #$32, $c45986.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1BA58: /* move.w  #$320, $c45988.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1BA60: /* bra     $c1c23c */
        step_branch(pc, opcode, 1); break;
    case 0xC1BA64: /* lea     $c1bad4.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1BA6A: /* move.b  $c457a7.l, D4 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1BA70: /* ext.w   D4 */
        SET_W(D(4), (int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case 0xC1BA72: /* move.b  (A0,D4.w), D4 */
        value = m68k_read_memory_8(step_indexed(A(0))); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1BA76: /* ext.w   D4 */
        SET_W(D(4), (int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case 0xC1BA78: /* move.w  D4, $c45986.l */
        value = D(4); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1BA7E: /* asl.w   #4, D4 */
        renderer_asl_word(&D(4), 4); break;
    case 0xC1BA80: /* move.w  D4, $c45988.l */
        value = D(4); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1BA86: /* jsr     $c082b8.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1BA8C: /* move.b  $c457a7.l, D4 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1BA92: /* cmpi.b  #$3, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BA96: /* blt     $c1baa4 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1BA98: /* cmpi.b  #$9, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BA9C: /* ble     $c1bab0 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1BA9E: /* cmpi.b  #$c, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BAA2: /* bge     $c1bab0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1BAA4: /* move.w  #$90, $c45984.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1BAAC: /* bra     $c1c23c */
        step_branch(pc, opcode, 1); break;
    case 0xC1BAB0: /* cmpi.b  #$4, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BAB4: /* ble     $c1bac8 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1BAB6: /* cmpi.b  #$8, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1BABA: /* bge     $c1bac8 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1BABC: /* move.w  #$b3, $c45984.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1BAC4: /* bra     $c1c23c */
        step_branch(pc, opcode, 1); break;
    case 0xC1BAC8: /* move.w  #$a7, $c45984.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1BAD0: /* bra     $c1c23c */
        step_branch(pc, opcode, 1); break;
    case 0xC1C23C: /* tst.b   $c457a3.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC1C242: /* bne     $c1c2a4 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1C244: /* btst    #$7, D0 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(0) & (1u << value); break;
    case 0xC1C248: /* bne     $c1c2a4 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1C24A: /* move.b  #$1, $c457a3.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1C252: /* cmpi.b  #$a, $c457f9.l */
        value = m68ki_read_imm_16(); address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); step_compare_byte(value, result); break;
    case 0xC1C25A: /* bge     $c1c2a4 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1C25C: /* move.b  $c457f7.l, D4 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1C262: /* cmpi.b  #$a, D4 */
        value = m68ki_read_imm_16(); step_compare_byte(value, D(4)); break;
    case 0xC1C266: /* blt     $c1c26a */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1C268: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC1C26A: /* ext.w   D4 */
        SET_W(D(4), (int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case 0xC1C26C: /* lea     $c457e1.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC1C272: /* move.b  D0, (A3,D4.w) */
        value = D(0); m68k_write_memory_8(step_indexed(A(3)), value); flags_logic_b(value); break;
    case 0xC1C276: /* andi.w  #$ff, D0 */
        value = m68ki_read_imm_16(); result = D(0) & value; SET_W(D(0), result); flags_logic_w(result); break;
    case 0xC1C27A: /* lea     $c331ce.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC1C280: /* move.b  (A3,D0.w), D0 */
        value = m68k_read_memory_8(step_indexed(A(3))); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC1C284: /* addq.b  #1, D4 */
        value = 1u; renderer_add_byte(&D(4), value); break;
    case 0xC1C286: /* move.b  D4, $c457f7.l */
        value = D(4); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1C28C: /* addq.b  #1, $c457f9.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); renderer_add_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC1C292: /* lea     $c457eb.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC1C298: /* move.b  $c457f6.l, D4 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(4), value); flags_logic_b(value); break;
    case 0xC1C29E: /* ext.w   D4 */
        SET_W(D(4), (int16_t)(int8_t)D(4)); flags_logic_w(D(4)); break;
    case 0xC1C2A0: /* move.b  D0, (A3,D4.w) */
        value = D(0); m68k_write_memory_8(step_indexed(A(3)), value); flags_logic_b(value); break;
    case 0xC1C2A4: /* clr.b   $c45878.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC1C2AA: /* clr.b   $c45879.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC1C2B0: /* clr.b   $c4587a.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC1C2B6: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C1B906_step(void) { return glue_C12098_step(); }

int glue_C1BA86_step(void) { return glue_C12098_step(); }

int glue_C08324_step(void) { return glue_C12098_step(); }

int glue_C082B8_step(void) { return glue_C12098_step(); }

int glue_C082B0_step(void) { return glue_C12098_step(); }
