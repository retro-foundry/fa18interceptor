/* C265E8 flagged-slot range scan: original scan, two magnitude children
 * and return boundaries replace the fixed 3500-cycle charge. Readable game
 * behavior remains flagged_slot_in_range() in fixed_math.c. Source listing:
 * generated/recomp_002.c, C265E8-C266AA. This is a temporary CPU adapter. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "ports_glue.h"

int glue_C265E8_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode = step_begin(pc), mask;
    unsigned mode = (opcode >> 3) & 7u, reg = opcode & 7u;
    unsigned destination = (opcode >> 9) & 7u, width;
    switch (pc) {
    /* moveq */
    case 0xC265E8: case 0xC26602: case 0xC266A8:
        D(destination) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    /* lea */
    case 0xC265EA: case 0xC2661E: case 0xC26626: case 0xC26658:
        A(destination) = cache_step_address(mode, reg, 4); break;
    /* move.w */
    case 0xC265F0: case 0xC26616: case 0xC26656: case 0xC26664:
    case 0xC26670: case 0xC266A0:
        width = 2; goto move;
    /* asl.w */
    case 0xC265F2: case 0xC2661A:
        renderer_asl_word(&D(reg), destination ? destination : 8); break;
    /* adda.w */
    case 0xC265F4: case 0xC26624:
        A(destination) += (uint32_t)(int32_t)(int16_t)cache_step_read(mode, reg, 2); break;
    /* btst */
    case 0xC265F6: case 0xC26606:
        value = 1u << (m68ki_read_imm_16() & 7u); FLAG_Z = cache_step_read(mode, reg, 1) & value; break;
    /* bne */
    case 0xC265FC: case 0xC2660C:
        step_branch(pc, opcode, COND_NE()); break;
    /* dbra */
    case 0xC265FE:
        step_dbf(pc, &D(reg)); break;
    /* rts */
    case 0xC26604: case 0xC266AA:
        REG_PC = m68ki_pull_32(); break;
    /* tst.b */
    case 0xC2660E:
        cache_step_logic(cache_step_read(mode, reg, 1), 1); break;
    /* beq */
    case 0xC26614:
        step_branch(pc, opcode, COND_EQ()); break;
    /* add.w */
    case 0xC2661C:
        step_add_word(&D(destination), (uint16_t)cache_step_read(mode, reg, 2)); break;
    /* movem.l */
    case 0xC2662C: case 0xC2665E:
        mask = m68ki_read_imm_16(); address = cache_step_address(mode, reg, 4); renderer_load(address, mask, 4, -1); break;
    /* sub.l */
    case 0xC26632: case 0xC2663A: case 0xC26642: case 0xC2667C:
    case 0xC26684: case 0xC2668C:
        step_subtract_long(&D(destination), cache_step_read(mode, reg, 4)); break;
    /* bge */
    case 0xC26636: case 0xC2663E: case 0xC26646: case 0xC26680:
    case 0xC26688: case 0xC26690:
        step_branch(pc, opcode, COND_GE()); break;
    /* neg.l */
    case 0xC26638: case 0xC26640: case 0xC26648: case 0xC26682:
    case 0xC2668A: case 0xC26692:
        renderer_negate(&D(reg), 4); break;
    /* asr.l */
    case 0xC2664A: case 0xC2664C: case 0xC2664E: case 0xC26694:
    case 0xC26696: case 0xC26698:
        step_asr_long(&D(reg), destination ? destination : 8); break;
    /* jsr */
    case 0xC26650: case 0xC2669A:
        address = cache_step_address(mode, reg, 4); m68ki_push_32(REG_PC); REG_PC = address; break;
    /* ext.l */
    case 0xC26668: case 0xC26674:
        D(reg) = (uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    /* swap */
    case 0xC2666A: case 0xC26676:
        step_swap(&D(reg)); break;
    /* asl.l */
    case 0xC2666C: case 0xC26678:
        step_asl_long(&D(reg), destination ? destination : 8); break;
    /* add.l */
    case 0xC2666E: case 0xC2667A:
        step_add_long(&D(destination), cache_step_read(mode, reg, 4)); break;
    /* cmp.w */
    case 0xC266A2:
        step_compare_word((uint16_t)cache_step_read(mode, reg, 2), (uint16_t)D(destination)); break;
    /* bgt */
    case 0xC266A4:
        step_branch(pc, opcode, COND_GT()); break;
    default: return 0;
    }
    goto finish;
move:
    value = cache_step_read(mode, reg, width);
    cache_step_write((opcode >> 6) & 7u, destination, width, value);
    if (((opcode >> 6) & 7u) != 1) cache_step_logic(value, width);
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
