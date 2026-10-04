/* Complete corner view family source CPU/bus/event boundaries.
 * Readable behavior lives in corner_view.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family corner_view. */
#include "glue_render_leaf_helpers_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_corner_view.h"

static int corner_view_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC200F6: case 0xC2CD6C: case 0xC2D0B2: case 0xC2D0BC:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC200FA: case 0xC2CCB2: case 0xC2CCBA: case 0xC2CCCA:
    case 0xC2CD06: case 0xC2CD08: case 0xC2CD0A: case 0xC2CD14:
    case 0xC2CD3A: case 0xC2CD4C: case 0xC2CD5C: case 0xC2CDA6:
    case 0xC2CDB6: case 0xC2CDF4: case 0xC2CDFA: case 0xC2CE02:
    case 0xC2CE10: case 0xC2CE3C: case 0xC2CE3E: case 0xC2CE40:
    case 0xC2CE98: case 0xC2CEB6: case 0xC2D082: case 0xC2D08A:
    case 0xC2D0BA: case 0xC2D0C0: case 0xC2D0DC: case 0xC2D0E2:
    case 0xC2D0EC: case 0xC2D0F2: case 0xC2D110: case 0xC2D164:
    case 0xC2D3EE: case 0xC2D3F0: case 0xC2D3F2: case 0xC2E76E:
    case 0xC2E77A: case 0xC2E786: case 0xC2E78C: case 0xC2E78E:
    case 0xC2E79C: case 0xC2E7C2: case 0xC2E7C6: case 0xC2E7F2:
    case 0xC2E7FC: case 0xC2E806: case 0xC2E80A: case 0xC2E838:
    case 0xC2E842: case 0xC2E846: case 0xC2E87E: case 0xC2E88E:
    case 0xC2E892: case 0xC2E8AC: case 0xC2E8B6: case 0xC2E8D0:
    case 0xC2E8DE: case 0xC2E8E2: case 0xC2E90C: case 0xC2E916:
    case 0xC2E920: case 0xC2E924: case 0xC2E950: case 0xC2E95A:
    case 0xC2E95E: case 0xC2E98C: case 0xC2E998: case 0xC2E99C:
    case 0xC2E9B2: case 0xC2E9BA: case 0xC2E9D0: case 0xC2E9DE:
    case 0xC2EA4E: case 0xC2EA54:
        width=2; goto move;
    case 0xC200FE: case 0xC203CE: case 0xC20590: case 0xC20828:
    case 0xC22C70: case 0xC2CD26: case 0xC2CE5C: case 0xC2CEBC:
    case 0xC2D080: case 0xC2D16A: case 0xC2D3FA: case 0xC2EA44:
        REG_PC=m68ki_pull_32(); break;
    case 0xC203CC: case 0xC2058E: case 0xC20826: case 0xC2CD7C:
    case 0xC2CD88: case 0xC2CD8A: case 0xC2CD8C: case 0xC2D130:
    case 0xC2D14E: case 0xC2D3AC: case 0xC2D3D2: case 0xC2D3E6:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC2CCA0: case 0xC2CD28: case 0xC2CD94: case 0xC2D106:
    case 0xC2E758:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC2CCA4: case 0xC2CCDA: case 0xC2CCE8: case 0xC2CCF6:
    case 0xC2CD2C: case 0xC2CD98: case 0xC2CDC6: case 0xC2CDD4:
    case 0xC2CDE2: case 0xC2CE4A: case 0xC2D098: case 0xC2D3C0:
    case 0xC2D3D4:
        width=4; goto move;
    case 0xC2CCA8: case 0xC2CCC4: case 0xC2CCC6: case 0xC2CCD4:
    case 0xC2CCD6: case 0xC2CD30: case 0xC2CD56: case 0xC2CD58:
    case 0xC2CD66: case 0xC2CD68: case 0xC2CD9C: case 0xC2CDB0:
    case 0xC2CDB2: case 0xC2CDC0: case 0xC2CDC2: case 0xC2E79A:
    case 0xC2E79E: case 0xC2E7B2: case 0xC2E9E0:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC2CCAA: case 0xC2CD32: case 0xC2CD9E: case 0xC2CE18:
    case 0xC2CE26: case 0xC2CE82: case 0xC2E760: case 0xC2E766:
    case 0xC2E7A0:
        A(destination)=(mode==7 && reg==0)?(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16():cache_step_address(mode,reg,4); break;
    case 0xC2CCB0: case 0xC2CD38: case 0xC2CDA4:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC2CCBE: case 0xC2CCCE: case 0xC2CD50: case 0xC2CD60:
    case 0xC2CD7E: case 0xC2CDAA: case 0xC2CDBA:
        width=2; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC2CCC8: case 0xC2CCD8: case 0xC2CD5A: case 0xC2CD6A:
    case 0xC2CDB4: case 0xC2CDC4: case 0xC2D0CE: case 0xC2D0F8:
    case 0xC2D0FA:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC2CCDE: case 0xC2CCEC: case 0xC2CD00: case 0xC2CD72:
    case 0xC2CD74: case 0xC2CD76: case 0xC2CDCA: case 0xC2CDD8:
    case 0xC2CDEC: case 0xC2CE96: case 0xC2CEA8: case 0xC2CEB4:
    case 0xC2D100: case 0xC2D102: case 0xC2D104: case 0xC2D3E8:
    case 0xC2D3EA: case 0xC2D3EC:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC2CCE0: case 0xC2CCE2: case 0xC2CCEE: case 0xC2CCF0:
    case 0xC2CDCC: case 0xC2CDCE: case 0xC2CDDA: case 0xC2CDDC:
    case 0xC2CE30: case 0xC2CE32: case 0xC2CE34:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC2CCFA: case 0xC2CD78: case 0xC2CD7A: case 0xC2CDE6:
    case 0xC2CE92: case 0xC2CE94: case 0xC2CEA4: case 0xC2CEA6:
    case 0xC2CEB0: case 0xC2CEB2: case 0xC2D09A: case 0xC2D0E6:
    case 0xC2D0FC: case 0xC2D0FE: case 0xC2D3AE: case 0xC2D3B4:
    case 0xC2D3BA:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC2CD02: case 0xC2CE38: case 0xC2D0B6: case 0xC2D118:
    case 0xC2D136: case 0xC2D15C: case 0xC2E7D0: case 0xC2E7E6:
    case 0xC2E816: case 0xC2E82C: case 0xC2E852: case 0xC2E870:
    case 0xC2E89C: case 0xC2E8C2: case 0xC2E8EC: case 0xC2E900:
    case 0xC2E930: case 0xC2E944: case 0xC2E968: case 0xC2E980:
    case 0xC2E9A4: case 0xC2E9C4:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC2CD0C: case 0xC2CE4C: case 0xC2D3F4:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC2CD12: case 0xC2CD1C: case 0xC2E778: case 0xC2E7A8:
    case 0xC2E7D4: case 0xC2E81A: case 0xC2E830: case 0xC2E856:
    case 0xC2E8A0: case 0xC2E8F0: case 0xC2E934: case 0xC2E948:
    case 0xC2E96C: case 0xC2E9A8: case 0xC2E9EE:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC2CD18: case 0xC2CD1E:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC2CD24: case 0xC2CD8E: case 0xC2CE5A: case 0xC2D168:
    case 0xC2EA42:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC2CD42: case 0xC2CD82: case 0xC2D0D6: case 0xC2EA16:
    case 0xC2EA2E:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC2CD48: case 0xC2CE58: case 0xC2D0AA: case 0xC2D3C8:
    case 0xC2D3CC: case 0xC2D3DC: case 0xC2D3E0: case 0xC2D3E4:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC2CD86: case 0xC2D0D2: case 0xC2D0DA: case 0xC2D3D0:
    case 0xC2E782: case 0xC2E796: case 0xC2E7CC: case 0xC2E7E4:
    case 0xC2E812: case 0xC2E82A: case 0xC2E84E: case 0xC2E86C:
    case 0xC2E898: case 0xC2E8BE: case 0xC2E8E8: case 0xC2E8FE:
    case 0xC2E92C: case 0xC2E942: case 0xC2E966: case 0xC2E97E:
    case 0xC2E9A2: case 0xC2E9C2: case 0xC2EA02: case 0xC2EA3E:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC2CD90: case 0xC2E78A: case 0xC2E7AE: case 0xC2E7F8:
    case 0xC2E884: case 0xC2E8D6: case 0xC2E912: case 0xC2E992:
    case 0xC2E9D6: case 0xC2E9E6: case 0xC2E9F6: case 0xC2EA48:
    case 0xC2EA4C: case 0xC2EA52: case 0xC2EA58:
        step_branch(pc,opcode,1); break;
    case 0xC2CDEE: case 0xC2CE2C: case 0xC2CE88: case 0xC2CE9A:
    case 0xC2D114: case 0xC2D120: case 0xC2D132: case 0xC2D13E:
    case 0xC2D3A4: case 0xC2E7B4: case 0xC2E9F8: case 0xC2EA34:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,2,(int)reg); else { address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); } break;
    case 0xC2CE0A: case 0xC2D092: case 0xC2E9DC:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC2CE1E: case 0xC2CE22:
        width=2; goto move;
    case 0xC2CE42:
        step_compare_long(cache_step_read(mode,reg,4),A(destination)); break;
    case 0xC2CE48: case 0xC2D128: case 0xC2D146: case 0xC2E7C0:
    case 0xC2E802: case 0xC2E83E: case 0xC2E864: case 0xC2E88A:
    case 0xC2E8B2: case 0xC2E8DC: case 0xC2E91C: case 0xC2E956:
    case 0xC2E978: case 0xC2E996: case 0xC2E9B8: case 0xC2EA14:
    case 0xC2EA2C:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC2CE52:
        width=4; goto move;
    case 0xC2CE54: case 0xC2D12C: case 0xC2D14A: case 0xC2E788:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC2CE8C: case 0xC2CE8E: case 0xC2CE90: case 0xC2CE9E:
    case 0xC2CEA0: case 0xC2CEA2: case 0xC2CEAA: case 0xC2CEAC:
    case 0xC2CEAE: case 0xC2EA04: case 0xC2EA1C:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC2D0A0: case 0xC2D12E: case 0xC2D14C: case 0xC2D3B0:
    case 0xC2D3B6: case 0xC2D3BC: case 0xC2E7DE: case 0xC2E824:
    case 0xC2E836: case 0xC2E85E: case 0xC2E8A8: case 0xC2E8F8:
    case 0xC2E93C: case 0xC2E94E: case 0xC2E974: case 0xC2E9B0:
    case 0xC2E9DA: case 0xC2EA1A: case 0xC2EA32:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC2D0A2: case 0xC2D3B2: case 0xC2D3B8: case 0xC2D3BE:
        renderer_negate(&D(reg),4); break;
    case 0xC2D0A4:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC2D0AC: case 0xC2D124: case 0xC2D142:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC2D0B0: case 0xC2E7EA: case 0xC2E874: case 0xC2E8C6:
    case 0xC2E904: case 0xC2E984: case 0xC2E9C8:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC2D0C4:
        timer_multiply_unsigned(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC2D0C6: case 0xC2D0D4:
        step_divide_unsigned(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC2D0CA: case 0xC2D150: case 0xC2D154: case 0xC2D158:
    case 0xC2EA10: case 0xC2EA28:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC2D0D0: case 0xC2E7D8: case 0xC2E81E: case 0xC2E834:
    case 0xC2E858: case 0xC2E8A2: case 0xC2E8F2: case 0xC2E936:
    case 0xC2E94C: case 0xC2E96E: case 0xC2E9AA: case 0xC2E9D8:
    case 0xC2EA00:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC2D0E0: case 0xC2D0E4: case 0xC2D10E: case 0xC2D112:
        step_swap(&D(reg)); break;
    case 0xC2D10A: case 0xC2E76C: case 0xC2E784: case 0xC2E798:
    case 0xC2E9E2: case 0xC2EA46: case 0xC2EA4A:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC2D11C: case 0xC2D13A: case 0xC2D160:
        width=2; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC2D12A: case 0xC2D148: case 0xC2E77C: case 0xC2E790:
    case 0xC2E7BA: case 0xC2E7BC: case 0xC2E7EC: case 0xC2E878:
    case 0xC2E8CA: case 0xC2E906: case 0xC2E986: case 0xC2E9CA:
    case 0xC2EA0E: case 0xC2EA26: case 0xC2EA38:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC2D3C6: case 0xC2D3CA: case 0xC2D3CE: case 0xC2D3DA:
    case 0xC2D3DE: case 0xC2D3E2:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC2E75C: case 0xC2E7AA:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC2E774: case 0xC2E9EA:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC2E77E: case 0xC2E792: case 0xC2E7BE: case 0xC2E7CA:
    case 0xC2E7E2: case 0xC2E800: case 0xC2E810: case 0xC2E828:
    case 0xC2E83C: case 0xC2E84C: case 0xC2E862: case 0xC2E86A:
    case 0xC2E888: case 0xC2E896: case 0xC2E8B0: case 0xC2E8BC:
    case 0xC2E8DA: case 0xC2E8E6: case 0xC2E8FC: case 0xC2E91A:
    case 0xC2E92A: case 0xC2E940: case 0xC2E954: case 0xC2E964:
    case 0xC2E976: case 0xC2E97C: case 0xC2E994: case 0xC2E9A0:
    case 0xC2E9B6: case 0xC2E9C0: case 0xC2EA3A:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC2E7A4:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC2E7E0: case 0xC2E7FE: case 0xC2E80E: case 0xC2E826:
    case 0xC2E83A: case 0xC2E84A: case 0xC2E868: case 0xC2E8AE:
    case 0xC2E8BA: case 0xC2E8FA: case 0xC2E918: case 0xC2E928:
    case 0xC2E93E: case 0xC2E952: case 0xC2E962: case 0xC2E97A:
    case 0xC2E9B4: case 0xC2E9BE:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC2E9F0:
        width=1; goto move;
    case 0xC2EA08: case 0xC2EA20:
        renderer_divide(&D(destination),(int16_t)cache_step_read(mode,reg,2)); break;
    case 0xC2EA0A: case 0xC2EA22:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC2EA0C: case 0xC2EA24:
        step_branch(pc,opcode,COND_CC()); break;
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
static const uint32_t owned_C2E758[]={
    0xC2E758,0xC2E75C,0xC2E760,0xC2E766,0xC2E76C,0xC2E76E,0xC2E774,0xC2E778,
    0xC2E77A,0xC2E77C,0xC2E77E,0xC2E782,0xC2E784,0xC2E786,0xC2E788,0xC2E78A,
    0xC2E78C,0xC2E78E,0xC2E790,0xC2E792,0xC2E796,0xC2E798,0xC2E79A,0xC2E79C,
    0xC2E79E,0xC2E7A0,0xC2E7A4,0xC2E7A8,0xC2E7AA,0xC2E7AE,0xC2E7B2,0xC2E7B4,
    0xC2E7BA,0xC2E7BC,0xC2E7BE,0xC2E7C0,0xC2E7C2,0xC2E7C6,0xC2E7CA,0xC2E7CC,
    0xC2E7D0,0xC2E7D4,0xC2E7D8,0xC2E7DE,0xC2E7E0,0xC2E7E2,0xC2E7E4,0xC2E7E6,
    0xC2E7EA,0xC2E7EC,0xC2E7F2,0xC2E7F8,0xC2E7FC,0xC2E7FE,0xC2E800,0xC2E802,
    0xC2E806,0xC2E80A,0xC2E80E,0xC2E810,0xC2E812,0xC2E816,0xC2E81A,0xC2E81E,
    0xC2E824,0xC2E826,0xC2E828,0xC2E82A,0xC2E82C,0xC2E830,0xC2E834,0xC2E836,
    0xC2E838,0xC2E83A,0xC2E83C,0xC2E83E,0xC2E842,0xC2E846,0xC2E84A,0xC2E84C,
    0xC2E84E,0xC2E852,0xC2E856,0xC2E858,0xC2E85E,0xC2E862,0xC2E864,0xC2E868,
    0xC2E86A,0xC2E86C,0xC2E870,0xC2E874,0xC2E878,0xC2E87E,0xC2E884,0xC2E888,
    0xC2E88A,0xC2E88E,0xC2E892,0xC2E896,0xC2E898,0xC2E89C,0xC2E8A0,0xC2E8A2,
    0xC2E8A8,0xC2E8AC,0xC2E8AE,0xC2E8B0,0xC2E8B2,0xC2E8B6,0xC2E8BA,0xC2E8BC,
    0xC2E8BE,0xC2E8C2,0xC2E8C6,0xC2E8CA,0xC2E8D0,0xC2E8D6,0xC2E8DA,0xC2E8DC,
    0xC2E8DE,0xC2E8E2,0xC2E8E6,0xC2E8E8,0xC2E8EC,0xC2E8F0,0xC2E8F2,0xC2E8F8,
    0xC2E8FA,0xC2E8FC,0xC2E8FE,0xC2E900,0xC2E904,0xC2E906,0xC2E90C,0xC2E912,
    0xC2E916,0xC2E918,0xC2E91A,0xC2E91C,0xC2E920,0xC2E924,0xC2E928,0xC2E92A,
    0xC2E92C,0xC2E930,0xC2E934,0xC2E936,0xC2E93C,0xC2E93E,0xC2E940,0xC2E942,
    0xC2E944,0xC2E948,0xC2E94C,0xC2E94E,0xC2E950,0xC2E952,0xC2E954,0xC2E956,
    0xC2E95A,0xC2E95E,0xC2E962,0xC2E964,0xC2E966,0xC2E968,0xC2E96C,0xC2E96E,
    0xC2E974,0xC2E976,0xC2E978,0xC2E97A,0xC2E97C,0xC2E97E,0xC2E980,0xC2E984,
    0xC2E986,0xC2E98C,0xC2E992,0xC2E994,0xC2E996,0xC2E998,0xC2E99C,0xC2E9A0,
    0xC2E9A2,0xC2E9A4,0xC2E9A8,0xC2E9AA,0xC2E9B0,0xC2E9B2,0xC2E9B4,0xC2E9B6,
    0xC2E9B8,0xC2E9BA,0xC2E9BE,0xC2E9C0,0xC2E9C2,0xC2E9C4,0xC2E9C8,0xC2E9CA,
    0xC2E9D0,0xC2E9D6,0xC2E9D8,0xC2E9DA,0xC2E9DC,0xC2E9DE,0xC2E9E0,0xC2E9E2,
    0xC2E9E6,0xC2E9EA,0xC2E9EE,0xC2E9F0,0xC2E9F6,0xC2E9F8,0xC2EA00,0xC2EA02,
    0xC2EA04,0xC2EA08,0xC2EA0A,0xC2EA0C,0xC2EA0E,0xC2EA10,0xC2EA14,0xC2EA16,
    0xC2EA1A,0xC2EA1C,0xC2EA20,0xC2EA22,0xC2EA24,0xC2EA26,0xC2EA28,0xC2EA2C,
    0xC2EA2E,0xC2EA32,0xC2EA34,0xC2EA38,0xC2EA3A,0xC2EA3E,0xC2EA42,0xC2EA44,
    0xC2EA46,0xC2EA48,0xC2EA4A,0xC2EA4C,0xC2EA4E,0xC2EA52,0xC2EA54,0xC2EA58,
};
int glue_C2E758_owns(uint32_t pc) { return owns_pc(owned_C2E758,sizeof owned_C2E758/sizeof owned_C2E758[0],pc); }
int glue_C2E758_complete_step(void) { if(!glue_C2E758_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C2CE82[]={
    0xC2CE82,0xC2CE88,0xC2CE8C,0xC2CE8E,0xC2CE90,0xC2CE92,0xC2CE94,0xC2CE96,
    0xC2CE98,0xC2CE9A,0xC2CE9E,0xC2CEA0,0xC2CEA2,0xC2CEA4,0xC2CEA6,0xC2CEA8,
    0xC2CEAA,0xC2CEAC,0xC2CEAE,0xC2CEB0,0xC2CEB2,0xC2CEB4,0xC2CEB6,0xC2CEBC,
};
int glue_C2CE82_owns(uint32_t pc) { return owns_pc(owned_C2CE82,sizeof owned_C2CE82/sizeof owned_C2CE82[0],pc); }
int glue_C2CE82_complete_step(void) { if(!glue_C2CE82_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C2CCA0[]={
    0xC2CCA0,0xC2CCA4,0xC2CCA8,0xC2CCAA,0xC2CCB0,0xC2CCB2,0xC2CCBA,0xC2CCBE,
    0xC2CCC4,0xC2CCC6,0xC2CCC8,0xC2CCCA,0xC2CCCE,0xC2CCD4,0xC2CCD6,0xC2CCD8,
    0xC2CCDA,0xC2CCDE,0xC2CCE0,0xC2CCE2,0xC2CCE8,0xC2CCEC,0xC2CCEE,0xC2CCF0,
    0xC2CCF6,0xC2CCFA,0xC2CD00,0xC2CD02,0xC2CD06,0xC2CD08,0xC2CD0A,0xC2CD0C,
    0xC2CD12,0xC2CD14,0xC2CD18,0xC2CD1C,0xC2CD1E,0xC2CD24,0xC2CD26,
};
int glue_C2CCA0_owns(uint32_t pc) { return owns_pc(owned_C2CCA0,sizeof owned_C2CCA0/sizeof owned_C2CCA0[0],pc); }
int glue_C2CCA0_complete_step(void) { if(!glue_C2CCA0_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C2CD28[]={
    0xC2CCBA,0xC2CCBE,0xC2CCC4,0xC2CCC6,0xC2CCC8,0xC2CCCA,0xC2CCCE,0xC2CCD4,
    0xC2CCD6,0xC2CCD8,0xC2CCDA,0xC2CCDE,0xC2CCE0,0xC2CCE2,0xC2CCE8,0xC2CCEC,
    0xC2CCEE,0xC2CCF0,0xC2CCF6,0xC2CCFA,0xC2CD00,0xC2CD02,0xC2CD06,0xC2CD08,
    0xC2CD0A,0xC2CD0C,0xC2CD12,0xC2CD14,0xC2CD18,0xC2CD1C,0xC2CD1E,0xC2CD24,
    0xC2CD26,0xC2CD28,0xC2CD2C,0xC2CD30,0xC2CD32,0xC2CD38,0xC2CD3A,0xC2CD42,
    0xC2CD48,0xC2CD4C,0xC2CD50,0xC2CD56,0xC2CD58,0xC2CD5A,0xC2CD5C,0xC2CD60,
    0xC2CD66,0xC2CD68,0xC2CD6A,0xC2CD6C,0xC2CD72,0xC2CD74,0xC2CD76,0xC2CD78,
    0xC2CD7A,0xC2CD7C,0xC2CD7E,0xC2CD82,0xC2CD86,0xC2CD88,0xC2CD8A,0xC2CD8C,
    0xC2CD8E,0xC2CD90,0xC2D080,0xC2D082,0xC2D08A,0xC2D092,0xC2D098,0xC2D09A,
    0xC2D0A0,0xC2D0A2,0xC2D0A4,0xC2D0AA,0xC2D0AC,0xC2D0B0,0xC2D0B2,0xC2D0B6,
    0xC2D0BA,0xC2D0BC,0xC2D0C0,0xC2D0C4,0xC2D0C6,0xC2D0CA,0xC2D0CE,0xC2D0D0,
    0xC2D0D2,0xC2D0D4,0xC2D0D6,0xC2D0DA,0xC2D0DC,0xC2D0E0,0xC2D0E2,0xC2D0E4,
    0xC2D0E6,0xC2D0EC,0xC2D0F2,0xC2D0F8,0xC2D0FA,0xC2D0FC,0xC2D0FE,0xC2D100,
    0xC2D102,0xC2D104,0xC2D106,0xC2D10A,0xC2D10E,0xC2D110,0xC2D112,0xC2D114,
    0xC2D118,0xC2D11C,0xC2D120,0xC2D124,0xC2D128,0xC2D12A,0xC2D12C,0xC2D12E,
    0xC2D130,0xC2D132,0xC2D136,0xC2D13A,0xC2D13E,0xC2D142,0xC2D146,0xC2D148,
    0xC2D14A,0xC2D14C,0xC2D14E,0xC2D150,0xC2D154,0xC2D158,0xC2D15C,0xC2D160,
    0xC2D164,0xC2D168,0xC2D16A,
};
int glue_C2CD28_owns(uint32_t pc) { return owns_pc(owned_C2CD28,sizeof owned_C2CD28/sizeof owned_C2CD28[0],pc); }
int glue_C2CD28_complete_step(void) { if(!glue_C2CD28_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C2CD94[]={
    0xC2CD94,0xC2CD98,0xC2CD9C,0xC2CD9E,0xC2CDA4,0xC2CDA6,0xC2CDAA,0xC2CDB0,
    0xC2CDB2,0xC2CDB4,0xC2CDB6,0xC2CDBA,0xC2CDC0,0xC2CDC2,0xC2CDC4,0xC2CDC6,
    0xC2CDCA,0xC2CDCC,0xC2CDCE,0xC2CDD4,0xC2CDD8,0xC2CDDA,0xC2CDDC,0xC2CDE2,
    0xC2CDE6,0xC2CDEC,0xC2CDEE,0xC2CDF4,0xC2CDFA,0xC2CE02,0xC2CE0A,0xC2CE10,
    0xC2CE18,0xC2CE1E,0xC2CE22,0xC2CE26,0xC2CE2C,0xC2CE30,0xC2CE32,0xC2CE34,
    0xC2CE38,0xC2CE3C,0xC2CE3E,0xC2CE40,0xC2CE42,0xC2CE48,0xC2CE4A,0xC2CE4C,
    0xC2CE52,0xC2CE54,0xC2CE58,0xC2CE5A,0xC2CE5C,
};
int glue_C2CD94_owns(uint32_t pc) { return owns_pc(owned_C2CD94,sizeof owned_C2CD94/sizeof owned_C2CD94[0],pc); }
int glue_C2CD94_complete_step(void) { if(!glue_C2CD94_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C2D082[]={
    0xC2D080,0xC2D082,0xC2D08A,0xC2D092,0xC2D098,0xC2D09A,0xC2D0A0,0xC2D0A2,
    0xC2D0A4,0xC2D0AA,0xC2D0AC,0xC2D0B0,0xC2D0B2,0xC2D0B6,0xC2D0BA,0xC2D0BC,
    0xC2D0C0,0xC2D0C4,0xC2D0C6,0xC2D0CA,0xC2D0CE,0xC2D0D0,0xC2D0D2,0xC2D0D4,
    0xC2D0D6,0xC2D0DA,0xC2D0DC,0xC2D0E0,0xC2D0E2,0xC2D0E4,0xC2D0E6,0xC2D0EC,
    0xC2D0F2,0xC2D0F8,0xC2D0FA,0xC2D0FC,0xC2D0FE,0xC2D100,0xC2D102,0xC2D104,
    0xC2D106,0xC2D10A,0xC2D10E,0xC2D110,0xC2D112,0xC2D114,0xC2D118,0xC2D11C,
    0xC2D120,0xC2D124,0xC2D128,0xC2D12A,0xC2D12C,0xC2D12E,0xC2D130,0xC2D132,
    0xC2D136,0xC2D13A,0xC2D13E,0xC2D142,0xC2D146,0xC2D148,0xC2D14A,0xC2D14C,
    0xC2D14E,0xC2D150,0xC2D154,0xC2D158,0xC2D15C,0xC2D160,0xC2D164,0xC2D168,
    0xC2D16A,
};
int glue_C2D082_owns(uint32_t pc) { return owns_pc(owned_C2D082,sizeof owned_C2D082/sizeof owned_C2D082[0],pc); }
int glue_C2D082_complete_step(void) { if(!glue_C2D082_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C2D3A4[]={
    0xC2D3A4,0xC2D3AC,0xC2D3AE,0xC2D3B0,0xC2D3B2,0xC2D3B4,0xC2D3B6,0xC2D3B8,
    0xC2D3BA,0xC2D3BC,0xC2D3BE,0xC2D3C0,0xC2D3C6,0xC2D3C8,0xC2D3CA,0xC2D3CC,
    0xC2D3CE,0xC2D3D0,0xC2D3D2,0xC2D3D4,0xC2D3DA,0xC2D3DC,0xC2D3DE,0xC2D3E0,
    0xC2D3E2,0xC2D3E4,0xC2D3E6,0xC2D3E8,0xC2D3EA,0xC2D3EC,0xC2D3EE,0xC2D3F0,
    0xC2D3F2,0xC2D3F4,0xC2D3FA,
};
int glue_C2D3A4_owns(uint32_t pc) { return owns_pc(owned_C2D3A4,sizeof owned_C2D3A4/sizeof owned_C2D3A4[0],pc); }
int glue_C2D3A4_complete_step(void) { if(!glue_C2D3A4_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C200F6[]={
    0xC200F6,0xC200FA,0xC200FE,
};
int glue_C200F6_owns(uint32_t pc) { return owns_pc(owned_C200F6,sizeof owned_C200F6/sizeof owned_C200F6[0],pc); }
int glue_C200F6_complete_step(void) { if(!glue_C200F6_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C203CC[]={
    0xC203CC,0xC203CE,
};
int glue_C203CC_owns(uint32_t pc) { return owns_pc(owned_C203CC,sizeof owned_C203CC/sizeof owned_C203CC[0],pc); }
int glue_C203CC_complete_step(void) { if(!glue_C203CC_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C2058E[]={
    0xC2058E,0xC20590,
};
int glue_C2058E_owns(uint32_t pc) { return owns_pc(owned_C2058E,sizeof owned_C2058E/sizeof owned_C2058E[0],pc); }
int glue_C2058E_complete_step(void) { if(!glue_C2058E_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C20826[]={
    0xC20826,0xC20828,
};
int glue_C20826_owns(uint32_t pc) { return owns_pc(owned_C20826,sizeof owned_C20826/sizeof owned_C20826[0],pc); }
int glue_C20826_complete_step(void) { if(!glue_C20826_owns(REG_PC)) return 0; return corner_view_step(); }
static const uint32_t owned_C22C70[]={
    0xC22C70,
};
int glue_C22C70_owns(uint32_t pc) { return owns_pc(owned_C22C70,sizeof owned_C22C70/sizeof owned_C22C70[0],pc); }
int glue_C22C70_complete_step(void) { if(!glue_C22C70_owns(REG_PC)) return 0; return corner_view_step(); }
