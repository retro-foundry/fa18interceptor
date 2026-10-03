/* Complete menu followup family source CPU/bus/event boundaries.
 * Readable behavior lives in menu_followup.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family menu_followup. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_menu_followup.h"

static int menu_followup_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC1029E: case 0xC102A4: case 0xC102B0: case 0xC102B6:
    case 0xC102BE: case 0xC10418: case 0xC1041E: case 0xC1042A:
    case 0xC10430: case 0xC10438: case 0xC1043E: case 0xC10458:
    case 0xC1045E: case 0xC10466: case 0xC1046C: case 0xC1047E:
    case 0xC10696: case 0xC1069E: case 0xC106BA: case 0xC106CA:
        width=1; goto move;
    case 0xC102AA: case 0xC10424:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;
    case 0xC102AC: case 0xC10426: case 0xC10474: case 0xC106D6:
    case 0xC106E6: case 0xC16446: case 0xC16464: case 0xC1647A:
    case 0xC16500:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC102AE: case 0xC10428: case 0xC1647C: case 0xC164B0:
    case 0xC164BC: case 0xC16502:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC102C4: case 0xC10444: case 0xC1048A: case 0xC1049E:
    case 0xC104AE: case 0xC1067C: case 0xC106A6: case 0xC106D0:
    case 0xC106EC: case 0xC106FA: case 0xC1070C: case 0xC1643E:
    case 0xC1645C: case 0xC16472:
        width=2; goto move;
    case 0xC102CC: case 0xC1044C: case 0xC10492: case 0xC104B6:
    case 0xC106AE: case 0xC10720: case 0xC164D0:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC102D0: case 0xC10450: case 0xC10496: case 0xC104BA:
    case 0xC106B2: case 0xC106C2: case 0xC10724: case 0xC16482:
    case 0xC16498: case 0xC164BE: case 0xC164C0: case 0xC164C2:
    case 0xC164C6: case 0xC164D6: case 0xC164E2:
        width=4; goto move;
    case 0xC102D6: case 0xC10456: case 0xC104C0: case 0xC1072C:
    case 0xC16480: case 0xC164B4: case 0xC16506: case 0xC16510:
        REG_PC=m68ki_pull_32(); break;
    case 0xC10464:
        width=1; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC10472: case 0xC10476: case 0xC1068E: case 0xC106D4:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC1047C: case 0xC10694:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC10486:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC1049C: case 0xC106B8: case 0xC106F4: case 0xC1071A:
        step_branch(pc,opcode,1); break;
    case 0xC104A4: case 0xC10682: case 0xC16444: case 0xC16462:
    case 0xC16478:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC104A6: case 0xC10684:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC104A8: case 0xC10688: case 0xC10702: case 0xC16450:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC10678: case 0xC1643A:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC106C0:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC106D8: case 0xC106DE:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC106DC:
        step_branch(pc,opcode,COND_CS()); break;
    case 0xC106E2:
        step_branch(pc,opcode,COND_HI()); break;
    case 0xC106E4:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC106E8: case 0xC106F6: case 0xC10708: case 0xC10714:
    case 0xC164B6:
        width=4; goto move;
    case 0xC106F0: case 0xC106FE: case 0xC10710: case 0xC16470:
    case 0xC16494: case 0xC164A2: case 0xC164E0: case 0xC164EC:
    case 0xC164F6:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC10718: case 0xC164AA:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC1072A: case 0xC1647E: case 0xC164B2: case 0xC16504:
    case 0xC1650E:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC1644A: case 0xC16456: case 0xC1646A: case 0xC1648E:
    case 0xC1649C: case 0xC164CA: case 0xC164DA: case 0xC164E6:
    case 0xC164F0: case 0xC16508:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC16468: case 0xC16496: case 0xC164D4: case 0xC164EE:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC16488:
        address=cache_step_address(mode,reg,4); m68ki_push_32(address); break;
    case 0xC164A4:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC164A8:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC164F8:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
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
static const uint32_t owned_C1029E[]={
    0xC1029E,0xC102A4,0xC102AA,0xC102AC,0xC102AE,0xC102B0,0xC102B6,0xC102BE,
    0xC102C4,0xC102CC,0xC102D0,0xC102D6,
};
int glue_C1029E_owns(uint32_t pc) { return owns_pc(owned_C1029E,sizeof owned_C1029E/sizeof owned_C1029E[0],pc); }
int glue_C1029E_step(void) { if(!glue_C1029E_owns(REG_PC)) return 0; return menu_followup_step(); }
static const uint32_t owned_C10418[]={
    0xC10418,0xC1041E,0xC10424,0xC10426,0xC10428,0xC1042A,0xC10430,0xC10438,
    0xC1043E,0xC10444,0xC1044C,0xC10450,0xC10456,
};
int glue_C10418_owns(uint32_t pc) { return owns_pc(owned_C10418,sizeof owned_C10418/sizeof owned_C10418[0],pc); }
int glue_C10418_step(void) { if(!glue_C10418_owns(REG_PC)) return 0; return menu_followup_step(); }
static const uint32_t owned_C10458[]={
    0xC10458,0xC1045E,0xC10464,0xC10466,0xC1046C,0xC10472,0xC10474,0xC10476,
    0xC1047C,0xC1047E,0xC10486,0xC1048A,0xC10492,0xC10496,0xC1049C,0xC1049E,
    0xC104A4,0xC104A6,0xC104A8,0xC104AE,0xC104B6,0xC104BA,0xC104C0,
};
int glue_C10458_owns(uint32_t pc) { return owns_pc(owned_C10458,sizeof owned_C10458/sizeof owned_C10458[0],pc); }
int glue_C10458_step(void) { if(!glue_C10458_owns(REG_PC)) return 0; return menu_followup_step(); }
static const uint32_t owned_C10678[]={
    0xC10678,0xC1067C,0xC10682,0xC10684,0xC10688,0xC1068E,0xC10694,0xC10696,
    0xC1069E,0xC106A6,0xC106AE,0xC106B2,0xC106B8,0xC106BA,0xC106C0,0xC106C2,
    0xC106CA,0xC106D0,0xC106D4,0xC106D6,0xC106D8,0xC106DC,0xC106DE,0xC106E2,
    0xC106E4,0xC106E6,0xC106E8,0xC106EC,0xC106F0,0xC106F4,0xC106F6,0xC106FA,
    0xC106FE,0xC10702,0xC10708,0xC1070C,0xC10710,0xC10714,0xC10718,0xC1071A,
    0xC10720,0xC10724,0xC1072A,0xC1072C,
};
int glue_C10678_owns(uint32_t pc) { return owns_pc(owned_C10678,sizeof owned_C10678/sizeof owned_C10678[0],pc); }
int glue_C10678_step(void) { if(!glue_C10678_owns(REG_PC)) return 0; return menu_followup_step(); }
static const uint32_t owned_C1643A[]={
    0xC1643A,0xC1643E,0xC16444,0xC16446,0xC1644A,0xC16450,0xC16456,0xC1645C,
    0xC16462,0xC16464,0xC16468,0xC1646A,0xC16470,0xC16472,0xC16478,0xC1647A,
    0xC1647C,0xC1647E,0xC16480,0xC16482,0xC16488,0xC1648E,0xC16494,0xC16496,
    0xC16498,0xC1649C,0xC164A2,0xC164A4,0xC164A8,0xC164AA,0xC164B0,0xC164B2,
    0xC164B4,0xC164B6,0xC164BC,0xC164BE,0xC164C0,0xC164C2,0xC164C6,0xC164CA,
    0xC164D0,0xC164D4,0xC164D6,0xC164DA,0xC164E0,0xC164E2,0xC164E6,0xC164EC,
    0xC164EE,0xC164F0,0xC164F6,0xC164F8,0xC16500,0xC16502,0xC16504,0xC16506,
    0xC16508,0xC1650E,0xC16510,
};
int glue_C1643A_owns(uint32_t pc) { return owns_pc(owned_C1643A,sizeof owned_C1643A/sizeof owned_C1643A[0],pc); }
int glue_C1643A_step(void) { if(!glue_C1643A_owns(REG_PC)) return 0; return menu_followup_step(); }
