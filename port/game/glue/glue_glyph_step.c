/* Source-timed glyph rows ($C32806 and $C330FE).
 * text.c owns the readable mask operations. This glue keeps each byte read,
 * plane longword read/write and DBRA boundary observable by the chipset. */
#include "glue_step.h"

static void glyph_shift_word(uint32_t *reg, int left) {
    uint32_t old = *reg & 0xFFFFu, value;
    if (left) {
        value = (old << 8) & 0xFFFFu; flags_logic_w(value);
        FLAG_X = FLAG_C = ((old >> 8) & 1u) << 8;
        old &= 0xFF80u; FLAG_V = (old != 0 && old != 0xFF80u) << 7;
        USE_CYCLES(8 << CYC_SHIFT);
    } else {
        value = old >> 6; flags_logic_w(value);
        FLAG_X = FLAG_C = ((old >> 5) & 1u) << 8;
        USE_CYCLES(6 << CYC_SHIFT);
    }
    SET_W(*reg, value);
}

static int glyph_instruction(void) {
    uint32_t pc = REG_PC, value;
    uint16_t opcode = step_begin(pc);
    switch (pc) {
    case 0xC330FE: step_predecrement_long(D(0)); flags_logic_l(D(0)); break;
    case 0xC33100: step_predecrement_long(D(6)); flags_logic_l(D(6)); break;
    case 0xC32806: step_save_registers(); break;
    case 0xC3280A: case 0xC3280E: case 0xC3289A:
    case 0xC33102: case 0xC33106: case 0xC3315E: step_swap(&D(2)); break;
    case 0xC3280C: case 0xC33104: SET_W(D(2), D(3)); flags_logic_w(D(2)); break;
    case 0xC32810: case 0xC33108: SET_W(D(6), D(7)); flags_logic_w(D(6)); break;
    case 0xC32812: case 0xC3310A: A(0) = D(4); break;
    case 0xC32814: case 0xC3310C: A(3) = D(1); break;
    case 0xC32816: case 0xC3310E: glyph_shift_word(&D(6), 0); break;
    case 0xC32818: case 0xC33110: step_subtract_word(&D(6), 1); break;
    case 0xC3281A: case 0xC33112: SET_W(D(0), D(2)); flags_logic_w(D(0)); break;
    case 0xC3281C: case 0xC33114:
        value = ((D(2) << 4) | ((D(2) & 0xFFFFu) >> 12)) & 0xFFFFu;
        SET_W(D(2), value); flags_logic_w(value); FLAG_C = (value & 1u) << 8;
        USE_CYCLES(4 << CYC_SHIFT); break;
    case 0xC3281E: case 0xC33116:
        value = m68ki_read_imm_16(); SET_W(D(2), D(2) & value); flags_logic_w(D(2)); break;
    case 0xC32822: case 0xC32828: case 0xC3311A:
        value = m68ki_read_imm_16(); SET_W(D(0), D(0) & value); flags_logic_w(D(0)); break;
    case 0xC32826: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC3282C: case 0xC3311E: step_branch(pc, opcode, COND_NE()); break;
    case 0xC3282E: case 0xC32858: case 0xC32880:
        D(1) = m68ki_read_imm_32(); flags_logic_l(D(1)); break;
    case 0xC32834: case 0xC3285E: case 0xC33120: case 0xC3313E:
        D(0) = 0; flags_logic_l(0); break;
    case 0xC32836: case 0xC32860: case 0xC33122: case 0xC33140:
        value = m68k_read_memory_8(A(0)); SET_B(D(0), value); flags_logic_b(value); break;
    case 0xC32838: case 0xC32862:
        D(0) = (D(0) >> 8) | (D(0) << 24); flags_logic_l(D(0));
        FLAG_C = D(0) >> 23; USE_CYCLES(8 << CYC_SHIFT); break;
    case 0xC33124: case 0xC33142: glyph_shift_word(&D(0), 1); break;
    case 0xC33126: case 0xC33144: step_swap(&D(0)); break;
    case 0xC3283A: case 0xC32864: case 0xC32886: step_lsr_long(&D(1), D(2)); break;
    case 0xC3283C: case 0xC32866: case 0xC3312A: case 0xC33148: step_lsr_long(&D(0), D(2)); break;
    case 0xC3283E: case 0xC32868: case 0xC32888: case 0xC33128: case 0xC33146:
        D(3) = m68k_read_memory_32(A(3)); flags_logic_l(D(3)); break;
    case 0xC32840: case 0xC3312C: case 0xC3314A: case 0xC3314E:
        D(0) = ~D(0); flags_logic_l(D(0)); break;
    case 0xC32842: case 0xC3286A: D(0) &= D(1); flags_logic_l(D(0)); break;
    case 0xC32844: case 0xC3286C: case 0xC3288A: D(1) = ~D(1); flags_logic_l(D(1)); break;
    case 0xC32846: case 0xC3286E: case 0xC3288C: D(1) &= D(3); flags_logic_l(D(1)); break;
    case 0xC32848: case 0xC32870: D(1) |= D(0); flags_logic_l(D(1)); break;
    case 0xC3312E: case 0xC3314C: D(3) &= D(0); flags_logic_l(D(3)); break;
    case 0xC33150: D(3) |= D(0); flags_logic_l(D(3)); break;
    case 0xC3284A: case 0xC32872: case 0xC3288E: step_write_long(A(3), D(1)); flags_logic_l(D(1)); break;
    case 0xC33130: case 0xC33152: step_write_long(A(3), D(3)); flags_logic_l(D(3)); break;
    case 0xC3284C: case 0xC32874: case 0xC32890: case 0xC33132: case 0xC33154: A(0) += 1; break;
    case 0xC3284E: case 0xC32876: case 0xC32892: case 0xC33134: case 0xC33156:
        A(3) += (int16_t)m68ki_read_imm_16(); break;
    case 0xC32852: case 0xC3287A: case 0xC32896: case 0xC33138: case 0xC3315A:
        step_dbf(pc, &D(6)); break;
    case 0xC32856: case 0xC3287E: case 0xC3313C: step_branch(pc, opcode, 1); break;
    case 0xC3289C: case 0xC33160: SET_W(D(3), D(2)); flags_logic_w(D(3)); break;
    case 0xC3289E: case 0xC33162: step_swap(&D(6)); break;
    case 0xC328A0: step_restore_registers(); break;
    case 0xC33164:
        D(6) = m68k_read_memory_32(A(7)); A(7) += 4; flags_logic_l(D(6)); break;
    case 0xC33166:
        D(0) = m68k_read_memory_32(A(7)); A(7) += 4; flags_logic_l(D(0)); break;
    case 0xC328A4: case 0xC33168: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}

int glue_C330FE_step(void) { return REG_PC >= 0xC330FE && REG_PC < 0xC3316A ? glyph_instruction() : 0; }
int glue_C32806_step(void) { return REG_PC >= 0xC32806 && REG_PC < 0xC328A6 ? glyph_instruction() : 0; }
