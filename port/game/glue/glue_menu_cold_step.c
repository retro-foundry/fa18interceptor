/* Complete menu cold family source CPU/bus/event boundaries.
 * Readable behavior lives in menu_cold.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family menu_cold. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_menu_cold.h"

static int menu_cold_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC09120: case 0xC09126: case 0xC0912C: case 0xC09148:
    case 0xC0914E: case 0xC09154: case 0xC0915A: case 0xC09160:
    case 0xC09166: case 0xC0917E: case 0xC09184: case 0xC0918A:
    case 0xC0FE78: case 0xC0FEC0: case 0xC10194: case 0xC101A2:
    case 0xC101F2: case 0xC10296: case 0xC10410: case 0xC1640A:
        width=4; goto move;
    case 0xC09132: case 0xC0FE82: case 0xC0FEB2: case 0xC101DA:
    case 0xC1642C: case 0xC29498:
        step_branch(pc,opcode,1); break;
    case 0xC0916C: case 0xC09172:
        width=4; value=m68ki_read_imm_32(); operation='&'; goto immediate_logic;
    case 0xC09178: case 0xC0917A: case 0xC0917C:
        renderer_negate(&D(reg),4); break;
    case 0xC09190: case 0xC0FECC: case 0xC101FA: case 0xC1029C:
    case 0xC10416: case 0xC10BAC: case 0xC16438: case 0xC294AA:
        REG_PC=m68ki_pull_32(); break;
    case 0xC0FE36: case 0xC0FE48: case 0xC0FEB4: case 0xC101C0:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC0FE3C: case 0xC0FE4E: case 0xC0FEBA: case 0xC101C2:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC0FE3E: case 0xC0FE60: case 0xC0FE84: case 0xC1019C:
    case 0xC101A6: case 0xC1027E: case 0xC10284: case 0xC1028C:
    case 0xC103E4: case 0xC103F8: case 0xC103FE: case 0xC10406:
    case 0xC1642E:
        width=1; goto move;
    case 0xC0FE44: case 0xC0FE8A:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0FE46: case 0xC0FE58: case 0xC0FE8C:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0FE50: case 0xC0FE6E: case 0xC0FEA4: case 0xC1018E:
    case 0xC101CC: case 0xC101E0: case 0xC10272: case 0xC103EC:
    case 0xC10B96: case 0xC10BA0:
        width=2; goto move;
    case 0xC0FE56: case 0xC10278: case 0xC103F2:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC0FE5A: case 0xC0FE8E: case 0xC0FE98: case 0xC0FE9E:
    case 0xC10182: case 0xC10B90: case 0xC10BA6:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0FE68: case 0xC101B4: case 0xC101C4: case 0xC101C8:
    case 0xC101DC: case 0xC101E8: case 0xC1641E:
        width=4; goto move;
    case 0xC0FE72: case 0xC16416:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC0FE76:
        step_branch(pc,opcode,COND_HI()); break;
    case 0xC0FE94:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC0FEAC: case 0xC0FEC6:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC0FEBC: case 0xC10188: case 0xC101EE: case 0xC10292:
    case 0xC1040C:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC1017E: case 0xC16406:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC10192: case 0xC101CE: case 0xC101D2: case 0xC101E4:
    case 0xC16424:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC101AA:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC101AE: case 0xC1641C:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC101B0:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC101B2:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC101BA:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC101BE:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC101D6:
        width=1; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC101EC: case 0xC16412: case 0xC16422:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC101F8: case 0xC16436:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC1027A: case 0xC103F4:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC1027C: case 0xC103F6:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC10B9C:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC16428:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC29490: case 0xC2949A: case 0xC294A2:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
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
static const uint32_t owned_C0FE36[]={
    0xC0FE36,0xC0FE3C,0xC0FE3E,0xC0FE44,0xC0FE46,0xC0FE48,0xC0FE4E,0xC0FE50,
    0xC0FE56,0xC0FE58,0xC0FE5A,0xC0FE60,0xC0FE68,0xC0FE6E,0xC0FE72,0xC0FE76,
    0xC0FE78,0xC0FE82,0xC0FE84,0xC0FE8A,0xC0FE8C,0xC0FE8E,0xC0FE94,0xC0FE98,
    0xC0FE9E,0xC0FEA4,0xC0FEAC,0xC0FEB2,0xC0FEB4,0xC0FEBA,0xC0FEBC,0xC0FEC0,
    0xC0FEC6,0xC0FECC,
};
int glue_C0FE36_owns(uint32_t pc) { return owns_pc(owned_C0FE36,sizeof owned_C0FE36/sizeof owned_C0FE36[0],pc); }
int glue_C0FE36_step(void) { if(!glue_C0FE36_owns(REG_PC)) return 0; return menu_cold_step(); }
static const uint32_t owned_C1017E[]={
    0xC1017E,0xC10182,0xC10188,0xC1018E,0xC10192,0xC10194,0xC1019C,0xC101A2,
    0xC101A6,0xC101AA,0xC101AE,0xC101B0,0xC101B2,0xC101B4,0xC101BA,0xC101BE,
    0xC101C0,0xC101C2,0xC101C4,0xC101C8,0xC101CC,0xC101CE,0xC101D2,0xC101D6,
    0xC101DA,0xC101DC,0xC101E0,0xC101E4,0xC101E8,0xC101EC,0xC101EE,0xC101F2,
    0xC101F8,0xC101FA,
};
int glue_C1017E_owns(uint32_t pc) { return owns_pc(owned_C1017E,sizeof owned_C1017E/sizeof owned_C1017E[0],pc); }
int glue_C1017E_step(void) { if(!glue_C1017E_owns(REG_PC)) return 0; return menu_cold_step(); }
static const uint32_t owned_C10272[]={
    0xC10272,0xC10278,0xC1027A,0xC1027C,0xC1027E,0xC10284,0xC1028C,0xC10292,
    0xC10296,0xC1029C,
};
int glue_C10272_owns(uint32_t pc) { return owns_pc(owned_C10272,sizeof owned_C10272/sizeof owned_C10272[0],pc); }
int glue_C10272_step(void) { if(!glue_C10272_owns(REG_PC)) return 0; return menu_cold_step(); }
static const uint32_t owned_C103E4[]={
    0xC103E4,0xC103EC,0xC103F2,0xC103F4,0xC103F6,0xC103F8,0xC103FE,0xC10406,
    0xC1040C,0xC10410,0xC10416,
};
int glue_C103E4_owns(uint32_t pc) { return owns_pc(owned_C103E4,sizeof owned_C103E4/sizeof owned_C103E4[0],pc); }
int glue_C103E4_step(void) { if(!glue_C103E4_owns(REG_PC)) return 0; return menu_cold_step(); }
static const uint32_t owned_C09120[]={
    0xC09120,0xC09126,0xC0912C,0xC09132,0xC0915A,0xC09160,0xC09166,0xC0916C,
    0xC09172,0xC09178,0xC0917A,0xC0917C,0xC0917E,0xC09184,0xC0918A,0xC09190,
};
int glue_C09120_owns(uint32_t pc) { return owns_pc(owned_C09120,sizeof owned_C09120/sizeof owned_C09120[0],pc); }
int glue_C09120_step(void) { if(!glue_C09120_owns(REG_PC)) return 0; return menu_cold_step(); }
static const uint32_t owned_C29490[]={
    0xC29490,0xC29498,0xC294A2,0xC294AA,
};
int glue_C29490_owns(uint32_t pc) { return owns_pc(owned_C29490,sizeof owned_C29490/sizeof owned_C29490[0],pc); }
int glue_C29490_step(void) { if(!glue_C29490_owns(REG_PC)) return 0; return menu_cold_step(); }
static const uint32_t owned_C2949A[]={
    0xC2949A,0xC294A2,0xC294AA,
};
int glue_C2949A_owns(uint32_t pc) { return owns_pc(owned_C2949A,sizeof owned_C2949A/sizeof owned_C2949A[0],pc); }
int glue_C2949A_step(void) { if(!glue_C2949A_owns(REG_PC)) return 0; return menu_cold_step(); }
static const uint32_t owned_C09148[]={
    0xC09148,0xC0914E,0xC09154,0xC0915A,0xC09160,0xC09166,0xC0916C,0xC09172,
    0xC09178,0xC0917A,0xC0917C,0xC0917E,0xC09184,0xC0918A,0xC09190,
};
int glue_C09148_owns(uint32_t pc) { return owns_pc(owned_C09148,sizeof owned_C09148/sizeof owned_C09148[0],pc); }
int glue_C09148_step(void) { if(!glue_C09148_owns(REG_PC)) return 0; return menu_cold_step(); }
static const uint32_t owned_C10B90[]={
    0xC10B90,0xC10B96,0xC10B9C,0xC10BA0,0xC10BA6,0xC10BAC,
};
int glue_C10B90_owns(uint32_t pc) { return owns_pc(owned_C10B90,sizeof owned_C10B90/sizeof owned_C10B90[0],pc); }
int glue_C10B90_step(void) { if(!glue_C10B90_owns(REG_PC)) return 0; return menu_cold_step(); }
static const uint32_t owned_C16406[]={
    0xC16406,0xC1640A,0xC16412,0xC16416,0xC1641C,0xC1641E,0xC16422,0xC16424,
    0xC16428,0xC1642C,0xC1642E,0xC16436,0xC16438,
};
int glue_C16406_owns(uint32_t pc) { return owns_pc(owned_C16406,sizeof owned_C16406/sizeof owned_C16406[0],pc); }
int glue_C16406_step(void) { if(!glue_C16406_owns(REG_PC)) return 0; return menu_cold_step(); }
