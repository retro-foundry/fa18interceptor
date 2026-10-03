/* Complete hud stream family source CPU/bus/event boundaries.
 * Readable behavior lives in hud_stream.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family hud_stream. */
#include "glue_hud_stream_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_hud_stream.h"

static int hud_stream_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC308D8: case 0xC308E6: case 0xC308EA: case 0xC308F4:
    case 0xC308FA: case 0xC30908: case 0xC3090C: case 0xC30F46:
    case 0xC30F4C: case 0xC30F5A: case 0xC30F5E: case 0xC30F62:
    case 0xC30F66: case 0xC31B86: case 0xC31BC2: case 0xC31BF6:
        width=4; goto move;
    case 0xC308DA: case 0xC308F6: case 0xC308FC: case 0xC30F48:
    case 0xC30F4E:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC308DC: case 0xC308FE: case 0xC30F50: case 0xC31B8C:
    case 0xC31BC8: case 0xC31BFC: case 0xC33AFE: case 0xC33B2E:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC308E2: case 0xC308EE: case 0xC30904: case 0xC30910:
    case 0xC30F56: case 0xC30F6A: case 0xC31B76: case 0xC31B7E:
    case 0xC31BB6: case 0xC31BEA: case 0xC33AE2: case 0xC33AE6:
    case 0xC33AF0: case 0xC33B12: case 0xC33B16: case 0xC33B20:
        width=2; goto move;
    case 0xC308F2: case 0xC30914: case 0xC30F6E: case 0xC31B74:
    case 0xC31C1E: case 0xC33B04: case 0xC33B34:
        REG_PC=m68ki_pull_32(); break;
    case 0xC308F8: case 0xC30F4A:
        width=4; goto move;
    case 0xC31B84: case 0xC31BC0: case 0xC31BF4:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC31B92: case 0xC31BCE: case 0xC31C02:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC31B94: case 0xC31B98: case 0xC31B9E: case 0xC31BA2:
    case 0xC31BA6: case 0xC31BD0: case 0xC31BD4: case 0xC31BDA:
    case 0xC31BDE: case 0xC31BE2: case 0xC31C04: case 0xC31C08:
    case 0xC31C0E: case 0xC31C12: case 0xC31C16:
        A(destination)=(mode==7 && reg==0)?(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16():cache_step_address(mode,reg,4); break;
    case 0xC31BAA: case 0xC31BE6: case 0xC31C1A:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC31BAE:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC31BB4:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC31BBC: case 0xC31BF0: case 0xC33AE0: case 0xC33B10:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC31BBE: case 0xC31BF2:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC33AD6: case 0xC33AF2: case 0xC33AF8: case 0xC33B06:
    case 0xC33B22: case 0xC33B28:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC33ADC: case 0xC33AEA: case 0xC33B0C: case 0xC33B1A:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC33AE8: case 0xC33B18:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC33AEE: case 0xC33B1E:
        step_branch(pc,opcode,COND_LE()); break;
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
static const uint32_t owned_C308E2[]={
    0xC308E2,0xC308E6,0xC308EA,0xC308EE,0xC308F2,
};
int glue_C308E2_owns(uint32_t pc) { return owns_pc(owned_C308E2,sizeof owned_C308E2/sizeof owned_C308E2[0],pc); }
int glue_C308E2_complete_step(void) { if(!glue_C308E2_owns(REG_PC)) return 0; return hud_stream_step(); }
static const uint32_t owned_C30904[]={
    0xC30904,0xC30908,0xC3090C,0xC30910,0xC30914,
};
int glue_C30904_owns(uint32_t pc) { return owns_pc(owned_C30904,sizeof owned_C30904/sizeof owned_C30904[0],pc); }
int glue_C30904_complete_step(void) { if(!glue_C30904_owns(REG_PC)) return 0; return hud_stream_step(); }
static const uint32_t owned_C308D8[]={
    0xC308D8,0xC308DA,0xC308DC,0xC308E2,0xC308E6,0xC308EA,0xC308EE,0xC308F2,
};
int glue_C308D8_owns(uint32_t pc) { return owns_pc(owned_C308D8,sizeof owned_C308D8/sizeof owned_C308D8[0],pc); }
int glue_C308D8_step(void) { if(!glue_C308D8_owns(REG_PC)) return 0; return hud_stream_step(); }
static const uint32_t owned_C308F4[]={
    0xC308F4,0xC308F6,0xC308F8,0xC308FA,0xC308FC,0xC308FE,0xC30904,0xC30908,
    0xC3090C,0xC30910,0xC30914,
};
int glue_C308F4_owns(uint32_t pc) { return owns_pc(owned_C308F4,sizeof owned_C308F4/sizeof owned_C308F4[0],pc); }
int glue_C308F4_step(void) { if(!glue_C308F4_owns(REG_PC)) return 0; return hud_stream_step(); }
static const uint32_t owned_C30F46[]={
    0xC30F46,0xC30F48,0xC30F4A,0xC30F4C,0xC30F4E,0xC30F50,0xC30F56,0xC30F5A,
    0xC30F5E,0xC30F62,0xC30F66,0xC30F6A,0xC30F6E,
};
int glue_C30F46_owns(uint32_t pc) { return owns_pc(owned_C30F46,sizeof owned_C30F46/sizeof owned_C30F46[0],pc); }
int glue_C30F46_step(void) { if(!glue_C30F46_owns(REG_PC)) return 0; return hud_stream_step(); }
static const uint32_t owned_C31B76[]={
    0xC31B74,0xC31B76,0xC31B7E,0xC31B84,0xC31B86,0xC31B8C,0xC31B92,0xC31B94,
    0xC31B98,0xC31B9E,0xC31BA2,0xC31BA6,0xC31BAA,0xC31BAE,0xC31BB4,0xC31BB6,
    0xC31BBC,0xC31BBE,0xC31BC0,0xC31BC2,0xC31BC8,0xC31BCE,0xC31BD0,0xC31BD4,
    0xC31BDA,0xC31BDE,0xC31BE2,0xC31BE6,0xC31BEA,0xC31BF0,0xC31BF2,0xC31BF4,
    0xC31BF6,0xC31BFC,0xC31C02,0xC31C04,0xC31C08,0xC31C0E,0xC31C12,0xC31C16,
    0xC31C1A,0xC31C1E,
};
int glue_C31B76_owns(uint32_t pc) { return owns_pc(owned_C31B76,sizeof owned_C31B76/sizeof owned_C31B76[0],pc); }
int glue_C31B76_step(void) { if(!glue_C31B76_owns(REG_PC)) return 0; return hud_stream_step(); }
static const uint32_t owned_C33AD6[]={
    0xC33AD6,0xC33ADC,0xC33AE0,0xC33AE2,0xC33AE6,0xC33AE8,0xC33AEA,0xC33AEE,
    0xC33AF0,0xC33AF2,0xC33AF8,0xC33AFE,0xC33B04,
};
int glue_C33AD6_owns(uint32_t pc) { return owns_pc(owned_C33AD6,sizeof owned_C33AD6/sizeof owned_C33AD6[0],pc); }
int glue_C33AD6_step(void) { if(!glue_C33AD6_owns(REG_PC)) return 0; return hud_stream_step(); }
static const uint32_t owned_C33B06[]={
    0xC33B06,0xC33B0C,0xC33B10,0xC33B12,0xC33B16,0xC33B18,0xC33B1A,0xC33B1E,
    0xC33B20,0xC33B22,0xC33B28,0xC33B2E,0xC33B34,
};
int glue_C33B06_owns(uint32_t pc) { return owns_pc(owned_C33B06,sizeof owned_C33B06/sizeof owned_C33B06[0],pc); }
int glue_C33B06_step(void) { if(!glue_C33B06_owns(REG_PC)) return 0; return hud_stream_step(); }
