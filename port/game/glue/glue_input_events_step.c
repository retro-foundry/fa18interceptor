/* Complete external event, keyboard and changed-button source boundaries.
 * Readable owners live in input_events.c. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
static int input_event_step(void) {
    uint32_t pc=REG_PC,value,address;
    uint16_t opcode;
    unsigned mode,reg,destination,width;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC16EAE: case 0xC16BF2: case 0xC16C56:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC16EB2: case 0xC16EC0: case 0xC16F08: case 0xC16BF6:
    case 0xC16C04: case 0xC16C20:
        width=4; goto move;
    case 0xC16EB8: case 0xC16EE8: case 0xC16EF8: case 0xC16F0E:
    case 0xC16BFC: case 0xC16C2A:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC16EBE: case 0xC16F14: case 0xC16C02: case 0xC16C30:
        if(mode==1) A(reg)+=destination?destination:8; else step_add_long(&D(reg),destination?destination:8); break;
    case 0xC16EC4: case 0xC16C08: case 0xC13D34:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC16EC6: case 0xC16EE6: case 0xC16EF6: case 0xC16C0A:
    case 0xC16C7C: case 0xC16C9A: case 0xC16CBA: case 0xC13D50:
    case 0xC13D5C:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC16EC8:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC16ECE: case 0xC16EEE: case 0xC13D72:
        step_branch(pc,opcode,1); break;
    case 0xC16ED0: case 0xC16EFE: case 0xC16C12:
        width=4; goto move;
    case 0xC16ED6: case 0xC16EDA: case 0xC16C18: case 0xC16C68:
    case 0xC16C7E: case 0xC16C9C: case 0xC16CB4: case 0xC13D44:
    case 0xC13D52:
        width=2; goto move;
    case 0xC16EDE: case 0xC16C5A: case 0xC16CB8:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC16EE0: case 0xC16C60: case 0xC13D3A: case 0xC13D42:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC16EE2: case 0xC16EF0:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC16F04: case 0xC16C1C: case 0xC16C62: case 0xC13D7C:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC16F16: case 0xC16C72:
        if(opcode&0xffu) value=(uint32_t)(int32_t)(int8_t)opcode;
        else value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16();
        m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC16F18: case 0xC16C0E: case 0xC16C36: case 0xC16C6E:
    case 0xC16C82: case 0xC16CA0: case 0xC16CD4:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC16F1A: case 0xC16C10: case 0xC16C38: case 0xC16C70:
    case 0xC16C84: case 0xC16CA2: case 0xC16CD6: case 0xC13D82:
        REG_PC=m68ki_pull_32(); break;
    case 0xC16C0C:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC16C26: case 0xC16C32: case 0xC16C76: case 0xC16C86:
    case 0xC16C8E: case 0xC16CA4: case 0xC16CBC: case 0xC16CCA:
    case 0xC16CCE: case 0xC13D5E: case 0xC13D6A: case 0xC13D74:
        width=1; goto move;
    case 0xC16C7A: case 0xC13D3C:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC16C8A: case 0xC16C92: case 0xC16CA8:
        width=1; value=m68ki_read_imm_16(); value&=D(reg); cache_step_write(mode,reg,width,value); cache_step_logic(value,width); break;
    case 0xC16C96: case 0xC13D64:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC16CAC:
        value=COND_EQ()?0xff:0; SET_B(D(reg),value);
        if(value) USE_CYCLES(CYC_SCC_R_TRUE); break;
    case 0xC16CAE:
        renderer_negate(&D(reg),1); break;
    case 0xC16CB0: case 0xC16CC0: case 0xC16CD2:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC16CB2: case 0xC16CC2:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC16CC4:
        step_add_long(&D(reg),m68ki_read_imm_32()); break;
    case 0xC13D4A:
        width=2; value=m68ki_read_imm_16(); value&=D(reg); cache_step_write(mode,reg,width,value); cache_step_logic(value,width); break;
    case 0xC13D4E:
        step_subtract_word(&D(reg),destination?destination:8); break;
    case 0xC13D58:
        value=m68ki_read_imm_16(); FLAG_Z=D(reg)&(1u<<(value&31u)); break;
    case 0xC13D68:
        step_branch(pc,opcode,COND_LT()); break;
    default: return 0;
    }
    goto finish;
move:
    value=cache_step_read(mode,reg,width); mode=(opcode>>6)&7u;
    cache_step_write(mode,destination,width,value);
    if(mode!=1) cache_step_logic(value,width);
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
int glue_C16EAE_step(void) {
    if(REG_PC<0xc16eaeu || REG_PC>=0xc16f1cu) return 0;
    return input_event_step();
}
int glue_C16BF2_step(void) {
    if(REG_PC<0xc16bf2u || REG_PC>=0xc16c3au) return 0;
    return input_event_step();
}
int glue_C16C56_step(void) {
    if(REG_PC<0xc16c56u || REG_PC>=0xc16cd8u) return 0;
    return input_event_step();
}
int glue_C13D34_step(void) {
    if(REG_PC<0xc13d34u || REG_PC>=0xc13d84u) return 0;
    return input_event_step();
}
