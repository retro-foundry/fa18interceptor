/* Complete C29042 source boundaries, including internal cold policies.
 * Readable selector-origin behavior lives in selector_origin.c. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
static uint32_t origin_address(uint32_t pc,unsigned mode,unsigned reg,unsigned width) {
    if(mode==7 && reg==2) return step_displacement(pc+2);
    return cache_step_address(mode,reg,width);
}
int glue_C29042_step(void) {
    uint32_t pc=REG_PC,value,address,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width;
    if(pc<0xc29040u || pc>=0xc295d2u) return 0;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC29040: case 0xC29344: case 0xC29366: case 0xC29408:
    case 0xC294AA: case 0xC29546: case 0xC295D0:
        REG_PC=m68ki_pull_32(); break;
    case 0xC29042: case 0xC29188: case 0xC29190: case 0xC29336:
    case 0xC293D0: case 0xC293D6: case 0xC294F4: case 0xC29560:
        address=origin_address(pc,mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC29048: case 0xC29050: case 0xC29058: case 0xC29060:
    case 0xC29068: case 0xC2917E: case 0xC292FA:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC2904E: case 0xC29056: case 0xC29066: case 0xC29096:
    case 0xC290F0: case 0xC290FC: case 0xC29114: case 0xC2911C:
    case 0xC2917C: case 0xC29180: case 0xC29186: case 0xC293AE:
    case 0xC293BC: case 0xC294BA: case 0xC294C4: case 0xC294E2:
    case 0xC294EC: case 0xC29582:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC2905E: case 0xC2906E: case 0xC290BE: case 0xC291B0:
    case 0xC293A6: case 0xC293C6:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC29070: case 0xC290A4: case 0xC290C0: case 0xC2911E:
    case 0xC29124: case 0xC2912A: case 0xC2921C: case 0xC2937E:
    case 0xC2940E:
        A(destination)=origin_address(pc,mode,reg,4); break;
    case 0xC29076: case 0xC290AA: case 0xC29392: case 0xC29414:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC2907C: case 0xC29080: case 0xC29086: case 0xC29170:
    case 0xC291C6: case 0xC291E6: case 0xC291EE: case 0xC291F6:
    case 0xC29204: case 0xC2920C: case 0xC29210: case 0xC293EC:
    case 0xC293F0: case 0xC293F4: case 0xC293FA: case 0xC2957C:
        width=4; goto move;
    case 0xC2908A: case 0xC290C4: case 0xC29122: case 0xC29128:
    case 0xC29166: case 0xC2918E: case 0xC291B4: case 0xC291D0:
    case 0xC29206: case 0xC2920E: case 0xC29254: case 0xC29278:
    case 0xC2929C: case 0xC292D8: case 0xC292F0: case 0xC292F6:
    case 0xC293BE: case 0xC293DC: case 0xC29486: case 0xC2948C:
    case 0xC29498: case 0xC294CA: case 0xC294D0: case 0xC294F0:
    case 0xC29558:
        step_branch(pc,opcode,1); break;
    case 0xC2908E: case 0xC29098: case 0xC290BA: case 0xC290F2:
    case 0xC290F8: case 0xC29100: case 0xC2910E: case 0xC29116:
    case 0xC29178: case 0xC29182: case 0xC293A2: case 0xC294B2:
    case 0xC294BC: case 0xC294DA: case 0xC294E4: case 0xC2950E:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC290A0: case 0xC290D4: case 0xC290F6: case 0xC291C4:
    case 0xC291E8: case 0xC291F0: case 0xC291F8: case 0xC2931A:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC290B0: case 0xC290B8: case 0xC290EA: case 0xC29108:
    case 0xC2912E: case 0xC29144: case 0xC29172: case 0xC291A8:
    case 0xC29212: case 0xC29232: case 0xC2924C: case 0xC29270:
    case 0xC29294: case 0xC292D0: case 0xC292E8: case 0xC29326:
    case 0xC2932E: case 0xC2935E: case 0xC29368: case 0xC29376:
    case 0xC2939A: case 0xC293E4: case 0xC29518: case 0xC29520:
    case 0xC29528:
        width=1; goto move;
    case 0xC290B4: case 0xC291AC: case 0xC2939E:
        width=1; value=m68ki_read_imm_16(); goto and_value;
    case 0xC290C8: case 0xC290CC: case 0xC290D8: case 0xC29138:
    case 0xC29154: case 0xC29156: case 0xC29158: case 0xC291B6:
    case 0xC2938E: case 0xC293B0: case 0xC293C8: case 0xC293DE:
    case 0xC2955A: case 0xC2955C: case 0xC29566:
        width=2; goto move;
    case 0xC290D2:
        step_subtract_word(&D(destination),cache_step_read(mode,reg,2)); break;
    case 0xC290D6:
        renderer_negate(&D(reg),2); break;
    case 0xC290DE: case 0xC290E4: case 0xC2914E:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC290E2: case 0xC29104: case 0xC2914A: case 0xC2938C:
    case 0xC29516: case 0xC2954E:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC290E8: case 0xC29152: case 0xC291FE: case 0xC29202:
    case 0xC2922E: case 0xC29248: case 0xC29260: case 0xC2926C:
    case 0xC29284: case 0xC29290: case 0xC292A8: case 0xC292B4:
    case 0xC292C0: case 0xC292CC: case 0xC292E4: case 0xC29300:
    case 0xC2930A: case 0xC29322: case 0xC2934E: case 0xC2935A:
    case 0xC2950C: case 0xC29536:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC290FE:
        width=1; value=destination?destination:8; goto add_value;
    case 0xC29106: case 0xC29530:
        width=1; value=destination?destination:8; goto subtract_value;
    case 0xC29134: case 0xC2914C: case 0xC29218:
        SET_W(D(reg),(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC29136: case 0xC2913A: case 0xC2913C: case 0xC29160:
    case 0xC29162: case 0xC29164: case 0xC29398:
        step_add_word(&D(destination),cache_step_read(mode,reg,2)); break;
    case 0xC2913E:
        width=2; goto movem;
    case 0xC2915A: case 0xC2915C: case 0xC2915E:
        renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC29168:
        width=2; value=destination?destination:8; goto subtract_value;
    case 0xC2916A: case 0xC2916C: case 0xC2916E: case 0xC2921A:
        renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC29196: case 0xC2919E: case 0xC291C8: case 0xC291D4:
    case 0xC291DC: case 0xC2933C: case 0xC29400: case 0xC2940A:
    case 0xC2941A: case 0xC29434: case 0xC2944A: case 0xC2945A:
    case 0xC2946A: case 0xC2947A: case 0xC29482: case 0xC29490:
    case 0xC2949A: case 0xC294A2: case 0xC294AC: case 0xC294D2:
    case 0xC294FA: case 0xC29502: case 0xC29574: case 0xC29598:
    case 0xC295A0: case 0xC295AE: case 0xC295C8:
        width=4; goto movem;
    case 0xC291A6: case 0xC29220: case 0xC29384:
        width=4; goto move;
    case 0xC291B2: case 0xC29226: case 0xC29240: case 0xC29258:
    case 0xC29264: case 0xC2927C: case 0xC29288: case 0xC292A0:
    case 0xC292AC: case 0xC292B8: case 0xC292C4: case 0xC292DC:
    case 0xC292F4: case 0xC29302: case 0xC2930E: case 0xC29346:
    case 0xC29352: case 0xC294B0: case 0xC294C6: case 0xC294C8:
    case 0xC294CC: case 0xC294CE: case 0xC294D6: case 0xC294D8:
    case 0xC294EE: case 0xC294F2: case 0xC2958A:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC291BA:
        width=2; value=m68ki_read_imm_16(); goto add_value;
    case 0xC291BE: case 0xC29568: case 0xC2956A: case 0xC2956C:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC291C0: case 0xC2956E: case 0xC29570: case 0xC29572:
        step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC291C2: case 0xC291FC: case 0xC29200: case 0xC29208:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC291E4: case 0xC291EC: case 0xC291F4: case 0xC293FE:
    case 0xC29584: case 0xC29586: case 0xC29588:
        step_subtract_long(&D(destination),cache_step_read(mode,reg,4)); break;
    case 0xC291EA: case 0xC291F2: case 0xC291FA: case 0xC295C2:
    case 0xC295C4: case 0xC295C6:
        renderer_negate(&D(reg),4); break;
    case 0xC2920A:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC29224:
        REG_PC=origin_address(pc,mode,reg,4); break;
    case 0xC29228: case 0xC29242: case 0xC2925A: case 0xC29266:
    case 0xC2927E: case 0xC2928A: case 0xC292A2: case 0xC292AE:
    case 0xC292BA: case 0xC292C6: case 0xC292DE: case 0xC29304:
    case 0xC29310: case 0xC2931C: case 0xC29348: case 0xC29354:
    case 0xC29506: case 0xC29548:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC2923A: case 0xC29540:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC29370:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC2938A: case 0xC293C0:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC29396:
        old=(uint16_t)D(reg); value=destination?destination:8; SET_W(D(reg),old<<value); flags_logic_w(D(reg)); FLAG_X=FLAG_C=((old>>(16-value))&1u)<<8; USE_CYCLES(value<<CYC_SHIFT); break;
    case 0xC293A8: case 0xC293B6:
        value=m68ki_read_imm_16(); address=origin_address(pc,mode,reg,1); FLAG_Z=m68k_read_memory_8(address)&(1u<<(value&7u)); break;
    case 0xC293FC: case 0xC2942E: case 0xC29430: case 0xC29432:
    case 0xC29444: case 0xC29446: case 0xC29448: case 0xC29454:
    case 0xC29456: case 0xC29458: case 0xC29464: case 0xC29466:
    case 0xC29468: case 0xC29474: case 0xC29476: case 0xC29478:
    case 0xC29550: case 0xC29552: case 0xC29554: case 0xC29556:
    case 0xC2958C: case 0xC2958E: case 0xC29590:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC29422: case 0xC29426: case 0xC2942A: case 0xC29438:
    case 0xC2943E: case 0xC2944E: case 0xC29450: case 0xC29452:
    case 0xC2945E: case 0xC29460: case 0xC29462: case 0xC2946E:
    case 0xC29470: case 0xC29472: case 0xC29592: case 0xC29594:
    case 0xC29596: case 0xC295A8: case 0xC295AA: case 0xC295AC:
        step_add_long(&D(destination),cache_step_read(mode,reg,4)); break;
    case 0xC29488:
        value=m68ki_read_imm_16(); m68ki_push_32(REG_PC); REG_PC=pc+2+(int16_t)value; break;
    case 0xC29538:
        width=2; value=m68ki_read_imm_16(); goto or_value;
    case 0xC2957E: case 0xC29580:
        D(destination)|=cache_step_read(mode,reg,4); cache_step_logic(D(destination),4); break;
    case 0xC295B6: case 0xC295BC:
        width=4; value=m68ki_read_imm_32(); goto and_value;
    default: return 0;
    }
    goto finish;
move:
    value=cache_step_read(mode,reg,width); mode=(opcode>>6)&7u;
    cache_step_write(mode,destination,width,value);
    if(mode!=1) cache_step_logic(value,width);
    goto finish;
movem:
    mask=m68ki_read_imm_16();
    if(mode==4) address=A(reg); else if(mode==3) address=A(reg);
    else address=origin_address(pc,mode,reg,width);
    if(opcode&0x0400u) renderer_load(address,mask,width,mode==3?(int)reg:-1);
    else renderer_store(address,mask,width,mode==4?(int)reg:-1);
    goto finish;
and_value:
    if(mode==0) { value&=D(reg); cache_step_write(mode,reg,width,value); }
    else {
        address=origin_address(pc,mode,reg,width); value&=cache_step_read_memory(address,width);
        cache_step_write_memory(address,value,width,mode==4);
    }
    cache_step_logic(value,width); goto finish;
or_value:
    address=origin_address(pc,mode,reg,width); value|=cache_step_read_memory(address,width);
    cache_step_write_memory(address,value,width,mode==4); cache_step_logic(value,width); goto finish;
add_value:
    if(width==1) renderer_add_byte(&D(reg),value); else step_add_word(&D(reg),value);
    goto finish;
subtract_value:
    if(mode==0) {
        if(width==1) step_subtract_byte(&D(reg),value); else step_subtract_word(&D(reg),value);
    } else {
        address=origin_address(pc,mode,reg,width); old=cache_step_read_memory(address,width);
        if(width==1) step_subtract_byte(&old,value); else step_subtract_word(&old,value);
        cache_step_write_memory(address,old,width,mode==4);
    }
finish:
    USE_CYCLES(CYC_INSTRUCTION[opcode]); return 1;
}
