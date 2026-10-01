/* Mark both control-record banks pending; control_records.c.
 * Source CPU effects and instruction/bus/event boundaries stay in glue. */
#include "glue_renderer_step_math.h"

int glue_C1CA82_step(void) {
    uint32_t pc = REG_PC, value, address, result;
    uint16_t opcode = step_begin(pc), mask;
    switch (pc) {
    case 0xC1CA82: /* lea     $c46184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1CA88: /* move.b  #$10, D7 */
        value = m68ki_read_imm_16(); SET_B(D(7), value); flags_logic_b(value); break;
    case 0xC1CA8C: /* or.b    D7, ($1,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CA90: /* or.b    D7, ($201,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CA94: /* or.b    D7, ($401,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CA98: /* or.b    D7, ($601,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CA9C: /* or.b    D7, ($801,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAA0: /* or.b    D7, ($a01,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAA4: /* or.b    D7, ($c01,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAA8: /* or.b    D7, ($e01,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAAC: /* or.b    D7, ($1001,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAB0: /* or.b    D7, ($1201,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAB4: /* or.b    D7, ($1401,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAB8: /* or.b    D7, ($1601,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CABC: /* or.b    D7, ($1801,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAC0: /* or.b    D7, ($1a01,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAC4: /* or.b    D7, ($1c01,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAC8: /* or.b    D7, ($1e01,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CACC: /* lea     $c48184.l, A0 */
        A(0) = m68ki_read_imm_32(); break;
    case 0xC1CAD2: /* or.b    D7, ($1,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAD6: /* or.b    D7, ($21,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CADA: /* or.b    D7, ($41,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CADE: /* or.b    D7, ($61,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAE2: /* or.b    D7, ($81,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAE6: /* or.b    D7, ($a1,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAEA: /* or.b    D7, ($c1,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAEE: /* or.b    D7, ($e1,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAF2: /* or.b    D7, ($101,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAF6: /* or.b    D7, ($121,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAFA: /* or.b    D7, ($141,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CAFE: /* or.b    D7, ($161,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CB02: /* or.b    D7, ($181,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CB06: /* or.b    D7, ($1a1,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CB0A: /* or.b    D7, ($1c1,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CB0E: /* or.b    D7, ($1e1,A0) */
        value = D(7); address = step_displacement(A(0)); result = m68k_read_memory_8(address); result |= value; flags_logic_b(result); m68k_write_memory_8(address, result); break;
    case 0xC1CB12: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
