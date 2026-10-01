/* Resumable timing bridge for $C2FD8C-$C2FF44.
 *
 * active_planes.c describes the game operation. This temporary bridge also
 * preserves its source instruction boundaries while generated callers and
 * the chipset still share a CPU clock. The PC and source stack retain the
 * continuation across children, interrupts and frame ends. Each switch arm
 * executes one source instruction in C, without an opcode handler. */
#include "glue_step.h"

int glue_C2FD8C_step(void) {
    uint32_t pc = REG_PC, address, value, old, result;
    uint16_t opcode;
    if (pc < 0xC2FD8Cu || pc >= 0xC2FF46u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    /* Plane table, custom base and blit size. */
    case 0xC2FD8C: A(2) = m68k_read_memory_32(m68ki_read_imm_32()); break;
    case 0xC2FD92: case 0xC2FF22: A(0) = m68ki_read_imm_32(); break;
    case 0xC2FF28: A(4) = m68ki_read_imm_32(); break;
    case 0xC2FD98:
        value = m68k_read_memory_16(m68ki_read_imm_32());
        SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC2FD9E:
        old = D(6) & 0xFFFFu;
        result = (old << 6) & 0xFFFFu;
        SET_W(D(6), result);
        FLAG_N = NFLAG_16(result); FLAG_Z = result;
        FLAG_X = FLAG_C = old >> 2;
        value = old & 0xFE00u;
        FLAG_V = (value != 0 && value != 0xFE00u) << 7;
        USE_CYCLES(6 << CYC_SHIFT); break;
    case 0xC2FDA0: step_add_word(&D(6), m68ki_read_imm_16()); break;
    case 0xC2FDA4: D(4) = m68k_read_memory_32(A(2)); flags_logic_l(D(4)); break;
    case 0xC2FDF4: case 0xC2FE3E: case 0xC2FE94:
        D(4) = m68k_read_memory_32(step_displacement(A(2)));
        flags_logic_l(D(4)); break;
    case 0xC2FDA6: case 0xC2FDF8: case 0xC2FE42: case 0xC2FE98:
        step_add_long(&D(4), m68ki_read_imm_32()); break;
    case 0xC2FDAC: case 0xC2FDFE: case 0xC2FE48: case 0xC2FE54: case 0xC2FE9E:
        value = m68ki_read_imm_16(); SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC2FDB0: D(5) = 0xFFFFFFFFu; flags_logic_l(D(5)); break;

    /* BBUSY polls remain individual instructions, including the initial NOPs
     * and each later counter increment. DMACONR is read as a byte. */
    case 0xC2FDB2: case 0xC2FE02: case 0xC2FE58: case 0xC2FEA2:
        value = m68ki_read_imm_16();
        address = step_displacement(A(0));
        FLAG_Z = m68k_read_memory_8(address) & (1u << (value & 7)); break;
    case 0xC2FDB8: case 0xC2FE08: case 0xC2FE5E: case 0xC2FEA8:
    case 0xC2FE52: case 0xC2FF00:
        step_branch(pc, opcode, COND_EQ()); break;
    case 0xC2FDBA: case 0xC2FDBC: break;
    case 0xC2FDBE: case 0xC2FE10: case 0xC2FE66: case 0xC2FEB0:
        step_branch(pc, opcode, 1); break;
    case 0xC2FE0A: case 0xC2FE60: case 0xC2FEAA:
        address = m68ki_read_imm_32(); old = m68k_read_memory_16(address);
        result = old + 1;
        FLAG_N = NFLAG_16(result); FLAG_Z = result & 0xFFFFu;
        FLAG_V = VFLAG_ADD_16(1u, old, result);
        FLAG_X = FLAG_C = CFLAG_16(result);
        m68k_write_memory_16(address, result); break;

    /* Signed peak selection, followed by a zero-extended word store. */
    case 0xC2FE12: case 0xC2FE68: case 0xC2FEB2:
        D(0) = m68k_read_memory_32(m68ki_read_imm_32()); flags_logic_l(D(0)); break;
    case 0xC2FE18: case 0xC2FE20: case 0xC2FE6E: case 0xC2FE76:
    case 0xC2FEB8: case 0xC2FEC0:
        SET_W(D(3), D(0)); flags_logic_w(D(3)); break;
    case 0xC2FE1A: case 0xC2FE24: case 0xC2FE70: case 0xC2FE7A:
    case 0xC2FEBA: case 0xC2FEC4:
        D(0) = (D(0) << 16) | (D(0) >> 16); flags_logic_l(D(0)); break;
    case 0xC2FE1C: case 0xC2FE72: case 0xC2FEBC:
        old = D(0) & 0xFFFFu; value = D(3) & 0xFFFFu; result = old - value;
        FLAG_N = NFLAG_16(result); FLAG_Z = result & 0xFFFFu;
        FLAG_V = VFLAG_SUB_16(value, old, result); FLAG_C = CFLAG_16(result); break;
    case 0xC2FE1E: case 0xC2FE74: case 0xC2FEBE:
        step_branch(pc, opcode, COND_LE()); break;
    case 0xC2FE22: case 0xC2FE78: case 0xC2FEC2:
        SET_W(D(0), 0); flags_logic_w(0); break;
    case 0xC2FE26: case 0xC2FE7C: case 0xC2FEC6:
        SET_W(D(0), D(3)); flags_logic_w(D(0)); break;
    case 0xC2FE28: case 0xC2FE7E: case 0xC2FEC8:
        step_write_long(m68ki_read_imm_32(), D(0)); flags_logic_l(D(0)); break;

    /* Blitter registers: extension reads precede the destination write. */
    case 0xC2FDC0: case 0xC2FE2E: case 0xC2FE84: case 0xC2FECE:
        step_write_word(step_displacement(A(0)), D(2)); flags_logic_w(D(2)); break;
    case 0xC2FDCA: case 0xC2FDCE: case 0xC2FDD2:
        step_write_word(step_displacement(A(0)), D(5)); flags_logic_w(D(5)); break;
    case 0xC2FDC4: case 0xC2FDD6: case 0xC2FDDC: case 0xC2FDE2:
        value = m68ki_read_imm_16(); address = step_displacement(A(0));
        step_write_word(address, value); flags_logic_w(value); break;
    case 0xC2FDE8: case 0xC2FDEC: case 0xC2FE32: case 0xC2FE36:
    case 0xC2FE88: case 0xC2FE8C: case 0xC2FED2: case 0xC2FED6:
        step_write_long(step_displacement(A(0)), D(4)); flags_logic_l(D(4)); break;
    case 0xC2FDF0: case 0xC2FE3A: case 0xC2FE90: case 0xC2FEDA:
        step_write_word(step_displacement(A(0)), D(6)); flags_logic_w(D(6)); break;
    case 0xC2FE4C: case 0xC2FEFA: case 0xC2FF12:
        flags_logic_b(m68k_read_memory_8(m68ki_read_imm_32())); break;

    /* Mask save, selectors, polygon preparation and optional composite. */
    case 0xC2FEDE:
        value = m68k_read_memory_32(m68ki_read_imm_32()); A(7) -= 4;
        m68k_write_memory_16(A(7) + 2, value);
        m68k_write_memory_16(A(7), value >> 16); flags_logic_l(value); break;
    case 0xC2FEE4:
        value = m68k_read_memory_32(step_displacement(A(2)));
        step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;
    case 0xC2FEEC: case 0xC2FF1A:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); m68ki_jump(address); break;
    case 0xC2FEF4: case 0xC2FF08:
        value = m68ki_read_imm_16(); m68ki_push_32(REG_PC);
        REG_PC = pc + 2 + (int16_t)value; break;
    case 0xC2FEF2: case 0xC2FEF8: case 0xC2FF18: case 0xC2FF20:
        step_branch(pc, opcode, COND_NE()); break;
    case 0xC2FF02: D(0) = 8; flags_logic_l(D(0)); break;
    case 0xC2FF04: D(3) = 0; flags_logic_l(0); break;
    case 0xC2FF06: D(4) = 0; flags_logic_l(0); break;
    case 0xC2FF0C:
        value = m68k_read_memory_32(A(7)); A(7) += 4;
        step_write_long(m68ki_read_imm_32(), value); flags_logic_l(value); break;

    /* Secondary record: one word then five longs, or clear its first word. */
    case 0xC2FF2E:
        value = m68k_read_memory_16(A(0)); A(0) += 2;
        step_write_word(A(4), value); A(4) += 2; flags_logic_w(value); break;
    case 0xC2FF30: case 0xC2FF32: case 0xC2FF34: case 0xC2FF36: case 0xC2FF38:
        value = m68k_read_memory_32(A(0)); A(0) += 4;
        step_write_long(A(4), value); A(4) += 4; flags_logic_l(value); break;
    case 0xC2FF3C:
        value = m68ki_read_imm_16(); step_write_word(m68ki_read_imm_32(), value);
        flags_logic_w(value); break;
    case 0xC2FF3A: case 0xC2FF44: m68ki_jump(m68ki_pull_32()); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
