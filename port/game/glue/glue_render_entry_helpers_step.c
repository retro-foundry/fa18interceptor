/* Complete render entry helpers family source CPU/bus/event boundaries.
 * Readable behavior lives in render_leaf_helpers.c and planar_lane_masks.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family render_entry_helpers. */
#include "glue_render_leaf_helpers_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_render_entry_helpers.h"

static int render_entry_helpers_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC2F622: case 0xC2F732: case 0xC2F740: case 0xC2F74E:
    case 0xC2F75C: case 0xC30258: case 0xC302C0: case 0xC302DA:
    case 0xC30390: case 0xC30392: case 0xC303CA: case 0xC30408:
    case 0xC33120: case 0xC3313E:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC2F624: case 0xC2F66C: case 0xC2F762: case 0xC2F82E:
    case 0xC2F842: case 0xC2F84C: case 0xC2F856: case 0xC2F860:
    case 0xC2F86A: case 0xC2F874: case 0xC2F87E: case 0xC2F888:
    case 0xC2F892: case 0xC2F89C: case 0xC2F8A6: case 0xC2F8B0:
    case 0xC2F8BA: case 0xC2F8C4: case 0xC2F8CE: case 0xC2F8E8:
    case 0xC2F902: case 0xC2F91C: case 0xC2F936: case 0xC2F950:
    case 0xC2F96A: case 0xC2F984: case 0xC2F99E: case 0xC2F9B8:
    case 0xC2F9D2: case 0xC2F9EC: case 0xC2FA06: case 0xC2FA20:
    case 0xC2FA3A: case 0xC2FA54: case 0xC2FA6E: case 0xC3025A:
    case 0xC302C2: case 0xC302DC: case 0xC3040A: case 0xC33168:
        REG_PC=m68ki_pull_32(); break;
    case 0xC2F63A: case 0xC2F648: case 0xC2F696: case 0xC2F698:
    case 0xC2F6A4: case 0xC2F6B2: case 0xC2F6B4: case 0xC2F6B6:
    case 0xC2F6BE: case 0xC30354: case 0xC3036A: case 0xC3036C:
    case 0xC3036E: case 0xC30370: case 0xC30384: case 0xC30386:
    case 0xC30388: case 0xC303C2:
        width=2; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC2F640: case 0xC2F71E: case 0xC30234:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC2F642: case 0xC30268: case 0xC3026E: case 0xC3027C:
    case 0xC30298: case 0xC302E6:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC2F646: case 0xC3020C: case 0xC30212: case 0xC30220:
    case 0xC30226: case 0xC30244: case 0xC30252: case 0xC30264:
    case 0xC30278: case 0xC30294: case 0xC302CC: case 0xC302E2:
    case 0xC302EE:
        step_branch(pc,opcode,COND_GE()); break;
    case 0xC2F64E: case 0xC2F650: case 0xC2F668: case 0xC2F66A:
    case 0xC2F68C: case 0xC2F69E: case 0xC2F6A6: case 0xC2F6AA:
    case 0xC2F6B0: case 0xC2F6CC: case 0xC2F6CE: case 0xC2F6D0:
    case 0xC2F6D2: case 0xC2F6D4: case 0xC2F6D6: case 0xC2F6E2:
    case 0xC2F6F2: case 0xC2F702: case 0xC2F712: case 0xC30202:
    case 0xC30214: case 0xC3021C: case 0xC30228: case 0xC30230:
    case 0xC30236: case 0xC30238: case 0xC3023E: case 0xC30246:
    case 0xC3024C: case 0xC30254: case 0xC30260: case 0xC30274:
    case 0xC30288: case 0xC30290: case 0xC302D4: case 0xC302DE:
    case 0xC302FC: case 0xC30312: case 0xC30322: case 0xC3032E:
    case 0xC30330: case 0xC3033A: case 0xC3034C: case 0xC30352:
    case 0xC3035C: case 0xC30362: case 0xC30364: case 0xC30368:
    case 0xC3037E: case 0xC30382: case 0xC303BA: case 0xC303C4:
    case 0xC303E0: case 0xC303E6: case 0xC303F8: case 0xC303FC:
    case 0xC30400: case 0xC30404: case 0xC33104: case 0xC33108:
    case 0xC33112: case 0xC33160:
        width=2; goto move;
    case 0xC2F652: case 0xC2F69A: case 0xC3310A: case 0xC3310C:
        width=4; goto move;
    case 0xC2F658: case 0xC2F65E: case 0xC301FC: case 0xC30306:
    case 0xC3030C: case 0xC30316: case 0xC303CC:
        A(destination)=(mode==7 && reg==0)?(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16():cache_step_address(mode,reg,4); break;
    case 0xC2F664: case 0xC3028A: case 0xC302B6: case 0xC302CE:
    case 0xC302D6: case 0xC30324: case 0xC3033C:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC2F688: case 0xC2F718:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC2F68A: case 0xC3021A: case 0xC3022E: case 0xC30272:
    case 0xC3029C: case 0xC302EA: case 0xC3037A:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC2F692: case 0xC2F6A0: case 0xC2F6B8: case 0xC33116:
    case 0xC3311A:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC2F6AC:
        value=~cache_step_read(mode,reg,2); cache_step_write(mode,reg,2,value);cache_step_logic(value,2);break;
    case 0xC2F6AE: case 0xC30366: case 0xC30380: case 0xC33124:
    case 0xC33142:
        if((opcode&0xc0u)!=0xc0u) { renderer_asl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asl_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC2F6BC: case 0xC3034A: case 0xC3034E:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC2F6C0:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC2F6C4: case 0xC2F6C6: case 0xC2F6C8: case 0xC2F6CA:
    case 0xC33134: case 0xC33156:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC2F6D8: case 0xC2F6E8: case 0xC2F6F8: case 0xC2F708:
    case 0xC2F726: case 0xC2F734: case 0xC2F742: case 0xC2F750:
    case 0xC303D2:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC2F6E0: case 0xC2F6F0: case 0xC2F700: case 0xC2F710:
    case 0xC302AA: case 0xC3311E:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC2F6E6: case 0xC2F6F6: case 0xC2F706: case 0xC2F716:
    case 0xC302F0:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC2F720:
        width=4; value=m68ki_read_imm_32(); operation='&'; goto immediate_logic;
    case 0xC2F72E: case 0xC2F73C: case 0xC2F74A: case 0xC2F758:
    case 0xC303D8:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC2F730: case 0xC2F73E: case 0xC2F74C: case 0xC2F75A:
        width=2; value=D(destination);operation='^';goto immediate_logic;
    case 0xC2F75E:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC2F760:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC2F764:
        REG_PC=cache_step_address(mode,reg,4); break;
    case 0xC2F826: case 0xC2F828: case 0xC2F82A: case 0xC2F82C:
    case 0xC2F83C: case 0xC2F83E: case 0xC2F840: case 0xC2F844:
    case 0xC2F848: case 0xC2F84A: case 0xC2F852: case 0xC2F854:
    case 0xC2F858: case 0xC2F85A: case 0xC2F85E: case 0xC2F864:
    case 0xC2F868: case 0xC2F86C: case 0xC2F872: case 0xC2F87C:
    case 0xC2F880: case 0xC2F882: case 0xC2F884: case 0xC2F88C:
    case 0xC2F88E: case 0xC2F894: case 0xC2F898: case 0xC2F8A2:
    case 0xC2F8A8: case 0xC2F8AA: case 0xC2F8B4: case 0xC2F8BC:
    case 0xC2F8D0: case 0xC2F8D2: case 0xC2F8D6: case 0xC2F8D8:
    case 0xC2F8DC: case 0xC2F8DE: case 0xC2F8E2: case 0xC2F8E4:
    case 0xC2F8F0: case 0xC2F8F2: case 0xC2F8F6: case 0xC2F8F8:
    case 0xC2F8FC: case 0xC2F8FE: case 0xC2F904: case 0xC2F906:
    case 0xC2F910: case 0xC2F912: case 0xC2F916: case 0xC2F918:
    case 0xC2F92A: case 0xC2F92C: case 0xC2F930: case 0xC2F932:
    case 0xC2F938: case 0xC2F93A: case 0xC2F93E: case 0xC2F940:
    case 0xC2F94A: case 0xC2F94C: case 0xC2F958: case 0xC2F95A:
    case 0xC2F964: case 0xC2F966: case 0xC2F96C: case 0xC2F96E:
    case 0xC2F97E: case 0xC2F980: case 0xC2F998: case 0xC2F99A:
    case 0xC2F9A0: case 0xC2F9A2: case 0xC2F9A6: case 0xC2F9A8:
    case 0xC2F9AC: case 0xC2F9AE: case 0xC2F9C0: case 0xC2F9C2:
    case 0xC2F9C6: case 0xC2F9C8: case 0xC2F9D4: case 0xC2F9D6:
    case 0xC2F9E0: case 0xC2F9E2: case 0xC2F9FA: case 0xC2F9FC:
    case 0xC2FA08: case 0xC2FA0A: case 0xC2FA0E: case 0xC2FA10:
    case 0xC2FA28: case 0xC2FA2A: case 0xC2FA3C: case 0xC2FA3E:
        width=2; if(opcode&0x100u) { value=D(destination); operation='&'; goto immediate_logic; } value=cache_step_read(mode,reg,2)&D(destination); cache_step_write(0,destination,2,value); cache_step_logic(value,2); break;
    case 0xC2F83A: case 0xC2F846: case 0xC2F84E: case 0xC2F850:
    case 0xC2F85C: case 0xC2F862: case 0xC2F866: case 0xC2F86E:
    case 0xC2F870: case 0xC2F876: case 0xC2F878: case 0xC2F87A:
    case 0xC2F886: case 0xC2F88A: case 0xC2F890: case 0xC2F896:
    case 0xC2F89A: case 0xC2F89E: case 0xC2F8A0: case 0xC2F8A4:
    case 0xC2F8AC: case 0xC2F8AE: case 0xC2F8B2: case 0xC2F8B6:
    case 0xC2F8B8: case 0xC2F8BE: case 0xC2F8C0: case 0xC2F8C2:
    case 0xC2F8C6: case 0xC2F8C8: case 0xC2F8CA: case 0xC2F8CC:
    case 0xC2F8EA: case 0xC2F8EC: case 0xC2F90A: case 0xC2F90C:
    case 0xC2F91E: case 0xC2F920: case 0xC2F924: case 0xC2F926:
    case 0xC2F944: case 0xC2F946: case 0xC2F952: case 0xC2F954:
    case 0xC2F95E: case 0xC2F960: case 0xC2F972: case 0xC2F974:
    case 0xC2F978: case 0xC2F97A: case 0xC2F986: case 0xC2F988:
    case 0xC2F98C: case 0xC2F98E: case 0xC2F992: case 0xC2F994:
    case 0xC2F9B2: case 0xC2F9B4: case 0xC2F9BA: case 0xC2F9BC:
    case 0xC2F9CC: case 0xC2F9CE: case 0xC2F9DA: case 0xC2F9DC:
    case 0xC2F9E6: case 0xC2F9E8: case 0xC2F9EE: case 0xC2F9F0:
    case 0xC2F9F4: case 0xC2F9F6: case 0xC2FA00: case 0xC2FA02:
    case 0xC2FA14: case 0xC2FA16: case 0xC2FA1A: case 0xC2FA1C:
    case 0xC2FA22: case 0xC2FA24: case 0xC2FA2E: case 0xC2FA30:
    case 0xC2FA34: case 0xC2FA36: case 0xC2FA42: case 0xC2FA44:
    case 0xC2FA48: case 0xC2FA4A: case 0xC2FA4E: case 0xC2FA50:
    case 0xC2FA56: case 0xC2FA58: case 0xC2FA5C: case 0xC2FA5E:
    case 0xC2FA62: case 0xC2FA64: case 0xC2FA68: case 0xC2FA6A:
        width=2; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
    case 0xC301F6: case 0xC30328: case 0xC30340:
        width=2; goto move;
    case 0xC30204: case 0xC30232: case 0xC302CA: case 0xC302EC:
    case 0xC30314: case 0xC3032A: case 0xC33110:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC30206: case 0xC302F4: case 0xC3031C: case 0xC30332:
    case 0xC30342:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,2,(int)reg); else { address=cache_step_address(mode,reg,2); if(opcode&0x400u) renderer_load(address,mask,2,mode==3?(int)reg:-1); else renderer_store(address,mask,2,-1); } break;
    case 0xC3020A: case 0xC30210: case 0xC30218: case 0xC3021E:
    case 0xC30224: case 0xC3022C: case 0xC3023A: case 0xC30242:
    case 0xC30248: case 0xC30250: case 0xC3025C: case 0xC30284:
    case 0xC302C6: case 0xC30378:
        step_compare_word(cache_step_read(mode,reg,2),D(destination)); break;
    case 0xC3020E: case 0xC30222: case 0xC302F2:
        if((opcode&0xf8u)==0x48u) { value=A(destination); A(destination)=A(reg); A(reg)=value; } else if((opcode&0xf8u)==0x40u) { value=D(destination); D(destination)=D(reg); D(reg)=value; } else { value=D(destination); D(destination)=A(reg); A(reg)=value; } break;
    case 0xC30216: case 0xC3022A: case 0xC30240: case 0xC3024E:
    case 0xC30256: case 0xC3028E: case 0xC302D2: case 0xC3038E:
    case 0xC303DE: case 0xC3313C:
        step_branch(pc,opcode,1); break;
    case 0xC3023C: case 0xC3024A: case 0xC3025E: case 0xC3026C:
    case 0xC30280: case 0xC30286: case 0xC302C8: case 0xC3032C:
        step_branch(pc,opcode,COND_GT()); break;
    case 0xC30262: case 0xC30276: case 0xC30292: case 0xC302E0:
    case 0xC30350: case 0xC3037C: case 0xC303B2: case 0xC303BE:
        width=2; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC30266: case 0xC3027A: case 0xC30296: case 0xC302E4:
    case 0xC30356:
        if(mode==0) renderer_negate(&D(reg),2); else { address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); } break;
    case 0xC30282: case 0xC302C4: case 0xC30320: case 0xC303B8:
    case 0xC303BC: case 0xC33132: case 0xC33154:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC3029E: case 0xC302AC: case 0xC302BA: case 0xC30376:
    case 0xC30394: case 0xC3039A: case 0xC303A4: case 0xC303AA:
    case 0xC303AC: case 0xC303EC: case 0xC303F0: case 0xC303F4:
    case 0xC330FE: case 0xC33100: case 0xC33128: case 0xC33130:
    case 0xC33146: case 0xC33152: case 0xC33164: case 0xC33166:
        width=4; goto move;
    case 0xC302A4:
        cache_step_logic(cache_step_read(mode,reg,1),1); break;
    case 0xC30358:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC30372: case 0xC3038A:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC30374: case 0xC3039C:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC3038C: case 0xC303A2:
        width=4; if(opcode&0x100u) { value=D(destination); operation='-'; goto arithmetic; } value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC303C0:
        menu_lsl_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC303DA: case 0xC303DC:
        break;
    case 0xC33102: case 0xC33106: case 0xC33126: case 0xC33144:
    case 0xC3315E: case 0xC33162:
        step_swap(&D(reg)); break;
    case 0xC3310E:
        flight_lsr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC33114:
        render_leaf_rol_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC33122: case 0xC33140:
        width=1; goto move;
    case 0xC3312A: case 0xC33148:
        step_lsr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC3312C: case 0xC3314A: case 0xC3314E:
        value=~cache_step_read(mode,reg,4); cache_step_write(mode,reg,4,value);cache_step_logic(value,4);break;
    case 0xC3312E: case 0xC3314C:
        width=4; if(opcode&0x100u) { value=D(destination); operation='&'; goto immediate_logic; } value=cache_step_read(mode,reg,4)&D(destination); cache_step_write(0,destination,4,value); cache_step_logic(value,4); break;
    case 0xC33138: case 0xC3315A:
        step_dbf(pc,&D(reg)); break;
    case 0xC33150:
        width=4; if(opcode&0x100u) { value=D(destination); operation='|'; goto immediate_logic; } value=cache_step_read(mode,reg,width)|D(destination); cache_step_write(0,destination,width,value); cache_step_logic(value,width); break;
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
static const uint32_t owned_C2F688[]={
    0xC2F622,0xC2F624,0xC2F688,0xC2F68A,0xC2F68C,0xC2F692,0xC2F696,0xC2F698,
    0xC2F69A,0xC2F69E,0xC2F6A0,0xC2F6A4,0xC2F6A6,0xC2F6AA,0xC2F6AC,0xC2F6AE,
    0xC2F6B0,0xC2F6B2,0xC2F6B4,0xC2F6B6,0xC2F6B8,0xC2F6BC,0xC2F6BE,0xC2F6C0,
    0xC2F6C4,0xC2F6C6,0xC2F6C8,0xC2F6CA,0xC2F6CC,0xC2F6CE,0xC2F6D0,0xC2F6D2,
    0xC2F6D4,0xC2F6D6,0xC2F6D8,0xC2F6E0,0xC2F6E2,0xC2F6E6,0xC2F6E8,0xC2F6F0,
    0xC2F6F2,0xC2F6F6,0xC2F6F8,0xC2F700,0xC2F702,0xC2F706,0xC2F708,0xC2F710,
    0xC2F712,0xC2F716,0xC2F718,0xC2F71E,0xC2F720,0xC2F726,0xC2F72E,0xC2F730,
    0xC2F732,0xC2F734,0xC2F73C,0xC2F73E,0xC2F740,0xC2F742,0xC2F74A,0xC2F74C,
    0xC2F74E,0xC2F750,0xC2F758,0xC2F75A,0xC2F75C,0xC2F75E,0xC2F760,0xC2F762,
    0xC2F764,0xC2F826,0xC2F828,0xC2F82A,0xC2F82C,0xC2F82E,0xC2F83A,0xC2F83C,
    0xC2F83E,0xC2F840,0xC2F842,0xC2F844,0xC2F846,0xC2F848,0xC2F84A,0xC2F84C,
    0xC2F84E,0xC2F850,0xC2F852,0xC2F854,0xC2F856,0xC2F858,0xC2F85A,0xC2F85C,
    0xC2F85E,0xC2F860,0xC2F862,0xC2F864,0xC2F866,0xC2F868,0xC2F86A,0xC2F86C,
    0xC2F86E,0xC2F870,0xC2F872,0xC2F874,0xC2F876,0xC2F878,0xC2F87A,0xC2F87C,
    0xC2F87E,0xC2F880,0xC2F882,0xC2F884,0xC2F886,0xC2F888,0xC2F88A,0xC2F88C,
    0xC2F88E,0xC2F890,0xC2F892,0xC2F894,0xC2F896,0xC2F898,0xC2F89A,0xC2F89C,
    0xC2F89E,0xC2F8A0,0xC2F8A2,0xC2F8A4,0xC2F8A6,0xC2F8A8,0xC2F8AA,0xC2F8AC,
    0xC2F8AE,0xC2F8B0,0xC2F8B2,0xC2F8B4,0xC2F8B6,0xC2F8B8,0xC2F8BA,0xC2F8BC,
    0xC2F8BE,0xC2F8C0,0xC2F8C2,0xC2F8C4,0xC2F8C6,0xC2F8C8,0xC2F8CA,0xC2F8CC,
    0xC2F8CE,0xC2F8D0,0xC2F8D2,0xC2F8D6,0xC2F8D8,0xC2F8DC,0xC2F8DE,0xC2F8E2,
    0xC2F8E4,0xC2F8E8,0xC2F8EA,0xC2F8EC,0xC2F8F0,0xC2F8F2,0xC2F8F6,0xC2F8F8,
    0xC2F8FC,0xC2F8FE,0xC2F902,0xC2F904,0xC2F906,0xC2F90A,0xC2F90C,0xC2F910,
    0xC2F912,0xC2F916,0xC2F918,0xC2F91C,0xC2F91E,0xC2F920,0xC2F924,0xC2F926,
    0xC2F92A,0xC2F92C,0xC2F930,0xC2F932,0xC2F936,0xC2F938,0xC2F93A,0xC2F93E,
    0xC2F940,0xC2F944,0xC2F946,0xC2F94A,0xC2F94C,0xC2F950,0xC2F952,0xC2F954,
    0xC2F958,0xC2F95A,0xC2F95E,0xC2F960,0xC2F964,0xC2F966,0xC2F96A,0xC2F96C,
    0xC2F96E,0xC2F972,0xC2F974,0xC2F978,0xC2F97A,0xC2F97E,0xC2F980,0xC2F984,
    0xC2F986,0xC2F988,0xC2F98C,0xC2F98E,0xC2F992,0xC2F994,0xC2F998,0xC2F99A,
    0xC2F99E,0xC2F9A0,0xC2F9A2,0xC2F9A6,0xC2F9A8,0xC2F9AC,0xC2F9AE,0xC2F9B2,
    0xC2F9B4,0xC2F9B8,0xC2F9BA,0xC2F9BC,0xC2F9C0,0xC2F9C2,0xC2F9C6,0xC2F9C8,
    0xC2F9CC,0xC2F9CE,0xC2F9D2,0xC2F9D4,0xC2F9D6,0xC2F9DA,0xC2F9DC,0xC2F9E0,
    0xC2F9E2,0xC2F9E6,0xC2F9E8,0xC2F9EC,0xC2F9EE,0xC2F9F0,0xC2F9F4,0xC2F9F6,
    0xC2F9FA,0xC2F9FC,0xC2FA00,0xC2FA02,0xC2FA06,0xC2FA08,0xC2FA0A,0xC2FA0E,
    0xC2FA10,0xC2FA14,0xC2FA16,0xC2FA1A,0xC2FA1C,0xC2FA20,0xC2FA22,0xC2FA24,
    0xC2FA28,0xC2FA2A,0xC2FA2E,0xC2FA30,0xC2FA34,0xC2FA36,0xC2FA3A,0xC2FA3C,
    0xC2FA3E,0xC2FA42,0xC2FA44,0xC2FA48,0xC2FA4A,0xC2FA4E,0xC2FA50,0xC2FA54,
    0xC2FA56,0xC2FA58,0xC2FA5C,0xC2FA5E,0xC2FA62,0xC2FA64,0xC2FA68,0xC2FA6A,
    0xC2FA6E,
};
int glue_C2F688_owns(uint32_t pc) { return owns_pc(owned_C2F688,sizeof owned_C2F688/sizeof owned_C2F688[0],pc); }
int glue_C2F688_complete_step(void) { if(!glue_C2F688_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F63A[]={
    0xC2F622,0xC2F624,0xC2F63A,0xC2F640,0xC2F642,0xC2F646,0xC2F648,0xC2F64E,
    0xC2F650,0xC2F652,0xC2F658,0xC2F65E,0xC2F664,0xC2F668,0xC2F66A,0xC2F66C,
};
int glue_C2F63A_owns(uint32_t pc) { return owns_pc(owned_C2F63A,sizeof owned_C2F63A/sizeof owned_C2F63A[0],pc); }
int glue_C2F63A_complete_step(void) { if(!glue_C2F63A_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F64E[]={
    0xC2F64E,0xC2F650,0xC2F652,0xC2F658,0xC2F65E,0xC2F664,0xC2F668,0xC2F66A,
    0xC2F66C,
};
int glue_C2F64E_owns(uint32_t pc) { return owns_pc(owned_C2F64E,sizeof owned_C2F64E/sizeof owned_C2F64E[0],pc); }
int glue_C2F64E_complete_step(void) { if(!glue_C2F64E_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C301F6[]={
    0xC301F6,0xC301FC,0xC30202,0xC30204,0xC30206,0xC3020A,0xC3020C,0xC3020E,
    0xC30210,0xC30212,0xC30214,0xC30216,0xC30218,0xC3021A,0xC3021C,0xC3021E,
    0xC30220,0xC30222,0xC30224,0xC30226,0xC30228,0xC3022A,0xC3022C,0xC3022E,
    0xC30230,0xC30232,0xC30234,0xC30236,0xC30238,0xC3023A,0xC3023C,0xC3023E,
    0xC30240,0xC30242,0xC30244,0xC30246,0xC30248,0xC3024A,0xC3024C,0xC3024E,
    0xC30250,0xC30252,0xC30254,0xC30256,0xC30258,0xC3025A,0xC3025C,0xC3025E,
    0xC30260,0xC30262,0xC30264,0xC30266,0xC30268,0xC3026C,0xC3026E,0xC30272,
    0xC30274,0xC30276,0xC30278,0xC3027A,0xC3027C,0xC30280,0xC30282,0xC30284,
    0xC30286,0xC30288,0xC3028A,0xC3028E,0xC30290,0xC30292,0xC30294,0xC30296,
    0xC30298,0xC3029C,0xC3029E,0xC302A4,0xC302AA,0xC302AC,0xC302B6,0xC302BA,
    0xC302C0,0xC302C2,0xC302C4,0xC302C6,0xC302C8,0xC302CA,0xC302CC,0xC302CE,
    0xC302D2,0xC302D4,0xC302D6,0xC302DA,0xC302DC,0xC302DE,0xC302E0,0xC302E2,
    0xC302E4,0xC302E6,0xC302EA,0xC302EC,0xC302EE,0xC302F0,0xC302F2,0xC302F4,
    0xC302FC,0xC30306,0xC3030C,0xC30312,0xC30314,0xC30316,0xC3031C,0xC30320,
    0xC30322,0xC30324,0xC30328,0xC3032A,0xC3032C,0xC3032E,0xC30330,0xC30332,
    0xC3033A,0xC3033C,0xC30340,0xC30342,0xC3034A,0xC3034C,0xC3034E,0xC30350,
    0xC30352,0xC30354,0xC30356,0xC30358,0xC3035C,0xC30362,0xC30364,0xC30366,
    0xC30368,0xC3036A,0xC3036C,0xC3036E,0xC30370,0xC30372,0xC30374,0xC30376,
    0xC30378,0xC3037A,0xC3037C,0xC3037E,0xC30380,0xC30382,0xC30384,0xC30386,
    0xC30388,0xC3038A,0xC3038C,0xC3038E,0xC30390,0xC30392,0xC30394,0xC3039A,
    0xC3039C,0xC303A2,0xC303A4,0xC303AA,0xC303AC,0xC303B2,0xC303B8,0xC303BA,
    0xC303BC,0xC303BE,0xC303C0,0xC303C2,0xC303C4,0xC303CA,0xC303CC,0xC303D2,
    0xC303D8,0xC303DA,0xC303DC,0xC303DE,0xC303E0,0xC303E6,0xC303EC,0xC303F0,
    0xC303F4,0xC303F8,0xC303FC,0xC30400,0xC30404,0xC30408,0xC3040A,
};
int glue_C301F6_owns(uint32_t pc) { return owns_pc(owned_C301F6,sizeof owned_C301F6/sizeof owned_C301F6[0],pc); }
int glue_C301F6_complete_step(void) { if(!glue_C301F6_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C330FE[]={
    0xC330FE,0xC33100,0xC33102,0xC33104,0xC33106,0xC33108,0xC3310A,0xC3310C,
    0xC3310E,0xC33110,0xC33112,0xC33114,0xC33116,0xC3311A,0xC3311E,0xC33120,
    0xC33122,0xC33124,0xC33126,0xC33128,0xC3312A,0xC3312C,0xC3312E,0xC33130,
    0xC33132,0xC33134,0xC33138,0xC3313C,0xC3313E,0xC33140,0xC33142,0xC33144,
    0xC33146,0xC33148,0xC3314A,0xC3314C,0xC3314E,0xC33150,0xC33152,0xC33154,
    0xC33156,0xC3315A,0xC3315E,0xC33160,0xC33162,0xC33164,0xC33166,0xC33168,
};
int glue_C330FE_owns(uint32_t pc) { return owns_pc(owned_C330FE,sizeof owned_C330FE/sizeof owned_C330FE[0],pc); }
int glue_C330FE_complete_step(void) { if(!glue_C330FE_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F826[]={
    0xC2F826,0xC2F828,0xC2F82A,0xC2F82C,0xC2F82E,
};
int glue_C2F826_owns(uint32_t pc) { return owns_pc(owned_C2F826,sizeof owned_C2F826/sizeof owned_C2F826[0],pc); }
int glue_C2F826_complete_step(void) { if(!glue_C2F826_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F83A[]={
    0xC2F83A,0xC2F83C,0xC2F83E,0xC2F840,0xC2F842,
};
int glue_C2F83A_owns(uint32_t pc) { return owns_pc(owned_C2F83A,sizeof owned_C2F83A/sizeof owned_C2F83A[0],pc); }
int glue_C2F83A_complete_step(void) { if(!glue_C2F83A_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F844[]={
    0xC2F844,0xC2F846,0xC2F848,0xC2F84A,0xC2F84C,
};
int glue_C2F844_owns(uint32_t pc) { return owns_pc(owned_C2F844,sizeof owned_C2F844/sizeof owned_C2F844[0],pc); }
int glue_C2F844_complete_step(void) { if(!glue_C2F844_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F84E[]={
    0xC2F84E,0xC2F850,0xC2F852,0xC2F854,0xC2F856,
};
int glue_C2F84E_owns(uint32_t pc) { return owns_pc(owned_C2F84E,sizeof owned_C2F84E/sizeof owned_C2F84E[0],pc); }
int glue_C2F84E_complete_step(void) { if(!glue_C2F84E_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F858[]={
    0xC2F858,0xC2F85A,0xC2F85C,0xC2F85E,0xC2F860,
};
int glue_C2F858_owns(uint32_t pc) { return owns_pc(owned_C2F858,sizeof owned_C2F858/sizeof owned_C2F858[0],pc); }
int glue_C2F858_complete_step(void) { if(!glue_C2F858_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F862[]={
    0xC2F862,0xC2F864,0xC2F866,0xC2F868,0xC2F86A,
};
int glue_C2F862_owns(uint32_t pc) { return owns_pc(owned_C2F862,sizeof owned_C2F862/sizeof owned_C2F862[0],pc); }
int glue_C2F862_complete_step(void) { if(!glue_C2F862_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F86C[]={
    0xC2F86C,0xC2F86E,0xC2F870,0xC2F872,0xC2F874,
};
int glue_C2F86C_owns(uint32_t pc) { return owns_pc(owned_C2F86C,sizeof owned_C2F86C/sizeof owned_C2F86C[0],pc); }
int glue_C2F86C_complete_step(void) { if(!glue_C2F86C_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F876[]={
    0xC2F876,0xC2F878,0xC2F87A,0xC2F87C,0xC2F87E,
};
int glue_C2F876_owns(uint32_t pc) { return owns_pc(owned_C2F876,sizeof owned_C2F876/sizeof owned_C2F876[0],pc); }
int glue_C2F876_complete_step(void) { if(!glue_C2F876_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F880[]={
    0xC2F880,0xC2F882,0xC2F884,0xC2F886,0xC2F888,
};
int glue_C2F880_owns(uint32_t pc) { return owns_pc(owned_C2F880,sizeof owned_C2F880/sizeof owned_C2F880[0],pc); }
int glue_C2F880_complete_step(void) { if(!glue_C2F880_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F88A[]={
    0xC2F88A,0xC2F88C,0xC2F88E,0xC2F890,0xC2F892,
};
int glue_C2F88A_owns(uint32_t pc) { return owns_pc(owned_C2F88A,sizeof owned_C2F88A/sizeof owned_C2F88A[0],pc); }
int glue_C2F88A_complete_step(void) { if(!glue_C2F88A_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F894[]={
    0xC2F894,0xC2F896,0xC2F898,0xC2F89A,0xC2F89C,
};
int glue_C2F894_owns(uint32_t pc) { return owns_pc(owned_C2F894,sizeof owned_C2F894/sizeof owned_C2F894[0],pc); }
int glue_C2F894_complete_step(void) { if(!glue_C2F894_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F89E[]={
    0xC2F89E,0xC2F8A0,0xC2F8A2,0xC2F8A4,0xC2F8A6,
};
int glue_C2F89E_owns(uint32_t pc) { return owns_pc(owned_C2F89E,sizeof owned_C2F89E/sizeof owned_C2F89E[0],pc); }
int glue_C2F89E_complete_step(void) { if(!glue_C2F89E_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F8A8[]={
    0xC2F8A8,0xC2F8AA,0xC2F8AC,0xC2F8AE,0xC2F8B0,
};
int glue_C2F8A8_owns(uint32_t pc) { return owns_pc(owned_C2F8A8,sizeof owned_C2F8A8/sizeof owned_C2F8A8[0],pc); }
int glue_C2F8A8_complete_step(void) { if(!glue_C2F8A8_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F8B2[]={
    0xC2F8B2,0xC2F8B4,0xC2F8B6,0xC2F8B8,0xC2F8BA,
};
int glue_C2F8B2_owns(uint32_t pc) { return owns_pc(owned_C2F8B2,sizeof owned_C2F8B2/sizeof owned_C2F8B2[0],pc); }
int glue_C2F8B2_complete_step(void) { if(!glue_C2F8B2_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F8BC[]={
    0xC2F8BC,0xC2F8BE,0xC2F8C0,0xC2F8C2,0xC2F8C4,
};
int glue_C2F8BC_owns(uint32_t pc) { return owns_pc(owned_C2F8BC,sizeof owned_C2F8BC/sizeof owned_C2F8BC[0],pc); }
int glue_C2F8BC_complete_step(void) { if(!glue_C2F8BC_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F8C6[]={
    0xC2F8C6,0xC2F8C8,0xC2F8CA,0xC2F8CC,0xC2F8CE,
};
int glue_C2F8C6_owns(uint32_t pc) { return owns_pc(owned_C2F8C6,sizeof owned_C2F8C6/sizeof owned_C2F8C6[0],pc); }
int glue_C2F8C6_complete_step(void) { if(!glue_C2F8C6_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F8D0[]={
    0xC2F8D0,0xC2F8D2,0xC2F8D6,0xC2F8D8,0xC2F8DC,0xC2F8DE,0xC2F8E2,0xC2F8E4,
    0xC2F8E8,
};
int glue_C2F8D0_owns(uint32_t pc) { return owns_pc(owned_C2F8D0,sizeof owned_C2F8D0/sizeof owned_C2F8D0[0],pc); }
int glue_C2F8D0_complete_step(void) { if(!glue_C2F8D0_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F8EA[]={
    0xC2F8EA,0xC2F8EC,0xC2F8F0,0xC2F8F2,0xC2F8F6,0xC2F8F8,0xC2F8FC,0xC2F8FE,
    0xC2F902,
};
int glue_C2F8EA_owns(uint32_t pc) { return owns_pc(owned_C2F8EA,sizeof owned_C2F8EA/sizeof owned_C2F8EA[0],pc); }
int glue_C2F8EA_complete_step(void) { if(!glue_C2F8EA_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F904[]={
    0xC2F904,0xC2F906,0xC2F90A,0xC2F90C,0xC2F910,0xC2F912,0xC2F916,0xC2F918,
    0xC2F91C,
};
int glue_C2F904_owns(uint32_t pc) { return owns_pc(owned_C2F904,sizeof owned_C2F904/sizeof owned_C2F904[0],pc); }
int glue_C2F904_complete_step(void) { if(!glue_C2F904_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F91E[]={
    0xC2F91E,0xC2F920,0xC2F924,0xC2F926,0xC2F92A,0xC2F92C,0xC2F930,0xC2F932,
    0xC2F936,
};
int glue_C2F91E_owns(uint32_t pc) { return owns_pc(owned_C2F91E,sizeof owned_C2F91E/sizeof owned_C2F91E[0],pc); }
int glue_C2F91E_complete_step(void) { if(!glue_C2F91E_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F938[]={
    0xC2F938,0xC2F93A,0xC2F93E,0xC2F940,0xC2F944,0xC2F946,0xC2F94A,0xC2F94C,
    0xC2F950,
};
int glue_C2F938_owns(uint32_t pc) { return owns_pc(owned_C2F938,sizeof owned_C2F938/sizeof owned_C2F938[0],pc); }
int glue_C2F938_complete_step(void) { if(!glue_C2F938_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F952[]={
    0xC2F952,0xC2F954,0xC2F958,0xC2F95A,0xC2F95E,0xC2F960,0xC2F964,0xC2F966,
    0xC2F96A,
};
int glue_C2F952_owns(uint32_t pc) { return owns_pc(owned_C2F952,sizeof owned_C2F952/sizeof owned_C2F952[0],pc); }
int glue_C2F952_complete_step(void) { if(!glue_C2F952_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F96C[]={
    0xC2F96C,0xC2F96E,0xC2F972,0xC2F974,0xC2F978,0xC2F97A,0xC2F97E,0xC2F980,
    0xC2F984,
};
int glue_C2F96C_owns(uint32_t pc) { return owns_pc(owned_C2F96C,sizeof owned_C2F96C/sizeof owned_C2F96C[0],pc); }
int glue_C2F96C_complete_step(void) { if(!glue_C2F96C_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F986[]={
    0xC2F986,0xC2F988,0xC2F98C,0xC2F98E,0xC2F992,0xC2F994,0xC2F998,0xC2F99A,
    0xC2F99E,
};
int glue_C2F986_owns(uint32_t pc) { return owns_pc(owned_C2F986,sizeof owned_C2F986/sizeof owned_C2F986[0],pc); }
int glue_C2F986_complete_step(void) { if(!glue_C2F986_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F9A0[]={
    0xC2F9A0,0xC2F9A2,0xC2F9A6,0xC2F9A8,0xC2F9AC,0xC2F9AE,0xC2F9B2,0xC2F9B4,
    0xC2F9B8,
};
int glue_C2F9A0_owns(uint32_t pc) { return owns_pc(owned_C2F9A0,sizeof owned_C2F9A0/sizeof owned_C2F9A0[0],pc); }
int glue_C2F9A0_complete_step(void) { if(!glue_C2F9A0_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F9BA[]={
    0xC2F9BA,0xC2F9BC,0xC2F9C0,0xC2F9C2,0xC2F9C6,0xC2F9C8,0xC2F9CC,0xC2F9CE,
    0xC2F9D2,
};
int glue_C2F9BA_owns(uint32_t pc) { return owns_pc(owned_C2F9BA,sizeof owned_C2F9BA/sizeof owned_C2F9BA[0],pc); }
int glue_C2F9BA_complete_step(void) { if(!glue_C2F9BA_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F9D4[]={
    0xC2F9D4,0xC2F9D6,0xC2F9DA,0xC2F9DC,0xC2F9E0,0xC2F9E2,0xC2F9E6,0xC2F9E8,
    0xC2F9EC,
};
int glue_C2F9D4_owns(uint32_t pc) { return owns_pc(owned_C2F9D4,sizeof owned_C2F9D4/sizeof owned_C2F9D4[0],pc); }
int glue_C2F9D4_complete_step(void) { if(!glue_C2F9D4_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2F9EE[]={
    0xC2F9EE,0xC2F9F0,0xC2F9F4,0xC2F9F6,0xC2F9FA,0xC2F9FC,0xC2FA00,0xC2FA02,
    0xC2FA06,
};
int glue_C2F9EE_owns(uint32_t pc) { return owns_pc(owned_C2F9EE,sizeof owned_C2F9EE/sizeof owned_C2F9EE[0],pc); }
int glue_C2F9EE_complete_step(void) { if(!glue_C2F9EE_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2FA08[]={
    0xC2FA08,0xC2FA0A,0xC2FA0E,0xC2FA10,0xC2FA14,0xC2FA16,0xC2FA1A,0xC2FA1C,
    0xC2FA20,
};
int glue_C2FA08_owns(uint32_t pc) { return owns_pc(owned_C2FA08,sizeof owned_C2FA08/sizeof owned_C2FA08[0],pc); }
int glue_C2FA08_complete_step(void) { if(!glue_C2FA08_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2FA22[]={
    0xC2FA22,0xC2FA24,0xC2FA28,0xC2FA2A,0xC2FA2E,0xC2FA30,0xC2FA34,0xC2FA36,
    0xC2FA3A,
};
int glue_C2FA22_owns(uint32_t pc) { return owns_pc(owned_C2FA22,sizeof owned_C2FA22/sizeof owned_C2FA22[0],pc); }
int glue_C2FA22_complete_step(void) { if(!glue_C2FA22_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2FA3C[]={
    0xC2FA3C,0xC2FA3E,0xC2FA42,0xC2FA44,0xC2FA48,0xC2FA4A,0xC2FA4E,0xC2FA50,
    0xC2FA54,
};
int glue_C2FA3C_owns(uint32_t pc) { return owns_pc(owned_C2FA3C,sizeof owned_C2FA3C/sizeof owned_C2FA3C[0],pc); }
int glue_C2FA3C_complete_step(void) { if(!glue_C2FA3C_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
static const uint32_t owned_C2FA56[]={
    0xC2FA56,0xC2FA58,0xC2FA5C,0xC2FA5E,0xC2FA62,0xC2FA64,0xC2FA68,0xC2FA6A,
    0xC2FA6E,
};
int glue_C2FA56_owns(uint32_t pc) { return owns_pc(owned_C2FA56,sizeof owned_C2FA56/sizeof owned_C2FA56[0],pc); }
int glue_C2FA56_complete_step(void) { if(!glue_C2FA56_owns(REG_PC)) return 0; return render_entry_helpers_step(); }
