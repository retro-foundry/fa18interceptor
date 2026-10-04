/* Complete display record selection family source CPU/bus/event boundaries.
 * Readable behavior lives in display_record_selection.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family display_record_selection. */
#include "glue_render_leaf_helpers_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_display_record_selection.h"

static int display_record_selection_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0D74A: case 0xC0D752: case 0xC0D758: case 0xC0D75E:
    case 0xC0D762: case 0xC0D768: case 0xC0D77E: case 0xC0D7CC:
    case 0xC0D7E0: case 0xC0D872: case 0xC0D89C: case 0xC0D8BE:
    case 0xC0D8E4: case 0xC0D90C: case 0xC0D946: case 0xC0D95A:
    case 0xC0D980: case 0xC0D9A8: case 0xC0D9C8: case 0xC0D9EA:
    case 0xC0DA24: case 0xC0DA38: case 0xC0DAA4:
        A(destination)=(mode==7 && reg==0)?(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16():cache_step_address(mode,reg,4); break;
    case 0xC0D750: case 0xC0D7FA: case 0xC0D80C: case 0xC0D81E:
    case 0xC0D822: case 0xC0D83A: case 0xC0D84C: case 0xC0D850:
    case 0xC0D86A: case 0xC0D86E: case 0xC0D8AA: case 0xC0D8BA:
    case 0xC0D8F4: case 0xC0D908: case 0xC0D932: case 0xC0D956:
    case 0xC0D990: case 0xC0D9A4: case 0xC0D9D6: case 0xC0D9E6:
    case 0xC0DA10: case 0xC0DA34: case 0xC0DA60: case 0xC0DA6E:
        step_branch(pc,opcode,1); break;
    case 0xC0D75A:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0D76C: case 0xC0D770: case 0xC0D77C: case 0xC0D780:
    case 0xC0D782: case 0xC0D784: case 0xC0D796: case 0xC0D798:
    case 0xC0D79A: case 0xC0D79C: case 0xC0D7AE: case 0xC0D7C0:
    case 0xC0D7C2: case 0xC0D7F2: case 0xC0D7F6: case 0xC0D804:
    case 0xC0D808: case 0xC0D816: case 0xC0D81A: case 0xC0D832:
    case 0xC0D836: case 0xC0D844: case 0xC0D848: case 0xC0D862:
    case 0xC0D866: case 0xC0D878: case 0xC0D884: case 0xC0D8C4:
    case 0xC0D8C8: case 0xC0D8EA: case 0xC0D912: case 0xC0D916:
    case 0xC0D94C: case 0xC0D960: case 0xC0D964: case 0xC0D986:
    case 0xC0D9AE: case 0xC0D9F0: case 0xC0D9F4: case 0xC0DA2A:
    case 0xC0DA3E: case 0xC0DA5A: case 0xC0DA62: case 0xC0DA70:
    case 0xC0DA78: case 0xC0DAAA: case 0xC0DAAE: case 0xC0DAB4:
    case 0xC0DAB8: case 0xC0DABE: case 0xC0DAC0: case 0xC0DACA:
    case 0xC0DACC: case 0xC0DAD4: case 0xC0DADC: case 0xC0DAE0:
    case 0xC0DAE8:
        width=2; goto move;
    case 0xC0D772:
        width=4; goto move;
    case 0xC0D778:
        step_swap(&D(reg)); break;
    case 0xC0D77A:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC0D786: case 0xC0D788: case 0xC0D78A: case 0xC0D79E:
    case 0xC0D7A0: case 0xC0D7A2: case 0xC0D7B0: case 0xC0D7B2:
    case 0xC0D7B4:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC0D78C: case 0xC0D78E: case 0xC0D7A4: case 0xC0D7A6:
    case 0xC0D7B6: case 0xC0D7B8:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC0D790: case 0xC0D7A8: case 0xC0D7BA:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC0D792: case 0xC0D7AA: case 0xC0D7BC:
        step_branch(pc,opcode,COND_CC()); break;
    case 0xC0D794: case 0xC0D7AC: case 0xC0D7BE: case 0xC0D8EE:
    case 0xC0D950: case 0xC0D98A: case 0xC0DA2E:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0D7C4:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC0D7C8:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0D7CA: case 0xC0DA6C:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC0D7D2: case 0xC0D7D4: case 0xC0D7D6: case 0xC0D7D8:
    case 0xC0DA80: case 0xC0DAD0:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC0D7DA:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0D7E6: case 0xC0D7EC: case 0xC0D7FE: case 0xC0D810:
    case 0xC0D826: case 0xC0D82C: case 0xC0D83E: case 0xC0D854:
    case 0xC0D85C:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC0D7EA: case 0xC0D7F0: case 0xC0D802: case 0xC0D814:
    case 0xC0D82A: case 0xC0D830: case 0xC0D842: case 0xC0D858:
    case 0xC0D860:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC0D87C: case 0xC0DA52:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC0D882: case 0xC0D88E: case 0xC0D8D2: case 0xC0D920:
    case 0xC0D96E: case 0xC0D9FE: case 0xC0DA58:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0D88A: case 0xC0D8CE: case 0xC0D91C: case 0xC0D96A:
    case 0xC0D9FA:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC0D890: case 0xC0D894: case 0xC0D898: case 0xC0D8A2:
    case 0xC0D8A6: case 0xC0D8AE: case 0xC0D8B2: case 0xC0D8B6:
    case 0xC0D8D4: case 0xC0D8D8: case 0xC0D8DC: case 0xC0D8E0:
    case 0xC0D8F0: case 0xC0D8F8: case 0xC0D8FC: case 0xC0D900:
    case 0xC0D904: case 0xC0D922: case 0xC0D926: case 0xC0D92A:
    case 0xC0D92E: case 0xC0D936: case 0xC0D93A: case 0xC0D93E:
    case 0xC0D942: case 0xC0D952: case 0xC0D970: case 0xC0D974:
    case 0xC0D978: case 0xC0D97C: case 0xC0D98C: case 0xC0D994:
    case 0xC0D998: case 0xC0D99C: case 0xC0D9A0: case 0xC0D9BC:
    case 0xC0D9C0: case 0xC0D9C4: case 0xC0D9CE: case 0xC0D9D2:
    case 0xC0D9DA: case 0xC0D9DE: case 0xC0D9E2: case 0xC0DA00:
    case 0xC0DA04: case 0xC0DA08: case 0xC0DA0C: case 0xC0DA14:
    case 0xC0DA18: case 0xC0DA1C: case 0xC0DA20: case 0xC0DA30:
    case 0xC0DA42: case 0xC0DA46: case 0xC0DA4A: case 0xC0DA4E:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC0D9B2: case 0xC0DA68:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC0D9BA:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC0DA86:
        width=1; goto move;
    case 0xC0DA8E: case 0xC0DA9A:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC0DA90: case 0xC0DA9C:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC0DA92: case 0xC0DA9E: case 0xC0DACE: case 0xC0DAD2:
    case 0xC0DADA: case 0xC0DAE4: case 0xC0DAEC:
        REG_PC=m68ki_pull_32(); break;
    case 0xC0DA94:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC0DAA0: case 0xC0DAA2:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC0DAB0: case 0xC0DABA: case 0xC0DAC2: case 0xC0DAC6:
        width=2; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC0DAD8: case 0xC0DAE6:
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
static const uint32_t owned_C0D74A[]={
    0xC0D74A,0xC0D750,0xC0D758,0xC0D75A,0xC0D75E,0xC0D762,0xC0D768,0xC0D76C,
    0xC0D770,0xC0D772,0xC0D778,0xC0D77A,0xC0D77C,0xC0D77E,0xC0D780,0xC0D782,
    0xC0D784,0xC0D786,0xC0D788,0xC0D78A,0xC0D78C,0xC0D78E,0xC0D790,0xC0D792,
    0xC0D794,0xC0D796,0xC0D798,0xC0D79A,0xC0D79C,0xC0D79E,0xC0D7A0,0xC0D7A2,
    0xC0D7A4,0xC0D7A6,0xC0D7A8,0xC0D7AA,0xC0D7AC,0xC0D7AE,0xC0D7B0,0xC0D7B2,
    0xC0D7B4,0xC0D7B6,0xC0D7B8,0xC0D7BA,0xC0D7BC,0xC0D7BE,0xC0D7C0,0xC0D7C2,
    0xC0D7C4,0xC0D7C8,0xC0D7CA,0xC0D7CC,0xC0D7D2,0xC0D7D4,0xC0D7D6,0xC0D7D8,
    0xC0D7DA,0xC0D7E0,0xC0D7E6,0xC0D7EA,0xC0D7EC,0xC0D7F0,0xC0D7F2,0xC0D7F6,
    0xC0D7FA,0xC0D7FE,0xC0D802,0xC0D804,0xC0D808,0xC0D80C,0xC0D810,0xC0D814,
    0xC0D816,0xC0D81A,0xC0D81E,0xC0D822,0xC0D826,0xC0D82A,0xC0D82C,0xC0D830,
    0xC0D832,0xC0D836,0xC0D83A,0xC0D83E,0xC0D842,0xC0D844,0xC0D848,0xC0D84C,
    0xC0D850,0xC0D854,0xC0D858,0xC0D85C,0xC0D860,0xC0D862,0xC0D866,0xC0D86A,
    0xC0D86E,0xC0D872,0xC0D878,0xC0D87C,0xC0D882,0xC0D884,0xC0D88A,0xC0D88E,
    0xC0D890,0xC0D894,0xC0D898,0xC0D89C,0xC0D8A2,0xC0D8A6,0xC0D8AA,0xC0D8AE,
    0xC0D8B2,0xC0D8B6,0xC0D8BA,0xC0D8BE,0xC0D8C4,0xC0D8C8,0xC0D8CE,0xC0D8D2,
    0xC0D8D4,0xC0D8D8,0xC0D8DC,0xC0D8E0,0xC0D8E4,0xC0D8EA,0xC0D8EE,0xC0D8F0,
    0xC0D8F4,0xC0D8F8,0xC0D8FC,0xC0D900,0xC0D904,0xC0D908,0xC0D90C,0xC0D912,
    0xC0D916,0xC0D91C,0xC0D920,0xC0D922,0xC0D926,0xC0D92A,0xC0D92E,0xC0D932,
    0xC0D936,0xC0D93A,0xC0D93E,0xC0D942,0xC0D946,0xC0D94C,0xC0D950,0xC0D952,
    0xC0D956,0xC0D95A,0xC0D960,0xC0D964,0xC0D96A,0xC0D96E,0xC0D970,0xC0D974,
    0xC0D978,0xC0D97C,0xC0D980,0xC0D986,0xC0D98A,0xC0D98C,0xC0D990,0xC0D994,
    0xC0D998,0xC0D99C,0xC0D9A0,0xC0D9A4,0xC0D9A8,0xC0D9AE,0xC0D9B2,0xC0D9BA,
    0xC0D9BC,0xC0D9C0,0xC0D9C4,0xC0D9C8,0xC0D9CE,0xC0D9D2,0xC0D9D6,0xC0D9DA,
    0xC0D9DE,0xC0D9E2,0xC0D9E6,0xC0D9EA,0xC0D9F0,0xC0D9F4,0xC0D9FA,0xC0D9FE,
    0xC0DA00,0xC0DA04,0xC0DA08,0xC0DA0C,0xC0DA10,0xC0DA14,0xC0DA18,0xC0DA1C,
    0xC0DA20,0xC0DA24,0xC0DA2A,0xC0DA2E,0xC0DA30,0xC0DA34,0xC0DA38,0xC0DA3E,
    0xC0DA42,0xC0DA46,0xC0DA4A,0xC0DA4E,0xC0DA52,0xC0DA58,0xC0DA5A,0xC0DA60,
    0xC0DA62,0xC0DA68,0xC0DA6C,0xC0DA6E,0xC0DA70,0xC0DA78,0xC0DA80,0xC0DA86,
    0xC0DA8E,0xC0DA90,0xC0DA92,0xC0DA94,0xC0DA9A,0xC0DA9C,0xC0DA9E,
};
int glue_C0D74A_owns(uint32_t pc) { return owns_pc(owned_C0D74A,sizeof owned_C0D74A/sizeof owned_C0D74A[0],pc); }
int glue_C0D74A_complete_step(void) { if(!glue_C0D74A_owns(REG_PC)) return 0; return display_record_selection_step(); }
static const uint32_t owned_C0D752[]={
    0xC0D752,0xC0D758,0xC0D75A,0xC0D75E,0xC0D762,0xC0D768,0xC0D76C,0xC0D770,
    0xC0D772,0xC0D778,0xC0D77A,0xC0D77C,0xC0D77E,0xC0D780,0xC0D782,0xC0D784,
    0xC0D786,0xC0D788,0xC0D78A,0xC0D78C,0xC0D78E,0xC0D790,0xC0D792,0xC0D794,
    0xC0D796,0xC0D798,0xC0D79A,0xC0D79C,0xC0D79E,0xC0D7A0,0xC0D7A2,0xC0D7A4,
    0xC0D7A6,0xC0D7A8,0xC0D7AA,0xC0D7AC,0xC0D7AE,0xC0D7B0,0xC0D7B2,0xC0D7B4,
    0xC0D7B6,0xC0D7B8,0xC0D7BA,0xC0D7BC,0xC0D7BE,0xC0D7C0,0xC0D7C2,0xC0D7C4,
    0xC0D7C8,0xC0D7CA,0xC0D7CC,0xC0D7D2,0xC0D7D4,0xC0D7D6,0xC0D7D8,0xC0D7DA,
    0xC0D7E0,0xC0D7E6,0xC0D7EA,0xC0D7EC,0xC0D7F0,0xC0D7F2,0xC0D7F6,0xC0D7FA,
    0xC0D7FE,0xC0D802,0xC0D804,0xC0D808,0xC0D80C,0xC0D810,0xC0D814,0xC0D816,
    0xC0D81A,0xC0D81E,0xC0D822,0xC0D826,0xC0D82A,0xC0D82C,0xC0D830,0xC0D832,
    0xC0D836,0xC0D83A,0xC0D83E,0xC0D842,0xC0D844,0xC0D848,0xC0D84C,0xC0D850,
    0xC0D854,0xC0D858,0xC0D85C,0xC0D860,0xC0D862,0xC0D866,0xC0D86A,0xC0D86E,
    0xC0D872,0xC0D878,0xC0D87C,0xC0D882,0xC0D884,0xC0D88A,0xC0D88E,0xC0D890,
    0xC0D894,0xC0D898,0xC0D89C,0xC0D8A2,0xC0D8A6,0xC0D8AA,0xC0D8AE,0xC0D8B2,
    0xC0D8B6,0xC0D8BA,0xC0D8BE,0xC0D8C4,0xC0D8C8,0xC0D8CE,0xC0D8D2,0xC0D8D4,
    0xC0D8D8,0xC0D8DC,0xC0D8E0,0xC0D8E4,0xC0D8EA,0xC0D8EE,0xC0D8F0,0xC0D8F4,
    0xC0D8F8,0xC0D8FC,0xC0D900,0xC0D904,0xC0D908,0xC0D90C,0xC0D912,0xC0D916,
    0xC0D91C,0xC0D920,0xC0D922,0xC0D926,0xC0D92A,0xC0D92E,0xC0D932,0xC0D936,
    0xC0D93A,0xC0D93E,0xC0D942,0xC0D946,0xC0D94C,0xC0D950,0xC0D952,0xC0D956,
    0xC0D95A,0xC0D960,0xC0D964,0xC0D96A,0xC0D96E,0xC0D970,0xC0D974,0xC0D978,
    0xC0D97C,0xC0D980,0xC0D986,0xC0D98A,0xC0D98C,0xC0D990,0xC0D994,0xC0D998,
    0xC0D99C,0xC0D9A0,0xC0D9A4,0xC0D9A8,0xC0D9AE,0xC0D9B2,0xC0D9BA,0xC0D9BC,
    0xC0D9C0,0xC0D9C4,0xC0D9C8,0xC0D9CE,0xC0D9D2,0xC0D9D6,0xC0D9DA,0xC0D9DE,
    0xC0D9E2,0xC0D9E6,0xC0D9EA,0xC0D9F0,0xC0D9F4,0xC0D9FA,0xC0D9FE,0xC0DA00,
    0xC0DA04,0xC0DA08,0xC0DA0C,0xC0DA10,0xC0DA14,0xC0DA18,0xC0DA1C,0xC0DA20,
    0xC0DA24,0xC0DA2A,0xC0DA2E,0xC0DA30,0xC0DA34,0xC0DA38,0xC0DA3E,0xC0DA42,
    0xC0DA46,0xC0DA4A,0xC0DA4E,0xC0DA52,0xC0DA58,0xC0DA5A,0xC0DA60,0xC0DA62,
    0xC0DA68,0xC0DA6C,0xC0DA6E,0xC0DA70,0xC0DA78,0xC0DA80,0xC0DA86,0xC0DA8E,
    0xC0DA90,0xC0DA92,0xC0DA94,0xC0DA9A,0xC0DA9C,0xC0DA9E,
};
int glue_C0D752_owns(uint32_t pc) { return owns_pc(owned_C0D752,sizeof owned_C0D752/sizeof owned_C0D752[0],pc); }
int glue_C0D752_complete_step(void) { if(!glue_C0D752_owns(REG_PC)) return 0; return display_record_selection_step(); }
static const uint32_t owned_C0DAA0[]={
    0xC0DAA0,0xC0DAA2,0xC0DAA4,0xC0DAAA,0xC0DAAE,0xC0DAB0,0xC0DAB4,0xC0DAB8,
    0xC0DABA,0xC0DABE,0xC0DAC0,0xC0DAC2,0xC0DAC6,0xC0DACA,0xC0DACC,0xC0DACE,
};
int glue_C0DAA0_owns(uint32_t pc) { return owns_pc(owned_C0DAA0,sizeof owned_C0DAA0/sizeof owned_C0DAA0[0],pc); }
int glue_C0DAA0_complete_step(void) { if(!glue_C0DAA0_owns(REG_PC)) return 0; return display_record_selection_step(); }
static const uint32_t owned_C0DAD0[]={
    0xC0DAD0,0xC0DAD2,
};
int glue_C0DAD0_owns(uint32_t pc) { return owns_pc(owned_C0DAD0,sizeof owned_C0DAD0/sizeof owned_C0DAD0[0],pc); }
int glue_C0DAD0_complete_step(void) { if(!glue_C0DAD0_owns(REG_PC)) return 0; return display_record_selection_step(); }
static const uint32_t owned_C0DAD4[]={
    0xC0DAD4,0xC0DAD8,0xC0DADA,
};
int glue_C0DAD4_owns(uint32_t pc) { return owns_pc(owned_C0DAD4,sizeof owned_C0DAD4/sizeof owned_C0DAD4[0],pc); }
int glue_C0DAD4_complete_step(void) { if(!glue_C0DAD4_owns(REG_PC)) return 0; return display_record_selection_step(); }
static const uint32_t owned_C0DADC[]={
    0xC0DADC,0xC0DAE0,0xC0DAE4,
};
int glue_C0DADC_owns(uint32_t pc) { return owns_pc(owned_C0DADC,sizeof owned_C0DADC/sizeof owned_C0DADC[0],pc); }
int glue_C0DADC_complete_step(void) { if(!glue_C0DADC_owns(REG_PC)) return 0; return display_record_selection_step(); }
static const uint32_t owned_C0DAE6[]={
    0xC0DAE6,0xC0DAE8,0xC0DAEC,
};
int glue_C0DAE6_owns(uint32_t pc) { return owns_pc(owned_C0DAE6,sizeof owned_C0DAE6/sizeof owned_C0DAE6[0],pc); }
int glue_C0DAE6_complete_step(void) { if(!glue_C0DAE6_owns(REG_PC)) return 0; return display_record_selection_step(); }
