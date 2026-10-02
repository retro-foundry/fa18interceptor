/* Workspace selector entry adapters. Readable record operations are in
 * control_records.c; register and CCR effects belong here. */
#include "glue_step.h"
#include "control_records.h"

int glue_C1EBB0(void) {
    gaddr record = workspace_record((uint16_t)D(1));
    int32_t field_0c, field_10, field_0e;
    read_record_fields(record, &field_0c, &field_10, &field_0e);
    SET_W(D(1), (D(1) & 0xFF00u) >> 3);
    A(2) = record;
    D(2) = (uint32_t)field_0c;
    D(3) = (uint32_t)field_10;
    D(4) = (uint32_t)field_0e;
    /* The masked index shifted right three clears X; final EXT.L sets NZVC. */
    FLAG_X = 0;
    flags_logic_l(D(4));
    return glue_return();
}

int glue_C1EC84(void) {
    uint32_t final_add = D(4);
    int32_t dz = add_workspace_cell_steps(A(1), (int16_t)m68ki_read_16(A(6) - 0x1C),
                                        (int16_t)m68ki_read_16(A(6) - 0x1E),
                                        &D(2), &D(4));
    D(1) = (uint32_t)dz;
    /* Reconstruct final ADD.L flags without repeating any record reads. */
    step_add_long(&final_add, (uint32_t)dz);
    return glue_return();
}

int glue_C1EBB0_step(void) {
    uint32_t pc = REG_PC, value;
    uint16_t opcode;
    if (pc < 0xC1EBB0u || pc >= 0xC1EBE0u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC1EBB0:
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value);
        flags_logic_w(D(1)); break;
    case 0xC1EBB4:
        SET_W(D(1), step_lsr_word_value((uint16_t)D(1), 3)); break;
    case 0xC1EBB6:
        A(2) = m68ki_read_imm_32(); break;
    case 0xC1EBBC:
        A(2) += (uint32_t)(int32_t)(int16_t)D(1); break;
    case 0xC1EBBE:
        step_branch(pc, opcode, 1); break;
    case 0xC1EBCE:
        value = m68k_read_memory_16(step_displacement(A(2)));
        SET_W(D(2), value); flags_logic_w(value); break;
    case 0xC1EBD2:
        D(2) = (uint32_t)(int32_t)(int16_t)D(2); flags_logic_l(D(2)); break;
    case 0xC1EBD4:
        D(3) = m68k_read_memory_32(step_displacement(A(2))); flags_logic_l(D(3)); break;
    case 0xC1EBD8:
        value = m68k_read_memory_16(step_displacement(A(2)));
        SET_W(D(4), value); flags_logic_w(value); break;
    case 0xC1EBDC:
        D(4) = (uint32_t)(int32_t)(int16_t)D(4); flags_logic_l(D(4)); break;
    case 0xC1EBDE:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}

int glue_C1EC84_step(void) {
    uint32_t pc = REG_PC, value;
    uint16_t opcode;
    if (pc < 0xC1EC84u || pc >= 0xC1ECD4u) return 0;
    opcode = step_begin(pc);
    switch (pc) {
    case 0xC1EC84:
        step_predecrement_long(A(2)); flags_logic_l(A(2)); break;
    case 0xC1EC86:
        value = m68k_read_memory_16(A(1)); SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1EC88: case 0xC1ECAE: case 0xC1ECC2:
        value = m68ki_read_imm_16(); SET_W(D(1), D(1) & value);
        flags_logic_w(D(1)); break;
    case 0xC1EC8C:
        SET_W(D(1), step_lsr_word_value((uint16_t)D(1), 3)); break;
    case 0xC1EC8E:
        A(2) = m68ki_read_imm_32(); break;
    case 0xC1EC94:
        step_branch(pc, opcode, 1); break;
    case 0xC1ECA6:
        A(2) += (uint32_t)(int32_t)(int16_t)D(1); break;
    case 0xC1ECA8: case 0xC1ECBC:
        D(1) = 0; flags_logic_l(D(1)); break;
    case 0xC1ECAA: case 0xC1ECBE:
        value = m68k_read_memory_16(step_displacement(A(2)));
        SET_W(D(1), value); flags_logic_w(value); break;
    case 0xC1ECB2: case 0xC1ECC6:
        value = m68k_read_memory_16(step_displacement(A(6)));
        step_subtract_word(&D(1), value); break;
    case 0xC1ECB6: case 0xC1ECCA:
        step_swap(&D(1)); break;
    case 0xC1ECB8: case 0xC1ECCC:
        step_asr_long(&D(1), 2); break;
    case 0xC1ECBA:
        step_add_long(&D(2), D(1)); break;
    case 0xC1ECCE:
        step_add_long(&D(4), D(1)); break;
    case 0xC1ECD0:
        A(2) = m68k_read_memory_32(A(7)); A(7) += 4; break;
    case 0xC1ECD2:
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
