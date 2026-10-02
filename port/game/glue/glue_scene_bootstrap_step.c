/* Source boundaries for the complete context/scene-bootstrap batch.
 * Readable game semantics live in context_refresh.c and scene_bootstrap.c. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"

static int bootstrap_step(uint32_t start,uint32_t end) {
    uint32_t pc=REG_PC,value,address;
    uint16_t opcode;
    unsigned mode,reg,destination,width,count;
    if(pc<start || pc>=end) return 0;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u;
    destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC1C85E: case 0xC1CA2C: case 0xC090C0: case 0xC0F944:
    case 0xC0FA02: case 0xC090F0: case 0xC0910A: case 0xC0911E:
    case 0xC09190: case 0xC0F4D4: case 0xC11B0C:
        REG_PC=m68ki_pull_32(); break;
    case 0xC1C860:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC1C86A:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC1C86C: case 0xC1C9E0:
        if(opcode&0x0400u) step_restore_registers(); else step_save_registers(); break;
    case 0xC1C870: case 0xC1C906: case 0xC1C90C: case 0xC1C990:
    case 0xC1C9A4: case 0xC1CA26: case 0xC0F9CA: case 0xC0F9E4:
    case 0xC090D2:
        width=1; goto clear;
    case 0xC1C874: case 0xC1C88A: case 0xC1C8FE: case 0xC1C93E:
    case 0xC1C952: case 0xC1C98A: case 0xC08F2E: case 0xC08F4C:
    case 0xC08F52: case 0xC08F58: case 0xC08FBE: case 0xC08FC6:
    case 0xC09030: case 0xC09036: case 0xC09068: case 0xC09094:
    case 0xC090A4: case 0xC0F928: case 0xC0F92E: case 0xC0F934:
    case 0xC0F99E: case 0xC0F9A8: case 0xC0F9B0: case 0xC0F9BA:
    case 0xC0F9D0: case 0xC09102: case 0xC11B02:
        width=1; goto move;
    case 0xC1C87A: case 0xC1C894: case 0xC1C89A: case 0xC1C8A0:
    case 0xC1C8B6: case 0xC1C916: case 0xC1C934: case 0xC1C962:
    case 0xC1C974: case 0xC1C9F2: case 0xC1CA02:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC1C87E: case 0xC1C8A2: case 0xC1C8E6: case 0xC1C8EA:
    case 0xC1C8F2: case 0xC1C8F8: case 0xC1C982: case 0xC1C99C:
    case 0xC1C9AE: case 0xC1C9B6: case 0xC1C9C8: case 0xC1C9D8:
    case 0xC1C9F4: case 0xC1C9F8: case 0xC1CA0E: case 0xC1CA18:
    case 0xC08F5E: case 0xC08FAE: case 0xC08FB6: case 0xC08FCE:
    case 0xC08FFA: case 0xC09002: case 0xC0900A: case 0xC09012:
    case 0xC0901A: case 0xC0903E: case 0xC09050: case 0xC09058:
    case 0xC09060: case 0xC0907A: case 0xC0F9C2:
        width=2; goto move;
    case 0xC1C886: case 0xC1C920: case 0xC1C946: case 0xC1C97E:
    case 0xC08F26: case 0xC08F2A: case 0xC08FAA: case 0xC08FD6:
    case 0xC08FDA: case 0xC090AA:
        value=m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC=pc+2+(int16_t)value; break;
    case 0xC1C88C:
        value=m68ki_read_imm_16(); SET_B(D(reg),D(reg)&value); flags_logic_b(D(reg)); break;
    case 0xC1C890: case 0xC1C896: case 0xC1C89C:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC1C8AA: case 0xC1C996: case 0xC1C9A8: case 0xC1C9C2:
    case 0xC1C9D2: case 0xC1CA20: case 0xC08F70: case 0xC090AE:
    case 0xC090B4: case 0xC090BA: case 0xC0F920: case 0xC0F992:
    case 0xC0F998: case 0xC0F9F0: case 0xC0F4A8: case 0xC0F4B4:
    case 0xC0F4C0: case 0xC0F4CC:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC1C8B0: case 0xC1C9E4: case 0xC1C9EC: case 0xC1C9FC:
        width=1; goto test;
    case 0xC1C8B8: case 0xC1C8BE: case 0xC1CA04: case 0xC08F66:
    case 0xC08FDE: case 0xC09024: case 0xC0902A: case 0xC09046:
    case 0xC0908A: case 0xC0F93E: case 0xC0F9DC: case 0xC0F9FC:
    case 0xC090C8: case 0xC090DE: case 0xC090F8: case 0xC0910C:
    case 0xC09112: case 0xC09118: case 0xC0915A: case 0xC09160:
    case 0xC09166: case 0xC0917E: case 0xC09184: case 0xC0918A:
    case 0xC0F4B2: case 0xC0F4BE: case 0xC0F4CA: case 0xC11AD0:
    case 0xC11AD6: case 0xC11AF2:
        width=4; goto move;
    case 0xC1C8C4: case 0xC1C8CA: case 0xC0916C: case 0xC09172:
        value=m68ki_read_imm_32(); D(reg)&=value; flags_logic_l(D(reg)); break;
    case 0xC1C8D0: case 0xC1C8D2:
        step_swap(&D(reg)); break;
    case 0xC1C8D4: case 0xC1C8D6: case 0xC1C8EE: case 0xC1C8F0:
        renderer_asr_word(&D(reg),destination?destination:8); break;
    case 0xC1C8D8: case 0xC1C9D0: case 0xC1CA16: case 0xC0F9E2:
    case 0xC11B00:
        step_branch(pc,opcode,1); break;
    case 0xC1C8DA: case 0xC08F76: case 0xC08F80: case 0xC08F90:
    case 0xC08F9A: case 0xC09070: case 0xC09084: case 0xC0909A:
    case 0xC0F93A: case 0xC0F9D8: case 0xC0F9F8: case 0xC090C2:
    case 0xC090D8: case 0xC090F2:
        A(destination)=mode==7 && reg==2 ? step_displacement(pc+2) : cache_step_address(mode,reg,4); break;
    case 0xC1C8E0: case 0xC08F88: case 0xC08FA2:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC1C912: case 0xC1C924: case 0xC1C92C: case 0xC1C94A:
    case 0xC1C95A: case 0xC1C96C:
        count=m68ki_read_imm_16(); width=mode==0?4:1; value=cache_step_read(mode,reg,width); FLAG_Z=value&(1u<<(count&(width==4?31u:7u))); break;
    case 0xC1C918: case 0xC1C936: case 0xC1C964: case 0xC1C976:
        count=m68ki_read_imm_16()&7u; address=cache_step_address(mode,reg,1); value=m68k_read_memory_8(address); FLAG_Z=value&(1u<<count); m68k_write_memory_8(address,value&~(1u<<count)); break;
    case 0xC1C9BC:
        value=m68ki_read_imm_16(); SET_W(D(reg),D(reg)&value); flags_logic_w(D(reg)); break;
    case 0xC1C9C0: case 0xC1C9EA: case 0xC0F9A6: case 0xC0F9B8:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC08F38: case 0xC08F3E: case 0xC09078: case 0xC090EA:
    case 0xC11ADE:
        width=2; goto clear;
    case 0xC08F44: case 0xC08F82: case 0xC08F9C: case 0xC08FE8:
    case 0xC08FEE: case 0xC08FF4: case 0xC0F4A6:
        width=4; goto clear;
    case 0xC08F4A: case 0xC08F7C: case 0xC08F7E: case 0xC08F96:
    case 0xC08F98: case 0xC09022: case 0xC09076: case 0xC09082:
    case 0xC090A0: case 0xC090A2: case 0xC0F926: case 0xC0F4B0:
    case 0xC0F4BC: case 0xC0F4C8:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC08F84: case 0xC08F8C: case 0xC08F9E: case 0xC08FA6:
    case 0xC0907E: case 0xC09096: case 0xC090A6: case 0xC090D4:
    case 0xC090EC: case 0xC09106:
        step_dbf(pc,&D(reg)); break;
    case 0xC0907C: case 0xC11AFC:
        width=2; goto quick_add;
    case 0xC09090: case 0xC090CE: case 0xC090E4: case 0xC090FE:
        step_subtract_long(&D(destination),cache_step_read(mode,reg,4)); break;
    case 0xC09092: case 0xC090D0: case 0xC090E8: case 0xC09100:
        step_subtract_word(&D(reg),destination?destination:8); break;
    case 0xC0F9A4: case 0xC0F9B6:
        step_subtract_byte(&D(reg),destination?destination:8); break;
    case 0xC0F9EA:
        address=cache_step_address(mode,reg,4); m68ki_push_32(address); break;
    case 0xC0F9F6: case 0xC0F4AE: case 0xC0F4BA: case 0xC0F4C6:
    case 0xC0F4D2: case 0xC11AF4: case 0xC11AF8:
        width=4; goto quick_add;
    case 0xC090E6:
        SET_W(D(reg),step_lsr_word_value(D(reg),destination?destination:8)); break;
    case 0xC09178: case 0xC0917A: case 0xC0917C:
        renderer_negate(&D(reg),4); break;
    case 0xC11ACC:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC11AE2:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC11AE8:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC11AEA: case 0xC11AEE:
        width=4; goto move;
    case 0xC11B0A:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
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
    else if(mode==0) {
        if(width==2) step_add_word(&D(reg),value); else step_add_long(&D(reg),value);
    } else {
        address=cache_step_address(mode,reg,width);
        { uint32_t old=cache_step_read_memory(address,width);
          if(width==2) step_add_word(&old,value); else step_add_long(&old,value);
          cache_step_write_memory(address,old,width,mode==4); }
    }
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
int glue_C1C860_step(void) { return bootstrap_step(0xC1C85Eu,0xC1CA2Eu); }
int glue_C08F26_step(void) { return bootstrap_step(0xC08F26u,0xC090C2u); }
int glue_C0F920_step(void) { return bootstrap_step(0xC0F920u,0xC0F946u); }
int glue_C0F992_step(void) { return bootstrap_step(0xC0F992u,0xC0FA04u); }
int glue_C090C2_step(void) { return bootstrap_step(0xC090C2u,0xC090F2u); }
int glue_C090F2_step(void) { return bootstrap_step(0xC090F2u,0xC0910Cu); }
int glue_C0910C_step(void) { return bootstrap_step(0xC0910Cu,0xC09120u); }
int glue_C0915A_step(void) { return bootstrap_step(0xC0915Au,0xC09192u); }
int glue_C0F4A6_step(void) { return bootstrap_step(0xC0F4A6u,0xC0F4D6u); }
int glue_C11ACC_step(void) { return bootstrap_step(0xC11ACCu,0xC11B0Eu); }
