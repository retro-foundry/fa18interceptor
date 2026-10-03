/* Complete menu context finish family source CPU/bus/event boundaries.
 * Readable behavior lives in menu_context_finish.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family menu_context_finish. */
#include "glue_menu_context_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_menu_context_finish.h"

static int menu_context_finish_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC09192: case 0xC09194: case 0xC10A46: case 0xC10A4E:
    case 0xC10A56: case 0xC10AB2: case 0xC10AC4: case 0xC10B04:
    case 0xC10B38: case 0xC10B6A: case 0xC10B78: case 0xC10C1A:
    case 0xC10D0A: case 0xC10DF8: case 0xC10E56: case 0xC10E5A:
    case 0xC10F34: case 0xC10F5C: case 0xC2506C: case 0xC250AA:
    case 0xC250AC: case 0xC250C4: case 0xC250CE: case 0xC250D0:
    case 0xC250D2: case 0xC25172:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC09196: case 0xC10A5E: case 0xC10ABA: case 0xC10ACC:
    case 0xC10AF0: case 0xC10B1E: case 0xC10C2E: case 0xC10C4C:
    case 0xC10C6C: case 0xC10CAA: case 0xC10CCA: case 0xC10CD8:
    case 0xC10D12: case 0xC10D1C: case 0xC10D22: case 0xC10D2C:
    case 0xC10D32: case 0xC10D3C: case 0xC10D76: case 0xC10D8A:
    case 0xC10DBA: case 0xC10DE8: case 0xC10DF4: case 0xC10E00:
    case 0xC10E0A: case 0xC10E10: case 0xC10E1A: case 0xC10E20:
    case 0xC10E2A: case 0xC10E74: case 0xC10E7E: case 0xC10EA0:
    case 0xC10F5E: case 0xC10F64: case 0xC10F7A: case 0xC10F82:
    case 0xC10F92: case 0xC10FA0: case 0xC10FA8: case 0xC11016:
    case 0xC11A3C: case 0xC16D20: case 0xC25080: case 0xC250AE:
    case 0xC250E2:
        width=2; goto move;
    case 0xC0919A: case 0xC10A2E: case 0xC10A42: case 0xC10B00:
    case 0xC10B30: case 0xC10F30: case 0xC250F6:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC0919E: case 0xC250D4:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC091A6: case 0xC10AB0: case 0xC10AE4: case 0xC10B1C:
    case 0xC10B8E: case 0xC10C66: case 0xC10CFC: case 0xC10D88:
    case 0xC10DAC: case 0xC1104A: case 0xC11A4E: case 0xC11ACA:
    case 0xC16D4A: case 0xC2506E: case 0xC25174:
        REG_PC=m68ki_pull_32(); break;
    case 0xC10A24: case 0xC10A48: case 0xC10A50: case 0xC10A58:
    case 0xC10A66: case 0xC10A7A: case 0xC10A88: case 0xC10A9E:
    case 0xC10AA8: case 0xC10AB4: case 0xC10AC6: case 0xC10AD4:
    case 0xC10AE6: case 0xC10AF8: case 0xC10B06: case 0xC10B0C:
    case 0xC10B28: case 0xC10B3A: case 0xC10B40: case 0xC10B48:
    case 0xC10B4E: case 0xC10C08: case 0xC10C12: case 0xC10C1C:
    case 0xC10C22: case 0xC10C28: case 0xC10C42: case 0xC10C54:
    case 0xC10C78: case 0xC10C8C: case 0xC10CBA: case 0xC10CE2:
    case 0xC10CFE: case 0xC10D0C: case 0xC10D42: case 0xC10D4C:
    case 0xC10D94: case 0xC10DB2: case 0xC10DC8: case 0xC10DFA:
    case 0xC10E30: case 0xC10E3A: case 0xC10E48: case 0xC10E96:
    case 0xC10EA8: case 0xC10ECE: case 0xC10EFC: case 0xC10F0E:
    case 0xC10F20: case 0xC10F28: case 0xC10F36: case 0xC10F3C:
    case 0xC10F44: case 0xC10F4C: case 0xC10F6A: case 0xC10F70:
    case 0xC10FB4: case 0xC10FBE: case 0xC10FC8: case 0xC10FDA:
    case 0xC11020: case 0xC1102A: case 0xC11036: case 0xC11040:
    case 0xC11A2C: case 0xC11A32: case 0xC11A50: case 0xC11A5A:
    case 0xC11A68: case 0xC11A70: case 0xC16D04: case 0xC2508C:
    case 0xC2510A: case 0xC2514C: case 0xC25156: case 0xC25158:
    case 0xC25164: case 0xC25166: case 0xC25170:
        width=1; goto move;
    case 0xC10A2A: case 0xC10A6C: case 0xC10AA4: case 0xC10D48:
    case 0xC11A76:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC10A2C: case 0xC10AA6: case 0xC10AEE: case 0xC10C10:
    case 0xC10D4A: case 0xC10DD4: case 0xC10E38: case 0xC10EFA:
    case 0xC10F0C: case 0xC10F1E: case 0xC10FD2: case 0xC10FF4:
    case 0xC11008: case 0xC1101E: case 0xC11028: case 0xC11A3A:
    case 0xC11A78: case 0xC2509E:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC10A32: case 0xC10A90: case 0xC10A9C: case 0xC10B76:
    case 0xC10C40: case 0xC10CD2: case 0xC10CE0: case 0xC10E70:
    case 0xC10E92: case 0xC10ECA: case 0xC10F04: case 0xC10F16:
    case 0xC10F8A: case 0xC10FC4: case 0xC10FEC: case 0xC11000:
    case 0xC11014: case 0xC1103E: case 0xC11A84:
        step_branch(pc,opcode,1); break;
    case 0xC10A34: case 0xC10AEC: case 0xC10C0E: case 0xC10C48:
    case 0xC10D04: case 0xC10DD2: case 0xC10DD8: case 0xC10E36:
    case 0xC10E9C: case 0xC10EE2: case 0xC10EEC: case 0xC10F76:
    case 0xC11026: case 0xC11A56:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC10A3A: case 0xC10A6E: case 0xC10B68: case 0xC10CB8:
    case 0xC10CC4: case 0xC10DC4: case 0xC10DDE: case 0xC10EE8:
    case 0xC10EF2: case 0xC10F58: case 0xC10F78: case 0xC11AAC:
    case 0xC25094: case 0xC250A6:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC10A3C: case 0xC10A82: case 0xC10C7E: case 0xC10F9A:
    case 0xC10FD4: case 0xC11A26: case 0xC16D0C:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC10A70: case 0xC10A92: case 0xC10ADA: case 0xC10B12:
    case 0xC10B84: case 0xC10C36: case 0xC10CF0: case 0xC10D7E:
    case 0xC10DA2: case 0xC11A44: case 0xC25070: case 0xC250FA:
    case 0xC2510C: case 0xC25124:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC10A74: case 0xC10A96: case 0xC10ADE: case 0xC10B16:
    case 0xC10B6C: case 0xC10B7A: case 0xC10B88: case 0xC10C3A:
    case 0xC10C5C: case 0xC10C84: case 0xC10CF4: case 0xC10D5A:
    case 0xC10D60: case 0xC10D66: case 0xC10D82: case 0xC10DA6:
    case 0xC10E58: case 0xC10E5C: case 0xC10E66: case 0xC10E84:
    case 0xC10EB6: case 0xC10EC0: case 0xC10FE2: case 0xC10FF6:
    case 0xC1100A: case 0xC11A48: case 0xC11A7A: case 0xC11A8C:
    case 0xC11A92: case 0xC11A9A: case 0xC11AAE: case 0xC11AC0:
    case 0xC16D14: case 0xC16D1A: case 0xC16D36: case 0xC16D40:
    case 0xC250B8: case 0xC250BA: case 0xC250F0: case 0xC25104:
    case 0xC2512A:
        width=4; goto move;
    case 0xC10AC0: case 0xC10B24: case 0xC10C72: case 0xC10D90:
    case 0xC10F52: case 0xC1101C: case 0xC2507C:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC10AC2: case 0xC10B26: case 0xC10C4A: case 0xC10C74:
    case 0xC10D06: case 0xC10D92: case 0xC10E9E:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC10B32: case 0xC10B6E: case 0xC10B7C: case 0xC10C9C:
    case 0xC10D54: case 0xC10D9C: case 0xC10E50: case 0xC10E5E:
    case 0xC10E8A: case 0xC10EB0: case 0xC10FAE: case 0xC11A62:
    case 0xC11A86: case 0xC16D2E: case 0xC250B2: case 0xC250D8:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC10B54: case 0xC10B5A: case 0xC10C90: case 0xC10C96:
    case 0xC10CB2: case 0xC10ED2: case 0xC10EDA: case 0xC10EF6:
    case 0xC10F06: case 0xC10F18: case 0xC10FCE: case 0xC10FEE:
    case 0xC11002: case 0xC11030: case 0xC25090: case 0xC2509A:
    case 0xC2511C:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC10B58: case 0xC10C94: case 0xC10EDE: case 0xC2507E:
    case 0xC25120: case 0xC25144:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC10B5E: case 0xC10C9A: case 0xC10ED6:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC10B60: case 0xC10CC0: case 0xC10DC0: case 0xC250A0:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC10B74: case 0xC10B82: case 0xC10CAE: case 0xC10CCE:
    case 0xC10CDC: case 0xC10E64: case 0xC10E90: case 0xC16D34:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC10C68: case 0xC10DAE:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC10CA2: case 0xC11AA6:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC10CA4: case 0xC11A58:
        step_branch(pc,opcode,COND_MI()); break;
    case 0xC10CA6: case 0xC10CC6: case 0xC10CD4: case 0xC10CEA:
    case 0xC10DE2: case 0xC10DEE: case 0xC25076:
        width=4; goto move;
    case 0xC10CEE: case 0xC10D70: case 0xC10F8C: case 0xC25146:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC10CFA: case 0xC11048:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC10D18: case 0xC10D28: case 0xC10D38:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC10DCE: case 0xC10FBA: case 0xC25096: case 0xC25112:
    case 0xC25118: case 0xC2514E: case 0xC2515C: case 0xC25168:
        width=1; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC10DEC: case 0xC25122:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC10E06: case 0xC10E16: case 0xC10E26: case 0xC10E7A:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC10E42: case 0xC11ABA:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC10FA6:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC11034:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC11A38:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;
    case 0xC11A98: case 0xC250BC: case 0xC250C6:
        width=4; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC11AA0: case 0xC11AB4:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC16D12:
        A(destination)-=cache_step_read(mode,reg,4); break;
    case 0xC16D28:
        address=cache_step_address(mode,reg,4); m68ki_push_32(address); break;
    case 0xC25084: case 0xC250A8: case 0xC250DE: case 0xC25100:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC25088:
        menu_lsl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC2508A:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC250C2: case 0xC250CC:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC250E8: case 0xC250EE:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC250EA:
        step_divide_unsigned(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC25132: case 0xC25152: case 0xC25160: case 0xC2516C:
        width=1; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC25136: case 0xC25138:
        value=cache_step_read_memory(A(reg)-=reg==7?2:1,1); address=(A(destination)-=destination==7?2:1); old=cache_step_read_memory(address,1); cache_step_write_memory(address,menu_decimal_flags((uint8_t)value,(uint8_t)old),1,0); break;
    case 0xC2513A:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC2515A:
        menu_lsr_byte(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    default: return 0;
    }
    goto finish;
move:
    value=cache_step_read(mode,reg,width); mode=(opcode>>6)&7u;
    cache_step_write(mode,destination,width,value); if(mode!=1) cache_step_logic(value,width);
    goto finish;
immediate_logic:
    if(mode==0) old=D(reg);
    else { address=cache_step_address(mode,reg,width); old=cache_step_read_memory(address,width); }
    value=operation=='&'?old&value:operation=='|'?old|value:old^value;
    if(mode==0) cache_step_write(0,reg,width,value); else cache_step_write_memory(address,value,width,0);
    cache_step_logic(value,width); goto finish;
arithmetic:
    if(mode==0) old=D(reg);
    else { address=cache_step_address(mode,reg,width); old=cache_step_read_memory(address,width); }
    if(operation=='+') {
        if(width==1) renderer_add_byte(&old,value); else if(width==2) step_add_word(&old,value); else step_add_long(&old,value);
    } else {
        if(width==1) step_subtract_byte(&old,value); else if(width==2) step_subtract_word(&old,value); else step_subtract_long(&old,value);
    }
    if(mode==0) cache_step_write(0,reg,width,old); else cache_step_write_memory(address,old,width,0);
    goto finish;
bit_value:
    mask=(uint16_t)(value&(mode==0?31u:7u)); value=1u<<mask;
    if(mode==0) old=D(reg);
    else { address=cache_step_address(mode,reg,1); old=cache_step_read_memory(address,1); }
    FLAG_Z=old&value;
    if(operation!='?') {
        value=operation=='|'?old|value:operation=='&'?old&~value:old^value;
        if(mode==0) { D(reg)=value; if(mask<16) USE_CYCLES(-2); }
        else cache_step_write_memory(address,value,1,0);
    }
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
static int owns_pc(const uint32_t *pcs,unsigned count,uint32_t pc) {
    unsigned lo=0,hi=count;
    while(lo<hi) { unsigned mid=lo+(hi-lo)/2; if(pcs[mid]<pc) lo=mid+1; else hi=mid; }
    return lo<count && pcs[lo]==pc;
}
static const uint32_t owned_C10A24[]={
    0xC10A24,0xC10A2A,0xC10A2C,0xC10A2E,0xC10A32,0xC10A34,0xC10A3A,0xC10A3C,
    0xC10A42,0xC10A46,0xC10A48,0xC10A4E,0xC10A50,0xC10A56,0xC10A58,0xC10A5E,
    0xC10A66,0xC10A6C,0xC10A6E,0xC10A70,0xC10A74,0xC10A7A,0xC10A82,0xC10A88,
    0xC10A90,0xC10A92,0xC10A96,0xC10A9C,0xC10A9E,0xC10AA4,0xC10AA6,0xC10AA8,
    0xC10AB0,
};
int glue_C10A24_owns(uint32_t pc) { return owns_pc(owned_C10A24,sizeof owned_C10A24/sizeof owned_C10A24[0],pc); }
int glue_C10A24_step(void) { if(!glue_C10A24_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C10C08[]={
    0xC10C08,0xC10C0E,0xC10C10,0xC10C12,0xC10C1A,0xC10C1C,0xC10C22,0xC10C28,
    0xC10C2E,0xC10C36,0xC10C3A,0xC10C40,0xC10C42,0xC10C48,0xC10C4A,0xC10C4C,
    0xC10C54,0xC10C5C,0xC10C66,
};
int glue_C10C08_owns(uint32_t pc) { return owns_pc(owned_C10C08,sizeof owned_C10C08/sizeof owned_C10C08[0],pc); }
int glue_C10C08_step(void) { if(!glue_C10C08_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C10C68[]={
    0xC10C68,0xC10C6C,0xC10C72,0xC10C74,0xC10C78,0xC10C7E,0xC10C84,0xC10C8C,
    0xC10C90,0xC10C94,0xC10C96,0xC10C9A,0xC10C9C,0xC10CA2,0xC10CA4,0xC10CA6,
    0xC10CAA,0xC10CAE,0xC10CB2,0xC10CB8,0xC10CBA,0xC10CC0,0xC10CC4,0xC10CC6,
    0xC10CCA,0xC10CCE,0xC10CD2,0xC10CD4,0xC10CD8,0xC10CDC,0xC10CE0,0xC10CE2,
    0xC10CEA,0xC10CEE,0xC10CF0,0xC10CF4,0xC10CFA,0xC10CFC,
};
int glue_C10C68_owns(uint32_t pc) { return owns_pc(owned_C10C68,sizeof owned_C10C68/sizeof owned_C10C68[0],pc); }
int glue_C10C68_step(void) { if(!glue_C10C68_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C10AB2[]={
    0xC10AB2,0xC10AB4,0xC10ABA,0xC10AC0,0xC10AC2,0xC10AC4,0xC10AC6,0xC10ACC,
    0xC10AD4,0xC10ADA,0xC10ADE,0xC10AE4,
};
int glue_C10AB2_owns(uint32_t pc) { return owns_pc(owned_C10AB2,sizeof owned_C10AB2/sizeof owned_C10AB2[0],pc); }
int glue_C10AB2_step(void) { if(!glue_C10AB2_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C10AE6[]={
    0xC10AE6,0xC10AEC,0xC10AEE,0xC10AF0,0xC10AF8,0xC10B00,0xC10B04,0xC10B06,
    0xC10B0C,0xC10B12,0xC10B16,0xC10B1C,
};
int glue_C10AE6_owns(uint32_t pc) { return owns_pc(owned_C10AE6,sizeof owned_C10AE6/sizeof owned_C10AE6[0],pc); }
int glue_C10AE6_step(void) { if(!glue_C10AE6_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C10B1E[]={
    0xC10B1E,0xC10B24,0xC10B26,0xC10B28,0xC10B30,0xC10B32,0xC10B38,0xC10B3A,
    0xC10B40,0xC10B48,0xC10B4E,0xC10B54,0xC10B58,0xC10B5A,0xC10B5E,0xC10B60,
    0xC10B68,0xC10B6A,0xC10B6C,0xC10B6E,0xC10B74,0xC10B76,0xC10B78,0xC10B7A,
    0xC10B7C,0xC10B82,0xC10B84,0xC10B88,0xC10B8E,
};
int glue_C10B1E_owns(uint32_t pc) { return owns_pc(owned_C10B1E,sizeof owned_C10B1E/sizeof owned_C10B1E[0],pc); }
int glue_C10B1E_step(void) { if(!glue_C10B1E_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C10CFE[]={
    0xC10CFE,0xC10D04,0xC10D06,0xC10D0A,0xC10D0C,0xC10D12,0xC10D18,0xC10D1C,
    0xC10D22,0xC10D28,0xC10D2C,0xC10D32,0xC10D38,0xC10D3C,0xC10D42,0xC10D48,
    0xC10D4A,0xC10D4C,0xC10D54,0xC10D5A,0xC10D60,0xC10D66,0xC10D70,0xC10D76,
    0xC10D7E,0xC10D82,0xC10D88,
};
int glue_C10CFE_owns(uint32_t pc) { return owns_pc(owned_C10CFE,sizeof owned_C10CFE/sizeof owned_C10CFE[0],pc); }
int glue_C10CFE_step(void) { if(!glue_C10CFE_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C10D8A[]={
    0xC10D8A,0xC10D90,0xC10D92,0xC10D94,0xC10D9C,0xC10DA2,0xC10DA6,0xC10DAC,
};
int glue_C10D8A_owns(uint32_t pc) { return owns_pc(owned_C10D8A,sizeof owned_C10D8A/sizeof owned_C10D8A[0],pc); }
int glue_C10D8A_step(void) { if(!glue_C10D8A_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C10DAE[]={
    0xC10DAE,0xC10DB2,0xC10DBA,0xC10DC0,0xC10DC4,0xC10DC8,0xC10DCE,0xC10DD2,
    0xC10DD4,0xC10DD8,0xC10DDE,0xC10DE2,0xC10DE8,0xC10DEC,0xC10DEE,0xC10DF4,
    0xC10DF8,0xC10DFA,0xC10E00,0xC10E06,0xC10E0A,0xC10E10,0xC10E16,0xC10E1A,
    0xC10E20,0xC10E26,0xC10E2A,0xC10E30,0xC10E36,0xC10E38,0xC10E3A,0xC10E42,
    0xC10E48,0xC10E50,0xC10E56,0xC10E58,0xC10E5A,0xC10E5C,0xC10E5E,0xC10E64,
    0xC10E66,0xC10E70,0xC10E74,0xC10E7A,0xC10E7E,0xC10E84,0xC10E8A,0xC10E90,
    0xC10E92,0xC10E96,0xC10E9C,0xC10E9E,0xC10EA0,0xC10EA8,0xC10EB0,0xC10EB6,
    0xC10EC0,0xC10ECA,0xC10ECE,0xC10ED2,0xC10ED6,0xC10EDA,0xC10EDE,0xC10EE2,
    0xC10EE8,0xC10EEC,0xC10EF2,0xC10EF6,0xC10EFA,0xC10EFC,0xC10F04,0xC10F06,
    0xC10F0C,0xC10F0E,0xC10F16,0xC10F18,0xC10F1E,0xC10F20,0xC10F28,0xC10F30,
    0xC10F34,0xC10F36,0xC10F3C,0xC10F44,0xC10F4C,0xC10F52,0xC10F58,0xC10F5C,
    0xC10F5E,0xC10F64,0xC10F6A,0xC10F70,0xC10F76,0xC10F78,0xC10F7A,0xC10F82,
    0xC10F8A,0xC10F8C,0xC10F92,0xC10F9A,0xC10FA0,0xC10FA6,0xC10FA8,0xC10FAE,
    0xC10FB4,0xC10FBA,0xC10FBE,0xC10FC4,0xC10FC8,0xC10FCE,0xC10FD2,0xC10FD4,
    0xC10FDA,0xC10FE2,0xC10FEC,0xC10FEE,0xC10FF4,0xC10FF6,0xC11000,0xC11002,
    0xC11008,0xC1100A,0xC11014,0xC11016,0xC1101C,0xC1101E,0xC11020,0xC11026,
    0xC11028,0xC1102A,0xC11030,0xC11034,0xC11036,0xC1103E,0xC11040,0xC11048,
    0xC1104A,
};
int glue_C10DAE_owns(uint32_t pc) { return owns_pc(owned_C10DAE,sizeof owned_C10DAE/sizeof owned_C10DAE[0],pc); }
int glue_C10DAE_step(void) { if(!glue_C10DAE_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C11A26[]={
    0xC11A26,0xC11A2C,0xC11A32,0xC11A38,0xC11A3A,0xC11A3C,0xC11A44,0xC11A48,
    0xC11A4E,
};
int glue_C11A26_owns(uint32_t pc) { return owns_pc(owned_C11A26,sizeof owned_C11A26/sizeof owned_C11A26[0],pc); }
int glue_C11A26_step(void) { if(!glue_C11A26_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C11A50[]={
    0xC11A50,0xC11A56,0xC11A58,0xC11A5A,0xC11A62,0xC11A68,0xC11A70,0xC11A76,
    0xC11A78,0xC11A7A,0xC11A84,0xC11A86,0xC11A8C,0xC11A92,0xC11A98,0xC11A9A,
    0xC11AA0,0xC11AA6,0xC11AAC,0xC11AAE,0xC11AB4,0xC11ABA,0xC11AC0,0xC11ACA,
};
int glue_C11A50_owns(uint32_t pc) { return owns_pc(owned_C11A50,sizeof owned_C11A50/sizeof owned_C11A50[0],pc); }
int glue_C11A50_step(void) { if(!glue_C11A50_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C09192[]={
    0xC09192,0xC09194,0xC09196,0xC0919A,0xC0919E,0xC091A6,
};
int glue_C09192_owns(uint32_t pc) { return owns_pc(owned_C09192,sizeof owned_C09192/sizeof owned_C09192[0],pc); }
int glue_C09192_step(void) { if(!glue_C09192_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C16D04[]={
    0xC16D04,0xC16D0C,0xC16D12,0xC16D14,0xC16D1A,0xC16D20,0xC16D28,0xC16D2E,
    0xC16D34,0xC16D36,0xC16D40,0xC16D4A,
};
int glue_C16D04_owns(uint32_t pc) { return owns_pc(owned_C16D04,sizeof owned_C16D04/sizeof owned_C16D04[0],pc); }
int glue_C16D04_step(void) { if(!glue_C16D04_owns(REG_PC)) return 0; return menu_context_finish_step(); }
static const uint32_t owned_C25070[]={
    0xC2506C,0xC2506E,0xC25070,0xC25076,0xC2507C,0xC2507E,0xC25080,0xC25084,
    0xC25088,0xC2508A,0xC2508C,0xC25090,0xC25094,0xC25096,0xC2509A,0xC2509E,
    0xC250A0,0xC250A6,0xC250A8,0xC250AA,0xC250AC,0xC250AE,0xC250B2,0xC250B8,
    0xC250BA,0xC250BC,0xC250C2,0xC250C4,0xC250C6,0xC250CC,0xC250CE,0xC250D0,
    0xC250D2,0xC250D4,0xC250D8,0xC250DE,0xC250E2,0xC250E8,0xC250EA,0xC250EE,
    0xC250F0,0xC250F6,0xC250FA,0xC25100,0xC25104,0xC2510A,0xC2510C,0xC25112,
    0xC25118,0xC2511C,0xC25120,0xC25122,0xC25124,0xC2512A,0xC25132,0xC25136,
    0xC25138,0xC2513A,0xC25144,0xC25146,0xC2514C,0xC2514E,0xC25152,0xC25156,
    0xC25158,0xC2515A,0xC2515C,0xC25160,0xC25164,0xC25166,0xC25168,0xC2516C,
    0xC25170,0xC25172,0xC25174,
};
int glue_C25070_owns(uint32_t pc) { return owns_pc(owned_C25070,sizeof owned_C25070/sizeof owned_C25070[0],pc); }
int glue_C25070_step(void) { if(!glue_C25070_owns(REG_PC)) return 0; return menu_context_finish_step(); }
