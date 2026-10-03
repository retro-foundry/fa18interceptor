/* Complete context publication family source CPU/bus/event boundaries.
 * Readable behavior lives in context_publication.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family context_publication. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_context_publication.h"

static int context_publication_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC083A6: case 0xC083DC:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC083AA: case 0xC1B7CC: case 0xC1B7D2: case 0xC1B7E4:
    case 0xC1B908: case 0xC1B9CC: case 0xC1B9D2: case 0xC1B9E8:
    case 0xC1B9F0: case 0xC1B9F8: case 0xC1BA0C: case 0xC1BA1C:
    case 0xC1BA6A: case 0xC1BA72: case 0xC1BA8C: case 0xC1BEE8:
    case 0xC1BEF8: case 0xC1BF1E: case 0xC1BF28: case 0xC1BF42:
    case 0xC1C218: case 0xC1C21E: case 0xC1C24A: case 0xC1C25C:
    case 0xC1C272: case 0xC1C280: case 0xC1C286: case 0xC1C298:
    case 0xC1C2A0:
        width=1; goto move;
    case 0xC083AE: case 0xC083D4: case 0xC1B7A8: case 0xC1B7EC:
    case 0xC1B910: case 0xC1BA60: case 0xC1BAAC: case 0xC1BAC4:
    case 0xC1BAD0: case 0xC1BF30: case 0xC1BF74: case 0xC1C21C:
    case 0xC1C222:
        step_branch(pc,opcode,1); break;
    case 0xC083B6: case 0xC1BA00: case 0xC1BA64: case 0xC1BF34:
    case 0xC1C26C: case 0xC1C27A: case 0xC1C292:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC083BC: case 0xC1BA06:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC083C2: case 0xC1C244:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC083C8: case 0xC083CC: case 0xC09DE4: case 0xC1B7E0:
    case 0xC1BA4E: case 0xC1BF4E: case 0xC1C242: case 0xC1C248:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC083CA: case 0xC1B7DA: case 0xC1BF16: case 0xC1C214:
    case 0xC1C23C:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC083CE:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='&'; goto bit_value;
    case 0xC083D6:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='|'; goto bit_value;
    case 0xC083E0: case 0xC09E04: case 0xC1C2B6:
        REG_PC=m68ki_pull_32(); break;
    case 0xC09DD0: case 0xC09E02:
        width=4; goto move;
    case 0xC09DD2: case 0xC09DE6: case 0xC09DF0: case 0xC1B9E0:
    case 0xC1BA34: case 0xC1BA44: case 0xC1BA50: case 0xC1BA58:
    case 0xC1BA78: case 0xC1BA80: case 0xC1BAA4: case 0xC1BABC:
    case 0xC1BAC8: case 0xC1BEF0: case 0xC1BF00: case 0xC1BF0A:
    case 0xC1BF3A: case 0xC1BF52: case 0xC1BF5A: case 0xC1BF6C:
        width=2; goto move;
    case 0xC09DD8:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC09DDC: case 0xC1BA18: case 0xC1BF1C: case 0xC1C216:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC09DDE: case 0xC1BA48:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC09DEA: case 0xC1B9DA: case 0xC1BA86: case 0xC1BF10:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC09DF8:
        width=4; value=m68ki_read_imm_32(); operation='&'; goto immediate_logic;
    case 0xC1B7A6: case 0xC1C268:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC1B906: case 0xC1BF62: case 0xC1C2A4: case 0xC1C2AA:
    case 0xC1C2B0:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC1BA10: case 0xC1BF46:
        width=1; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC1BA14: case 0xC1BA22: case 0xC1BA28: case 0xC1BA2E:
    case 0xC1BA38: case 0xC1BA3E: case 0xC1BA92: case 0xC1BA98:
    case 0xC1BA9E: case 0xC1BAB0: case 0xC1BAB6: case 0xC1BF4A:
    case 0xC1C252: case 0xC1C262:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC1BA26: case 0xC1BA32: case 0xC1BA3C: case 0xC1BA96:
    case 0xC1C266:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC1BA2C: case 0xC1BA42: case 0xC1BA9C: case 0xC1BAB4:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC1BA70: case 0xC1BA76: case 0xC1C26A: case 0xC1C29E:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC1BA7E: case 0xC1BF06:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC1BAA2: case 0xC1BABA: case 0xC1C25A:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC1BF08:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC1BF68:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC1C276:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC1C284: case 0xC1C28C:
        width=1; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
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
static const uint32_t owned_C1B7A6[]={
    0xC1B7A6,0xC1B7A8,0xC1B7CC,0xC1B7D2,0xC1B7DA,0xC1B7E0,0xC1B7E4,0xC1B7EC,
    0xC1C23C,0xC1C242,0xC1C244,0xC1C248,0xC1C24A,0xC1C252,0xC1C25A,0xC1C25C,
    0xC1C262,0xC1C266,0xC1C268,0xC1C26A,0xC1C26C,0xC1C272,0xC1C276,0xC1C27A,
    0xC1C280,0xC1C284,0xC1C286,0xC1C28C,0xC1C292,0xC1C298,0xC1C29E,0xC1C2A0,
    0xC1C2A4,0xC1C2AA,0xC1C2B0,0xC1C2B6,
};
int glue_C1B7A6_owns(uint32_t pc) { return owns_pc(owned_C1B7A6,sizeof owned_C1B7A6/sizeof owned_C1B7A6[0],pc); }
int glue_C1B7A6_step(void) { if(!glue_C1B7A6_owns(REG_PC)) return 0; return context_publication_step(); }
static const uint32_t owned_C1BEE8[]={
    0xC1B906,0xC1B908,0xC1B910,0xC1B9CC,0xC1B9D2,0xC1B9DA,0xC1B9E0,0xC1B9E8,
    0xC1B9F0,0xC1B9F8,0xC1BA00,0xC1BA06,0xC1BA0C,0xC1BA10,0xC1BA14,0xC1BA18,
    0xC1BA1C,0xC1BA22,0xC1BA26,0xC1BA28,0xC1BA2C,0xC1BA2E,0xC1BA32,0xC1BA34,
    0xC1BA38,0xC1BA3C,0xC1BA3E,0xC1BA42,0xC1BA44,0xC1BA48,0xC1BA4E,0xC1BA50,
    0xC1BA58,0xC1BA60,0xC1BA64,0xC1BA6A,0xC1BA70,0xC1BA72,0xC1BA76,0xC1BA78,
    0xC1BA7E,0xC1BA80,0xC1BA86,0xC1BA8C,0xC1BA92,0xC1BA96,0xC1BA98,0xC1BA9C,
    0xC1BA9E,0xC1BAA2,0xC1BAA4,0xC1BAAC,0xC1BAB0,0xC1BAB4,0xC1BAB6,0xC1BABA,
    0xC1BABC,0xC1BAC4,0xC1BAC8,0xC1BAD0,0xC1BEE8,0xC1BEF0,0xC1BEF8,0xC1BF00,
    0xC1BF06,0xC1BF08,0xC1BF0A,0xC1BF10,0xC1BF16,0xC1BF1C,0xC1BF1E,0xC1BF28,
    0xC1BF30,0xC1BF34,0xC1BF3A,0xC1BF42,0xC1BF46,0xC1BF4A,0xC1BF4E,0xC1BF52,
    0xC1BF5A,0xC1BF62,0xC1BF68,0xC1BF6C,0xC1BF74,0xC1C23C,0xC1C242,0xC1C244,
    0xC1C248,0xC1C24A,0xC1C252,0xC1C25A,0xC1C25C,0xC1C262,0xC1C266,0xC1C268,
    0xC1C26A,0xC1C26C,0xC1C272,0xC1C276,0xC1C27A,0xC1C280,0xC1C284,0xC1C286,
    0xC1C28C,0xC1C292,0xC1C298,0xC1C29E,0xC1C2A0,0xC1C2A4,0xC1C2AA,0xC1C2B0,
    0xC1C2B6,
};
int glue_C1BEE8_owns(uint32_t pc) { return owns_pc(owned_C1BEE8,sizeof owned_C1BEE8/sizeof owned_C1BEE8[0],pc); }
int glue_C1BEE8_step(void) { if(!glue_C1BEE8_owns(REG_PC)) return 0; return context_publication_step(); }
static const uint32_t owned_C1C214[]={
    0xC1C214,0xC1C216,0xC1C218,0xC1C21C,0xC1C21E,0xC1C222,0xC1C23C,0xC1C242,
    0xC1C244,0xC1C248,0xC1C24A,0xC1C252,0xC1C25A,0xC1C25C,0xC1C262,0xC1C266,
    0xC1C268,0xC1C26A,0xC1C26C,0xC1C272,0xC1C276,0xC1C27A,0xC1C280,0xC1C284,
    0xC1C286,0xC1C28C,0xC1C292,0xC1C298,0xC1C29E,0xC1C2A0,0xC1C2A4,0xC1C2AA,
    0xC1C2B0,0xC1C2B6,
};
int glue_C1C214_owns(uint32_t pc) { return owns_pc(owned_C1C214,sizeof owned_C1C214/sizeof owned_C1C214[0],pc); }
int glue_C1C214_step(void) { if(!glue_C1C214_owns(REG_PC)) return 0; return context_publication_step(); }
static const uint32_t owned_C083A6[]={
    0xC083A6,0xC083AA,0xC083AE,0xC083B6,0xC083BC,0xC083C2,0xC083C8,0xC083CA,
    0xC083CC,0xC083CE,0xC083D4,0xC083D6,0xC083DC,0xC083E0,
};
int glue_C083A6_owns(uint32_t pc) { return owns_pc(owned_C083A6,sizeof owned_C083A6/sizeof owned_C083A6[0],pc); }
int glue_C083A6_step(void) { if(!glue_C083A6_owns(REG_PC)) return 0; return context_publication_step(); }
static const uint32_t owned_C09DD0[]={
    0xC09DD0,0xC09DD2,0xC09DD8,0xC09DDC,0xC09DDE,0xC09DE4,0xC09DE6,0xC09DEA,
    0xC09DF0,0xC09DF8,0xC09E02,0xC09E04,
};
int glue_C09DD0_owns(uint32_t pc) { return owns_pc(owned_C09DD0,sizeof owned_C09DD0/sizeof owned_C09DD0[0],pc); }
int glue_C09DD0_step(void) { if(!glue_C09DD0_owns(REG_PC)) return 0; return context_publication_step(); }
