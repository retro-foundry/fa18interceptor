#ifndef AMIGA_ABI_13_H
#define AMIGA_ABI_13_H
/* Guest byte offsets, cross-checked against the SDK declarations and pinned
 * 1.3 implementations. Never sizeof(host structs) or SDK header dependencies.
 * Later SDK-only fields are deliberately absent. */
enum {
    AMIGA_NODE_SUCC=0, AMIGA_NODE_PRED=4, AMIGA_NODE_NAME=10,
    AMIGA_EXEC_THIS_TASK=0x114, AMIGA_EXEC_ID_NEST_CNT=0x126,
    AMIGA_EXEC_TD_NEST_CNT=0x127, AMIGA_EXEC_TASK_READY=0x196,
    AMIGA_EXEC_TASK_WAIT=0x1A4, AMIGA_EXEC_FIND_NAME_LVO=-276
};
#endif
