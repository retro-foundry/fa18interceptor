/* Complete hud parents family source CPU/bus/event boundaries.
 * Readable behavior lives in hud_parents.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family hud_parents. */
#include "glue_hud_parents_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_hud_parents.h"

static int hud_parents_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC30762: case 0xC308D6: case 0xC309E0: case 0xC30C6E:
    case 0xC30D20: case 0xC30D32: case 0xC30EA8: case 0xC30F76:
    case 0xC310A8: case 0xC31128: case 0xC31222: case 0xC31AC8:
    case 0xC31ACA: case 0xC327F4:
        REG_PC=m68ki_pull_32(); break;
    case 0xC30764: case 0xC30B5C: case 0xC30BC4: case 0xC30C20:
    case 0xC30C54: case 0xC30C70: case 0xC30CA4: case 0xC30D5A:
    case 0xC30D7E: case 0xC30F82: case 0xC31A64: case 0xC31ACC:
    case 0xC32772:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC3076A: case 0xC3079A: case 0xC30874: case 0xC30B62:
    case 0xC30BCA: case 0xC30C26: case 0xC30C76: case 0xC30D60:
    case 0xC30D84: case 0xC31A6A: case 0xC31AD2: case 0xC32766:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC3076C: case 0xC30B64: case 0xC30BCC: case 0xC30C28:
    case 0xC30C78: case 0xC30D86: case 0xC31A6C: case 0xC31AD4:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC30772: case 0xC3077E: case 0xC309B6: case 0xC309BA:
    case 0xC30B98: case 0xC30BF8: case 0xC30CE8: case 0xC30D8C:
    case 0xC30D9C: case 0xC30DFA: case 0xC30E44: case 0xC30FC0:
    case 0xC311E6: case 0xC31A72: case 0xC31AA8: case 0xC31AAC:
    case 0xC31AB2: case 0xC31AB6: case 0xC31ABA: case 0xC31B56:
    case 0xC31B5A: case 0xC31B60: case 0xC31B64: case 0xC31B68:
    case 0xC327CC:
        A(destination)=(mode==7 && reg==0)?(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16():cache_step_address(mode,reg,4); break;
    case 0xC30778: case 0xC307CC: case 0xC30880: case 0xC309C2:
    case 0xC30CC4: case 0xC30DCA: case 0xC30FC6: case 0xC3279A:
        width=4; goto move;
    case 0xC30782: case 0xC30790: case 0xC307AC: case 0xC307DA:
    case 0xC307E0: case 0xC307E6: case 0xC307EC: case 0xC307F4:
    case 0xC30812: case 0xC3081C: case 0xC30820: case 0xC30830:
    case 0xC3083E: case 0xC30844: case 0xC3085A: case 0xC3085E:
    case 0xC30876: case 0xC30892: case 0xC308A0: case 0xC308A6:
    case 0xC308AA: case 0xC308AE: case 0xC308B2: case 0xC308B6:
    case 0xC308BE: case 0xC308C6: case 0xC308CE: case 0xC309BE:
    case 0xC309D0: case 0xC309D8: case 0xC30B6A: case 0xC30B7C:
    case 0xC30B86: case 0xC30B8C: case 0xC30BAC: case 0xC30BB6:
    case 0xC30BDC: case 0xC30BE4: case 0xC30BF4: case 0xC30BFE:
    case 0xC30C04: case 0xC30C0E: case 0xC30C14: case 0xC30C18:
    case 0xC30C38: case 0xC30C40: case 0xC30C50: case 0xC30C5C:
    case 0xC30C62: case 0xC30C66: case 0xC30C88: case 0xC30C90:
    case 0xC30CA0: case 0xC30CB6: case 0xC30CBC: case 0xC30CC0:
    case 0xC30CD2: case 0xC30CDA: case 0xC30CF4: case 0xC30CF8:
    case 0xC30CFE: case 0xC30D04: case 0xC30D08: case 0xC30D0C:
    case 0xC30D10: case 0xC30D1C: case 0xC30D3E: case 0xC30D46:
    case 0xC30D56: case 0xC30D6C: case 0xC30D72: case 0xC30D76:
    case 0xC30DBA: case 0xC30DC2: case 0xC30DF6: case 0xC30E06:
    case 0xC30E0A: case 0xC30E10: case 0xC30E16: case 0xC30E1C:
    case 0xC30E20: case 0xC30E28: case 0xC30E2C: case 0xC30E40:
    case 0xC30E58: case 0xC30E5C: case 0xC30E60: case 0xC30E64:
    case 0xC30E9A: case 0xC30F7C: case 0xC30F8A: case 0xC30FAE:
    case 0xC30FB2: case 0xC30FDA: case 0xC30FDE: case 0xC31002:
    case 0xC3102A: case 0xC3102E: case 0xC3103A: case 0xC3103E:
    case 0xC31042: case 0xC31048: case 0xC3104E: case 0xC31052:
    case 0xC3105C: case 0xC31060: case 0xC31070: case 0xC3107E:
    case 0xC31090: case 0xC31094: case 0xC31096: case 0xC3109A:
    case 0xC3113E: case 0xC31144: case 0xC31148: case 0xC3114E:
    case 0xC31152: case 0xC31170: case 0xC31176: case 0xC3117A:
    case 0xC3119A: case 0xC311A0: case 0xC311A4: case 0xC311C6:
    case 0xC311CC: case 0xC311D0: case 0xC311F2: case 0xC31208:
    case 0xC3120C: case 0xC31ABE: case 0xC31ADA: case 0xC31B08:
    case 0xC31B0C: case 0xC31B10: case 0xC31B1A: case 0xC31B1E:
    case 0xC31B22: case 0xC31B2E: case 0xC31B32: case 0xC3273C:
    case 0xC32742: case 0xC3274E: case 0xC32758: case 0xC32776:
    case 0xC32796: case 0xC327A6: case 0xC327A8: case 0xC327AC:
    case 0xC327DA: case 0xC327F6:
        width=2; goto move;
    case 0xC30786: case 0xC307C4: case 0xC307CE: case 0xC3082A:
    case 0xC30886: case 0xC30896: case 0xC309C4: case 0xC309C6:
    case 0xC30B8E: case 0xC30BD2: case 0xC30C2E: case 0xC30C7E:
    case 0xC30CCA: case 0xC30D14: case 0xC30D18: case 0xC30D34:
    case 0xC30DAE: case 0xC30DB4: case 0xC30DEC: case 0xC30DF0:
    case 0xC30E30: case 0xC30E34: case 0xC30E38: case 0xC30E3C:
    case 0xC30E90: case 0xC30FCC: case 0xC30FFA: case 0xC31064:
    case 0xC31068: case 0xC3106C: case 0xC31074: case 0xC3112A:
    case 0xC31A9C: case 0xC31B36: case 0xC31B4A: case 0xC32748:
    case 0xC32752: case 0xC327A0: case 0xC327D6: case 0xC327E6:
    case 0xC327E8: case 0xC327EE:
        width=4; goto move;
    case 0xC3078C: case 0xC307A8: case 0xC309CC: case 0xC309D4:
    case 0xC30BD8: case 0xC30BE0: case 0xC30C34: case 0xC30C3C:
    case 0xC30C84: case 0xC30C8C: case 0xC30D3A: case 0xC30D42:
    case 0xC30DBE: case 0xC30DC6: case 0xC30FD2: case 0xC30FD6:
    case 0xC30FF8:
        width=2; goto move;
    case 0xC30794: case 0xC30808: case 0xC3086E: case 0xC30CCE:
    case 0xC30CD6:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC3079C: case 0xC307C8: case 0xC3087E: case 0xC30890:
    case 0xC30CE2: case 0xC30DE4: case 0xC30FF4:
        width=2; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC307A2: case 0xC3081E: case 0xC3088E:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC307A4: case 0xC3084A: case 0xC3086A: case 0xC30B88:
    case 0xC31010: case 0xC31030: case 0xC3275E:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC307B0: case 0xC307C6: case 0xC307D0: case 0xC30850:
    case 0xC30888: case 0xC3088C: case 0xC30898: case 0xC30BE8:
    case 0xC30C44: case 0xC30C94: case 0xC30CE0: case 0xC30D4A:
    case 0xC30DD0: case 0xC30DEE: case 0xC30DF2: case 0xC30DF4:
    case 0xC30FE2: case 0xC30FF0: case 0xC31028: case 0xC327BE:
    case 0xC327C0:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC307B6: case 0xC307F8: case 0xC307FC: case 0xC30800:
    case 0xC30804: case 0xC308BA: case 0xC308C2: case 0xC308CA:
    case 0xC308D2: case 0xC309DC: case 0xC30BEE: case 0xC30C1C:
    case 0xC30C4A: case 0xC30C6A: case 0xC30C9A: case 0xC30D50:
    case 0xC30D7A: case 0xC30DD6: case 0xC30F78: case 0xC30FE8:
    case 0xC327EA:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC307BA: case 0xC30858: case 0xC30B74: case 0xC30BF2:
    case 0xC30C4E: case 0xC30C9E: case 0xC30D54: case 0xC30DDA:
    case 0xC30E6E: case 0xC30E7C: case 0xC30F90: case 0xC30FEC:
    case 0xC31088: case 0xC3115C: case 0xC327B2:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC307BE: case 0xC307C0: case 0xC307CA: case 0xC307F2:
    case 0xC30822: case 0xC30824: case 0xC30826: case 0xC30848:
    case 0xC30866: case 0xC30B6E: case 0xC30B80: case 0xC30CDE:
    case 0xC30CE4: case 0xC30CE6: case 0xC30DA8: case 0xC30DAA:
    case 0xC30DDE: case 0xC30DE0: case 0xC30DE6: case 0xC30E26:
    case 0xC30E68: case 0xC30E76: case 0xC30E84: case 0xC30E8A:
    case 0xC30FEE: case 0xC30FF6: case 0xC31024: case 0xC3105A:
    case 0xC31082: case 0xC327A4: case 0xC327AE: case 0xC327B0:
    case 0xC327BA: case 0xC327CA:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC307C2: case 0xC30828: case 0xC3084E: case 0xC30DE2:
    case 0xC30DEA: case 0xC3101C: case 0xC31026: case 0xC327BC:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC307D2: case 0xC30868: case 0xC30DE8: case 0xC31098:
    case 0xC3115E: case 0xC31180: case 0xC31188: case 0xC311AC:
    case 0xC311B4: case 0xC311DE: case 0xC31212: case 0xC3121A:
    case 0xC32768:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC307D4: case 0xC3089A: case 0xC30BBE: case 0xC30CEE:
    case 0xC30E00: case 0xC30EA2: case 0xC31034: case 0xC310A2:
    case 0xC31156: case 0xC31160: case 0xC31182: case 0xC3118A:
    case 0xC311AE: case 0xC311B6: case 0xC311D8: case 0xC311E0:
    case 0xC31214: case 0xC3121C: case 0xC31AA2: case 0xC31B40:
    case 0xC31B50: case 0xC327FE:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC307F0: case 0xC30E24: case 0xC311AA: case 0xC311D6:
    case 0xC32778:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC3080E: case 0xC30C0C: case 0xC30C5A: case 0xC30CAA:
    case 0xC30CB4: case 0xC30CD0: case 0xC30CD8: case 0xC30D6A:
    case 0xC30E56: case 0xC30F94: case 0xC31008: case 0xC311FC:
    case 0xC31206: case 0xC31B06:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC30818: case 0xC30860: case 0xC31056: case 0xC327C6:
        width=2; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC30836: case 0xC3083C: case 0xC30B7A: case 0xC30F88:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC30838: case 0xC30854: case 0xC30B76: case 0xC30E70:
    case 0xC30E7E: case 0xC3108A: case 0xC31AE0: case 0xC31AEA:
    case 0xC32762: case 0xC327B4:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC30842: case 0xC30852: case 0xC30BB4: case 0xC30FA0:
    case 0xC30FB0: case 0xC31142: case 0xC31174: case 0xC3119E:
    case 0xC311CA: case 0xC31A8E: case 0xC31A98: case 0xC31AC4:
    case 0xC31AE8: case 0xC31AF2: case 0xC31B18: case 0xC31B6E:
    case 0xC3278A: case 0xC32804:
        step_branch(pc,opcode,1); break;
    case 0xC30846: case 0xC30864: case 0xC3100E:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC3087C:
        menu_lsl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC3088A: case 0xC30C12: case 0xC30C60: case 0xC30CBA:
    case 0xC30D70: case 0xC31A8C: case 0xC31A96: case 0xC31A9A:
    case 0xC31AC2: case 0xC31AE6: case 0xC31AF0: case 0xC31AF4:
    case 0xC31B6C: case 0xC32750:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC30B9E: case 0xC30D92: case 0xC30DAC: case 0xC30E4A:
    case 0xC311EC: case 0xC31A78: case 0xC327D2:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC30BA4: case 0xC30CAC: case 0xC30D62: case 0xC30E50:
    case 0xC30F96: case 0xC31134: case 0xC31166: case 0xC31190:
    case 0xC311BC: case 0xC311F6: case 0xC311FE: case 0xC31AFE:
    case 0xC327E0:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC30BAA: case 0xC30F9E: case 0xC3113C: case 0xC3116E:
    case 0xC31198: case 0xC311C4: case 0xC31A8A: case 0xC31A94:
    case 0xC31AE4: case 0xC31AEE: case 0xC32774: case 0xC3277E:
    case 0xC327E4:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC30C08: case 0xC30FA2: case 0xC30FAA: case 0xC31004:
    case 0xC31016: case 0xC3275A: case 0xC327C2:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC30D98: case 0xC31A7E: case 0xC31AF6: case 0xC3276A:
    case 0xC32780: case 0xC327AA:
        width=1; goto move;
    case 0xC30DA0:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='&'; goto bit_value;
    case 0xC30DA4:
        hud_parent_asr_byte(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC30DA6:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC30E74: case 0xC30E82: case 0xC3108E: case 0xC327B8:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC30F92:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC30FB8:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC31000: case 0xC3101A:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC3100A:
        width=4; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC3100C: case 0xC3104C: case 0xC32740: case 0xC327D8:
    case 0xC327DC:
        step_swap(&D(reg)); break;
    case 0xC31014:
        hud_parent_ror_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC3101E:
        width=4; value=m68ki_read_imm_32(); operation='+'; goto arithmetic;
    case 0xC31A82:
        width=1; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC31A86: case 0xC31A90: case 0xC3277A:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC31B2A: case 0xC31B46:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,2,(int)reg); else { address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); } break;
    case 0xC3276C:
        step_lsr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC3276E: case 0xC32786: case 0xC327F0:
        step_dbf(pc,&D(reg)); break;
    case 0xC32794:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC327DE:
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
static const uint32_t owned_C30764[]={
    0xC30762,0xC30764,0xC3076A,0xC3076C,0xC30772,0xC30778,0xC3077E,0xC30782,
    0xC30786,0xC3078C,0xC30790,0xC30794,0xC3079A,0xC3079C,0xC307A2,0xC307A4,
    0xC307A8,0xC307AC,0xC307B0,0xC307B6,0xC307BA,0xC307BE,0xC307C0,0xC307C2,
    0xC307C4,0xC307C6,0xC307C8,0xC307CA,0xC307CC,0xC307CE,0xC307D0,0xC307D2,
    0xC307D4,0xC307DA,0xC307E0,0xC307E6,0xC307EC,0xC307F0,0xC307F2,0xC307F4,
    0xC307F8,0xC307FC,0xC30800,0xC30804,0xC30808,0xC3080E,0xC30812,0xC30818,
    0xC3081C,0xC3081E,0xC30820,0xC30822,0xC30824,0xC30826,0xC30828,0xC3082A,
    0xC30830,0xC30836,0xC30838,0xC3083C,0xC3083E,0xC30842,0xC30844,0xC30846,
    0xC30848,0xC3084A,0xC3084E,0xC30850,0xC30852,0xC30854,0xC30858,0xC3085A,
    0xC3085E,0xC30860,0xC30864,0xC30866,0xC30868,0xC3086A,0xC3086E,0xC30874,
    0xC30876,0xC3087C,0xC3087E,0xC30880,0xC30886,0xC30888,0xC3088A,0xC3088C,
    0xC3088E,0xC30890,0xC30892,0xC30896,0xC30898,0xC3089A,0xC308A0,0xC308A6,
    0xC308AA,0xC308AE,0xC308B2,0xC308B6,0xC308BA,0xC308BE,0xC308C2,0xC308C6,
    0xC308CA,0xC308CE,0xC308D2,0xC308D6,
};
int glue_C30764_owns(uint32_t pc) { return owns_pc(owned_C30764,sizeof owned_C30764/sizeof owned_C30764[0],pc); }
int glue_C30764_complete_step(void) { if(!glue_C30764_owns(REG_PC)) return 0; return hud_parents_step(); }
static const uint32_t owned_C309B6[]={
    0xC309B6,0xC309BA,0xC309BE,0xC309C2,0xC309C4,0xC309C6,0xC309CC,0xC309D0,
    0xC309D4,0xC309D8,0xC309DC,0xC309E0,
};
int glue_C309B6_owns(uint32_t pc) { return owns_pc(owned_C309B6,sizeof owned_C309B6/sizeof owned_C309B6[0],pc); }
int glue_C309B6_complete_step(void) { if(!glue_C309B6_owns(REG_PC)) return 0; return hud_parents_step(); }
static const uint32_t owned_C30B5C[]={
    0xC30B5C,0xC30B62,0xC30B64,0xC30B6A,0xC30B6E,0xC30B74,0xC30B76,0xC30B7A,
    0xC30B7C,0xC30B80,0xC30B86,0xC30B88,0xC30B8C,0xC30B8E,0xC30B98,0xC30B9E,
    0xC30BA4,0xC30BAA,0xC30BAC,0xC30BB4,0xC30BB6,0xC30BBE,0xC30BC4,0xC30BCA,
    0xC30BCC,0xC30BD2,0xC30BD8,0xC30BDC,0xC30BE0,0xC30BE4,0xC30BE8,0xC30BEE,
    0xC30BF2,0xC30BF4,0xC30BF8,0xC30BFE,0xC30C04,0xC30C08,0xC30C0C,0xC30C0E,
    0xC30C12,0xC30C14,0xC30C18,0xC30C1C,0xC30C20,0xC30C26,0xC30C28,0xC30C2E,
    0xC30C34,0xC30C38,0xC30C3C,0xC30C40,0xC30C44,0xC30C4A,0xC30C4E,0xC30C50,
    0xC30C54,0xC30C5A,0xC30C5C,0xC30C60,0xC30C62,0xC30C66,0xC30C6A,0xC30C6E,
    0xC30C70,0xC30C76,0xC30C78,0xC30C7E,0xC30C84,0xC30C88,0xC30C8C,0xC30C90,
    0xC30C94,0xC30C9A,0xC30C9E,0xC30CA0,0xC30CA4,0xC30CAA,0xC30CAC,0xC30CB4,
    0xC30CB6,0xC30CBA,0xC30CBC,0xC30CC0,0xC30CC4,0xC30CCA,0xC30CCE,0xC30CD0,
    0xC30CD2,0xC30CD6,0xC30CD8,0xC30CDA,0xC30CDE,0xC30CE0,0xC30CE2,0xC30CE4,
    0xC30CE6,0xC30CE8,0xC30CEE,0xC30CF4,0xC30CF8,0xC30CFE,0xC30D04,0xC30D08,
    0xC30D0C,0xC30D10,0xC30D14,0xC30D18,0xC30D1C,0xC30D20,
};
int glue_C30B5C_owns(uint32_t pc) { return owns_pc(owned_C30B5C,sizeof owned_C30B5C/sizeof owned_C30B5C[0],pc); }
int glue_C30B5C_complete_step(void) { if(!glue_C30B5C_owns(REG_PC)) return 0; return hud_parents_step(); }
static const uint32_t owned_C30D34[]={
    0xC30D32,0xC30D34,0xC30D3A,0xC30D3E,0xC30D42,0xC30D46,0xC30D4A,0xC30D50,
    0xC30D54,0xC30D56,0xC30D5A,0xC30D60,0xC30D62,0xC30D6A,0xC30D6C,0xC30D70,
    0xC30D72,0xC30D76,0xC30D7A,0xC30D7E,0xC30D84,0xC30D86,0xC30D8C,0xC30D92,
    0xC30D98,0xC30D9C,0xC30DA0,0xC30DA4,0xC30DA6,0xC30DA8,0xC30DAA,0xC30DAC,
    0xC30DAE,0xC30DB4,0xC30DBA,0xC30DBE,0xC30DC2,0xC30DC6,0xC30DCA,0xC30DD0,
    0xC30DD6,0xC30DDA,0xC30DDE,0xC30DE0,0xC30DE2,0xC30DE4,0xC30DE6,0xC30DE8,
    0xC30DEA,0xC30DEC,0xC30DEE,0xC30DF0,0xC30DF2,0xC30DF4,0xC30DF6,0xC30DFA,
    0xC30E00,0xC30E06,0xC30E0A,0xC30E10,0xC30E16,0xC30E1C,0xC30E20,0xC30E24,
    0xC30E26,0xC30E28,0xC30E2C,0xC30E30,0xC30E34,0xC30E38,0xC30E3C,0xC30E40,
    0xC30E44,0xC30E4A,0xC30E50,0xC30E56,0xC30E58,0xC30E5C,0xC30E60,0xC30E64,
    0xC30E68,0xC30E6E,0xC30E70,0xC30E74,0xC30E76,0xC30E7C,0xC30E7E,0xC30E82,
    0xC30E84,0xC30E8A,0xC30E90,0xC30E9A,0xC30EA2,0xC30EA8,
};
int glue_C30D34_owns(uint32_t pc) { return owns_pc(owned_C30D34,sizeof owned_C30D34/sizeof owned_C30D34[0],pc); }
int glue_C30D34_complete_step(void) { if(!glue_C30D34_owns(REG_PC)) return 0; return hud_parents_step(); }
static const uint32_t owned_C30F78[]={
    0xC30F76,0xC30F78,0xC30F7C,0xC30F82,0xC30F88,0xC30F8A,0xC30F90,0xC30F92,
    0xC30F94,0xC30F96,0xC30F9E,0xC30FA0,0xC30FA2,0xC30FAA,0xC30FAE,0xC30FB0,
    0xC30FB2,0xC30FB8,0xC30FC0,0xC30FC6,0xC30FCC,0xC30FD2,0xC30FD6,0xC30FDA,
    0xC30FDE,0xC30FE2,0xC30FE8,0xC30FEC,0xC30FEE,0xC30FF0,0xC30FF4,0xC30FF6,
    0xC30FF8,0xC30FFA,0xC31000,0xC31002,0xC31004,0xC31008,0xC3100A,0xC3100C,
    0xC3100E,0xC31010,0xC31014,0xC31016,0xC3101A,0xC3101C,0xC3101E,0xC31024,
    0xC31026,0xC31028,0xC3102A,0xC3102E,0xC31030,0xC31034,0xC3103A,0xC3103E,
    0xC31042,0xC31048,0xC3104C,0xC3104E,0xC31052,0xC31056,0xC3105A,0xC3105C,
    0xC31060,0xC31064,0xC31068,0xC3106C,0xC31070,0xC31074,0xC3107E,0xC31082,
    0xC31088,0xC3108A,0xC3108E,0xC31090,0xC31094,0xC31096,0xC31098,0xC3109A,
    0xC310A2,0xC310A8,
};
int glue_C30F78_owns(uint32_t pc) { return owns_pc(owned_C30F78,sizeof owned_C30F78/sizeof owned_C30F78[0],pc); }
int glue_C30F78_complete_step(void) { if(!glue_C30F78_owns(REG_PC)) return 0; return hud_parents_step(); }
static const uint32_t owned_C3112A[]={
    0xC31128,0xC3112A,0xC31134,0xC3113C,0xC3113E,0xC31142,0xC31144,0xC31148,
    0xC3114E,0xC31152,0xC31156,0xC3115C,0xC3115E,0xC31160,0xC31166,0xC3116E,
    0xC31170,0xC31174,0xC31176,0xC3117A,0xC31180,0xC31182,0xC31188,0xC3118A,
    0xC31190,0xC31198,0xC3119A,0xC3119E,0xC311A0,0xC311A4,0xC311AA,0xC311AC,
    0xC311AE,0xC311B4,0xC311B6,0xC311BC,0xC311C4,0xC311C6,0xC311CA,0xC311CC,
    0xC311D0,0xC311D6,0xC311D8,0xC311DE,0xC311E0,0xC311E6,0xC311EC,0xC311F2,
    0xC311F6,0xC311FC,0xC311FE,0xC31206,0xC31208,0xC3120C,0xC31212,0xC31214,
    0xC3121A,0xC3121C,0xC31222,
};
int glue_C3112A_owns(uint32_t pc) { return owns_pc(owned_C3112A,sizeof owned_C3112A/sizeof owned_C3112A[0],pc); }
int glue_C3112A_complete_step(void) { if(!glue_C3112A_owns(REG_PC)) return 0; return hud_parents_step(); }
static const uint32_t owned_C31A64[]={
    0xC31A64,0xC31A6A,0xC31A6C,0xC31A72,0xC31A78,0xC31A7E,0xC31A82,0xC31A86,
    0xC31A8A,0xC31A8C,0xC31A8E,0xC31A90,0xC31A94,0xC31A96,0xC31A98,0xC31A9A,
    0xC31A9C,0xC31AA2,0xC31AA8,0xC31AAC,0xC31AB2,0xC31AB6,0xC31ABA,0xC31ABE,
    0xC31AC2,0xC31AC4,0xC31AC8,0xC3273C,0xC32740,0xC32742,0xC32748,0xC3274E,
    0xC32750,0xC32752,0xC32758,0xC3275A,0xC3275E,0xC32762,0xC32766,0xC32768,
    0xC3276A,0xC3276C,0xC3276E,0xC32772,0xC32774,0xC32776,0xC32778,0xC3277A,
    0xC3277E,0xC32780,0xC32786,0xC3278A,0xC32794,0xC32796,0xC3279A,0xC327A0,
    0xC327A4,0xC327A6,0xC327A8,0xC327AA,0xC327AC,0xC327AE,0xC327B0,0xC327B2,
    0xC327B4,0xC327B8,0xC327BA,0xC327BC,0xC327BE,0xC327C0,0xC327C2,0xC327C6,
    0xC327CA,0xC327CC,0xC327D2,0xC327D6,0xC327D8,0xC327DA,0xC327DC,0xC327DE,
    0xC327E0,0xC327E4,0xC327E6,0xC327E8,0xC327EA,0xC327EE,0xC327F0,0xC327F4,
    0xC327F6,0xC327FE,0xC32804,
};
int glue_C31A64_owns(uint32_t pc) { return owns_pc(owned_C31A64,sizeof owned_C31A64/sizeof owned_C31A64[0],pc); }
int glue_C31A64_complete_step(void) { if(!glue_C31A64_owns(REG_PC)) return 0; return hud_parents_step(); }
static const uint32_t owned_C31ACC[]={
    0xC31ACA,0xC31ACC,0xC31AD2,0xC31AD4,0xC31ADA,0xC31AE0,0xC31AE4,0xC31AE6,
    0xC31AE8,0xC31AEA,0xC31AEE,0xC31AF0,0xC31AF2,0xC31AF4,0xC31AF6,0xC31AFE,
    0xC31B06,0xC31B08,0xC31B0C,0xC31B10,0xC31B18,0xC31B1A,0xC31B1E,0xC31B22,
    0xC31B2A,0xC31B2E,0xC31B32,0xC31B36,0xC31B40,0xC31B46,0xC31B4A,0xC31B50,
    0xC31B56,0xC31B5A,0xC31B60,0xC31B64,0xC31B68,0xC31B6C,0xC31B6E,0xC32740,
    0xC32742,0xC32748,0xC3274E,0xC32750,0xC32752,0xC32758,0xC3275A,0xC3275E,
    0xC32762,0xC32766,0xC32768,0xC3276A,0xC3276C,0xC3276E,0xC32772,0xC32774,
    0xC32776,0xC32778,0xC3277A,0xC3277E,0xC32780,0xC32786,0xC3278A,0xC32794,
    0xC32796,0xC3279A,0xC327A0,0xC327A4,0xC327A6,0xC327A8,0xC327AA,0xC327AC,
    0xC327AE,0xC327B0,0xC327B2,0xC327B4,0xC327B8,0xC327BA,0xC327BC,0xC327BE,
    0xC327C0,0xC327C2,0xC327C6,0xC327CA,0xC327CC,0xC327D2,0xC327D6,0xC327D8,
    0xC327DA,0xC327DC,0xC327DE,0xC327E0,0xC327E4,0xC327E6,0xC327E8,0xC327EA,
    0xC327EE,0xC327F0,0xC327F4,0xC327F6,0xC327FE,0xC32804,
};
int glue_C31ACC_owns(uint32_t pc) { return owns_pc(owned_C31ACC,sizeof owned_C31ACC/sizeof owned_C31ACC[0],pc); }
int glue_C31ACC_complete_step(void) { if(!glue_C31ACC_owns(REG_PC)) return 0; return hud_parents_step(); }
