/* Complete input display setup family source CPU/bus/event boundaries.
 * Readable behavior lives in input_display_setup.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family input_display_setup. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_input_display_setup.h"

static int input_display_setup_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC1612C: case 0xC16D4C: case 0xC16FF4: case 0xC17066:
    case 0xC1787A:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC16130: case 0xC16170: case 0xC16188: case 0xC161AC:
    case 0xC161B2: case 0xC161C2: case 0xC161D0: case 0xC161E8:
    case 0xC161F8: case 0xC16206: case 0xC16248: case 0xC16260:
    case 0xC16DBC: case 0xC17886: case 0xC178A4: case 0xC1799A:
    case 0xC179F2: case 0xC17A4C: case 0xC17A9A:
        address=cache_step_address(mode,reg,4); m68ki_push_32(address); break;
    case 0xC16136: case 0xC16176: case 0xC1618E: case 0xC16196:
    case 0xC161B8: case 0xC161C8: case 0xC161D6: case 0xC161EE:
    case 0xC161FE: case 0xC1620C: case 0xC1624E: case 0xC16266:
    case 0xC16D60: case 0xC16D76: case 0xC16D84: case 0xC16D9C:
    case 0xC16DA8: case 0xC16DC2: case 0xC16DDA: case 0xC16DE8:
    case 0xC16DF4: case 0xC16E1A: case 0xC16E28: case 0xC16E34:
    case 0xC16E4A: case 0xC16E58: case 0xC16E64: case 0xC16EA2:
    case 0xC17030: case 0xC1703E: case 0xC1704C: case 0xC170A6:
    case 0xC1788C: case 0xC178AA: case 0xC178CA: case 0xC1791C:
    case 0xC17934: case 0xC1795A: case 0xC17972: case 0xC179A0:
    case 0xC179C4: case 0xC179F8: case 0xC17A28: case 0xC17A52:
    case 0xC17A64: case 0xC17AA0: case 0xC17AB4:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC1613C: case 0xC1617C: case 0xC16194: case 0xC161CE:
    case 0xC161DC: case 0xC16204: case 0xC16212: case 0xC16254:
    case 0xC16D66: case 0xC16D7C: case 0xC16D8A: case 0xC16DA2:
    case 0xC16DAE: case 0xC16DE0: case 0xC16DEE: case 0xC16DFA:
    case 0xC16E0E: case 0xC16E20: case 0xC16E2E: case 0xC16E3A:
    case 0xC16E50: case 0xC16E5E: case 0xC16E6A: case 0xC16EA8:
    case 0xC17036: case 0xC17044: case 0xC17052: case 0xC170AC:
    case 0xC17892: case 0xC178B0: case 0xC178D0: case 0xC17922:
    case 0xC1793A: case 0xC17960: case 0xC17978: case 0xC179A6:
    case 0xC179CA: case 0xC179FE: case 0xC17A2E: case 0xC17A58:
    case 0xC17A6A: case 0xC17AA6: case 0xC17ABA:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC1613E: case 0xC16144: case 0xC1615A: case 0xC1623C:
    case 0xC16270: case 0xC1627A: case 0xC16DCC: case 0xC16E72:
    case 0xC16FFE: case 0xC1701E: case 0xC17070: case 0xC17090:
    case 0xC1709C: case 0xC170A0: case 0xC17898: case 0xC178B6:
        width=2; goto move;
    case 0xC16148: case 0xC1615E: case 0xC17060:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC1614A: case 0xC16160:
        step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC1614C: case 0xC16162: case 0xC16E6C: case 0xC16E78:
    case 0xC16E88: case 0xC16E96: case 0xC16FF8: case 0xC17006:
    case 0xC17010: case 0xC17022: case 0xC17054: case 0xC1706A:
    case 0xC17078: case 0xC17086: case 0xC178D8: case 0xC178E2:
    case 0xC17928: case 0xC1793C: case 0xC17966: case 0xC1797A:
    case 0xC179D0: case 0xC17A06: case 0xC17A34: case 0xC17A72:
    case 0xC17A7C: case 0xC17AC0: case 0xC17ACC: case 0xC17ADC:
        width=4; goto move;
    case 0xC1614E: case 0xC16164:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC16154: case 0xC1616A: case 0xC161AA: case 0xC161E0:
    case 0xC161E2: case 0xC16258: case 0xC1625A: case 0xC16D50:
    case 0xC16D5C: case 0xC16D5E: case 0xC16D68: case 0xC16D74:
    case 0xC16D7E: case 0xC16D8C: case 0xC16D96: case 0xC16DA6:
    case 0xC16DB2: case 0xC16DBA: case 0xC16DD4: case 0xC16DE2:
    case 0xC16DF2: case 0xC16DFC: case 0xC16E08: case 0xC16E14:
    case 0xC16E22: case 0xC16E32: case 0xC16E44: case 0xC16E52:
    case 0xC16E62: case 0xC16E7E: case 0xC16E8E: case 0xC16E9C:
    case 0xC16EA0: case 0xC1700C: case 0xC17016: case 0xC1702A:
    case 0xC17038: case 0xC17046: case 0xC1707E: case 0xC1708C:
    case 0xC170A4: case 0xC17884: case 0xC178A2: case 0xC178C4:
    case 0xC178C8: case 0xC178DE: case 0xC178E8: case 0xC178F2:
    case 0xC17908: case 0xC17916: case 0xC1791A: case 0xC1792E:
    case 0xC17932: case 0xC17942: case 0xC1794C: case 0xC17952:
    case 0xC17954: case 0xC1796C: case 0xC17970: case 0xC17982:
    case 0xC17986: case 0xC17998: case 0xC179BE: case 0xC179C2:
    case 0xC179D6: case 0xC179DE: case 0xC179F0: case 0xC17A0C:
    case 0xC17A22: case 0xC17A26: case 0xC17A3A: case 0xC17A60:
    case 0xC17A78: case 0xC17A82: case 0xC17A86: case 0xC17A98:
    case 0xC17AAE: case 0xC17AB2: case 0xC17AC8: case 0xC17AD2:
    case 0xC17AE2: case 0xC17AF8:
        width=4; goto move;
    case 0xC1617E: case 0xC161A2: case 0xC16226:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC16184: case 0xC1622C: case 0xC16DD2: case 0xC16E12:
    case 0xC16E42: case 0xC17896: case 0xC178B4: case 0xC178C0:
    case 0xC178D4: case 0xC17926: case 0xC17964: case 0xC179AA:
    case 0xC179BA: case 0xC179CE: case 0xC17A02: case 0xC17A1E:
    case 0xC17A32: case 0xC17A5C: case 0xC17A6E: case 0xC17AAA:
    case 0xC17ABE:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC1619C: case 0xC16214: case 0xC1621C: case 0xC1622E:
    case 0xC16236: case 0xC17028: case 0xC1705A:
        width=1; goto move;
    case 0xC161A4:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC161A8: case 0xC161DE: case 0xC16256: case 0xC16276:
    case 0xC16D5A: case 0xC16D72: case 0xC16DA4: case 0xC16DB8:
    case 0xC16DF0: case 0xC16E06: case 0xC16E30: case 0xC16E60:
    case 0xC16E86: case 0xC17004: case 0xC17076: case 0xC1709A:
    case 0xC17882: case 0xC178A0: case 0xC178C2: case 0xC178C6:
    case 0xC178D6: case 0xC17914: case 0xC17918: case 0xC1794A:
    case 0xC17950: case 0xC17980: case 0xC17996: case 0xC179BC:
    case 0xC179C0: case 0xC179EE: case 0xC17A04: case 0xC17A20:
    case 0xC17A24: case 0xC17A5E: case 0xC17A70: case 0xC17A96:
    case 0xC17AAC: case 0xC17AB0: case 0xC17AC6:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC161BE: case 0xC161F4: case 0xC1626C: case 0xC16DC8:
    case 0xC17082:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC1621A: case 0xC16234:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC16222: case 0xC1789E:
        step_branch(pc,opcode,1); break;
    case 0xC16242:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC16246: case 0xC16D70: case 0xC16D94:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC16278:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC16280: case 0xC16EAA: case 0xC17062: case 0xC170AE:
    case 0xC17B04:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC16282: case 0xC16EAC: case 0xC17064: case 0xC170B0:
    case 0xC17B06:
        REG_PC=m68ki_pull_32(); break;
    case 0xC16D6E: case 0xC16D92: case 0xC16E10: case 0xC16E40:
    case 0xC17894: case 0xC178B2: case 0xC178D2: case 0xC17924:
    case 0xC17962: case 0xC179A8: case 0xC179B4: case 0xC179CC:
    case 0xC17A00: case 0xC17A18: case 0xC17A30: case 0xC17A5A:
    case 0xC17A6C: case 0xC17AA8: case 0xC17ABC:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC16DB0: case 0xC17A4A: case 0xC17A62:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC16DD0: case 0xC178BC:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC16E0A: case 0xC16E3C:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC16E92:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC1705E:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC17096: case 0xC1787E:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC178EC: case 0xC17AD6:
        width=4; value=m68ki_read_imm_32(); operation='+'; goto arithmetic;
    case 0xC178F6: case 0xC17AE6:
        width=4; value=m68ki_read_imm_32(); operation='&'; goto immediate_logic;
    case 0xC178FC: case 0xC17AEC:
        width=4; value=m68ki_read_imm_32(); operation='-'; goto arithmetic;
    case 0xC17902: case 0xC17AF2:
        width=4; value=m68ki_read_imm_32(); operation='|'; goto immediate_logic;
    case 0xC1790C: case 0xC1798E: case 0xC179AC: case 0xC179E6:
    case 0xC17A10: case 0xC17A42: case 0xC17A8E: case 0xC17AFC:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='|'; goto bit_value;
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
static const uint32_t owned_C16D4C[]={
    0xC16D4C,0xC16D50,0xC16D5A,0xC16D5C,0xC16D5E,0xC16D60,0xC16D66,0xC16D68,
    0xC16D6E,0xC16D70,0xC16D72,0xC16D74,0xC16D76,0xC16D7C,0xC16D7E,0xC16D84,
    0xC16D8A,0xC16D8C,0xC16D92,0xC16D94,0xC16D96,0xC16D9C,0xC16DA2,0xC16DA4,
    0xC16DA6,0xC16DA8,0xC16DAE,0xC16DB0,0xC16DB2,0xC16DB8,0xC16DBA,0xC16DBC,
    0xC16DC2,0xC16DC8,0xC16DCC,0xC16DD0,0xC16DD2,0xC16DD4,0xC16DDA,0xC16DE0,
    0xC16DE2,0xC16DE8,0xC16DEE,0xC16DF0,0xC16DF2,0xC16DF4,0xC16DFA,0xC16DFC,
    0xC16E06,0xC16E08,0xC16E0A,0xC16E0E,0xC16E10,0xC16E12,0xC16E14,0xC16E1A,
    0xC16E20,0xC16E22,0xC16E28,0xC16E2E,0xC16E30,0xC16E32,0xC16E34,0xC16E3A,
    0xC16E3C,0xC16E40,0xC16E42,0xC16E44,0xC16E4A,0xC16E50,0xC16E52,0xC16E58,
    0xC16E5E,0xC16E60,0xC16E62,0xC16E64,0xC16E6A,0xC16E6C,0xC16E72,0xC16E78,
    0xC16E7E,0xC16E86,0xC16E88,0xC16E8E,0xC16E92,0xC16E96,0xC16E9C,0xC16EA0,
    0xC16EA2,0xC16EA8,0xC16EAA,0xC16EAC,
};
int glue_C16D4C_owns(uint32_t pc) { return owns_pc(owned_C16D4C,sizeof owned_C16D4C/sizeof owned_C16D4C[0],pc); }
int glue_C16D4C_step(void) { if(!glue_C16D4C_owns(REG_PC)) return 0; return input_display_setup_step(); }
static const uint32_t owned_C16FF4[]={
    0xC16FF4,0xC16FF8,0xC16FFE,0xC17004,0xC17006,0xC1700C,0xC17010,0xC17016,
    0xC1701E,0xC17022,0xC17028,0xC1702A,0xC17030,0xC17036,0xC17038,0xC1703E,
    0xC17044,0xC17046,0xC1704C,0xC17052,0xC17054,0xC1705A,0xC1705E,0xC17060,
    0xC17062,0xC17064,
};
int glue_C16FF4_owns(uint32_t pc) { return owns_pc(owned_C16FF4,sizeof owned_C16FF4/sizeof owned_C16FF4[0],pc); }
int glue_C16FF4_step(void) { if(!glue_C16FF4_owns(REG_PC)) return 0; return input_display_setup_step(); }
static const uint32_t owned_C17066[]={
    0xC17066,0xC1706A,0xC17070,0xC17076,0xC17078,0xC1707E,0xC17082,0xC17086,
    0xC1708C,0xC17090,0xC17096,0xC1709A,0xC1709C,0xC170A0,0xC170A4,0xC170A6,
    0xC170AC,0xC170AE,0xC170B0,
};
int glue_C17066_owns(uint32_t pc) { return owns_pc(owned_C17066,sizeof owned_C17066/sizeof owned_C17066[0],pc); }
int glue_C17066_step(void) { if(!glue_C17066_owns(REG_PC)) return 0; return input_display_setup_step(); }
static const uint32_t owned_C1787A[]={
    0xC1787A,0xC1787E,0xC17882,0xC17884,0xC17886,0xC1788C,0xC17892,0xC17894,
    0xC17896,0xC17898,0xC1789E,0xC178A0,0xC178A2,0xC178A4,0xC178AA,0xC178B0,
    0xC178B2,0xC178B4,0xC178B6,0xC178BC,0xC178C0,0xC178C2,0xC178C4,0xC178C6,
    0xC178C8,0xC178CA,0xC178D0,0xC178D2,0xC178D4,0xC178D6,0xC178D8,0xC178DE,
    0xC178E2,0xC178E8,0xC178EC,0xC178F2,0xC178F6,0xC178FC,0xC17902,0xC17908,
    0xC1790C,0xC17914,0xC17916,0xC17918,0xC1791A,0xC1791C,0xC17922,0xC17924,
    0xC17926,0xC17928,0xC1792E,0xC17932,0xC17934,0xC1793A,0xC1793C,0xC17942,
    0xC1794A,0xC1794C,0xC17950,0xC17952,0xC17954,0xC1795A,0xC17960,0xC17962,
    0xC17964,0xC17966,0xC1796C,0xC17970,0xC17972,0xC17978,0xC1797A,0xC17980,
    0xC17982,0xC17986,0xC1798E,0xC17996,0xC17998,0xC1799A,0xC179A0,0xC179A6,
    0xC179A8,0xC179AA,0xC179AC,0xC179B4,0xC179BA,0xC179BC,0xC179BE,0xC179C0,
    0xC179C2,0xC179C4,0xC179CA,0xC179CC,0xC179CE,0xC179D0,0xC179D6,0xC179DE,
    0xC179E6,0xC179EE,0xC179F0,0xC179F2,0xC179F8,0xC179FE,0xC17A00,0xC17A02,
    0xC17A04,0xC17A06,0xC17A0C,0xC17A10,0xC17A18,0xC17A1E,0xC17A20,0xC17A22,
    0xC17A24,0xC17A26,0xC17A28,0xC17A2E,0xC17A30,0xC17A32,0xC17A34,0xC17A3A,
    0xC17A42,0xC17A4A,0xC17A4C,0xC17A52,0xC17A58,0xC17A5A,0xC17A5C,0xC17A5E,
    0xC17A60,0xC17A62,0xC17A64,0xC17A6A,0xC17A6C,0xC17A6E,0xC17A70,0xC17A72,
    0xC17A78,0xC17A7C,0xC17A82,0xC17A86,0xC17A8E,0xC17A96,0xC17A98,0xC17A9A,
    0xC17AA0,0xC17AA6,0xC17AA8,0xC17AAA,0xC17AAC,0xC17AAE,0xC17AB0,0xC17AB2,
    0xC17AB4,0xC17ABA,0xC17ABC,0xC17ABE,0xC17AC0,0xC17AC6,0xC17AC8,0xC17ACC,
    0xC17AD2,0xC17AD6,0xC17ADC,0xC17AE2,0xC17AE6,0xC17AEC,0xC17AF2,0xC17AF8,
    0xC17AFC,0xC17B04,0xC17B06,
};
int glue_C1787A_owns(uint32_t pc) { return owns_pc(owned_C1787A,sizeof owned_C1787A/sizeof owned_C1787A[0],pc); }
int glue_C1787A_step(void) { if(!glue_C1787A_owns(REG_PC)) return 0; return input_display_setup_step(); }
static const uint32_t owned_C1612C[]={
    0xC1612C,0xC16130,0xC16136,0xC1613C,0xC1613E,0xC16144,0xC16148,0xC1614A,
    0xC1614C,0xC1614E,0xC16154,0xC1615A,0xC1615E,0xC16160,0xC16162,0xC16164,
    0xC1616A,0xC16170,0xC16176,0xC1617C,0xC1617E,0xC16184,0xC16188,0xC1618E,
    0xC16194,0xC16196,0xC1619C,0xC161A2,0xC161A4,0xC161A8,0xC161AA,0xC161AC,
    0xC161B2,0xC161B8,0xC161BE,0xC161C2,0xC161C8,0xC161CE,0xC161D0,0xC161D6,
    0xC161DC,0xC161DE,0xC161E0,0xC161E2,0xC161E8,0xC161EE,0xC161F4,0xC161F8,
    0xC161FE,0xC16204,0xC16206,0xC1620C,0xC16212,0xC16214,0xC1621A,0xC1621C,
    0xC16222,0xC16226,0xC1622C,0xC1622E,0xC16234,0xC16236,0xC1623C,0xC16242,
    0xC16246,0xC16248,0xC1624E,0xC16254,0xC16256,0xC16258,0xC1625A,0xC16260,
    0xC16266,0xC1626C,0xC16270,0xC16276,0xC16278,0xC1627A,0xC16280,0xC16282,
};
int glue_C1612C_owns(uint32_t pc) { return owns_pc(owned_C1612C,sizeof owned_C1612C/sizeof owned_C1612C[0],pc); }
int glue_C1612C_step(void) { if(!glue_C1612C_owns(REG_PC)) return 0; return input_display_setup_step(); }
