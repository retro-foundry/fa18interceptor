/* Complete input device callbacks family source CPU/bus/event boundaries.
 * Readable behavior lives in input_device_callbacks.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family input_device_callbacks. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_input_device_callbacks.h"

static int input_device_callbacks_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC16B8C: case 0xC16BCA: case 0xC16CDC: case 0xC17340:
    case 0xC1734A: case 0xC1736A: case 0xC1739A: case 0xC1742C:
    case 0xC1744C: case 0xC1747E: case 0xC17492: case 0xC174A4:
    case 0xC174BC:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC16B8E: case 0xC16B90: case 0xC16B96: case 0xC16BA8:
    case 0xC16BC2: case 0xC16BCC: case 0xC16BD6: case 0xC16BE0:
    case 0xC16BE2: case 0xC16CDE: case 0xC16CE6: case 0xC1727A:
    case 0xC172B8: case 0xC1734C: case 0xC1734E: case 0xC17356:
    case 0xC1737E: case 0xC17394: case 0xC1739C: case 0xC1739E:
    case 0xC173C2: case 0xC173D8: case 0xC173EE: case 0xC1742E:
    case 0xC17430: case 0xC17464: case 0xC17472: case 0xC17480:
    case 0xC17494: case 0xC174BE: case 0xC174DE: case 0xC174E6:
        width=4; goto move;
    case 0xC16B98: case 0xC16CE0: case 0xC16CE8: case 0xC17350:
    case 0xC173A2: case 0xC17436: case 0xC17478: case 0xC1748C:
        address=cache_step_address(mode,reg,4); m68ki_push_32(address); break;
    case 0xC16B9E: case 0xC16BB4: case 0xC16BE8: case 0xC16CEE:
    case 0xC1735A: case 0xC173A8: case 0xC1743C: case 0xC17446:
    case 0xC17482: case 0xC17496: case 0xC174C0: case 0xC174D2:
    case 0xC174E8:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC16BA4: case 0xC16BD0: case 0xC16CF4: case 0xC17330:
    case 0xC17360: case 0xC173AE: case 0xC17442: case 0xC1746E:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC16BAE: case 0xC16BDA: case 0xC16CF8: case 0xC17108:
    case 0xC17110: case 0xC17118: case 0xC17120: case 0xC17130:
    case 0xC17136: case 0xC1713E: case 0xC17148: case 0xC1714E:
    case 0xC17196: case 0xC1719C: case 0xC171A4: case 0xC171AE:
    case 0xC171B8: case 0xC171C2: case 0xC171C6: case 0xC171D8:
    case 0xC171E8: case 0xC171FA: case 0xC1721A: case 0xC17220:
    case 0xC17224: case 0xC1722E: case 0xC17234: case 0xC1723E:
    case 0xC17244: case 0xC1724A: case 0xC17254: case 0xC1725A:
    case 0xC17264: case 0xC1726A: case 0xC17274: case 0xC1727C:
    case 0xC17282: case 0xC17288: case 0xC17292: case 0xC17298:
    case 0xC172A2: case 0xC172A8: case 0xC172B2: case 0xC172BA:
    case 0xC172C0: case 0xC172C8: case 0xC172D0: case 0xC172D8:
    case 0xC17364: case 0xC1736E: case 0xC17384: case 0xC173B2:
    case 0xC173C8: case 0xC1740A:
        width=2; goto move;
    case 0xC16BBA: case 0xC16BEE: case 0xC1740C: case 0xC17410:
    case 0xC17488: case 0xC1749C: case 0xC174C6: case 0xC174D8:
    case 0xC174EE:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC16BBC: case 0xC17376: case 0xC1738C: case 0xC173BA:
    case 0xC173D0: case 0xC17402: case 0xC17406: case 0xC174A6:
    case 0xC174C8: case 0xC174DA:
        width=4; goto move;
    case 0xC16BF0: case 0xC16D02: case 0xC1712A: case 0xC1715A:
    case 0xC17454: case 0xC1748A: case 0xC1749E: case 0xC174F2:
        REG_PC=m68ki_pull_32(); break;
    case 0xC16CD8: case 0xC17104: case 0xC1712C: case 0xC1718E:
    case 0xC174A0:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC16CFC: case 0xC17214:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC16CFE:
        break;
    case 0xC16D00: case 0xC17128: case 0xC17158: case 0xC17452:
    case 0xC174F0:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC1713A: case 0xC17144: case 0xC171A0: case 0xC171AA:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC17142: case 0xC171A8: case 0xC1721E:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC17154:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC17192: case 0xC1744E:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC171B2: case 0xC171BC: case 0xC1723A: case 0xC1736C:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC171CA: case 0xC171DC: case 0xC171EC: case 0xC171FE:
    case 0xC173FA:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC171CE: case 0xC171F0: case 0xC17400:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC171D0: case 0xC171F2:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC171D6: case 0xC171F8: case 0xC17318: case 0xC17418:
    case 0xC17422:
        step_branch(pc,opcode,1); break;
    case 0xC171E0: case 0xC17202: case 0xC17306:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC171E2: case 0xC17204:
        width=2; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC1720A: case 0xC172DE: case 0xC172E4: case 0xC172F0:
    case 0xC172F8: case 0xC1730A: case 0xC17310: case 0xC1731A:
    case 0xC17322: case 0xC17328: case 0xC17336: case 0xC173DE:
    case 0xC173E4: case 0xC1741A: case 0xC17456: case 0xC174AA:
    case 0xC174AE: case 0xC174B6: case 0xC174CC:
        width=1; goto move;
    case 0xC17210: case 0xC172FE: case 0xC17424:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC17212: case 0xC173EC:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC17218: case 0xC17300:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC1722A:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC17250: case 0xC17260: case 0xC17270: case 0xC1728E:
    case 0xC1729E: case 0xC172AE:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC17252: case 0xC17272: case 0xC17290: case 0xC172B0:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC17262: case 0xC172A0:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC172D6: case 0xC17414:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC172EA: case 0xC17304: case 0xC173EA:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;
    case 0xC172EC: case 0xC1742A:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC172F6: case 0xC17308:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC17320:
        width=1; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC1733C:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC1733E: case 0xC17372: case 0xC17388: case 0xC173B6:
    case 0xC173CC:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC17342:
        width=4; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC17344: case 0xC17346: case 0xC17374: case 0xC1738A:
    case 0xC173B8: case 0xC173CE:
        step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC17348: case 0xC17378: case 0xC1738E: case 0xC173BC:
    case 0xC173D2:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC173F6:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC1745E:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC174B2: case 0xC174D0:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC174E2:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
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
static const uint32_t owned_C1718E[]={
    0xC1718E,0xC17192,0xC17196,0xC1719C,0xC171A0,0xC171A4,0xC171A8,0xC171AA,
    0xC171AE,0xC171B2,0xC171B8,0xC171BC,0xC171C2,0xC171C6,0xC171CA,0xC171CE,
    0xC171D0,0xC171D6,0xC171D8,0xC171DC,0xC171E0,0xC171E2,0xC171E8,0xC171EC,
    0xC171F0,0xC171F2,0xC171F8,0xC171FA,0xC171FE,0xC17202,0xC17204,0xC1720A,
    0xC17210,0xC17212,0xC17214,0xC17218,0xC1721A,0xC1721E,0xC17220,0xC17224,
    0xC1722A,0xC1722E,0xC17234,0xC1723A,0xC1723E,0xC17244,0xC1724A,0xC17250,
    0xC17252,0xC17254,0xC1725A,0xC17260,0xC17262,0xC17264,0xC1726A,0xC17270,
    0xC17272,0xC17274,0xC1727A,0xC1727C,0xC17282,0xC17288,0xC1728E,0xC17290,
    0xC17292,0xC17298,0xC1729E,0xC172A0,0xC172A2,0xC172A8,0xC172AE,0xC172B0,
    0xC172B2,0xC172B8,0xC172BA,0xC172C0,0xC172C8,0xC172D0,0xC172D6,0xC172D8,
    0xC172DE,0xC172E4,0xC172EA,0xC172EC,0xC172F0,0xC172F6,0xC172F8,0xC172FE,
    0xC17300,0xC17304,0xC17306,0xC17308,0xC1730A,0xC17310,0xC17318,0xC1731A,
    0xC17320,0xC17322,0xC17328,0xC17330,0xC17336,0xC1733C,0xC1733E,0xC17340,
    0xC17342,0xC17344,0xC17346,0xC17348,0xC1734A,0xC1734C,0xC1734E,0xC17350,
    0xC17356,0xC1735A,0xC17360,0xC17364,0xC1736A,0xC1736C,0xC1736E,0xC17372,
    0xC17374,0xC17376,0xC17378,0xC1737E,0xC17384,0xC17388,0xC1738A,0xC1738C,
    0xC1738E,0xC17394,0xC1739A,0xC1739C,0xC1739E,0xC173A2,0xC173A8,0xC173AE,
    0xC173B2,0xC173B6,0xC173B8,0xC173BA,0xC173BC,0xC173C2,0xC173C8,0xC173CC,
    0xC173CE,0xC173D0,0xC173D2,0xC173D8,0xC173DE,0xC173E4,0xC173EA,0xC173EC,
    0xC173EE,0xC173F6,0xC173FA,0xC17400,0xC17402,0xC17406,0xC1740A,0xC1740C,
    0xC17410,0xC17414,0xC17418,0xC1741A,0xC17422,0xC17424,0xC1742A,0xC1742C,
    0xC1742E,0xC17430,0xC17436,0xC1743C,0xC17442,0xC17446,0xC1744C,0xC1744E,
    0xC17452,0xC17454,
};
int glue_C1718E_owns(uint32_t pc) { return owns_pc(owned_C1718E,sizeof owned_C1718E/sizeof owned_C1718E[0],pc); }
int glue_C1718E_step(void) { if(!glue_C1718E_owns(REG_PC)) return 0; return input_device_callbacks_step(); }
static const uint32_t owned_C17456[]={
    0xC17456,0xC1745E,0xC17464,0xC1746E,0xC17472,0xC17478,0xC1747E,0xC17480,
    0xC17482,0xC17488,0xC1748A,
};
int glue_C17456_owns(uint32_t pc) { return owns_pc(owned_C17456,sizeof owned_C17456/sizeof owned_C17456[0],pc); }
int glue_C17456_step(void) { if(!glue_C17456_owns(REG_PC)) return 0; return input_device_callbacks_step(); }
static const uint32_t owned_C1748C[]={
    0xC1748C,0xC17492,0xC17494,0xC17496,0xC1749C,0xC1749E,
};
int glue_C1748C_owns(uint32_t pc) { return owns_pc(owned_C1748C,sizeof owned_C1748C/sizeof owned_C1748C[0],pc); }
int glue_C1748C_step(void) { if(!glue_C1748C_owns(REG_PC)) return 0; return input_device_callbacks_step(); }
static const uint32_t owned_C174A0[]={
    0xC174A0,0xC174A4,0xC174A6,0xC174AA,0xC174AE,0xC174B2,0xC174B6,0xC174BC,
    0xC174BE,0xC174C0,0xC174C6,0xC174C8,0xC174CC,0xC174D0,0xC174D2,0xC174D8,
    0xC174DA,0xC174DE,0xC174E2,0xC174E6,0xC174E8,0xC174EE,0xC174F0,0xC174F2,
};
int glue_C174A0_owns(uint32_t pc) { return owns_pc(owned_C174A0,sizeof owned_C174A0/sizeof owned_C174A0[0],pc); }
int glue_C174A0_step(void) { if(!glue_C174A0_owns(REG_PC)) return 0; return input_device_callbacks_step(); }
static const uint32_t owned_C16CD8[]={
    0xC16CD8,0xC16CDC,0xC16CDE,0xC16CE0,0xC16CE6,0xC16CE8,0xC16CEE,0xC16CF4,
    0xC16CF8,0xC16CFC,0xC16CFE,0xC16D00,0xC16D02,
};
int glue_C16CD8_owns(uint32_t pc) { return owns_pc(owned_C16CD8,sizeof owned_C16CD8/sizeof owned_C16CD8[0],pc); }
int glue_C16CD8_step(void) { if(!glue_C16CD8_owns(REG_PC)) return 0; return input_device_callbacks_step(); }
static const uint32_t owned_C16B8C[]={
    0xC16B8C,0xC16B8E,0xC16B90,0xC16B96,0xC16B98,0xC16B9E,0xC16BA4,0xC16BA8,
    0xC16BAE,0xC16BB4,0xC16BBA,0xC16BBC,0xC16BC2,0xC16BCA,0xC16BCC,0xC16BD0,
    0xC16BD6,0xC16BDA,0xC16BE0,0xC16BE2,0xC16BE8,0xC16BEE,0xC16BF0,
};
int glue_C16B8C_owns(uint32_t pc) { return owns_pc(owned_C16B8C,sizeof owned_C16B8C/sizeof owned_C16B8C[0],pc); }
int glue_C16B8C_step(void) { if(!glue_C16B8C_owns(REG_PC)) return 0; return input_device_callbacks_step(); }
static const uint32_t owned_C17104[]={
    0xC17104,0xC17108,0xC17110,0xC17118,0xC17120,0xC17128,0xC1712A,
};
int glue_C17104_owns(uint32_t pc) { return owns_pc(owned_C17104,sizeof owned_C17104/sizeof owned_C17104[0],pc); }
int glue_C17104_step(void) { if(!glue_C17104_owns(REG_PC)) return 0; return input_device_callbacks_step(); }
static const uint32_t owned_C1712C[]={
    0xC1712C,0xC17130,0xC17136,0xC1713A,0xC1713E,0xC17142,0xC17144,0xC17148,
    0xC1714E,0xC17154,0xC17158,0xC1715A,
};
int glue_C1712C_owns(uint32_t pc) { return owns_pc(owned_C1712C,sizeof owned_C1712C/sizeof owned_C1712C[0],pc); }
int glue_C1712C_step(void) { if(!glue_C1712C_owns(REG_PC)) return 0; return input_device_callbacks_step(); }
