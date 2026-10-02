/* Complete follow-up parent and its shared position/list children. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"

static int followup_step(uint32_t start,uint32_t end) {
    uint32_t pc=REG_PC,value,address;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width,count;
    if(pc<start || pc>=end) return 0;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC1CCBC: case 0xC1CCC4: case 0xC1CDB8: case 0xC1CDD8:
    case 0xC1CE5A: case 0xC1CE60: case 0xC1CF52: case 0xC1CF5E:
    case 0xC1D102:
        width=1; goto move;
    case 0xC1CCCC: case 0xC1CE9E: case 0xC1CFD6: case 0xC1CFDC:
        width=1; goto clear;
    case 0xC1CCD2: case 0xC1CCDE: case 0xC1D01A:
        width=2; goto clear;
    case 0xC1CCD8: case 0xC2589C: case 0xC258B6: case 0xC258B8:
    case 0xC258BC:
        width=4; goto clear;
    case 0xC1CCE4: case 0xC1CCF6: case 0xC1CDB2: case 0xC1CE02:
    case 0xC1CE42: case 0xC1CFA8: case 0xC1D020: case 0xC1D0A4:
    case 0xC1D0B6: case 0xC2587A:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC1CCEA: case 0xC1CD0C: case 0xC1CE1A: case 0xC1CE4E:
    case 0xC1D0C6:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC1CCF0: case 0xC1CCFC: case 0xC1CD06: case 0xC1CD10:
    case 0xC1CD2C: case 0xC1CD4E: case 0xC1CD60: case 0xC1CD88:
    case 0xC1CD8E: case 0xC1CD96: case 0xC1CDA0: case 0xC1CDAE:
    case 0xC1CDBE: case 0xC1CE08: case 0xC1CE12: case 0xC1CE38:
    case 0xC1CE48: case 0xC1CE50: case 0xC1CE66: case 0xC1CE94:
    case 0xC1CE98: case 0xC1CEEC: case 0xC1CF04: case 0xC1CF0A:
    case 0xC1CF32: case 0xC1CF42: case 0xC1CF66: case 0xC1CF6A:
    case 0xC1CF76: case 0xC1CF8C: case 0xC1CF90: case 0xC1CFAE:
    case 0xC1CFBA: case 0xC1CFF6: case 0xC1CFFC: case 0xC1D028:
    case 0xC1D080: case 0xC1D08A: case 0xC1D0AA: case 0xC1D0AC:
    case 0xC1D0BC: case 0xC1D0BE: case 0xC25876: case 0xC25878:
    case 0xC25886: case 0xC25888: case 0xC2588A: case 0xC258B0:
    case 0xC258B2:
        width=2; goto move;
    case 0xC1CCF2: case 0xC1CD9E: case 0xC1D078:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC1CD02:
        renderer_asl_word(&D(reg),destination?destination:8); break;
    case 0xC1CD04: case 0xC1CE0E: case 0xC1CE10: case 0xC1CE14:
    case 0xC1CE16: case 0xC1CE18: case 0xC1CF74: case 0xC1D0C4:
        step_add_word(&D(destination),cache_step_read(mode,reg,2)); break;
    case 0xC1CD0E: case 0xC1CD2A: case 0xC1CFB8: case 0xC1D026:
    case 0xC1D070:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC1CD14: case 0xC1CD30:
        step_subtract_word(&D(destination),cache_step_read(mode,reg,2)); break;
    case 0xC1CD1A: case 0xC1CD36:
        step_swap(&D(reg)); break;
    case 0xC1CD1C: case 0xC1CD38: case 0xC1CEBA: case 0xC1CEBC:
    case 0xC1CEBE: case 0xC1CEF2: case 0xC1D0D8: case 0xC1D0E8:
    case 0xC25898: case 0xC258AC:
        count=opcode&0x20u?D(destination)&63u:destination?destination:8; step_asl_long(&D(reg),count); break;
    case 0xC1CD1E: case 0xC1CD3A: case 0xC1CD46: case 0xC1CD48:
    case 0xC1CD72: case 0xC1CDC4: case 0xC1CDC8: case 0xC1CDD2:
    case 0xC1CE20: case 0xC1CE26: case 0xC1CE8E: case 0xC1CF9A:
    case 0xC1CFA0: case 0xC1D04C: case 0xC1D052: case 0xC1D058:
    case 0xC1D066: case 0xC1D06E: case 0xC1D0CC: case 0xC1D0DC:
    case 0xC1D0EC: case 0xC1D0F0: case 0xC1D0FC: case 0xC2589A:
    case 0xC258AE: case 0xC258C0:
        width=4; goto move;
    case 0xC1CD22: case 0xC1CD3E: case 0xC1D0D0: case 0xC1D0E0:
        value=m68ki_read_imm_32(); D(reg)&=value; flags_logic_l(D(reg)); break;
    case 0xC1CD28: case 0xC1CD44: case 0xC1CD56: case 0xC1CD68:
    case 0xC1CD76: case 0xC1CDCA: case 0xC1D0DA: case 0xC1D0EA:
    case 0xC1D0F4: case 0xC25892: case 0xC25894: case 0xC258A6:
    case 0xC258A8:
        step_add_long(&D(destination),cache_step_read(mode,reg,4)); break;
    case 0xC1CD4A: case 0xC1CD4C: case 0xC1CD5C: case 0xC1CD5E:
    case 0xC1CD6E: case 0xC1CD70: case 0xC1CD80: case 0xC1CD82:
    case 0xC1CDD0: case 0xC1CDE0: case 0xC1CDE2: case 0xC1CDE4:
    case 0xC1CDEE: case 0xC1CDF0: case 0xC1CDF2: case 0xC1CED4:
    case 0xC1CED6: case 0xC1CED8: case 0xC1D07C: case 0xC1D084:
    case 0xC1D086: case 0xC1D088: case 0xC1D0D6: case 0xC1D0E6:
    case 0xC1D0F2: case 0xC1D0FA: case 0xC25896: case 0xC258AA:
        count=opcode&0x20u?D(destination)&63u:destination?destination:8; step_asr_long(&D(reg),count); break;
    case 0xC1CD54: case 0xC1CD66: case 0xC1CEF0:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC1CD58: case 0xC1CD6A: case 0xC1CD7C: case 0xC1CD86:
    case 0xC1CD8C: case 0xC1CD94: case 0xC1CEFA: case 0xC1CF4A:
    case 0xC1CF50: case 0xC1D04E: case 0xC1D054: case 0xC1D05A:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC1CD5A: case 0xC1CD6C: case 0xC1CD7E: case 0xC1D050:
    case 0xC1D056: case 0xC1D05C:
        renderer_negate(&D(reg),4); break;
    case 0xC1CD84: case 0xC1CD8A: case 0xC1CD92: case 0xC1CF7C:
    case 0xC1D02E:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC1CD90: case 0xC1CE34: case 0xC1CEC0: case 0xC1CEC6:
    case 0xC1CEE2: case 0xC1CF18: case 0xC1CFC6: case 0xC1D012:
    case 0xC1D068: case 0xC1D07E: case 0xC1D0B4:
        step_branch(pc,opcode,1); break;
    case 0xC1CD98: case 0xC1CE96:
        SET_W(D(reg),step_lsr_word_value(D(reg),destination?destination:8)); break;
    case 0xC1CD9A: case 0xC1CE52: case 0xC1CF1E: case 0xC1D09C:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC1CDA8: case 0xC1CDFC: case 0xC1CE2C: case 0xC1CF2C:
    case 0xC1CFA6: case 0xC1D092:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC1CDBC:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC1CDE6: case 0xC1CECC: case 0xC1CEE4: case 0xC1D03A:
        width=4; goto multiple;
    case 0xC1CDF4: case 0xC1CE8A: case 0xC1CEB2: case 0xC1CEDA:
    case 0xC1CF24:
        width=2; goto multiple;
    case 0xC1CE1C: case 0xC1CE1E: case 0xC1CE6C: case 0xC1CF96:
    case 0xC1CF98: case 0xC25880:
        width=4; goto move;
    case 0xC1CE2E: case 0xC1D07A: case 0xC1D082: case 0xC2589E:
    case 0xC258B4:
        width=2; goto quick_add;
    case 0xC1CE56: case 0xC1CF16: case 0xC1D030: case 0xC1D038:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC1CE62: case 0xC1CF12: case 0xC1CF1A: case 0xC1CF70:
    case 0xC1CFFE: case 0xC1D00A: case 0xC1D0AE: case 0xC1D0C0:
    case 0xC1D0C8:
        value=m68ki_read_imm_16(); SET_W(D(reg),D(reg)&value); flags_logic_w(D(reg)); break;
    case 0xC1CE6E: case 0xC1CFB4:
        width=2; goto test;
    case 0xC1CE70: case 0xC1CE86: case 0xC1CF02: case 0xC1D060:
    case 0xC1D0A0:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC1CE74: case 0xC1CE7C: case 0xC1CEF4: case 0xC1CEFC:
    case 0xC1D072:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC1CE7A: case 0xC1CEA8: case 0xC1CEB0: case 0xC1CF10:
    case 0xC1CF22: case 0xC1CF3A: case 0xC1CF40: case 0xC1CF82:
    case 0xC1CF8A: case 0xC1CFE8: case 0xC1D002: case 0xC1D00E:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC1CEA4: case 0xC1CEAC: case 0xC1CF0C: case 0xC1CF36:
    case 0xC1CF3C: case 0xC1D032:
        count=m68ki_read_imm_16(); width=mode==0?4:1; value=cache_step_read(mode,reg,width); FLAG_Z=value&(1u<<(count&(width==4?31u:7u))); break;
    case 0xC1CEC2: case 0xC1CEC8:
        value=m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC=pc+2+(int16_t)value; break;
    case 0xC1CF4C: case 0xC1CF5A:
        address=cache_step_address(mode,reg,1); value=m68k_read_memory_8(address); step_subtract_byte(&value,destination?destination:8); m68k_write_memory_8(address,value); break;
    case 0xC1CF84: case 0xC1CFE2:
        width=1; goto test;
    case 0xC1CFB6: case 0xC1CFF4: case 0xC1D064: case 0xC1D06C:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC1CFBE: case 0xC1D098:
        value=m68ki_read_imm_16(); address=mode==0?0:cache_step_address(mode,reg,2); if (mode==0) step_add_word(&D(reg),value); else { uint32_t old=m68k_read_memory_16(address); step_add_word(&old,value); step_write_word(address,old); } break;
    case 0xC1CFEC:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC1D004: case 0xC1D014:
        width=1; goto quick_add;
    case 0xC1D040: case 0xC1D044: case 0xC1D048:
        step_subtract_long(&D(destination),cache_step_read(mode,reg,4)); break;
    case 0xC1D05E: case 0xC1D062: case 0xC1D06A:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC1D08C: case 0xC1D0B2:
        renderer_asr_word(&D(reg),destination?destination:8); break;
    case 0xC1D08E:
        count=m68ki_read_imm_16()&31u; FLAG_Z=D(reg)&(1u<<count); D(reg)|=1u<<count;
        if(count<16) USE_CYCLES(-2); break;
    case 0xC1D0A2: case 0xC1D10A: case 0xC258C6:
        REG_PC=m68ki_pull_32(); break;
    case 0xC2588C: case 0xC2588E: case 0xC25890: case 0xC258A0:
    case 0xC258A2: case 0xC258A4:
        renderer_multiply(&D(destination),cache_step_read(mode,reg,2)); break;
    default: return 0;
    }
    goto finish;
move:
    value=cache_step_read(mode,reg,width); mode=(opcode>>6)&7u;
    cache_step_write(mode,destination,width,value);
    if(mode!=1) cache_step_logic(value,width); goto finish;
clear:
    cache_step_write(mode,reg,width,0); cache_step_logic(0,width); goto finish;
test:
    cache_step_logic(cache_step_read(mode,reg,width),width); goto finish;
quick_add:
    value=destination?destination:8;
    if(mode==1) A(reg)+=value;
    else if(mode==0) step_add_word(&D(reg),value);
    else { address=cache_step_address(mode,reg,width);
        { uint32_t old=cache_step_read_memory(address,width);
          if(width==1) renderer_add_byte(&old,value); else step_add_word(&old,value);
          cache_step_write_memory(address,old,width,mode==4); }
    }
    goto finish;
multiple:
    mask=m68ki_read_imm_16(); address=mode==3?A(reg):cache_step_address(mode,reg,width);
    if(opcode&0x0400u) renderer_load(address,mask,width,mode==3?(int)reg:-1);
    else renderer_store(address,mask,width,-1);
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
int glue_C1CCBC_step(void) { return followup_step(0xc1ccbcu,0xc1d0a4u); }
int glue_C1D0A4_step(void) { return followup_step(0xc1d0a4u,0xc1d10cu); }
int glue_C1D0B6_step(void) { return glue_C1D0A4_step(); }
int glue_C25876_step(void) { return followup_step(0xc25876u,0xc258c8u); }
