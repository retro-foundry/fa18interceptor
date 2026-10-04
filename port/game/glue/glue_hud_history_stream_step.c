/* Complete hud history stream family source CPU/bus/event boundaries.
 * Readable behavior lives in hud_history_stream.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family hud_history_stream. */
#include "glue_hud_history_stream_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_hud_history_stream.h"

static int hud_history_stream_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0CF98: case 0xC0D068: case 0xC0D110: case 0xC0D12C:
    case 0xC1FE24: case 0xC1FE46: case 0xC33378: case 0xC333BC:
    case 0xC333C2: case 0xC333D6: case 0xC333DC: case 0xC333F2:
    case 0xC333F8: case 0xC333FC: case 0xC33400: case 0xC33528:
    case 0xC3352E: case 0xC33582: case 0xC33588: case 0xC335B6:
    case 0xC335DE: case 0xC335E4: case 0xC33600: case 0xC33606:
    case 0xC3361C: case 0xC33622: case 0xC33626: case 0xC3362A:
    case 0xC3371A: case 0xC33720: case 0xC3375A: case 0xC33760:
    case 0xC337FC: case 0xC3389A: case 0xC338C6: case 0xC338CC:
    case 0xC338EE: case 0xC33924: case 0xC3392A: case 0xC3393A:
        A(destination)=(mode==7 && reg==0)?(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16():cache_step_address(mode,reg,4); break;
    case 0xC0CF9E: case 0xC0D06E: case 0xC0D116: case 0xC0D132:
    case 0xC1FE2A: case 0xC1FE4C: case 0xC3337E: case 0xC335BC:
    case 0xC33802: case 0xC338A0: case 0xC338F4: case 0xC33940:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC0CFA0: case 0xC0CFA6: case 0xC0D04C: case 0xC0D05E:
    case 0xC0D10A: case 0xC0D126: case 0xC0D1A2: case 0xC0D1AC:
    case 0xC0D1BE: case 0xC0D1C8: case 0xC0D1D0: case 0xC0D1D8:
    case 0xC0D1DA: case 0xC0D1DC: case 0xC0D1F0: case 0xC0D1F2:
    case 0xC0D1FA: case 0xC0D1FC: case 0xC0D1FE: case 0xC0D200:
    case 0xC0D210: case 0xC0D234: case 0xC0D26E: case 0xC0D2A8:
    case 0xC0D2E4: case 0xC0D32C: case 0xC1FE30: case 0xC1FE52:
    case 0xC33370: case 0xC333DA: case 0xC333E0: case 0xC333E6:
    case 0xC333EE: case 0xC33404: case 0xC3340A: case 0xC334A2:
    case 0xC334AC: case 0xC334B8: case 0xC334D0: case 0xC334D4:
    case 0xC334E6: case 0xC334F6: case 0xC33502: case 0xC33516:
    case 0xC3351A: case 0xC33544: case 0xC33548: case 0xC33570:
    case 0xC33574: case 0xC3359E: case 0xC335A2: case 0xC335A6:
    case 0xC335AE: case 0xC335CC: case 0xC33604: case 0xC3360A:
    case 0xC33610: case 0xC33618: case 0xC3362E: case 0xC33634:
    case 0xC336CE: case 0xC336DA: case 0xC336E2: case 0xC3377A:
    case 0xC3377E: case 0xC33786: case 0xC3378E: case 0xC337A4:
    case 0xC337B4: case 0xC337B8: case 0xC337CA: case 0xC337DC:
    case 0xC337E0: case 0xC33808: case 0xC3388C: case 0xC33892:
    case 0xC338A2: case 0xC338A8: case 0xC338BA: case 0xC338F6:
    case 0xC338FC: case 0xC33900: case 0xC33942: case 0xC33948:
        width=2; goto move;
    case 0xC0CFA8: case 0xC0CFB0: case 0xC0D09E: case 0xC0D0A4:
    case 0xC0D134: case 0xC1FE36: case 0xC1FE40: case 0xC1FE58:
    case 0xC1FE62:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC0CFAC: case 0xC334E8: case 0xC3353E: case 0xC33598:
    case 0xC335AA: case 0xC335B2: case 0xC336F4: case 0xC33732:
    case 0xC33772: case 0xC33782: case 0xC3378A: case 0xC338A4:
    case 0xC338F8: case 0xC33944: case 0xC33958:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC0CFB4: case 0xC0D04A: case 0xC0D332: case 0xC1FE44:
    case 0xC1FE66: case 0xC3395C:
        REG_PC=m68ki_pull_32(); break;
    case 0xC0D048: case 0xC0D14A: case 0xC0D170: case 0xC0D184:
    case 0xC0D1E8: case 0xC333C6: case 0xC333C8: case 0xC33420:
    case 0xC335C2: case 0xC335E8: case 0xC335EA: case 0xC3364C:
    case 0xC33810: case 0xC3385E:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC0D052:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC0D058: case 0xC0D1A0: case 0xC0D1BC: case 0xC335CA:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC0D05A:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0D064: case 0xC0D0DC:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC0D070: case 0xC0D320:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC0D072: case 0xC0D090: case 0xC0D0EC: case 0xC0D21C:
    case 0xC333B2: case 0xC335D4:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC0D07A: case 0xC0D0C2: case 0xC0D0DA: case 0xC0D218:
    case 0xC0D2F8: case 0xC33510: case 0xC33718: case 0xC337EA:
    case 0xC3385C: case 0xC338E6: case 0xC33922:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC0D07C: case 0xC0D082: case 0xC0D086: case 0xC0D0E0:
    case 0xC0D104: case 0xC0D11C: case 0xC0D300: case 0xC0D308:
    case 0xC0D312:
        width=1; goto move;
    case 0xC0D08C: case 0xC0D16E: case 0xC0D206: case 0xC0D20E:
    case 0xC0D222: case 0xC33398: case 0xC33442: case 0xC334C2:
    case 0xC3366E: case 0xC3379C: case 0xC33832:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC0D098: case 0xC0D0F4: case 0xC0D0FC: case 0xC0D14E:
    case 0xC0D154: case 0xC0D15A: case 0xC0D2FE: case 0xC0D31E:
    case 0xC0D328: case 0xC334C8: case 0xC335D0: case 0xC337A2:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC0D09A: case 0xC0D0F6: case 0xC0D2FA: case 0xC0D306:
    case 0xC0D318: case 0xC0D324:
        width=1; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0D0AA: case 0xC0D0B0: case 0xC0D0B6: case 0xC0D138:
    case 0xC0D13E: case 0xC0D144:
        width=4; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC0D0BC: case 0xC0D20A: case 0xC3343E: case 0xC33444:
    case 0xC334C4: case 0xC3366A: case 0xC33670: case 0xC33798:
    case 0xC3379E: case 0xC3382E: case 0xC338C0: case 0xC3391E:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC0D0C4: case 0xC0D0C6: case 0xC0D0C8: case 0xC0D0CA:
    case 0xC0D0CC: case 0xC0D0CE: case 0xC0D186: case 0xC0D188:
    case 0xC0D18A: case 0xC0D18C: case 0xC0D18E: case 0xC0D190:
    case 0xC333A6: case 0xC333A8:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC0D0D0: case 0xC0D0D2: case 0xC0D0D4:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC0D0D6: case 0xC0D0D8: case 0xC333AC: case 0xC333AE:
    case 0xC333B0: case 0xC3349C: case 0xC336C8: case 0xC33884:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC0D0E6:
        width=1; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC0D0F8: case 0xC0D0FE:
        width=1; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC0D108: case 0xC0D1AA: case 0xC0D1B4: case 0xC0D1C6:
    case 0xC0D30C: case 0xC333A0: case 0xC33414: case 0xC33454:
    case 0xC33546: case 0xC335A0: case 0xC3363E: case 0xC33680:
    case 0xC33738: case 0xC33778: case 0xC338FE: case 0xC3394A:
        step_branch(pc,opcode,1); break;
    case 0xC0D120:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC0D122: case 0xC0D124: case 0xC0D128: case 0xC0D12A:
    case 0xC0D24E: case 0xC0D250: case 0xC0D252: case 0xC0D254:
    case 0xC0D288: case 0xC0D28A: case 0xC0D28C: case 0xC0D28E:
    case 0xC0D2C2: case 0xC0D2C4: case 0xC0D2C6: case 0xC0D2C8:
    case 0xC334A6: case 0xC334B2: case 0xC334D6: case 0xC334D8:
    case 0xC334DA: case 0xC336D4: case 0xC336E4: case 0xC336E6:
    case 0xC336E8: case 0xC33792: case 0xC337A8: case 0xC337CE:
    case 0xC33952:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC0D14C: case 0xC0D152: case 0xC0D158: case 0xC0D15E:
    case 0xC0D172: case 0xC3338C: case 0xC3339A: case 0xC333A2:
    case 0xC333AA: case 0xC333CA: case 0xC33422: case 0xC3342E:
    case 0xC33434: case 0xC3344E: case 0xC33456: case 0xC3346C:
    case 0xC33478: case 0xC3347E: case 0xC33484: case 0xC33490:
    case 0xC33496: case 0xC334E4: case 0xC334EC: case 0xC334F8:
    case 0xC334FA: case 0xC33500: case 0xC33518: case 0xC33524:
    case 0xC33526: case 0xC33542: case 0xC3354A: case 0xC3354C:
    case 0xC33572: case 0xC3357E: case 0xC33580: case 0xC3359C:
    case 0xC335F4: case 0xC3364E: case 0xC3365A: case 0xC33660:
    case 0xC3367A: case 0xC33682: case 0xC33698: case 0xC336A4:
    case 0xC336AA: case 0xC336B0: case 0xC336BC: case 0xC336C2:
    case 0xC336F2: case 0xC336F8: case 0xC336FA: case 0xC33704:
    case 0xC3370A: case 0xC33730: case 0xC33736: case 0xC3373A:
    case 0xC3373C: case 0xC33770: case 0xC33776: case 0xC33812:
    case 0xC3381E: case 0xC33824: case 0xC33834: case 0xC3384A:
    case 0xC33860: case 0xC33866: case 0xC3386C: case 0xC33878:
    case 0xC3387E: case 0xC338AA: case 0xC338B4: case 0xC33902:
    case 0xC33910: case 0xC3394C:
        width=4; goto move;
    case 0xC0D150: case 0xC0D156: case 0xC0D15C:
        renderer_negate(&D(reg),4); break;
    case 0xC0D164: case 0xC0D168: case 0xC0D16C: case 0xC0D178:
    case 0xC0D17C: case 0xC0D180:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC0D166: case 0xC0D16A: case 0xC0D17A: case 0xC0D17E:
    case 0xC0D182: case 0xC0D2E2: case 0xC33448: case 0xC33568:
    case 0xC33674: case 0xC33758: case 0xC338C4: case 0xC3390E:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC0D192: case 0xC0D19A: case 0xC0D1B6: case 0xC335C4:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC0D198: case 0xC3338A: case 0xC333B8: case 0xC33558:
    case 0xC335DA: case 0xC33748:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC0D1D4: case 0xC0D1E4: case 0xC0D226: case 0xC0D22A:
    case 0xC0D260: case 0xC0D264: case 0xC0D29A: case 0xC0D29E:
    case 0xC0D2D4: case 0xC0D2D8: case 0xC1FE2C: case 0xC1FE4E:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,2,(int)reg); else { address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); } break;
    case 0xC0D1DE: case 0xC0D1F4: case 0xC0D256: case 0xC0D290:
    case 0xC0D2CA: case 0xC0D2EA: case 0xC1FE3A: case 0xC1FE5C:
    case 0xC333D0: case 0xC333E8: case 0xC3340E: case 0xC33428:
    case 0xC33466: case 0xC3348A: case 0xC334CA: case 0xC3351E:
    case 0xC33578: case 0xC335FA: case 0xC33612: case 0xC33638:
    case 0xC33654: case 0xC33692: case 0xC336B6: case 0xC337AE:
    case 0xC337BC: case 0xC337C4: case 0xC337D6: case 0xC337E4:
    case 0xC337EE: case 0xC337F6: case 0xC33818: case 0xC33844:
    case 0xC33872:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0D1EA: case 0xC0D23E: case 0xC0D240: case 0xC0D242:
    case 0xC0D244: case 0xC0D278: case 0xC0D27A: case 0xC0D27C:
    case 0xC0D27E: case 0xC0D2B2: case 0xC0D2B4: case 0xC0D2B6:
    case 0xC0D2B8: case 0xC0D2E8:
        width=2; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC0D1EC:
        width=2; goto move;
    case 0xC0D1EE: case 0xC0D246: case 0xC0D248: case 0xC0D24A:
    case 0xC0D24C: case 0xC0D280: case 0xC0D282: case 0xC0D284:
    case 0xC0D286: case 0xC0D2BA: case 0xC0D2BC: case 0xC0D2BE:
    case 0xC0D2C0: case 0xC0D2E6: case 0xC3380C:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC0D202: case 0xC3341E: case 0xC334DC: case 0xC335EC:
    case 0xC335F2: case 0xC33644: case 0xC3364A: case 0xC336EA:
    case 0xC3380E:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC0D204: case 0xC0D214: case 0xC0D2F4: case 0xC334C0:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC0D208: case 0xC3341A: case 0xC335EE: case 0xC33646:
        step_divide_unsigned(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC0D230: case 0xC0D23C: case 0xC0D26A: case 0xC0D276:
    case 0xC0D2A4: case 0xC0D2B0: case 0xC333E4: case 0xC33408:
    case 0xC3360E: case 0xC33632:
        step_swap(&D(reg)); break;
    case 0xC0D232: case 0xC0D26C: case 0xC0D2A6:
        A(destination)-=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC0D236: case 0xC0D238: case 0xC0D23A: case 0xC0D270:
    case 0xC0D272: case 0xC0D274: case 0xC0D2AA: case 0xC0D2AC:
    case 0xC0D2AE: case 0xC334D2: case 0xC336E0: case 0xC33898:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC0D25C: case 0xC0D296: case 0xC0D2D0: case 0xC0D2F0:
        width=2; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC0D2DE: case 0xC33384:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC0D30E:
        width=1; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0D31A:
        step_compare_byte(cache_step_read(mode,reg,1),D(destination)); break;
    case 0xC0D330:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC33392: case 0xC3350A: case 0xC33562: case 0xC33712:
    case 0xC33752: case 0xC33856: case 0xC338DC:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC33436: case 0xC3343A: case 0xC33472: case 0xC33662:
    case 0xC33666: case 0xC3369E: case 0xC33826: case 0xC3382A:
    case 0xC33850:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC3344A: case 0xC33676:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC3345C: case 0xC334DE: case 0xC3355C: case 0xC33688:
    case 0xC336EC: case 0xC3374C: case 0xC3383A:
        width=4; value=m68ki_read_imm_32(); operation='+'; goto arithmetic;
    case 0xC33476: case 0xC336A2: case 0xC33854:
        step_lsr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC3349E: case 0xC336CA: case 0xC33886:
        renderer_divide(&D(destination),(int16_t)cache_step_read(mode,reg,2)); break;
    case 0xC334AE: case 0xC33534: case 0xC3356C: case 0xC3358E:
    case 0xC336D0: case 0xC33726: case 0xC33766: case 0xC3388E:
    case 0xC338BC: case 0xC338D2: case 0xC33930:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC334BE: case 0xC337C2: case 0xC337D4: case 0xC337EC:
    case 0xC337F4:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC33504: case 0xC3370C:
        width=4; value=m68ki_read_imm_32(); operation='-'; goto arithmetic;
    case 0xC33512: case 0xC3391A:
        width=2; value=m68ki_read_imm_16(); operation='-'; goto arithmetic;
    case 0xC33538: case 0xC3353A: case 0xC3353C: case 0xC3372A:
    case 0xC3372C: case 0xC3372E: case 0xC338D6: case 0xC338D8:
    case 0xC338DA:
        value=cache_step_read_memory(A(reg)-=reg==7?2:1,1); address=(A(destination)-=destination==7?2:1); old=cache_step_read_memory(address,1); cache_step_write_memory(address,menu_decimal_flags((uint8_t)value,(uint8_t)old),1,0); break;
    case 0xC33552: case 0xC33742: case 0xC33908:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC33592: case 0xC33594: case 0xC33596: case 0xC3376A:
    case 0xC3376C: case 0xC3376E: case 0xC33934: case 0xC33936:
    case 0xC33938:
        value=cache_step_read_memory(A(reg)-=reg==7?2:1,1); address=(A(destination)-=destination==7?2:1); old=cache_step_read_memory(address,1); cache_step_write_memory(address,history_decimal_subtract_flags((uint8_t)value,(uint8_t)old),1,0); break;
    case 0xC335D2: case 0xC3388A:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC338E8:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
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
static const uint32_t owned_C0D04C[]={
    0xC0D048,0xC0D04A,0xC0D04C,0xC0D052,0xC0D058,0xC0D05A,0xC0D05E,0xC0D064,
    0xC0D068,0xC0D06E,0xC0D070,0xC0D072,0xC0D07A,0xC0D07C,0xC0D082,0xC0D086,
    0xC0D08C,0xC0D090,0xC0D098,0xC0D09A,0xC0D09E,0xC0D0A4,0xC0D0AA,0xC0D0B0,
    0xC0D0B6,0xC0D0BC,0xC0D0C2,0xC0D0C4,0xC0D0C6,0xC0D0C8,0xC0D0CA,0xC0D0CC,
    0xC0D0CE,0xC0D0D0,0xC0D0D2,0xC0D0D4,0xC0D0D6,0xC0D0D8,0xC0D0DA,0xC0D0DC,
    0xC0D0E0,0xC0D0E6,0xC0D0EC,0xC0D0F4,0xC0D0F6,0xC0D0F8,0xC0D0FC,0xC0D0FE,
    0xC0D104,0xC0D108,0xC0D10A,0xC0D110,0xC0D116,0xC0D11C,0xC0D120,0xC0D122,
    0xC0D124,0xC0D126,0xC0D128,0xC0D12A,0xC0D12C,0xC0D132,0xC0D134,0xC0D138,
    0xC0D13E,0xC0D144,0xC0D14A,0xC0D14C,0xC0D14E,0xC0D150,0xC0D152,0xC0D154,
    0xC0D156,0xC0D158,0xC0D15A,0xC0D15C,0xC0D15E,0xC0D164,0xC0D166,0xC0D168,
    0xC0D16A,0xC0D16C,0xC0D16E,0xC0D170,0xC0D172,0xC0D178,0xC0D17A,0xC0D17C,
    0xC0D17E,0xC0D180,0xC0D182,0xC0D184,0xC0D186,0xC0D188,0xC0D18A,0xC0D18C,
    0xC0D18E,0xC0D190,0xC0D192,0xC0D198,0xC0D19A,0xC0D1A0,0xC0D1A2,0xC0D1AA,
    0xC0D1AC,0xC0D1B4,0xC0D1B6,0xC0D1BC,0xC0D1BE,0xC0D1C6,0xC0D1C8,0xC0D1D0,
    0xC0D1D4,0xC0D1D8,0xC0D1DA,0xC0D1DC,0xC0D1DE,0xC0D1E4,0xC0D1E8,0xC0D1EA,
    0xC0D1EC,0xC0D1EE,0xC0D1F0,0xC0D1F2,0xC0D1F4,0xC0D1FA,0xC0D1FC,0xC0D1FE,
    0xC0D200,0xC0D202,0xC0D204,0xC0D206,0xC0D208,0xC0D20A,0xC0D20E,0xC0D210,
    0xC0D214,0xC0D218,0xC0D21C,0xC0D222,0xC0D226,0xC0D22A,0xC0D230,0xC0D232,
    0xC0D234,0xC0D236,0xC0D238,0xC0D23A,0xC0D23C,0xC0D23E,0xC0D240,0xC0D242,
    0xC0D244,0xC0D246,0xC0D248,0xC0D24A,0xC0D24C,0xC0D24E,0xC0D250,0xC0D252,
    0xC0D254,0xC0D256,0xC0D25C,0xC0D260,0xC0D264,0xC0D26A,0xC0D26C,0xC0D26E,
    0xC0D270,0xC0D272,0xC0D274,0xC0D276,0xC0D278,0xC0D27A,0xC0D27C,0xC0D27E,
    0xC0D280,0xC0D282,0xC0D284,0xC0D286,0xC0D288,0xC0D28A,0xC0D28C,0xC0D28E,
    0xC0D290,0xC0D296,0xC0D29A,0xC0D29E,0xC0D2A4,0xC0D2A6,0xC0D2A8,0xC0D2AA,
    0xC0D2AC,0xC0D2AE,0xC0D2B0,0xC0D2B2,0xC0D2B4,0xC0D2B6,0xC0D2B8,0xC0D2BA,
    0xC0D2BC,0xC0D2BE,0xC0D2C0,0xC0D2C2,0xC0D2C4,0xC0D2C6,0xC0D2C8,0xC0D2CA,
    0xC0D2D0,0xC0D2D4,0xC0D2D8,0xC0D2DE,0xC0D2E2,0xC0D2E4,0xC0D2E6,0xC0D2E8,
    0xC0D2EA,0xC0D2F0,0xC0D2F4,0xC0D2F8,0xC0D2FA,0xC0D2FE,0xC0D300,0xC0D306,
    0xC0D308,0xC0D30C,0xC0D30E,0xC0D312,0xC0D318,0xC0D31A,0xC0D31E,0xC0D320,
    0xC0D324,0xC0D328,0xC0D32C,0xC0D330,0xC0D332,
};
int glue_C0D04C_owns(uint32_t pc) { return owns_pc(owned_C0D04C,sizeof owned_C0D04C/sizeof owned_C0D04C[0],pc); }
int glue_C0D04C_complete_step(void) { if(!glue_C0D04C_owns(REG_PC)) return 0; return hud_history_stream_step(); }
static const uint32_t owned_C33370[]={
    0xC33370,0xC33378,0xC3337E,0xC33384,0xC3338A,0xC3338C,0xC33392,0xC33398,
    0xC3339A,0xC333A0,0xC333A2,0xC333A6,0xC333A8,0xC333AA,0xC333AC,0xC333AE,
    0xC333B0,0xC333B2,0xC333B8,0xC333BC,0xC333C2,0xC333C6,0xC333C8,0xC333CA,
    0xC333D0,0xC333D6,0xC333DA,0xC333DC,0xC333E0,0xC333E4,0xC333E6,0xC333E8,
    0xC333EE,0xC333F2,0xC333F8,0xC333FC,0xC33400,0xC33404,0xC33408,0xC3340A,
    0xC3340E,0xC33414,0xC3341A,0xC3341E,0xC33420,0xC33422,0xC33428,0xC3342E,
    0xC33434,0xC33436,0xC3343A,0xC3343E,0xC33442,0xC33444,0xC33448,0xC3344A,
    0xC3344E,0xC33454,0xC33456,0xC3345C,0xC33466,0xC3346C,0xC33472,0xC33476,
    0xC33478,0xC3347E,0xC33484,0xC3348A,0xC33490,0xC33496,0xC3349C,0xC3349E,
    0xC334A2,0xC334A6,0xC334AC,0xC334AE,0xC334B2,0xC334B8,0xC334BE,0xC334C0,
    0xC334C2,0xC334C4,0xC334C8,0xC334CA,0xC334D0,0xC334D2,0xC334D4,0xC334D6,
    0xC334D8,0xC334DA,0xC334DC,0xC334DE,0xC334E4,0xC334E6,0xC334E8,0xC334EC,
    0xC334F6,0xC334F8,0xC334FA,0xC33500,0xC33502,0xC33504,0xC3350A,0xC33510,
    0xC33512,0xC33516,0xC33518,0xC3351A,0xC3351E,0xC33524,0xC33526,0xC33528,
    0xC3352E,0xC33534,0xC33538,0xC3353A,0xC3353C,0xC3353E,0xC33542,0xC33544,
    0xC33546,0xC33548,0xC3354A,0xC3354C,0xC33552,0xC33558,0xC3355C,0xC33562,
    0xC33568,0xC3356C,0xC33570,0xC33572,0xC33574,0xC33578,0xC3357E,0xC33580,
    0xC33582,0xC33588,0xC3358E,0xC33592,0xC33594,0xC33596,0xC33598,0xC3359C,
    0xC3359E,0xC335A0,0xC335A2,0xC335A6,0xC335AA,0xC335AE,0xC335B2,0xC335B6,
    0xC335BC,0xC335C2,0xC335C4,0xC335CA,0xC335CC,0xC335D0,0xC335D2,0xC335D4,
    0xC335DA,0xC335DE,0xC335E4,0xC335E8,0xC335EA,0xC335EC,0xC335EE,0xC335F2,
    0xC335F4,0xC335FA,0xC33600,0xC33604,0xC33606,0xC3360A,0xC3360E,0xC33610,
    0xC33612,0xC33618,0xC3361C,0xC33622,0xC33626,0xC3362A,0xC3362E,0xC33632,
    0xC33634,0xC33638,0xC3363E,0xC33644,0xC33646,0xC3364A,0xC3364C,0xC3364E,
    0xC33654,0xC3365A,0xC33660,0xC33662,0xC33666,0xC3366A,0xC3366E,0xC33670,
    0xC33674,0xC33676,0xC3367A,0xC33680,0xC33682,0xC33688,0xC33692,0xC33698,
    0xC3369E,0xC336A2,0xC336A4,0xC336AA,0xC336B0,0xC336B6,0xC336BC,0xC336C2,
    0xC336C8,0xC336CA,0xC336CE,0xC336D0,0xC336D4,0xC336DA,0xC336E0,0xC336E2,
    0xC336E4,0xC336E6,0xC336E8,0xC336EA,0xC336EC,0xC336F2,0xC336F4,0xC336F8,
    0xC336FA,0xC33704,0xC3370A,0xC3370C,0xC33712,0xC33718,0xC3371A,0xC33720,
    0xC33726,0xC3372A,0xC3372C,0xC3372E,0xC33730,0xC33732,0xC33736,0xC33738,
    0xC3373A,0xC3373C,0xC33742,0xC33748,0xC3374C,0xC33752,0xC33758,0xC3375A,
    0xC33760,0xC33766,0xC3376A,0xC3376C,0xC3376E,0xC33770,0xC33772,0xC33776,
    0xC33778,0xC3377A,0xC3377E,0xC33782,0xC33786,0xC3378A,0xC3378E,0xC33792,
    0xC33798,0xC3379C,0xC3379E,0xC337A2,0xC337A4,0xC337A8,0xC337AE,0xC337B4,
    0xC337B8,0xC337BC,0xC337C2,0xC337C4,0xC337CA,0xC337CE,0xC337D4,0xC337D6,
    0xC337DC,0xC337E0,0xC337E4,0xC337EA,0xC337EC,0xC337EE,0xC337F4,0xC337F6,
    0xC337FC,0xC33802,0xC33808,0xC3380C,0xC3380E,0xC33810,0xC33812,0xC33818,
    0xC3381E,0xC33824,0xC33826,0xC3382A,0xC3382E,0xC33832,0xC33834,0xC3383A,
    0xC33844,0xC3384A,0xC33850,0xC33854,0xC33856,0xC3385C,0xC3385E,0xC33860,
    0xC33866,0xC3386C,0xC33872,0xC33878,0xC3387E,0xC33884,0xC33886,0xC3388A,
    0xC3388C,0xC3388E,0xC33892,0xC33898,0xC3389A,0xC338A0,0xC338A2,0xC338A4,
    0xC338A8,0xC338AA,0xC338B4,0xC338BA,0xC338BC,0xC338C0,0xC338C4,0xC338C6,
    0xC338CC,0xC338D2,0xC338D6,0xC338D8,0xC338DA,0xC338DC,0xC338E6,0xC338E8,
    0xC338EE,0xC338F4,0xC338F6,0xC338F8,0xC338FC,0xC338FE,0xC33900,0xC33902,
    0xC33908,0xC3390E,0xC33910,0xC3391A,0xC3391E,0xC33922,0xC33924,0xC3392A,
    0xC33930,0xC33934,0xC33936,0xC33938,0xC3393A,0xC33940,0xC33942,0xC33944,
    0xC33948,0xC3394A,0xC3394C,0xC33952,0xC33958,0xC3395C,
};
int glue_C33370_owns(uint32_t pc) { return owns_pc(owned_C33370,sizeof owned_C33370/sizeof owned_C33370[0],pc); }
int glue_C33370_complete_step(void) { if(!glue_C33370_owns(REG_PC)) return 0; return hud_history_stream_step(); }
static const uint32_t owned_C1FE24[]={
    0xC1FE24,0xC1FE2A,0xC1FE2C,0xC1FE30,0xC1FE36,0xC1FE3A,0xC1FE40,0xC1FE44,
};
int glue_C1FE24_owns(uint32_t pc) { return owns_pc(owned_C1FE24,sizeof owned_C1FE24/sizeof owned_C1FE24[0],pc); }
int glue_C1FE24_complete_step(void) { if(!glue_C1FE24_owns(REG_PC)) return 0; return hud_history_stream_step(); }
static const uint32_t owned_C1FE46[]={
    0xC1FE46,0xC1FE4C,0xC1FE4E,0xC1FE52,0xC1FE58,0xC1FE5C,0xC1FE62,0xC1FE66,
};
int glue_C1FE46_owns(uint32_t pc) { return owns_pc(owned_C1FE46,sizeof owned_C1FE46/sizeof owned_C1FE46[0],pc); }
int glue_C1FE46_complete_step(void) { if(!glue_C1FE46_owns(REG_PC)) return 0; return hud_history_stream_step(); }
static const uint32_t owned_C0CF98[]={
    0xC0CF98,0xC0CF9E,0xC0CFA0,0xC0CFA6,0xC0CFA8,0xC0CFAC,0xC0CFB0,0xC0CFB4,
};
int glue_C0CF98_owns(uint32_t pc) { return owns_pc(owned_C0CF98,sizeof owned_C0CF98/sizeof owned_C0CF98[0],pc); }
int glue_C0CF98_complete_step(void) { if(!glue_C0CF98_owns(REG_PC)) return 0; return hud_history_stream_step(); }
