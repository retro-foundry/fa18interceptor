#ifndef AMIGA_EXEC_LISTS_H
#define AMIGA_EXEC_LISTS_H
#include <stdint.h>
/* Original Exec list phases. Embedders provide a register view and ordered
 * guest bus operations. No host list pointers, CPU core or ROM is required. */
typedef enum {
    AMIGA_LIST_INSERT_ANCHOR, AMIGA_LIST_INSERT_HEAD_BRANCH,
    AMIGA_LIST_INSERT_NEXT, AMIGA_LIST_INSERT_TAIL_BRANCH,
    AMIGA_LIST_A0_FROM_D0, AMIGA_LIST_INSERT_LINKS,
    AMIGA_LIST_NODE_TO_A0_PRED, AMIGA_LIST_NODE_TO_ANCHOR,
    AMIGA_LIST_RETURN, AMIGA_LIST_ANCHOR_TO_NODE,
    AMIGA_LIST_ANCHOR_PRED_TO_A0, AMIGA_LIST_A0_TO_NODE_PRED,
    AMIGA_LIST_NODE_TO_ANCHOR_PRED, AMIGA_LIST_NODE_TO_A0,
    AMIGA_LIST_A0_NEXT_TO_D0, AMIGA_LIST_HEAD_LINKS,
    AMIGA_LIST_A0_TO_TAIL, AMIGA_LIST_A0_PRED_TO_D0,
    AMIGA_LIST_A0_TO_NODE, AMIGA_LIST_D0_TO_NODE_PRED,
    AMIGA_LIST_NODE_NEXT_TO_A0, AMIGA_LIST_NODE_PRED_TO_A1,
    AMIGA_LIST_NODE_TO_A0_FROM_A1, AMIGA_LIST_A1_TO_A0_PRED,
    AMIGA_LIST_HEAD_TO_A1, AMIGA_LIST_NODE_NEXT_TO_D0,
    AMIGA_LIST_REMHEAD_EMPTY_BRANCH, AMIGA_LIST_D0_TO_A0,
    AMIGA_LIST_EXCHANGE_D0_A1, AMIGA_LIST_TAIL_PRED_TO_A1,
    AMIGA_LIST_NODE_PRED_TO_D0, AMIGA_LIST_REMTAIL_EMPTY_BRANCH,
    AMIGA_LIST_D0_TO_TAIL_PRED, AMIGA_LIST_ADD_TAIL_OFFSET,
    AMIGA_LIST_NODE_PRIORITY_TO_D1, AMIGA_LIST_ENQUEUE_TAIL_BRANCH,
    AMIGA_LIST_COMPARE_PRIORITY, AMIGA_LIST_ENQUEUE_NEXT_BRANCH,
    AMIGA_LIST_PHASE_COUNT
} AmigaExecListPhase;
enum { AMIGA_CCR_C=1, AMIGA_CCR_V=2, AMIGA_CCR_Z=4,
       AMIGA_CCR_N=8, AMIGA_CCR_X=16 };
typedef struct {
    uint32_t d0,d1,a0,a1,a2,sp,return_pc;
    uint8_t ccr;
} AmigaExecListState;
typedef struct {
    void *context;
    uint8_t (*read8)(void *,uint32_t);
    uint32_t (*read32)(void *,uint32_t);
    void (*write32)(void *,uint32_t,uint32_t);
} AmigaExecListBus;
typedef struct {
    int branch_taken,returned;
    unsigned movem_longs;
} AmigaExecListEffect;
/* Caller consumes phase-specific instruction/extension timing before this
 * step and branch/MOVEM timing afterwards. Interrupts may occur between steps.
 * Invalid arguments return zero without running any guest bus callbacks. */
int amiga_exec_list_step(AmigaExecListPhase phase,AmigaExecListState *state,
                         const AmigaExecListBus *bus,AmigaExecListEffect *effect);
#endif
