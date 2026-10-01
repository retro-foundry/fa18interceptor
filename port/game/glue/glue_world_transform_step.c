/* Source boundaries for the three local-to-world entry variants.
 * Domain matrix behavior remains in matrix.c. The signed products retain
 * the original data-dependent multiply cycles and DMA access order. */
#include "glue_renderer_step_math.h"

int glue_C091E0_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode, mask;
    if (pc < 0xC091A8u || pc >= 0xC0924Au) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC091A8: case 0xC091CE: case 0xC091E0:
        step_save_registers(); break;
    case 0xC091AC:
        mask = m68ki_read_imm_16(); renderer_store(A(7), mask, 2, 7); break;
    case 0xC091B0:
        address = m68ki_read_imm_32(); m68ki_push_32(REG_PC); REG_PC = address; break;
    case 0xC091B6:
        mask = m68ki_read_imm_16(); renderer_load(A(7), mask, 2, 7); break;
    case 0xC091BA: case 0xC091D2:
        A(1) = m68ki_read_imm_32(); break;
    case 0xC091C0: case 0xC091D8:
        value = m68k_read_memory_16(m68ki_read_imm_32());
        A(1) += (uint32_t)(int32_t)(int16_t)value; break;
    case 0xC091C6:
        A(2) = m68ki_read_imm_32(); break;
    case 0xC091CC: case 0xC091DE: case 0xC091E4:
        step_branch(pc, opcode, 1); break;
    case 0xC091F0:
        A(2) = A(1); break;
    case 0xC091F2:
        A(2) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC091F6: case 0xC091F8: case 0xC091FA:
    case 0xC0920A: case 0xC0920C: case 0xC0920E: case 0xC09220:
        value = D(opcode & 7u); SET_W(D((opcode >> 9) & 7u), value);
        flags_logic_w(value); break;
    case 0xC091FC:
        renderer_multiply(&D(6), m68k_read_memory_16(A(2))); break;
    case 0xC091FE: case 0xC09202: case 0xC09210: case 0xC09214:
    case 0xC09218: case 0xC09222: case 0xC09226: case 0xC0922A:
        value = m68k_read_memory_16(step_displacement(A(2)));
        renderer_multiply(&D((opcode >> 9) & 7u), value); break;
    case 0xC09206: case 0xC09208: case 0xC0921C: case 0xC0921E:
    case 0xC0922E: case 0xC09230:
        value = D(opcode & 7u); step_add_long(&D((opcode >> 9) & 7u), value); break;
    case 0xC09232: case 0xC09234: case 0xC09236:
        step_asr_long(&D(opcode & 7u), 4); break;
    case 0xC09238: case 0xC0923C: case 0xC09240:
        value = m68k_read_memory_32(step_displacement(A(1)));
        step_add_long(&D((opcode >> 9) & 7u), value); break;
    case 0xC09244:
        step_restore_registers(); break;
    case 0xC09248:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C091CE_step(void) { return glue_C091E0_step(); }
int glue_C091A8_step(void) { return glue_C091E0_step(); }
