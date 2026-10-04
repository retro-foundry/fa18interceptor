#ifndef AMIGA_EXEC_CONTEXT_H
#define AMIGA_EXEC_CONTEXT_H
#include "exec_task_services.h"
/* 68000 task-context transfers. Normal-order masks (D0..A7), explicit guest
 * base register, and ordered bus operations; status/stack banks stay CPU-owned. */
int amiga_exec_context_save(AmigaExecTaskState *,const AmigaExecTaskBus *,
                            unsigned base,unsigned mask,unsigned *transferred);
int amiga_exec_context_restore(AmigaExecTaskState *,const AmigaExecTaskBus *,
                               unsigned base,unsigned mask,int postincrement,
                               unsigned *transferred);
#endif
