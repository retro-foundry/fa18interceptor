/* Complete menu setup family source CPU/bus/event boundaries.
 * Readable behavior lives in menu_setup.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family menu_setup. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_menu_setup.h"

static int menu_setup_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0FBE0: case 0xC11BB0: case 0xC17B96: case 0xC24FA4:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0FBE4: case 0xC0FBEC: case 0xC0FC0A: case 0xC0FC16:
    case 0xC0FC32: case 0xC0FC54: case 0xC0FCAA: case 0xC10882:
    case 0xC17BB8: case 0xC17BBE: case 0xC17BCA: case 0xC17BCE:
    case 0xC17BD2: case 0xC17BF0: case 0xC17BF8: case 0xC17C02:
    case 0xC17C08: case 0xC17C0C: case 0xC24FB4: case 0xC24FC2:
    case 0xC24FCC:
        width=4; goto move;
    case 0xC0FBF6: case 0xC11BBA: case 0xC17BA6: case 0xC17BE6:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC0FBFE: case 0xC11BBE: case 0xC11BE0: case 0xC17BAE:
    case 0xC17BEE:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC0FC00: case 0xC0FC20: case 0xC1086E: case 0xC17B9A:
    case 0xC17BDC: case 0xC17C16:
        width=1; goto move;
    case 0xC0FC06: case 0xC17BA0:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC0FC08: case 0xC11BE6: case 0xC17BA2:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0FC14: case 0xC17BB6: case 0xC17BBC: case 0xC17BC8:
    case 0xC17BCC: case 0xC17BD0: case 0xC17BF6: case 0xC17C06:
    case 0xC17C0A:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC0FC18: case 0xC0FC28: case 0xC0FC38: case 0xC0FC4C:
    case 0xC17BB0: case 0xC17C20:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0FC1E: case 0xC0FC3E: case 0xC0FC52: case 0xC0FC66:
    case 0xC0FC6C: case 0xC0FC72: case 0xC0FC78: case 0xC0FC7E:
    case 0xC0FC84: case 0xC0FC8A: case 0xC0FC90: case 0xC0FC96:
    case 0xC0FC9C: case 0xC0FCA2:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0FC2E: case 0xC17BC0: case 0xC17BD4: case 0xC17BFA:
    case 0xC17C0E:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC0FC40:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC0FC46:
        address=cache_step_address(mode,reg,4); m68ki_push_32(address); break;
    case 0xC0FC5E:
        width=4; goto move;
    case 0xC0FC62: case 0xC0FC68: case 0xC0FC6E: case 0xC0FC74:
    case 0xC0FC7A: case 0xC0FC80: case 0xC0FC86: case 0xC0FC8C:
    case 0xC0FC92: case 0xC0FC98: case 0xC0FC9E: case 0xC10876:
    case 0xC11BB4: case 0xC11BC0: case 0xC11BC4: case 0xC11BCE:
    case 0xC11BD8: case 0xC11BE8: case 0xC11BF2: case 0xC24FC8:
    case 0xC24FD8:
        width=2; goto move;
    case 0xC0FCA4: case 0xC24FDC:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC0FCA6: case 0xC1087E: case 0xC17BC4: case 0xC17BD8:
    case 0xC17BFE: case 0xC17C12: case 0xC24FA8: case 0xC24FAE:
    case 0xC24FBC: case 0xC24FD2:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC0FCB0: case 0xC108D6: case 0xC11BF8: case 0xC17C26:
    case 0xC24FE0:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC0FCB2: case 0xC108D8: case 0xC11BFA: case 0xC17C28:
    case 0xC24FE2:
        REG_PC=m68ki_pull_32(); break;
    case 0xC1082C: case 0xC10888: case 0xC17BE4: case 0xC17C1E:
        step_branch(pc,opcode,1); break;
    case 0xC11BCA:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC11BD4: case 0xC11BEE:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC11BDC: case 0xC11BE2:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC17BBA: case 0xC17BF4:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC24FB8:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC24FBA: case 0xC24FC6: case 0xC24FD0:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC24FBE:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
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
static const uint32_t owned_C0FBE0[]={
    0xC0FBE0,0xC0FBE4,0xC0FBEC,0xC0FBF6,0xC0FBFE,0xC0FC00,0xC0FC06,0xC0FC08,
    0xC0FC0A,0xC0FC14,0xC0FC16,0xC0FC18,0xC0FC1E,0xC0FC20,0xC0FC28,0xC0FC2E,
    0xC0FC32,0xC0FC38,0xC0FC3E,0xC0FC40,0xC0FC46,0xC0FC4C,0xC0FC52,0xC0FC54,
    0xC0FC5E,0xC0FC62,0xC0FC66,0xC0FC68,0xC0FC6C,0xC0FC6E,0xC0FC72,0xC0FC74,
    0xC0FC78,0xC0FC7A,0xC0FC7E,0xC0FC80,0xC0FC84,0xC0FC86,0xC0FC8A,0xC0FC8C,
    0xC0FC90,0xC0FC92,0xC0FC96,0xC0FC98,0xC0FC9C,0xC0FC9E,0xC0FCA2,0xC0FCA4,
    0xC0FCA6,0xC0FCAA,0xC0FCB0,0xC0FCB2,
};
int glue_C0FBE0_owns(uint32_t pc) { return owns_pc(owned_C0FBE0,sizeof owned_C0FBE0/sizeof owned_C0FBE0[0],pc); }
int glue_C0FBE0_step(void) { if(!glue_C0FBE0_owns(REG_PC)) return 0; return menu_setup_step(); }
static const uint32_t owned_C17B96[]={
    0xC17B96,0xC17B9A,0xC17BA0,0xC17BA2,0xC17BA6,0xC17BAE,0xC17BB0,0xC17BB6,
    0xC17BB8,0xC17BBA,0xC17BBC,0xC17BBE,0xC17BC0,0xC17BC4,0xC17BC8,0xC17BCA,
    0xC17BCC,0xC17BCE,0xC17BD0,0xC17BD2,0xC17BD4,0xC17BD8,0xC17BDC,0xC17BE4,
    0xC17BE6,0xC17BEE,0xC17BF0,0xC17BF4,0xC17BF6,0xC17BF8,0xC17BFA,0xC17BFE,
    0xC17C02,0xC17C06,0xC17C08,0xC17C0A,0xC17C0C,0xC17C0E,0xC17C12,0xC17C16,
    0xC17C1E,0xC17C20,0xC17C26,0xC17C28,
};
int glue_C17B96_owns(uint32_t pc) { return owns_pc(owned_C17B96,sizeof owned_C17B96/sizeof owned_C17B96[0],pc); }
int glue_C17B96_step(void) { if(!glue_C17B96_owns(REG_PC)) return 0; return menu_setup_step(); }
static const uint32_t owned_C1082C[]={
    0xC1082C,0xC1086E,0xC10876,0xC1087E,0xC10882,0xC10888,0xC108D6,0xC108D8,
};
int glue_C1082C_owns(uint32_t pc) { return owns_pc(owned_C1082C,sizeof owned_C1082C/sizeof owned_C1082C[0],pc); }
int glue_C1082C_step(void) { if(!glue_C1082C_owns(REG_PC)) return 0; return menu_setup_step(); }
static const uint32_t owned_C11BB0[]={
    0xC11BB0,0xC11BB4,0xC11BBA,0xC11BBE,0xC11BC0,0xC11BC4,0xC11BCA,0xC11BCE,
    0xC11BD4,0xC11BD8,0xC11BDC,0xC11BE0,0xC11BE2,0xC11BE6,0xC11BE8,0xC11BEE,
    0xC11BF2,0xC11BF8,0xC11BFA,
};
int glue_C11BB0_owns(uint32_t pc) { return owns_pc(owned_C11BB0,sizeof owned_C11BB0/sizeof owned_C11BB0[0],pc); }
int glue_C11BB0_step(void) { if(!glue_C11BB0_owns(REG_PC)) return 0; return menu_setup_step(); }
static const uint32_t owned_C24FA4[]={
    0xC24FA4,0xC24FA8,0xC24FAE,0xC24FB4,0xC24FB8,0xC24FBA,0xC24FBC,0xC24FBE,
    0xC24FC2,0xC24FC6,0xC24FC8,0xC24FCC,0xC24FD0,0xC24FD2,0xC24FD8,0xC24FDC,
    0xC24FE0,0xC24FE2,
};
int glue_C24FA4_owns(uint32_t pc) { return owns_pc(owned_C24FA4,sizeof owned_C24FA4/sizeof owned_C24FA4[0],pc); }
int glue_C24FA4_step(void) { if(!glue_C24FA4_owns(REG_PC)) return 0; return menu_setup_step(); }
