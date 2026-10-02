/* C0F5F8 timing boundary. Whole-call game semantics are post_input_tick.c. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"

int glue_C0F5F8_step(void) {
    uint32_t pc=REG_PC,value,address;
    uint16_t opcode;
    unsigned mode,reg,destination,width;
    if(pc<0xc0f5f8u || pc>=0xc0f812u) return 0;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u;
    destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0F5F8:
        m68ki_push_32(A(reg)); A(reg)=A(7);
        A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0F5FC: case 0xC0F608: case 0xC0F61E: case 0xC0F62A:
    case 0xC0F6B2: case 0xC0F6D0: case 0xC0F6DC: case 0xC0F6EA:
    case 0xC0F6F0: case 0xC0F704: case 0xC0F71C: case 0xC0F724:
    case 0xC0F736: case 0xC0F73C: case 0xC0F750: case 0xC0F75C:
    case 0xC0F762: case 0xC0F76C: case 0xC0F77C: case 0xC0F782:
    case 0xC0F79C: case 0xC0F7A8: case 0xC0F7AE: case 0xC0F7D2:
    case 0xC0F7DE: case 0xC0F7E4: case 0xC0F7EC:
        width=1; goto move;
    case 0xC0F6BC: case 0xC0F72C: case 0xC0F788: case 0xC0F7BE:
    case 0xC0F7F2: case 0xC0F7FA:
        width=2; goto move;
    case 0xC0F640: case 0xC0F652: case 0xC0F65E: case 0xC0F66E:
    case 0xC0F678: case 0xC0F684: case 0xC0F6A4: case 0xC0F6A8:
    case 0xC0F6FA: case 0xC0F746: case 0xC0F794: case 0xC0F7CC:
    case 0xC0F800:
        width=4; goto move;
    case 0xC0F602: case 0xC0F60E: case 0xC0F614: case 0xC0F624:
    case 0xC0F630: case 0xC0F6D6: case 0xC0F714: case 0xC0F768:
    case 0xC0F7B4: case 0xC0F7D8:
        width=1; goto test;
    case 0xC0F636: case 0xC0F656: case 0xC0F67C: case 0xC0F694:
        width=4; goto test;
    case 0xC0F604: case 0xC0F698: case 0xC0F6D8: case 0xC0F7DA:
        step_branch(pc,opcode,COND_MI()); break;
    case 0xC0F610: step_branch(pc,opcode,COND_LE()); break;
    case 0xC0F61A: case 0xC0F63C: case 0xC0F65C: case 0xC0F682:
    case 0xC0F71A:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC0F626: case 0xC0F6E6: case 0xC0F70C: case 0xC0F758:
    case 0xC0F7A4:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0F632: case 0xC0F76A: case 0xC0F7B6:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC0F6A2: step_branch(pc,opcode,COND_GE()); break;
    case 0xC0F6BA: case 0xC0F700: case 0xC0F74C: case 0xC0F79A:
        step_branch(pc,opcode,1); break;
    case 0xC0F646: case 0xC0F64C: case 0xC0F664: case 0xC0F672:
    case 0xC0F68A:
        step_subtract_long(&D(destination),cache_step_read(mode,reg,4)); break;
    case 0xC0F66A: case 0xC0F690:
        address=cache_step_address(mode,reg,4); value=m68k_read_memory_32(address);
        step_subtract_long(&value,D(destination)); step_write_long(address,value); break;
    case 0xC0F6AE:
        address=cache_step_address(mode,reg,4); value=m68k_read_memory_32(address);
        step_add_long(&value,D(destination)); step_write_long(address,value); break;
    case 0xC0F69A:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC0F6E2:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC0F6C4: case 0xC0F806:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0F6CA:
        width=4; goto clear;
    case 0xC0F70E: case 0xC0F774: case 0xC0F7B8: case 0xC0F808:
        width=1; goto clear;
    case 0xC0F6E8: case 0xC0F734: case 0xC0F75A: case 0xC0F77A:
    case 0xC0F7A6:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode;
        flags_logic_l(D(destination)); break;
    case 0xC0F6F6: case 0xC0F742: case 0xC0F790: case 0xC0F7C8:
        value=m68ki_read_imm_16(); A(destination)=pc+2+(int16_t)value; break;
    case 0xC0F70A: case 0xC0F756: case 0xC0F7A2: case 0xC0F7DC:
        step_subtract_byte(&D(reg),destination?destination:8); break;
    case 0xC0F7EA: renderer_add_byte(&D(reg),destination?destination:8); break;
    case 0xC0F7F8: step_subtract_word(&D(reg),destination?destination:8); break;
    case 0xC0F80E: A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC0F810: REG_PC=m68ki_pull_32(); break;
    default: return 0;
    }
    goto finish;
move:
    value=cache_step_read(mode,reg,width); mode=(opcode>>6)&7u;
    cache_step_write(mode,destination,width,value);
    if(mode!=1) cache_step_logic(value,width); goto finish;
test:
    cache_step_logic(cache_step_read(mode,reg,width),width); goto finish;
clear:
    cache_step_write(mode,reg,width,0); cache_step_logic(0,width);
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
