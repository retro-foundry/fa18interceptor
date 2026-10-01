/* Display-list keys, permutation and target distance; stages.c and fixed_math.c.
 * Source CPU effects and instruction/bus/event boundaries stay in glue. */
#include "glue_renderer_step_math.h"
#include "glue_unsigned_division_step.h"

/* MULU's internal work depends on the source word's set bits. */
static void terrain_multiply_unsigned(uint32_t *reg, uint16_t source) {
    uint16_t bits = source;
    while (bits) { USE_CYCLES(2); bits &= (uint16_t)(bits - 1); }
    *reg = (uint32_t)(uint16_t)*reg * source;
    flags_logic_l(*reg);
}

int glue_C1E328_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC1D90A: /* move.w  #$7fff, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1D90E: /* move.w  D1, $c45b40.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1D914: /* movem.l (A7)+, D5/D7 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC1D918: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1D91A: /* movem.l D5/D7, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC1D91E: /* move.w  $c45a72.l, D1 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1D924: /* move.l  $c45a78.l, D6 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(6) = value; flags_logic_l(value); break;
    case 0xC1D92A: /* move.w  $c45a76.l, D7 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1D930: /* move.w  $c45ab8.l, D5 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC1D936: /* asr.w   D5, D1 */
        renderer_asr_word(&D(1), D(5)); break;
    case 0xC1D938: /* asr.l   D5, D6 */
        step_asr_long(&D(6), D(5)); break;
    case 0xC1D93A: /* asr.w   D5, D7 */
        renderer_asr_word(&D(7), D(5)); break;
    case 0xC1D93C: /* add.w   D1, D2 */
        value = D(1); step_add_word(&D(2), value); break;
    case 0xC1D93E: /* bpl     $c1d942 */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC1D940: /* neg.w   D2 */
        renderer_negate(&D(2), 2); break;
    case 0xC1D942: /* tst.b   $c458bb.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC1D948: /* beq     $c1d954 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D94A: /* move.l  $c45b3c.l, D3 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); D(3) = value; flags_logic_l(value); break;
    case 0xC1D950: /* asr.l   #8, D3 */
        step_asr_long(&D(3), 8); break;
    case 0xC1D952: /* bra     $c1d958 */
        step_branch(pc, opcode, 1); break;
    case 0xC1D954: /* ext.l   D3 */
        D(3) = (uint32_t)(int32_t)(int16_t)D(3); flags_logic_l(D(3)); break;
    case 0xC1D956: /* add.l   D6, D3 */
        value = D(6); step_add_long(&D(3), value); break;
    case 0xC1D958: /* bpl     $c1d95c */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC1D95A: /* neg.l   D3 */
        renderer_negate(&D(3), 4); break;
    case 0xC1D95C: /* cmpi.l  #$7fff0, D3 */
        value = m68ki_read_imm_32(); step_compare_long(value, D(3)); break;
    case 0xC1D962: /* bge     $c1d90a */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1D964: /* add.w   D7, D4 */
        value = D(7); step_add_word(&D(4), value); break;
    case 0xC1D966: /* bpl     $c1d96a */
        step_branch(pc, opcode, COND_PL()); break;
    case 0xC1D968: /* neg.w   D4 */
        renderer_negate(&D(4), 2); break;
    case 0xC1D96A: /* asr.w   #4, D2 */
        renderer_asr_word(&D(2), 4); break;
    case 0xC1D96C: /* asr.l   #4, D3 */
        step_asr_long(&D(3), 4); break;
    case 0xC1D96E: /* asr.w   #4, D4 */
        renderer_asr_word(&D(4), 4); break;
    case 0xC1D970: /* movem.l (A7)+, D5/D7 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC1D974: /* movem.l D5/A0, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC1D978: /* moveq   #$e, D5 */
        D(5) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(5)); break;
    case 0xC1D97A: /* lea     $c1d9d8.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1D980: /* cmp.w   D2, D3 */
        value = D(2); step_compare_word(value, D(3)); break;
    case 0xC1D982: /* ble     $c1d986 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1D984: /* exg     D2, D3 */
        value = D(2); D(2) = D(3); D(3) = value; break;
    case 0xC1D986: /* ext.l   D3 */
        D(3) = (uint32_t)(int32_t)(int16_t)D(3); flags_logic_l(D(3)); break;
    case 0xC1D988: /* beq     $c1d998 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1D98A: /* asl.l   #8, D3 */
        step_asl_long(&D(3), 8); break;
    case 0xC1D98C: /* tst.w   D2 */
        value = D(2); flags_logic_w(value); break;
    case 0xC1D98E: /* bne     $c1d994 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D990: /* moveq   #$0, D3 */
        D(3) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(3)); break;
    case 0xC1D992: /* bra     $c1d998 */
        step_branch(pc, opcode, 1); break;
    case 0xC1D994: /* divu.w  D2, D3 */
        step_divide_unsigned(&D(3), (uint16_t)D(2)); break;
    case 0xC1D996: /* add.w   D3, D3 */
        value = D(3); step_add_word(&D(3), value); break;
    case 0xC1D998: /* move.w  (A0,D3.w), D3 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC1D99C: /* mulu.w  D3, D2 */
        terrain_multiply_unsigned(&D(2), (uint16_t)D(3)); break;
    case 0xC1D99E: /* ext.l   D4 */
        D(4) = (uint32_t)(int32_t)(int16_t)D(4); flags_logic_l(D(4)); break;
    case 0xC1D9A0: /* asl.l   D5, D4 */
        step_asl_long(&D(4), D(5)); break;
    case 0xC1D9A2: /* cmp.l   D2, D4 */
        value = D(2); step_compare_long(value, D(4)); break;
    case 0xC1D9A4: /* ble     $c1d9a8 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1D9A6: /* exg     D2, D4 */
        value = D(2); D(2) = D(4); D(4) = value; break;
    case 0xC1D9A8: /* asr.l   D5, D2 */
        step_asr_long(&D(2), D(5)); break;
    case 0xC1D9AA: /* tst.w   D2 */
        value = D(2); flags_logic_w(value); break;
    case 0xC1D9AC: /* bne     $c1d9b2 */
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC1D9AE: /* moveq   #$0, D4 */
        D(4) = (uint32_t)(int32_t)(int8_t)(opcode & 0xffu); flags_logic_l(D(4)); break;
    case 0xC1D9B0: /* bra     $c1d9b8 */
        step_branch(pc, opcode, 1); break;
    case 0xC1D9B2: /* divu.w  D2, D4 */
        step_divide_unsigned(&D(4), (uint16_t)D(2)); break;
    case 0xC1D9B4: /* asr.w   #6, D4 */
        renderer_asr_word(&D(4), 6); break;
    case 0xC1D9B6: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC1D9B8: /* move.w  (A0,D4.w), D1 */
        value = m68k_read_memory_16(step_indexed(A(0))); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1D9BC: /* mulu.w  D2, D1 */
        terrain_multiply_unsigned(&D(1), (uint16_t)D(2)); break;
    case 0xC1D9BE: /* asr.l   D5, D1 */
        step_asr_long(&D(1), D(5)); break;
    case 0xC1D9C0: /* movem.l (A7)+, D5/A0 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC1D9C4: /* cmpi.l  #$7fff, D1 */
        value = m68ki_read_imm_32(); step_compare_long(value, D(1)); break;
    case 0xC1D9CA: /* ble     $c1d9d0 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1D9CC: /* move.w  #$7fff, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1D9D0: /* move.w  D1, $c45b40.l */
        value = D(1); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1D9D6: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1E328: /* movem.l D0-D7/A0-A5, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC1E32C: /* tst.b   $c457a5.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); flags_logic_b(value); break;
    case 0xC1E332: /* beq     $c1e4a0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1E336: /* lea     $c459ce.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1E33C: /* move.b  $c4585d.l, D0 */
        value = m68k_read_memory_8(m68ki_read_imm_32()); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC1E342: /* subq.b  #1, D0 */
        value = 1u; step_subtract_byte(&D(0), value); break;
    case 0xC1E344: /* blt     $c1e484 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1E348: /* ext.w   D0 */
        SET_W(D(0), (int16_t)(int8_t)D(0)); flags_logic_w(D(0)); break;
    case 0xC1E34A: /* move.w  D0, D1 */
        value = D(0); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1E34C: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC1E34E: /* add.w   D0, D0 */
        value = D(0); step_add_word(&D(0), value); break;
    case 0xC1E350: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC1E352: /* add.w   D1, D0 */
        value = D(1); step_add_word(&D(0), value); break;
    case 0xC1E354: /* adda.w  D0, A0 */
        value = D(0); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1E356: /* tst.w   (A0) */
        value = m68k_read_memory_16(A(0)); flags_logic_w(value); break;
    case 0xC1E358: /* blt     $c1e4a0 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1E35C: /* lea     $c4e778.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC1E362: /* movea.l (A0)+, A1 */
        address = A(0); A(0) += 4; value = m68k_read_memory_32(address); A(1) = value; break;
    case 0xC1E364: /* lea     (A1), A2 */
        A(2) = A(1); break;
    case 0xC1E366: /* move.w  (A0)+, D0 */
        address = A(0); A(0) += 2; value = m68k_read_memory_16(address); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1E368: /* ble     $c1e3ac */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1E36A: /* move.w  D0, D7 */
        value = D(0); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1E36C: /* cmpi.w  #$16, D7 */
        value = m68ki_read_imm_16(); step_compare_word(value, D(7)); break;
    case 0xC1E370: /* ble     $c1e376 */
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC1E372: /* move.w  #$16, D7 */
        value = m68ki_read_imm_16(); SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC1E376: /* move.w  D7, D0 */
        value = D(7); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1E378: /* subq.w  #1, D0 */
        value = 1u; step_subtract_word(&D(0), value); break;
    case 0xC1E37A: /* move.w  (A1)+, D1 */
        address = A(1); A(1) += 2; value = m68k_read_memory_16(address); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1E37C: /* move.w  D1, D2 */
        value = D(1); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1E37E: /* andi.w  #$f, D2 */
        value = m68ki_read_imm_16(); result = D(2) & value; SET_W(D(2), result); flags_logic_w(result); break;
    case 0xC1E382: /* move.w  D2, $c45ab8.l */
        value = D(2); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1E388: /* btst    #$6, D1 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(1) & (1u << value); break;
    case 0xC1E38C: /* beq     $c1e39a */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1E38E: /* move.w  #$7fff, D1 */
        value = m68ki_read_imm_16(); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1E392: /* adda.w  #$a, A1 */
        value = m68ki_read_imm_16(); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1E396: /* bra     $c1e436 */
        step_branch(pc, opcode, 1); break;
    case 0xC1E39A: /* move.w  ($e,A1), D3 */
        value = m68k_read_memory_16(step_displacement(A(1))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC1E39E: /* beq     $c1e3bc */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1E3A0: /* asl.w   D2, D3 */
        renderer_asl_word(&D(3), D(2)); break;
    case 0xC1E3A2: /* adda.w  #$a, A1 */
        value = m68ki_read_imm_16(); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1E3A6: /* move.w  D3, D1 */
        value = D(3); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1E3A8: /* bra     $c1e436 */
        step_branch(pc, opcode, 1); break;
    case 0xC1E3AC: /* move.w  #$37, $c4599e.l */
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    case 0xC1E3B4: /* jsr     $c06c02.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1E3BA: /* bra     $c1e3ac */
        step_branch(pc, opcode, 1); break;
    case 0xC1E3BC: /* addq.w  #4, A1 */
        value = 4u; A(1) += value; break;
    case 0xC1E3BE: /* movem.w (A1)+, D2-D4 */
        mask = m68ki_read_imm_16(); renderer_load(A(1), mask, 2, 1); break;
    case 0xC1E3C2: /* clr.b   $c458bb.l */
        m68k_write_memory_8(m68ki_read_imm_32(), 0); flags_logic_b(0); break;
    case 0xC1E3C8: /* btst    #$4, D1 */
        value = m68ki_read_imm_16(); value &= 31u; FLAG_Z = D(1) & (1u << value); break;
    case 0xC1E3CC: /* beq     $c1e428 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1E3CE: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1E3D4: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1E3D6: /* andi.w  #$ff00, D1 */
        value = m68ki_read_imm_16(); result = D(1) & value; SET_W(D(1), result); flags_logic_w(result); break;
    case 0xC1E3DA: /* add.w   D1, D1 */
        value = D(1); step_add_word(&D(1), value); break;
    case 0xC1E3DC: /* bra     $c1e3ec */
        step_branch(pc, opcode, 1); break;
    case 0xC1E3EC: /* adda.w  D1, A0 */
        value = D(1); A(0) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1E3EE: /* andi.w  #$f, D6 */
        value = m68ki_read_imm_16(); result = D(6) & value; SET_W(D(6), result); flags_logic_w(result); break;
    case 0xC1E3F2: /* move.w  ($c,A0), D3 */
        value = m68k_read_memory_16(step_displacement(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC1E3F6: /* andi.w  #$fff, D3 */
        value = m68ki_read_imm_16(); result = D(3) & value; SET_W(D(3), result); flags_logic_w(result); break;
    case 0xC1E3FA: /* asr.w   D6, D3 */
        renderer_asr_word(&D(3), D(6)); break;
    case 0xC1E3FC: /* add.w   D3, D2 */
        value = D(3); step_add_word(&D(2), value); break;
    case 0xC1E3FE: /* move.w  ($e,A0), D3 */
        value = m68k_read_memory_16(step_displacement(A(0))); SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC1E402: /* andi.w  #$fff, D3 */
        value = m68ki_read_imm_16(); result = D(3) & value; SET_W(D(3), result); flags_logic_w(result); break;
    case 0xC1E406: /* asr.w   D6, D3 */
        renderer_asr_word(&D(3), D(6)); break;
    case 0xC1E408: /* add.w   D3, D4 */
        value = D(3); step_add_word(&D(4), value); break;
    case 0xC1E40A: /* move.l  ($10,A0), D3 */
        value = m68k_read_memory_32(step_displacement(A(0))); D(3) = value; flags_logic_l(value); break;
    case 0xC1E40E: /* move.l  D3, D1 */
        value = D(3); D(1) = value; flags_logic_l(value); break;
    case 0xC1E410: /* asr.l   D6, D3 */
        step_asr_long(&D(3), D(6)); break;
    case 0xC1E412: /* add.l   $c45a78.l, D1 */
        value = m68k_read_memory_32(m68ki_read_imm_32()); step_add_long(&D(1), value); break;
    case 0xC1E418: /* asr.l   D6, D1 */
        step_asr_long(&D(1), D(6)); break;
    case 0xC1E41A: /* move.l  D1, $c45b3c.l */
        value = D(1); step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC1E420: /* move.b  #$1, $c458bb.l */
        value = m68ki_read_imm_16(); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1E428: /* jsr     $c1d91a.l */
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC1E42E: /* move.w  $c45ab8.l, D2 */
        value = m68k_read_memory_16(m68ki_read_imm_32()); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1E434: /* asl.w   D2, D1 */
        renderer_asl_word(&D(1), D(2)); break;
    case 0xC1E436: /* move.w  D1, (A3)+ */
        value = D(1); address = A(3); A(3) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC1E438: /* adda.w  #$c, A1 */
        value = m68ki_read_imm_16(); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1E43C: /* dbra    D0, $c1e37a */
        step_dbf(pc, &D(0)); break;
    case 0xC1E440: /* bsr     $c1e4a6 */
        value = (opcode & 0xffu) ? (uint32_t)(int32_t)(int8_t)opcode : (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    case 0xC1E444: /* lea     (A2), A1 */
        A(1) = A(2); break;
    case 0xC1E446: /* lea     $c48390.l, A5 */
        A(5) = m68ki_read_imm_32(); break;
    case 0xC1E44C: /* lea     (A5), A4 */
        A(4) = A(5); break;
    case 0xC1E44E: /* subq.w  #1, D7 */
        value = 1u; step_subtract_word(&D(7), value); break;
    case 0xC1E450: /* move.w  D7, D6 */
        value = D(7); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1E452: /* movem.l (A2)+, D0-D5 */
        mask = m68ki_read_imm_16(); renderer_load(A(2), mask, 4, 2); break;
    case 0xC1E456: /* movem.l D0-D5, (A5) */
        mask = m68ki_read_imm_16(); renderer_store(A(5), mask, 4, -1); break;
    case 0xC1E45A: /* adda.w  #$18, A5 */
        value = m68ki_read_imm_16(); A(5) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1E45E: /* dbra    D6, $c1e452 */
        step_dbf(pc, &D(6)); break;
    case 0xC1E462: /* lea     $c4e828.l, A3 */
        A(3) = m68ki_read_imm_32(); break;
    case 0xC1E468: /* move.w  (A3)+, D4 */
        address = A(3); A(3) += 2; value = m68k_read_memory_16(address); SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC1E46A: /* asl.w   #3, D4 */
        renderer_asl_word(&D(4), 3); break;
    case 0xC1E46C: /* move.w  D4, D5 */
        value = D(4); SET_W(D(5), value); flags_logic_w(value); break;
    case 0xC1E46E: /* add.w   D4, D4 */
        value = D(4); step_add_word(&D(4), value); break;
    case 0xC1E470: /* add.w   D5, D4 */
        value = D(5); step_add_word(&D(4), value); break;
    case 0xC1E472: /* movem.l (A4,D4.w), D0-D5 */
        mask = m68ki_read_imm_16(); renderer_load(step_indexed(A(4)), mask, 4, -1); break;
    case 0xC1E478: /* movem.l D0-D5, (A1) */
        mask = m68ki_read_imm_16(); renderer_store(A(1), mask, 4, -1); break;
    case 0xC1E47C: /* adda.w  #$18, A1 */
        value = m68ki_read_imm_16(); A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC1E480: /* dbra    D7, $c1e468 */
        step_dbf(pc, &D(7)); break;
    case 0xC1E484: /* subq.b  #1, $c4585d.l */
        value = 1u; address = m68ki_read_imm_32(); result = m68k_read_memory_8(address); step_subtract_byte(&result, value); m68k_write_memory_8(address, result); break;
    case 0xC1E48A: /* blt     $c1e496 */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1E48C: /* tst.b   (-$2c,A6) */
        value = m68k_read_memory_8(step_displacement(A(6))); flags_logic_b(value); break;
    case 0xC1E490: /* beq     $c1e4a0 */
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC1E492: /* bra     $c1e336 */
        step_branch(pc, opcode, 1); break;
    case 0xC1E496: /* move.b  $c4585c.l, $c4585d.l */
        value = m68k_read_memory_8(m68ki_read_imm_32()); m68k_write_memory_8(m68ki_read_imm_32(), value); flags_logic_b(value); break;
    case 0xC1E4A0: /* movem.l (A7)+, D0-D7/A0-A5 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC1E4A4: /* rts */
        REG_PC = m68ki_pull_32(); break;
    case 0xC1E4A6: /* movem.l D0-D6/A0-A5, -(A7) */
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 4, 7); break;
    case 0xC1E4AA: /* lea     $c4e7a4.l, A5 */
        A(5) = m68ki_read_imm_32(); break;
    case 0xC1E4B0: /* movem.l $c4e778.l, D0-D6/A1-A4 */
        mask = m68ki_read_imm_16(); renderer_load(m68ki_read_imm_32(), mask, 4, -1); break;
    case 0xC1E4B8: /* movem.l D0-D6/A1-A4, (A5) */
        mask = m68ki_read_imm_16(); renderer_store(A(5), mask, 4, -1); break;
    case 0xC1E4BC: /* lea     ($84,A5), A4 */
        A(4) = step_displacement(A(5)); break;
    case 0xC1E4C0: /* move.w  D7, D1 */
        value = D(7); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1E4C2: /* subq.w  #1, D1 */
        value = 1u; step_subtract_word(&D(1), value); break;
    case 0xC1E4C4: /* lea     (A5), A0 */
        A(0) = A(5); break;
    case 0xC1E4C6: /* move.w  D1, D6 */
        value = D(1); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC1E4C8: /* move.w  (A0)+, D0 */
        address = A(0); A(0) += 2; value = m68k_read_memory_16(address); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1E4CA: /* bge     $c1e4d2 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1E4CC: /* dbra    D6, $c1e4c8 */
        step_dbf(pc, &D(6)); break;
    case 0xC1E4D0: /* bra     $c1e4fe */
        step_branch(pc, opcode, 1); break;
    case 0xC1E4D2: /* lea     (-$2,A0), A1 */
        A(1) = step_displacement(A(0)); break;
    case 0xC1E4D6: /* subq.w  #1, D6 */
        value = 1u; step_subtract_word(&D(6), value); break;
    case 0xC1E4D8: /* bge     $c1e4e0 */
        step_branch(pc, opcode, COND_GE()); break;
    case 0xC1E4DA: /* move.w  ($2c,A1), (A4) */
        value = m68k_read_memory_16(step_displacement(A(1))); step_write_word(A(4), value); flags_logic_w(value); break;
    case 0xC1E4DE: /* bra     $c1e4fe */
        step_branch(pc, opcode, 1); break;
    case 0xC1E4E0: /* cmp.w   (A0)+, D0 */
        address = A(0); A(0) += 2; value = m68k_read_memory_16(address); step_compare_word(value, D(0)); break;
    case 0xC1E4E2: /* blt     $c1e4ea */
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC1E4E4: /* dbra    D6, $c1e4e0 */
        step_dbf(pc, &D(6)); break;
    case 0xC1E4E8: /* bra     $c1e4f4 */
        step_branch(pc, opcode, 1); break;
    case 0xC1E4EA: /* lea     (-$2,A0), A1 */
        A(1) = step_displacement(A(0)); break;
    case 0xC1E4EE: /* move.w  (A1), D0 */
        value = m68k_read_memory_16(A(1)); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC1E4F0: /* dbra    D6, $c1e4e0 */
        step_dbf(pc, &D(6)); break;
    case 0xC1E4F4: /* move.w  #$ffff, (A1) */
        value = m68ki_read_imm_16(); step_write_word(A(1), value); flags_logic_w(value); break;
    case 0xC1E4F8: /* move.w  ($2c,A1), (A4)+ */
        value = m68k_read_memory_16(step_displacement(A(1))); address = A(4); A(4) += 2; step_write_word(address, value); flags_logic_w(value); break;
    case 0xC1E4FC: /* bra     $c1e4c4 */
        step_branch(pc, opcode, 1); break;
    case 0xC1E4FE: /* movem.l (A7)+, D0-D6/A0-A5 */
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 4, 7); break;
    case 0xC1E502: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C1E4A6_step(void) { return glue_C1E328_step(); }

int glue_C1D91A_step(void) { return glue_C1E328_step(); }

int glue_C1D974_step(void) { return glue_C1E328_step(); }
