/* Complete record control actions family source CPU/bus/event boundaries.
 * Readable behavior lives in record_control_actions.c.
 * Reproduce with tools/recomp/make_command_dispatch_step.py --family record_control_actions. */
#include "glue_renderer_step_math.h"
#include "glue_cache_step_operands.h"
#include "glue_unsigned_division_step.h"
#include "glue_record_control_actions.h"

static int record_control_actions_step(void) {
    uint32_t pc=REG_PC,value,address=0,old;
    uint16_t opcode,mask;
    unsigned mode,reg,destination,width=0;
    char operation;
    opcode=step_begin(pc); mode=(opcode>>3)&7u; reg=opcode&7u; destination=(opcode>>9)&7u;
    switch(pc) {
    case 0xC15138: case 0xC153FC: case 0xC15688: case 0xC159AE:
    case 0xC15AD4: case 0xC181A0:
        m68ki_push_32(A(reg)); A(reg)=A(7); A(7)+=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); break;
    case 0xC1513C: case 0xC15144: case 0xC15148: case 0xC1515C:
    case 0xC15162: case 0xC15170: case 0xC15176: case 0xC1517A:
    case 0xC15182: case 0xC15414: case 0xC15430: case 0xC15442:
    case 0xC1545A: case 0xC15464: case 0xC1546C: case 0xC15482:
    case 0xC1548C: case 0xC15494: case 0xC154A6: case 0xC154B2:
    case 0xC154CC: case 0xC154D2: case 0xC154E6: case 0xC154F2:
    case 0xC154FA: case 0xC1551E: case 0xC1552A: case 0xC15550:
    case 0xC1555C: case 0xC15564: case 0xC15588: case 0xC15594:
    case 0xC155B2: case 0xC155E8: case 0xC155F2: case 0xC15608:
    case 0xC15610: case 0xC15622: case 0xC15632: case 0xC15640:
    case 0xC15646: case 0xC15648: case 0xC15664: case 0xC1569A:
    case 0xC156A8: case 0xC156B4: case 0xC156BA: case 0xC156BE:
    case 0xC156C8: case 0xC156D4: case 0xC156DA: case 0xC156E6:
    case 0xC156FC: case 0xC15702: case 0xC15706: case 0xC1570C:
    case 0xC15712: case 0xC15718: case 0xC15724: case 0xC1572A:
    case 0xC15730: case 0xC15736: case 0xC1573C: case 0xC15744:
    case 0xC1574A: case 0xC1575A: case 0xC15762: case 0xC1576C:
    case 0xC1577E: case 0xC15786: case 0xC15790: case 0xC157A8:
    case 0xC157B6: case 0xC15812: case 0xC1581C: case 0xC15828:
    case 0xC1582E: case 0xC15834: case 0xC15866: case 0xC1586C:
    case 0xC15872: case 0xC15878: case 0xC15888: case 0xC15890:
    case 0xC158A2: case 0xC158AA: case 0xC158C2: case 0xC159B6:
    case 0xC159DE: case 0xC159EC: case 0xC159F2: case 0xC15A06:
    case 0xC15A12: case 0xC15A16: case 0xC15A20: case 0xC15A2E:
    case 0xC15A38: case 0xC15A40: case 0xC15A4C: case 0xC15AA6:
    case 0xC15AAC: case 0xC15AB2: case 0xC15AB8: case 0xC15ADC:
    case 0xC15AE2: case 0xC15AE8: case 0xC15AEE: case 0xC15B06:
    case 0xC15B1E: case 0xC15B36: case 0xC15B82:
        width=2; goto move;
    case 0xC15140: case 0xC15A48:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='+'; goto arithmetic;
    case 0xC1514C: case 0xC1541C: case 0xC154AC: case 0xC154F6:
    case 0xC15560: case 0xC155EE: case 0xC15818: case 0xC15B8A:
        cache_step_logic(cache_step_read(mode,reg,2),2); break;
    case 0xC1514E: case 0xC1547C: case 0xC154DE: case 0xC154F8:
    case 0xC15548: case 0xC15562:
        step_branch(pc,opcode,COND_PL()); break;
    case 0xC15150:
        address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_negate(&old,2); cache_step_write_memory(address,old,2,0); break;
    case 0xC15154: case 0xC15168: case 0xC1552E: case 0xC15598:
        value=m68ki_read_imm_16(); step_compare_word(value,cache_step_read(mode,reg,2)); break;
    case 0xC1515A: case 0xC1516E: case 0xC1541E: case 0xC15532:
    case 0xC1559C:
        step_branch(pc,opcode,COND_LE()); break;
    case 0xC15160: case 0xC15174: case 0xC15A0A:
        if((opcode&0xc0u)!=0xc0u) { renderer_asr_word(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break; } address=cache_step_address(mode,reg,2); old=cache_step_read_memory(address,2); renderer_asr_word(&old,1); USE_CYCLES(-2); cache_step_write_memory(address,old,2,0); break;
    case 0xC15166: case 0xC15472: case 0xC154D4: case 0xC1550A:
    case 0xC15574: case 0xC155AC: case 0xC15602: case 0xC15620:
    case 0xC1563A: case 0xC156C2: case 0xC156EC: case 0xC1570A:
    case 0xC15826: case 0xC1585A: case 0xC15860: case 0xC159DC:
    case 0xC159E4: case 0xC159EA: case 0xC15A26: case 0xC15A3E:
        step_branch(pc,opcode,1); break;
    case 0xC1517E:
        width=2; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC15186: case 0xC15446: case 0xC154B6: case 0xC155F6:
    case 0xC15614: case 0xC15626: case 0xC1564C: case 0xC15658:
    case 0xC15668: case 0xC15674: case 0xC1583A: case 0xC158C6:
    case 0xC159BC: case 0xC159FA: case 0xC15AAA: case 0xC15AB0:
    case 0xC15AB6: case 0xC15ABC: case 0xC15AE0: case 0xC15AE6:
    case 0xC15AEC: case 0xC15AF2: case 0xC15B0C: case 0xC15B24:
    case 0xC15B3C:
        D(reg)=(uint32_t)(int32_t)(int16_t)D(reg); flags_logic_l(D(reg)); break;
    case 0xC15188: case 0xC15684: case 0xC158D4: case 0xC15AD0:
    case 0xC15BF2: case 0xC181F6:
        A(7)=A(reg); A(reg)=m68ki_pull_32(); break;
    case 0xC1518A: case 0xC15686: case 0xC158D6: case 0xC15AD2:
    case 0xC15BF4: case 0xC181F8:
        REG_PC=m68ki_pull_32(); break;
    case 0xC15400: case 0xC15680: case 0xC1568C: case 0xC158D0:
    case 0xC159B2: case 0xC15ACC: case 0xC15AD8: case 0xC15BEE:
        mask=m68ki_read_imm_16(); if(!(opcode&0x400u) && mode==4) renderer_store(A(reg),mask,4,(int)reg); else { address=cache_step_address(mode,reg,4); if(opcode&0x400u) renderer_load(address,mask,4,mode==3?(int)reg:-1); else renderer_store(address,mask,4,-1); } break;
    case 0xC15404: case 0xC1540E: case 0xC15428: case 0xC15456:
    case 0xC15466: case 0xC15476: case 0xC1547E: case 0xC1548E:
    case 0xC1549A: case 0xC154C8: case 0xC154D8: case 0xC154E0:
    case 0xC154EC: case 0xC15500: case 0xC1550C: case 0xC15518:
    case 0xC15524: case 0xC15538: case 0xC15542: case 0xC1554A:
    case 0xC15556: case 0xC1556A: case 0xC15576: case 0xC15582:
    case 0xC1558E: case 0xC155A2: case 0xC155AE: case 0xC155C2:
    case 0xC155D0: case 0xC155DE: case 0xC155E4: case 0xC15604:
    case 0xC1563C: case 0xC15690: case 0xC156AE: case 0xC156C4:
    case 0xC156E0: case 0xC1571E: case 0xC157A2: case 0xC157B0:
    case 0xC157D2: case 0xC157E6: case 0xC15804: case 0xC1580E:
    case 0xC159FE: case 0xC15A0C: case 0xC15A28: case 0xC15A64:
    case 0xC15B14: case 0xC15B2C: case 0xC15B44: case 0xC15B52:
    case 0xC15B60: case 0xC15B6E: case 0xC15B7C: case 0xC15B9E:
    case 0xC15BAE: case 0xC15BC0: case 0xC15BD2: case 0xC15BE4:
    case 0xC181B6:
        width=4; goto move;
    case 0xC1540A: case 0xC15696:
        A(destination)+=(uint32_t)(int32_t)(int16_t)cache_step_read(mode,reg,2); break;
    case 0xC15418: case 0xC15422: case 0xC15432: case 0xC15436:
    case 0xC15448: case 0xC154A0: case 0xC154B8: case 0xC155BA:
    case 0xC155C8: case 0xC155D6: case 0xC155F8: case 0xC15616:
    case 0xC15628: case 0xC1569C: case 0xC15754: case 0xC15778:
    case 0xC1579C: case 0xC157BE: case 0xC157CA: case 0xC157D8:
    case 0xC157DA: case 0xC157EC: case 0xC157F0: case 0xC157FC:
    case 0xC1580A: case 0xC15882: case 0xC1589C: case 0xC158B6:
    case 0xC158C8: case 0xC15A50: case 0xC15A5C: case 0xC15A6C:
    case 0xC15A7C: case 0xC15A88: case 0xC15A94: case 0xC15A9A:
    case 0xC15AA0: case 0xC15ABE: case 0xC15AC0: case 0xC15AC2:
    case 0xC15AC4: case 0xC15AF4: case 0xC15AF6: case 0xC15AF8:
    case 0xC15AFA: case 0xC15B0E: case 0xC15B1A: case 0xC15B26:
    case 0xC15B32: case 0xC15B3E: case 0xC15B4A: case 0xC15B4E:
    case 0xC15B58: case 0xC15B5C: case 0xC15B66: case 0xC15B6A:
    case 0xC15B74: case 0xC15B8E: case 0xC15B98: case 0xC15BA8:
    case 0xC15BB8: case 0xC15BCA: case 0xC15BDC: case 0xC181AE:
    case 0xC181BE: case 0xC181C2: case 0xC181CC: case 0xC181D2:
    case 0xC181DA: case 0xC181E0: case 0xC181E8: case 0xC181EC:
        width=4; goto move;
    case 0xC15426: case 0xC1542E: case 0xC15450: case 0xC154C0:
    case 0xC15600: case 0xC1561E: case 0xC15630: case 0xC1564E:
    case 0xC1566A: case 0xC158CE: case 0xC181B4:
        width=4; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC1543A: case 0xC155B4: case 0xC1560A: case 0xC156A0:
    case 0xC156CA: case 0xC15A1A: case 0xC15A32:
        value=(opcode&0x0100u)?D(destination):m68ki_read_imm_16(); operation='?'; goto bit_value;
    case 0xC1543E: case 0xC1565C: case 0xC15678: case 0xC156F8:
    case 0xC15854: case 0xC159D6:
        step_branch(pc,opcode,COND_NE()); break;
    case 0xC1544A: case 0xC154BA: case 0xC155FA: case 0xC15618:
    case 0xC1562A: case 0xC15AFC:
        address=cache_step_address(mode,reg,4); m68ki_push_32(REG_PC); REG_PC=address; break;
    case 0xC15452: case 0xC1547A: case 0xC154C2: case 0xC154DC:
    case 0xC15546: case 0xC15852: case 0xC159D4: case 0xC181A4:
        cache_step_logic(cache_step_read(mode,reg,4),4); break;
    case 0xC15454: case 0xC154AE: case 0xC154C4: case 0xC155B8:
    case 0xC155F0: case 0xC1560E: case 0xC156A4: case 0xC156CE:
    case 0xC1581A: case 0xC15848: case 0xC15850: case 0xC159CA:
    case 0xC159D2: case 0xC15A1E: case 0xC15A36: case 0xC15B8C:
    case 0xC181AA:
        step_branch(pc,opcode,COND_EQ()); break;
    case 0xC1545C: case 0xC15484: case 0xC155EA: case 0xC15642:
    case 0xC15814: case 0xC15B86:
        width=2; value=m68ki_read_imm_16(); operation='&'; goto immediate_logic;
    case 0xC15460: case 0xC15488: case 0xC154CE:
        width=2; value=m68ki_read_imm_16(); operation='|'; goto immediate_logic;
    case 0xC154A4: case 0xC155C0: case 0xC155CE: case 0xC155DC:
    case 0xC15752: case 0xC15776: case 0xC1579A: case 0xC15880:
    case 0xC1589A: case 0xC158B4: case 0xC15A98: case 0xC15A9E:
    case 0xC15AA4: case 0xC15BBE: case 0xC15BD0: case 0xC15BE2:
        step_asr_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC154EA: case 0xC15554: case 0xC1585C:
        width=2; value=destination?destination:8; operation='-'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC15504: case 0xC1556E:
        width=4; value=m68ki_read_imm_32(); operation='+'; goto arithmetic;
    case 0xC15510: case 0xC1557A: case 0xC15842: case 0xC1584A:
    case 0xC159C4: case 0xC159CC:
        value=m68ki_read_imm_32(); step_compare_long(value,cache_step_read(mode,reg,4)); break;
    case 0xC15516: case 0xC15580:
        step_branch(pc,opcode,COND_LT()); break;
    case 0xC15522: case 0xC1558C: case 0xC15856: case 0xC15862:
        width=2; value=destination?destination:8; operation='+'; if(mode==1) { A(reg)=operation=='+'?A(reg)+value:A(reg)-value; break; } goto arithmetic;
    case 0xC15534: case 0xC1559E: case 0xC156D0: case 0xC15822:
    case 0xC158BC: case 0xC159D8: case 0xC159E6: case 0xC15B78:
        cache_step_write(mode,reg,2,0); cache_step_logic(0,2); break;
    case 0xC1553C: case 0xC155A6:
        width=4; value=m68ki_read_imm_32(); operation='-'; goto arithmetic;
    case 0xC155C6: case 0xC155D4: case 0xC155E2: case 0xC15742:
    case 0xC15750: case 0xC1576A: case 0xC15774: case 0xC1578E:
    case 0xC15798: case 0xC157D0: case 0xC157E0: case 0xC15802:
    case 0xC1587E: case 0xC15898: case 0xC158B2: case 0xC15A62:
    case 0xC15A72: case 0xC15A8E: case 0xC15B94: case 0xC15BA4:
    case 0xC15BB4: case 0xC15BC6: case 0xC15BD8: case 0xC15BEA:
        width=4; if(opcode&0x100u) { value=D(destination); operation='+'; goto arithmetic; } value=cache_step_read(mode,reg,4); mode=0; reg=destination; operation='+'; goto arithmetic;
    case 0xC15650: case 0xC1566C: case 0xC156EE:
        width=1; goto move;
    case 0xC15656: case 0xC15672:
        SET_W(D(reg),(int16_t)(int8_t)D(reg)); flags_logic_w(D(reg)); break;
    case 0xC1565A: case 0xC15676:
        step_compare_long(cache_step_read(mode,reg,4),D(destination)); break;
    case 0xC1565E: case 0xC1567A:
        cache_step_write(mode,reg,1,0); cache_step_logic(0,1); break;
    case 0xC156A6: case 0xC156FA: case 0xC159F8: case 0xC181AC:
    case 0xC181BC: case 0xC181CA: case 0xC181D8: case 0xC181DE:
    case 0xC181E6: case 0xC181EA:
        D(destination)=(uint32_t)(int32_t)(int8_t)opcode; flags_logic_l(D(destination)); break;
    case 0xC156F4:
        value=m68ki_read_imm_16(); step_compare_byte(value,cache_step_read(mode,reg,1)); break;
    case 0xC15734: case 0xC15740: case 0xC1574E: case 0xC15760:
    case 0xC15768: case 0xC15772: case 0xC15784: case 0xC1578C:
    case 0xC15796: case 0xC15870: case 0xC1587C: case 0xC1588E:
    case 0xC15896: case 0xC158A8: case 0xC158B0:
        renderer_multiply(&D(destination),(uint16_t)cache_step_read(mode,reg,2)); break;
    case 0xC157C4: case 0xC157F6: case 0xC1583C: case 0xC159BE:
    case 0xC15A56: case 0xC15A82:
        width=4; value=m68ki_read_imm_32(); operation='&'; goto immediate_logic;
    case 0xC158CA: case 0xC15AC6: case 0xC181B0: case 0xC181EE:
        value=opcode&0xffu; if(!value) value=(uint32_t)(int32_t)(int16_t)m68ki_read_imm_16(); else value=(uint32_t)(int32_t)(int8_t)value; m68ki_push_32(REG_PC); REG_PC=pc+2+value; break;
    case 0xC159FC: case 0xC15B12: case 0xC15B2A: case 0xC15B42:
    case 0xC181D0:
        step_asl_long(&D(reg),(opcode&0x20u)?D(destination):(destination?destination:8)); break;
    case 0xC15A00:
        A(destination)+=cache_step_read(mode,reg,4); break;
    case 0xC15A44:
        width=2; value=m68ki_read_imm_16(); operation='+'; goto arithmetic;
    case 0xC15A6A: case 0xC15A78: case 0xC15A90:
        width=4; value=cache_step_read(mode,reg,width); reg=destination; mode=0; operation='-'; goto arithmetic;
    case 0xC15AC8: case 0xC15B02: case 0xC181F2:
        A(destination)=cache_step_address(mode,reg,4); break;
    case 0xC181E4:
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
static const uint32_t owned_C153FC[]={
    0xC153FC,0xC15400,0xC15404,0xC1540A,0xC1540E,0xC15414,0xC15418,0xC1541C,
    0xC1541E,0xC15422,0xC15426,0xC15428,0xC1542E,0xC15430,0xC15432,0xC15436,
    0xC1543A,0xC1543E,0xC15442,0xC15446,0xC15448,0xC1544A,0xC15450,0xC15452,
    0xC15454,0xC15456,0xC1545A,0xC1545C,0xC15460,0xC15464,0xC15466,0xC1546C,
    0xC15472,0xC15476,0xC1547A,0xC1547C,0xC1547E,0xC15482,0xC15484,0xC15488,
    0xC1548C,0xC1548E,0xC15494,0xC1549A,0xC154A0,0xC154A4,0xC154A6,0xC154AC,
    0xC154AE,0xC154B2,0xC154B6,0xC154B8,0xC154BA,0xC154C0,0xC154C2,0xC154C4,
    0xC154C8,0xC154CC,0xC154CE,0xC154D2,0xC154D4,0xC154D8,0xC154DC,0xC154DE,
    0xC154E0,0xC154E6,0xC154EA,0xC154EC,0xC154F2,0xC154F6,0xC154F8,0xC154FA,
    0xC15500,0xC15504,0xC1550A,0xC1550C,0xC15510,0xC15516,0xC15518,0xC1551E,
    0xC15522,0xC15524,0xC1552A,0xC1552E,0xC15532,0xC15534,0xC15538,0xC1553C,
    0xC15542,0xC15546,0xC15548,0xC1554A,0xC15550,0xC15554,0xC15556,0xC1555C,
    0xC15560,0xC15562,0xC15564,0xC1556A,0xC1556E,0xC15574,0xC15576,0xC1557A,
    0xC15580,0xC15582,0xC15588,0xC1558C,0xC1558E,0xC15594,0xC15598,0xC1559C,
    0xC1559E,0xC155A2,0xC155A6,0xC155AC,0xC155AE,0xC155B2,0xC155B4,0xC155B8,
    0xC155BA,0xC155C0,0xC155C2,0xC155C6,0xC155C8,0xC155CE,0xC155D0,0xC155D4,
    0xC155D6,0xC155DC,0xC155DE,0xC155E2,0xC155E4,0xC155E8,0xC155EA,0xC155EE,
    0xC155F0,0xC155F2,0xC155F6,0xC155F8,0xC155FA,0xC15600,0xC15602,0xC15604,
    0xC15608,0xC1560A,0xC1560E,0xC15610,0xC15614,0xC15616,0xC15618,0xC1561E,
    0xC15620,0xC15622,0xC15626,0xC15628,0xC1562A,0xC15630,0xC15632,0xC1563A,
    0xC1563C,0xC15640,0xC15642,0xC15646,0xC15648,0xC1564C,0xC1564E,0xC15650,
    0xC15656,0xC15658,0xC1565A,0xC1565C,0xC1565E,0xC15664,0xC15668,0xC1566A,
    0xC1566C,0xC15672,0xC15674,0xC15676,0xC15678,0xC1567A,0xC15680,0xC15684,
    0xC15686,
};
int glue_C153FC_owns(uint32_t pc) { return owns_pc(owned_C153FC,sizeof owned_C153FC/sizeof owned_C153FC[0],pc); }
int glue_C153FC_step(void) { if(!glue_C153FC_owns(REG_PC)) return 0; return record_control_actions_step(); }
static const uint32_t owned_C15688[]={
    0xC15688,0xC1568C,0xC15690,0xC15696,0xC1569A,0xC1569C,0xC156A0,0xC156A4,
    0xC156A6,0xC156A8,0xC156AE,0xC156B4,0xC156BA,0xC156BE,0xC156C2,0xC156C4,
    0xC156C8,0xC156CA,0xC156CE,0xC156D0,0xC156D4,0xC156DA,0xC156E0,0xC156E6,
    0xC156EC,0xC156EE,0xC156F4,0xC156F8,0xC156FA,0xC156FC,0xC15702,0xC15706,
    0xC1570A,0xC1570C,0xC15712,0xC15718,0xC1571E,0xC15724,0xC1572A,0xC15730,
    0xC15734,0xC15736,0xC1573C,0xC15740,0xC15742,0xC15744,0xC1574A,0xC1574E,
    0xC15750,0xC15752,0xC15754,0xC1575A,0xC15760,0xC15762,0xC15768,0xC1576A,
    0xC1576C,0xC15772,0xC15774,0xC15776,0xC15778,0xC1577E,0xC15784,0xC15786,
    0xC1578C,0xC1578E,0xC15790,0xC15796,0xC15798,0xC1579A,0xC1579C,0xC157A2,
    0xC157A8,0xC157B0,0xC157B6,0xC157BE,0xC157C4,0xC157CA,0xC157D0,0xC157D2,
    0xC157D8,0xC157DA,0xC157E0,0xC157E6,0xC157EC,0xC157F0,0xC157F6,0xC157FC,
    0xC15802,0xC15804,0xC1580A,0xC1580E,0xC15812,0xC15814,0xC15818,0xC1581A,
    0xC1581C,0xC15822,0xC15826,0xC15828,0xC1582E,0xC15834,0xC1583A,0xC1583C,
    0xC15842,0xC15848,0xC1584A,0xC15850,0xC15852,0xC15854,0xC15856,0xC1585A,
    0xC1585C,0xC15860,0xC15862,0xC15866,0xC1586C,0xC15870,0xC15872,0xC15878,
    0xC1587C,0xC1587E,0xC15880,0xC15882,0xC15888,0xC1588E,0xC15890,0xC15896,
    0xC15898,0xC1589A,0xC1589C,0xC158A2,0xC158A8,0xC158AA,0xC158B0,0xC158B2,
    0xC158B4,0xC158B6,0xC158BC,0xC158C2,0xC158C6,0xC158C8,0xC158CA,0xC158CE,
    0xC158D0,0xC158D4,0xC158D6,
};
int glue_C15688_owns(uint32_t pc) { return owns_pc(owned_C15688,sizeof owned_C15688/sizeof owned_C15688[0],pc); }
int glue_C15688_step(void) { if(!glue_C15688_owns(REG_PC)) return 0; return record_control_actions_step(); }
static const uint32_t owned_C159AE[]={
    0xC159AE,0xC159B2,0xC159B6,0xC159BC,0xC159BE,0xC159C4,0xC159CA,0xC159CC,
    0xC159D2,0xC159D4,0xC159D6,0xC159D8,0xC159DC,0xC159DE,0xC159E4,0xC159E6,
    0xC159EA,0xC159EC,0xC159F2,0xC159F8,0xC159FA,0xC159FC,0xC159FE,0xC15A00,
    0xC15A06,0xC15A0A,0xC15A0C,0xC15A12,0xC15A16,0xC15A1A,0xC15A1E,0xC15A20,
    0xC15A26,0xC15A28,0xC15A2E,0xC15A32,0xC15A36,0xC15A38,0xC15A3E,0xC15A40,
    0xC15A44,0xC15A48,0xC15A4C,0xC15A50,0xC15A56,0xC15A5C,0xC15A62,0xC15A64,
    0xC15A6A,0xC15A6C,0xC15A72,0xC15A78,0xC15A7C,0xC15A82,0xC15A88,0xC15A8E,
    0xC15A90,0xC15A94,0xC15A98,0xC15A9A,0xC15A9E,0xC15AA0,0xC15AA4,0xC15AA6,
    0xC15AAA,0xC15AAC,0xC15AB0,0xC15AB2,0xC15AB6,0xC15AB8,0xC15ABC,0xC15ABE,
    0xC15AC0,0xC15AC2,0xC15AC4,0xC15AC6,0xC15AC8,0xC15ACC,0xC15AD0,0xC15AD2,
};
int glue_C159AE_owns(uint32_t pc) { return owns_pc(owned_C159AE,sizeof owned_C159AE/sizeof owned_C159AE[0],pc); }
int glue_C159AE_step(void) { if(!glue_C159AE_owns(REG_PC)) return 0; return record_control_actions_step(); }
static const uint32_t owned_C15AD4[]={
    0xC15AD4,0xC15AD8,0xC15ADC,0xC15AE0,0xC15AE2,0xC15AE6,0xC15AE8,0xC15AEC,
    0xC15AEE,0xC15AF2,0xC15AF4,0xC15AF6,0xC15AF8,0xC15AFA,0xC15AFC,0xC15B02,
    0xC15B06,0xC15B0C,0xC15B0E,0xC15B12,0xC15B14,0xC15B1A,0xC15B1E,0xC15B24,
    0xC15B26,0xC15B2A,0xC15B2C,0xC15B32,0xC15B36,0xC15B3C,0xC15B3E,0xC15B42,
    0xC15B44,0xC15B4A,0xC15B4E,0xC15B52,0xC15B58,0xC15B5C,0xC15B60,0xC15B66,
    0xC15B6A,0xC15B6E,0xC15B74,0xC15B78,0xC15B7C,0xC15B82,0xC15B86,0xC15B8A,
    0xC15B8C,0xC15B8E,0xC15B94,0xC15B98,0xC15B9E,0xC15BA4,0xC15BA8,0xC15BAE,
    0xC15BB4,0xC15BB8,0xC15BBE,0xC15BC0,0xC15BC6,0xC15BCA,0xC15BD0,0xC15BD2,
    0xC15BD8,0xC15BDC,0xC15BE2,0xC15BE4,0xC15BEA,0xC15BEE,0xC15BF2,0xC15BF4,
};
int glue_C15AD4_owns(uint32_t pc) { return owns_pc(owned_C15AD4,sizeof owned_C15AD4/sizeof owned_C15AD4[0],pc); }
int glue_C15AD4_step(void) { if(!glue_C15AD4_owns(REG_PC)) return 0; return record_control_actions_step(); }
static const uint32_t owned_C181A0[]={
    0xC181A0,0xC181A4,0xC181AA,0xC181AC,0xC181AE,0xC181B0,0xC181B4,0xC181B6,
    0xC181BC,0xC181BE,0xC181C2,0xC181CA,0xC181CC,0xC181D0,0xC181D2,0xC181D8,
    0xC181DA,0xC181DE,0xC181E0,0xC181E4,0xC181E6,0xC181E8,0xC181EA,0xC181EC,
    0xC181EE,0xC181F2,0xC181F6,0xC181F8,
};
int glue_C181A0_owns(uint32_t pc) { return owns_pc(owned_C181A0,sizeof owned_C181A0/sizeof owned_C181A0[0],pc); }
int glue_C181A0_step(void) { if(!glue_C181A0_owns(REG_PC)) return 0; return record_control_actions_step(); }
static const uint32_t owned_C15138[]={
    0xC15138,0xC1513C,0xC15140,0xC15144,0xC15148,0xC1514C,0xC1514E,0xC15150,
    0xC15154,0xC1515A,0xC1515C,0xC15160,0xC15162,0xC15166,0xC15168,0xC1516E,
    0xC15170,0xC15174,0xC15176,0xC1517A,0xC1517E,0xC15182,0xC15186,0xC15188,
    0xC1518A,
};
int glue_C15138_owns(uint32_t pc) { return owns_pc(owned_C15138,sizeof owned_C15138/sizeof owned_C15138[0],pc); }
int glue_C15138_step(void) { if(!glue_C15138_owns(REG_PC)) return 0; return record_control_actions_step(); }
