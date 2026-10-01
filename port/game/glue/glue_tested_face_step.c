/* Resumable source timing for the indexed tested-face command ($C2005C).
 * draw_stream.c remains the readable operation. The face test and polygon
 * clipper continue through their registered child boundaries. */
#include "glue_step.h"

static void tested_face_jsr(uint32_t address) {
    m68ki_push_32(REG_PC);
    m68ki_jump(address);
}

static void tested_face_add_word_memory(uint32_t address, uint16_t amount) {
    uint16_t old = m68k_read_memory_16(address);
    uint32_t result = (uint32_t)old + amount;
    m68k_write_memory_16(address, result);
    FLAG_N = NFLAG_16(result); FLAG_Z = result & 0xFFFFu;
    FLAG_V = VFLAG_ADD_16(amount, old, result);
    FLAG_X = FLAG_C = CFLAG_16(result);
}

static void tested_face_copy_vertex_long(void) {
    uint32_t value = m68k_read_memory_32(A(4));
    A(4) += 4; m68k_write_memory_32(A(0), value); A(0) += 4;
    flags_logic_l(value);
}

static void tested_face_copy_vertex_word(void) {
    uint16_t value = m68k_read_memory_16(A(4));
    m68k_write_memory_16(A(0), value); A(0) += 2;
    flags_logic_w(value);
}

int glue_C2005C_step(void) {
    uint32_t pc = REG_PC, address, value;
    uint16_t opcode;
    if (pc < 0xC2005Cu || pc >= 0xC200F6u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC2005C: A(0) = m68ki_read_imm_32(); break;
    case 0xC20062:
        m68k_write_memory_16(A(0), 0); A(0) += 2; flags_logic_w(0); break;
    case 0xC20064: A(0) += 2; break;
    case 0xC20066: A(3) = m68ki_read_imm_32(); break;
    case 0xC2006C: D(7) = 3; flags_logic_l(D(7)); break;
    case 0xC2006E: case 0xC2007A: case 0xC20086:
        value = m68k_read_memory_16(A(2)); A(2) += 2;
        SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC20070: case 0xC2007C: case 0xC2008A: case 0xC2009C:
        A(4) = step_indexed(A(3)); break;
    case 0xC20074: case 0xC20080: case 0xC2008E: case 0xC200A0:
        tested_face_copy_vertex_long(); break;
    case 0xC20076:
        value = m68k_read_memory_16(A(4)); SET_W(D(6), value); flags_logic_w(value); break;
    case 0xC20078:
        m68k_write_memory_16(A(0), D(6)); A(0) += 2; flags_logic_w(D(6)); break;
    case 0xC20082: case 0xC20090: case 0xC200A2:
        tested_face_copy_vertex_word(); break;
    case 0xC20084: case 0xC20092: case 0xC200A4:
        SET_W(D(6), D(6) & m68k_read_memory_16(A(4))); flags_logic_w(D(6)); break;
    case 0xC20088: case 0xC200A6: case 0xC200B2:
        step_branch(pc, opcode, COND_LT()); break;
    case 0xC20094: step_add_word(&D(7), 1); break;
    case 0xC20096: case 0xC200CA: step_branch(pc, opcode, 1); break;
    case 0xC20098:
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC200A8:
        address = m68ki_read_imm_32(); m68k_write_memory_16(address, D(7));
        flags_logic_w(D(7)); break;
    case 0xC200AE:
        A(3) = (uint32_t)(int32_t)(int16_t)m68k_read_memory_16(A(2)); A(2) += 2; break;
    case 0xC200B0: SET_W(D(7), A(3)); flags_logic_w(D(7)); break;
    case 0xC200C0: case 0xC200D0:
        SET_W(D(1), A(3)); flags_logic_w(D(1)); break;
    case 0xC200D2: SET_W(D(7), D(1)); flags_logic_w(D(7)); break;
    case 0xC200B4: case 0xC200CC:
        tested_face_add_word_memory(step_displacement(A(6)), 1); break;
    case 0xC200B8: case 0xC200E6: tested_face_jsr(m68ki_read_imm_32()); break;
    case 0xC200BE: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC200C2: case 0xC200D4:
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value); flags_logic_w(D(1)); break;
    case 0xC200C6: case 0xC200D8: step_branch(pc, opcode, COND_EQ()); break;
    case 0xC200C8:
        value = m68k_read_memory_16(A(2)); A(2) += 2;
        SET_W(D(7), value); flags_logic_w(value); break;
    case 0xC200DA: A(2) += 2; break;
    case 0xC200DC:
        address = m68ki_read_imm_32(); m68k_write_memory_16(address, D(7));
        flags_logic_w(D(7)); break;
    case 0xC200E2: step_save_registers(); break;
    case 0xC200EC: step_restore_registers(); break;
    case 0xC200F0: case 0xC200F4: m68ki_jump(m68ki_pull_32()); break;
    case 0xC200F2: D(0) = 0xFFFFFFFFu; flags_logic_l(D(0)); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
