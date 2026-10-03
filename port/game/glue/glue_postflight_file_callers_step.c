/* Complete postflight file callers family source CPU/bus/event boundaries.
 * Readable behavior lives in postflight_file_callers.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family postflight_file_callers. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_postflight_file_callers.h"

static int postflight_file_callers_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0EF08: case 0xC1631C: case 0xC16386:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0EF0C: case 0xC0EF14: case 0xC0EF1E: case 0xC0EF2C:
    case 0xC0EF34: case 0xC0EF3C: case 0xC0EF48: case 0xC0EF4E:
    case 0xC0EF50: case 0xC0EF5C: case 0xC0EF6E: case 0xC0EF72:
    case 0xC0EF7C: case 0xC0EF8E: case 0xC0EFA6: case 0xC0EFBA:
    case 0xC0EFBC: case 0xC0EFCA: case 0xC16320: case 0xC16334:
    case 0xC1634A: case 0xC1634C: case 0xC1634E: case 0xC16352:
    case 0xC16360: case 0xC16376: case 0xC1638A: case 0xC1639E:
    case 0xC163C2: case 0xC163C4: case 0xC163C6: case 0xC163CA:
    case 0xC163D8: case 0xC163DC:
        width=4; goto move;
    case 0xC0EF12: case 0xC0EF32: case 0xC0EF7A: case 0xC0EF8C:
    case 0xC0EFA4: case 0xC0EFB8: case 0xC1633C: case 0xC16348:
    case 0xC16368: case 0xC163AC: case 0xC163C0: case 0xC163F2:
    case 0xC16400:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC0EF16: case 0xC0EF40: case 0xC0EF54: case 0xC0EF60:
    case 0xC0EFC2: case 0xC162E4: case 0xC162EA: case 0xC16314:
    case 0xC1632C: case 0xC16356: case 0xC1637A: case 0xC16396:
    case 0xC163CE: case 0xC163E0:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0EF1C: case 0xC0EF24: case 0xC0EF46: case 0xC0EF5A:
    case 0xC0EF66: case 0xC0EFC8: case 0xC16332: case 0xC16364:
    case 0xC16380: case 0xC1639C: case 0xC163E6:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0EF26:
        width=4; value=m68ki_read_imm_32(); operation='&'; goto immediate_logic;
    case 0xC0EF36: case 0xC16326: case 0xC16390:
        address=cache_step_address(mode,reg,4); m68ki_push_32(address); break;
    case 0xC0EF68: case 0xC0EF94: case 0xC16342: case 0xC163BA:
        width=4; goto move;
    case 0xC0EF76: case 0xC0EFB0: case 0xC16338: case 0xC163A2:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC0EF78:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC0EF80: case 0xC0EF92: case 0xC0EFAA: case 0xC16310:
        step_branch(pc,opcode,1); break;
    case 0xC0EF82: case 0xC0EF9A: case 0xC163E8:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC0EF8A: case 0xC0EFA2: case 0xC162F8: case 0xC16306:
    case 0xC16366: case 0xC163F0:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0EFAC:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC0EFB6:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC0EFCE: case 0xC1633E: case 0xC1636A: case 0xC16382:
    case 0xC163AE: case 0xC163F4: case 0xC16402:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC0EFD0: case 0xC1631A: case 0xC16340: case 0xC1636C:
    case 0xC16384: case 0xC163B0: case 0xC163F6: case 0xC16404:
        REG_PC=m68ki_pull_32(); break;
    case 0xC162F0: case 0xC162FE: case 0xC16308: case 0xC1636E:
    case 0xC163B2: case 0xC163F8:
        width=2; goto move;
    case 0xC162F6:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC162FA: case 0xC16312:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC16304:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC1633A: case 0xC163A4:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC1635C: case 0xC163D4:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC163A6:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
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
static const uint32_t owned_C0F56A[]={
    0xC0F56A,0xC0F56E,0xC0F572,0xC0F574,0xC0F576,0xC0F57A,0xC0F57E,0xC0F582,
    0xC0F586,0xC0F588,0xC0F58C,0xC0F592,0xC0F598,0xC0F59C,0xC0F5A0,0xC0F5A2,
    0xC0F5A6,0xC0F5AA,0xC0F5AE,0xC0F5B2,0xC0F5B6,0xC0F5BA,0xC0F5BC,0xC0F5C0,
    0xC0F5C2,0xC0F5C6,0xC0F5CA,0xC0F5CE,0xC0F5D2,0xC0F5D6,0xC0F5D8,0xC0F5DC,
    0xC0F5DE,0xC0F5E2,0xC0F5E4,0xC0F5E8,0xC0F5EC,0xC0F5EE,0xC0F5F2,0xC0F5F4,
    0xC0F5F6,
};
int glue_C0F56A_owns(uint32_t pc) { return owns_pc(owned_C0F56A,sizeof owned_C0F56A/sizeof owned_C0F56A[0],pc); }
extern int glue_C24E2C_step(void);
int glue_C0F56A_step(void) { if(!glue_C0F56A_owns(REG_PC)) return 0; return glue_C24E2C_step(); }
static const uint32_t owned_C0EF08[]={
    0xC0EF08,0xC0EF0C,0xC0EF12,0xC0EF14,0xC0EF16,0xC0EF1C,0xC0EF1E,0xC0EF24,
    0xC0EF26,0xC0EF2C,0xC0EF32,0xC0EF34,0xC0EF36,0xC0EF3C,0xC0EF40,0xC0EF46,
    0xC0EF48,0xC0EF4E,0xC0EF50,0xC0EF54,0xC0EF5A,0xC0EF5C,0xC0EF60,0xC0EF66,
    0xC0EF68,0xC0EF6E,0xC0EF72,0xC0EF76,0xC0EF78,0xC0EF7A,0xC0EF7C,0xC0EF80,
    0xC0EF82,0xC0EF8A,0xC0EF8C,0xC0EF8E,0xC0EF92,0xC0EF94,0xC0EF9A,0xC0EFA2,
    0xC0EFA4,0xC0EFA6,0xC0EFAA,0xC0EFAC,0xC0EFB0,0xC0EFB6,0xC0EFB8,0xC0EFBA,
    0xC0EFBC,0xC0EFC2,0xC0EFC8,0xC0EFCA,0xC0EFCE,0xC0EFD0,
};
int glue_C0EF08_owns(uint32_t pc) { return owns_pc(owned_C0EF08,sizeof owned_C0EF08/sizeof owned_C0EF08[0],pc); }
int glue_C0EF08_step(void) { if(!glue_C0EF08_owns(REG_PC)) return 0; return postflight_file_callers_step(); }
static const uint32_t owned_C162E4[]={
    0xC162E4,0xC162EA,0xC162F0,0xC162F6,0xC162F8,0xC162FA,0xC162FE,0xC16304,
    0xC16306,0xC16308,0xC16310,0xC16312,0xC16314,0xC1631A,
};
int glue_C162E4_owns(uint32_t pc) { return owns_pc(owned_C162E4,sizeof owned_C162E4/sizeof owned_C162E4[0],pc); }
int glue_C162E4_step(void) { if(!glue_C162E4_owns(REG_PC)) return 0; return postflight_file_callers_step(); }
static const uint32_t owned_C1631C[]={
    0xC1631C,0xC16320,0xC16326,0xC1632C,0xC16332,0xC16334,0xC16338,0xC1633A,
    0xC1633C,0xC1633E,0xC16340,0xC16342,0xC16348,0xC1634A,0xC1634C,0xC1634E,
    0xC16352,0xC16356,0xC1635C,0xC16360,0xC16364,0xC16366,0xC16368,0xC1636A,
    0xC1636C,0xC1636E,0xC16376,0xC1637A,0xC16380,0xC16382,0xC16384,
};
int glue_C1631C_owns(uint32_t pc) { return owns_pc(owned_C1631C,sizeof owned_C1631C/sizeof owned_C1631C[0],pc); }
int glue_C1631C_step(void) { if(!glue_C1631C_owns(REG_PC)) return 0; return postflight_file_callers_step(); }
static const uint32_t owned_C16386[]={
    0xC16386,0xC1638A,0xC16390,0xC16396,0xC1639C,0xC1639E,0xC163A2,0xC163A4,
    0xC163A6,0xC163AC,0xC163AE,0xC163B0,0xC163B2,0xC163BA,0xC163C0,0xC163C2,
    0xC163C4,0xC163C6,0xC163CA,0xC163CE,0xC163D4,0xC163D8,0xC163DC,0xC163E0,
    0xC163E6,0xC163E8,0xC163F0,0xC163F2,0xC163F4,0xC163F6,0xC163F8,0xC16400,
    0xC16402,0xC16404,
};
int glue_C16386_owns(uint32_t pc) { return owns_pc(owned_C16386,sizeof owned_C16386/sizeof owned_C16386[0],pc); }
int glue_C16386_step(void) { if(!glue_C16386_owns(REG_PC)) return 0; return postflight_file_callers_step(); }
