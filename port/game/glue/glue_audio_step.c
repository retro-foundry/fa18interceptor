/* Source boundaries for volume fading, the four-voice iterator and helpers.
 * audio.c owns the readable sound operations. These bridges retain the
 * source stack and Paula write timing during mixed C/generated execution. */
#include "glue_step.h"

static void voice_decrement(uint32_t address) {
    uint32_t old = m68k_read_memory_32(address), result = old - 1;
    FLAG_N = NFLAG_32(result); FLAG_Z = result;
    FLAG_V = VFLAG_SUB_32(1u, old, result);
    FLAG_X = FLAG_C = CFLAG_SUB_32(1u, old, result);
    step_write_long(address, result);
}

static int voice_instruction(void) {
    uint32_t pc = REG_PC, address, value, old, result;
    uint16_t opcode, mask;
    unsigned i, count;
    opcode = step_begin(pc);
    switch (pc) {
    /* $C24FE8: fade by the source quarter-unit step, with shared early RTS. */
    case 0xC24FE6: case 0xC25020: m68ki_jump(m68ki_pull_32()); break;
    case 0xC24FE8: flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;
    case 0xC24FEE: case 0xC24FFE: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC24FF0: D(0) = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(D(0)); break;
    case 0xC24FF6: D(1) = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(D(1)); break;
    case 0xC24FFC: step_compare_long(D(0), D(1)); break;
    case 0xC25000: case 0xC2500E: step_branch(pc, opcode, COND_GT()); break;
    case 0xC25002: step_add_long(&D(1), m68ki_read_imm_32()); break;
    case 0xC25008: step_compare_long(m68ki_read_imm_32(), D(1)); break;
    case 0xC25010: case 0xC25024: case 0xC2502C: step_branch(pc, opcode, 1); break;
    case 0xC25012: step_subtract_long(&D(1), m68ki_read_imm_32()); break;
    case 0xC25018: step_branch(pc, opcode, COND_LT()); break;
    case 0xC2501A: step_write_long(m68ki_read_imm_32(), D(1)); flags_logic_l(D(1)); break;
    case 0xC25022: D(1) = 0; flags_logic_l(0); break;
    case 0xC25026: D(1) = m68ki_read_imm_32(); flags_logic_l(D(1)); break;
    /* $C50158: save the source registers and walk four voice slots. */
    case 0xC50158:
        mask = m68ki_read_imm_16(); count = 0;
        for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
            step_predecrement_long(REG_DA[15 - i]); ++count;
        }
        USE_CYCLES(count << CYC_MOVEM_L); break;
    case 0xC5015C: D(3) = 0; flags_logic_l(0); break;
    case 0xC5015E: case 0xC4FFB6: A(0) = m68ki_read_imm_32(); break;
    case 0xC50164: step_predecrement_long(A(0)); flags_logic_l(A(0)); break;
    case 0xC50166:
        A(7) -= 2; step_write_word(A(7), D(3)); flags_logic_w(D(3)); break;
    case 0xC50168: A(1) = m68k_read_memory_32(step_displacement(A(0))); break;
    case 0xC5016C: A(2) = m68k_read_memory_32(step_displacement(A(1))); break;
    case 0xC50170: A(3) = m68k_read_memory_32(step_displacement(A(2))); break;
    case 0xC50174: step_compare_long(m68ki_read_imm_32(), A(3)); break;
    case 0xC5017A: case 0xC501A4: case 0xC501BC:
    case 0xC5021A: case 0xC5023E: case 0xC5024E: case 0xC50254:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC5017C: case 0xC50186: case 0xC50274:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC50182: A(0) = m68k_read_memory_32(step_displacement(A(1))); break;
    case 0xC5018C: case 0xC50194: case 0xC501E0: case 0xC501F4:
        D(0) = m68k_read_memory_32(step_displacement(A(3))); flags_logic_l(D(0)); break;
    case 0xC50190: case 0xC50198:
        address = step_displacement(A(3)); value = m68k_read_memory_32(address);
        step_add_long(&value, D(0)); step_write_long(address, value); break;
    case 0xC5019C: case 0xC501B4: case 0xC50212:
        value = m68ki_read_imm_32(); address = step_displacement(A(3));
        step_compare_long(value, m68k_read_memory_32(address)); break;
    case 0xC501A6: case 0xC501BE: case 0xC5021C:
        voice_decrement(step_displacement(A(3))); break;
    case 0xC501AA: case 0xC501C2: case 0xC501D8: case 0xC50220: case 0xC5026C:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC501AC: case 0xC501C4:
        value = m68ki_read_imm_32(); step_write_long(step_displacement(A(3)), value);
        flags_logic_l(value); break;
    case 0xC501CC:
        value = m68k_read_memory_16(A(7)); A(7) += 2;
        SET_W(D(3), value); flags_logic_w(value); break;
    case 0xC501CE: A(0) = m68k_read_memory_32(A(7)); A(7) += 4; break;
    case 0xC501D0: A(0) += 4; break;
    case 0xC501D2: step_add_word(&D(3), 1); break;
    case 0xC501D4: step_compare_word(m68ki_read_imm_16(), D(3)); break;
    case 0xC501DA:
        mask = m68ki_read_imm_16(); address = A(7); count = 0;
        for (i = 0; i < 16; ++i) if (mask & (1u << i)) {
            REG_DA[i] = m68k_read_memory_32(address); address += 4; ++count;
        }
        A(7) = address; USE_CYCLES(count << CYC_MOVEM_L); break;
    case 0xC501DE: case 0xC50210: case 0xC5027A: case 0xC4FFC8:
        m68ki_jump(m68ki_pull_32()); break;

    /* $C501E0: period floor and volume cap, preserving the source high half. */
    case 0xC501E4: case 0xC501F8:
        D(0) = (D(0) << 16) | (D(0) >> 16); flags_logic_l(D(0)); break;
    case 0xC501E6: step_compare_word(m68ki_read_imm_16(), D(0)); break;
    case 0xC501EA: case 0xC50234: step_branch(pc, opcode, COND_GE()); break;
    case 0xC501EC:
        value = m68ki_read_imm_16(); SET_W(D(0), value); flags_logic_w(value); break;
    case 0xC501F0: case 0xC5020C:
        step_write_word(step_displacement(A(0)), D(0)); flags_logic_w(D(0)); break;
    case 0xC501FA:
        value = m68ki_read_imm_16(); SET_W(D(0), D(0) & value); flags_logic_w(D(0)); break;
    case 0xC501FE:
        step_compare_word(m68k_read_memory_16(m68ki_read_imm_32()), D(0)); break;
    case 0xC50204: step_branch(pc, opcode, COND_LE()); break;
    case 0xC50206:
        value = m68k_read_memory_16(m68ki_read_imm_32());
        SET_W(D(0), value); flags_logic_w(value); break;

    /* $C50212: source program delay, field commands and loop counters. */
    case 0xC50222: A(0) = m68k_read_memory_32(step_displacement(A(3))); break;
    case 0xC50226: A(0) += m68k_read_memory_32(step_displacement(A(3))); break;
    case 0xC5022A: A(4) = m68k_read_memory_32(A(0)); A(0) += 4; break;
    case 0xC5022C: D(0) = m68k_read_memory_32(A(0)); A(0) += 4; flags_logic_l(D(0)); break;
    case 0xC5022E: step_compare_long(m68ki_read_imm_32(), A(4)); break;
    case 0xC50236: A(4) += A(3); break;
    case 0xC50238: step_write_long(step_displacement(A(4)), D(0)); flags_logic_l(D(0)); break;
    case 0xC5023C: case 0xC50258: step_branch(pc, opcode, 1); break;
    case 0xC50240: SET_W(D(1), A(4)); flags_logic_w(D(1)); break;
    case 0xC50242:
        value = m68ki_read_imm_16(); old = D(1) & 0xFFFFu; result = old - value;
        SET_W(D(1), result); step_compare_word(value, old); FLAG_X = FLAG_C; break;
    case 0xC50246:
        value = m68ki_read_imm_32(); address = step_indexed(A(3));
        step_compare_long(value, m68k_read_memory_32(address)); break;
    case 0xC50250: voice_decrement(step_indexed(A(3))); break;
    case 0xC50256: A(0) = D(0); break;
    case 0xC5025A: step_write_long(step_displacement(A(3)), D(0)); flags_logic_l(D(0)); break;
    case 0xC5025E: A(0) -= m68k_read_memory_32(step_displacement(A(3))); break;
    case 0xC50262: step_write_long(step_displacement(A(3)), A(0)); flags_logic_l(A(0)); break;
    case 0xC50266: step_compare_long(m68ki_read_imm_32(), D(0)); break;
    case 0xC5026E: step_write_long(step_displacement(A(2)), D(0)); flags_logic_l(D(0)); break;
    case 0xC50272: D(0) = D(3); flags_logic_l(D(0)); break;

    /* $C4FFB4: clear the interrupt belonging to the selected voice slot. */
    case 0xC4FFB4:
        old = D(0); result = old << 2; D(0) = result;
        FLAG_N = NFLAG_32(result); FLAG_Z = result; FLAG_X = FLAG_C = old >> 22;
        value = old & 0xE0000000u; FLAG_V = (value != 0 && value != 0xE0000000u) << 7;
        USE_CYCLES(2 << CYC_SHIFT); break;
    case 0xC4FFBC: A(0) = m68k_read_memory_32(step_indexed(A(0))); break;
    case 0xC4FFC0:
        value = m68k_read_memory_16(step_displacement(A(0)));
        step_write_word(m68ki_read_imm_32(), value); flags_logic_w(value); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}

int glue_C50158_step(void) { return REG_PC >= 0xC50158 && REG_PC < 0xC501E0 ? voice_instruction() : 0; }
int glue_C501E0_step(void) { return REG_PC >= 0xC501E0 && REG_PC < 0xC50212 ? voice_instruction() : 0; }
int glue_C50212_step(void) { return REG_PC >= 0xC50212 && REG_PC < 0xC5027C ? voice_instruction() : 0; }
int glue_C4FFB4_step(void) { return REG_PC >= 0xC4FFB4 && REG_PC < 0xC4FFCA ? voice_instruction() : 0; }
int glue_C24FE8_step(void) { return REG_PC >= 0xC24FE6 && REG_PC < 0xC2502E ? voice_instruction() : 0; }
