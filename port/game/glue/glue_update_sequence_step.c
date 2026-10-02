/* C0EFD4, C0F3C4 and C0D730 source instruction boundaries.
 * Readable owners live in update_sequence.c and pending_input.c. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
static int update_step(void) {
    uint32_t pc=REG_PC,value,address;
    uint16_t opcode;
    unsigned mode,reg,destination,width;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0EFD4: case 0xC0F3C4:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0EFD8: case 0xC0EFFA: case 0xC0F008: case 0xC0F01C:
    case 0xC0F048: case 0xC0F062: case 0xC0F082: case 0xC0F09E:
    case 0xC0F0AC: case 0xC0F0BA: case 0xC0F0C8: case 0xC0F0EA:
    case 0xC0F0F8: case 0xC0F108: case 0xC0F116: case 0xC0F124:
    case 0xC0F138: case 0xC0F154: case 0xC0F1AA: case 0xC0F1CE:
    case 0xC0F1EC: case 0xC0F216: case 0xC0F234: case 0xC0F248:
    case 0xC0F29E: case 0xC0F2DC: case 0xC0F2FC: case 0xC0F30E:
    case 0xC0F324: case 0xC0F342: case 0xC0F360: case 0xC0F368:
    case 0xC0F396: case 0xC0F3A4: case 0xC0F3B2: case 0xC0F3D4:
    case 0xC0F3DA: case 0xC0F3E2: case 0xC0F3E8: case 0xC0F420:
    case 0xC0F426: case 0xC0F466: case 0xC0F470: case 0xC0F47C:
    case 0xC0F482: case 0xC0F492: case 0xC0D730:
        width=2; goto move;
    case 0xC0EFE0: case 0xC0D744:
        value=m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC=pc+2+(int16_t)value; break;
    case 0xC0EFE4: case 0xC0EFEA: case 0xC0F002: case 0xC0F010:
    case 0xC0F016: case 0xC0F024: case 0xC0F02A: case 0xC0F030:
    case 0xC0F036: case 0xC0F03C: case 0xC0F042: case 0xC0F050:
    case 0xC0F056: case 0xC0F05C: case 0xC0F07C: case 0xC0F08A:
    case 0xC0F0A6: case 0xC0F0B4: case 0xC0F0C2: case 0xC0F0D8:
    case 0xC0F0E0: case 0xC0F0F2: case 0xC0F100: case 0xC0F110:
    case 0xC0F11E: case 0xC0F12C: case 0xC0F132: case 0xC0F17C:
    case 0xC0F182: case 0xC0F188: case 0xC0F18E: case 0xC0F194:
    case 0xC0F19A: case 0xC0F1B8: case 0xC0F1BE: case 0xC0F1DC:
    case 0xC0F206: case 0xC0F224: case 0xC0F242: case 0xC0F250:
    case 0xC0F256: case 0xC0F25C: case 0xC0F262: case 0xC0F268:
    case 0xC0F26E: case 0xC0F274: case 0xC0F27A: case 0xC0F280:
    case 0xC0F286: case 0xC0F2B8: case 0xC0F2BE: case 0xC0F2C4:
    case 0xC0F2D6: case 0xC0F2E4: case 0xC0F2EA: case 0xC0F2F0:
    case 0xC0F2F6: case 0xC0F308: case 0xC0F31C: case 0xC0F330:
    case 0xC0F350: case 0xC0F370: case 0xC0F378: case 0xC0F380:
    case 0xC0F39E: case 0xC0F3AC: case 0xC0F3BA: case 0xC0F3C8:
    case 0xC0F3CE: case 0xC0F3EE: case 0xC0F410: case 0xC0F432:
    case 0xC0F43A: case 0xC0F456: case 0xC0D73C:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0EFF0: case 0xC0F070: case 0xC0F074: case 0xC0F0D0:
    case 0xC0F146: case 0xC0F14A: case 0xC0F172: case 0xC0F1A6:
    case 0xC0F1CA: case 0xC0F1E8: case 0xC0F212: case 0xC0F230:
    case 0xC0F292: case 0xC0F2A8: case 0xC0F33E: case 0xC0F35C:
    case 0xC0F386: case 0xC0F38E: case 0xC0F460:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC0EFF6: case 0xC0F072: case 0xC0F07A: case 0xC0F0D6:
    case 0xC0F0E8: case 0xC0F148: case 0xC0F150: case 0xC0F178:
    case 0xC0F2AE: case 0xC0F38C: case 0xC0F394: case 0xC0F3E0:
    case 0xC0F402: case 0xC0F40E: case 0xC0F430: case 0xC0F448:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC0F06A: case 0xC0F140: case 0xC0F168: case 0xC0F1A0:
    case 0xC0F1C4: case 0xC0F1E2: case 0xC0F20C: case 0xC0F22A:
    case 0xC0F28C: case 0xC0F298: case 0xC0F2B0: case 0xC0F2CC:
    case 0xC0F338: case 0xC0F356: case 0xC0F3F4: case 0xC0F3FA:
    case 0xC0F440: case 0xC0F44A:
        width=1; goto move;
    case 0xC0F090: case 0xC0F1FA:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC0F09A: case 0xC0F1B6: case 0xC0F204:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC0F0DE: case 0xC0F106: case 0xC0F2A6: case 0xC0F336:
    case 0xC0F36E: case 0xC0F416: case 0xC0F438: case 0xC0F45E:
    case 0xC0F488:
        step_branch(pc,opcode,1); break;
    case 0xC0F0E6:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC0F15A: case 0xC0F47A:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC0F15C:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC0F15E:
        step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC0F160:
        width=4; goto move;
    case 0xC0F162:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC0F16C: case 0xC0F3FE: case 0xC0F418: case 0xC0F444:
    case 0xC0F48A:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC0F170: case 0xC0F2D4: case 0xC0F306: case 0xC0F318:
    case 0xC0F32E: case 0xC0F340: case 0xC0F34E: case 0xC0F35E:
    case 0xC0F406: case 0xC0F41E: case 0xC0F464: case 0xC0F490:
    case 0xC0D73A:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0F1A8: case 0xC0F1CC: case 0xC0F1EA: case 0xC0F214:
    case 0xC0F232:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC0F1AE: case 0xC0F1D2: case 0xC0F1F0: case 0xC0F21A:
    case 0xC0F238: case 0xC0F300: case 0xC0F312: case 0xC0F328:
    case 0xC0F346: case 0xC0D736:
        width=2; value=m68ki_read_imm_16(); value&=D(reg); cache_step_write(mode,reg,width,value); cache_step_logic(value,width); break;
    case 0xC0F1B2: case 0xC0F1D6: case 0xC0F1F4: case 0xC0F21E:
    case 0xC0F23C: case 0xC0F34A:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC0F1DA: case 0xC0F222: case 0xC0F240:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC0F1F8:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC0F294:
        step_branch(pc,opcode,COND_MI()); break;
    case 0xC0F296: case 0xC0F2D2: case 0xC0F404:
        step_subtract_byte(&D(reg),destination?destination:8); break;
    case 0xC0F2CA:
        break;
    case 0xC0F304: case 0xC0F316: case 0xC0F32C:
        step_subtract_word(&D(reg),destination?destination:8); break;
    case 0xC0F31A: case 0xC0F376:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC0F322: case 0xC0F37E: case 0xC0F45C:
        if(mode==1) A(reg)+=destination?destination:8; else step_add_long(&D(reg),destination?destination:8); break;
    case 0xC0F366:
        if(mode==1) A(reg)+=destination?destination:8; else step_add_word(&D(reg),destination?destination:8); break;
    case 0xC0F3C0: case 0xC0F4A2:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC0F3C2: case 0xC0F4A4: case 0xC0D742: case 0xC0D748:
        REG_PC=m68ki_pull_32(); break;
    case 0xC0F3DE:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC0F408: case 0xC0F42E:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC0F42C:
        SET_W(D(destination),D(destination)|cache_step_read(mode,reg,2)); flags_logic_w(D(destination)); break;
    case 0xC0F44E:
        width=4; value=m68ki_read_imm_32(); value&=D(reg); cache_step_write(mode,reg,width,value); cache_step_logic(value,width); break;
    case 0xC0F454:
        width=4; goto move;
    case 0xC0F49C:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    default: return 0;
    }
    goto finish;
move:
    value=cache_step_read(mode,reg,width); mode=(opcode>>6)&7u;
    cache_step_write(mode,destination,width,value);
    if(mode!=1) cache_step_logic(value,width);
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
int glue_C0EFD4_step(void) {
    if(REG_PC<0xc0efd4u || REG_PC>=0xc0f3c4u) return 0;
    return update_step();
}
int glue_C0F3C4_step(void) {
    if(REG_PC<0xc0f3c4u || REG_PC>=0xc0f4a6u) return 0;
    return update_step();
}
int glue_C0D730_step(void) {
    if(REG_PC<0xc0d730u || REG_PC>=0xc0d74au) return 0;
    return update_step();
}
