/* Complete projection readouts family source CPU/bus/event boundaries.
 * Readable behavior lives in projection_readouts.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family projection_readouts. */
#include "glue_projection_readouts_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_projection_readouts.h"

static int projection_readouts_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC2EC70: case 0xC2EC94: case 0xC2ECB2: case 0xC2ECBA:
    case 0xC2ED0C: case 0xC2ED18: case 0xC2ED4C: case 0xC2ED52:
    case 0xC2ED68: case 0xC32A5C: case 0xC32A62: case 0xC32A78:
    case 0xC32A8C: case 0xC32ACC: case 0xC32AD6: case 0xC32AD8:
    case 0xC32B02: case 0xC32B0E: case 0xC32B10: case 0xC32B1E:
    case 0xC32B5A: case 0xC32B68: case 0xC32B78: case 0xC32B86:
    case 0xC32B96: case 0xC32BA4: case 0xC32BB4: case 0xC32BC2:
    case 0xC33F76: case 0xC33F90: case 0xC33FA2: case 0xC33FA8:
    case 0xC33FC6: case 0xC33FCA: case 0xC33FD0: case 0xC33FD6:
    case 0xC33FDE: case 0xC33FE0: case 0xC33FE6: case 0xC33FF2:
    case 0xC34016: case 0xC3401E: case 0xC34024: case 0xC3402A:
    case 0xC34036: case 0xC3405A:
        width=2; goto move;
    case 0xC2EC78: case 0xC2ED24: case 0xC2ED2C: case 0xC2ED34:
    case 0xC32A4C: case 0xC33FAA: case 0xC33FD8: case 0xC34004:
    case 0xC34010: case 0xC34048: case 0xC34054:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC2EC7E: case 0xC2EC82: case 0xC2EC90: case 0xC2EC9C:
    case 0xC2ECA4: case 0xC2ECA8: case 0xC2ED40: case 0xC32A8E:
    case 0xC32A90: case 0xC32A92: case 0xC32AC8: case 0xC32ACA:
    case 0xC32ACE: case 0xC33F70: case 0xC33F7A: case 0xC33F7C:
    case 0xC33F8A: case 0xC33F94: case 0xC33F96:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC2EC80: case 0xC2EC8E: case 0xC2ED42: case 0xC2ED6A:
    case 0xC32BD0: case 0xC33FB0: case 0xC33FB2: case 0xC34064:
        REG_PC=m68ki_pull_32(); break;
    case 0xC2EC84: case 0xC32A46: case 0xC32AD0: case 0xC32B4C:
    case 0xC32B4E: case 0xC32B72: case 0xC32B90: case 0xC32BAE:
        width=4; goto move;
    case 0xC2EC92: case 0xC2EC9A: case 0xC2EC9E: case 0xC2ECA6:
    case 0xC2ED2A: case 0xC2ED32: case 0xC2ED3A: case 0xC2ED46:
    case 0xC2ED4A: case 0xC2ED50: case 0xC2ED56: case 0xC32A94:
    case 0xC32B58: case 0xC33F88: case 0xC3400A: case 0xC3401C:
    case 0xC3404E: case 0xC34060:
        step_branch(pc,opcode,1); break;
    case 0xC2ECAA: case 0xC2ECAE: case 0xC2ECB6: case 0xC2ECBE:
    case 0xC2ECF8: case 0xC2ED12: case 0xC2ED1E:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC2ECAC: case 0xC2ECB0: case 0xC2ECB8: case 0xC2ECC0:
    case 0xC2ECD6: case 0xC2ECE8: case 0xC2ED16: case 0xC2ED22:
    case 0xC2ED5A: case 0xC2ED5E: case 0xC2ED62: case 0xC2ED66:
    case 0xC32AFE: case 0xC32B2E: case 0xC33FC0: case 0xC34034:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC2ECB4: case 0xC2ECBC: case 0xC2ECEE: case 0xC2ECF4:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC2ECC2: case 0xC2ED08:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC2ECC4: case 0xC33FBA: case 0xC33FF0:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC2ECC6: case 0xC2ECD8:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC2ECCA: case 0xC2ECDC:
        renderer_divide(&D(destination),(int16_t)cache_step_read(mode,reg,2)); break;
    case 0xC2ECCC: case 0xC2ECDE: case 0xC32ADE:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC2ECD0: case 0xC2ECE2: case 0xC2ED0A: case 0xC32B26:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC2ECD2: case 0xC2ECE4: case 0xC32B2A: case 0xC33FBC:
    case 0xC33FEC: case 0xC33FF4: case 0xC33FFC: case 0xC34030:
    case 0xC34038: case 0xC34040:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC2ECEA: case 0xC2ECF0: case 0xC32B3C:
        width=2; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC2ECF6: case 0xC2ED58: case 0xC2ED5C: case 0xC2ED60:
    case 0xC2ED64: case 0xC32A6A: case 0xC34018: case 0xC3402E:
    case 0xC3405C:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC2ECFE:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC2ED00:
        mask=m68ki_read_imm_16(); address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); break;
    case 0xC2ED10: case 0xC2ED1C: case 0xC32A68: case 0xC32A72:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC2ED3C: case 0xC32B6E: case 0xC32B8C: case 0xC32BAA:
    case 0xC32BC8:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC2ED44: case 0xC2ED48:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC32A44: case 0xC32A74: case 0xC32A80: case 0xC32B34:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC32A52: case 0xC32A56: case 0xC32A5E: case 0xC32B42:
    case 0xC33F72: case 0xC33F7E: case 0xC33F84: case 0xC33F8C:
    case 0xC33F98: case 0xC33F9E:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC32A64: case 0xC32ADA: case 0xC32B38:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC32A6C: case 0xC32A6E: case 0xC32A7A: case 0xC32A7C:
    case 0xC32A7E: case 0xC32B0C: case 0xC32B22: case 0xC32B24:
    case 0xC32B32: case 0xC32B40: case 0xC33FB4: case 0xC33FCE:
    case 0xC3400C: case 0xC34050:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC32A70: case 0xC32B48:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC32A76:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC32A82: case 0xC32B06: case 0xC33FA4:
        width=4; goto move;
    case 0xC32A84:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='&'; goto bit_value;
    case 0xC32A88: case 0xC32B00:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC32A8A: case 0xC32B1C: case 0xC32B20: case 0xC33FA6:
        step_swap(&D(reg)); break;
    case 0xC32AE2: case 0xC32AF6: case 0xC32B12:
        width=1; goto move;
    case 0xC32AE4:
        step_lsr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC32AE6: case 0xC32BCC:
        step_dbf(pc,&D(reg)); break;
    case 0xC32AEA:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC32AEC: case 0xC32AF4:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC32AEE: case 0xC32AFC: case 0xC33FEA:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC32AF0: case 0xC32B14:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC32B18: case 0xC32B56: case 0xC32B66: case 0xC32B84:
    case 0xC32BA2: case 0xC32BC0: case 0xC33FFA: case 0xC34002:
    case 0xC3403E: case 0xC34046:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC32B36: case 0xC32B50: case 0xC32B76: case 0xC32B94:
    case 0xC32BB2:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC32B52: case 0xC32B5E: case 0xC32B7C: case 0xC32B9A:
    case 0xC32BB8:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC32B6C: case 0xC32B8A: case 0xC32BA8: case 0xC32BC6:
        width=2; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC33FC2:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC34062:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
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
static const uint32_t owned_C2EC90[]={
    0xC2EC70,0xC2EC78,0xC2EC7E,0xC2EC80,0xC2EC82,0xC2EC84,0xC2EC8E,0xC2EC90,
    0xC2EC92,0xC2ECAA,0xC2ECAC,0xC2ECAE,0xC2ECB0,0xC2ECB2,0xC2ECB4,0xC2ECB6,
    0xC2ECB8,0xC2ECBA,0xC2ECBC,0xC2ECBE,0xC2ECC0,0xC2ECC2,0xC2ECC4,0xC2ECC6,
    0xC2ECCA,0xC2ECCC,0xC2ECD0,0xC2ECD2,0xC2ECD6,0xC2ECD8,0xC2ECDC,0xC2ECDE,
    0xC2ECE2,0xC2ECE4,0xC2ECE8,0xC2ECEA,0xC2ECEE,0xC2ECF0,0xC2ECF4,0xC2ECF6,
    0xC2ECF8,0xC2ECFE,0xC2ED00,0xC2ED08,0xC2ED0A,0xC2ED0C,0xC2ED10,0xC2ED12,
    0xC2ED16,0xC2ED18,0xC2ED1C,0xC2ED1E,0xC2ED22,0xC2ED24,0xC2ED2A,0xC2ED2C,
    0xC2ED32,0xC2ED34,0xC2ED3A,0xC2ED3C,0xC2ED40,0xC2ED42,0xC2ED44,0xC2ED46,
    0xC2ED48,0xC2ED4A,0xC2ED4C,0xC2ED50,0xC2ED52,0xC2ED56,0xC2ED58,0xC2ED5A,
    0xC2ED5C,0xC2ED5E,0xC2ED60,0xC2ED62,0xC2ED64,0xC2ED66,0xC2ED68,0xC2ED6A,
};
int glue_C2EC90_owns(uint32_t pc) { return owns_pc(owned_C2EC90,sizeof owned_C2EC90/sizeof owned_C2EC90[0],pc); }
int glue_C2EC90_complete_step(void) { if(!glue_C2EC90_owns(REG_PC)) return 0; return projection_readouts_step(); }
static const uint32_t owned_C2EC94[]={
    0xC2EC70,0xC2EC78,0xC2EC7E,0xC2EC80,0xC2EC82,0xC2EC84,0xC2EC8E,0xC2EC94,
    0xC2EC9A,0xC2ECAA,0xC2ECAC,0xC2ECAE,0xC2ECB0,0xC2ECB2,0xC2ECB4,0xC2ECB6,
    0xC2ECB8,0xC2ECBA,0xC2ECBC,0xC2ECBE,0xC2ECC0,0xC2ECC2,0xC2ECC4,0xC2ECC6,
    0xC2ECCA,0xC2ECCC,0xC2ECD0,0xC2ECD2,0xC2ECD6,0xC2ECD8,0xC2ECDC,0xC2ECDE,
    0xC2ECE2,0xC2ECE4,0xC2ECE8,0xC2ECEA,0xC2ECEE,0xC2ECF0,0xC2ECF4,0xC2ECF6,
    0xC2ECF8,0xC2ECFE,0xC2ED00,0xC2ED08,0xC2ED0A,0xC2ED0C,0xC2ED10,0xC2ED12,
    0xC2ED16,0xC2ED18,0xC2ED1C,0xC2ED1E,0xC2ED22,0xC2ED24,0xC2ED2A,0xC2ED2C,
    0xC2ED32,0xC2ED34,0xC2ED3A,0xC2ED3C,0xC2ED40,0xC2ED42,0xC2ED44,0xC2ED46,
    0xC2ED48,0xC2ED4A,0xC2ED4C,0xC2ED50,0xC2ED52,0xC2ED56,0xC2ED58,0xC2ED5A,
    0xC2ED5C,0xC2ED5E,0xC2ED60,0xC2ED62,0xC2ED64,0xC2ED66,0xC2ED68,0xC2ED6A,
};
int glue_C2EC94_owns(uint32_t pc) { return owns_pc(owned_C2EC94,sizeof owned_C2EC94/sizeof owned_C2EC94[0],pc); }
int glue_C2EC94_complete_step(void) { if(!glue_C2EC94_owns(REG_PC)) return 0; return projection_readouts_step(); }
static const uint32_t owned_C2EC9C[]={
    0xC2EC70,0xC2EC78,0xC2EC7E,0xC2EC80,0xC2EC82,0xC2EC84,0xC2EC8E,0xC2EC9C,
    0xC2EC9E,0xC2ECAA,0xC2ECAC,0xC2ECAE,0xC2ECB0,0xC2ECB2,0xC2ECB4,0xC2ECB6,
    0xC2ECB8,0xC2ECBA,0xC2ECBC,0xC2ECBE,0xC2ECC0,0xC2ECC2,0xC2ECC4,0xC2ECC6,
    0xC2ECCA,0xC2ECCC,0xC2ECD0,0xC2ECD2,0xC2ECD6,0xC2ECD8,0xC2ECDC,0xC2ECDE,
    0xC2ECE2,0xC2ECE4,0xC2ECE8,0xC2ECEA,0xC2ECEE,0xC2ECF0,0xC2ECF4,0xC2ECF6,
    0xC2ECF8,0xC2ECFE,0xC2ED00,0xC2ED08,0xC2ED0A,0xC2ED0C,0xC2ED10,0xC2ED12,
    0xC2ED16,0xC2ED18,0xC2ED1C,0xC2ED1E,0xC2ED22,0xC2ED24,0xC2ED2A,0xC2ED2C,
    0xC2ED32,0xC2ED34,0xC2ED3A,0xC2ED3C,0xC2ED40,0xC2ED42,0xC2ED44,0xC2ED46,
    0xC2ED48,0xC2ED4A,0xC2ED4C,0xC2ED50,0xC2ED52,0xC2ED56,0xC2ED58,0xC2ED5A,
    0xC2ED5C,0xC2ED5E,0xC2ED60,0xC2ED62,0xC2ED64,0xC2ED66,0xC2ED68,0xC2ED6A,
};
int glue_C2EC9C_owns(uint32_t pc) { return owns_pc(owned_C2EC9C,sizeof owned_C2EC9C/sizeof owned_C2EC9C[0],pc); }
int glue_C2EC9C_complete_step(void) { if(!glue_C2EC9C_owns(REG_PC)) return 0; return projection_readouts_step(); }
static const uint32_t owned_C2ECA4[]={
    0xC2EC70,0xC2EC78,0xC2EC7E,0xC2EC80,0xC2EC82,0xC2EC84,0xC2EC8E,0xC2ECA4,
    0xC2ECA6,0xC2ECAA,0xC2ECAC,0xC2ECAE,0xC2ECB0,0xC2ECB2,0xC2ECB4,0xC2ECB6,
    0xC2ECB8,0xC2ECBA,0xC2ECBC,0xC2ECBE,0xC2ECC0,0xC2ECC2,0xC2ECC4,0xC2ECC6,
    0xC2ECCA,0xC2ECCC,0xC2ECD0,0xC2ECD2,0xC2ECD6,0xC2ECD8,0xC2ECDC,0xC2ECDE,
    0xC2ECE2,0xC2ECE4,0xC2ECE8,0xC2ECEA,0xC2ECEE,0xC2ECF0,0xC2ECF4,0xC2ECF6,
    0xC2ECF8,0xC2ECFE,0xC2ED00,0xC2ED08,0xC2ED0A,0xC2ED0C,0xC2ED10,0xC2ED12,
    0xC2ED16,0xC2ED18,0xC2ED1C,0xC2ED1E,0xC2ED22,0xC2ED24,0xC2ED2A,0xC2ED2C,
    0xC2ED32,0xC2ED34,0xC2ED3A,0xC2ED3C,0xC2ED40,0xC2ED42,0xC2ED44,0xC2ED46,
    0xC2ED48,0xC2ED4A,0xC2ED4C,0xC2ED50,0xC2ED52,0xC2ED56,0xC2ED58,0xC2ED5A,
    0xC2ED5C,0xC2ED5E,0xC2ED60,0xC2ED62,0xC2ED64,0xC2ED66,0xC2ED68,0xC2ED6A,
};
int glue_C2ECA4_owns(uint32_t pc) { return owns_pc(owned_C2ECA4,sizeof owned_C2ECA4/sizeof owned_C2ECA4[0],pc); }
int glue_C2ECA4_complete_step(void) { if(!glue_C2ECA4_owns(REG_PC)) return 0; return projection_readouts_step(); }
static const uint32_t owned_C2ECA8[]={
    0xC2EC70,0xC2EC78,0xC2EC7E,0xC2EC80,0xC2EC82,0xC2EC84,0xC2EC8E,0xC2ECA8,
    0xC2ECAA,0xC2ECAC,0xC2ECAE,0xC2ECB0,0xC2ECB2,0xC2ECB4,0xC2ECB6,0xC2ECB8,
    0xC2ECBA,0xC2ECBC,0xC2ECBE,0xC2ECC0,0xC2ECC2,0xC2ECC4,0xC2ECC6,0xC2ECCA,
    0xC2ECCC,0xC2ECD0,0xC2ECD2,0xC2ECD6,0xC2ECD8,0xC2ECDC,0xC2ECDE,0xC2ECE2,
    0xC2ECE4,0xC2ECE8,0xC2ECEA,0xC2ECEE,0xC2ECF0,0xC2ECF4,0xC2ECF6,0xC2ECF8,
    0xC2ECFE,0xC2ED00,0xC2ED08,0xC2ED0A,0xC2ED0C,0xC2ED10,0xC2ED12,0xC2ED16,
    0xC2ED18,0xC2ED1C,0xC2ED1E,0xC2ED22,0xC2ED24,0xC2ED2A,0xC2ED2C,0xC2ED32,
    0xC2ED34,0xC2ED3A,0xC2ED3C,0xC2ED40,0xC2ED42,0xC2ED44,0xC2ED46,0xC2ED48,
    0xC2ED4A,0xC2ED4C,0xC2ED50,0xC2ED52,0xC2ED56,0xC2ED58,0xC2ED5A,0xC2ED5C,
    0xC2ED5E,0xC2ED60,0xC2ED62,0xC2ED64,0xC2ED66,0xC2ED68,0xC2ED6A,
};
int glue_C2ECA8_owns(uint32_t pc) { return owns_pc(owned_C2ECA8,sizeof owned_C2ECA8/sizeof owned_C2ECA8[0],pc); }
int glue_C2ECA8_step(void) { if(!glue_C2ECA8_owns(REG_PC)) return 0; return projection_readouts_step(); }
static const uint32_t owned_C32A44[]={
    0xC32A44,0xC32A46,0xC32A4C,0xC32A52,0xC32A56,0xC32A5C,0xC32A5E,0xC32A62,
    0xC32A64,0xC32A68,0xC32A6A,0xC32A6C,0xC32A6E,0xC32A70,0xC32A72,0xC32A74,
    0xC32A76,0xC32A78,0xC32A7A,0xC32A7C,0xC32A7E,0xC32A80,0xC32A82,0xC32A84,
    0xC32A88,0xC32A8A,0xC32A8C,0xC32A8E,0xC32A90,0xC32A92,0xC32A94,0xC32AD0,
    0xC32AD6,0xC32AD8,0xC32ADA,0xC32ADE,0xC32AE2,0xC32AE4,0xC32AE6,0xC32AEA,
    0xC32AEC,0xC32AEE,0xC32AF0,0xC32AF4,0xC32AF6,0xC32AFC,0xC32AFE,0xC32B00,
    0xC32B02,0xC32B06,0xC32B0C,0xC32B0E,0xC32B10,0xC32B12,0xC32B14,0xC32B18,
    0xC32B1C,0xC32B1E,0xC32B20,0xC32B22,0xC32B24,0xC32B26,0xC32B2A,0xC32B2E,
    0xC32B32,0xC32B34,0xC32B36,0xC32B38,0xC32B3C,0xC32B40,0xC32B42,0xC32B48,
    0xC32B4C,0xC32B4E,0xC32B50,0xC32B52,0xC32B56,0xC32B58,0xC32B5A,0xC32B5E,
    0xC32B66,0xC32B68,0xC32B6C,0xC32B6E,0xC32B72,0xC32B76,0xC32B78,0xC32B7C,
    0xC32B84,0xC32B86,0xC32B8A,0xC32B8C,0xC32B90,0xC32B94,0xC32B96,0xC32B9A,
    0xC32BA2,0xC32BA4,0xC32BA8,0xC32BAA,0xC32BAE,0xC32BB2,0xC32BB4,0xC32BB8,
    0xC32BC0,0xC32BC2,0xC32BC6,0xC32BC8,0xC32BCC,0xC32BD0,
};
int glue_C32A44_owns(uint32_t pc) { return owns_pc(owned_C32A44,sizeof owned_C32A44/sizeof owned_C32A44[0],pc); }
int glue_C32A44_step(void) { if(!glue_C32A44_owns(REG_PC)) return 0; return projection_readouts_step(); }
static const uint32_t owned_C32AC8[]={
    0xC32AC8,0xC32ACA,0xC32ACC,0xC32ACE,0xC32AD0,0xC32AD6,0xC32AD8,0xC32ADA,
    0xC32ADE,0xC32AE2,0xC32AE4,0xC32AE6,0xC32AEA,0xC32AEC,0xC32AEE,0xC32AF0,
    0xC32AF4,0xC32AF6,0xC32AFC,0xC32AFE,0xC32B00,0xC32B02,0xC32B06,0xC32B0C,
    0xC32B0E,0xC32B10,0xC32B12,0xC32B14,0xC32B18,0xC32B1C,0xC32B1E,0xC32B20,
    0xC32B22,0xC32B24,0xC32B26,0xC32B2A,0xC32B2E,0xC32B32,0xC32B34,0xC32B36,
    0xC32B38,0xC32B3C,0xC32B40,0xC32B42,0xC32B48,0xC32B4C,0xC32B4E,0xC32B50,
    0xC32B52,0xC32B56,0xC32B58,0xC32B5A,0xC32B5E,0xC32B66,0xC32B68,0xC32B6C,
    0xC32B6E,0xC32B72,0xC32B76,0xC32B78,0xC32B7C,0xC32B84,0xC32B86,0xC32B8A,
    0xC32B8C,0xC32B90,0xC32B94,0xC32B96,0xC32B9A,0xC32BA2,0xC32BA4,0xC32BA8,
    0xC32BAA,0xC32BAE,0xC32BB2,0xC32BB4,0xC32BB8,0xC32BC0,0xC32BC2,0xC32BC6,
    0xC32BC8,0xC32BCC,0xC32BD0,
};
int glue_C32AC8_owns(uint32_t pc) { return owns_pc(owned_C32AC8,sizeof owned_C32AC8/sizeof owned_C32AC8[0],pc); }
int glue_C32AC8_step(void) { if(!glue_C32AC8_owns(REG_PC)) return 0; return projection_readouts_step(); }
static const uint32_t owned_C33F70[]={
    0xC33F70,0xC33F72,0xC33F76,0xC33F7A,0xC33F7C,0xC33F7E,0xC33F84,0xC33F88,
    0xC33FA2,0xC33FA4,0xC33FA6,0xC33FA8,0xC33FAA,0xC33FB0,
};
int glue_C33F70_owns(uint32_t pc) { return owns_pc(owned_C33F70,sizeof owned_C33F70/sizeof owned_C33F70[0],pc); }
int glue_C33F70_step(void) { if(!glue_C33F70_owns(REG_PC)) return 0; return projection_readouts_step(); }
static const uint32_t owned_C33F8A[]={
    0xC33F8A,0xC33F8C,0xC33F90,0xC33F94,0xC33F96,0xC33F98,0xC33F9E,0xC33FA2,
    0xC33FA4,0xC33FA6,0xC33FA8,0xC33FAA,0xC33FB0,
};
int glue_C33F8A_owns(uint32_t pc) { return owns_pc(owned_C33F8A,sizeof owned_C33F8A/sizeof owned_C33F8A[0],pc); }
int glue_C33F8A_step(void) { if(!glue_C33F8A_owns(REG_PC)) return 0; return projection_readouts_step(); }
static const uint32_t owned_C33FB4[]={
    0xC33FB2,0xC33FB4,0xC33FBA,0xC33FBC,0xC33FC0,0xC33FC2,0xC33FC6,0xC33FCA,
    0xC33FCE,0xC33FD0,0xC33FD6,0xC33FD8,0xC33FDE,0xC33FE0,0xC33FE6,0xC33FEA,
    0xC33FEC,0xC33FF0,0xC33FF2,0xC33FF4,0xC33FFA,0xC33FFC,0xC34002,0xC34004,
    0xC3400A,0xC3400C,0xC34010,0xC34016,0xC34018,0xC3401C,0xC3401E,0xC34024,
    0xC3402A,0xC3402E,0xC34030,0xC34034,0xC34036,0xC34038,0xC3403E,0xC34040,
    0xC34046,0xC34048,0xC3404E,0xC34050,0xC34054,0xC3405A,0xC3405C,0xC34060,
    0xC34062,0xC34064,
};
int glue_C33FB4_owns(uint32_t pc) { return owns_pc(owned_C33FB4,sizeof owned_C33FB4/sizeof owned_C33FB4[0],pc); }
int glue_C33FB4_step(void) { if(!glue_C33FB4_owns(REG_PC)) return 0; return projection_readouts_step(); }
