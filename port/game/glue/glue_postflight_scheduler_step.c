/* Complete postflight scheduler family source CPU/bus/event boundaries.
 * Readable behavior lives in postflight_scheduler.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family postflight_scheduler. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_postflight_scheduler.h"

static int schedule_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC09E06: case 0xC09E0C: case 0xC09E12: case 0xC09F80:
    case 0xC0A280:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC09E18: case 0xC09FB6: case 0xC0A2B6:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC09E1E: case 0xC09EE6: case 0xC09EEE: case 0xC09EFE:
    case 0xC09F08: case 0xC09F36: case 0xC09F5E: case 0xC09FA8:
    case 0xC09FE0: case 0xC0A018: case 0xC0A020: case 0xC0A02A:
    case 0xC0A038: case 0xC0A176: case 0xC0A202: case 0xC0A20A:
    case 0xC0A236: case 0xC0A25E: case 0xC0A2A8: case 0xC0A2FE:
    case 0xC0A342: case 0xC0A3F0: case 0xC0A3F8: case 0xC0A40A:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC09E26: case 0xC09EB2: case 0xC09EE4: case 0xC09EEC:
    case 0xC09EF4: case 0xC09F28: case 0xC09F64: case 0xC09FAE:
    case 0xC09FC4: case 0xC09FDE: case 0xC09FE6: case 0xC0A026:
    case 0xC0A030: case 0xC0A03E: case 0xC0A104: case 0xC0A11C:
    case 0xC0A16C: case 0xC0A17C: case 0xC0A1B4: case 0xC0A1CE:
    case 0xC0A200: case 0xC0A208: case 0xC0A228: case 0xC0A264:
    case 0xC0A2C4: case 0xC0A2DC: case 0xC0A304: case 0xC0A322:
    case 0xC0A352: case 0xC0A39C: case 0xC0A3D0: case 0xC0A3F6:
    case 0xC0A3FE: case 0xC0A408: case 0xC0A410: case 0xC0A41C:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC09E28: case 0xC09E98: case 0xC09EA2: case 0xC09EC4:
    case 0xC09F12: case 0xC09F86: case 0xC0A002: case 0xC0A0E4:
    case 0xC0A15C: case 0xC0A1E0: case 0xC0A212: case 0xC0A286:
    case 0xC0A2F0: case 0xC0A334: case 0xC0A364: case 0xC0A37C:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC09E2E: case 0xC09E40: case 0xC09E4C: case 0xC09E58:
    case 0xC09E64: case 0xC09E70: case 0xC09E7C: case 0xC09E88:
    case 0xC09E9E: case 0xC09ECA: case 0xC09F06: case 0xC09F10:
    case 0xC09F18: case 0xC09F3E: case 0xC09F8C: case 0xC0A008:
    case 0xC0A01E: case 0xC0A0EA: case 0xC0A0FA: case 0xC0A162:
    case 0xC0A1E6: case 0xC0A210: case 0xC0A218: case 0xC0A23E:
    case 0xC0A28C: case 0xC0A2AE: case 0xC0A2F6: case 0xC0A312:
    case 0xC0A318: case 0xC0A33A: case 0xC0A348: case 0xC0A36A:
    case 0xC0A382: case 0xC0A392: case 0xC0A3D4: case 0xC0A3DA:
    case 0xC0A422:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC09E30: case 0xC09EB4: case 0xC09F1A: case 0xC09F42:
    case 0xC09F54: case 0xC09F68: case 0xC09F70: case 0xC09F78:
    case 0xC09F9E: case 0xC09FC6: case 0xC09FE8: case 0xC09FF2:
    case 0xC0A0D6: case 0xC0A0EC: case 0xC0A106: case 0xC0A11E:
    case 0xC0A130: case 0xC0A1B6: case 0xC0A1D0: case 0xC0A21A:
    case 0xC0A242: case 0xC0A254: case 0xC0A268: case 0xC0A270:
    case 0xC0A278: case 0xC0A29E: case 0xC0A2C6: case 0xC0A2DE:
    case 0xC0A324: case 0xC0A354: case 0xC0A36E: case 0xC0A384:
    case 0xC0A39E: case 0xC0A3AC: case 0xC0A3B4: case 0xC0A3BC:
    case 0xC0A3C6: case 0xC0A3DC: case 0xC0A414:
        width=1; goto move;
    case 0xC09E36: case 0xC09E3C: case 0xC09E48: case 0xC09E54:
    case 0xC09E60: case 0xC09E6C: case 0xC09E78: case 0xC09E84:
    case 0xC09EAA: case 0xC09F2A: case 0xC09FBC: case 0xC09FD6:
    case 0xC0A05A: case 0xC0A064: case 0xC0A0FC: case 0xC0A114:
    case 0xC0A1AC: case 0xC0A1C6: case 0xC0A22A: case 0xC0A2BC:
    case 0xC0A2D4: case 0xC0A31A: case 0xC0A34A: case 0xC0A394:
    case 0xC0A3CC: case 0xC0A400:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC09E3A:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC09E42: case 0xC09E4E: case 0xC09E5A: case 0xC09E66:
    case 0xC09E72: case 0xC09E7E: case 0xC09E8A: case 0xC09E90:
    case 0xC0A0B4: case 0xC0A0BA: case 0xC0A3D6:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC09E46: case 0xC09E52: case 0xC09E5E: case 0xC09E6A:
    case 0xC09E76: case 0xC09E82: case 0xC09E8E: case 0xC09EBE:
    case 0xC09FA6: case 0xC09FD0: case 0xC09FF0: case 0xC09FFC:
    case 0xC0A036: case 0xC0A074: case 0xC0A07C: case 0xC0A0B0:
    case 0xC0A110: case 0xC0A128: case 0xC0A1C0: case 0xC0A1DA:
    case 0xC0A2A6: case 0xC0A2D0: case 0xC0A2E8: case 0xC0A32E:
    case 0xC0A35E: case 0xC0A3E6: case 0xC0A412:
        step_branch(pc,opcode,1); break;
    case 0xC09E94: case 0xC09E96: case 0xC09EC2: case 0xC09FD4:
    case 0xC0A12C: case 0xC0A158: case 0xC0A15A: case 0xC0A1C4:
    case 0xC0A1DE: case 0xC0A2EC: case 0xC0A2EE: case 0xC0A332:
    case 0xC0A362: case 0xC0A3C4: case 0xC0A3E8: case 0xC0A426:
    case 0xC0A42A:
        REG_PC=m68ki_pull_32(); break;
    case 0xC09EA8: case 0xC0A048: case 0xC0A062: case 0xC0A06C:
    case 0xC0A086: case 0xC0A090: case 0xC0A09A: case 0xC0A0D4:
    case 0xC0A190: case 0xC0A198:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC09EBC: case 0xC09FCE: case 0xC09FFA: case 0xC0A10E:
    case 0xC0A126: case 0xC0A12E: case 0xC0A1BE: case 0xC0A1D8:
    case 0xC0A2CE: case 0xC0A2E6: case 0xC0A32C: case 0xC0A35C:
    case 0xC0A3E4: case 0xC0A424: case 0xC0A428:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC09ECE: case 0xC0A00C: case 0xC0A012: case 0xC0A0B2:
    case 0xC0A0B8: case 0xC0A136: case 0xC0A16E: case 0xC0A1EA:
    case 0xC0A2F8: case 0xC0A33C: case 0xC0A3EA:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC09ED4: case 0xC09EDC: case 0xC09F8E: case 0xC09FB0:
    case 0xC0A0A2: case 0xC0A0C6: case 0xC0A144: case 0xC0A148:
    case 0xC0A14C: case 0xC0A150: case 0xC0A166: case 0xC0A1F0:
    case 0xC0A1F8: case 0xC0A28E: case 0xC0A2B0: case 0xC0A306:
    case 0xC0A3A6:
        width=2; goto move;
    case 0xC09EE0: case 0xC0A1FC:
        width=2; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC09EF6: case 0xC0A042: case 0xC0A30E:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC09EFC: case 0xC09F32: case 0xC0A08C: case 0xC0A096:
    case 0xC0A0A0: case 0xC0A0E2: case 0xC0A182: case 0xC0A1A2:
    case 0xC0A1AA: case 0xC0A232: case 0xC0A37A:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC09F22: case 0xC0A0F4: case 0xC0A222: case 0xC0A38C:
    case 0xC0A3D2:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC09F4C: case 0xC0A24C:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='|'; goto bit_value;
    case 0xC09F96: case 0xC0A296:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC0A034:
        value=A(destination); A(destination)=A(reg); A(reg)=value; break;
    case 0xC0A04C: case 0xC0A052: case 0xC0A184:
        mask=m68ki_read_imm_16(); address=cache_step_address(mode,reg,4); renderer_load(address,mask,4,mode==3?(int)reg:-1); break;
    case 0xC0A06E: case 0xC0A076: case 0xC0A07E: case 0xC0A154:
        width=4; goto move;
    case 0xC0A084: case 0xC0A08E: case 0xC0A098: case 0xC0A18C:
    case 0xC0A194:
        width=4; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC0A088: case 0xC0A092: case 0xC0A09C: case 0xC0A192:
    case 0xC0A19A:
        renderer_negate(&D(reg),4); break;
    case 0xC0A08A: case 0xC0A094: case 0xC0A09E:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC0A0A8: case 0xC0A0C4:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC0A0AA:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0A0BE: case 0xC0A0CE: case 0xC0A17E: case 0xC0A314:
    case 0xC0A41E:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC0A0DC: case 0xC0A374:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;
    case 0xC0A134:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC0A13C: case 0xC0A174:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC0A140:
        mask=m68ki_read_imm_16(); address=cache_step_address(mode,reg,2); renderer_load(address,mask,2,mode==3?(int)reg:-1); break;
    case 0xC0A19C: case 0xC0A1A4:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC0A30A:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC0A418:
        width=1; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
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
static const uint32_t owned_C09E06[]={
    0xC09E06,0xC09E0C,0xC09E12,0xC09E18,0xC09E1E,0xC09E26,0xC09E28,0xC09E2E,
    0xC09E30,0xC09E36,0xC09E3A,0xC09E3C,0xC09E40,0xC09E42,0xC09E46,0xC09E48,
    0xC09E4C,0xC09E4E,0xC09E52,0xC09E54,0xC09E58,0xC09E5A,0xC09E5E,0xC09E60,
    0xC09E64,0xC09E66,0xC09E6A,0xC09E6C,0xC09E70,0xC09E72,0xC09E76,0xC09E78,
    0xC09E7C,0xC09E7E,0xC09E82,0xC09E84,0xC09E88,0xC09E8A,0xC09E8E,0xC09E90,
    0xC09E94,
};
int glue_C09E06_owns(uint32_t pc) { return owns_pc(owned_C09E06,sizeof owned_C09E06/sizeof owned_C09E06[0],pc); }
int glue_C09E06_step(void) { if(!glue_C09E06_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C09E98[]={
    0xC09E96,0xC09E98,0xC09E9E,0xC09EA2,0xC09EA8,0xC09EAA,0xC09EB2,0xC09EB4,
    0xC09EBC,0xC09EBE,0xC0A3A6,0xC0A3AC,0xC0A3B4,0xC0A3BC,0xC0A3C4,0xC0A3C6,
    0xC0A3CC,0xC0A3D0,0xC0A3D2,0xC0A3D4,0xC0A3D6,0xC0A3DA,0xC0A3DC,0xC0A3E4,
    0xC0A3E6,0xC0A3E8,
};
int glue_C09E98_owns(uint32_t pc) { return owns_pc(owned_C09E98,sizeof owned_C09E98/sizeof owned_C09E98[0],pc); }
int glue_C09E98_step(void) { if(!glue_C09E98_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C09EC4[]={
    0xC09EC2,0xC09EC4,0xC09ECA,0xC09ECE,0xC09ED4,0xC09EDC,0xC09EE0,0xC09EE4,
    0xC09EE6,0xC09EEC,0xC09EEE,0xC09EF4,0xC09EF6,0xC09EFC,0xC09EFE,0xC09F06,
    0xC09F08,0xC09F10,0xC09F12,0xC09F18,0xC09F1A,0xC09F22,0xC09F28,0xC09F2A,
    0xC09F32,0xC09F36,0xC09F3E,0xC09F42,0xC09F4C,0xC09F54,0xC09F5E,0xC09F64,
    0xC09F68,0xC09F70,0xC09F78,0xC09F80,0xC09F86,0xC09F8C,0xC09F8E,0xC09F96,
    0xC09F9E,0xC09FA6,0xC09FA8,0xC09FAE,0xC09FB0,0xC09FB6,0xC09FBC,0xC09FC4,
    0xC09FC6,0xC09FCE,0xC09FD0,0xC09FD4,0xC09FD6,0xC09FDE,0xC09FE0,0xC09FE6,
    0xC09FE8,0xC09FF0,0xC09FF2,0xC09FFA,0xC09FFC,0xC0A3A6,0xC0A3AC,0xC0A3B4,
    0xC0A3BC,0xC0A3C4,0xC0A3C6,0xC0A3CC,0xC0A3D0,0xC0A3D2,0xC0A3D4,0xC0A3D6,
    0xC0A3DA,0xC0A3DC,0xC0A3E4,0xC0A3E6,0xC0A3E8,
};
int glue_C09EC4_owns(uint32_t pc) { return owns_pc(owned_C09EC4,sizeof owned_C09EC4/sizeof owned_C09EC4[0],pc); }
int glue_C09EC4_step(void) { if(!glue_C09EC4_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C0A002[]={
    0xC0A002,0xC0A008,0xC0A00C,0xC0A012,0xC0A018,0xC0A01E,0xC0A020,0xC0A026,
    0xC0A02A,0xC0A030,0xC0A034,0xC0A036,0xC0A038,0xC0A03E,0xC0A042,0xC0A048,
    0xC0A04C,0xC0A052,0xC0A05A,0xC0A062,0xC0A064,0xC0A06C,0xC0A06E,0xC0A074,
    0xC0A076,0xC0A07C,0xC0A07E,0xC0A084,0xC0A086,0xC0A088,0xC0A08A,0xC0A08C,
    0xC0A08E,0xC0A090,0xC0A092,0xC0A094,0xC0A096,0xC0A098,0xC0A09A,0xC0A09C,
    0xC0A09E,0xC0A0A0,0xC0A0A2,0xC0A0A8,0xC0A0AA,0xC0A0B0,0xC0A0B2,0xC0A0B4,
    0xC0A0B8,0xC0A0BA,0xC0A0BE,0xC0A0C4,0xC0A0C6,0xC0A0CE,0xC0A0D4,0xC0A0D6,
    0xC0A0DC,0xC0A0E2,0xC0A0E4,0xC0A0EA,0xC0A0EC,0xC0A0F4,0xC0A0FA,0xC0A0FC,
    0xC0A104,0xC0A106,0xC0A10E,0xC0A110,0xC0A114,0xC0A11C,0xC0A11E,0xC0A126,
    0xC0A128,0xC0A12C,0xC0A3A6,0xC0A3AC,0xC0A3B4,0xC0A3BC,0xC0A3C4,0xC0A3C6,
    0xC0A3CC,0xC0A3D0,0xC0A3D2,0xC0A3D4,0xC0A3D6,0xC0A3DA,0xC0A3DC,0xC0A3E4,
    0xC0A3E6,0xC0A3E8,
};
int glue_C0A002_owns(uint32_t pc) { return owns_pc(owned_C0A002,sizeof owned_C0A002/sizeof owned_C0A002[0],pc); }
int glue_C0A002_step(void) { if(!glue_C0A002_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C0A12E[]={
    0xC0A12E,0xC0A130,0xC0A134,0xC0A136,0xC0A13C,0xC0A140,0xC0A144,0xC0A148,
    0xC0A14C,0xC0A150,0xC0A154,0xC0A158,
};
int glue_C0A12E_owns(uint32_t pc) { return owns_pc(owned_C0A12E,sizeof owned_C0A12E/sizeof owned_C0A12E[0],pc); }
int glue_C0A12E_step(void) { if(!glue_C0A12E_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C0A15C[]={
    0xC0A15A,0xC0A15C,0xC0A162,0xC0A166,0xC0A16C,0xC0A16E,0xC0A174,0xC0A176,
    0xC0A17C,0xC0A17E,0xC0A182,0xC0A184,0xC0A18C,0xC0A190,0xC0A192,0xC0A194,
    0xC0A198,0xC0A19A,0xC0A19C,0xC0A1A2,0xC0A1A4,0xC0A1AA,0xC0A1AC,0xC0A1B4,
    0xC0A1B6,0xC0A1BE,0xC0A1C0,0xC0A1C4,0xC0A1C6,0xC0A1CE,0xC0A1D0,0xC0A1D8,
    0xC0A1DA,0xC0A3A6,0xC0A3AC,0xC0A3B4,0xC0A3BC,0xC0A3C4,0xC0A3C6,0xC0A3CC,
    0xC0A3D0,0xC0A3D2,0xC0A3D4,0xC0A3D6,0xC0A3DA,0xC0A3DC,0xC0A3E4,0xC0A3E6,
    0xC0A3E8,
};
int glue_C0A15C_owns(uint32_t pc) { return owns_pc(owned_C0A15C,sizeof owned_C0A15C/sizeof owned_C0A15C[0],pc); }
int glue_C0A15C_step(void) { if(!glue_C0A15C_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C0A1E0[]={
    0xC0A1DE,0xC0A1E0,0xC0A1E6,0xC0A1EA,0xC0A1F0,0xC0A1F8,0xC0A1FC,0xC0A200,
    0xC0A202,0xC0A208,0xC0A20A,0xC0A210,0xC0A212,0xC0A218,0xC0A21A,0xC0A222,
    0xC0A228,0xC0A22A,0xC0A232,0xC0A236,0xC0A23E,0xC0A242,0xC0A24C,0xC0A254,
    0xC0A25E,0xC0A264,0xC0A268,0xC0A270,0xC0A278,0xC0A280,0xC0A286,0xC0A28C,
    0xC0A28E,0xC0A296,0xC0A29E,0xC0A2A6,0xC0A2A8,0xC0A2AE,0xC0A2B0,0xC0A2B6,
    0xC0A2BC,0xC0A2C4,0xC0A2C6,0xC0A2CE,0xC0A2D0,0xC0A2D4,0xC0A2DC,0xC0A2DE,
    0xC0A2E6,0xC0A2E8,0xC0A2EC,0xC0A3A6,0xC0A3AC,0xC0A3B4,0xC0A3BC,0xC0A3C4,
    0xC0A3C6,0xC0A3CC,0xC0A3D0,0xC0A3D2,0xC0A3D4,0xC0A3D6,0xC0A3DA,0xC0A3DC,
    0xC0A3E4,0xC0A3E6,0xC0A3E8,
};
int glue_C0A1E0_owns(uint32_t pc) { return owns_pc(owned_C0A1E0,sizeof owned_C0A1E0/sizeof owned_C0A1E0[0],pc); }
int glue_C0A1E0_step(void) { if(!glue_C0A1E0_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C0A2F0[]={
    0xC0A2EE,0xC0A2F0,0xC0A2F6,0xC0A2F8,0xC0A2FE,0xC0A304,0xC0A306,0xC0A30A,
    0xC0A30E,0xC0A312,0xC0A314,0xC0A318,0xC0A31A,0xC0A322,0xC0A324,0xC0A32C,
    0xC0A32E,0xC0A3A6,0xC0A3AC,0xC0A3B4,0xC0A3BC,0xC0A3C4,
};
int glue_C0A2F0_owns(uint32_t pc) { return owns_pc(owned_C0A2F0,sizeof owned_C0A2F0/sizeof owned_C0A2F0[0],pc); }
int glue_C0A2F0_step(void) { if(!glue_C0A2F0_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C0A334[]={
    0xC0A332,0xC0A334,0xC0A33A,0xC0A33C,0xC0A342,0xC0A348,0xC0A34A,0xC0A352,
    0xC0A354,0xC0A35C,0xC0A35E,0xC0A3A6,0xC0A3AC,0xC0A3B4,0xC0A3BC,0xC0A3C4,
};
int glue_C0A334_owns(uint32_t pc) { return owns_pc(owned_C0A334,sizeof owned_C0A334/sizeof owned_C0A334[0],pc); }
int glue_C0A334_step(void) { if(!glue_C0A334_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C0A364[]={
    0xC0A362,0xC0A364,0xC0A36A,0xC0A36E,0xC0A374,0xC0A37A,0xC0A37C,0xC0A382,
    0xC0A384,0xC0A38C,0xC0A392,0xC0A394,0xC0A39C,0xC0A39E,0xC0A3A6,0xC0A3AC,
    0xC0A3B4,0xC0A3BC,0xC0A3C4,0xC0A3C6,0xC0A3CC,0xC0A3D0,0xC0A3D2,0xC0A3D4,
    0xC0A3D6,0xC0A3DA,0xC0A3DC,0xC0A3E4,0xC0A3E6,0xC0A3E8,
};
int glue_C0A364_owns(uint32_t pc) { return owns_pc(owned_C0A364,sizeof owned_C0A364/sizeof owned_C0A364[0],pc); }
int glue_C0A364_step(void) { if(!glue_C0A364_owns(REG_PC)) return 0; return schedule_step(); }
static const uint32_t owned_C0A3EA[]={
    0xC0A3EA,0xC0A3F0,0xC0A3F6,0xC0A3F8,0xC0A3FE,0xC0A400,0xC0A408,0xC0A40A,
    0xC0A410,0xC0A412,0xC0A414,0xC0A418,0xC0A41C,0xC0A41E,0xC0A422,0xC0A424,
    0xC0A426,0xC0A428,0xC0A42A,
};
int glue_C0A3EA_owns(uint32_t pc) { return owns_pc(owned_C0A3EA,sizeof owned_C0A3EA/sizeof owned_C0A3EA[0],pc); }
int glue_C0A3EA_step(void) { if(!glue_C0A3EA_owns(REG_PC)) return 0; return schedule_step(); }
