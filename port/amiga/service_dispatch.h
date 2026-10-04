#ifndef AMIGA_SERVICE_DISPATCH_H
#define AMIGA_SERVICE_DISPATCH_H
#include "runtime_guard.h"
/* One callback completes one resumable phase on the embedding machine's
 * timeline. Guest PC/stack retain continuations, interrupts and nested calls.
 * No interpreter or SDK types are part of this interface. */
typedef struct {
    uint32_t start,end,entry;
    const char *name;
    int enabled;
    int (*step)(void *context);
    void *context;
} AmigaService;
/* 1 handled; 0 no phase; -1 invalid/ambiguous registry. Unhandled targets must
 * reach the embedding runtime's unsupported-service guard before opcode fetch. */
int amiga_services_step(const AmigaService *,size_t,uint32_t pc,uint32_t caller,
                        AmigaRuntimeGuard *);
#endif
