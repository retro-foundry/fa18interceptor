/* Complete hud projection parents family source CPU/bus/event boundaries.
 * Readable behavior lives in hud_projection_parents.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family hud_projection_parents. */
#include "glue_hud_projection_parents_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_hud_projection_parents.h"

static int hud_projection_parents_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0CFFA: case 0xC0D008: case 0xC0D02C: case 0xC33B80:
    case 0xC33C04: case 0xC33C18: case 0xC33C4C: case 0xC33D3A:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,2,(int)reg); else { address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); } break;
    case 0xC0CFFE: case 0xC0D00C: case 0xC0D012: case 0xC0D018:
    case 0xC0D01E: case 0xC0D020: case 0xC0D022: case 0xC0D02A:
    case 0xC0D03C: case 0xC0DAEE: case 0xC0DAF2: case 0xC0DAF6:
    case 0xC0DB00: case 0xC0DB02: case 0xC0DB04: case 0xC0DB12:
    case 0xC0DB14: case 0xC0DB16: case 0xC0DB32: case 0xC32796:
    case 0xC327A6: case 0xC327A8: case 0xC327AC: case 0xC327DA:
    case 0xC327F6: case 0xC33B66: case 0xC33B6A: case 0xC33B8C:
    case 0xC33BA4: case 0xC33C2C: case 0xC33C36: case 0xC33C66:
    case 0xC33C7A: case 0xC33C88: case 0xC33CA2: case 0xC33CAA:
    case 0xC33CB2: case 0xC33CB8: case 0xC33CFE: case 0xC33D00:
    case 0xC33D02: case 0xC33D10: case 0xC33D12: case 0xC33D14:
    case 0xC33D16: case 0xC33D30: case 0xC33D32: case 0xC33D42:
    case 0xC33D46: case 0xC33D50: case 0xC33D52: case 0xC33D5E:
    case 0xC33D60: case 0xC33D6C:
        width=2; goto move;
    case 0xC0D002: case 0xC0D004: case 0xC0D006:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC0D00E: case 0xC0D014: case 0xC0D01A: case 0xC327B8:
    case 0xC33B7C: case 0xC33B9A: case 0xC33BB0:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC0D010: case 0xC0D016: case 0xC0D01C: case 0xC33B9C:
    case 0xC33BB2:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC0D024: case 0xC0D040: case 0xC0DB3A: case 0xC327FE:
    case 0xC332DE: case 0xC332EC: case 0xC33BE4: case 0xC33C12:
    case 0xC33CBE: case 0xC33D34:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0D030: case 0xC33B5C: case 0xC33B6E: case 0xC33B76:
    case 0xC33BCA: case 0xC33C0C: case 0xC33C20: case 0xC33C42:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC0D032: case 0xC0D03A: case 0xC33B74: case 0xC33B92:
    case 0xC33C0E: case 0xC33C22:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC0D034: case 0xC33C70:
        step_divide_unsigned(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC0D036: case 0xC327B4: case 0xC33B38: case 0xC33B9E:
    case 0xC33BB4: case 0xC33BBA: case 0xC33C8C:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC0D046: case 0xC0DB40: case 0xC32678: case 0xC327F4:
    case 0xC332BA: case 0xC332FA: case 0xC33B36: case 0xC33CD0:
    case 0xC33DA2:
        REG_PC=m68ki_pull_32(); break;
    case 0xC0DAFA: case 0xC327CC: case 0xC33B42: case 0xC33C30:
    case 0xC33C5A: case 0xC33C74: case 0xC33CD2: case 0xC33CF8:
    case 0xC33D4A:
        A(destination)=(mode==7 && reg==0)?(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16():cache_step_address(mode,reg,4); break;
    case 0xC0DB06: case 0xC0DB08: case 0xC0DB0A: case 0xC0DB18:
    case 0xC0DB1A: case 0xC0DB1C: case 0xC0DB24: case 0xC0DB26:
    case 0xC0DB28: case 0xC33D04: case 0xC33D06: case 0xC33D08:
    case 0xC33D18: case 0xC33D1A: case 0xC33D1C: case 0xC33D24:
    case 0xC33D26: case 0xC33D28: case 0xC33D54: case 0xC33D58:
    case 0xC33D62: case 0xC33D66: case 0xC33D6E: case 0xC33D72:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC0DB0C: case 0xC0DB0E: case 0xC0DB1E: case 0xC0DB20:
    case 0xC0DB2A: case 0xC0DB2C: case 0xC327BE: case 0xC327C0:
    case 0xC33D0A: case 0xC33D0C: case 0xC33D1E: case 0xC33D20:
    case 0xC33D2A: case 0xC33D2C: case 0xC33D5C: case 0xC33D6A:
    case 0xC33D76: case 0xC33D7E: case 0xC33D82: case 0xC33D86:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC0DB10: case 0xC0DB22: case 0xC0DB2E: case 0xC33CF2:
    case 0xC33CF4: case 0xC33CF6: case 0xC33D0E: case 0xC33D22:
    case 0xC33D2E: case 0xC33D78: case 0xC33D7A: case 0xC33D7C:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC0DB30:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC32662: case 0xC3266C: case 0xC332C6: case 0xC332E4:
    case 0xC33BC2: case 0xC33BEC: case 0xC33C92: case 0xC33CC4:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC32668: case 0xC32672: case 0xC332EA: case 0xC33B5A:
    case 0xC33BF2:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC32674: case 0xC32804: case 0xC33B88: case 0xC33BEA:
    case 0xC33CA8:
        step_branch(pc,opcode,1); break;
    case 0xC32794:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC3279A: case 0xC33C86:
        width=4; goto move;
    case 0xC327A0: case 0xC327D6: case 0xC327E6: case 0xC327E8:
    case 0xC327EE: case 0xC332BC: case 0xC33C6A:
        width=4; goto move;
    case 0xC327A4: case 0xC327AE: case 0xC327B0: case 0xC327BA:
    case 0xC327CA: case 0xC33C26: case 0xC33C54:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC327AA: case 0xC33B4E: case 0xC33BDC:
        width=1; goto move;
    case 0xC327B2: case 0xC33B62: case 0xC33BD0: case 0xC33C48:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC327BC:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC327C2:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC327C6:
        width=2; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC327D2: case 0xC33B48: case 0xC33C60: case 0xC33CD8:
    case 0xC33D4C:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC327D8: case 0xC327DC:
        step_swap(&D(reg)); break;
    case 0xC327DE:
        width=2; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC327E0:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC327E4: case 0xC332CC: case 0xC33B40: case 0xC33BC8:
    case 0xC33C98: case 0xC33CA0: case 0xC33CCA:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC327EA: case 0xC332CE: case 0xC332D2: case 0xC332D6:
    case 0xC332DA: case 0xC332F2: case 0xC332F6: case 0xC33C3E:
    case 0xC33C82: case 0xC33CCC:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC327F0:
        step_dbf(pc,&D(reg)); break;
    case 0xC332B4: case 0xC33BF4:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC33B52:
        width=1; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC33B56:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC33B94: case 0xC33BAA: case 0xC33CAC:
        width=2; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC33BA2: case 0xC33BB8: case 0xC33BC0: case 0xC33C90:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC33BD2: case 0xC33BFA:
        width=4; value=m68ki_read_imm_32(); operation='|'; goto immediate_logic;
    case 0xC33C6C:
        timer_multiply_unsigned(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC33C9A:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='&'; goto bit_value;
    case 0xC33CDE: case 0xC33D8A: case 0xC33D92: case 0xC33D9A:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC33CE6: case 0xC33CEA: case 0xC33CEE:
        width=4; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
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
static const uint32_t owned_C0DAEE[]={
    0xC0DAEE,0xC0DAF2,0xC0DAF6,0xC0DAFA,0xC0DB00,0xC0DB02,0xC0DB04,0xC0DB06,
    0xC0DB08,0xC0DB0A,0xC0DB0C,0xC0DB0E,0xC0DB10,0xC0DB12,0xC0DB14,0xC0DB16,
    0xC0DB18,0xC0DB1A,0xC0DB1C,0xC0DB1E,0xC0DB20,0xC0DB22,0xC0DB24,0xC0DB26,
    0xC0DB28,0xC0DB2A,0xC0DB2C,0xC0DB2E,0xC0DB30,0xC0DB32,0xC0DB3A,0xC0DB40,
};
int glue_C0DAEE_owns(uint32_t pc) { return owns_pc(owned_C0DAEE,sizeof owned_C0DAEE/sizeof owned_C0DAEE[0],pc); }
int glue_C0DAEE_complete_step(void) { if(!glue_C0DAEE_owns(REG_PC)) return 0; return hud_projection_parents_step(); }
static const uint32_t owned_C0CFFA[]={
    0xC0CFFA,0xC0CFFE,0xC0D002,0xC0D004,0xC0D006,0xC0D008,0xC0D00C,0xC0D00E,
    0xC0D010,0xC0D012,0xC0D014,0xC0D016,0xC0D018,0xC0D01A,0xC0D01C,0xC0D01E,
    0xC0D020,0xC0D022,0xC0D024,0xC0D02A,0xC0D02C,0xC0D030,0xC0D032,0xC0D034,
    0xC0D036,0xC0D03A,0xC0D03C,0xC0D040,0xC0D046,
};
int glue_C0CFFA_owns(uint32_t pc) { return owns_pc(owned_C0CFFA,sizeof owned_C0CFFA/sizeof owned_C0CFFA[0],pc); }
int glue_C0CFFA_complete_step(void) { if(!glue_C0CFFA_owns(REG_PC)) return 0; return hud_projection_parents_step(); }
static const uint32_t owned_C33CD2[]={
    0xC33CD2,0xC33CD8,0xC33CDE,0xC33CE6,0xC33CEA,0xC33CEE,0xC33CF2,0xC33CF4,
    0xC33CF6,0xC33CF8,0xC33CFE,0xC33D00,0xC33D02,0xC33D04,0xC33D06,0xC33D08,
    0xC33D0A,0xC33D0C,0xC33D0E,0xC33D10,0xC33D12,0xC33D14,0xC33D16,0xC33D18,
    0xC33D1A,0xC33D1C,0xC33D1E,0xC33D20,0xC33D22,0xC33D24,0xC33D26,0xC33D28,
    0xC33D2A,0xC33D2C,0xC33D2E,0xC33D30,0xC33D32,0xC33D34,0xC33D3A,0xC33D42,
    0xC33D46,0xC33D4A,0xC33D4C,0xC33D50,0xC33D52,0xC33D54,0xC33D58,0xC33D5C,
    0xC33D5E,0xC33D60,0xC33D62,0xC33D66,0xC33D6A,0xC33D6C,0xC33D6E,0xC33D72,
    0xC33D76,0xC33D78,0xC33D7A,0xC33D7C,0xC33D7E,0xC33D82,0xC33D86,0xC33D8A,
    0xC33D92,0xC33D9A,0xC33DA2,
};
int glue_C33CD2_owns(uint32_t pc) { return owns_pc(owned_C33CD2,sizeof owned_C33CD2/sizeof owned_C33CD2[0],pc); }
int glue_C33CD2_complete_step(void) { if(!glue_C33CD2_owns(REG_PC)) return 0; return hud_projection_parents_step(); }
static const uint32_t owned_C33B38[]={
    0xC33B36,0xC33B38,0xC33B40,0xC33B42,0xC33B48,0xC33B4E,0xC33B52,0xC33B56,
    0xC33B5A,0xC33B5C,0xC33B62,0xC33B66,0xC33B6A,0xC33B6E,0xC33B74,0xC33B76,
    0xC33B7C,0xC33B80,0xC33B88,0xC33B8C,0xC33B92,0xC33B94,0xC33B9A,0xC33B9C,
    0xC33B9E,0xC33BA2,0xC33BA4,0xC33BAA,0xC33BB0,0xC33BB2,0xC33BB4,0xC33BB8,
    0xC33BBA,0xC33BC0,0xC33BC2,0xC33BC8,0xC33BCA,0xC33BD0,0xC33BD2,0xC33BDC,
    0xC33BE4,0xC33BEA,0xC33BEC,0xC33BF2,0xC33BF4,0xC33BFA,0xC33C04,0xC33C0C,
    0xC33C0E,0xC33C12,0xC33C18,0xC33C20,0xC33C22,0xC33C26,0xC33C2C,0xC33C30,
    0xC33C36,0xC33C3E,0xC33C42,0xC33C48,0xC33C4C,0xC33C54,0xC33C5A,0xC33C60,
    0xC33C66,0xC33C6A,0xC33C6C,0xC33C70,0xC33C74,0xC33C7A,0xC33C82,0xC33C86,
    0xC33C88,0xC33C8C,0xC33C90,0xC33C92,0xC33C98,0xC33C9A,0xC33CA0,0xC33CA2,
    0xC33CA8,0xC33CAA,0xC33CAC,0xC33CB2,0xC33CB8,0xC33CBE,0xC33CC4,0xC33CCA,
    0xC33CCC,0xC33CD0,
};
int glue_C33B38_owns(uint32_t pc) { return owns_pc(owned_C33B38,sizeof owned_C33B38/sizeof owned_C33B38[0],pc); }
int glue_C33B38_complete_step(void) { if(!glue_C33B38_owns(REG_PC)) return 0; return hud_projection_parents_step(); }
static const uint32_t owned_C332BC[]={
    0xC332B4,0xC332BA,0xC332BC,0xC332C6,0xC332CC,0xC332CE,0xC332D2,0xC332D6,
    0xC332DA,0xC332DE,0xC332E4,0xC332EA,0xC332EC,0xC332F2,0xC332F6,0xC332FA,
};
int glue_C332BC_owns(uint32_t pc) { return owns_pc(owned_C332BC,sizeof owned_C332BC/sizeof owned_C332BC[0],pc); }
int glue_C332BC_complete_step(void) { if(!glue_C332BC_owns(REG_PC)) return 0; return hud_projection_parents_step(); }
static const uint32_t owned_C32662[]={
    0xC32662,0xC32668,0xC3266C,0xC32672,0xC32674,0xC32678,0xC32794,0xC32796,
    0xC3279A,0xC327A0,0xC327A4,0xC327A6,0xC327A8,0xC327AA,0xC327AC,0xC327AE,
    0xC327B0,0xC327B2,0xC327B4,0xC327B8,0xC327BA,0xC327BC,0xC327BE,0xC327C0,
    0xC327C2,0xC327C6,0xC327CA,0xC327CC,0xC327D2,0xC327D6,0xC327D8,0xC327DA,
    0xC327DC,0xC327DE,0xC327E0,0xC327E4,0xC327E6,0xC327E8,0xC327EA,0xC327EE,
    0xC327F0,0xC327F4,0xC327F6,0xC327FE,0xC32804,
};
int glue_C32662_owns(uint32_t pc) { return owns_pc(owned_C32662,sizeof owned_C32662/sizeof owned_C32662[0],pc); }
int glue_C32662_complete_step(void) { if(!glue_C32662_owns(REG_PC)) return 0; return hud_projection_parents_step(); }
