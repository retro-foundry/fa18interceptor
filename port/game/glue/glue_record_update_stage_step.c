/* Record update is retained C; only C1C63E keeps source CPU boundaries.
 * Readable game behavior lives outside this CPU/timing adapter. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "recomp_ports.h"
extern int fa18_write_log_active;
extern int glue_C22C80(void);
extern int glue_schedule_control_records(void);
static int update_step(uint32_t start,uint32_t end) {
    uint32_t pc=REG_PC,value,address,old;
    uint16_t opcode;
    unsigned mode,reg,destination,width;
    if(pc<start || pc>=end) return 0;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u;
    destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC1C660: case 0xC1C68E: case 0xC1C72A: case 0xC1C730:
        width=4; goto move;

    case 0xC1C654: case 0xC1C6BC:
        flags_logic_b(cache_step_read(mode,reg,1)); break;

    case 0xC1C65A: case 0xC1C6C2: case 0xC1C792: case 0xC1C7B8:
        step_branch(pc,opcode,COND_NE()); break;

    case 0xC1C6C4: case 0xC1C720:
        A(destination)=cache_step_address(mode,reg,4); break;

    case 0xC1C6F6: case 0xC1C704:
        width=2; goto subtract_quick;

    case 0xC1C6B0: case 0xC1C6D4: case 0xC1C6DC: case 0xC1C6EC: case 0xC1C6FA: case 0xC1C716: case 0xC1C71E: case 0xC1C74A: case 0xC1C74C: case 0xC1C768: case 0xC1C76A: case 0xC1C7D8: case 0xC1C7E8:
        width=2; goto move;

    case 0xC1C6F2: case 0xC1C700: case 0xC1C74E: case 0xC1C752: case 0xC1C76C: case 0xC1C770:
        width=2; value=m68ki_read_imm_16(); goto and_memory;


    case 0xC1C6B6: case 0xC1C718:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;


    case 0xC1C79E: case 0xC1C7C4:
        step_branch(pc,opcode,COND_LE()); break;

    case 0xC1C756: case 0xC1C75A: case 0xC1C774: case 0xC1C778:
        width=1; goto subtract_quick;

    case 0xC1C6D0: case 0xC1C726:
        value=m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC=pc+2+(int16_t)value; break;

    case 0xC1C694:
        cache_step_write(mode,reg,2,0); flags_logic_w(0); break;


    case 0xC1C67C: case 0xC1C712:
        step_branch(pc,opcode,1); break;

    case 0xC1C64C: case 0xC1C6A0: case 0xC1C6AA: case 0xC1C788: case 0xC1C7AE: case 0xC1C7D4: case 0xC1C7E4:
        step_branch(pc,opcode,COND_EQ()); break;


    case 0xC1C7F4:
        REG_PC=m68ki_pull_32(); break;

    case 0xC1C63E: case 0xC1C7A0: case 0xC1C7C6: case 0xC1C7D6: case 0xC1C7E6:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;

    case 0xC1C640: case 0xC1C64E: case 0xC1C6E4: case 0xC1C70C: case 0xC1C7A2: case 0xC1C7C8:
        width=1; goto move;

    case 0xC1C646: case 0xC1C782: case 0xC1C7A8:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;

    case 0xC1C65C: case 0xC1C68A: case 0xC1C6AC:
        width=1; value=m68ki_read_imm_16(); goto or_memory;

    case 0xC1C666:
        renderer_negate(&D(reg),4); break;

    case 0xC1C668: case 0xC1C670: case 0xC1C67E: case 0xC1C794: case 0xC1C7BA:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;

    case 0xC1C66E: case 0xC1C67A: case 0xC1C688:
        step_branch(pc,opcode,COND_GE()); break;

    case 0xC1C696: case 0xC1C742: case 0xC1C744:
        step_swap(&D(reg)); break;

    case 0xC1C698:
        step_asr_long(&D(reg),destination?destination:8); break;

    case 0xC1C69A: case 0xC1C7CE: case 0xC1C7DE:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;

    case 0xC1C6A2: case 0xC1C78A: case 0xC1C7B0:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;

    case 0xC1C6CA:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;

    case 0xC1C6F8: case 0xC1C706:
        renderer_negate(&D(reg),2); break;

    case 0xC1C708:
        renderer_asl_word(&D(reg),destination?destination:8); break;

    case 0xC1C70A:
        step_add_word(&D(destination),cache_step_read(mode,reg,2)); break;

    case 0xC1C736: case 0xC1C73C:
        width=4; value=m68ki_read_imm_32(); goto and_memory;

    case 0xC1C746: case 0xC1C748: case 0xC1C764: case 0xC1C766:
        renderer_asr_word(&D(reg),destination?destination:8); break;

    case 0xC1C758: case 0xC1C75C: case 0xC1C776: case 0xC1C77A:
        renderer_negate(&D(reg),1); break;

    case 0xC1C75E: case 0xC1C760: case 0xC1C762: case 0xC1C77C: case 0xC1C77E: case 0xC1C780:
        renderer_add_byte(&D(destination),cache_step_read(mode,reg,1)); break;

    case 0xC1C7EE:
        width=1; value=D(destination); goto or_memory;

    default: return 0;
    }
    goto finish;
move:
    value=cache_step_read(mode,reg,width); mode=(opcode>>6)&7u;
    cache_step_write(mode,destination,width,value);
    if(mode!=1) cache_step_logic(value,width); goto finish;
subtract_quick:
    value=destination?destination:8;
    if(mode==0) {
        if(width==1) step_subtract_byte(&D(reg),value); else step_subtract_word(&D(reg),value);
    } else {
        address=cache_step_address(mode,reg,width); old=cache_step_read_memory(address,width);
        if(width==1) step_subtract_byte(&old,value); else step_subtract_word(&old,value);
        cache_step_write_memory(address,old,width,mode==4);
    }
    goto finish;
and_memory:
    if(mode==0) { value&=D(reg); cache_step_write(mode,reg,width,value); }
    else {
        address=cache_step_address(mode,reg,width); value&=cache_step_read_memory(address,width);
        cache_step_write_memory(address,value,width,mode==4);
    }
    cache_step_logic(value,width); goto finish;
or_memory:
    if(mode==0) { value|=D(reg); cache_step_write(mode,reg,width,value); }
    else {
        address=cache_step_address(mode,reg,width); value|=cache_step_read_memory(address,width);
        cache_step_write_memory(address,value,width,mode==4);
    }
    cache_step_logic(value,width);
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
int glue_C22C80_step(void) {
    if(REG_PC!=0xc22c80u) return 0;
    if(fa18_write_log_active) { glue_C22C80(); return 1; }
    REG_PPC=REG_PC; return glue_schedule_control_records();
}
int glue_C1C63E_step(void) { return update_step(0xC1C63Eu,0xC1C7F6u); }
