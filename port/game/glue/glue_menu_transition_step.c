/* Complete menu transition family source CPU/bus/event boundaries.
 * Readable behavior lives in menu_transition.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family menu_transition. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_menu_transition.h"

static int menu_transition_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0FCB4: case 0xC0FECE: case 0xC17C2A:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0FCB8: case 0xC0FCEA: case 0xC0FDC8: case 0xC0FDF4:
    case 0xC0FE1E: case 0xC0FEFC: case 0xC0FF06: case 0xC0FF22:
    case 0xC0FF3A: case 0xC0FF48: case 0xC0FF4A: case 0xC0FFAC:
    case 0xC1001A: case 0xC10036: case 0xC1009E: case 0xC100FA:
    case 0xC1015C: case 0xC17C38: case 0xC17C40: case 0xC17C4A:
    case 0xC17C50: case 0xC17C54: case 0xC24F22: case 0xC24F2A:
    case 0xC24F3C: case 0xC24F76: case 0xC24F80: case 0xC24F88:
        width=4; goto move;
    case 0xC0FCC0: case 0xC0FCF4: case 0xC0FD9E: case 0xC0FDD0:
    case 0xC0FDE4: case 0xC0FED2: case 0xC0FF6C: case 0xC0FF74:
    case 0xC0FF88: case 0xC0FF90: case 0xC0FF98: case 0xC0FFB2:
    case 0xC0FFBA: case 0xC1000E: case 0xC1002A: case 0xC10048:
    case 0xC10058: case 0xC10060: case 0xC1006C: case 0xC10082:
    case 0xC100B2: case 0xC100C2: case 0xC100CA: case 0xC100D2:
    case 0xC10102: case 0xC1010C: case 0xC10112: case 0xC1012A:
    case 0xC10130: case 0xC1014A: case 0xC10162: case 0xC1016C:
        width=1; goto move;
    case 0xC0FCC8: case 0xC0FCFA: case 0xC0FD7E: case 0xC0FDA4:
    case 0xC0FDD6:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC0FCCE: case 0xC0FCDE: case 0xC0FD84: case 0xC0FDB0:
    case 0xC0FF36: case 0xC17C36:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC0FCD0: case 0xC0FD92: case 0xC0FE24: case 0xC0FE2C:
    case 0xC0FEEA: case 0xC0FF82: case 0xC10066: case 0xC100A8:
    case 0xC100F0: case 0xC10152:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC0FCD6: case 0xC0FD1C: case 0xC0FD32: case 0xC0FD48:
    case 0xC0FD5E: case 0xC0FD74: case 0xC0FD8A: case 0xC0FDB2:
    case 0xC0FDBC: case 0xC0FE0C: case 0xC0FEDA: case 0xC0FEE0:
    case 0xC0FF10: case 0xC0FF58: case 0xC0FF64: case 0xC0FFC4:
    case 0xC10040: case 0xC10072: case 0xC1007A: case 0xC1008A:
    case 0xC10094: case 0xC100BA: case 0xC10138: case 0xC1013E:
    case 0xC24E92: case 0xC24EA4: case 0xC24EB6: case 0xC24EC8:
    case 0xC24EDA: case 0xC24EEC: case 0xC24EFE: case 0xC24F10:
    case 0xC24F56: case 0xC24F68:
        width=2; goto move;
    case 0xC0FCDC:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0FCE2: case 0xC0FD06: case 0xC0FDDA: case 0xC0FF60:
    case 0xC100DA: case 0xC10124: case 0xC17C42: case 0xC17C56:
    case 0xC24EA0: case 0xC24EB2: case 0xC24EC4: case 0xC24ED6:
    case 0xC24EE8: case 0xC24EFA: case 0xC24F0C: case 0xC24F1E:
    case 0xC24F38: case 0xC24F52: case 0xC24F64: case 0xC24F7C:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC0FCE6: case 0xC0FDC4: case 0xC0FDF0: case 0xC0FE1A:
    case 0xC0FFA8: case 0xC10016: case 0xC10032: case 0xC1009A:
    case 0xC100F6: case 0xC10158: case 0xC17C46: case 0xC17C5A:
    case 0xC24E96: case 0xC24EA8: case 0xC24EBA: case 0xC24ECC:
    case 0xC24EDE: case 0xC24EF0: case 0xC24F02: case 0xC24F14:
    case 0xC24F2E: case 0xC24F48: case 0xC24F5A: case 0xC24F6C:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC0FCF0: case 0xC0FD24: case 0xC0FD3A: case 0xC0FD50:
    case 0xC0FD66: case 0xC0FD7C: case 0xC0FDBA: case 0xC0FDCE:
    case 0xC0FDFA: case 0xC0FE2A: case 0xC0FF2C: case 0xC0FF44:
    case 0xC0FFE2: case 0xC0FFEA: case 0xC0FFF2: case 0xC0FFFA:
    case 0xC10002: case 0xC1000A: case 0xC10020: case 0xC1003C:
    case 0xC100A4: case 0xC100AE: case 0xC10100:
        step_branch(pc,opcode,1); break;
    case 0xC0FCFC:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC0FD00: case 0xC0FEF0: case 0xC17C3C:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC0FD0A: case 0xC0FDDE: case 0xC0FDFC: case 0xC0FE02:
    case 0xC0FEF2: case 0xC0FEFE: case 0xC0FF24: case 0xC0FF3C:
    case 0xC0FF50: case 0xC0FF7C: case 0xC0FFA2: case 0xC10024:
    case 0xC10050: case 0xC100DE: case 0xC100E4: case 0xC100EA:
    case 0xC10118: case 0xC1011E: case 0xC10144: case 0xC10174:
    case 0xC24F8E:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0FD10: case 0xC0FD26: case 0xC0FD3C: case 0xC0FD52:
    case 0xC0FD68: case 0xC0FDEA:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC0FD16: case 0xC0FD2C: case 0xC0FD42: case 0xC0FD58:
    case 0xC0FD6E: case 0xC0FDA6: case 0xC0FDEE: case 0xC0FFD8:
    case 0xC1016A:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0FD18: case 0xC0FD2E: case 0xC0FD44: case 0xC0FD5A:
    case 0xC0FD70: case 0xC0FD86: case 0xC0FD98: case 0xC0FE08:
    case 0xC0FE14: case 0xC24E8C: case 0xC24F9C:
        width=4; goto move;
    case 0xC0FD20: case 0xC0FD36: case 0xC0FD4C: case 0xC0FD62:
    case 0xC0FD78: case 0xC0FD8E: case 0xC0FE10: case 0xC0FEF8:
    case 0xC0FF04: case 0xC0FF2A: case 0xC0FF42: case 0xC0FF56:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0FD9C: case 0xC0FE18:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC0FDA8: case 0xC0FF2E: case 0xC17C2E:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC0FDD8: case 0xC0FEE6:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC0FE32: case 0xC1017A: case 0xC17C5E:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC0FE34: case 0xC1017C: case 0xC17C60: case 0xC24FA2:
        REG_PC=m68ki_pull_32(); break;
    case 0xC0FED8:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC0FEE4:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC0FEFA: case 0xC0FF20: case 0xC0FF38: case 0xC0FF46:
    case 0xC0FFC2: case 0xC0FFC8: case 0xC10056: case 0xC1005E:
    case 0xC1010A: case 0xC10128: case 0xC10136: case 0xC17C3E:
    case 0xC17C4E: case 0xC17C52: case 0xC24E8A: case 0xC24E9C:
    case 0xC24E9E: case 0xC24EAE: case 0xC24EB0: case 0xC24EC0:
    case 0xC24EC2: case 0xC24ED2: case 0xC24ED4: case 0xC24EE4:
    case 0xC24EE6: case 0xC24EF6: case 0xC24EF8: case 0xC24F08:
    case 0xC24F0A: case 0xC24F1A: case 0xC24F1C: case 0xC24F34:
    case 0xC24F36: case 0xC24F4E: case 0xC24F50: case 0xC24F60:
    case 0xC24F62: case 0xC24F72: case 0xC24F74: case 0xC24F9A:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC0FF14: case 0xC0FF1A:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC0FF18:
        step_branch(pc,opcode,COND_CS()); break;
    case 0xC0FF1E:
        step_branch(pc,opcode,COND_HI()); break;
    case 0xC0FFCA:
        width=4; value=m68ki_read_imm_32(); operation='-'; goto arithmetic;
    case 0xC0FFD0:
        step_branch(pc,opcode,COND_MI()); break;
    case 0xC0FFD4:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC0FFDA:
        REG_PC=cache_step_address(mode,reg,4); break;
    case 0xC10090:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC10168:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC24F26: case 0xC24F42:
        step_divide_unsigned(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC24F2C: case 0xC24F40: case 0xC24F46:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC24F3E:
        step_swap(&D(reg)); break;
    case 0xC24F86:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC24F8A:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC24F94:
        A(destination)+=cache_step_read(mode,reg,4); break;
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
static const uint32_t owned_C0FCB4[]={
    0xC0FCB4,0xC0FCB8,0xC0FCC0,0xC0FCC8,0xC0FCCE,0xC0FCD0,0xC0FCD6,0xC0FCDC,
    0xC0FCDE,0xC0FCE2,0xC0FCE6,0xC0FCEA,0xC0FCF0,0xC0FCF4,0xC0FCFA,0xC0FCFC,
    0xC0FD00,0xC0FD06,0xC0FD0A,0xC0FD10,0xC0FD16,0xC0FD18,0xC0FD1C,0xC0FD20,
    0xC0FD24,0xC0FD26,0xC0FD2C,0xC0FD2E,0xC0FD32,0xC0FD36,0xC0FD3A,0xC0FD3C,
    0xC0FD42,0xC0FD44,0xC0FD48,0xC0FD4C,0xC0FD50,0xC0FD52,0xC0FD58,0xC0FD5A,
    0xC0FD5E,0xC0FD62,0xC0FD66,0xC0FD68,0xC0FD6E,0xC0FD70,0xC0FD74,0xC0FD78,
    0xC0FD7C,0xC0FD7E,0xC0FD84,0xC0FD86,0xC0FD8A,0xC0FD8E,0xC0FD92,0xC0FD98,
    0xC0FD9C,0xC0FD9E,0xC0FDA4,0xC0FDA6,0xC0FDA8,0xC0FDB0,0xC0FDB2,0xC0FDBA,
    0xC0FDBC,0xC0FDC4,0xC0FDC8,0xC0FDCE,0xC0FDD0,0xC0FDD6,0xC0FDD8,0xC0FDDA,
    0xC0FDDE,0xC0FDE4,0xC0FDEA,0xC0FDEE,0xC0FDF0,0xC0FDF4,0xC0FDFA,0xC0FDFC,
    0xC0FE02,0xC0FE08,0xC0FE0C,0xC0FE10,0xC0FE14,0xC0FE18,0xC0FE1A,0xC0FE1E,
    0xC0FE24,0xC0FE2A,0xC0FE2C,0xC0FE32,0xC0FE34,
};
int glue_C0FCB4_owns(uint32_t pc) { return owns_pc(owned_C0FCB4,sizeof owned_C0FCB4/sizeof owned_C0FCB4[0],pc); }
int glue_C0FCB4_step(void) { if(!glue_C0FCB4_owns(REG_PC)) return 0; return menu_transition_step(); }
static const uint32_t owned_C0FECE[]={
    0xC0FECE,0xC0FED2,0xC0FED8,0xC0FEDA,0xC0FEE0,0xC0FEE4,0xC0FEE6,0xC0FEEA,
    0xC0FEF0,0xC0FEF2,0xC0FEF8,0xC0FEFA,0xC0FEFC,0xC0FEFE,0xC0FF04,0xC0FF06,
    0xC0FF10,0xC0FF14,0xC0FF18,0xC0FF1A,0xC0FF1E,0xC0FF20,0xC0FF22,0xC0FF24,
    0xC0FF2A,0xC0FF2C,0xC0FF2E,0xC0FF36,0xC0FF38,0xC0FF3A,0xC0FF3C,0xC0FF42,
    0xC0FF44,0xC0FF46,0xC0FF48,0xC0FF4A,0xC0FF50,0xC0FF56,0xC0FF58,0xC0FF60,
    0xC0FF64,0xC0FF6C,0xC0FF74,0xC0FF7C,0xC0FF82,0xC0FF88,0xC0FF90,0xC0FF98,
    0xC0FFA2,0xC0FFA8,0xC0FFAC,0xC0FFB2,0xC0FFBA,0xC0FFC2,0xC0FFC4,0xC0FFC8,
    0xC0FFCA,0xC0FFD0,0xC0FFD4,0xC0FFD8,0xC0FFDA,0xC0FFE2,0xC0FFEA,0xC0FFF2,
    0xC0FFFA,0xC10002,0xC1000A,0xC1000E,0xC10016,0xC1001A,0xC10020,0xC10024,
    0xC1002A,0xC10032,0xC10036,0xC1003C,0xC10040,0xC10048,0xC10050,0xC10056,
    0xC10058,0xC1005E,0xC10060,0xC10066,0xC1006C,0xC10072,0xC1007A,0xC10082,
    0xC1008A,0xC10090,0xC10094,0xC1009A,0xC1009E,0xC100A4,0xC100A8,0xC100AE,
    0xC100B2,0xC100BA,0xC100C2,0xC100CA,0xC100D2,0xC100DA,0xC100DE,0xC100E4,
    0xC100EA,0xC100F0,0xC100F6,0xC100FA,0xC10100,0xC10102,0xC1010A,0xC1010C,
    0xC10112,0xC10118,0xC1011E,0xC10124,0xC10128,0xC1012A,0xC10130,0xC10136,
    0xC10138,0xC1013E,0xC10144,0xC1014A,0xC10152,0xC10158,0xC1015C,0xC10162,
    0xC10168,0xC1016A,0xC1016C,0xC10174,0xC1017A,0xC1017C,
};
int glue_C0FECE_owns(uint32_t pc) { return owns_pc(owned_C0FECE,sizeof owned_C0FECE/sizeof owned_C0FECE[0],pc); }
int glue_C0FECE_step(void) { if(!glue_C0FECE_owns(REG_PC)) return 0; return menu_transition_step(); }
static const uint32_t owned_C0FFE2[]={
    0xC0FFE2,0xC10102,0xC1010A,0xC1010C,0xC10112,0xC10118,0xC1011E,0xC10124,
    0xC10128,0xC1012A,0xC10130,0xC10136,0xC10138,0xC1013E,0xC10144,0xC1014A,
    0xC10152,0xC10158,0xC1015C,0xC10162,0xC10168,0xC1016A,0xC1016C,0xC10174,
    0xC1017A,0xC1017C,
};
int glue_C0FFE2_owns(uint32_t pc) { return owns_pc(owned_C0FFE2,sizeof owned_C0FFE2/sizeof owned_C0FFE2[0],pc); }
int glue_C0FFE2_step(void) { if(!glue_C0FFE2_owns(REG_PC)) return 0; return menu_transition_step(); }
static const uint32_t owned_C1000A[]={
    0xC1000A,0xC1000E,0xC10016,0xC1001A,0xC10020,0xC10162,0xC10168,0xC1016A,
    0xC1016C,0xC10174,0xC1017A,0xC1017C,
};
int glue_C1000A_owns(uint32_t pc) { return owns_pc(owned_C1000A,sizeof owned_C1000A/sizeof owned_C1000A[0],pc); }
int glue_C1000A_step(void) { if(!glue_C1000A_owns(REG_PC)) return 0; return menu_transition_step(); }
static const uint32_t owned_C17C2A[]={
    0xC17C2A,0xC17C2E,0xC17C36,0xC17C38,0xC17C3C,0xC17C3E,0xC17C40,0xC17C42,
    0xC17C46,0xC17C4A,0xC17C4E,0xC17C50,0xC17C52,0xC17C54,0xC17C56,0xC17C5A,
    0xC17C5E,0xC17C60,
};
int glue_C17C2A_owns(uint32_t pc) { return owns_pc(owned_C17C2A,sizeof owned_C17C2A/sizeof owned_C17C2A[0],pc); }
int glue_C17C2A_step(void) { if(!glue_C17C2A_owns(REG_PC)) return 0; return menu_transition_step(); }
static const uint32_t owned_C24E8A[]={
    0xC24E8A,0xC24E8C,0xC24E92,0xC24E96,0xC24E9C,0xC24E9E,0xC24EA0,0xC24EA4,
    0xC24EA8,0xC24EAE,0xC24EB0,0xC24EB2,0xC24EB6,0xC24EBA,0xC24EC0,0xC24EC2,
    0xC24EC4,0xC24EC8,0xC24ECC,0xC24ED2,0xC24ED4,0xC24ED6,0xC24EDA,0xC24EDE,
    0xC24EE4,0xC24EE6,0xC24EE8,0xC24EEC,0xC24EF0,0xC24EF6,0xC24EF8,0xC24EFA,
    0xC24EFE,0xC24F02,0xC24F08,0xC24F0A,0xC24F0C,0xC24F10,0xC24F14,0xC24F1A,
    0xC24F1C,0xC24F1E,0xC24F22,0xC24F26,0xC24F2A,0xC24F2C,0xC24F2E,0xC24F34,
    0xC24F36,0xC24F38,0xC24F3C,0xC24F3E,0xC24F40,0xC24F42,0xC24F46,0xC24F48,
    0xC24F4E,0xC24F50,0xC24F52,0xC24F56,0xC24F5A,0xC24F60,0xC24F62,0xC24F64,
    0xC24F68,0xC24F6C,0xC24F72,0xC24F74,0xC24F76,0xC24F7C,0xC24F80,0xC24F86,
    0xC24F88,0xC24F8A,0xC24F8E,0xC24F94,0xC24F9A,0xC24F9C,0xC24FA2,
};
int glue_C24E8A_owns(uint32_t pc) { return owns_pc(owned_C24E8A,sizeof owned_C24E8A/sizeof owned_C24E8A[0],pc); }
int glue_C24E8A_step(void) { if(!glue_C24E8A_owns(REG_PC)) return 0; return menu_transition_step(); }
