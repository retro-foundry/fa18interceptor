/* Complete flight motion helpers family source CPU/bus/event boundaries.
 * Readable behavior lives in flight_motion_helpers.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family flight_motion_helpers. */
#include "glue_flight_motion_helpers_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_flight_motion_helpers.h"

static int flight_motion_helpers_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC26322: case 0xC26324: case 0xC26326: case 0xC2632C:
    case 0xC26C76: case 0xC26C8E:
        width=4; goto move;
    case 0xC26328: case 0xC26330: case 0xC26340: case 0xC26342:
    case 0xC26C8A: case 0xC26C92: case 0xC26CA2: case 0xC26CA4:
    case 0xC26CB2: case 0xC26CB4:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC2632A: case 0xC26360: case 0xC2638A: case 0xC26C8C:
    case 0xC26D2E:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC26332: case 0xC26C94: case 0xC26E8C: case 0xC26E8E:
    case 0xC26E90:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC26334: case 0xC26336: case 0xC26C96: case 0xC26C98:
        step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC26338: case 0xC2633A: case 0xC26C9A: case 0xC26C9C:
        renderer_divide(&D(destination),(int16_t)cache_step_read(mode,reg,2)); break;
    case 0xC2633C: case 0xC2633E: case 0xC26C9E: case 0xC26CA0:
    case 0xC26E4C: case 0xC26E4E: case 0xC26E54: case 0xC26E56:
    case 0xC26E5C: case 0xC26E5E: case 0xC26E92: case 0xC26E94:
    case 0xC26E96:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC26344: case 0xC2634C: case 0xC26CA6: case 0xC26CAE:
    case 0xC26E7E: case 0xC26E98: case 0xC26E9A:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC26348: case 0xC26CAA:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC26350: case 0xC2639A: case 0xC26CBA: case 0xC26D88:
    case 0xC26EAA:
        REG_PC=m68ki_pull_32(); break;
    case 0xC26352: case 0xC26C7C: case 0xC26CDC: case 0xC26CF6:
    case 0xC26CFE: case 0xC26D3C: case 0xC26D44: case 0xC26DA4:
    case 0xC26DB6: case 0xC26E0A: case 0xC26E22:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC26358: case 0xC26384: case 0xC2638C: case 0xC26CB6:
    case 0xC26CBC: case 0xC26D7E: case 0xC26D82: case 0xC26DD6:
    case 0xC26EA0: case 0xC26EA4:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC2635A: case 0xC26386:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC26362: case 0xC26C82: case 0xC26CEC: case 0xC26D5E:
    case 0xC26DB4:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC26366:
        step_dbf(pc,&D(reg)); break;
    case 0xC2636A: case 0xC26CBE: case 0xC26CFC: case 0xC26D30:
    case 0xC26D42: case 0xC26D80: case 0xC26E20: case 0xC26EA2:
        step_branch(pc,opcode,1); break;
    case 0xC2636C: case 0xC26370: case 0xC26C84: case 0xC26CC0:
    case 0xC26CC8: case 0xC26D84: case 0xC26D8A: case 0xC26D92:
    case 0xC26EA6:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC26374: case 0xC2637A: case 0xC2638E: case 0xC26CD8:
    case 0xC26CE2: case 0xC26D06: case 0xC26D16: case 0xC26D1A:
    case 0xC26D4C: case 0xC26D54: case 0xC26D64: case 0xC26D6C:
    case 0xC26DAA: case 0xC26DD8: case 0xC26DFA: case 0xC26E26:
    case 0xC26E48: case 0xC26E4A: case 0xC26E64: case 0xC26E66:
    case 0xC26E68: case 0xC26E6E:
        width=2; goto move;
    case 0xC26380: case 0xC26D24:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC26382: case 0xC26CF4: case 0xC26D3A: case 0xC26D7C:
    case 0xC26DF6: case 0xC26E08:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC26392: case 0xC26D0E: case 0xC26DBC: case 0xC26DC4:
    case 0xC26DDC:
        width=1; goto move;
    case 0xC26C72:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC26C7A: case 0xC26CE8: case 0xC26DB0:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC26CB8:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC26CC4: case 0xC26D8E:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC26CCE: case 0xC26CD0: case 0xC26CD2: case 0xC26D98:
    case 0xC26D9A: case 0xC26D9C: case 0xC26E52: case 0xC26E5A:
    case 0xC26E62:
        motion_lsr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC26CD4: case 0xC26D9E: case 0xC26DA2:
        width=2; goto move;
    case 0xC26CD6: case 0xC26D04: case 0xC26D4A: case 0xC26D62:
    case 0xC26DA0: case 0xC26DEA:
        width=4; goto move;
    case 0xC26CEA: case 0xC26DB2: case 0xC26DE6: case 0xC26DE8:
    case 0xC26DFE: case 0xC26E00: case 0xC26E02: case 0xC26E7A:
    case 0xC26E82:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC26CEE: case 0xC26D34:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC26D0A: case 0xC26D50: case 0xC26D68:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC26D12: case 0xC26DC8: case 0xC26DCE: case 0xC26DE0:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC26D1E: case 0xC26E72: case 0xC26E74: case 0xC26E76:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC26D20:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC26D22: case 0xC26D5C: case 0xC26DD4: case 0xC26E9C:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC26D26: case 0xC26D74: case 0xC26DC0:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC26D28: case 0xC26D76:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC26D58: case 0xC26D70: case 0xC26E78:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC26D5A: case 0xC26D72:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC26DCC:
        action_lsr_byte(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC26DD2: case 0xC26E14: case 0xC26E18: case 0xC26E1C:
    case 0xC26E2E: case 0xC26E30: case 0xC26E34: case 0xC26E3E:
    case 0xC26E40: case 0xC26E44: case 0xC26E86: case 0xC26E8A:
        width=2; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC26DE4:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC26DEE: case 0xC26E0E: case 0xC26E28: case 0xC26E38:
    case 0xC26E6A:
        mask=m68ki_read_imm_16(); address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); break;
    case 0xC26DF2: case 0xC26E04:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='&'; goto bit_value;
    case 0xC26E50: case 0xC26E58: case 0xC26E60: case 0xC26E88:
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
static const uint32_t owned_C26322[]={
    0xC26322,0xC26324,0xC26326,0xC26328,0xC2632A,0xC2632C,0xC26330,0xC26332,
    0xC26334,0xC26336,0xC26338,0xC2633A,0xC2633C,0xC2633E,0xC26340,0xC26342,
    0xC26344,0xC26348,0xC2634C,0xC26350,
};
int glue_C26322_owns(uint32_t pc) { return owns_pc(owned_C26322,sizeof owned_C26322/sizeof owned_C26322[0],pc); }
int glue_C26322_step(void) { if(!glue_C26322_owns(REG_PC)) return 0; return flight_motion_helpers_step(); }
static const uint32_t owned_C26352[]={
    0xC26352,0xC26358,0xC2635A,0xC26360,0xC26362,0xC26366,0xC2636A,0xC2636C,
    0xC26370,0xC26374,0xC2637A,0xC26380,0xC26382,0xC26384,0xC26386,0xC2638A,
    0xC2638C,0xC2638E,0xC26392,0xC2639A,
};
int glue_C26352_owns(uint32_t pc) { return owns_pc(owned_C26352,sizeof owned_C26352/sizeof owned_C26352[0],pc); }
int glue_C26352_step(void) { if(!glue_C26352_owns(REG_PC)) return 0; return flight_motion_helpers_step(); }
static const uint32_t owned_C26C72[]={
    0xC26C72,0xC26C76,0xC26C7A,0xC26C7C,0xC26C82,0xC26C84,0xC26C8A,0xC26C8C,
    0xC26C8E,0xC26C92,0xC26C94,0xC26C96,0xC26C98,0xC26C9A,0xC26C9C,0xC26C9E,
    0xC26CA0,0xC26CA2,0xC26CA4,0xC26CA6,0xC26CAA,0xC26CAE,0xC26CB2,0xC26CB4,
    0xC26CB6,0xC26CB8,0xC26CBA,0xC26CBC,0xC26CBE,
};
int glue_C26C72_owns(uint32_t pc) { return owns_pc(owned_C26C72,sizeof owned_C26C72/sizeof owned_C26C72[0],pc); }
int glue_C26C72_step(void) { if(!glue_C26C72_owns(REG_PC)) return 0; return flight_motion_helpers_step(); }
static const uint32_t owned_C26CC0[]={
    0xC26CC0,0xC26CC4,0xC26CC8,0xC26CCE,0xC26CD0,0xC26CD2,0xC26CD4,0xC26CD6,
    0xC26CD8,0xC26CDC,0xC26CE2,0xC26CE8,0xC26CEA,0xC26CEC,0xC26CEE,0xC26CF4,
    0xC26CF6,0xC26CFC,0xC26CFE,0xC26D04,0xC26D06,0xC26D0A,0xC26D0E,0xC26D12,
    0xC26D16,0xC26D1A,0xC26D1E,0xC26D20,0xC26D22,0xC26D24,0xC26D26,0xC26D28,
    0xC26D2E,0xC26D30,0xC26D34,0xC26D3A,0xC26D3C,0xC26D42,0xC26D44,0xC26D4A,
    0xC26D4C,0xC26D50,0xC26D54,0xC26D58,0xC26D5A,0xC26D5C,0xC26D5E,0xC26D62,
    0xC26D64,0xC26D68,0xC26D6C,0xC26D70,0xC26D72,0xC26D74,0xC26D76,0xC26D7C,
    0xC26D7E,0xC26D80,0xC26D82,0xC26D84,0xC26D88,
};
int glue_C26CC0_owns(uint32_t pc) { return owns_pc(owned_C26CC0,sizeof owned_C26CC0/sizeof owned_C26CC0[0],pc); }
int glue_C26CC0_step(void) { if(!glue_C26CC0_owns(REG_PC)) return 0; return flight_motion_helpers_step(); }
static const uint32_t owned_C26D8A[]={
    0xC26D8A,0xC26D8E,0xC26D92,0xC26D98,0xC26D9A,0xC26D9C,0xC26D9E,0xC26DA0,
    0xC26DA2,0xC26DA4,0xC26DAA,0xC26DB0,0xC26DB2,0xC26DB4,0xC26DB6,0xC26DBC,
    0xC26DC0,0xC26DC4,0xC26DC8,0xC26DCC,0xC26DCE,0xC26DD2,0xC26DD4,0xC26DD6,
    0xC26DD8,0xC26DDC,0xC26DE0,0xC26DE4,0xC26DE6,0xC26DE8,0xC26DEA,0xC26DEE,
    0xC26DF2,0xC26DF6,0xC26DFA,0xC26DFE,0xC26E00,0xC26E02,0xC26E04,0xC26E08,
    0xC26E0A,0xC26E0E,0xC26E14,0xC26E18,0xC26E1C,0xC26E20,0xC26E22,0xC26E26,
    0xC26E28,0xC26E2E,0xC26E30,0xC26E34,0xC26E38,0xC26E3E,0xC26E40,0xC26E44,
    0xC26E48,0xC26E4A,0xC26E4C,0xC26E4E,0xC26E50,0xC26E52,0xC26E54,0xC26E56,
    0xC26E58,0xC26E5A,0xC26E5C,0xC26E5E,0xC26E60,0xC26E62,0xC26E64,0xC26E66,
    0xC26E68,0xC26E6A,0xC26E6E,0xC26E72,0xC26E74,0xC26E76,0xC26E78,0xC26E7A,
    0xC26E7E,0xC26E82,0xC26E86,0xC26E88,0xC26E8A,0xC26E8C,0xC26E8E,0xC26E90,
    0xC26E92,0xC26E94,0xC26E96,0xC26E98,0xC26E9A,0xC26E9C,0xC26EA0,0xC26EA2,
    0xC26EA4,0xC26EA6,0xC26EAA,
};
int glue_C26D8A_owns(uint32_t pc) { return owns_pc(owned_C26D8A,sizeof owned_C26D8A/sizeof owned_C26D8A[0],pc); }
int glue_C26D8A_step(void) { if(!glue_C26D8A_owns(REG_PC)) return 0; return flight_motion_helpers_step(); }
