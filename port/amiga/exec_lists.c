#include "exec_lists.h"
#include "abi_13.h"
#include <string.h>

static void move_flags(AmigaExecListState *s,uint32_t v,uint32_t sign) {
    s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!v?AMIGA_CCR_Z:0)|
                    (v&sign?AMIGA_CCR_N:0));
}
static void write_move(const AmigaExecListBus *b,AmigaExecListState *s,
                       uint32_t address,uint32_t value) {
    b->write32(b->context,address,value); move_flags(s,value,0x80000000u);
}
int amiga_exec_list_step(AmigaExecListPhase p,AmigaExecListState *s,
                         const AmigaExecListBus *b,AmigaExecListEffect *e) {
    uint32_t v,old; uint8_t source,dest,result;
    if ((unsigned)p>=AMIGA_LIST_PHASE_COUNT || !s || !b || !e ||
        !b->read8 || !b->read32 || !b->write32) return 0;
    memset(e,0,sizeof *e);
    switch (p) {
    case AMIGA_LIST_INSERT_ANCHOR:
        s->d0=s->a2; move_flags(s,s->d0,0x80000000u); break;
    case AMIGA_LIST_INSERT_HEAD_BRANCH: case AMIGA_LIST_INSERT_TAIL_BRANCH:
    case AMIGA_LIST_REMHEAD_EMPTY_BRANCH: case AMIGA_LIST_REMTAIL_EMPTY_BRANCH:
    case AMIGA_LIST_ENQUEUE_TAIL_BRANCH:
        e->branch_taken=(s->ccr&AMIGA_CCR_Z)!=0; break;
    case AMIGA_LIST_INSERT_NEXT:
        s->d0=b->read32(b->context,s->a2); move_flags(s,s->d0,0x80000000u); break;
    case AMIGA_LIST_A0_FROM_D0: s->a0=s->d0; break;
    case AMIGA_LIST_INSERT_LINKS:
        b->write32(b->context,s->a1,s->d0);
        b->write32(b->context,s->a1+4,s->a2); e->movem_longs=2; break;
    case AMIGA_LIST_HEAD_LINKS:
        b->write32(b->context,s->a1,s->d0);
        b->write32(b->context,s->a1+4,s->a0); e->movem_longs=2; break;
    case AMIGA_LIST_NODE_TO_A0_PRED: case AMIGA_LIST_A1_TO_A0_PRED:
        write_move(b,s,s->a0+AMIGA_NODE_PRED,s->a1); break;
    case AMIGA_LIST_NODE_TO_ANCHOR: write_move(b,s,s->a2,s->a1); break;
    case AMIGA_LIST_RETURN:
        s->return_pc=b->read32(b->context,s->sp); s->sp+=4; e->returned=1; break;
    case AMIGA_LIST_ANCHOR_TO_NODE: write_move(b,s,s->a1,s->a2); break;
    case AMIGA_LIST_ANCHOR_PRED_TO_A0:
        s->a0=b->read32(b->context,s->a2+AMIGA_NODE_PRED); break;
    case AMIGA_LIST_A0_TO_NODE_PRED: write_move(b,s,s->a1+AMIGA_NODE_PRED,s->a0); break;
    case AMIGA_LIST_NODE_TO_ANCHOR_PRED:
        write_move(b,s,s->a2+AMIGA_NODE_PRED,s->a1); break;
    case AMIGA_LIST_NODE_TO_A0: write_move(b,s,s->a0,s->a1); break;
    case AMIGA_LIST_A0_NEXT_TO_D0:
        s->d0=b->read32(b->context,s->a0); move_flags(s,s->d0,0x80000000u); break;
    case AMIGA_LIST_A0_TO_TAIL: s->a0+=AMIGA_LIST_TAIL; break;
    case AMIGA_LIST_A0_PRED_TO_D0:
        s->d0=b->read32(b->context,s->a0+AMIGA_NODE_PRED);
        move_flags(s,s->d0,0x80000000u); break;
    case AMIGA_LIST_A0_TO_NODE: case AMIGA_LIST_NODE_TO_A0_FROM_A1:
        write_move(b,s,s->a1,s->a0); break;
    case AMIGA_LIST_D0_TO_NODE_PRED: write_move(b,s,s->a1+AMIGA_NODE_PRED,s->d0); break;
    case AMIGA_LIST_NODE_NEXT_TO_A0: s->a0=b->read32(b->context,s->a1); break;
    case AMIGA_LIST_NODE_PRED_TO_A1:
        s->a1=b->read32(b->context,s->a1+AMIGA_NODE_PRED); break;
    case AMIGA_LIST_HEAD_TO_A1: s->a1=b->read32(b->context,s->a0); break;
    case AMIGA_LIST_NODE_NEXT_TO_D0:
        s->d0=b->read32(b->context,s->a1); move_flags(s,s->d0,0x80000000u); break;
    case AMIGA_LIST_D0_TO_A0: write_move(b,s,s->a0,s->d0); break;
    case AMIGA_LIST_EXCHANGE_D0_A1: v=s->d0; s->d0=s->a1; s->a1=v; break;
    case AMIGA_LIST_TAIL_PRED_TO_A1:
        s->a1=b->read32(b->context,s->a0+AMIGA_LIST_TAIL_PRED); break;
    case AMIGA_LIST_NODE_PRED_TO_D0:
        s->d0=b->read32(b->context,s->a1+AMIGA_NODE_PRED);
        move_flags(s,s->d0,0x80000000u); break;
    case AMIGA_LIST_D0_TO_TAIL_PRED:
        write_move(b,s,s->a0+AMIGA_LIST_TAIL_PRED,s->d0); break;
    case AMIGA_LIST_ADD_TAIL_OFFSET:
        old=b->read32(b->context,s->a1); v=old+AMIGA_LIST_TAIL;
        s->ccr=(uint8_t)((!v?AMIGA_CCR_Z:0)|(v&0x80000000u?AMIGA_CCR_N:0)|
            ((~old&v&0x80000000u)?AMIGA_CCR_V:0)|
            (v<old?AMIGA_CCR_C|AMIGA_CCR_X:0));
        b->write32(b->context,s->a1,v); break;
    case AMIGA_LIST_NODE_PRIORITY_TO_D1:
        v=b->read8(b->context,s->a1+AMIGA_NODE_PRIORITY);
        s->d1=(s->d1&0xFFFFFF00u)|v; move_flags(s,v,0x80u); break;
    case AMIGA_LIST_COMPARE_PRIORITY:
        source=b->read8(b->context,s->a0+AMIGA_NODE_PRIORITY); dest=(uint8_t)s->d1;
        result=(uint8_t)(dest-source);
        s->ccr=(uint8_t)((s->ccr&AMIGA_CCR_X)|(!result?AMIGA_CCR_Z:0)|
            (result&0x80u?AMIGA_CCR_N:0)|((source^dest)&(result^dest)&0x80u?AMIGA_CCR_V:0)|
            (dest<source?AMIGA_CCR_C:0)); break;
    case AMIGA_LIST_ENQUEUE_NEXT_BRANCH:
        e->branch_taken=(s->ccr&AMIGA_CCR_Z)!=0 ||
            ((s->ccr&AMIGA_CCR_N)!=0)!=((s->ccr&AMIGA_CCR_V)!=0); break;
    default: return 0;
    }
    return 1;
}
