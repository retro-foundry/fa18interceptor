/* Complete postflight completion family source CPU/bus/event boundaries.
 * Readable behavior lives in postflight_completion.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family postflight_completion. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_postflight_completion.h"

static int postflight_completion_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC091E6: case 0xC09244:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC091EA: case 0xC091F0: case 0xC0F968: case 0xC0F986:
    case 0xC11818: case 0xC11824: case 0xC11866: case 0xC118DA:
    case 0xC11928: case 0xC11984: case 0xC119C8:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC091F2:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC091F6: case 0xC091F8: case 0xC091FA: case 0xC0920A:
    case 0xC0920C: case 0xC0920E: case 0xC09220: case 0xC0F946:
    case 0xC0F960: case 0xC0F974: case 0xC1104C: case 0xC1105E:
    case 0xC117A6: case 0xC117B2: case 0xC117BC: case 0xC117C6:
    case 0xC117E2: case 0xC117EC: case 0xC1180A: case 0xC11838:
    case 0xC11842: case 0xC1185E: case 0xC11872: case 0xC1187C:
    case 0xC11886: case 0xC118A0: case 0xC118C2: case 0xC118CC:
    case 0xC11906: case 0xC11910: case 0xC1191A: case 0xC11920:
    case 0xC11934: case 0xC11962: case 0xC1196C: case 0xC11976:
    case 0xC1197C: case 0xC119C0: case 0xC119D4: case 0xC11A04:
        width=2; goto move;
    case 0xC091FC: case 0xC091FE: case 0xC09202: case 0xC09210:
    case 0xC09214: case 0xC09218: case 0xC09222: case 0xC09226:
    case 0xC0922A:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC09206: case 0xC09208: case 0xC0921C: case 0xC0921E:
    case 0xC0922E: case 0xC09230: case 0xC09238: case 0xC0923C:
    case 0xC09240:
        width=4; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC09232: case 0xC09234: case 0xC09236:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC09248: case 0xC0F972: case 0xC0F990: case 0xC11076:
    case 0xC1182E: case 0xC11870: case 0xC1189E: case 0xC118E4:
    case 0xC118FA: case 0xC11932: case 0xC11956: case 0xC119D2:
    case 0xC11A24:
        REG_PC=m68ki_pull_32(); break;
    case 0xC0F94C: case 0xC0F97A: case 0xC11052: case 0xC11878:
    case 0xC118A6: case 0xC1193A: case 0xC119DA:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC0F94E: case 0xC0F97C: case 0xC11054: case 0xC1187A:
    case 0xC118A8: case 0xC118EE: case 0xC11904: case 0xC1193C:
    case 0xC11960: case 0xC119DC:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC0F950: case 0xC0F956: case 0xC0F97E: case 0xC11058:
    case 0xC11066: case 0xC11788: case 0xC11792: case 0xC117D2:
    case 0xC117DC: case 0xC117F2: case 0xC117FA: case 0xC11832:
    case 0xC11848: case 0xC1188C: case 0xC118B6: case 0xC118E6:
    case 0xC118FC: case 0xC11940: case 0xC11946: case 0xC11958:
    case 0xC11990: case 0xC1199E: case 0xC119A4: case 0xC119B6:
    case 0xC119E0: case 0xC119E6: case 0xC119F4: case 0xC119FC:
    case 0xC11A0E: case 0xC11A14:
        width=1; goto move;
    case 0xC0F95C:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;
    case 0xC0F95E: case 0xC11790: case 0xC117B0: case 0xC11850:
    case 0xC118C0: case 0xC1199A: case 0xC119BE:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0F96C: case 0xC0F98A: case 0xC1106C: case 0xC1181C:
    case 0xC11828: case 0xC1186A: case 0xC11894: case 0xC118DE:
    case 0xC118F0: case 0xC1192C: case 0xC1194C: case 0xC11988:
    case 0xC119AA: case 0xC119CC: case 0xC11A1A:
        width=4; goto move;
    case 0xC11056: case 0xC117DA: case 0xC11830: case 0xC1193E:
    case 0xC1199C: case 0xC119DE: case 0xC11A0C:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC1178E: case 0xC11798: case 0xC1179C: case 0xC11800:
    case 0xC1184E: case 0xC118EC: case 0xC11902: case 0xC1195E:
    case 0xC119EC:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC1179A: case 0xC117A2: case 0xC119F2:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC117AC:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC117B8: case 0xC117C2: case 0xC117E8: case 0xC1190C:
    case 0xC11916: case 0xC11968: case 0xC11972:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC117CC: case 0xC11804: case 0xC11852: case 0xC11858:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC117F8: case 0xC119BC:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC11802:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC11812: case 0xC118D4:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC11822: case 0xC118CA: case 0xC1198E: case 0xC119B4:
        step_branch(pc,opcode,1); break;
    case 0xC1183E: case 0xC11882:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC118AA:
        address=cache_step_address(mode,reg,4); m68ki_push_32(address); break;
    case 0xC118B0:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC118B4:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC118BC: case 0xC11996:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
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
static const uint32_t owned_C11788[]={
    0xC11788,0xC1178E,0xC11790,0xC11792,0xC11798,0xC1179A,0xC1179C,0xC117A2,
    0xC117A6,0xC117AC,0xC117B0,0xC117B2,0xC117B8,0xC117BC,0xC117C2,0xC117C6,
    0xC117CC,0xC117D2,0xC117DA,0xC117DC,0xC117E2,0xC117E8,0xC117EC,0xC117F2,
    0xC117F8,0xC117FA,0xC11800,0xC11802,0xC11804,0xC1180A,0xC11812,0xC11818,
    0xC1181C,0xC11822,0xC11824,0xC11828,0xC1182E,
};
int glue_C11788_owns(uint32_t pc) { return owns_pc(owned_C11788,sizeof owned_C11788/sizeof owned_C11788[0],pc); }
int glue_C11788_step(void) { if(!glue_C11788_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C11830[]={
    0xC11830,0xC11832,0xC11838,0xC1183E,0xC11842,0xC11848,0xC1184E,0xC11850,
    0xC11852,0xC11858,0xC1185E,0xC11866,0xC1186A,0xC11870,
};
int glue_C11830_owns(uint32_t pc) { return owns_pc(owned_C11830,sizeof owned_C11830/sizeof owned_C11830[0],pc); }
int glue_C11830_step(void) { if(!glue_C11830_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C11872[]={
    0xC11872,0xC11878,0xC1187A,0xC1187C,0xC11882,0xC11886,0xC1188C,0xC11894,
    0xC1189E,
};
int glue_C11872_owns(uint32_t pc) { return owns_pc(owned_C11872,sizeof owned_C11872/sizeof owned_C11872[0],pc); }
int glue_C11872_step(void) { if(!glue_C11872_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C118A0[]={
    0xC118A0,0xC118A6,0xC118A8,0xC118AA,0xC118B0,0xC118B4,0xC118B6,0xC118BC,
    0xC118C0,0xC118C2,0xC118CA,0xC118CC,0xC118D4,0xC118DA,0xC118DE,0xC118E4,
};
int glue_C118A0_owns(uint32_t pc) { return owns_pc(owned_C118A0,sizeof owned_C118A0/sizeof owned_C118A0[0],pc); }
int glue_C118A0_step(void) { if(!glue_C118A0_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C118E6[]={
    0xC118E6,0xC118EC,0xC118EE,0xC118F0,0xC118FA,
};
int glue_C118E6_owns(uint32_t pc) { return owns_pc(owned_C118E6,sizeof owned_C118E6/sizeof owned_C118E6[0],pc); }
int glue_C118E6_step(void) { if(!glue_C118E6_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C118FC[]={
    0xC118FC,0xC11902,0xC11904,0xC11906,0xC1190C,0xC11910,0xC11916,0xC1191A,
    0xC11920,0xC11928,0xC1192C,0xC11932,
};
int glue_C118FC_owns(uint32_t pc) { return owns_pc(owned_C118FC,sizeof owned_C118FC/sizeof owned_C118FC[0],pc); }
int glue_C118FC_step(void) { if(!glue_C118FC_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C11934[]={
    0xC11934,0xC1193A,0xC1193C,0xC1193E,0xC11940,0xC11946,0xC1194C,0xC11956,
};
int glue_C11934_owns(uint32_t pc) { return owns_pc(owned_C11934,sizeof owned_C11934/sizeof owned_C11934[0],pc); }
int glue_C11934_step(void) { if(!glue_C11934_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C11958[]={
    0xC11958,0xC1195E,0xC11960,0xC11962,0xC11968,0xC1196C,0xC11972,0xC11976,
    0xC1197C,0xC11984,0xC11988,0xC1198E,0xC11990,0xC11996,0xC1199A,0xC1199C,
    0xC1199E,0xC119A4,0xC119AA,0xC119B4,0xC119B6,0xC119BC,0xC119BE,0xC119C0,
    0xC119C8,0xC119CC,0xC119D2,
};
int glue_C11958_owns(uint32_t pc) { return owns_pc(owned_C11958,sizeof owned_C11958/sizeof owned_C11958[0],pc); }
int glue_C11958_step(void) { if(!glue_C11958_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C119D4[]={
    0xC119D4,0xC119DA,0xC119DC,0xC119DE,0xC119E0,0xC119E6,0xC119EC,0xC119F2,
    0xC119F4,0xC119FC,0xC11A04,0xC11A0C,0xC11A0E,0xC11A14,0xC11A1A,0xC11A24,
};
int glue_C119D4_owns(uint32_t pc) { return owns_pc(owned_C119D4,sizeof owned_C119D4/sizeof owned_C119D4[0],pc); }
int glue_C119D4_step(void) { if(!glue_C119D4_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C1104C[]={
    0xC1104C,0xC11052,0xC11054,0xC11056,0xC11058,0xC1105E,0xC11066,0xC1106C,
    0xC11076,
};
int glue_C1104C_owns(uint32_t pc) { return owns_pc(owned_C1104C,sizeof owned_C1104C/sizeof owned_C1104C[0],pc); }
int glue_C1104C_step(void) { if(!glue_C1104C_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C0F946[]={
    0xC0F946,0xC0F94C,0xC0F94E,0xC0F950,0xC0F956,0xC0F95C,0xC0F95E,0xC0F960,
    0xC0F968,0xC0F96C,0xC0F972,
};
int glue_C0F946_owns(uint32_t pc) { return owns_pc(owned_C0F946,sizeof owned_C0F946/sizeof owned_C0F946[0],pc); }
int glue_C0F946_step(void) { if(!glue_C0F946_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C0F974[]={
    0xC0F974,0xC0F97A,0xC0F97C,0xC0F97E,0xC0F986,0xC0F98A,0xC0F990,
};
int glue_C0F974_owns(uint32_t pc) { return owns_pc(owned_C0F974,sizeof owned_C0F974/sizeof owned_C0F974[0],pc); }
int glue_C0F974_step(void) { if(!glue_C0F974_owns(REG_PC)) return 0; return postflight_completion_step(); }
static const uint32_t owned_C091E6[]={
    0xC091E6,0xC091EA,0xC091F0,0xC091F2,0xC091F6,0xC091F8,0xC091FA,0xC091FC,
    0xC091FE,0xC09202,0xC09206,0xC09208,0xC0920A,0xC0920C,0xC0920E,0xC09210,
    0xC09214,0xC09218,0xC0921C,0xC0921E,0xC09220,0xC09222,0xC09226,0xC0922A,
    0xC0922E,0xC09230,0xC09232,0xC09234,0xC09236,0xC09238,0xC0923C,0xC09240,
    0xC09244,0xC09248,
};
int glue_C091E6_owns(uint32_t pc) { return owns_pc(owned_C091E6,sizeof owned_C091E6/sizeof owned_C091E6[0],pc); }
int glue_C091E6_step(void) { if(!glue_C091E6_owns(REG_PC)) return 0; return postflight_completion_step(); }
