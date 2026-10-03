/* Complete menu outcome family source CPU/bus/event boundaries.
 * Readable behavior lives in menu_outcome.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family menu_outcome. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_menu_outcome.h"

static int menu_outcome_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC104C2: case 0xC1078A:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC104C6: case 0xC1059E:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC104CA: case 0xC1054A: case 0xC10562: case 0xC1056A:
    case 0xC1057C: case 0xC10582: case 0xC105FE: case 0xC107D0:
    case 0xC107D6: case 0xC107DA: case 0xC1080C: case 0xC10812:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC104CC: case 0xC104D2: case 0xC104D8: case 0xC104E0:
    case 0xC104E6: case 0xC104F8: case 0xC1052A: case 0xC1055C:
    case 0xC105A6: case 0xC105B8: case 0xC105BE: case 0xC105C6:
    case 0xC105CC: case 0xC105E0: case 0xC10600: case 0xC10606:
    case 0xC10630: case 0xC10638: case 0xC1072E: case 0xC10746:
    case 0xC1076A: case 0xC1079A: case 0xC107A2: case 0xC10800:
    case 0xC1086E: case 0xC1088A: case 0xC1089A: case 0xC108AE:
    case 0xC108B6:
        width=1; goto move;
    case 0xC104DE: case 0xC105C4:
        width=1; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC104EC: case 0xC104F0: case 0xC1055E: case 0xC105AC:
    case 0xC105B0: case 0xC1075A: case 0xC10762: case 0xC107AC:
    case 0xC10890: case 0xC108BC:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC104EE: case 0xC10560: case 0xC10736: case 0xC10822:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC104F6: case 0xC105B6: case 0xC10760: case 0xC10768:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC10500: case 0xC105D4: case 0xC10772: case 0xC108A2:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC10504: case 0xC1051A: case 0xC10532: case 0xC10542:
    case 0xC1054C: case 0xC10564: case 0xC1057E: case 0xC105D8:
    case 0xC105F4: case 0xC1060C: case 0xC10626: case 0xC10738:
    case 0xC10776: case 0xC1078E: case 0xC107A8: case 0xC107C8:
    case 0xC107D2: case 0xC107F0: case 0xC107FC: case 0xC1080E:
    case 0xC10876: case 0xC108A6: case 0xC108DA: case 0xC108EA:
        width=2; goto move;
    case 0xC1050C: case 0xC10576: case 0xC10590: case 0xC10594:
    case 0xC105E8: case 0xC1061A: case 0xC10640: case 0xC1074E:
    case 0xC1077E: case 0xC107B2: case 0xC107E6: case 0xC10848:
    case 0xC10856: case 0xC10862: case 0xC1087E: case 0xC108C0:
    case 0xC108CC: case 0xC108F2:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC10510: case 0xC1056C: case 0xC1056E: case 0xC10584:
    case 0xC10586: case 0xC10588: case 0xC10598: case 0xC105EC:
    case 0xC1061E: case 0xC10644: case 0xC10752: case 0xC10782:
    case 0xC107B6: case 0xC107D8: case 0xC107DC: case 0xC107DE:
    case 0xC1084C: case 0xC1085A: case 0xC10866: case 0xC10882:
    case 0xC108C4: case 0xC108D0: case 0xC108F6:
        width=4; goto move;
    case 0xC10516: case 0xC1057A: case 0xC10808: case 0xC1082C:
    case 0xC10834: case 0xC1083C: case 0xC10844: case 0xC10852:
    case 0xC10860: case 0xC1086C: case 0xC10888: case 0xC108CA:
        step_branch(pc,opcode,1); break;
    case 0xC10520: case 0xC105FA: case 0xC1062C: case 0xC10794:
    case 0xC108E0:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC10522: case 0xC105FC: case 0xC1062E: case 0xC10796:
    case 0xC10892: case 0xC108E2:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC10524: case 0xC10740: case 0xC10894: case 0xC108E4:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC10530: case 0xC107A0:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC10536: case 0xC1053C: case 0xC107BC: case 0xC107C2:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC1053A: case 0xC107C0:
        step_branch(pc,opcode,COND_CS()); break;
    case 0xC10540: case 0xC107C6:
        step_branch(pc,opcode,COND_HI()); break;
    case 0xC10550: case 0xC107EA: case 0xC107F6:
        width=4; goto move;
    case 0xC10556:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC1055A:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC10568:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC10570: case 0xC1058A: case 0xC10614: case 0xC107E0:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC105A2: case 0xC108D6:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC105A4: case 0xC105F2: case 0xC10624: case 0xC1064A:
    case 0xC10758: case 0xC10788: case 0xC108D8: case 0xC108FC:
        REG_PC=m68ki_pull_32(); break;
    case 0xC105AE: case 0xC1081A: case 0xC108BE:
        step_branch(pc,opcode,COND_MI()); break;
    case 0xC10734: case 0xC108B4:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC107AE:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC107F4:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC10814:
        width=4; value=m68ki_read_imm_32(); operation='-'; goto arithmetic;
    case 0xC1081E:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC10824:
        REG_PC=cache_step_address(mode,reg,4); break;
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
static const uint32_t owned_C104C2[]={
    0xC104C2,0xC104C6,0xC104CA,0xC104CC,0xC104D2,0xC104D8,0xC104DE,0xC104E0,
    0xC104E6,0xC104EC,0xC104EE,0xC104F0,0xC104F6,0xC104F8,0xC10500,0xC10504,
    0xC1050C,0xC10510,0xC10516,0xC1051A,0xC10520,0xC10522,0xC10524,0xC1052A,
    0xC10530,0xC10532,0xC10536,0xC1053A,0xC1053C,0xC10540,0xC10542,0xC1054A,
    0xC1054C,0xC10550,0xC10556,0xC1055A,0xC1055C,0xC1055E,0xC10560,0xC10562,
    0xC10564,0xC10568,0xC1056A,0xC1056C,0xC1056E,0xC10570,0xC10576,0xC1057A,
    0xC1057C,0xC1057E,0xC10582,0xC10584,0xC10586,0xC10588,0xC1058A,0xC10590,
    0xC10594,0xC10598,0xC1059E,0xC105A2,0xC105A4,
};
int glue_C104C2_owns(uint32_t pc) { return owns_pc(owned_C104C2,sizeof owned_C104C2/sizeof owned_C104C2[0],pc); }
int glue_C104C2_step(void) { if(!glue_C104C2_owns(REG_PC)) return 0; return menu_outcome_step(); }
static const uint32_t owned_C105F4[]={
    0xC105F4,0xC105FA,0xC105FC,0xC105FE,0xC10600,0xC10606,0xC1060C,0xC10614,
    0xC1061A,0xC1061E,0xC10624,
};
int glue_C105F4_owns(uint32_t pc) { return owns_pc(owned_C105F4,sizeof owned_C105F4/sizeof owned_C105F4[0],pc); }
int glue_C105F4_step(void) { if(!glue_C105F4_owns(REG_PC)) return 0; return menu_outcome_step(); }
static const uint32_t owned_C1072E[]={
    0xC1072E,0xC10734,0xC10736,0xC10738,0xC10740,0xC10746,0xC1074E,0xC10752,
    0xC10758,
};
int glue_C1072E_owns(uint32_t pc) { return owns_pc(owned_C1072E,sizeof owned_C1072E/sizeof owned_C1072E[0],pc); }
int glue_C1072E_step(void) { if(!glue_C1072E_owns(REG_PC)) return 0; return menu_outcome_step(); }
static const uint32_t owned_C1078A[]={
    0xC1078A,0xC1078E,0xC10794,0xC10796,0xC1079A,0xC107A0,0xC107A2,0xC107A8,
    0xC107AC,0xC107AE,0xC107B2,0xC107B6,0xC107BC,0xC107C0,0xC107C2,0xC107C6,
    0xC107C8,0xC107D0,0xC107D2,0xC107D6,0xC107D8,0xC107DA,0xC107DC,0xC107DE,
    0xC107E0,0xC107E6,0xC107EA,0xC107F0,0xC107F4,0xC107F6,0xC107FC,0xC10800,
    0xC10808,0xC1080C,0xC1080E,0xC10812,0xC10814,0xC1081A,0xC1081E,0xC10822,
    0xC10824,0xC1082C,0xC10834,0xC1083C,0xC10844,0xC10848,0xC1084C,0xC10852,
    0xC10856,0xC1085A,0xC10860,0xC10862,0xC10866,0xC1086C,0xC1086E,0xC10876,
    0xC1087E,0xC10882,0xC10888,0xC1088A,0xC10890,0xC10892,0xC10894,0xC1089A,
    0xC108A2,0xC108A6,0xC108AE,0xC108B4,0xC108B6,0xC108BC,0xC108BE,0xC108C0,
    0xC108C4,0xC108CA,0xC108CC,0xC108D0,0xC108D6,0xC108D8,
};
int glue_C1078A_owns(uint32_t pc) { return owns_pc(owned_C1078A,sizeof owned_C1078A/sizeof owned_C1078A[0],pc); }
int glue_C1078A_step(void) { if(!glue_C1078A_owns(REG_PC)) return 0; return menu_outcome_step(); }
static const uint32_t owned_C105A6[]={
    0xC105A6,0xC105AC,0xC105AE,0xC105B0,0xC105B6,0xC105B8,0xC105BE,0xC105C4,
    0xC105C6,0xC105CC,0xC105D4,0xC105D8,0xC105E0,0xC105E8,0xC105EC,0xC105F2,
};
int glue_C105A6_owns(uint32_t pc) { return owns_pc(owned_C105A6,sizeof owned_C105A6/sizeof owned_C105A6[0],pc); }
int glue_C105A6_step(void) { if(!glue_C105A6_owns(REG_PC)) return 0; return menu_outcome_step(); }
static const uint32_t owned_C10626[]={
    0xC10626,0xC1062C,0xC1062E,0xC10630,0xC10638,0xC10640,0xC10644,0xC1064A,
};
int glue_C10626_owns(uint32_t pc) { return owns_pc(owned_C10626,sizeof owned_C10626/sizeof owned_C10626[0],pc); }
int glue_C10626_step(void) { if(!glue_C10626_owns(REG_PC)) return 0; return menu_outcome_step(); }
static const uint32_t owned_C1075A[]={
    0xC1075A,0xC10760,0xC10762,0xC10768,0xC1076A,0xC10772,0xC10776,0xC1077E,
    0xC10782,0xC10788,
};
int glue_C1075A_owns(uint32_t pc) { return owns_pc(owned_C1075A,sizeof owned_C1075A/sizeof owned_C1075A[0],pc); }
int glue_C1075A_step(void) { if(!glue_C1075A_owns(REG_PC)) return 0; return menu_outcome_step(); }
static const uint32_t owned_C108DA[]={
    0xC108DA,0xC108E0,0xC108E2,0xC108E4,0xC108EA,0xC108F2,0xC108F6,0xC108FC,
};
int glue_C108DA_owns(uint32_t pc) { return owns_pc(owned_C108DA,sizeof owned_C108DA/sizeof owned_C108DA[0],pc); }
int glue_C108DA_step(void) { if(!glue_C108DA_owns(REG_PC)) return 0; return menu_outcome_step(); }
static const uint32_t owned_C29368[]={
    0xC29368,0xC29370,0xC29376,0xC2937E,0xC29384,0xC2938A,0xC2938C,0xC2938E,
    0xC29392,0xC29396,0xC29398,0xC2939A,0xC2939E,0xC293A2,0xC293A6,0xC293A8,
    0xC293AE,0xC293B0,0xC293B6,0xC293BC,0xC293BE,0xC293C0,0xC293C6,0xC293C8,
    0xC293D0,0xC293D6,0xC293DC,0xC293DE,0xC293E4,0xC293EC,0xC293F0,0xC293F4,
    0xC293FA,0xC293FC,0xC293FE,0xC29400,0xC29408,
};
int glue_C29368_owns(uint32_t pc) { return owns_pc(owned_C29368,sizeof owned_C29368/sizeof owned_C29368[0],pc); }
extern int glue_C29042_step(void);
int glue_C29368_step(void) { if(!glue_C29368_owns(REG_PC)) return 0; return glue_C29042_step(); }
