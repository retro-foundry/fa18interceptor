/* Complete record steering family source CPU/bus/event boundaries.
 * Readable behavior lives in record_steering.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family record_steering. */
#include "glue_render_leaf_helpers_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_record_steering.h"

static int record_steering_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC2CA26: case 0xC2CA40: case 0xC2CA4C: case 0xC2CAA2:
    case 0xC2CAAC: case 0xC2CBA2: case 0xC2CBB6:
        width=1; goto move;
    case 0xC2CA2A: case 0xC2CAA6: case 0xC2CBAA: case 0xC2CBB0:
        width=1; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC2CA2E:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC2CA32:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC2CA34: case 0xC2CA92: case 0xC2CB88: case 0xC2CBA6:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC2CA36: case 0xC2CA78: case 0xC2CA94:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC2CA38: case 0xC2CA88: case 0xC2CA96: case 0xC2CB8A:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC2CA3A: case 0xC2CA46: case 0xC2CB8C: case 0xC2CB96:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC2CA3E: case 0xC2CA5E: case 0xC2CA90: case 0xC2CB90:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC2CA44: case 0xC2CA64: case 0xC2CA70: case 0xC2CA8A:
    case 0xC2CA9A: case 0xC2CA9E: case 0xC2CB84: case 0xC2CB94:
    case 0xC2CB9E: case 0xC2CBAE: case 0xC2CBBC:
        step_branch(pc,opcode,1); break;
    case 0xC2CA4A: case 0xC2CB9A: case 0xC2CBA8:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC2CA50: case 0xC2CA7A:
        width=2; goto move;
    case 0xC2CA54: case 0xC2CA5A: case 0xC2CA66: case 0xC2CA7E:
    case 0xC2CA84: case 0xC2CA8C:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC2CA58: case 0xC2CA6A: case 0xC2CA82:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC2CA60: case 0xC2CA6C:
        width=1; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC2CA72:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC2CA98: case 0xC2CA9C: case 0xC2CAA0: case 0xC2CB82:
    case 0xC2CB86: case 0xC2CB92: case 0xC2CB9C: case 0xC2CBA0:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC2CAAA: case 0xC2CBB4:
        width=1; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC2CAB0: case 0xC2CBBA:
        REG_PC=m68ki_pull_32(); break;
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
static const uint32_t owned_C2CA26[]={
    0xC2CA26,0xC2CA2A,0xC2CA2E,0xC2CA32,0xC2CA34,0xC2CA36,0xC2CA38,0xC2CA3A,
    0xC2CA3E,0xC2CA40,0xC2CA44,0xC2CA46,0xC2CA4A,0xC2CA4C,0xC2CA50,0xC2CA54,
    0xC2CA58,0xC2CA5A,0xC2CA5E,0xC2CA60,0xC2CA64,0xC2CA66,0xC2CA6A,0xC2CA6C,
    0xC2CA70,0xC2CA72,0xC2CA78,0xC2CA7A,0xC2CA7E,0xC2CA82,0xC2CA84,0xC2CA88,
    0xC2CA8A,0xC2CA8C,0xC2CA90,0xC2CA92,0xC2CA94,0xC2CA96,0xC2CA98,0xC2CA9A,
    0xC2CA9C,0xC2CA9E,0xC2CAA0,0xC2CAA2,0xC2CAA6,0xC2CAAA,0xC2CAAC,0xC2CAB0,
};
int glue_C2CA26_owns(uint32_t pc) { return owns_pc(owned_C2CA26,sizeof owned_C2CA26/sizeof owned_C2CA26[0],pc); }
int glue_C2CA26_complete_step(void) { if(!glue_C2CA26_owns(REG_PC)) return 0; return record_steering_step(); }
static const uint32_t owned_C2CA92[]={
    0xC2CA92,0xC2CA94,0xC2CA96,0xC2CA98,0xC2CA9A,0xC2CA9C,0xC2CA9E,0xC2CAA0,
    0xC2CAA2,0xC2CAA6,0xC2CAAA,0xC2CAAC,0xC2CAB0,
};
int glue_C2CA92_owns(uint32_t pc) { return owns_pc(owned_C2CA92,sizeof owned_C2CA92/sizeof owned_C2CA92[0],pc); }
int glue_C2CA92_complete_step(void) { if(!glue_C2CA92_owns(REG_PC)) return 0; return record_steering_step(); }
static const uint32_t owned_C2CAA0[]={
    0xC2CAA0,0xC2CAA2,0xC2CAA6,0xC2CAAA,0xC2CAAC,0xC2CAB0,
};
int glue_C2CAA0_owns(uint32_t pc) { return owns_pc(owned_C2CAA0,sizeof owned_C2CAA0/sizeof owned_C2CAA0[0],pc); }
int glue_C2CAA0_complete_step(void) { if(!glue_C2CAA0_owns(REG_PC)) return 0; return record_steering_step(); }
static const uint32_t owned_C2CB86[]={
    0xC2CB86,0xC2CB88,0xC2CB8A,0xC2CB8C,0xC2CB90,0xC2CB92,0xC2CB94,0xC2CB96,
    0xC2CB9A,0xC2CB9C,0xC2CB9E,0xC2CBA0,0xC2CBA2,0xC2CBA6,0xC2CBA8,0xC2CBAA,
    0xC2CBAE,0xC2CBB0,0xC2CBB4,0xC2CBB6,0xC2CBBA,
};
int glue_C2CB86_owns(uint32_t pc) { return owns_pc(owned_C2CB86,sizeof owned_C2CB86/sizeof owned_C2CB86[0],pc); }
int glue_C2CB86_complete_step(void) { if(!glue_C2CB86_owns(REG_PC)) return 0; return record_steering_step(); }
static const uint32_t owned_C2CB82[]={
    0xC2CB82,0xC2CB84,0xC2CB88,0xC2CB8A,0xC2CB8C,0xC2CB90,0xC2CB92,0xC2CB94,
    0xC2CB96,0xC2CB9A,0xC2CB9C,0xC2CB9E,0xC2CBA0,0xC2CBA2,0xC2CBA6,0xC2CBA8,
    0xC2CBAA,0xC2CBAE,0xC2CBB0,0xC2CBB4,0xC2CBB6,0xC2CBBA,
};
int glue_C2CB82_owns(uint32_t pc) { return owns_pc(owned_C2CB82,sizeof owned_C2CB82/sizeof owned_C2CB82[0],pc); }
int glue_C2CB82_complete_step(void) { if(!glue_C2CB82_owns(REG_PC)) return 0; return record_steering_step(); }
static const uint32_t owned_C2CBBC[]={
    0xC2CB86,0xC2CB88,0xC2CB8A,0xC2CB8C,0xC2CB90,0xC2CB92,0xC2CB94,0xC2CB96,
    0xC2CB9A,0xC2CB9C,0xC2CB9E,0xC2CBA0,0xC2CBA2,0xC2CBA6,0xC2CBA8,0xC2CBAA,
    0xC2CBAE,0xC2CBB0,0xC2CBB4,0xC2CBB6,0xC2CBBA,0xC2CBBC,
};
int glue_C2CBBC_owns(uint32_t pc) { return owns_pc(owned_C2CBBC,sizeof owned_C2CBBC/sizeof owned_C2CBBC[0],pc); }
int glue_C2CBBC_complete_step(void) { if(!glue_C2CBBC_owns(REG_PC)) return 0; return record_steering_step(); }
