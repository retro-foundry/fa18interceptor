/* Complete menu return family source CPU/bus/event boundaries.
 * Readable behavior lives in menu_return.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family menu_return. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_menu_return.h"

static int menu_return_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0FB70: case 0xC0FB8A: case 0xC101FC: case 0xC10238:
    case 0xC1025E: case 0xC102EE: case 0xC1031A: case 0xC10338:
    case 0xC10346: case 0xC103D0: case 0xC10664: case 0xC1092E:
    case 0xC10952: case 0xC10998: case 0xC109BC: case 0xC109F2:
    case 0xC10BF4:
        width=2; goto move;
    case 0xC0FB76: case 0xC10202: case 0xC10320: case 0xC10958:
    case 0xC109C2:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC0FB78: case 0xC10204: case 0xC10322: case 0xC10918:
    case 0xC1095A: case 0xC109C4:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC0FB7A: case 0xC0FB9E: case 0xC0FBB6: case 0xC0FBC8:
    case 0xC10208: case 0xC1020E: case 0xC10216: case 0xC10228:
    case 0xC1022E: case 0xC10242: case 0xC10248: case 0xC10250:
    case 0xC10258: case 0xC102DA: case 0xC102E0: case 0xC102E8:
    case 0xC1032A: case 0xC10362: case 0xC10374: case 0xC1037C:
    case 0xC1038A: case 0xC10390: case 0xC1039C: case 0xC103A8:
    case 0xC103BC: case 0xC1064C: case 0xC10658: case 0xC1065E:
    case 0xC10900: case 0xC10910: case 0xC1091A: case 0xC10926:
    case 0xC10942: case 0xC1095C: case 0xC10970: case 0xC10980:
    case 0xC1098C: case 0xC10992: case 0xC109AC: case 0xC109CC:
    case 0xC109D8: case 0xC109E0: case 0xC109FA: case 0xC10A02:
    case 0xC10A0C: case 0xC10A12: case 0xC10BAE: case 0xC10BB8:
    case 0xC10BCC: case 0xC10BD2: case 0xC10BDA: case 0xC10BE0:
    case 0xC10BE8: case 0xC10BEE:
        width=1; goto move;
    case 0xC0FB80: case 0xC0FBBC: case 0xC0FBC0: case 0xC1030E:
    case 0xC10368: case 0xC1036C: case 0xC10916:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC0FB82: case 0xC10236: case 0xC10332: case 0xC103A6:
    case 0xC10654: case 0xC10908: case 0xC1094A: case 0xC10978:
    case 0xC10988: case 0xC109B4: case 0xC109F0:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0FB84: case 0xC10324: case 0xC1034E: case 0xC10396:
    case 0xC103CA: case 0xC109C6: case 0xC10BC0:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC0FB92: case 0xC0FBAA: case 0xC0FBD4: case 0xC1021C:
    case 0xC10266: case 0xC102F6: case 0xC10354: case 0xC103B0:
    case 0xC103D8: case 0xC1066C: case 0xC10936: case 0xC10964:
    case 0xC109A0: case 0xC10A18: case 0xC10BFC:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC0FB96: case 0xC0FBAE: case 0xC0FBD8: case 0xC10220:
    case 0xC1026A: case 0xC102FA: case 0xC10306: case 0xC10358:
    case 0xC103B4: case 0xC103DC: case 0xC10670: case 0xC1093A:
    case 0xC10968: case 0xC109A4: case 0xC10A1C: case 0xC10C00:
        width=4; goto move;
    case 0xC0FB9C: case 0xC10318: case 0xC10340: case 0xC103BA:
    case 0xC1090E: case 0xC10950: case 0xC1097E: case 0xC109BA:
        step_branch(pc,opcode,1); break;
    case 0xC0FBA6: case 0xC0FBD0: case 0xC10316: case 0xC10384:
    case 0xC1090A: case 0xC10922: case 0xC1094C: case 0xC1097A:
    case 0xC109B6: case 0xC10BB6: case 0xC10BC6:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC0FBB4: case 0xC0FBDE: case 0xC10226: case 0xC10270:
    case 0xC10300: case 0xC10360: case 0xC103E2: case 0xC10676:
    case 0xC108FE: case 0xC10940: case 0xC1096E: case 0xC109AA:
    case 0xC10A22: case 0xC10C06:
        REG_PC=m68ki_pull_32(); break;
    case 0xC0FBBE: case 0xC1036A:
        step_branch(pc,opcode,COND_MI()); break;
    case 0xC0FBC6: case 0xC10314: case 0xC10372:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC10206: case 0xC10240: case 0xC102D8: case 0xC10388:
    case 0xC10656: case 0xC1098A: case 0xC10A0A: case 0xC10BCA:
    case 0xC10BD8: case 0xC10BE6:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC10234:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;
    case 0xC10302:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC10330: case 0xC10652: case 0xC10906: case 0xC10948:
    case 0xC10976: case 0xC10986: case 0xC109B2:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC10334: case 0xC10342:
        width=4; goto move;
    case 0xC1033C: case 0xC1034A:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC1035E:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC103A2: case 0xC109D2:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC103C4:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC109D6:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC109E6:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC109E8:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC109EA:
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
static const uint32_t owned_C1064C[]={
    0xC1064C,0xC10652,0xC10654,0xC10656,0xC10658,0xC1065E,0xC10664,0xC1066C,
    0xC10670,0xC10676,
};
int glue_C1064C_owns(uint32_t pc) { return owns_pc(owned_C1064C,sizeof owned_C1064C/sizeof owned_C1064C[0],pc); }
int glue_C1064C_step(void) { if(!glue_C1064C_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C108FE[]={
    0xC108FE,
};
int glue_C108FE_owns(uint32_t pc) { return owns_pc(owned_C108FE,sizeof owned_C108FE/sizeof owned_C108FE[0],pc); }
int glue_C108FE_step(void) { if(!glue_C108FE_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C10900[]={
    0xC10900,0xC10906,0xC10908,0xC1090A,0xC1090E,0xC10910,0xC10916,0xC10918,
    0xC1091A,0xC10922,0xC10926,0xC1092E,0xC10936,0xC1093A,0xC10940,
};
int glue_C10900_owns(uint32_t pc) { return owns_pc(owned_C10900,sizeof owned_C10900/sizeof owned_C10900[0],pc); }
int glue_C10900_step(void) { if(!glue_C10900_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C10970[]={
    0xC10970,0xC10976,0xC10978,0xC1097A,0xC1097E,0xC10980,0xC10986,0xC10988,
    0xC1098A,0xC1098C,0xC10992,0xC10998,0xC109A0,0xC109A4,0xC109AA,
};
int glue_C10970_owns(uint32_t pc) { return owns_pc(owned_C10970,sizeof owned_C10970/sizeof owned_C10970[0],pc); }
int glue_C10970_step(void) { if(!glue_C10970_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C102D8[]={
    0xC102D8,0xC102DA,0xC102E0,0xC102E8,0xC102EE,0xC102F6,0xC102FA,0xC10300,
};
int glue_C102D8_owns(uint32_t pc) { return owns_pc(owned_C102D8,sizeof owned_C102D8/sizeof owned_C102D8[0],pc); }
int glue_C102D8_step(void) { if(!glue_C102D8_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C0FB70[]={
    0xC0FB70,0xC0FB76,0xC0FB78,0xC0FB7A,0xC0FB80,0xC0FB82,0xC0FB84,0xC0FB8A,
    0xC0FB92,0xC0FB96,0xC0FB9C,0xC0FB9E,0xC0FBA6,0xC0FBAA,0xC0FBAE,0xC0FBB4,
};
int glue_C0FB70_owns(uint32_t pc) { return owns_pc(owned_C0FB70,sizeof owned_C0FB70/sizeof owned_C0FB70[0],pc); }
int glue_C0FB70_step(void) { if(!glue_C0FB70_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C0FBB6[]={
    0xC0FBB6,0xC0FBBC,0xC0FBBE,0xC0FBC0,0xC0FBC6,0xC0FBC8,0xC0FBD0,0xC0FBD4,
    0xC0FBD8,0xC0FBDE,
};
int glue_C0FBB6_owns(uint32_t pc) { return owns_pc(owned_C0FBB6,sizeof owned_C0FBB6/sizeof owned_C0FBB6[0],pc); }
int glue_C0FBB6_step(void) { if(!glue_C0FBB6_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C101FC[]={
    0xC101FC,0xC10202,0xC10204,0xC10206,0xC10208,0xC1020E,0xC10216,0xC1021C,
    0xC10220,0xC10226,
};
int glue_C101FC_owns(uint32_t pc) { return owns_pc(owned_C101FC,sizeof owned_C101FC/sizeof owned_C101FC[0],pc); }
int glue_C101FC_step(void) { if(!glue_C101FC_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C10228[]={
    0xC10228,0xC1022E,0xC10234,0xC10236,0xC10238,0xC10240,0xC10242,0xC10248,
    0xC10250,0xC10258,0xC1025E,0xC10266,0xC1026A,0xC10270,
};
int glue_C10228_owns(uint32_t pc) { return owns_pc(owned_C10228,sizeof owned_C10228/sizeof owned_C10228[0],pc); }
int glue_C10228_step(void) { if(!glue_C10228_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C10942[]={
    0xC10942,0xC10948,0xC1094A,0xC1094C,0xC10950,0xC10952,0xC10958,0xC1095A,
    0xC1095C,0xC10964,0xC10968,0xC1096E,
};
int glue_C10942_owns(uint32_t pc) { return owns_pc(owned_C10942,sizeof owned_C10942/sizeof owned_C10942[0],pc); }
int glue_C10942_step(void) { if(!glue_C10942_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C109AC[]={
    0xC109AC,0xC109B2,0xC109B4,0xC109B6,0xC109BA,0xC109BC,0xC109C2,0xC109C4,
    0xC109C6,0xC109CC,0xC109D2,0xC109D6,0xC109D8,0xC109E0,0xC109E6,0xC109E8,
    0xC109EA,0xC109F0,0xC109F2,0xC109FA,0xC10A02,0xC10A0A,0xC10A0C,0xC10A12,
    0xC10A18,0xC10A1C,0xC10A22,
};
int glue_C109AC_owns(uint32_t pc) { return owns_pc(owned_C109AC,sizeof owned_C109AC/sizeof owned_C109AC[0],pc); }
int glue_C109AC_step(void) { if(!glue_C109AC_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C10302[]={
    0xC10302,0xC10306,0xC1030E,0xC10314,0xC10316,0xC10318,0xC1031A,0xC10320,
    0xC10322,0xC10324,0xC1032A,0xC10330,0xC10332,0xC10334,0xC10338,0xC1033C,
    0xC10340,0xC10342,0xC10346,0xC1034A,0xC1034E,0xC10354,0xC10358,0xC1035E,
    0xC10360,
};
int glue_C10302_owns(uint32_t pc) { return owns_pc(owned_C10302,sizeof owned_C10302/sizeof owned_C10302[0],pc); }
int glue_C10302_step(void) { if(!glue_C10302_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C10BAE[]={
    0xC10BAE,0xC10BB6,0xC10BB8,0xC10BC0,0xC10BC6,0xC10BCA,0xC10BCC,0xC10BD2,
    0xC10BD8,0xC10BDA,0xC10BE0,0xC10BE6,0xC10BE8,0xC10BEE,0xC10BF4,0xC10BFC,
    0xC10C00,0xC10C06,
};
int glue_C10BAE_owns(uint32_t pc) { return owns_pc(owned_C10BAE,sizeof owned_C10BAE/sizeof owned_C10BAE[0],pc); }
int glue_C10BAE_step(void) { if(!glue_C10BAE_owns(REG_PC)) return 0; return menu_return_step(); }
static const uint32_t owned_C10362[]={
    0xC10362,0xC10368,0xC1036A,0xC1036C,0xC10372,0xC10374,0xC1037C,0xC10384,
    0xC10388,0xC1038A,0xC10390,0xC10396,0xC1039C,0xC103A2,0xC103A6,0xC103A8,
    0xC103B0,0xC103B4,0xC103BA,0xC103BC,0xC103C4,0xC103CA,0xC103D0,0xC103D8,
    0xC103DC,0xC103E2,
};
int glue_C10362_owns(uint32_t pc) { return owns_pc(owned_C10362,sizeof owned_C10362/sizeof owned_C10362[0],pc); }
int glue_C10362_step(void) { if(!glue_C10362_owns(REG_PC)) return 0; return menu_return_step(); }
