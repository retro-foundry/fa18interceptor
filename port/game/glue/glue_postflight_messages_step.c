/* Complete postflight messages family source CPU/bus/event boundaries.
 * Readable behavior lives in postflight_messages.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family postflight_messages. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_postflight_messages.h"

static int postflight_messages_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC0F4D8: case 0xC0F812: case 0xC110A4: case 0xC11350:
    case 0xC114D2: case 0xC115BA:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC0F4DC: case 0xC0F4E2: case 0xC0F4EE: case 0xC0F500:
    case 0xC0F508: case 0xC0F50E: case 0xC0F514: case 0xC0F52E:
    case 0xC0F542: case 0xC0F550: case 0xC0F556: case 0xC0F816:
    case 0xC0F914: case 0xC11108: case 0xC1117C: case 0xC112FC:
    case 0xC11302: case 0xC1136E: case 0xC11416: case 0xC11470:
    case 0xC1149E: case 0xC114A4: case 0xC114C0: case 0xC114D6:
    case 0xC114EE: case 0xC115A8: case 0xC1167E: case 0xC11686:
    case 0xC116CE: case 0xC11742:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC0F4E8: case 0xC113EE: case 0xC11450: case 0xC11492:
    case 0xC11526: case 0xC115DE:
        address=cache_step_address(mode,reg,4); m68ki_push_32(address); break;
    case 0xC0F4F4: case 0xC0F506: case 0xC0F548: case 0xC0F844:
    case 0xC0F8EC: case 0xC0F91A: case 0xC11116: case 0xC11128:
    case 0xC11138: case 0xC11162: case 0xC1118A: case 0xC11190:
    case 0xC111B2: case 0xC11208: case 0xC11216: case 0xC11224:
    case 0xC1123A: case 0xC11248: case 0xC1125E: case 0xC11274:
    case 0xC11282: case 0xC11296: case 0xC1129C: case 0xC112A2:
    case 0xC112D0: case 0xC112F0: case 0xC112F6: case 0xC113F8:
    case 0xC1145A: case 0xC1149C: case 0xC11530: case 0xC1154A:
    case 0xC11562: case 0xC1157A: case 0xC11586: case 0xC115E8:
    case 0xC11664: case 0xC11668:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0F4F6: case 0xC0F538: case 0xC0F854: case 0xC0F886:
    case 0xC0F8A2: case 0xC0F8C2: case 0xC0F8EE: case 0xC0F910:
    case 0xC11082: case 0xC110FA: case 0xC11100: case 0xC115EA:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC0F4F8: case 0xC0F4FA: case 0xC0F4FC: case 0xC0F51A:
    case 0xC0F520: case 0xC0F522: case 0xC0F524: case 0xC0F52A:
    case 0xC0F52C: case 0xC0F53A: case 0xC0F53C: case 0xC0F560:
    case 0xC0F81C: case 0xC0F86A: case 0xC0F87A: case 0xC0F88E:
    case 0xC0F89E: case 0xC0F8BE: case 0xC0F8DE: case 0xC0F8F0:
    case 0xC0F8F2: case 0xC0F8F6: case 0xC0F8F8: case 0xC0F904:
    case 0xC0F912: case 0xC11096: case 0xC110BC: case 0xC110CE:
    case 0xC11102: case 0xC11106: case 0xC11118: case 0xC111A2:
    case 0xC11250: case 0xC112A4: case 0xC112F8: case 0xC1136A:
    case 0xC1140E: case 0xC1143E: case 0xC11468: case 0xC11480:
    case 0xC114CA: case 0xC11500: case 0xC1151E: case 0xC1153E:
    case 0xC11594: case 0xC115B2: case 0xC11614: case 0xC1162A:
    case 0xC11640: case 0xC11644: case 0xC11676: case 0xC1168C:
    case 0xC116A8: case 0xC116C6: case 0xC1170E: case 0xC11716:
    case 0xC11750:
        width=4; goto move;
    case 0xC0F534: case 0xC0F55C: case 0xC0F874: case 0xC0F900:
    case 0xC11092: case 0xC110CA: case 0xC1110E: case 0xC1140A:
    case 0xC1143A: case 0xC11464: case 0xC114C6: case 0xC114FC:
    case 0xC1151A: case 0xC11532: case 0xC11590: case 0xC115AE:
    case 0xC11610: case 0xC11624: case 0xC11672: case 0xC116A4:
    case 0xC116C2: case 0xC1170A:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC0F54A: case 0xC0F84E: case 0xC110C4: case 0xC114BA:
    case 0xC11514: case 0xC116F4:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC0F566: case 0xC0F91C: case 0xC1130E: case 0xC113E0:
    case 0xC1159A: case 0xC11696:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC0F568: case 0xC0F91E: case 0xC110A2: case 0xC11310:
    case 0xC113E2: case 0xC1141C: case 0xC11444: case 0xC11476:
    case 0xC114D0: case 0xC1159C: case 0xC115B8: case 0xC11698:
    case 0xC116AE: case 0xC116CC: case 0xC11736: case 0xC1175A:
        REG_PC=m68ki_pull_32(); break;
    case 0xC0F824: case 0xC1130C: case 0xC1158E: case 0xC1163C:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC0F828: case 0xC0F842: case 0xC0F862: case 0xC0F888:
    case 0xC0F8A4: case 0xC0F8C4: case 0xC11078: case 0xC1108A:
    case 0xC110A8: case 0xC110D4: case 0xC110EA: case 0xC110FC:
    case 0xC11124: case 0xC11134: case 0xC1115E: case 0xC11178:
    case 0xC11186: case 0xC1118C: case 0xC111AE: case 0xC111F4:
    case 0xC11204: case 0xC11212: case 0xC11220: case 0xC11236:
    case 0xC11244: case 0xC1124A: case 0xC1125A: case 0xC11270:
    case 0xC1127E: case 0xC11298: case 0xC1129E: case 0xC112CC:
    case 0xC112EC: case 0xC112F2: case 0xC11396: case 0xC113A2:
    case 0xC113E4: case 0xC113FA: case 0xC11402: case 0xC11428:
    case 0xC11432: case 0xC11446: case 0xC1145C: case 0xC114B2:
    case 0xC114DC: case 0xC1150A: case 0xC11538: case 0xC11546:
    case 0xC11550: case 0xC1155E: case 0xC11568: case 0xC11576:
    case 0xC11582: case 0xC115BE: case 0xC115CA: case 0xC115FE:
    case 0xC11608: case 0xC1161C: case 0xC116D4: case 0xC116E2:
    case 0xC116EC: case 0xC11726: case 0xC11732: case 0xC11748:
        width=2; goto move;
    case 0xC0F82C: case 0xC110EE: case 0xC110F4: case 0xC1111C:
    case 0xC11602: case 0xC11648: case 0xC116E6:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC0F830: case 0xC111E2: case 0xC1164E:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC0F832: case 0xC11148: case 0xC111D0: case 0xC112BE:
    case 0xC1135C:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC0F834: case 0xC111E6:
        step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC0F836: case 0xC0F83E: case 0xC0F8B2: case 0xC0F8D2:
    case 0xC0F8E8: case 0xC11112: case 0xC11130: case 0xC1115A:
    case 0xC11172: case 0xC11182: case 0xC111AA: case 0xC11200:
    case 0xC1120E: case 0xC1121C: case 0xC11232: case 0xC11240:
    case 0xC1126C: case 0xC1127A: case 0xC11292: case 0xC112C8:
    case 0xC112E8: case 0xC11308: case 0xC1135E: case 0xC11374:
    case 0xC11382: case 0xC11386: case 0xC11390: case 0xC1139C:
    case 0xC113C2: case 0xC1155A: case 0xC11572: case 0xC1157E:
    case 0xC1158A: case 0xC115F8: case 0xC11632: case 0xC11650:
    case 0xC1165A: case 0xC1165E: case 0xC116DC: case 0xC11720:
    case 0xC1172C:
        width=4; goto move;
    case 0xC0F838: case 0xC11368:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC0F848: case 0xC1139A: case 0xC1166C: case 0xC1172A:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC0F84C: case 0xC0F90E: case 0xC1112C: case 0xC1113C:
    case 0xC1116E: case 0xC111A6: case 0xC111B6: case 0xC111EC:
    case 0xC111EE: case 0xC111F0: case 0xC111F2: case 0xC1120C:
    case 0xC1121A: case 0xC11230: case 0xC1123E: case 0xC1126A:
    case 0xC11278: case 0xC112A8: case 0xC11414: case 0xC1146E:
    case 0xC11506: case 0xC11524: case 0xC1154E: case 0xC11566:
    case 0xC1161A: case 0xC11670: case 0xC1167C: case 0xC11684:
    case 0xC11714:
        step_branch(pc,opcode,1); break;
    case 0xC0F856: case 0xC0F85C: case 0xC0F89A: case 0xC0F8B6:
    case 0xC0F8BC: case 0xC0F8D6: case 0xC0F8DC: case 0xC11084:
    case 0xC1109C: case 0xC110B4: case 0xC110DC: case 0xC11140:
    case 0xC11166: case 0xC11192: case 0xC1119A: case 0xC111BA:
    case 0xC111C8: case 0xC11228: case 0xC11262: case 0xC11286:
    case 0xC112AA: case 0xC112B6: case 0xC112D4: case 0xC112DC:
    case 0xC11354: case 0xC1137A: case 0xC1138C: case 0xC113A6:
    case 0xC113AE: case 0xC113BA: case 0xC113C6: case 0xC113CA:
    case 0xC113D2: case 0xC113D6: case 0xC1141E: case 0xC1148A:
    case 0xC114AA: case 0xC114E4: case 0xC114F4: case 0xC1159E:
    case 0xC115D6: case 0xC115EC: case 0xC115F2: case 0xC11654:
    case 0xC11662: case 0xC1169A: case 0xC116B0: case 0xC116FA:
    case 0xC11702: case 0xC11738:
        width=1; goto move;
    case 0xC0F87E: case 0xC11364: case 0xC1162E: case 0xC11638:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC0F882: case 0xC11104:
        cache_step_write(mode,reg,4,0); cache_step_logic(0,4); break;
    case 0xC0F892: case 0xC0F8AA: case 0xC0F8CA: case 0xC1114A:
    case 0xC11152: case 0xC111DC: case 0xC112C0:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC0F898: case 0xC0F8B0: case 0xC0F8D0: case 0xC0F8E6:
    case 0xC11150: case 0xC111FE: case 0xC11258: case 0xC11290:
    case 0xC1147E: case 0xC11658: case 0xC116C0:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC0F8B8: case 0xC0F8D8:
        width=1; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC0F8E2:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC0F8FC: case 0xC110D8: case 0xC113F4: case 0xC11456:
    case 0xC11498: case 0xC1152C: case 0xC115E4:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC1107E: case 0xC110AE: case 0xC113EA: case 0xC1142E:
    case 0xC1144C: case 0xC11510: case 0xC115C4: case 0xC115D0:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC11080: case 0xC110B0: case 0xC113EC: case 0xC11426:
    case 0xC11430: case 0xC1144E: case 0xC115D2: case 0xC116A2:
    case 0xC11740:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC110BA: case 0xC11146: case 0xC111CE: case 0xC112BC:
    case 0xC1135A:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC110E2: case 0xC111C0: case 0xC1128C: case 0xC112B0:
    case 0xC112E2: case 0xC113B4: case 0xC113CC:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC110E6: case 0xC11122: case 0xC11158: case 0xC111C4:
    case 0xC112B4: case 0xC112C6: case 0xC112E6: case 0xC114EC:
    case 0xC11512: case 0xC11544: case 0xC11558: case 0xC11570:
    case 0xC115A6: case 0xC115C6:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC110F2:
        step_branch(pc,opcode,COND_CS()); break;
    case 0xC110F8: case 0xC11606: case 0xC116EA:
        step_branch(pc,opcode,COND_HI()); break;
    case 0xC111D2:
        width=4; value=m68ki_read_imm_32(); operation='-'; goto arithmetic;
    case 0xC111D8:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC111E8:
        REG_PC=cache_step_address(mode,reg,4); break;
    case 0xC111FA: case 0xC11254:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC113AC: case 0xC113C8:
        width=1; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC113B8:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC113D0:
        step_branch(pc,opcode,COND_LS()); break;
    case 0xC11424: case 0xC11478: case 0xC114EA: case 0xC115A4:
    case 0xC11656: case 0xC116A0: case 0xC116B6: case 0xC116BA:
    case 0xC1173E:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC11542: case 0xC11556: case 0xC1156E:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC116B8:
        step_branch(pc,opcode,COND_MI()); break;
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
static const uint32_t owned_C0F4D8[]={
    0xC0F4D8,0xC0F4DC,0xC0F4E2,0xC0F4E8,0xC0F4EE,0xC0F4F4,0xC0F4F6,0xC0F4F8,
    0xC0F4FA,0xC0F4FC,0xC0F500,0xC0F506,0xC0F508,0xC0F50E,0xC0F514,0xC0F51A,
    0xC0F520,0xC0F522,0xC0F524,0xC0F52A,0xC0F52C,0xC0F52E,0xC0F534,0xC0F538,
    0xC0F53A,0xC0F53C,0xC0F542,0xC0F548,0xC0F54A,0xC0F550,0xC0F556,0xC0F55C,
    0xC0F560,0xC0F566,0xC0F568,
};
int glue_C0F4D8_owns(uint32_t pc) { return owns_pc(owned_C0F4D8,sizeof owned_C0F4D8/sizeof owned_C0F4D8[0],pc); }
int glue_C0F4D8_step(void) { if(!glue_C0F4D8_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C0F812[]={
    0xC0F812,0xC0F816,0xC0F81C,0xC0F824,0xC0F828,0xC0F82C,0xC0F830,0xC0F832,
    0xC0F834,0xC0F836,0xC0F838,0xC0F83E,0xC0F842,0xC0F844,0xC0F848,0xC0F84C,
    0xC0F84E,0xC0F854,0xC0F856,0xC0F85C,0xC0F862,0xC0F86A,0xC0F874,0xC0F87A,
    0xC0F87E,0xC0F882,0xC0F886,0xC0F888,0xC0F88E,0xC0F892,0xC0F898,0xC0F89A,
    0xC0F89E,0xC0F8A2,0xC0F8A4,0xC0F8AA,0xC0F8B0,0xC0F8B2,0xC0F8B6,0xC0F8B8,
    0xC0F8BC,0xC0F8BE,0xC0F8C2,0xC0F8C4,0xC0F8CA,0xC0F8D0,0xC0F8D2,0xC0F8D6,
    0xC0F8D8,0xC0F8DC,0xC0F8DE,0xC0F8E2,0xC0F8E6,0xC0F8E8,0xC0F8EC,0xC0F8EE,
    0xC0F8F0,0xC0F8F2,0xC0F8F6,0xC0F8F8,0xC0F8FC,0xC0F900,0xC0F904,0xC0F90E,
    0xC0F910,0xC0F912,0xC0F914,0xC0F91A,0xC0F91C,0xC0F91E,
};
int glue_C0F812_owns(uint32_t pc) { return owns_pc(owned_C0F812,sizeof owned_C0F812/sizeof owned_C0F812[0],pc); }
int glue_C0F812_step(void) { if(!glue_C0F812_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C11078[]={
    0xC11078,0xC1107E,0xC11080,0xC11082,0xC11084,0xC1108A,0xC11092,0xC11096,
    0xC1109C,0xC110A2,
};
int glue_C11078_owns(uint32_t pc) { return owns_pc(owned_C11078,sizeof owned_C11078/sizeof owned_C11078[0],pc); }
int glue_C11078_step(void) { if(!glue_C11078_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C110A4[]={
    0xC110A4,0xC110A8,0xC110AE,0xC110B0,0xC110B4,0xC110BA,0xC110BC,0xC110C4,
    0xC110CA,0xC110CE,0xC110D4,0xC110D8,0xC110DC,0xC110E2,0xC110E6,0xC110EA,
    0xC110EE,0xC110F2,0xC110F4,0xC110F8,0xC110FA,0xC110FC,0xC11100,0xC11102,
    0xC11104,0xC11106,0xC11108,0xC1110E,0xC11112,0xC11116,0xC11118,0xC1111C,
    0xC11122,0xC11124,0xC11128,0xC1112C,0xC11130,0xC11134,0xC11138,0xC1113C,
    0xC11140,0xC11146,0xC11148,0xC1114A,0xC11150,0xC11152,0xC11158,0xC1115A,
    0xC1115E,0xC11162,0xC11166,0xC1116E,0xC11172,0xC11178,0xC1117C,0xC11182,
    0xC11186,0xC1118A,0xC1118C,0xC11190,0xC11192,0xC1119A,0xC111A2,0xC111A6,
    0xC111AA,0xC111AE,0xC111B2,0xC111B6,0xC111BA,0xC111C0,0xC111C4,0xC111C8,
    0xC111CE,0xC111D0,0xC111D2,0xC111D8,0xC111DC,0xC111E2,0xC111E6,0xC111E8,
    0xC111EC,0xC111EE,0xC111F0,0xC111F2,0xC111F4,0xC111FA,0xC111FE,0xC11200,
    0xC11204,0xC11208,0xC1120C,0xC1120E,0xC11212,0xC11216,0xC1121A,0xC1121C,
    0xC11220,0xC11224,0xC11228,0xC11230,0xC11232,0xC11236,0xC1123A,0xC1123E,
    0xC11240,0xC11244,0xC11248,0xC1124A,0xC11250,0xC11254,0xC11258,0xC1125A,
    0xC1125E,0xC11262,0xC1126A,0xC1126C,0xC11270,0xC11274,0xC11278,0xC1127A,
    0xC1127E,0xC11282,0xC11286,0xC1128C,0xC11290,0xC11292,0xC11296,0xC11298,
    0xC1129C,0xC1129E,0xC112A2,0xC112A4,0xC112A8,0xC112AA,0xC112B0,0xC112B4,
    0xC112B6,0xC112BC,0xC112BE,0xC112C0,0xC112C6,0xC112C8,0xC112CC,0xC112D0,
    0xC112D4,0xC112DC,0xC112E2,0xC112E6,0xC112E8,0xC112EC,0xC112F0,0xC112F2,
    0xC112F6,0xC112F8,0xC112FC,0xC11302,0xC11308,0xC1130C,0xC1130E,0xC11310,
};
int glue_C110A4_owns(uint32_t pc) { return owns_pc(owned_C110A4,sizeof owned_C110A4/sizeof owned_C110A4[0],pc); }
int glue_C110A4_step(void) { if(!glue_C110A4_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C11350[]={
    0xC11350,0xC11354,0xC1135A,0xC1135C,0xC1135E,0xC11364,0xC11368,0xC1136A,
    0xC1136E,0xC11374,0xC1137A,0xC11382,0xC11386,0xC1138C,0xC11390,0xC11396,
    0xC1139A,0xC1139C,0xC113A2,0xC113A6,0xC113AC,0xC113AE,0xC113B4,0xC113B8,
    0xC113BA,0xC113C2,0xC113C6,0xC113C8,0xC113CA,0xC113CC,0xC113D0,0xC113D2,
    0xC113D6,0xC113E0,0xC113E2,
};
int glue_C11350_owns(uint32_t pc) { return owns_pc(owned_C11350,sizeof owned_C11350/sizeof owned_C11350[0],pc); }
int glue_C11350_step(void) { if(!glue_C11350_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C113E4[]={
    0xC113E4,0xC113EA,0xC113EC,0xC113EE,0xC113F4,0xC113F8,0xC113FA,0xC11402,
    0xC1140A,0xC1140E,0xC11414,0xC11416,0xC1141C,
};
int glue_C113E4_owns(uint32_t pc) { return owns_pc(owned_C113E4,sizeof owned_C113E4/sizeof owned_C113E4[0],pc); }
int glue_C113E4_step(void) { if(!glue_C113E4_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C1141E[]={
    0xC1141E,0xC11424,0xC11426,0xC11428,0xC1142E,0xC11430,0xC11432,0xC1143A,
    0xC1143E,0xC11444,
};
int glue_C1141E_owns(uint32_t pc) { return owns_pc(owned_C1141E,sizeof owned_C1141E/sizeof owned_C1141E[0],pc); }
int glue_C1141E_step(void) { if(!glue_C1141E_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C11446[]={
    0xC11446,0xC1144C,0xC1144E,0xC11450,0xC11456,0xC1145A,0xC1145C,0xC11464,
    0xC11468,0xC1146E,0xC11470,0xC11476,
};
int glue_C11446_owns(uint32_t pc) { return owns_pc(owned_C11446,sizeof owned_C11446/sizeof owned_C11446[0],pc); }
int glue_C11446_step(void) { if(!glue_C11446_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C11478[]={
    0xC11478,0xC1147E,0xC11480,0xC1148A,0xC11492,0xC11498,0xC1149C,0xC1149E,
    0xC114A4,0xC114AA,0xC114B2,0xC114BA,0xC114C0,0xC114C6,0xC114CA,0xC114D0,
};
int glue_C11478_owns(uint32_t pc) { return owns_pc(owned_C11478,sizeof owned_C11478/sizeof owned_C11478[0],pc); }
int glue_C11478_step(void) { if(!glue_C11478_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C114D2[]={
    0xC114D2,0xC114D6,0xC114DC,0xC114E4,0xC114EA,0xC114EC,0xC114EE,0xC114F4,
    0xC114FC,0xC11500,0xC11506,0xC1150A,0xC11510,0xC11512,0xC11514,0xC1151A,
    0xC1151E,0xC11524,0xC11526,0xC1152C,0xC11530,0xC11532,0xC11538,0xC1153E,
    0xC11542,0xC11544,0xC11546,0xC1154A,0xC1154E,0xC11550,0xC11556,0xC11558,
    0xC1155A,0xC1155E,0xC11562,0xC11566,0xC11568,0xC1156E,0xC11570,0xC11572,
    0xC11576,0xC1157A,0xC1157E,0xC11582,0xC11586,0xC1158A,0xC1158E,0xC11590,
    0xC11594,0xC1159A,0xC1159C,
};
int glue_C114D2_owns(uint32_t pc) { return owns_pc(owned_C114D2,sizeof owned_C114D2/sizeof owned_C114D2[0],pc); }
int glue_C114D2_step(void) { if(!glue_C114D2_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C1159E[]={
    0xC1159E,0xC115A4,0xC115A6,0xC115A8,0xC115AE,0xC115B2,0xC115B8,
};
int glue_C1159E_owns(uint32_t pc) { return owns_pc(owned_C1159E,sizeof owned_C1159E/sizeof owned_C1159E[0],pc); }
int glue_C1159E_step(void) { if(!glue_C1159E_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C115BA[]={
    0xC115BA,0xC115BE,0xC115C4,0xC115C6,0xC115CA,0xC115D0,0xC115D2,0xC115D6,
    0xC115DE,0xC115E4,0xC115E8,0xC115EA,0xC115EC,0xC115F2,0xC115F8,0xC115FE,
    0xC11602,0xC11606,0xC11608,0xC11610,0xC11614,0xC1161A,0xC1161C,0xC11624,
    0xC1162A,0xC1162E,0xC11632,0xC11638,0xC1163C,0xC11640,0xC11644,0xC11648,
    0xC1164E,0xC11650,0xC11654,0xC11656,0xC11658,0xC1165A,0xC1165E,0xC11662,
    0xC11664,0xC11668,0xC1166C,0xC11670,0xC11672,0xC11676,0xC1167C,0xC1167E,
    0xC11684,0xC11686,0xC1168C,0xC11696,0xC11698,
};
int glue_C115BA_owns(uint32_t pc) { return owns_pc(owned_C115BA,sizeof owned_C115BA/sizeof owned_C115BA[0],pc); }
int glue_C115BA_step(void) { if(!glue_C115BA_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C1169A[]={
    0xC1169A,0xC116A0,0xC116A2,0xC116A4,0xC116A8,0xC116AE,
};
int glue_C1169A_owns(uint32_t pc) { return owns_pc(owned_C1169A,sizeof owned_C1169A/sizeof owned_C1169A[0],pc); }
int glue_C1169A_step(void) { if(!glue_C1169A_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C116B0[]={
    0xC116B0,0xC116B6,0xC116B8,0xC116BA,0xC116C0,0xC116C2,0xC116C6,0xC116CC,
};
int glue_C116B0_owns(uint32_t pc) { return owns_pc(owned_C116B0,sizeof owned_C116B0/sizeof owned_C116B0[0],pc); }
int glue_C116B0_step(void) { if(!glue_C116B0_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C116CE[]={
    0xC116CE,0xC116D4,0xC116DC,0xC116E2,0xC116E6,0xC116EA,0xC116EC,0xC116F4,
    0xC116FA,0xC11702,0xC1170A,0xC1170E,0xC11714,0xC11716,0xC11720,0xC11726,
    0xC1172A,0xC1172C,0xC11732,0xC11736,
};
int glue_C116CE_owns(uint32_t pc) { return owns_pc(owned_C116CE,sizeof owned_C116CE/sizeof owned_C116CE[0],pc); }
int glue_C116CE_step(void) { if(!glue_C116CE_owns(REG_PC)) return 0; return postflight_messages_step(); }
static const uint32_t owned_C11738[]={
    0xC11738,0xC1173E,0xC11740,0xC11742,0xC11748,0xC11750,0xC1175A,
};
int glue_C11738_owns(uint32_t pc) { return owns_pc(owned_C11738,sizeof owned_C11738/sizeof owned_C11738[0],pc); }
int glue_C11738_step(void) { if(!glue_C11738_owns(REG_PC)) return 0; return postflight_messages_step(); }
