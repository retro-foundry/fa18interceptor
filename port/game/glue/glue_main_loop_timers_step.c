/* Complete main loop timers family source CPU/bus/event boundaries.
 * Readable behavior lives in main_loop_timers.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family main_loop_timers. */
#include "glue_main_loop_timers_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_main_loop_timers.h"

static int main_loop_timers_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC2527A: case 0xC25480: case 0xC254E6:
        REG_PC=m68ki_pull_32(); break;
    case 0xC2527C: case 0xC25286: case 0xC253BE: case 0xC253D2:
    case 0xC253DC: case 0xC253E6: case 0xC2546A:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC25282: case 0xC252A8: case 0xC252AA: case 0xC252B8:
    case 0xC252BA: case 0xC252C8: case 0xC252CA: case 0xC252D8:
    case 0xC252DA: case 0xC252E8: case 0xC252EA: case 0xC25322:
    case 0xC2545C: case 0xC254B8: case 0xC254C6: case 0xC254CC:
        width=2; goto move;
    case 0xC25284: case 0xC2531E: case 0xC2537A: case 0xC25394:
    case 0xC2540C: case 0xC2542C: case 0xC25490:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC2528C: case 0xC2530A:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC2528E: case 0xC25304:
        mask=m68ki_read_imm_16(); address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); break;
    case 0xC25292: case 0xC25372:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC25296: case 0xC2532C: case 0xC253B0: case 0xC253C6:
    case 0xC253FC:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC25298: case 0xC2529A:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC2529C: case 0xC252A0: case 0xC25406:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC2529E:
        value=D(destination); D(destination)=D(reg); D(reg)=value; break;
    case 0xC252A2: case 0xC25312: case 0xC25420:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC252AC: case 0xC252B0: case 0xC252BC: case 0xC252C0:
    case 0xC252CC: case 0xC252D0: case 0xC252DC: case 0xC252E0:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC252B4: case 0xC252C4: case 0xC252D4: case 0xC252E4:
    case 0xC25364: case 0xC25366: case 0xC2539E: case 0xC2544C:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,4); mode=0; reg=destination; operation='+'; goto arithmetic;
    case 0xC252B6: case 0xC252C6: case 0xC252D6: case 0xC252E6:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC252EC: case 0xC252EE:
        renderer_negate(&D(reg),2); break;
    case 0xC252F0: case 0xC252F2: case 0xC252F6: case 0xC252F8:
    case 0xC252FC: case 0xC25300: case 0xC2536C: case 0xC25476:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,2); mode=0; reg=destination; operation='+'; goto arithmetic;
    case 0xC2530E: case 0xC25342: case 0xC25408: case 0xC2547E:
    case 0xC254A6:
        step_branch(pc,opcode,1); break;
    case 0xC25318: case 0xC25352: case 0xC25382: case 0xC253A4:
    case 0xC253B2: case 0xC25416: case 0xC25426: case 0xC2543A:
    case 0xC2544E: case 0xC25492: case 0xC254A0: case 0xC254A8:
    case 0xC254D2: case 0xC254DC:
        width=4; goto move;
    case 0xC25328: case 0xC2532E: case 0xC25462:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC25336: case 0xC2533C:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC25346: case 0xC25358: case 0xC25388: case 0xC2542E:
    case 0xC25440:
        width=4; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC2534C: case 0xC25434:
        renderer_negate(&D(reg),4); break;
    case 0xC2534E: case 0xC25436:
        timer_multiply_unsigned(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC2535E: case 0xC25446:
        renderer_divide(&D(destination),(int16_t)cache_step_read(mode,reg,2)); break;
    case 0xC25362: case 0xC2544A:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC2537C:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC2538E: case 0xC25454: case 0xC25498:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC25396:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC253AA:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC253B8: case 0xC253CC: case 0xC2540E:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC253C4: case 0xC253F6: case 0xC2540A:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC253C8:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='&'; goto bit_value;
    case 0xC253CE: case 0xC2547C: case 0xC254B6:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC253D0:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC253D8: case 0xC253E2: case 0xC253EC:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC253F0: case 0xC25410: case 0xC2546E:
        width=1; goto move;
    case 0xC253FE:
        width=1; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC25400:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC25404: case 0xC2549E: case 0xC254C4:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC2545A:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC25466:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC25468: case 0xC254AE:
        step_divide_unsigned(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC25474:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC25478: case 0xC254B0: case 0xC254BE:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC2548A:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
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
static const uint32_t owned_C2527C[]={
    0xC2527A,0xC2527C,0xC25282,0xC25284,0xC25286,0xC2528C,0xC2528E,0xC25292,
    0xC25296,0xC25298,0xC2529A,0xC2529C,0xC2529E,0xC252A0,0xC252A2,0xC252A8,
    0xC252AA,0xC252AC,0xC252B0,0xC252B4,0xC252B6,0xC252B8,0xC252BA,0xC252BC,
    0xC252C0,0xC252C4,0xC252C6,0xC252C8,0xC252CA,0xC252CC,0xC252D0,0xC252D4,
    0xC252D6,0xC252D8,0xC252DA,0xC252DC,0xC252E0,0xC252E4,0xC252E6,0xC252E8,
    0xC252EA,0xC252EC,0xC252EE,0xC252F0,0xC252F2,0xC252F6,0xC252F8,0xC252FC,
    0xC25300,0xC25304,0xC2530A,0xC2530E,
};
int glue_C2527C_owns(uint32_t pc) { return owns_pc(owned_C2527C,sizeof owned_C2527C/sizeof owned_C2527C[0],pc); }
int glue_C2527C_step(void) { if(!glue_C2527C_owns(REG_PC)) return 0; return main_loop_timers_step(); }
static const uint32_t owned_C25312[]={
    0xC25312,0xC25318,0xC2531E,0xC25322,0xC25328,0xC2532C,0xC2532E,0xC25336,
    0xC2533C,0xC25342,0xC25346,0xC2534C,0xC2534E,0xC25352,0xC25358,0xC2535E,
    0xC25362,0xC25364,0xC25366,0xC2536C,0xC25372,0xC2537A,0xC2537C,0xC25382,
    0xC25388,0xC2538E,0xC25394,0xC25396,0xC2539E,0xC253A4,0xC253AA,0xC253B0,
    0xC253B2,0xC253B8,0xC253BE,0xC253C4,0xC253C6,0xC253C8,0xC253CC,0xC253CE,
    0xC253D0,0xC253D2,0xC253D8,0xC253DC,0xC253E2,0xC253E6,0xC253EC,0xC253F0,
    0xC253F6,0xC253FC,0xC253FE,0xC25400,0xC25404,0xC25406,0xC25408,0xC2540A,
    0xC2540C,0xC2540E,0xC25410,0xC25416,0xC25420,0xC25426,0xC2542C,0xC2542E,
    0xC25434,0xC25436,0xC2543A,0xC25440,0xC25446,0xC2544A,0xC2544C,0xC2544E,
    0xC25454,0xC2545A,0xC2545C,0xC25462,0xC25466,0xC25468,0xC2546A,0xC2546E,
    0xC25474,0xC25476,0xC25478,0xC2547C,0xC2547E,0xC25480,
};
int glue_C25312_owns(uint32_t pc) { return owns_pc(owned_C25312,sizeof owned_C25312/sizeof owned_C25312[0],pc); }
int glue_C25312_step(void) { if(!glue_C25312_owns(REG_PC)) return 0; return main_loop_timers_step(); }
static const uint32_t owned_C2548A[]={
    0xC2548A,0xC25490,0xC25492,0xC25498,0xC2549E,0xC254A0,0xC254A6,0xC254A8,
    0xC254AE,0xC254B0,0xC254B6,0xC254B8,0xC254BE,0xC254C4,0xC254C6,0xC254CC,
    0xC254D2,0xC254DC,0xC254E6,
};
int glue_C2548A_owns(uint32_t pc) { return owns_pc(owned_C2548A,sizeof owned_C2548A/sizeof owned_C2548A[0],pc); }
int glue_C2548A_step(void) { if(!glue_C2548A_owns(REG_PC)) return 0; return main_loop_timers_step(); }
