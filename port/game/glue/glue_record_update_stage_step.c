/* Source boundaries for complete record-update and update-stage owners.
 * Readable game behavior lives outside this CPU/timing adapter. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
static int update_step(uint32_t start,uint32_t end) {
    uint32_t pc=REG_PC,value,address,old;
    uint16_t opcode;
    unsigned mode,reg,destination,width;
    if(pc<start || pc>=end) return 0;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u;
    destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC22C80: case 0xC230AC: case 0xC1C660: case 0xC1C68E:
    case 0xC1C72A: case 0xC1C730:
        width=4; goto move;
    case 0xC22C82: case 0xC22D2A: case 0xC22D32: case 0xC22D40:
    case 0xC22D6A: case 0xC22DB4: case 0xC22DF8: case 0xC22E3C:
    case 0xC22EAE: case 0xC1C654: case 0xC1C6BC:
        flags_logic_b(cache_step_read(mode,reg,1)); break;
    case 0xC22C88: case 0xC22CDC: case 0xC22D30: case 0xC22D70:
    case 0xC22DB0: case 0xC22DBA: case 0xC22DC0: case 0xC22DF6:
    case 0xC22DFE: case 0xC22E04: case 0xC22E3A: case 0xC22E42:
    case 0xC22E48: case 0xC22EAC: case 0xC22EB4: case 0xC22EBA:
    case 0xC22F66: case 0xC22FCA: case 0xC2302E: case 0xC1C65A:
    case 0xC1C6C2: case 0xC1C792: case 0xC1C7B8:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC22C8A: case 0xC22CE4: case 0xC22D5E: case 0xC22D64:
    case 0xC22D9E: case 0xC22DA4: case 0xC22DE4: case 0xC22DEA:
    case 0xC22E28: case 0xC22E2E: case 0xC22E5C: case 0xC22E9A:
    case 0xC22EA0: case 0xC22ECE: case 0xC22F08: case 0xC22F0E:
    case 0xC22F1A: case 0xC22F54: case 0xC22F5A: case 0xC22F7E:
    case 0xC22FB8: case 0xC22FBE: case 0xC22FE2: case 0xC2301C:
    case 0xC23022: case 0xC23046: case 0xC23076: case 0xC1C6C4:
    case 0xC1C720:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC22C90: case 0xC22C92: case 0xC22C96: case 0xC22C9A:
    case 0xC22C9E: case 0xC22CA2: case 0xC22CA6: case 0xC22CAA:
    case 0xC22CAE: case 0xC22CB2: case 0xC22CB6: case 0xC22CBA:
    case 0xC22CBE: case 0xC22CC2: case 0xC22CC6: case 0xC22CCA:
    case 0xC22D72: case 0xC1C6F6: case 0xC1C704:
        width=2; goto subtract_quick;
    case 0xC22CCE: case 0xC22CEA: case 0xC22D8E: case 0xC22D96:
    case 0xC22DD4: case 0xC22DDC: case 0xC22E18: case 0xC22E20:
    case 0xC22E6A: case 0xC22E72: case 0xC22E8A: case 0xC22E92:
    case 0xC22EDC: case 0xC22EE4: case 0xC22EF8: case 0xC22F00:
    case 0xC22F28: case 0xC22F30: case 0xC22F44: case 0xC22F4C:
    case 0xC22F8C: case 0xC22F94: case 0xC22FA8: case 0xC22FB0:
    case 0xC22FF0: case 0xC22FF8: case 0xC2300C: case 0xC23014:
    case 0xC2305A: case 0xC23062: case 0xC2308A: case 0xC23092:
    case 0xC1C6B0: case 0xC1C6D4: case 0xC1C6DC: case 0xC1C6EC:
    case 0xC1C6FA: case 0xC1C716: case 0xC1C71E: case 0xC1C74A:
    case 0xC1C74C: case 0xC1C768: case 0xC1C76A: case 0xC1C7D8:
    case 0xC1C7E8:
        width=2; goto move;
    case 0xC22CD4: case 0xC22D76: case 0xC1C6F2: case 0xC1C700:
    case 0xC1C74E: case 0xC1C752: case 0xC1C76C: case 0xC1C770:
        width=2; value=m68ki_read_imm_16(); goto and_memory;
    case 0xC22CD8:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC22CDE: case 0xC22D88: case 0xC22DCE: case 0xC22E12:
    case 0xC22E56: case 0xC22E84: case 0xC22EC8: case 0xC22EF2:
    case 0xC22F3E: case 0xC22F78: case 0xC22FA2: case 0xC22FDC:
    case 0xC23006: case 0xC23040: case 0xC23070: case 0xC230A0:
    case 0xC230A6: case 0xC1C6B6: case 0xC1C718:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC22CEE: case 0xC22CF2: case 0xC22CF6: case 0xC22CFA:
    case 0xC22CFE: case 0xC22D02: case 0xC22D06: case 0xC22D0A:
    case 0xC22D0E: case 0xC22D12: case 0xC22D16: case 0xC22D1A:
    case 0xC22D1E: case 0xC22D22: case 0xC22D26:
        width=2; value=D(destination); goto and_memory;
    case 0xC22D38: case 0xC22D46: case 0xC1C79E: case 0xC1C7C4:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC22D3A: case 0xC22D48: case 0xC1C756: case 0xC1C75A:
    case 0xC1C774: case 0xC1C778:
        width=1; goto subtract_quick;
    case 0xC22D4E: case 0xC22D7C: case 0xC22D80: case 0xC22D84:
    case 0xC22DBC: case 0xC22DC4: case 0xC22DC8: case 0xC22E00:
    case 0xC22E08: case 0xC22E0C: case 0xC22E44: case 0xC22E4C:
    case 0xC22E50: case 0xC22E7A: case 0xC22E7E: case 0xC22EB6:
    case 0xC22EBE: case 0xC22EC2: case 0xC22EEC: case 0xC22F38:
    case 0xC22F68: case 0xC22F6E: case 0xC22F72: case 0xC22F9C:
    case 0xC22FCC: case 0xC22FD2: case 0xC22FD6: case 0xC23000:
    case 0xC23030: case 0xC23036: case 0xC2303A: case 0xC2306A:
    case 0xC2309A: case 0xC1C6D0: case 0xC1C726:
        value=m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC=pc+2+(int16_t)value; break;
    case 0xC22D52: case 0xC22D58: case 0xC1C694:
        cache_step_write(mode,reg,2,0); flags_logic_w(0); break;
    case 0xC22DAA: case 0xC22DF0: case 0xC22E34: case 0xC22E62:
    case 0xC22EA6: case 0xC22ED4: case 0xC22F14: case 0xC22F20:
    case 0xC22F60: case 0xC22F84: case 0xC22FC4: case 0xC22FE8:
    case 0xC23028: case 0xC2304C: case 0xC2307C:
        value=m68ki_read_imm_16(); address=cache_step_address(mode,reg,1); FLAG_Z=m68k_read_memory_8(address)&(1u<<(value&7u)); break;
    case 0xC22DC2: case 0xC22E06: case 0xC22E4A: case 0xC22EBC:
    case 0xC1C67C: case 0xC1C712:
        step_branch(pc,opcode,1); break;
    case 0xC22DCC: case 0xC22E10: case 0xC22E54: case 0xC22E68:
    case 0xC22E82: case 0xC22EC6: case 0xC22EDA: case 0xC22EF0:
    case 0xC22F26: case 0xC22F3C: case 0xC22F6C: case 0xC22F76:
    case 0xC22F8A: case 0xC22FA0: case 0xC22FD0: case 0xC22FDA:
    case 0xC22FEE: case 0xC23004: case 0xC23034: case 0xC2303E:
    case 0xC23052: case 0xC2306E: case 0xC23082: case 0xC2309E:
    case 0xC1C64C: case 0xC1C6A0: case 0xC1C6AA: case 0xC1C788:
    case 0xC1C7AE: case 0xC1C7D4: case 0xC1C7E4:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC23054: case 0xC23084:
        value=m68ki_read_imm_16(); address=cache_step_address(mode,reg,1); old=m68k_read_memory_8(address); value=1u<<(value&7u); FLAG_Z=old&value; m68k_write_memory_8(address,old|value); break;
    case 0xC230AE: case 0xC1C7F4:
        REG_PC=m68ki_pull_32(); break;
    case 0xC1C63E: case 0xC1C7A0: case 0xC1C7C6: case 0xC1C7D6:
    case 0xC1C7E6:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC1C640: case 0xC1C64E: case 0xC1C6E4: case 0xC1C70C:
    case 0xC1C7A2: case 0xC1C7C8:
        width=1; goto move;
    case 0xC1C646: case 0xC1C782: case 0xC1C7A8:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;
    case 0xC1C65C: case 0xC1C68A: case 0xC1C6AC:
        width=1; value=m68ki_read_imm_16(); goto or_memory;
    case 0xC1C666:
        renderer_negate(&D(reg),4); break;
    case 0xC1C668: case 0xC1C670: case 0xC1C67E: case 0xC1C794:
    case 0xC1C7BA:
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
    case 0xC1C75E: case 0xC1C760: case 0xC1C762: case 0xC1C77C:
    case 0xC1C77E: case 0xC1C780:
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
int glue_C22C80_step(void) { return update_step(0xC22C80u,0xC230B0u); }
int glue_C1C63E_step(void) { return update_step(0xC1C63Eu,0xC1C7F6u); }
