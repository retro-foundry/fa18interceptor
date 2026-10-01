/* C06132: game interrupt-server count and data-pointer result.
 * The domain counter stays in interrupts.c; CPU flags, the original branch
 * and instruction/bus/event boundaries stay in this bridge. */
#include "glue_step.h"

int glue_C06132_step(void) {
    uint32_t pc = REG_PC, address, value;
    uint16_t opcode = step_begin(pc);
    switch (pc) {
    case 0xC06132: /* nop */
        break;
    case 0xC06134: /* bra $c0616e */
        step_branch(pc, opcode, 1); break;
    case 0xC0616E: /* addi.w #$1, ($20,A6) */
        value = m68ki_read_imm_16(); address = step_displacement(A(6));
        {
            uint32_t count = m68k_read_memory_16(address);
            step_add_word(&count, value); step_write_word(address, count);
        }
        break;
    case 0xC06174: /* move.l A6, D0 */
        D(0) = A(6); flags_logic_l(D(0)); break;
    case 0xC06176: /* rts */
        REG_PC = m68ki_pull_32(); break;
    default: return 0;
    }
    USE_CYCLES(CYC_INSTRUCTION[opcode]);
    return 1;
}
