/* Complete flight markers family source CPU/bus/event boundaries.
 * Readable behavior lives in flight_markers.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family flight_markers. */
#include "glue_flight_markers_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_flight_markers.h"

static int flight_markers_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC2AFFA: case 0xC2B48A: case 0xC2B48E: case 0xC2B578:
    case 0xC2B5A8: case 0xC2B618: case 0xC2B61E: case 0xC2B67C:
    case 0xC2B6DE: case 0xC2B6E4: case 0xC2B72E: case 0xC2B768:
    case 0xC2B7C6: case 0xC2B7C8: case 0xC2B7D2: case 0xC2B95A:
    case 0xC2B95E: case 0xC2B988: case 0xC2BADE:
        width=4; goto move;
    case 0xC2AFFC: case 0xC2B496: case 0xC2B49A: case 0xC2B594:
    case 0xC2B59C: case 0xC2B59E: case 0xC2B5A4: case 0xC2B620:
    case 0xC2B67E: case 0xC2B684: case 0xC2B6E6: case 0xC2B75E:
    case 0xC2B760: case 0xC2B962: case 0xC2B968:
        step_swap(&D(reg)); break;
    case 0xC2AFFE: case 0xC2B00A: case 0xC2B00C: case 0xC2B00E:
    case 0xC2B01C: case 0xC2B01E: case 0xC2B020: case 0xC2B022:
    case 0xC2B030: case 0xC2B03E: case 0xC2B3FE: case 0xC2B40E:
    case 0xC2B428: case 0xC2B4EC: case 0xC2B4EE: case 0xC2B4F0:
    case 0xC2B4FE: case 0xC2B500: case 0xC2B502: case 0xC2B520:
    case 0xC2B52E: case 0xC2B536: case 0xC2B53E: case 0xC2B546:
    case 0xC2B582: case 0xC2B598: case 0xC2B5A6: case 0xC2B5AA:
    case 0xC2B5AE: case 0xC2B5B4: case 0xC2B5BA: case 0xC2B5C6:
    case 0xC2B5CA: case 0xC2B5CE: case 0xC2B5D8: case 0xC2B5DC:
    case 0xC2B5E0: case 0xC2B5EE: case 0xC2B5FC: case 0xC2B60A:
    case 0xC2B63A: case 0xC2B63E: case 0xC2B66A: case 0xC2B670:
    case 0xC2B676: case 0xC2B680: case 0xC2B68C: case 0xC2B690:
    case 0xC2B694: case 0xC2B69E: case 0xC2B6A2: case 0xC2B6A6:
    case 0xC2B6B4: case 0xC2B6C2: case 0xC2B6D0: case 0xC2B6F8:
    case 0xC2B702: case 0xC2B73A: case 0xC2B73E: case 0xC2B76A:
    case 0xC2B76C: case 0xC2B77A: case 0xC2B7A8: case 0xC2B7B2:
    case 0xC2B7BC: case 0xC2B7C4: case 0xC2B7CA: case 0xC2B7D0:
    case 0xC2B940: case 0xC2B964: case 0xC2B9CA: case 0xC2BA4E:
    case 0xC2BA52: case 0xC2BA5C: case 0xC2BA74: case 0xC2BA76:
    case 0xC2BAA2: case 0xC2BABC: case 0xC2BABE: case 0xC2BAC0:
    case 0xC2BAC2: case 0xC2BAE8: case 0xC2BAEA:
        width=2; goto move;
    case 0xC2B000: case 0xC2B002: case 0xC2B476: case 0xC2BA1E:
    case 0xC2BA44: case 0xC2BA9C: case 0xC2BAC4: case 0xC2BACE:
    case 0xC2BAD0: case 0xC2BAD4: case 0xC2BAD6:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC2B004: case 0xC2B3F0: case 0xC2B3F8: case 0xC2B404:
    case 0xC2B408: case 0xC2B478: case 0xC2B4A0: case 0xC2B4E6:
    case 0xC2B5C0: case 0xC2B686: case 0xC2B732: case 0xC2B762:
    case 0xC2B948: case 0xC2BA9E:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC2B010: case 0xC2B012: case 0xC2B014: case 0xC2B024:
    case 0xC2B026: case 0xC2B028: case 0xC2B032: case 0xC2B034:
    case 0xC2B036: case 0xC2B4F2: case 0xC2B4F4: case 0xC2B4F6:
    case 0xC2B504: case 0xC2B506: case 0xC2B508: case 0xC2B510:
    case 0xC2B512: case 0xC2B514:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC2B016: case 0xC2B018: case 0xC2B02A: case 0xC2B02C:
    case 0xC2B038: case 0xC2B03A: case 0xC2B4B4: case 0xC2B4B6:
    case 0xC2B4C8: case 0xC2B4CA: case 0xC2B4CC: case 0xC2B4CE:
    case 0xC2B4F8: case 0xC2B4FA: case 0xC2B50A: case 0xC2B50C:
    case 0xC2B516: case 0xC2B518:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC2B01A: case 0xC2B02E: case 0xC2B03C: case 0xC2B4E0:
    case 0xC2B4E2: case 0xC2B4E4: case 0xC2B4FC: case 0xC2B50E:
    case 0xC2B51A:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC2B040: case 0xC2B3C0: case 0xC2B45E: case 0xC2B562:
    case 0xC2B7E2: case 0xC2B950: case 0xC2BAEE:
        REG_PC=m68ki_pull_32(); break;
    case 0xC2B3C2: case 0xC2B3CC: case 0xC2B416: case 0xC2B436:
    case 0xC2B78A: case 0xC2B796: case 0xC2BA04: case 0xC2BA10:
    case 0xC2BA2A: case 0xC2BA36: case 0xC2BA62: case 0xC2BAAA:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC2B3CA: case 0xC2B426: case 0xC2B452: case 0xC2B754:
    case 0xC2B78E: case 0xC2B79E: case 0xC2B7B0: case 0xC2B996:
    case 0xC2B9C6:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC2B3D4: case 0xC2B53C: case 0xC2B62C: case 0xC2B632:
    case 0xC2B638: case 0xC2B6F6: case 0xC2B92E: case 0xC2B93E:
    case 0xC2B99E: case 0xC2B9A4: case 0xC2B9B0: case 0xC2B9D6:
    case 0xC2B9E8: case 0xC2BA14: case 0xC2BA3A: case 0xC2BA5A:
    case 0xC2BAC6: case 0xC2BAD2:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC2B3D6:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC2B3E0: case 0xC2B45A: case 0xC2B466: case 0xC2B6EE:
    case 0xC2B6FE: case 0xC2B97E: case 0xC2BA02: case 0xC2BA28:
    case 0xC2BA6A:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC2B3E2: case 0xC2BA60:
        width=1; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC2B3E8: case 0xC2B420: case 0xC2B564: case 0xC2B56C:
    case 0xC2B928: case 0xC2B998: case 0xC2B9C0: case 0xC2B9E4:
    case 0xC2BA88:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC2B3EE: case 0xC2B434: case 0xC2B43E: case 0xC2B446:
    case 0xC2B46C: case 0xC2B486: case 0xC2B572: case 0xC2B608:
    case 0xC2B610: case 0xC2B65C: case 0xC2B6CE: case 0xC2B6D6:
    case 0xC2B720: case 0xC2B744: case 0xC2B74C: case 0xC2B778:
    case 0xC2B7A6: case 0xC2B9DA: case 0xC2B9E0: case 0xC2BA8E:
    case 0xC2BA98: case 0xC2BAAE:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC2B3F6: case 0xC2B492: case 0xC2B55E: case 0xC2B794:
    case 0xC2B7BA: case 0xC2B94C: case 0xC2B986: case 0xC2BA0E:
    case 0xC2BA34: case 0xC2BA4A: case 0xC2BA6C: case 0xC2BAEC:
        step_branch(pc,opcode,1); break;
    case 0xC2B41E: case 0xC2B56A: case 0xC2B7DC:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC2B42E:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;
    case 0xC2B440: case 0xC2B448: case 0xC2B782: case 0xC2B9B8:
    case 0xC2B9D0: case 0xC2B9EE: case 0xC2B9F2: case 0xC2B9FA:
    case 0xC2BA20: case 0xC2BA46: case 0xC2BA70: case 0xC2BA84:
    case 0xC2BAA8: case 0xC2BAB0: case 0xC2BAB2:
        width=1; goto move;
    case 0xC2B44E: case 0xC2B786: case 0xC2B9F6: case 0xC2B9FC:
    case 0xC2BA22: case 0xC2BA78: case 0xC2BA7C:
        width=1; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC2B454: case 0xC2B47E: case 0xC2B738: case 0xC2BAA6:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC2B458: case 0xC2B464: case 0xC2B60E: case 0xC2B6D4:
    case 0xC2B6FC: case 0xC2B776:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC2B460: case 0xC2B4A6: case 0xC2B4B8: case 0xC2B930:
    case 0xC2B952: case 0xC2B970:
        mask=m68ki_read_imm_16(); address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); break;
    case 0xC2B468: case 0xC2B628: case 0xC2B62E: case 0xC2B634:
    case 0xC2B6EA: case 0xC2B6F2: case 0xC2B7D8: case 0xC2B9A0:
    case 0xC2B9A6: case 0xC2B9AC: case 0xC2B9B2: case 0xC2BAC8:
    case 0xC2BAD8:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC2B46E: case 0xC2B534: case 0xC2B558: case 0xC2B65E:
    case 0xC2B722:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC2B470: case 0xC2B740: case 0xC2B748: case 0xC2B7AC:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC2B474: case 0xC2B49E:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC2B480: case 0xC2B602: case 0xC2B656: case 0xC2B6C8:
    case 0xC2B71A: case 0xC2B74E: case 0xC2B7A0: case 0xC2BA90:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC2B498: case 0xC2B49C: case 0xC2B4AC: case 0xC2B4AE:
    case 0xC2B4B0: case 0xC2B4B2: case 0xC2B4BC: case 0xC2B4BE:
    case 0xC2B4C0: case 0xC2B4C2: case 0xC2B4C4: case 0xC2B4C6:
        step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC2B4D0: case 0xC2B51C: case 0xC2B554: case 0xC2B58A:
    case 0xC2B5EA: case 0xC2B5FE: case 0xC2B612: case 0xC2B64C:
    case 0xC2B6B0: case 0xC2B6C4: case 0xC2B6D8: case 0xC2B710:
    case 0xC2B758:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC2B4D4: case 0xC2B4D6:
        width=4; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC2B4D8: case 0xC2B4DA: case 0xC2B4DC:
        renderer_negate(&D(reg),4); break;
    case 0xC2B4DE: case 0xC2B544: case 0xC2B616: case 0xC2B6DC:
    case 0xC2B6F0: case 0xC2B700: case 0xC2B730: case 0xC2B9EC:
    case 0xC2BA56: case 0xC2BA6E:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC2B528: case 0xC2B54E: case 0xC2B5D2: case 0xC2B5E4:
    case 0xC2B5F6: case 0xC2B646: case 0xC2B698: case 0xC2B6AA:
    case 0xC2B6BC: case 0xC2B70A: case 0xC2B770: case 0xC2B938:
    case 0xC2B96A: case 0xC2B978: case 0xC2BAE0:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC2B574:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC2B592:
        if((opcode&0xf8u)==0x48u) { value=A(destination); A(destination)=A(reg); A(reg)=value; } else if((opcode&0xf8u)==0x40u) { value=D(destination); D(destination)=D(reg); D(reg)=value; } else { value=D(destination); D(destination)=A(reg); A(reg)=value; } break;
    case 0xC2B596: case 0xC2B5A0: case 0xC2B5A2:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC2B622:
        width=2; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC2B626: case 0xC2B662: case 0xC2B6E8: case 0xC2B726:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC2B650: case 0xC2B714: case 0xC2B7D4:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC2B666: case 0xC2B72A: case 0xC2B9AA: case 0xC2B9B6:
    case 0xC2BA08: case 0xC2BA2E: case 0xC2BA66: case 0xC2BACC:
    case 0xC2BADC:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC2B790: case 0xC2B7CC:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC2B7E0:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC2B980:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC2B98A:
        width=4; value=m68ki_read_imm_32(); operation='-'; goto arithmetic;
    case 0xC2B990:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC2B9DC: case 0xC2BA1C: case 0xC2BA42: case 0xC2BA9A:
    case 0xC2BAB6: case 0xC2BAB8: case 0xC2BABA:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC2B9DE:
        width=2; if(opcode&0x100u) { value=D(destination); operation='&'; goto immediate_logic; } value=cache_step_read(mode,reg,2)&D(destination); cache_step_write(0,destination,2,value); cache_step_logic(value,2); break;
    case 0xC2B9F4:
        action_lsr_byte(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC2BA00: case 0xC2BA26:
        width=1; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC2BA0A: case 0xC2BA30:
        width=1; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC2BA16: case 0xC2BA3C:
        width=1; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC2BA1A: case 0xC2BA40:
        if(mode==0) renderer_negate(&D(reg),1); else { address=cache_step_address(mode,reg,1); old=cache_step_read_memory(address,1); renderer_negate(&old,1); cache_step_write_memory(address,old,1,0); } break;
    case 0xC2BA58: case 0xC2BA68:
        width=2; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC2BA80:
        marker_asl_byte(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC2BA82:
        width=1; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC2BAB4:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC2BAE6:
        width=4; goto move;
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
static const uint32_t owned_C2AFFA[]={
    0xC2AFFA,0xC2AFFC,0xC2AFFE,0xC2B000,0xC2B002,0xC2B004,0xC2B00A,0xC2B00C,
    0xC2B00E,0xC2B010,0xC2B012,0xC2B014,0xC2B016,0xC2B018,0xC2B01A,0xC2B01C,
    0xC2B01E,0xC2B020,0xC2B022,0xC2B024,0xC2B026,0xC2B028,0xC2B02A,0xC2B02C,
    0xC2B02E,0xC2B030,0xC2B032,0xC2B034,0xC2B036,0xC2B038,0xC2B03A,0xC2B03C,
    0xC2B03E,0xC2B040,
};
int glue_C2AFFA_owns(uint32_t pc) { return owns_pc(owned_C2AFFA,sizeof owned_C2AFFA/sizeof owned_C2AFFA[0],pc); }
int glue_C2AFFA_step(void) { if(!glue_C2AFFA_owns(REG_PC)) return 0; return flight_markers_step(); }
static const uint32_t owned_C2B3C2[]={
    0xC2B3C0,0xC2B3C2,0xC2B3CA,0xC2B3CC,0xC2B3D4,0xC2B3D6,0xC2B3E0,0xC2B3E2,
    0xC2B3E8,0xC2B3EE,0xC2B3F0,0xC2B3F6,0xC2B3F8,0xC2B3FE,0xC2B404,0xC2B408,
    0xC2B40E,0xC2B416,0xC2B41E,0xC2B420,0xC2B426,0xC2B428,0xC2B42E,0xC2B434,
    0xC2B436,0xC2B43E,0xC2B440,0xC2B446,0xC2B448,0xC2B44E,0xC2B452,0xC2B454,
    0xC2B458,0xC2B45A,0xC2B45E,0xC2B460,0xC2B464,0xC2B466,0xC2B468,0xC2B46C,
    0xC2B46E,0xC2B470,0xC2B474,0xC2B476,0xC2B478,0xC2B47E,0xC2B480,0xC2B486,
    0xC2B48A,0xC2B48E,0xC2B492,0xC2B496,0xC2B498,0xC2B49A,0xC2B49C,0xC2B49E,
    0xC2B4A0,0xC2B4A6,0xC2B4AC,0xC2B4AE,0xC2B4B0,0xC2B4B2,0xC2B4B4,0xC2B4B6,
    0xC2B4B8,0xC2B4BC,0xC2B4BE,0xC2B4C0,0xC2B4C2,0xC2B4C4,0xC2B4C6,0xC2B4C8,
    0xC2B4CA,0xC2B4CC,0xC2B4CE,0xC2B4D0,0xC2B4D4,0xC2B4D6,0xC2B4D8,0xC2B4DA,
    0xC2B4DC,0xC2B4DE,0xC2B4E0,0xC2B4E2,0xC2B4E4,0xC2B4E6,0xC2B4EC,0xC2B4EE,
    0xC2B4F0,0xC2B4F2,0xC2B4F4,0xC2B4F6,0xC2B4F8,0xC2B4FA,0xC2B4FC,0xC2B4FE,
    0xC2B500,0xC2B502,0xC2B504,0xC2B506,0xC2B508,0xC2B50A,0xC2B50C,0xC2B50E,
    0xC2B510,0xC2B512,0xC2B514,0xC2B516,0xC2B518,0xC2B51A,0xC2B51C,0xC2B520,
    0xC2B528,0xC2B52E,0xC2B534,0xC2B536,0xC2B53C,0xC2B53E,0xC2B544,0xC2B546,
    0xC2B54E,0xC2B554,0xC2B558,0xC2B55E,
};
int glue_C2B3C2_owns(uint32_t pc) { return owns_pc(owned_C2B3C2,sizeof owned_C2B3C2/sizeof owned_C2B3C2[0],pc); }
int glue_C2B3C2_step(void) { if(!glue_C2B3C2_owns(REG_PC)) return 0; return flight_markers_step(); }
static const uint32_t owned_C2B564[]={
    0xC2B562,0xC2B564,0xC2B56A,0xC2B56C,0xC2B572,0xC2B574,0xC2B578,0xC2B582,
    0xC2B58A,0xC2B592,0xC2B594,0xC2B596,0xC2B598,0xC2B59C,0xC2B59E,0xC2B5A0,
    0xC2B5A2,0xC2B5A4,0xC2B5A6,0xC2B5A8,0xC2B5AA,0xC2B5AE,0xC2B5B4,0xC2B5BA,
    0xC2B5C0,0xC2B5C6,0xC2B5CA,0xC2B5CE,0xC2B5D2,0xC2B5D8,0xC2B5DC,0xC2B5E0,
    0xC2B5E4,0xC2B5EA,0xC2B5EE,0xC2B5F6,0xC2B5FC,0xC2B5FE,0xC2B602,0xC2B608,
    0xC2B60A,0xC2B60E,0xC2B610,0xC2B612,0xC2B616,0xC2B618,0xC2B61E,0xC2B620,
    0xC2B622,0xC2B626,0xC2B628,0xC2B62C,0xC2B62E,0xC2B632,0xC2B634,0xC2B638,
    0xC2B63A,0xC2B63E,0xC2B646,0xC2B64C,0xC2B650,0xC2B656,0xC2B65C,0xC2B65E,
    0xC2B662,0xC2B666,0xC2B66A,0xC2B670,0xC2B676,0xC2B67C,0xC2B67E,0xC2B680,
    0xC2B684,0xC2B686,0xC2B68C,0xC2B690,0xC2B694,0xC2B698,0xC2B69E,0xC2B6A2,
    0xC2B6A6,0xC2B6AA,0xC2B6B0,0xC2B6B4,0xC2B6BC,0xC2B6C2,0xC2B6C4,0xC2B6C8,
    0xC2B6CE,0xC2B6D0,0xC2B6D4,0xC2B6D6,0xC2B6D8,0xC2B6DC,0xC2B6DE,0xC2B6E4,
    0xC2B6E6,0xC2B6E8,0xC2B6EA,0xC2B6EE,0xC2B6F0,0xC2B6F2,0xC2B6F6,0xC2B6F8,
    0xC2B6FC,0xC2B6FE,0xC2B700,0xC2B702,0xC2B70A,0xC2B710,0xC2B714,0xC2B71A,
    0xC2B720,0xC2B722,0xC2B726,0xC2B72A,0xC2B72E,0xC2B730,0xC2B732,0xC2B738,
    0xC2B73A,0xC2B73E,0xC2B740,0xC2B744,0xC2B748,0xC2B74C,0xC2B74E,0xC2B754,
    0xC2B758,0xC2B75E,0xC2B760,0xC2B762,0xC2B768,0xC2B76A,0xC2B76C,0xC2B770,
    0xC2B776,0xC2B778,0xC2B77A,0xC2B782,0xC2B786,0xC2B78A,0xC2B78E,0xC2B790,
    0xC2B794,0xC2B796,0xC2B79E,0xC2B7A0,0xC2B7A6,0xC2B7A8,0xC2B7AC,0xC2B7B0,
    0xC2B7B2,0xC2B7BA,0xC2B7BC,0xC2B7C4,0xC2B7C6,0xC2B7C8,0xC2B7CA,0xC2B7CC,
    0xC2B7D0,0xC2B7D2,0xC2B7D4,0xC2B7D8,0xC2B7DC,0xC2B7E0,0xC2B7E2,
};
int glue_C2B564_owns(uint32_t pc) { return owns_pc(owned_C2B564,sizeof owned_C2B564/sizeof owned_C2B564[0],pc); }
int glue_C2B564_step(void) { if(!glue_C2B564_owns(REG_PC)) return 0; return flight_markers_step(); }
static const uint32_t owned_C2B928[]={
    0xC2B928,0xC2B92E,0xC2B930,0xC2B938,0xC2B93E,0xC2B940,0xC2B948,0xC2B94C,
    0xC2B950,0xC2BAA8,0xC2BAAA,0xC2BAAE,0xC2BAB0,0xC2BAB2,0xC2BAB4,0xC2BAB6,
    0xC2BAB8,0xC2BABA,0xC2BABC,0xC2BABE,0xC2BAC0,0xC2BAC2,0xC2BAC4,0xC2BAC6,
    0xC2BAC8,0xC2BACC,0xC2BACE,0xC2BAD0,0xC2BAD2,0xC2BAD4,0xC2BAD6,0xC2BAD8,
    0xC2BADC,0xC2BADE,0xC2BAE0,0xC2BAE6,0xC2BAE8,0xC2BAEA,0xC2BAEC,0xC2BAEE,
};
int glue_C2B928_owns(uint32_t pc) { return owns_pc(owned_C2B928,sizeof owned_C2B928/sizeof owned_C2B928[0],pc); }
int glue_C2B928_step(void) { if(!glue_C2B928_owns(REG_PC)) return 0; return flight_markers_step(); }
static const uint32_t owned_C2B952[]={
    0xC2B950,0xC2B952,0xC2B95A,0xC2B95E,0xC2B962,0xC2B964,0xC2B968,0xC2B96A,
    0xC2B970,0xC2B978,0xC2B97E,0xC2B980,0xC2B986,0xC2B988,0xC2B98A,0xC2B990,
    0xC2B996,0xC2B998,0xC2B99E,0xC2B9A0,0xC2B9A4,0xC2B9A6,0xC2B9AA,0xC2B9AC,
    0xC2B9B0,0xC2B9B2,0xC2B9B6,0xC2B9B8,0xC2B9C0,0xC2B9C6,0xC2B9CA,0xC2B9D0,
    0xC2B9D6,0xC2B9DA,0xC2B9DC,0xC2B9DE,0xC2B9E0,0xC2B9E4,0xC2B9E8,0xC2B9EC,
    0xC2B9EE,0xC2B9F2,0xC2B9F4,0xC2B9F6,0xC2B9FA,0xC2B9FC,0xC2BA00,0xC2BA02,
    0xC2BA04,0xC2BA08,0xC2BA0A,0xC2BA0E,0xC2BA10,0xC2BA14,0xC2BA16,0xC2BA1A,
    0xC2BA1C,0xC2BA1E,0xC2BA20,0xC2BA22,0xC2BA26,0xC2BA28,0xC2BA2A,0xC2BA2E,
    0xC2BA30,0xC2BA34,0xC2BA36,0xC2BA3A,0xC2BA3C,0xC2BA40,0xC2BA42,0xC2BA44,
    0xC2BA46,0xC2BA4A,0xC2BA4E,0xC2BA52,0xC2BA56,0xC2BA58,0xC2BA5A,0xC2BA5C,
    0xC2BA60,0xC2BA62,0xC2BA66,0xC2BA68,0xC2BA6A,0xC2BA6C,0xC2BA6E,0xC2BA70,
    0xC2BA74,0xC2BA76,0xC2BA78,0xC2BA7C,0xC2BA80,0xC2BA82,0xC2BA84,0xC2BA88,
    0xC2BA8E,0xC2BA90,0xC2BA98,0xC2BA9A,0xC2BA9C,0xC2BA9E,0xC2BAA2,0xC2BAA6,
    0xC2BAA8,0xC2BAAA,0xC2BAAE,0xC2BAB0,0xC2BAB2,0xC2BAB4,0xC2BAB6,0xC2BAB8,
    0xC2BABA,0xC2BABC,0xC2BABE,0xC2BAC0,0xC2BAC2,0xC2BAC4,0xC2BAC6,0xC2BAC8,
    0xC2BACC,0xC2BACE,0xC2BAD0,0xC2BAD2,0xC2BAD4,0xC2BAD6,0xC2BAD8,0xC2BADC,
    0xC2BADE,0xC2BAE0,0xC2BAE6,0xC2BAE8,0xC2BAEA,0xC2BAEC,0xC2BAEE,
};
int glue_C2B952_owns(uint32_t pc) { return owns_pc(owned_C2B952,sizeof owned_C2B952/sizeof owned_C2B952[0],pc); }
int glue_C2B952_step(void) { if(!glue_C2B952_owns(REG_PC)) return 0; return flight_markers_step(); }
