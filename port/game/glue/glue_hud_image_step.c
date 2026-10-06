/* Source timing for C30EAA's four-plane image blit (hud_bars.c).
 * Preserve bound-span, blitter-wait and per-plane submit boundaries; the
 * shared C30F46-C30F6E tail already belongs to the HUD stream timing bridge.
 * Original instruction listing: generated/recomp_003.c, C30EAA-C30F72. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_hud_stream.h"
#include "ports_glue.h"

int glue_C30EAA_step(void) {
    uint32_t pc = REG_PC, value, address;
    uint16_t opcode;
    unsigned mode, reg, destination, width;
    if (pc >= 0xC30F46 && pc <= 0xC30F6E) return glue_C30F46_step();
    opcode = step_begin(pc);
    mode = (opcode >> 3) & 7u; reg = opcode & 7u;
    destination = (opcode >> 9) & 7u;
    switch (pc) {
    /* movea.l */
    case 0xC30EAA: case 0xC30EB0: case 0xC30EFA:
        width = 4; goto move;
    /* add.l */
    case 0xC30EB6: case 0xC30EE8: case 0xC30EEA: case 0xC30EF8:
    case 0xC30EFE: case 0xC30F00:
        step_add_long(&D(destination), cache_step_read(mode, reg, 4)); break;
    /* bsr */
    case 0xC30EBC: case 0xC30F3A: case 0xC30F3E: case 0xC30F42:
        value = opcode & 0xffu; if (!value) value = (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value = (uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC = pc + 2 + value; break;
    /* blt */
    case 0xC30EC0:
        step_branch(pc, opcode, COND_LT()); break;
    /* add.w */
    case 0xC30EC4: case 0xC30EC6: case 0xC30ECC: case 0xC30ED4:
    case 0xC30F30:
        step_add_word(&D(destination), (uint16_t)cache_step_read(mode, reg, 2)); break;
    /* ext.l */
    case 0xC30EC8: case 0xC30ED8:
        D(reg) = (uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    /* sub.w */
    case 0xC30ECA: case 0xC30F26:
        step_subtract_word(&D(destination), (uint16_t)cache_step_read(mode, reg, 2)); break;
    /* addq.w */
    case 0xC30ECE:
        step_add_word(&D(reg), destination ? destination : 8); break;
    /* swap */
    case 0xC30ED0: case 0xC30ED6: case 0xC30F24: case 0xC30F28:
        step_swap(&D(reg)); break;
    /* move.w */
    case 0xC30ED2: case 0xC30F0E: case 0xC30F14: case 0xC30F1A:
    case 0xC30F20: case 0xC30F2A: case 0xC30F32: case 0xC30F36:
        width = 2; goto move;
    /* cmpi.l */
    case 0xC30EDA:
        value = m68ki_read_imm_32(); step_compare_long(value, D(reg)); break;
    /* bge */
    case 0xC30EE0: case 0xC30EF0:
        step_branch(pc, opcode, COND_GE()); break;
    /* addi.l */
    case 0xC30EE2:
        value = m68ki_read_imm_32(); step_add_long(&D(reg), value); break;
    /* subi.w */
    case 0xC30EEC:
        value = m68ki_read_imm_16(); step_subtract_word(&D(reg), (uint16_t)value); break;
    /* bra */
    case 0xC30EF2:
        step_branch(pc, opcode, 1); break;
    /* move.l */
    case 0xC30EF6: case 0xC30EFC:
        width = 4; goto move;
    /* lea */
    case 0xC30F02:
        A(destination) = cache_step_address(mode, reg, 4); break;
    /* jsr */
    case 0xC30F08:
        address = cache_step_address(mode, reg, 4); m68ki_push_32(REG_PC); REG_PC = address; break;
    /* subq.w */
    case 0xC30F2E:
        step_subtract_word(&D(reg), destination ? destination : 8); break;
    /* moveq */
    case 0xC30F70:
        D(destination) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    /* rts */
    case 0xC30F72:
        REG_PC = m68ki_pull_32(); break;
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
