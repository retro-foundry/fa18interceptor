/* Complete face list parents family source CPU/bus/event boundaries.
 * Readable behavior lives in face_list_parents.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family face_list_parents. */
#include "glue_face_list_parents_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_face_list_parents.h"

static int face_list_parents_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC1FF0A: case 0xC1FF10: case 0xC20002: case 0xC20008:
    case 0xC2005C: case 0xC20066: case 0xC20070: case 0xC2007C:
    case 0xC2008A: case 0xC2009C: case 0xC20118: case 0xC2011E:
    case 0xC2089A: case 0xC208AC: case 0xC20A70: case 0xC20A88:
    case 0xC20AE8: case 0xC20B14: case 0xC20BAA: case 0xC20C5A:
    case 0xC20CA0: case 0xC20CD4: case 0xC21096: case 0xC2109C:
    case 0xC210A2: case 0xC210AC: case 0xC210B6: case 0xC210C0:
    case 0xC219AE: case 0xC219FA: case 0xC21C2E: case 0xC21C44:
        A(destination)=(mode==7 && reg==0)?(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16():cache_step_address(mode,reg,4); break;
    case 0xC1FF16: case 0xC20014: case 0xC20030: case 0xC20040:
    case 0xC20056: case 0xC20862: case 0xC20886: case 0xC20A68:
    case 0xC20A84: case 0xC20AA0: case 0xC20AAC: case 0xC20AC2:
    case 0xC20AC8: case 0xC20ADE: case 0xC20AFC: case 0xC20B0E:
    case 0xC20B1A: case 0xC20B3C: case 0xC20B44: case 0xC20B48:
    case 0xC20B6C: case 0xC20B92: case 0xC20BA4: case 0xC20BB0:
    case 0xC20BD4: case 0xC20BDC: case 0xC20BE2: case 0xC20C06:
    case 0xC20C52: case 0xC20C66: case 0xC20C82: case 0xC20C88:
    case 0xC20C96: case 0xC20CBC: case 0xC20CCE: case 0xC20CDA:
    case 0xC20CFC: case 0xC20D04: case 0xC20D14: case 0xC20D38:
    case 0xC21078: case 0xC21092: case 0xC219B8: case 0xC219BE:
    case 0xC219CC: case 0xC219D6: case 0xC219E2: case 0xC219F4:
    case 0xC21C4C: case 0xC21C64: case 0xC21C7C:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,2,(int)reg); else { address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); } break;
    case 0xC1FF1A: case 0xC1FF22: case 0xC1FF2A: case 0xC2000E:
    case 0xC20018: case 0xC20020: case 0xC20028: case 0xC20074:
    case 0xC20080: case 0xC2008E: case 0xC200A0: case 0xC20114:
    case 0xC20128: case 0xC20134: case 0xC20144: case 0xC20158:
    case 0xC2018E: case 0xC20A40: case 0xC20AEE: case 0xC20C26:
    case 0xC20CA6: case 0xC21084: case 0xC210A6: case 0xC210B0:
    case 0xC210BA: case 0xC210C4: case 0xC210CC:
        width=4; goto move;
    case 0xC1FF1E: case 0xC1FF26: case 0xC1FF2E: case 0xC1FF34:
    case 0xC2001C: case 0xC20024: case 0xC2002C: case 0xC20036:
    case 0xC2006E: case 0xC20076: case 0xC20078: case 0xC2007A:
    case 0xC20082: case 0xC20086: case 0xC20090: case 0xC200A2:
    case 0xC200A8: case 0xC200B0: case 0xC200C0: case 0xC200C8:
    case 0xC200D0: case 0xC200D2: case 0xC200DC: case 0xC200FA:
    case 0xC20110: case 0xC20126: case 0xC2012C: case 0xC20130:
    case 0xC20132: case 0xC20138: case 0xC20140: case 0xC20148:
    case 0xC2015C: case 0xC20166: case 0xC2016E: case 0xC2017A:
    case 0xC20182: case 0xC20186: case 0xC20188: case 0xC2084A:
    case 0xC2084C: case 0xC20858: case 0xC20866: case 0xC20872:
    case 0xC2087C: case 0xC208B2: case 0xC208BE: case 0xC208C4:
    case 0xC20A4A: case 0xC20A52: case 0xC20A5A: case 0xC20A5E:
    case 0xC20A62: case 0xC20A78: case 0xC20A7C: case 0xC20B2A:
    case 0xC20B2C: case 0xC20B2E: case 0xC20B5A: case 0xC20B5C:
    case 0xC20B5E: case 0xC20BC2: case 0xC20BC4: case 0xC20BC6:
    case 0xC20BF4: case 0xC20BF6: case 0xC20BF8: case 0xC20C1C:
    case 0xC20C30: case 0xC20C3C: case 0xC20C44: case 0xC20C48:
    case 0xC20C4C: case 0xC20C62: case 0xC20CB0: case 0xC20CEA:
    case 0xC20CEC: case 0xC20CEE: case 0xC20D26: case 0xC20D28:
    case 0xC20D2A: case 0xC20D62: case 0xC21064: case 0xC2106C:
    case 0xC21070: case 0xC2108E: case 0xC210A8: case 0xC210AA:
    case 0xC210B2: case 0xC210BC: case 0xC210C6: case 0xC210E0:
    case 0xC219B4: case 0xC219B6: case 0xC21A14: case 0xC21C34:
        width=2; goto move;
    case 0xC1FF32: case 0xC200AE: case 0xC2016C:
        width=2; goto move;
    case 0xC1FF36: case 0xC200B8: case 0xC200E6: case 0xC20172:
    case 0xC20190: case 0xC2085C: case 0xC20880: case 0xC20B74:
    case 0xC20C0A: case 0xC20D40: case 0xC210CE:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC1FF3C: case 0xC20848:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC1FF3E: case 0xC1FF44: case 0xC20054: case 0xC200F0:
    case 0xC200F4: case 0xC200FE: case 0xC20828: case 0xC208CE:
    case 0xC208D2: case 0xC20C20: case 0xC20D66: case 0xC210E4:
    case 0xC21A1E: case 0xC21C84:
        REG_PC=m68ki_pull_32(); break;
    case 0xC1FF40: case 0xC20116: case 0xC20830: case 0xC20834:
    case 0xC20A76: case 0xC20B40: case 0xC20BD8: case 0xC20C60:
    case 0xC20D00: case 0xC219C4: case 0xC219F2: case 0xC21A00:
    case 0xC21A1A: case 0xC21C3E: case 0xC21C4A:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC20038: case 0xC20046: case 0xC2004E: case 0xC20084:
    case 0xC20092: case 0xC200A4: case 0xC2013C: case 0xC2014C:
    case 0xC20160: case 0xC210B4: case 0xC210BE: case 0xC210C8:
        width=2; if(opcode&0x100u) { value=D(destination); operation='&'; goto immediate_logic; } value=cache_step_read(mode,reg,2)&D(destination); cache_step_write(0,destination,2,value); cache_step_logic(value,2); break;
    case 0xC2003A: case 0xC2003C: case 0xC2003E: case 0xC20048:
    case 0xC2004A: case 0xC2004C: case 0xC2084E: case 0xC20850:
    case 0xC2086A: case 0xC20876: case 0xC20A8C: case 0xC20A8E:
    case 0xC20A92: case 0xC20A96: case 0xC20A98: case 0xC20A9C:
    case 0xC20AB0: case 0xC20AB2: case 0xC20AB4: case 0xC20ACC:
    case 0xC20ACE: case 0xC20AD0: case 0xC20BBC: case 0xC20BBE:
    case 0xC20BC0: case 0xC20BC8: case 0xC20BCC: case 0xC20BD0:
    case 0xC20BEE: case 0xC20BF0: case 0xC20BF2: case 0xC20BFA:
    case 0xC20BFE: case 0xC20C02: case 0xC20C6A: case 0xC20C6E:
    case 0xC20C72: case 0xC20C76: case 0xC20C7A: case 0xC20C7E:
    case 0xC20C8C: case 0xC20C8E: case 0xC20C92: case 0xC219C6:
    case 0xC219C8: case 0xC219CA: case 0xC21A18: case 0xC21C52:
    case 0xC21C54: case 0xC21C56: case 0xC21C6A: case 0xC21C6C:
    case 0xC21C6E:
        width=2; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC20050: case 0xC20894:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC20052: case 0xC2006C: case 0xC200F2: case 0xC20124:
    case 0xC20826: case 0xC208CC: case 0xC208D0: case 0xC21A1C:
    case 0xC21C82:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC2005A: case 0xC20096: case 0xC200CA: case 0xC20152:
    case 0xC20184: case 0xC2019E: case 0xC20A50: case 0xC20AFA:
    case 0xC20B90: case 0xC20C36: case 0xC20CAC: case 0xC20CBA:
    case 0xC210DA:
        step_branch(pc,opcode,1); break;
    case 0xC20062: case 0xC20104: case 0xC20108: case 0xC20856:
    case 0xC20870: case 0xC20A66: case 0xC20AE4: case 0xC20AF4:
    case 0xC20AF6: case 0xC20AF8: case 0xC20B8A: case 0xC20B8C:
    case 0xC20B8E: case 0xC20C50: case 0xC20C9C: case 0xC20CB4:
    case 0xC20CB6: case 0xC20CB8: case 0xC21074: case 0xC21076:
    case 0xC21080:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC20064: case 0xC20094: case 0xC200B4: case 0xC200CC:
    case 0xC200DA: case 0xC20150: case 0xC20CAE:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC20088: case 0xC200A6: case 0xC200B2: case 0xC20112:
    case 0xC20142: case 0xC20164: case 0xC20170: case 0xC208CA:
    case 0xC21090: case 0xC210CA:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC20098: case 0xC200C2: case 0xC200D4: case 0xC20154:
    case 0xC2017C:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC200BE: case 0xC200C6: case 0xC200D8: case 0xC20178:
    case 0xC20180:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC200E2: case 0xC200EC: case 0xC200F6: case 0xC20100:
    case 0xC20A80: case 0xC20B70: case 0xC20B7E: case 0xC20C18:
    case 0xC20C22: case 0xC20C38: case 0xC20D3C: case 0xC20D4A:
    case 0xC20D5E: case 0xC21060: case 0xC210DC:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC2010E: case 0xC2019A: case 0xC2019C: case 0xC2082A:
    case 0xC210D8:
        width=4; goto move;
    case 0xC20196: case 0xC20B7A: case 0xC20D46: case 0xC210D4:
        width=2; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC20836: case 0xC208A0:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC20840: case 0xC208BC:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC20842:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC20852: case 0xC20854:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC2088A: case 0xC2088C: case 0xC2088E:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC20890: case 0xC20892:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC20896:
        renderer_negate(&D(reg),4); break;
    case 0xC20898:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC208AA: case 0xC20B86: case 0xC20C14: case 0xC20D52:
    case 0xC20D5A:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC208B6: case 0xC20AA6: case 0xC20AA8: case 0xC20AAA:
    case 0xC20B1E: case 0xC20B20: case 0xC20B22: case 0xC20B4E:
    case 0xC20B50: case 0xC20B52: case 0xC20BB6: case 0xC20BB8:
    case 0xC20BBA: case 0xC20BE8: case 0xC20BEA: case 0xC20BEC:
    case 0xC20CDE: case 0xC20CE0: case 0xC20CE2: case 0xC20D1A:
    case 0xC20D1C: case 0xC20D1E: case 0xC219DC: case 0xC219DE:
    case 0xC219E0: case 0xC21C58: case 0xC21C5A: case 0xC21C5C:
    case 0xC21C70: case 0xC21C72: case 0xC21C74:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC208B8:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC208C2: case 0xC20AB6: case 0xC20ABA: case 0xC20ABE:
    case 0xC20AD2: case 0xC20AD6: case 0xC20ADA: case 0xC20B02:
    case 0xC20B06: case 0xC20B0A: case 0xC20B24: case 0xC20B26:
    case 0xC20B28: case 0xC20B30: case 0xC20B34: case 0xC20B38:
    case 0xC20B54: case 0xC20B56: case 0xC20B58: case 0xC20B60:
    case 0xC20B64: case 0xC20B68: case 0xC20B98: case 0xC20B9C:
    case 0xC20BA0: case 0xC20CC2: case 0xC20CC6: case 0xC20CCA:
    case 0xC20CE4: case 0xC20CE6: case 0xC20CE8: case 0xC20CF0:
    case 0xC20CF4: case 0xC20CF8: case 0xC20D08: case 0xC20D0C:
    case 0xC20D10: case 0xC20D20: case 0xC20D22: case 0xC20D24:
    case 0xC20D2C: case 0xC20D30: case 0xC20D34: case 0xC219D0:
    case 0xC219D2: case 0xC219D4: case 0xC219E8: case 0xC219EA:
    case 0xC219EC: case 0xC219EE: case 0xC219F0: case 0xC21A12:
    case 0xC21C5E: case 0xC21C60: case 0xC21C62: case 0xC21C76:
    case 0xC21C78: case 0xC21C7A:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC208C8:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC20B82: case 0xC20C10: case 0xC20D4E: case 0xC20D56:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC21A06:
        width=1; goto move;
    case 0xC21A0A:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='&'; goto bit_value;
    case 0xC21A0E:
        hud_parent_asr_byte(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC21A10:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC21A16:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC21C3A:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC21C40:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
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
static const uint32_t owned_C1FF0A[]={
    0xC1FF0A,0xC1FF10,0xC1FF16,0xC1FF1A,0xC1FF1E,0xC1FF22,0xC1FF26,0xC1FF2A,
    0xC1FF2E,0xC1FF32,0xC1FF34,0xC1FF36,0xC1FF3C,0xC1FF3E,0xC1FF40,0xC1FF44,
};
int glue_C1FF0A_owns(uint32_t pc) { return owns_pc(owned_C1FF0A,sizeof owned_C1FF0A/sizeof owned_C1FF0A[0],pc); }
int glue_C1FF0A_complete_step(void) { if(!glue_C1FF0A_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C2005C[]={
    0xC2005C,0xC20062,0xC20064,0xC20066,0xC2006C,0xC2006E,0xC20070,0xC20074,
    0xC20076,0xC20078,0xC2007A,0xC2007C,0xC20080,0xC20082,0xC20084,0xC20086,
    0xC20088,0xC2008A,0xC2008E,0xC20090,0xC20092,0xC20094,0xC20096,0xC20098,
    0xC2009C,0xC200A0,0xC200A2,0xC200A4,0xC200A6,0xC200A8,0xC200AE,0xC200B0,
    0xC200B2,0xC200B4,0xC200B8,0xC200BE,0xC200C0,0xC200C2,0xC200C6,0xC200C8,
    0xC200CA,0xC200CC,0xC200D0,0xC200D2,0xC200D4,0xC200D8,0xC200DA,0xC200DC,
    0xC200E2,0xC200E6,0xC200EC,0xC200F0,0xC200F2,0xC200F4,
};
int glue_C2005C_owns(uint32_t pc) { return owns_pc(owned_C2005C,sizeof owned_C2005C/sizeof owned_C2005C[0],pc); }
int glue_C2005C_complete_step(void) { if(!glue_C2005C_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C20100[]={
    0xC200F6,0xC200FA,0xC200FE,0xC20100,0xC20104,0xC20108,0xC2010E,0xC20110,
    0xC20112,0xC20114,0xC20116,0xC20118,0xC2011E,0xC20124,0xC20126,0xC20128,
    0xC2012C,0xC20130,0xC20132,0xC20134,0xC20138,0xC2013C,0xC20140,0xC20142,
    0xC20144,0xC20148,0xC2014C,0xC20150,0xC20152,0xC20154,0xC20158,0xC2015C,
    0xC20160,0xC20164,0xC20166,0xC2016C,0xC2016E,0xC20170,0xC20172,0xC20178,
    0xC2017A,0xC2017C,0xC20180,0xC20182,0xC20184,0xC20186,0xC20188,0xC2018E,
    0xC20190,0xC20196,0xC2019A,0xC2019C,0xC2019E,
};
int glue_C20100_owns(uint32_t pc) { return owns_pc(owned_C20100,sizeof owned_C20100/sizeof owned_C20100[0],pc); }
int glue_C20100_complete_step(void) { if(!glue_C20100_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C21060[]={
    0xC21060,0xC21064,0xC2106C,0xC21070,0xC21074,0xC21076,0xC21078,0xC21080,
    0xC21084,0xC2108E,0xC21090,0xC21092,0xC21096,0xC2109C,0xC210A2,0xC210A6,
    0xC210A8,0xC210AA,0xC210AC,0xC210B0,0xC210B2,0xC210B4,0xC210B6,0xC210BA,
    0xC210BC,0xC210BE,0xC210C0,0xC210C4,0xC210C6,0xC210C8,0xC210CA,0xC210CC,
    0xC210CE,0xC210D4,0xC210D8,0xC210DA,0xC210DC,0xC210E0,0xC210E4,
};
int glue_C21060_owns(uint32_t pc) { return owns_pc(owned_C21060,sizeof owned_C21060/sizeof owned_C21060[0],pc); }
int glue_C21060_complete_step(void) { if(!glue_C21060_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C20C38[]={
    0xC20C38,0xC20C3C,0xC20C44,0xC20C48,0xC20C4C,0xC20C50,0xC20C52,0xC20C5A,
    0xC20C60,0xC20C62,0xC20C66,0xC20C6A,0xC20C6E,0xC20C72,0xC20C76,0xC20C7A,
    0xC20C7E,0xC20C82,0xC20C88,0xC20C8C,0xC20C8E,0xC20C92,0xC20C96,0xC20C9C,
    0xC20CA0,0xC20CA6,0xC20CAC,0xC20CAE,0xC20CB0,0xC20CB4,0xC20CB6,0xC20CB8,
    0xC20CBA,0xC20CBC,0xC20CC2,0xC20CC6,0xC20CCA,0xC20CCE,0xC20CD4,0xC20CDA,
    0xC20CDE,0xC20CE0,0xC20CE2,0xC20CE4,0xC20CE6,0xC20CE8,0xC20CEA,0xC20CEC,
    0xC20CEE,0xC20CF0,0xC20CF4,0xC20CF8,0xC20CFC,0xC20D00,0xC20D04,0xC20D08,
    0xC20D0C,0xC20D10,0xC20D14,0xC20D1A,0xC20D1C,0xC20D1E,0xC20D20,0xC20D22,
    0xC20D24,0xC20D26,0xC20D28,0xC20D2A,0xC20D2C,0xC20D30,0xC20D34,0xC20D38,
    0xC20D3C,0xC20D40,0xC20D46,0xC20D4A,0xC20D4E,0xC20D52,0xC20D56,0xC20D5A,
    0xC20D5E,0xC20D62,0xC20D66,
};
int glue_C20C38_owns(uint32_t pc) { return owns_pc(owned_C20C38,sizeof owned_C20C38/sizeof owned_C20C38[0],pc); }
int glue_C20C38_complete_step(void) { if(!glue_C20C38_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C20C22[]={
    0xC20C22,0xC20C26,0xC20C30,0xC20C36,0xC20C5A,0xC20C60,0xC20C62,0xC20C66,
    0xC20C6A,0xC20C6E,0xC20C72,0xC20C76,0xC20C7A,0xC20C7E,0xC20C82,0xC20C88,
    0xC20C8C,0xC20C8E,0xC20C92,0xC20C96,0xC20C9C,0xC20CA0,0xC20CA6,0xC20CAC,
    0xC20CAE,0xC20CB0,0xC20CB4,0xC20CB6,0xC20CB8,0xC20CBA,0xC20CBC,0xC20CC2,
    0xC20CC6,0xC20CCA,0xC20CCE,0xC20CD4,0xC20CDA,0xC20CDE,0xC20CE0,0xC20CE2,
    0xC20CE4,0xC20CE6,0xC20CE8,0xC20CEA,0xC20CEC,0xC20CEE,0xC20CF0,0xC20CF4,
    0xC20CF8,0xC20CFC,0xC20D00,0xC20D04,0xC20D08,0xC20D0C,0xC20D10,0xC20D14,
    0xC20D1A,0xC20D1C,0xC20D1E,0xC20D20,0xC20D22,0xC20D24,0xC20D26,0xC20D28,
    0xC20D2A,0xC20D2C,0xC20D30,0xC20D34,0xC20D38,0xC20D3C,0xC20D40,0xC20D46,
    0xC20D4A,0xC20D4E,0xC20D52,0xC20D56,0xC20D5A,0xC20D5E,0xC20D62,0xC20D66,
};
int glue_C20C22_owns(uint32_t pc) { return owns_pc(owned_C20C22,sizeof owned_C20C22/sizeof owned_C20C22[0],pc); }
int glue_C20C22_complete_step(void) { if(!glue_C20C22_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C20A52[]={
    0xC20A52,0xC20A5A,0xC20A5E,0xC20A62,0xC20A66,0xC20A68,0xC20A70,0xC20A76,
    0xC20A78,0xC20A7C,0xC20A80,0xC20A84,0xC20A88,0xC20A8C,0xC20A8E,0xC20A92,
    0xC20A96,0xC20A98,0xC20A9C,0xC20AA0,0xC20AA6,0xC20AA8,0xC20AAA,0xC20AAC,
    0xC20AB0,0xC20AB2,0xC20AB4,0xC20AB6,0xC20ABA,0xC20ABE,0xC20AC2,0xC20AC8,
    0xC20ACC,0xC20ACE,0xC20AD0,0xC20AD2,0xC20AD6,0xC20ADA,0xC20ADE,0xC20AE4,
    0xC20AE8,0xC20AEE,0xC20AF4,0xC20AF6,0xC20AF8,0xC20AFA,0xC20AFC,0xC20B02,
    0xC20B06,0xC20B0A,0xC20B0E,0xC20B14,0xC20B1A,0xC20B1E,0xC20B20,0xC20B22,
    0xC20B24,0xC20B26,0xC20B28,0xC20B2A,0xC20B2C,0xC20B2E,0xC20B30,0xC20B34,
    0xC20B38,0xC20B3C,0xC20B40,0xC20B44,0xC20B48,0xC20B4E,0xC20B50,0xC20B52,
    0xC20B54,0xC20B56,0xC20B58,0xC20B5A,0xC20B5C,0xC20B5E,0xC20B60,0xC20B64,
    0xC20B68,0xC20B6C,0xC20B70,0xC20B74,0xC20B7A,0xC20B7E,0xC20B82,0xC20B86,
    0xC20B8A,0xC20B8C,0xC20B8E,0xC20B90,0xC20B92,0xC20B98,0xC20B9C,0xC20BA0,
    0xC20BA4,0xC20BAA,0xC20BB0,0xC20BB6,0xC20BB8,0xC20BBA,0xC20BBC,0xC20BBE,
    0xC20BC0,0xC20BC2,0xC20BC4,0xC20BC6,0xC20BC8,0xC20BCC,0xC20BD0,0xC20BD4,
    0xC20BD8,0xC20BDC,0xC20BE2,0xC20BE8,0xC20BEA,0xC20BEC,0xC20BEE,0xC20BF0,
    0xC20BF2,0xC20BF4,0xC20BF6,0xC20BF8,0xC20BFA,0xC20BFE,0xC20C02,0xC20C06,
    0xC20C0A,0xC20C10,0xC20C14,0xC20C18,0xC20C1C,0xC20C20,
};
int glue_C20A52_owns(uint32_t pc) { return owns_pc(owned_C20A52,sizeof owned_C20A52/sizeof owned_C20A52[0],pc); }
int glue_C20A52_complete_step(void) { if(!glue_C20A52_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C20A40[]={
    0xC20A40,0xC20A4A,0xC20A50,0xC20A70,0xC20A76,0xC20A78,0xC20A7C,0xC20A80,
    0xC20A84,0xC20A88,0xC20A8C,0xC20A8E,0xC20A92,0xC20A96,0xC20A98,0xC20A9C,
    0xC20AA0,0xC20AA6,0xC20AA8,0xC20AAA,0xC20AAC,0xC20AB0,0xC20AB2,0xC20AB4,
    0xC20AB6,0xC20ABA,0xC20ABE,0xC20AC2,0xC20AC8,0xC20ACC,0xC20ACE,0xC20AD0,
    0xC20AD2,0xC20AD6,0xC20ADA,0xC20ADE,0xC20AE4,0xC20AE8,0xC20AEE,0xC20AF4,
    0xC20AF6,0xC20AF8,0xC20AFA,0xC20AFC,0xC20B02,0xC20B06,0xC20B0A,0xC20B0E,
    0xC20B14,0xC20B1A,0xC20B1E,0xC20B20,0xC20B22,0xC20B24,0xC20B26,0xC20B28,
    0xC20B2A,0xC20B2C,0xC20B2E,0xC20B30,0xC20B34,0xC20B38,0xC20B3C,0xC20B40,
    0xC20B44,0xC20B48,0xC20B4E,0xC20B50,0xC20B52,0xC20B54,0xC20B56,0xC20B58,
    0xC20B5A,0xC20B5C,0xC20B5E,0xC20B60,0xC20B64,0xC20B68,0xC20B6C,0xC20B70,
    0xC20B74,0xC20B7A,0xC20B7E,0xC20B82,0xC20B86,0xC20B8A,0xC20B8C,0xC20B8E,
    0xC20B90,0xC20B92,0xC20B98,0xC20B9C,0xC20BA0,0xC20BA4,0xC20BAA,0xC20BB0,
    0xC20BB6,0xC20BB8,0xC20BBA,0xC20BBC,0xC20BBE,0xC20BC0,0xC20BC2,0xC20BC4,
    0xC20BC6,0xC20BC8,0xC20BCC,0xC20BD0,0xC20BD4,0xC20BD8,0xC20BDC,0xC20BE2,
    0xC20BE8,0xC20BEA,0xC20BEC,0xC20BEE,0xC20BF0,0xC20BF2,0xC20BF4,0xC20BF6,
    0xC20BF8,0xC20BFA,0xC20BFE,0xC20C02,0xC20C06,0xC20C0A,0xC20C10,0xC20C14,
    0xC20C18,0xC20C1C,0xC20C20,
};
int glue_C20A40_owns(uint32_t pc) { return owns_pc(owned_C20A40,sizeof owned_C20A40/sizeof owned_C20A40[0],pc); }
int glue_C20A40_complete_step(void) { if(!glue_C20A40_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C20002[]={
    0xC20002,0xC20008,0xC2000E,0xC20014,0xC20018,0xC2001C,0xC20020,0xC20024,
    0xC20028,0xC2002C,0xC20030,0xC20036,0xC20038,0xC2003A,0xC2003C,0xC2003E,
    0xC20040,0xC20046,0xC20048,0xC2004A,0xC2004C,0xC2004E,0xC20050,0xC20052,
    0xC20054,0xC20056,0xC2005A,0xC200AE,0xC200B0,0xC200B2,0xC200B4,0xC200B8,
    0xC200BE,0xC200C0,0xC200C2,0xC200C6,0xC200C8,0xC200CA,0xC200CC,0xC200D0,
    0xC200D2,0xC200D4,0xC200D8,0xC200DA,0xC200DC,0xC200E2,0xC200E6,0xC200EC,
    0xC200F0,0xC200F2,0xC200F4,
};
int glue_C20002_owns(uint32_t pc) { return owns_pc(owned_C20002,sizeof owned_C20002/sizeof owned_C20002[0],pc); }
int glue_C20002_complete_step(void) { if(!glue_C20002_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C2084A[]={
    0xC2084A,0xC2084C,0xC2084E,0xC20850,0xC20852,0xC20854,0xC20856,0xC20858,
    0xC2085C,0xC20862,0xC20866,0xC2086A,0xC20870,0xC20872,0xC20876,0xC2087C,
    0xC20880,0xC20886,0xC2088A,0xC2088C,0xC2088E,0xC20890,0xC20892,0xC20894,
    0xC20896,0xC20898,0xC2089A,0xC208A0,0xC208AA,0xC208AC,0xC208B2,0xC208B6,
    0xC208B8,0xC208BC,0xC208BE,0xC208C2,0xC208C4,0xC208C8,0xC208CA,0xC208CC,
    0xC208CE,0xC208D0,0xC208D2,
};
int glue_C2084A_owns(uint32_t pc) { return owns_pc(owned_C2084A,sizeof owned_C2084A/sizeof owned_C2084A[0],pc); }
int glue_C2084A_complete_step(void) { if(!glue_C2084A_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C2082A[]={
    0xC20826,0xC20828,0xC2082A,0xC20830,0xC20834,0xC20836,0xC20840,0xC20842,
    0xC20848,0xC2084A,0xC2084C,0xC2084E,0xC20850,0xC20852,0xC20854,0xC20856,
    0xC20858,0xC2085C,0xC20862,0xC20866,0xC2086A,0xC20870,0xC20872,0xC20876,
    0xC2087C,0xC20880,0xC20886,0xC2088A,0xC2088C,0xC2088E,0xC20890,0xC20892,
    0xC20894,0xC20896,0xC20898,0xC2089A,0xC208A0,0xC208AA,0xC208AC,0xC208B2,
    0xC208B6,0xC208B8,0xC208BC,0xC208BE,0xC208C2,0xC208C4,0xC208C8,0xC208CA,
    0xC208CC,0xC208CE,0xC208D0,0xC208D2,
};
int glue_C2082A_owns(uint32_t pc) { return owns_pc(owned_C2082A,sizeof owned_C2082A/sizeof owned_C2082A[0],pc); }
int glue_C2082A_complete_step(void) { if(!glue_C2082A_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C219AE[]={
    0xC219AE,0xC219B4,0xC219B6,0xC219B8,0xC219BE,0xC219C4,0xC219C6,0xC219C8,
    0xC219CA,0xC219CC,0xC219D0,0xC219D2,0xC219D4,0xC219D6,0xC219DC,0xC219DE,
    0xC219E0,0xC219E2,0xC219E8,0xC219EA,0xC219EC,0xC219EE,0xC219F0,0xC219F2,
    0xC219F4,0xC219FA,0xC21A00,0xC21A06,0xC21A0A,0xC21A0E,0xC21A10,0xC21A12,
    0xC21A14,0xC21A16,0xC21A18,0xC21A1A,0xC21A1C,0xC21A1E,
};
int glue_C219AE_owns(uint32_t pc) { return owns_pc(owned_C219AE,sizeof owned_C219AE/sizeof owned_C219AE[0],pc); }
int glue_C219AE_complete_step(void) { if(!glue_C219AE_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C21C4C[]={
    0xC21C4C,0xC21C52,0xC21C54,0xC21C56,0xC21C58,0xC21C5A,0xC21C5C,0xC21C5E,
    0xC21C60,0xC21C62,0xC21C64,0xC21C6A,0xC21C6C,0xC21C6E,0xC21C70,0xC21C72,
    0xC21C74,0xC21C76,0xC21C78,0xC21C7A,0xC21C7C,0xC21C82,0xC21C84,
};
int glue_C21C4C_owns(uint32_t pc) { return owns_pc(owned_C21C4C,sizeof owned_C21C4C/sizeof owned_C21C4C[0],pc); }
int glue_C21C4C_complete_step(void) { if(!glue_C21C4C_owns(REG_PC)) return 0; return face_list_parents_step(); }
static const uint32_t owned_C21C2E[]={
    0xC21C2E,0xC21C34,0xC21C3A,0xC21C3E,0xC21C40,0xC21C44,0xC21C4A,0xC21C4C,
    0xC21C52,0xC21C54,0xC21C56,0xC21C58,0xC21C5A,0xC21C5C,0xC21C5E,0xC21C60,
    0xC21C62,0xC21C64,0xC21C6A,0xC21C6C,0xC21C6E,0xC21C70,0xC21C72,0xC21C74,
    0xC21C76,0xC21C78,0xC21C7A,0xC21C7C,0xC21C82,0xC21C84,
};
int glue_C21C2E_owns(uint32_t pc) { return owns_pc(owned_C21C2E,sizeof owned_C21C2E/sizeof owned_C21C2E[0],pc); }
int glue_C21C2E_complete_step(void) { if(!glue_C21C2E_owns(REG_PC)) return 0; return face_list_parents_step(); }
