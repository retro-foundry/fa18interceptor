/* Pinned 1.3 ABI/timing adapter. Semantic list operations are reusable host C. */
#include "exec_lists_adapter.h"
#include "../amiga/exec_lists.h"
#include "service_phase.h"
#include "m68kops.h"
#include <string.h>

int fa18_os_exec_lists_signature_matches(const uint8_t *rom) {
    static const uint8_t source[]={
        0x20,0x0A,0x67,0x00,0x00,0x28,0x20,0x12,0x67,0x00,0x00,0x10,
        0x20,0x40,0x48,0xD1,0x04,0x01,0x21,0x49,0x00,0x04,0x24,0x89,0x4E,0x75,
        0x22,0x8A,0x20,0x6A,0x00,0x04,0x23,0x48,0x00,0x04,0x25,0x49,0x00,0x04,
        0x20,0x89,0x4E,0x75,0x20,0x10,0x20,0x89,0x48,0xD1,0x01,0x01,0x20,0x40,
        0x21,0x49,0x00,0x04,0x4E,0x75,0x41,0xE8,0x00,0x04,0x20,0x28,0x00,0x04,
        0x21,0x49,0x00,0x04,0x22,0x88,0x23,0x40,0x00,0x04,0x20,0x40,0x20,0x89,
        0x4E,0x75,0x20,0x51,0x22,0x69,0x00,0x04,0x22,0x88,0x21,0x49,0x00,0x04,
        0x4E,0x75,0x22,0x50,0x20,0x11,0x67,0x08,0x20,0x80,0xC1,0x89,0x23,0x48,
        0x00,0x04,0x4E,0x75,0x22,0x68,0x00,0x08,0x20,0x29,0x00,0x04,0x67,0x0A,
        0x21,0x40,0x00,0x08,0xC1,0x89,0x22,0x88,0x58,0x91,0x4E,0x75,0x12,0x29,
        0x00,0x09,0x20,0x10,0x20,0x40,0x20,0x10,0x67,0x06,0xB2,0x28,0x00,0x09,
        0x6F,0xF4,0x20,0x28,0x00,0x04,0x21,0x49,0x00,0x04,0x22,0x88,0x23,0x40,
        0x00,0x04,0x20,0x40,0x20,0x89,0x4E,0x75
    };
    return rom && !memcmp(rom+0x15E8,source,sizeof source);
}
static uint8_t read_byte(void *c,uint32_t a) { (void)c; return (uint8_t)m68k_read_memory_8(a); }
static uint32_t read_long(void *c,uint32_t a) { (void)c; return m68k_read_memory_32(a); }
static void write_long(void *c,uint32_t a,uint32_t v) { (void)c; m68k_write_memory_32(a,v); }
int fa18_os_exec_lists_step(void) {
    uint32_t pc=REG_PC,target=0;
    uint16_t op; unsigned extensions=0; int word_branch=0;
    AmigaExecListPhase phase;
    switch (pc) {
    case 0xFC15E8u: op=0x200A; phase=AMIGA_LIST_INSERT_ANCHOR; break;
    case 0xFC15EAu: op=0x6700; extensions=1; word_branch=1; target=0xFC1614; phase=AMIGA_LIST_INSERT_HEAD_BRANCH; break;
    case 0xFC15EEu: op=0x2012; phase=AMIGA_LIST_INSERT_NEXT; break;
    case 0xFC15F0u: op=0x6700; extensions=1; word_branch=1; target=0xFC1602; phase=AMIGA_LIST_INSERT_TAIL_BRANCH; break;
    case 0xFC15F4u: case 0xFC161Cu: case 0xFC1636u: case 0xFC1676u: case 0xFC1690u:
        op=0x2040; phase=AMIGA_LIST_A0_FROM_D0; break;
    case 0xFC15F6u: op=0x48D1; extensions=1; phase=AMIGA_LIST_INSERT_LINKS; break;
    case 0xFC15FAu: case 0xFC161Eu: case 0xFC162Cu: case 0xFC1644u: case 0xFC1686u:
        op=0x2149; extensions=1; phase=AMIGA_LIST_NODE_TO_A0_PRED; break;
    case 0xFC15FEu: op=0x2489; phase=AMIGA_LIST_NODE_TO_ANCHOR; break;
    case 0xFC1600u: case 0xFC1612u: case 0xFC1622u: case 0xFC163Au:
    case 0xFC1648u: case 0xFC1658u: case 0xFC166Eu: case 0xFC1694u:
        op=0x4E75; phase=AMIGA_LIST_RETURN; break;
    case 0xFC1602u: op=0x228A; phase=AMIGA_LIST_ANCHOR_TO_NODE; break;
    case 0xFC1604u: op=0x206A; extensions=1; phase=AMIGA_LIST_ANCHOR_PRED_TO_A0; break;
    case 0xFC1608u: case 0xFC1654u: op=0x2348; extensions=1; phase=AMIGA_LIST_A0_TO_NODE_PRED; break;
    case 0xFC160Cu: op=0x2549; extensions=1; phase=AMIGA_LIST_NODE_TO_ANCHOR_PRED; break;
    case 0xFC1610u: case 0xFC1616u: case 0xFC1638u: case 0xFC1692u:
        op=0x2089; phase=AMIGA_LIST_NODE_TO_A0; break;
    case 0xFC1614u: case 0xFC1674u: case 0xFC1678u:
        op=0x2010; phase=AMIGA_LIST_A0_NEXT_TO_D0; break;
    case 0xFC1618u: op=0x48D1; extensions=1; phase=AMIGA_LIST_HEAD_LINKS; break;
    case 0xFC1624u: op=0x41E8; extensions=1; phase=AMIGA_LIST_A0_TO_TAIL; break;
    case 0xFC1628u: case 0xFC1682u: op=0x2028; extensions=1; phase=AMIGA_LIST_A0_PRED_TO_D0; break;
    case 0xFC1630u: case 0xFC1642u: case 0xFC166Au: case 0xFC168Au:
        op=0x2288; phase=AMIGA_LIST_A0_TO_NODE; break;
    case 0xFC1632u: case 0xFC168Cu: op=0x2340; extensions=1; phase=AMIGA_LIST_D0_TO_NODE_PRED; break;
    case 0xFC163Cu: op=0x2051; phase=AMIGA_LIST_NODE_NEXT_TO_A0; break;
    case 0xFC163Eu: op=0x2269; extensions=1; phase=AMIGA_LIST_NODE_PRED_TO_A1; break;
    case 0xFC164Au: op=0x2250; phase=AMIGA_LIST_HEAD_TO_A1; break;
    case 0xFC164Cu: op=0x2011; phase=AMIGA_LIST_NODE_NEXT_TO_D0; break;
    case 0xFC164Eu: op=0x6708; target=0xFC1658; phase=AMIGA_LIST_REMHEAD_EMPTY_BRANCH; break;
    case 0xFC1650u: op=0x2080; phase=AMIGA_LIST_D0_TO_A0; break;
    case 0xFC1652u: case 0xFC1668u: op=0xC189; phase=AMIGA_LIST_EXCHANGE_D0_A1; break;
    case 0xFC165Au: op=0x2268; extensions=1; phase=AMIGA_LIST_TAIL_PRED_TO_A1; break;
    case 0xFC165Eu: op=0x2029; extensions=1; phase=AMIGA_LIST_NODE_PRED_TO_D0; break;
    case 0xFC1662u: op=0x670A; target=0xFC166E; phase=AMIGA_LIST_REMTAIL_EMPTY_BRANCH; break;
    case 0xFC1664u: op=0x2140; extensions=1; phase=AMIGA_LIST_D0_TO_TAIL_PRED; break;
    case 0xFC166Cu: op=0x5891; phase=AMIGA_LIST_ADD_TAIL_OFFSET; break;
    case 0xFC1670u: op=0x1229; extensions=1; phase=AMIGA_LIST_NODE_PRIORITY_TO_D1; break;
    case 0xFC167Au: op=0x6706; target=0xFC1682; phase=AMIGA_LIST_ENQUEUE_TAIL_BRANCH; break;
    case 0xFC167Cu: op=0xB228; extensions=1; phase=AMIGA_LIST_COMPARE_PRIORITY; break;
    case 0xFC1680u: op=0x6FF4; target=0xFC1676; phase=AMIGA_LIST_ENQUEUE_NEXT_BRANCH; break;
    default: return 0;
    }
    AmigaExecListState state={REG_D[0],REG_D[1],REG_A[0],REG_A[1],REG_A[2],REG_A[7],0,
        (uint8_t)m68k_get_reg(NULL,M68K_REG_SR)};
    AmigaExecListBus bus={NULL,read_byte,read_long,write_long};
    AmigaExecListEffect effect;
    fa18_service_begin(pc,op); fa18_service_extension_words(extensions);
    if (!amiga_exec_list_step(phase,&state,&bus,&effect)) return 0;
    REG_D[0]=state.d0; REG_D[1]=state.d1;
    REG_A[0]=state.a0; REG_A[1]=state.a1; REG_A[2]=state.a2; REG_A[7]=state.sp;
    FLAG_X=state.ccr&AMIGA_CCR_X?XFLAG_SET:XFLAG_CLEAR;
    FLAG_N=state.ccr&AMIGA_CCR_N?NFLAG_SET:NFLAG_CLEAR;
    FLAG_Z=state.ccr&AMIGA_CCR_Z?ZFLAG_SET:ZFLAG_CLEAR;
    FLAG_V=state.ccr&AMIGA_CCR_V?VFLAG_SET:VFLAG_CLEAR;
    FLAG_C=state.ccr&AMIGA_CCR_C?CFLAG_SET:CFLAG_CLEAR;
    if (target) {
        if (effect.branch_taken) REG_PC=target;
        else USE_CYCLES(word_branch?CYC_BCC_NOTAKE_W:CYC_BCC_NOTAKE_B);
    }
    if (effect.returned) REG_PC=state.return_pc;
    USE_CYCLES(effect.movem_longs<<CYC_MOVEM_L);
    USE_CYCLES(CYC_INSTRUCTION[op]); return 1;
}
