/* Complete main loop control messages family source CPU/bus/event boundaries.
 * Readable behavior lives in main_loop_control_messages.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family main_loop_control_messages. */
#include "glue_main_loop_control_messages_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_main_loop_control_messages.h"

static int main_loop_control_messages_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC1518C:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC15190: case 0xC152CE: case 0xC152D2: case 0xC15342:
    case 0xC15346: case 0xC15354: case 0xC15358: case 0xC15366:
    case 0xC1536A: case 0xC32C32: case 0xC32D58: case 0xC32DB0:
    case 0xC32DBA: case 0xC32FCA: case 0xC32FFC: case 0xC3304E:
    case 0xC33052:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC15192: case 0xC15196: case 0xC1519A: case 0xC151B8:
    case 0xC151BC: case 0xC1520A: case 0xC15248: case 0xC15266:
    case 0xC15268: case 0xC15274: case 0xC1527E: case 0xC1529E:
    case 0xC152B8: case 0xC152C4: case 0xC152DE: case 0xC152FC:
    case 0xC15318: case 0xC15326: case 0xC1538E: case 0xC1539A:
    case 0xC153B2: case 0xC153EC: case 0xC153F6: case 0xC32BD8:
    case 0xC32BF0: case 0xC32C26: case 0xC32C3C: case 0xC32C48:
    case 0xC32C52: case 0xC32C5A: case 0xC32C5C: case 0xC32C82:
    case 0xC32C94: case 0xC32C9E: case 0xC32CFC: case 0xC32D5A:
    case 0xC32D64: case 0xC32DC4: case 0xC32DCC: case 0xC32DD4:
    case 0xC32DDC: case 0xC32DE2: case 0xC32DE4: case 0xC32DF8:
    case 0xC32E08: case 0xC32E6C: case 0xC32E74: case 0xC32EA4:
    case 0xC32ED8: case 0xC32EF6: case 0xC32F04: case 0xC32F30:
    case 0xC32F86: case 0xC32FB0: case 0xC32FB8: case 0xC32FCE:
    case 0xC3301E: case 0xC33042:
        width=1; goto move;
    case 0xC1519E: case 0xC152D6: case 0xC1534A: case 0xC1535C:
    case 0xC1536E: case 0xC32EB8: case 0xC32ED2:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC151A4: case 0xC151AC: case 0xC151DA: case 0xC15210:
    case 0xC152D0: case 0xC152D4: case 0xC15344: case 0xC15348:
    case 0xC15356: case 0xC1535A: case 0xC15368: case 0xC1536C:
    case 0xC153A4: case 0xC153CE: case 0xC32CC4: case 0xC3305E:
    case 0xC33072: case 0xC3307A: case 0xC33096: case 0xC330B2:
        width=4; goto move;
    case 0xC151B4: case 0xC151D2: case 0xC151F6: case 0xC15200:
    case 0xC1522E: case 0xC15244: case 0xC15288: case 0xC15294:
    case 0xC152A6: case 0xC152AE: case 0xC152E8: case 0xC15304:
    case 0xC15396: case 0xC153AE: case 0xC153BC: case 0xC153E2:
    case 0xC153E8: case 0xC32D34: case 0xC32D6C: case 0xC32F26:
    case 0xC32F62: case 0xC32FA6:
        width=4; goto move;
    case 0xC151BA: case 0xC152E4: case 0xC32C0E: case 0xC32C2E:
    case 0xC32E8A: case 0xC32E90: case 0xC32EEE: case 0xC32F1A:
    case 0xC32FC2: case 0xC3304A:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC151BE: case 0xC152B4: case 0xC15388: case 0xC32D6E:
    case 0xC32D74: case 0xC32D7A:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC151C2: case 0xC151F0: case 0xC151FC: case 0xC15218:
    case 0xC15232: case 0xC15254: case 0xC1528E: case 0xC1529A:
    case 0xC152AA: case 0xC152EC: case 0xC152F2: case 0xC152F4:
    case 0xC15308: case 0xC1530E: case 0xC15310: case 0xC15376:
    case 0xC1537E: case 0xC1539E: case 0xC153C0: case 0xC153C8:
    case 0xC32BF8: case 0xC32C42: case 0xC32C64: case 0xC32C6A:
    case 0xC32C8A: case 0xC32D04: case 0xC32D0E: case 0xC32D30:
    case 0xC32D48: case 0xC32D5E: case 0xC32DEC: case 0xC32EFE:
    case 0xC32F68: case 0xC32FD8: case 0xC32FDE: case 0xC32FE2:
    case 0xC32FFE: case 0xC33066: case 0xC3306C: case 0xC33086:
    case 0xC3308C: case 0xC330A2: case 0xC330A8: case 0xC330BE:
    case 0xC330C4:
        width=2; goto move;
    case 0xC151C6: case 0xC1525A:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC151CA: case 0xC32D52: case 0xC32D88: case 0xC32E2C:
    case 0xC32FC8:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC151CE: case 0xC152F8: case 0xC15314: case 0xC153A2:
    case 0xC153CC: case 0xC32D6A: case 0xC32FDA:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC151D0:
        step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC151D4: case 0xC32C72: case 0xC32D3A:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC151E0: case 0xC151E8: case 0xC15214: case 0xC15224:
    case 0xC1523C: case 0xC1524E: case 0xC152BC: case 0xC152CA:
    case 0xC1532C: case 0xC1533A: case 0xC153B4: case 0xC153DC:
    case 0xC32CEE: case 0xC32D0A: case 0xC32D14: case 0xC32E5C:
    case 0xC32E82: case 0xC32EE4: case 0xC32F12: case 0xC32F76:
    case 0xC32F94: case 0xC32FF2: case 0xC3303A: case 0xC330E8:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC151E6: case 0xC151EE: case 0xC15216: case 0xC15222:
    case 0xC15238: case 0xC15240: case 0xC152CC: case 0xC152E6:
    case 0xC1532E: case 0xC153E0: case 0xC32C0C: case 0xC32C30:
    case 0xC32CF4: case 0xC32D0C: case 0xC32D1A: case 0xC32E40:
    case 0xC32EB6: case 0xC32F10: case 0xC32FF0: case 0xC33012:
    case 0xC3304C: case 0xC33064: case 0xC33084: case 0xC330A0:
    case 0xC330BC: case 0xC330D6:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC151F4: case 0xC1537C: case 0xC32CD6: case 0xC32D44:
    case 0xC32E2E: case 0xC32E4E: case 0xC32F50: case 0xC32FFA:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC15206: case 0xC32F2C: case 0xC32F42: case 0xC32FAC:
    case 0xC3305A:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC1521E: case 0xC15234: case 0xC15330: case 0xC153C2:
    case 0xC32C04: case 0xC32C8E: case 0xC32DF2: case 0xC32E38:
    case 0xC33006: case 0xC33060: case 0xC33080: case 0xC3309C:
    case 0xC330B8: case 0xC330CE:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC1522A: case 0xC15250: case 0xC152C2: case 0xC15338:
    case 0xC15340: case 0xC153C6: case 0xC32BE4: case 0xC32C2C:
    case 0xC32C92: case 0xC32D08: case 0xC32DF6: case 0xC32E78:
    case 0xC32E80: case 0xC32F78: case 0xC32F8A: case 0xC32FA4:
    case 0xC32FD4: case 0xC3300E: case 0xC3301C: case 0xC33028:
    case 0xC33040: case 0xC33048:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC1524A: case 0xC1526C: case 0xC15278: case 0xC153EE:
    case 0xC32ECA: case 0xC33024:
        width=1; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC1525E: case 0xC32C14: case 0xC32C4E: case 0xC32E62:
    case 0xC32EA0: case 0xC32F74: case 0xC32F92: case 0xC32FD0:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC15262: case 0xC15280: case 0xC15284: case 0xC153B8:
    case 0xC32BE6: case 0xC32E9A: case 0xC32F20: case 0xC32FBC:
        width=1; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC15270:
        width=1; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC1527C:
        width=1; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC15292: case 0xC153D4: case 0xC32C3A: case 0xC32C50:
    case 0xC32D54: case 0xC3302A: case 0xC3302C: case 0xC330D8:
    case 0xC330DA: case 0xC330F0: case 0xC330F2:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC152DC: case 0xC152FA: case 0xC15316: case 0xC15350:
    case 0xC15362: case 0xC15374: case 0xC153AA: case 0xC153D2:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC152EE: case 0xC1530A:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC15302: case 0xC15324: case 0xC15352: case 0xC15364:
    case 0xC153AC: case 0xC153D8: case 0xC32BEC: case 0xC32C00:
    case 0xC32C9C: case 0xC32CB2: case 0xC32CC2: case 0xC32D56:
    case 0xC32D90: case 0xC32DC2: case 0xC32E00: case 0xC32E16:
    case 0xC32E4A: case 0xC32E58: case 0xC32EAA: case 0xC32EC6:
    case 0xC32EE0: case 0xC32EF4: case 0xC32F52: case 0xC32FCC:
    case 0xC33036: case 0xC33050: case 0xC3306A: case 0xC3308A:
    case 0xC330A6: case 0xC330C2: case 0xC330E4:
        step_branch(pc,opcode,1); break;
    case 0xC1531E: case 0xC153E6: case 0xC32CB6: case 0xC32CE6:
    case 0xC32D1E: case 0xC32D8A: case 0xC32D92: case 0xC32D98:
    case 0xC32D9E: case 0xC32DB2: case 0xC32DB4: case 0xC32DBC:
    case 0xC32E96: case 0xC32EA2: case 0xC32F38: case 0xC32F44:
    case 0xC32F46:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC15384: case 0xC32BE0: case 0xC32CCE: case 0xC32D50:
    case 0xC32E1A: case 0xC33010:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC15386:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC153A6: case 0xC153D0: case 0xC32C34: case 0xC33054:
    case 0xC33076: case 0xC33092: case 0xC330AE: case 0xC330CA:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC153B6: case 0xC32CD4: case 0xC32E54: case 0xC32E88:
    case 0xC32EEA: case 0xC32F18: case 0xC32F9A:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC153F2: case 0xC32CDE: case 0xC32EBE:
        width=1; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC153F8:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC153FA: case 0xC32C38: case 0xC32CEC: case 0xC330FC:
        REG_PC=m68ki_pull_32(); break;
    case 0xC32BD2: case 0xC32C7C: case 0xC32CA4: case 0xC32CF6:
    case 0xC32D24: case 0xC32D2A: case 0xC32D4C: case 0xC32DA4:
    case 0xC32DAA: case 0xC32E66: case 0xC32F3C: case 0xC32F5C:
    case 0xC32F80:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC32BDE: case 0xC32C3E: case 0xC32C88: case 0xC32D02:
    case 0xC32D66: case 0xC32E72: case 0xC32EFC: case 0xC32F36:
    case 0xC32FB6:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC32C16: case 0xC32C1E: case 0xC32CAA: case 0xC32E0E:
    case 0xC32E42: case 0xC32F48: case 0xC32F54: case 0xC3302E:
    case 0xC330DC: case 0xC330F4:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC32C40: case 0xC32D5C: case 0xC32D60:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC32C5E: case 0xC32C78: case 0xC32D40: case 0xC32DE6:
    case 0xC32E04: case 0xC32F7C: case 0xC32FE8:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC32C62: case 0xC32DEA:
        message_lsr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC32CBC:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC32CDC: case 0xC32D32: case 0xC32E20: case 0xC32E34:
    case 0xC32FF8: case 0xC330EE:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC32D46: case 0xC32D62: case 0xC32D68: case 0xC33058:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC32D80: case 0xC32E24: case 0xC32E7C: case 0xC32E9C:
    case 0xC32EAE: case 0xC32F0C: case 0xC32F6C: case 0xC32F8E:
    case 0xC32F9C: case 0xC32FEC: case 0xC33014:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC32DB6:
        step_dbf(pc,&D(reg)); break;
    case 0xC32FDC: case 0xC33074: case 0xC3307E: case 0xC3309A:
    case 0xC330B6:
        width=4; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC33002:
        width=2; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC33070: case 0xC33090: case 0xC330AC: case 0xC330C8:
        width=2; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
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
static const uint32_t owned_C1518C[]={
    0xC1518C,0xC15190,0xC15192,0xC15196,0xC1519A,0xC1519E,0xC151A4,0xC151AC,
    0xC151B4,0xC151B8,0xC151BA,0xC151BC,0xC151BE,0xC151C2,0xC151C6,0xC151CA,
    0xC151CE,0xC151D0,0xC151D2,0xC151D4,0xC151DA,0xC151E0,0xC151E6,0xC151E8,
    0xC151EE,0xC151F0,0xC151F4,0xC151F6,0xC151FC,0xC15200,0xC15206,0xC1520A,
    0xC15210,0xC15214,0xC15216,0xC15218,0xC1521E,0xC15222,0xC15224,0xC1522A,
    0xC1522E,0xC15232,0xC15234,0xC15238,0xC1523C,0xC15240,0xC15244,0xC15248,
    0xC1524A,0xC1524E,0xC15250,0xC15254,0xC1525A,0xC1525E,0xC15262,0xC15266,
    0xC15268,0xC1526C,0xC15270,0xC15274,0xC15278,0xC1527C,0xC1527E,0xC15280,
    0xC15284,0xC15288,0xC1528E,0xC15292,0xC15294,0xC1529A,0xC1529E,0xC152A6,
    0xC152AA,0xC152AE,0xC152B4,0xC152B8,0xC152BC,0xC152C2,0xC152C4,0xC152CA,
    0xC152CC,0xC152CE,0xC152D0,0xC152D2,0xC152D4,0xC152D6,0xC152DC,0xC152DE,
    0xC152E4,0xC152E6,0xC152E8,0xC152EC,0xC152EE,0xC152F2,0xC152F4,0xC152F8,
    0xC152FA,0xC152FC,0xC15302,0xC15304,0xC15308,0xC1530A,0xC1530E,0xC15310,
    0xC15314,0xC15316,0xC15318,0xC1531E,0xC15324,0xC15326,0xC1532C,0xC1532E,
    0xC15330,0xC15338,0xC1533A,0xC15340,0xC15342,0xC15344,0xC15346,0xC15348,
    0xC1534A,0xC15350,0xC15352,0xC15354,0xC15356,0xC15358,0xC1535A,0xC1535C,
    0xC15362,0xC15364,0xC15366,0xC15368,0xC1536A,0xC1536C,0xC1536E,0xC15374,
    0xC15376,0xC1537C,0xC1537E,0xC15384,0xC15386,0xC15388,0xC1538E,0xC15396,
    0xC1539A,0xC1539E,0xC153A2,0xC153A4,0xC153A6,0xC153AA,0xC153AC,0xC153AE,
    0xC153B2,0xC153B4,0xC153B6,0xC153B8,0xC153BC,0xC153C0,0xC153C2,0xC153C6,
    0xC153C8,0xC153CC,0xC153CE,0xC153D0,0xC153D2,0xC153D4,0xC153D8,0xC153DC,
    0xC153E0,0xC153E2,0xC153E6,0xC153E8,0xC153EC,0xC153EE,0xC153F2,0xC153F6,
    0xC153F8,0xC153FA,
};
int glue_C1518C_owns(uint32_t pc) { return owns_pc(owned_C1518C,sizeof owned_C1518C/sizeof owned_C1518C[0],pc); }
int glue_C1518C_step(void) { if(!glue_C1518C_owns(REG_PC)) return 0; return main_loop_control_messages_step(); }
static const uint32_t owned_C32CEE[]={
    0xC32BD2,0xC32BD8,0xC32BDE,0xC32BE0,0xC32BE4,0xC32BE6,0xC32BEC,0xC32BF0,
    0xC32BF8,0xC32C00,0xC32C04,0xC32C0C,0xC32C0E,0xC32C14,0xC32C16,0xC32C1E,
    0xC32C26,0xC32C2C,0xC32C2E,0xC32C30,0xC32C32,0xC32C34,0xC32C38,0xC32C3A,
    0xC32C3C,0xC32C3E,0xC32C40,0xC32C42,0xC32C48,0xC32C4E,0xC32C50,0xC32C52,
    0xC32C5A,0xC32C5C,0xC32C5E,0xC32C62,0xC32C64,0xC32C6A,0xC32C72,0xC32C78,
    0xC32C7C,0xC32C82,0xC32C88,0xC32C8A,0xC32C8E,0xC32C92,0xC32C94,0xC32C9C,
    0xC32C9E,0xC32CA4,0xC32CAA,0xC32CB2,0xC32CB6,0xC32CBC,0xC32CC2,0xC32CC4,
    0xC32CCE,0xC32CD4,0xC32CD6,0xC32CDC,0xC32CDE,0xC32CE6,0xC32CEC,0xC32CEE,
    0xC32CF4,0xC32CF6,0xC32CFC,0xC32D02,0xC32D04,0xC32D08,0xC32D0A,0xC32D0C,
    0xC32D0E,0xC32D14,0xC32D1A,0xC32D1E,0xC32D24,0xC32D2A,0xC32D30,0xC32D32,
    0xC32D34,0xC32D3A,0xC32D40,0xC32D44,0xC32D46,0xC32D48,0xC32D4C,0xC32D50,
    0xC32D52,0xC32D54,0xC32D56,0xC32D58,0xC32D5A,0xC32D5C,0xC32D5E,0xC32D60,
    0xC32D62,0xC32D64,0xC32D66,0xC32D68,0xC32D6A,0xC32D6C,0xC32D6E,0xC32D74,
    0xC32D7A,0xC32D80,0xC32D88,0xC32D8A,0xC32D90,0xC32D92,0xC32D98,0xC32D9E,
    0xC32DA4,0xC32DAA,0xC32DB0,0xC32DB2,0xC32DB4,0xC32DB6,0xC32DBA,0xC32DBC,
    0xC32DC2,0xC32DC4,0xC32DCC,0xC32DD4,0xC32DDC,0xC32DE2,0xC32DE4,0xC32DE6,
    0xC32DEA,0xC32DEC,0xC32DF2,0xC32DF6,0xC32DF8,0xC32E00,0xC32E04,0xC32E08,
    0xC32E0E,0xC32E16,0xC32E1A,0xC32E20,0xC32E24,0xC32E2C,0xC32E2E,0xC32E34,
    0xC32E38,0xC32E40,0xC32E42,0xC32E4A,0xC32E4E,0xC32E54,0xC32E58,0xC32E5C,
    0xC32E62,0xC32E66,0xC32E6C,0xC32E72,0xC32E74,0xC32E78,0xC32E7C,0xC32E80,
    0xC32E82,0xC32E88,0xC32E8A,0xC32E90,0xC32E96,0xC32E9A,0xC32E9C,0xC32EA0,
    0xC32EA2,0xC32EA4,0xC32EAA,0xC32EAE,0xC32EB6,0xC32EB8,0xC32EBE,0xC32EC6,
    0xC32ECA,0xC32ED2,0xC32ED8,0xC32EE0,0xC32EE4,0xC32EEA,0xC32EEE,0xC32EF4,
    0xC32EF6,0xC32EFC,0xC32EFE,0xC32F04,0xC32F0C,0xC32F10,0xC32F12,0xC32F18,
    0xC32F1A,0xC32F20,0xC32F26,0xC32F2C,0xC32F30,0xC32F36,0xC32F38,0xC32F3C,
    0xC32F42,0xC32F44,0xC32F46,0xC32F48,0xC32F50,0xC32F52,0xC32F54,0xC32F5C,
    0xC32F62,0xC32F68,0xC32F6C,0xC32F74,0xC32F76,0xC32F78,0xC32F7C,0xC32F80,
    0xC32F86,0xC32F8A,0xC32F8E,0xC32F92,0xC32F94,0xC32F9A,0xC32F9C,0xC32FA4,
    0xC32FA6,0xC32FAC,0xC32FB0,0xC32FB6,0xC32FB8,0xC32FBC,0xC32FC2,0xC32FC8,
    0xC32FCA,0xC32FCC,0xC32FCE,0xC32FD0,0xC32FD4,0xC32FD8,0xC32FDA,0xC32FDC,
    0xC32FDE,0xC32FE2,0xC32FE8,0xC32FEC,0xC32FF0,0xC32FF2,0xC32FF8,0xC32FFA,
    0xC32FFC,0xC32FFE,0xC33002,0xC33006,0xC3300E,0xC33010,0xC33012,0xC33014,
    0xC3301C,0xC3301E,0xC33024,0xC33028,0xC3302A,0xC3302C,0xC3302E,0xC33036,
    0xC3303A,0xC33040,0xC33042,0xC33048,0xC3304A,0xC3304C,0xC3304E,0xC33050,
    0xC33052,0xC33054,0xC33058,0xC3305A,0xC3305E,0xC33060,0xC33064,0xC33066,
    0xC3306A,0xC3306C,0xC33070,0xC33072,0xC33074,0xC33076,0xC3307A,0xC3307E,
    0xC33080,0xC33084,0xC33086,0xC3308A,0xC3308C,0xC33090,0xC33092,0xC33096,
    0xC3309A,0xC3309C,0xC330A0,0xC330A2,0xC330A6,0xC330A8,0xC330AC,0xC330AE,
    0xC330B2,0xC330B6,0xC330B8,0xC330BC,0xC330BE,0xC330C2,0xC330C4,0xC330C8,
    0xC330CA,0xC330CE,0xC330D6,0xC330D8,0xC330DA,0xC330DC,0xC330E4,0xC330E8,
    0xC330EE,0xC330F0,0xC330F2,0xC330F4,0xC330FC,
};
int glue_C32CEE_owns(uint32_t pc) { return owns_pc(owned_C32CEE,sizeof owned_C32CEE/sizeof owned_C32CEE[0],pc); }
int glue_C32CEE_step(void) { if(!glue_C32CEE_owns(REG_PC)) return 0; return main_loop_control_messages_step(); }
