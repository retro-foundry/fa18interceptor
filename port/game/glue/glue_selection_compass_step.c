/* Source timing for compass update and lost-selection cleanup.
 * The fixed 300/700-cycle pair changes drawing together even where either
 * entry alone matches. Keep operand-dependent math and the view-key child
 * at their source boundaries. Readable behavior: control_records.c and
 * player_input.c; original listings: recomp_003.c / recomp_001.c. */
#include "glue_main_loop_timers_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "ports_glue.h"

static int selection_compass_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode = step_begin(pc);
    unsigned mode = (opcode >> 3) & 7u, reg = opcode & 7u;
    unsigned destination = (opcode >> 9) & 7u, width;
    switch (pc) {
    /* lea */
    case 0xC310AA:
        A(destination) = cache_step_address(mode, reg, 4); break;
    /* move.w */
    case 0xC310B0: case 0xC310B6: case 0xC310C2: case 0xC310DA:
    case 0xC1224A: case 0xC1225E: case 0xC12268: case 0xC1226E:
    case 0xC1228E: case 0xC12294:
        width = 2; goto move;
    /* ext.l */
    case 0xC310BA: case 0xC12252:
        D(reg) = (uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    /* asr.l */
    case 0xC310BC: case 0xC310CE:
        step_asr_long(&D(reg), destination ? destination : 8); break;
    /* divu.w */
    case 0xC310BE:
        step_divide_unsigned(&D(destination), (uint16_t)cache_step_read(mode, reg, 2)); break;
    /* asr.w */
    case 0xC310C8:
        renderer_asr_word(&D(reg), destination ? destination : 8); break;
    /* mulu.w */
    case 0xC310CA:
        timer_multiply_unsigned(&D(destination), (uint16_t)cache_step_read(mode, reg, 2)); break;
    /* subi.w */
    case 0xC310D0:
        value = m68ki_read_imm_16(); step_subtract_word(&D(reg), (uint16_t)value); break;
    /* bge */
    case 0xC310D4:
        step_branch(pc, opcode, COND_GE()); break;
    /* addi.w */
    case 0xC310D6:
        value = m68ki_read_imm_16(); step_add_word(&D(reg), (uint16_t)value); break;
    /* rts */
    case 0xC310E0: case 0xC122A0:
        REG_PC = m68ki_pull_32(); break;
    /* tst.w */
    case 0xC12242:
        cache_step_logic(cache_step_read(mode, reg, 2), 2); break;
    /* beq */
    case 0xC12248:
        step_branch(pc, opcode, COND_EQ()); break;
    /* moveq */
    case 0xC12250: case 0xC12266: case 0xC1228C:
        D(destination) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    /* asl.l */
    case 0xC12254:
        step_asl_long(&D(reg), D(destination)); break;
    /* movea.l */
    case 0xC12256:
        width = 4; goto move;
    /* adda.l */
    case 0xC12258:
        A(destination) += cache_step_read(mode, reg, 4); break;
    /* btst */
    case 0xC12260:
        value = m68ki_read_imm_16(); FLAG_Z = D(reg) & (1u << (value & 31u)); break;
    /* bne */
    case 0xC12264: case 0xC12284:
        step_branch(pc, opcode, COND_NE()); break;
    /* move.b */
    case 0xC12274: case 0xC1227C:
        width = 1; goto move;
    /* tst.b */
    case 0xC12282:
        cache_step_logic(cache_step_read(mode, reg, 1), 1); break;
    /* clr.b */
    case 0xC12286:
        cache_step_write(mode, reg, 1, 0); cache_step_logic(0, 1); break;
    /* jsr */
    case 0xC1229A:
        address = cache_step_address(mode, reg, 4); m68ki_push_32(REG_PC); REG_PC = address; break;
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

int glue_C310AA_step(void) { return selection_compass_step(); }
int glue_C12242_step(void) { return selection_compass_step(); }
