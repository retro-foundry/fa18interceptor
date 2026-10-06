/* Source-timed C11BFC message update. Readable behavior remains messages.c.
 * These temporary CPU boundaries replace the fixed 3000-cycle charge;
 * original instructions: generated/recomp_001.c, C11BFC-C12096. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "ports_glue.h"

int glue_C11BFC_step(void) {
    uint32_t pc = REG_PC, value, address, old;
    uint16_t opcode = step_begin(pc);
    unsigned mode = (opcode >> 3) & 7u, reg = opcode & 7u;
    unsigned destination = (opcode >> 9) & 7u, width;
    switch (pc) {
    /* link */
    case 0xC11BFC:
        m68ki_push_32(A(reg)); A(reg) = A(7); A(7) += (uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    /* clr.b */
    case 0xC11C00: case 0xC1208E:
        cache_step_write(mode, reg, 1, 0); cache_step_logic(0, 1); break;
    /* move.w */
    case 0xC11C04: case 0xC11C0A: case 0xC11C1A: case 0xC11C20:
    case 0xC11C2C: case 0xC11C3A: case 0xC11C44: case 0xC11C50:
    case 0xC11C6A: case 0xC11C74: case 0xC11C86: case 0xC11C9C:
    case 0xC11CAE: case 0xC11CC0: case 0xC11CD2: case 0xC11CE8:
    case 0xC11CF2: case 0xC11CF8: case 0xC11D0E: case 0xC11D18:
    case 0xC11D1E: case 0xC11D2E: case 0xC11D42: case 0xC11D4C:
    case 0xC11D52: case 0xC11D5A: case 0xC11D6A: case 0xC11D74:
    case 0xC11D7C: case 0xC11D86: case 0xC11D90: case 0xC11D9A:
    case 0xC11DA4: case 0xC11DB2: case 0xC11DB6: case 0xC11DBC:
    case 0xC11DC2: case 0xC11DCC: case 0xC11DD2: case 0xC11DDC:
    case 0xC11DE2: case 0xC11DEC: case 0xC11DF2: case 0xC11DFA:
    case 0xC11E02: case 0xC11E3C: case 0xC11E4C: case 0xC11E5C:
    case 0xC11E62: case 0xC11E84: case 0xC11EAE: case 0xC11EC4:
    case 0xC11ECE: case 0xC11ED4: case 0xC11EEC: case 0xC11EF4:
    case 0xC11F10: case 0xC11F1A: case 0xC11F20: case 0xC11F58:
    case 0xC11F62: case 0xC11F68: case 0xC11F6E: case 0xC11F74:
    case 0xC11F7E: case 0xC11F84: case 0xC11F8E: case 0xC11F94:
    case 0xC11F9E: case 0xC11FA8: case 0xC11FB6: case 0xC11FC2:
    case 0xC11FCC: case 0xC11FD2: case 0xC11FF4: case 0xC1201E:
    case 0xC12024: case 0xC1202A: case 0xC12034: case 0xC1203A:
    case 0xC12044: case 0xC1204A: case 0xC12054: case 0xC1205C:
        width = 2; goto move;
    /* andi.w */
    case 0xC11C0E: case 0xC11C32: case 0xC11CEE: case 0xC11D14:
    case 0xC11D48: case 0xC11D70: case 0xC11D8C: case 0xC11DA0:
    case 0xC11DA8: case 0xC11DC8: case 0xC11DD8: case 0xC11DE8:
    case 0xC11E42: case 0xC11ECA: case 0xC11EDA: case 0xC11F5E:
    case 0xC11F7A: case 0xC11F8A: case 0xC11F9A: case 0xC12030:
    case 0xC12040: case 0xC12050:
        value = m68ki_read_imm_16(); value = cache_step_read(mode, reg, 2) & value; cache_step_write(mode, reg, 2, value); cache_step_logic(value, 2); break;
    /* move.l */
    case 0xC11C12:
        width = 4; goto move;
    /* btst */
    case 0xC11C24: case 0xC11C4A: case 0xC11C80: case 0xC11C96:
    case 0xC11CA6: case 0xC11CB8: case 0xC11CCA: case 0xC11CE2:
    case 0xC11D08: case 0xC11D26: case 0xC11D3C: case 0xC11D60:
    case 0xC11E52: case 0xC11E68: case 0xC11E8A: case 0xC11EB4:
    case 0xC11FAE: case 0xC11FBC: case 0xC11FD8: case 0xC12004:
        value = m68ki_read_imm_16(); width = mode == 0 ? 4 : 1; FLAG_Z = cache_step_read(mode, reg, width) & (1u << (value & (width == 4 ? 31 : 7))); break;
    /* bne */
    case 0xC11C28: case 0xC11C5E: case 0xC11D64: case 0xC11D84:
    case 0xC11E56: case 0xC11E6C: case 0xC11EC2: case 0xC11EE2:
    case 0xC11FC0: case 0xC11FDC:
        step_branch(pc, opcode, COND_NE()); break;
    /* tst.w */
    case 0xC11C36: case 0xC11D82: case 0xC11E46:
        cache_step_logic(cache_step_read(mode, reg, 2), 2); break;
    /* beq */
    case 0xC11C38: case 0xC11C4E: case 0xC11C66: case 0xC11C84:
    case 0xC11C9A: case 0xC11CAC: case 0xC11CBE: case 0xC11CD0:
    case 0xC11CE6: case 0xC11D0C: case 0xC11D2C: case 0xC11D40:
    case 0xC11DAE: case 0xC11E48: case 0xC11E8E: case 0xC11EAA:
    case 0xC11EB8: case 0xC11F00: case 0xC11F30: case 0xC11F40:
    case 0xC11FB2: case 0xC12008: case 0xC1207E:
        step_branch(pc, opcode, COND_EQ()); break;
    /* bra */
    case 0xC11C40: case 0xC11C56: case 0xC11C8C: case 0xC11CA2:
    case 0xC11CB4: case 0xC11CC6: case 0xC11CD8: case 0xC11CFE:
    case 0xC11D24: case 0xC11D34: case 0xC11D58: case 0xC11D7A:
    case 0xC11E78: case 0xC11E96: case 0xC11EEA: case 0xC11F0C:
    case 0xC11FA4: case 0xC11FE8: case 0xC1205A:
        step_branch(pc, opcode, 1); break;
    /* tst.l */
    case 0xC11C5A:
        cache_step_logic(cache_step_read(mode, reg, 4), 4); break;
    /* tst.b */
    case 0xC11C60: case 0xC11EA8: case 0xC11EC0: case 0xC11EFA:
    case 0xC11F2A: case 0xC11F3A: case 0xC11F52: case 0xC12018:
        cache_step_logic(cache_step_read(mode, reg, 1), 1); break;
    /* ori.w */
    case 0xC11C70: case 0xC11E58: case 0xC11F16: case 0xC11FC8:
        value = m68ki_read_imm_16(); value = cache_step_read(mode, reg, 2) | value; cache_step_write(mode, reg, 2, value); cache_step_logic(value, 2); break;
    /* move.b */
    case 0xC11C7A: case 0xC11C90: case 0xC11CDC: case 0xC11D02:
    case 0xC11D36: case 0xC11E10: case 0xC11E12: case 0xC11E26:
    case 0xC11E2C: case 0xC11E36: case 0xC11E6E: case 0xC11E7A:
    case 0xC11E90: case 0xC11E98: case 0xC11E9E: case 0xC11EBA:
    case 0xC11F44: case 0xC11F4C: case 0xC11FDE: case 0xC11FEA:
    case 0xC11FFE: case 0xC1200A: case 0xC12012: case 0xC12066:
    case 0xC12070: case 0xC12076: case 0xC12080: case 0xC12086:
        width = 1; goto move;
    /* clr.w */
    case 0xC11D66: case 0xC11D96: case 0xC11EE4:
        cache_step_write(mode, reg, 2, 0); cache_step_logic(0, 2); break;
    /* cmp.w */
    case 0xC11DAC:
        step_compare_word((uint16_t)cache_step_read(mode, reg, 2), (uint16_t)D(destination)); break;
    /* asl.w */
    case 0xC11DF0: case 0xC11DF6: case 0xC11DFE:
        renderer_asl_word(&D(reg), destination ? destination : 8); break;
    /* add.w */
    case 0xC11DF8: case 0xC11E00:
        step_add_word(&D(destination), (uint16_t)cache_step_read(mode, reg, 2)); break;
    /* ext.l */
    case 0xC11E06:
        D(reg) = (uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    /* movea.l */
    case 0xC11E08: case 0xC11E1E:
        width = 4; goto move;
    /* adda.l */
    case 0xC11E0A: case 0xC11E20:
        A(destination) += cache_step_read(mode, reg, 4); break;
    /* addi.l */
    case 0xC11E18:
        value = m68ki_read_imm_32(); step_add_long(&D(reg), value); break;
    /* andi.b */
    case 0xC11E32: case 0xC1206C:
        value = m68ki_read_imm_16(); value = cache_step_read(mode, reg, 1) & value; cache_step_write(mode, reg, 1, value); cache_step_logic(value, 1); break;
    /* and.b */
    case 0xC11EA4:
        SET_B(D(destination), D(destination) & cache_step_read(mode, reg, 1)); flags_logic_b(D(destination)); break;
    /* cmpi.w */
    case 0xC11EDE:
        value = m68ki_read_imm_16(); step_compare_word((uint16_t)value, (uint16_t)D(reg)); break;
    /* addq.w */
    case 0xC11EF2:
        step_add_word(&D(reg), destination ? destination : 8); break;
    /* bset */
    case 0xC11F04: case 0xC11F32:
        value = 1u << (m68ki_read_imm_16() & 7u); address = cache_step_address(mode, reg, 1); old = cache_step_read_memory(address, 1); FLAG_Z = old & value; cache_step_write_memory(address, old | value, 1, 0); break;
    /* subq.b */
    case 0xC11F4A: case 0xC12010:
        step_subtract_byte(&D(reg), destination ? destination : 8); break;
    /* bpl */
    case 0xC11F54: case 0xC1201A:
        step_branch(pc, opcode, COND_PL()); break;
    /* moveq */
    case 0xC1201C:
        D(destination) = (uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    /* cmp.b */
    case 0xC1207C:
        step_compare_byte((uint8_t)cache_step_read(mode, reg, 1), (uint8_t)D(destination)); break;
    /* unlk */
    case 0xC12094:
        A(7) = A(reg); A(reg) = m68ki_pull_32(); break;
    /* rts */
    case 0xC12096:
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
