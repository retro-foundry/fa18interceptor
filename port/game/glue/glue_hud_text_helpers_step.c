/* Complete hud text helpers family source CPU/bus/event boundaries.
 * Readable behavior lives in hud_text_helpers.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family hud_text_helpers. */
#include "glue_hud_text_helpers_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_hud_text_helpers.h"

static int hud_text_helpers_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC31C20: case 0xC31C3A: case 0xC32772: case 0xC32AEA:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC31C26:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC31C28: case 0xC31C50: case 0xC31C54: case 0xC3271C:
    case 0xC32722: case 0xC32728: case 0xC32736: case 0xC32742:
    case 0xC3274E: case 0xC32758: case 0xC32776: case 0xC32796:
    case 0xC327A6: case 0xC327A8: case 0xC327AC: case 0xC327DA:
    case 0xC327F6: case 0xC32AA6: case 0xC32AB4: case 0xC32AD6:
    case 0xC32AD8: case 0xC32B02: case 0xC32B0E: case 0xC32B10:
    case 0xC32B1E: case 0xC32B5A: case 0xC32B68: case 0xC32B78:
    case 0xC32B86: case 0xC32B96: case 0xC32BA4: case 0xC32BB4:
    case 0xC32BC2:
        width=2; goto move;
    case 0xC31C2A: case 0xC327B2: case 0xC32B26:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC31C2C:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC31C2E: case 0xC32B18: case 0xC32B56: case 0xC32B66:
    case 0xC32B84: case 0xC32BA2: case 0xC32BC0:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC31C30: case 0xC327E0: case 0xC32B52: case 0xC32B5E:
    case 0xC32B7C: case 0xC32B9A: case 0xC32BB8:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC31C38: case 0xC31C40: case 0xC32774: case 0xC3277E:
    case 0xC327E4: case 0xC32AEC: case 0xC32AF4:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC31C42: case 0xC31C52: case 0xC32724: case 0xC32730:
    case 0xC3273A: case 0xC3278A: case 0xC32804: case 0xC32AB2:
    case 0xC32AC0: case 0xC32B58:
        step_branch(pc,opcode,1); break;
    case 0xC31C44: case 0xC32720: case 0xC3272C: case 0xC3272E:
    case 0xC32750: case 0xC32AA4:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC31C46: case 0xC31C5C: case 0xC327F4: case 0xC32BD0:
        REG_PC=m68ki_pull_32(); break;
    case 0xC31C48: case 0xC31C4C: case 0xC3275A: case 0xC327C2:
    case 0xC32ADA: case 0xC32B38:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC31C56:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC31C5A:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC3271A: case 0xC32726: case 0xC32740: case 0xC327D8:
    case 0xC327DC: case 0xC32B1C: case 0xC32B20:
        step_swap(&D(reg)); break;
    case 0xC32748: case 0xC32752: case 0xC327A0: case 0xC327D6:
    case 0xC327E6: case 0xC327E8: case 0xC327EE: case 0xC32AAC:
    case 0xC32ABA: case 0xC32AD0: case 0xC32B4C: case 0xC32B4E:
    case 0xC32B72: case 0xC32B90: case 0xC32BAE:
        width=4; goto move;
    case 0xC3275E: case 0xC32ADE:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC32762: case 0xC327B4: case 0xC32B2A:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC32766:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC32768:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC3276A: case 0xC32780: case 0xC327AA: case 0xC32AE2:
    case 0xC32AF6: case 0xC32B12:
        width=1; goto move;
    case 0xC3276C: case 0xC32AE4:
        step_lsr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC3276E: case 0xC32786: case 0xC327F0: case 0xC32AE6:
    case 0xC32BCC:
        step_dbf(pc,&D(reg)); break;
    case 0xC32778: case 0xC32AEE: case 0xC32AFC:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC3277A: case 0xC32AF0: case 0xC32B14:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC32794: case 0xC32B00:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC3279A: case 0xC32B06:
        width=4; goto move;
    case 0xC327A4: case 0xC327AE: case 0xC327B0: case 0xC327BA:
    case 0xC327CA: case 0xC32B0C: case 0xC32B22: case 0xC32B24:
    case 0xC32B32: case 0xC32B40:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC327B8: case 0xC32AFE: case 0xC32B2E:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC327BC: case 0xC32B34:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC327BE: case 0xC327C0: case 0xC32B36: case 0xC32B50:
    case 0xC32B76: case 0xC32B94: case 0xC32BB2:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC327C6: case 0xC32B3C:
        width=2; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC327CC: case 0xC32B42:
        A(destination)=(mode==7 && reg==0)?(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16():cache_step_address(mode,reg,4); break;
    case 0xC327D2: case 0xC32B48:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC327DE: case 0xC32B6C: case 0xC32B8A: case 0xC32BA8:
    case 0xC32BC6:
        width=2; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC327EA: case 0xC32B6E: case 0xC32B8C: case 0xC32BAA:
    case 0xC32BC8:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC327FE:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
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
static const uint32_t owned_C31C20[]={
    0xC31C20,0xC31C26,0xC31C28,0xC31C2A,0xC31C2C,0xC31C2E,0xC31C30,0xC31C38,
    0xC31C3A,0xC31C40,0xC31C42,0xC31C44,0xC31C46,0xC31C48,0xC31C4C,0xC31C50,
    0xC31C52,0xC31C54,0xC31C56,0xC31C5A,0xC31C5C,
};
int glue_C31C20_owns(uint32_t pc) { return owns_pc(owned_C31C20,sizeof owned_C31C20/sizeof owned_C31C20[0],pc); }
int glue_C31C20_complete_step(void) { if(!glue_C31C20_owns(REG_PC)) return 0; return hud_text_helpers_step(); }
static const uint32_t owned_C3271A[]={
    0xC3271A,0xC3271C,0xC32720,0xC32722,0xC32724,0xC32750,0xC32752,0xC32758,
    0xC3275A,0xC3275E,0xC32762,0xC32766,0xC32768,0xC3276A,0xC3276C,0xC3276E,
    0xC32772,0xC32774,0xC32776,0xC32778,0xC3277A,0xC3277E,0xC32780,0xC32786,
    0xC3278A,0xC32794,0xC32796,0xC3279A,0xC327A0,0xC327A4,0xC327A6,0xC327A8,
    0xC327AA,0xC327AC,0xC327AE,0xC327B0,0xC327B2,0xC327B4,0xC327B8,0xC327BA,
    0xC327BC,0xC327BE,0xC327C0,0xC327C2,0xC327C6,0xC327CA,0xC327CC,0xC327D2,
    0xC327D6,0xC327D8,0xC327DA,0xC327DC,0xC327DE,0xC327E0,0xC327E4,0xC327E6,
    0xC327E8,0xC327EA,0xC327EE,0xC327F0,0xC327F4,0xC327F6,0xC327FE,0xC32804,
};
int glue_C3271A_owns(uint32_t pc) { return owns_pc(owned_C3271A,sizeof owned_C3271A/sizeof owned_C3271A[0],pc); }
int glue_C3271A_complete_step(void) { if(!glue_C3271A_owns(REG_PC)) return 0; return hud_text_helpers_step(); }
static const uint32_t owned_C32726[]={
    0xC32726,0xC32728,0xC3272C,0xC3272E,0xC32730,0xC32752,0xC32758,0xC3275A,
    0xC3275E,0xC32762,0xC32766,0xC32768,0xC3276A,0xC3276C,0xC3276E,0xC32772,
    0xC32774,0xC32776,0xC32778,0xC3277A,0xC3277E,0xC32780,0xC32786,0xC3278A,
    0xC32794,0xC32796,0xC3279A,0xC327A0,0xC327A4,0xC327A6,0xC327A8,0xC327AA,
    0xC327AC,0xC327AE,0xC327B0,0xC327B2,0xC327B4,0xC327B8,0xC327BA,0xC327BC,
    0xC327BE,0xC327C0,0xC327C2,0xC327C6,0xC327CA,0xC327CC,0xC327D2,0xC327D6,
    0xC327D8,0xC327DA,0xC327DC,0xC327DE,0xC327E0,0xC327E4,0xC327E6,0xC327E8,
    0xC327EA,0xC327EE,0xC327F0,0xC327F4,0xC327F6,0xC327FE,0xC32804,
};
int glue_C32726_owns(uint32_t pc) { return owns_pc(owned_C32726,sizeof owned_C32726/sizeof owned_C32726[0],pc); }
int glue_C32726_complete_step(void) { if(!glue_C32726_owns(REG_PC)) return 0; return hud_text_helpers_step(); }
static const uint32_t owned_C32736[]={
    0xC32736,0xC3273A,0xC32740,0xC32742,0xC32748,0xC3274E,0xC32750,0xC32752,
    0xC32758,0xC3275A,0xC3275E,0xC32762,0xC32766,0xC32768,0xC3276A,0xC3276C,
    0xC3276E,0xC32772,0xC32774,0xC32776,0xC32778,0xC3277A,0xC3277E,0xC32780,
    0xC32786,0xC3278A,0xC32794,0xC32796,0xC3279A,0xC327A0,0xC327A4,0xC327A6,
    0xC327A8,0xC327AA,0xC327AC,0xC327AE,0xC327B0,0xC327B2,0xC327B4,0xC327B8,
    0xC327BA,0xC327BC,0xC327BE,0xC327C0,0xC327C2,0xC327C6,0xC327CA,0xC327CC,
    0xC327D2,0xC327D6,0xC327D8,0xC327DA,0xC327DC,0xC327DE,0xC327E0,0xC327E4,
    0xC327E6,0xC327E8,0xC327EA,0xC327EE,0xC327F0,0xC327F4,0xC327F6,0xC327FE,
    0xC32804,
};
int glue_C32736_owns(uint32_t pc) { return owns_pc(owned_C32736,sizeof owned_C32736/sizeof owned_C32736[0],pc); }
int glue_C32736_complete_step(void) { if(!glue_C32736_owns(REG_PC)) return 0; return hud_text_helpers_step(); }
static const uint32_t owned_C32794[]={
    0xC32794,0xC32796,0xC3279A,0xC327A0,0xC327A4,0xC327A6,0xC327A8,0xC327AA,
    0xC327AC,0xC327AE,0xC327B0,0xC327B2,0xC327B4,0xC327B8,0xC327BA,0xC327BC,
    0xC327BE,0xC327C0,0xC327C2,0xC327C6,0xC327CA,0xC327CC,0xC327D2,0xC327D6,
    0xC327D8,0xC327DA,0xC327DC,0xC327DE,0xC327E0,0xC327E4,0xC327E6,0xC327E8,
    0xC327EA,0xC327EE,0xC327F0,0xC327F4,0xC327F6,0xC327FE,0xC32804,
};
int glue_C32794_owns(uint32_t pc) { return owns_pc(owned_C32794,sizeof owned_C32794/sizeof owned_C32794[0],pc); }
int glue_C32794_complete_step(void) { if(!glue_C32794_owns(REG_PC)) return 0; return hud_text_helpers_step(); }
static const uint32_t owned_C32AA4[]={
    0xC32AA4,0xC32AA6,0xC32AAC,0xC32AB2,0xC32AD0,0xC32AD6,0xC32AD8,0xC32ADA,
    0xC32ADE,0xC32AE2,0xC32AE4,0xC32AE6,0xC32AEA,0xC32AEC,0xC32AEE,0xC32AF0,
    0xC32AF4,0xC32AF6,0xC32AFC,0xC32AFE,0xC32B00,0xC32B02,0xC32B06,0xC32B0C,
    0xC32B0E,0xC32B10,0xC32B12,0xC32B14,0xC32B18,0xC32B1C,0xC32B1E,0xC32B20,
    0xC32B22,0xC32B24,0xC32B26,0xC32B2A,0xC32B2E,0xC32B32,0xC32B34,0xC32B36,
    0xC32B38,0xC32B3C,0xC32B40,0xC32B42,0xC32B48,0xC32B4C,0xC32B4E,0xC32B50,
    0xC32B52,0xC32B56,0xC32B58,0xC32B5A,0xC32B5E,0xC32B66,0xC32B68,0xC32B6C,
    0xC32B6E,0xC32B72,0xC32B76,0xC32B78,0xC32B7C,0xC32B84,0xC32B86,0xC32B8A,
    0xC32B8C,0xC32B90,0xC32B94,0xC32B96,0xC32B9A,0xC32BA2,0xC32BA4,0xC32BA8,
    0xC32BAA,0xC32BAE,0xC32BB2,0xC32BB4,0xC32BB8,0xC32BC0,0xC32BC2,0xC32BC6,
    0xC32BC8,0xC32BCC,0xC32BD0,
};
int glue_C32AA4_owns(uint32_t pc) { return owns_pc(owned_C32AA4,sizeof owned_C32AA4/sizeof owned_C32AA4[0],pc); }
int glue_C32AA4_complete_step(void) { if(!glue_C32AA4_owns(REG_PC)) return 0; return hud_text_helpers_step(); }
static const uint32_t owned_C32AA6[]={
    0xC32AA6,0xC32AAC,0xC32AB2,0xC32AD0,0xC32AD6,0xC32AD8,0xC32ADA,0xC32ADE,
    0xC32AE2,0xC32AE4,0xC32AE6,0xC32AEA,0xC32AEC,0xC32AEE,0xC32AF0,0xC32AF4,
    0xC32AF6,0xC32AFC,0xC32AFE,0xC32B00,0xC32B02,0xC32B06,0xC32B0C,0xC32B0E,
    0xC32B10,0xC32B12,0xC32B14,0xC32B18,0xC32B1C,0xC32B1E,0xC32B20,0xC32B22,
    0xC32B24,0xC32B26,0xC32B2A,0xC32B2E,0xC32B32,0xC32B34,0xC32B36,0xC32B38,
    0xC32B3C,0xC32B40,0xC32B42,0xC32B48,0xC32B4C,0xC32B4E,0xC32B50,0xC32B52,
    0xC32B56,0xC32B58,0xC32B5A,0xC32B5E,0xC32B66,0xC32B68,0xC32B6C,0xC32B6E,
    0xC32B72,0xC32B76,0xC32B78,0xC32B7C,0xC32B84,0xC32B86,0xC32B8A,0xC32B8C,
    0xC32B90,0xC32B94,0xC32B96,0xC32B9A,0xC32BA2,0xC32BA4,0xC32BA8,0xC32BAA,
    0xC32BAE,0xC32BB2,0xC32BB4,0xC32BB8,0xC32BC0,0xC32BC2,0xC32BC6,0xC32BC8,
    0xC32BCC,0xC32BD0,
};
int glue_C32AA6_owns(uint32_t pc) { return owns_pc(owned_C32AA6,sizeof owned_C32AA6/sizeof owned_C32AA6[0],pc); }
int glue_C32AA6_complete_step(void) { if(!glue_C32AA6_owns(REG_PC)) return 0; return hud_text_helpers_step(); }
static const uint32_t owned_C32AB4[]={
    0xC32AB4,0xC32ABA,0xC32AC0,0xC32B00,0xC32B02,0xC32B06,0xC32B0C,0xC32B0E,
    0xC32B10,0xC32B12,0xC32B14,0xC32B18,0xC32B1C,0xC32B1E,0xC32B20,0xC32B22,
    0xC32B24,0xC32B26,0xC32B2A,0xC32B2E,0xC32B32,0xC32B34,0xC32B36,0xC32B38,
    0xC32B3C,0xC32B40,0xC32B42,0xC32B48,0xC32B4C,0xC32B4E,0xC32B50,0xC32B52,
    0xC32B56,0xC32B58,0xC32B5A,0xC32B5E,0xC32B66,0xC32B68,0xC32B6C,0xC32B6E,
    0xC32B72,0xC32B76,0xC32B78,0xC32B7C,0xC32B84,0xC32B86,0xC32B8A,0xC32B8C,
    0xC32B90,0xC32B94,0xC32B96,0xC32B9A,0xC32BA2,0xC32BA4,0xC32BA8,0xC32BAA,
    0xC32BAE,0xC32BB2,0xC32BB4,0xC32BB8,0xC32BC0,0xC32BC2,0xC32BC6,0xC32BC8,
    0xC32BCC,0xC32BD0,
};
int glue_C32AB4_owns(uint32_t pc) { return owns_pc(owned_C32AB4,sizeof owned_C32AB4/sizeof owned_C32AB4[0],pc); }
int glue_C32AB4_complete_step(void) { if(!glue_C32AB4_owns(REG_PC)) return 0; return hud_text_helpers_step(); }
