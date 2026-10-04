#ifndef AMIGA_RUNTIME_GUARD_H
#define AMIGA_RUNTIME_GUARD_H
#include <stddef.h>
#include <stdint.h>

/* A CPU/bus adapter checks real memory accesses, never virtual service timing.
 * The embedding runtime decides how to terminate and report a failed check. */
typedef struct { uint32_t low, high; } AmigaForbiddenRange;
typedef enum {
    AMIGA_RUNTIME_NO_FAULT, AMIGA_RUNTIME_ROM_READ,
    AMIGA_RUNTIME_ROM_FETCH, AMIGA_RUNTIME_UNSUPPORTED_SERVICE
} AmigaRuntimeFaultKind;
typedef struct {
    uint32_t caller, entry;
    const char *name; /* Borrowed, must outlive this context. */
} AmigaServiceContext;
typedef struct {
    AmigaRuntimeFaultKind kind;
    uint32_t pc, target;
    uint64_t cycle;
    AmigaServiceContext service;
} AmigaRuntimeFault;
typedef struct {
    AmigaForbiddenRange ranges[8];
    size_t range_count;
    int enabled;
    uint64_t rom_reads, rom_instruction_fetches, unsupported_services;
    AmigaServiceContext service;
    AmigaRuntimeFault fault;
} AmigaRuntimeGuard;

/* Initialization is atomic; invalid ranges leave the guard unchanged. */
int amiga_runtime_guard_init(AmigaRuntimeGuard *, const AmigaForbiddenRange *, size_t);
int amiga_runtime_guard_contains(const AmigaRuntimeGuard *, uint32_t address, unsigned size);
int amiga_runtime_guard_read(AmigaRuntimeGuard *, uint32_t pc, uint32_t address,
                             unsigned size, uint64_t cycle);
int amiga_runtime_guard_fetch(AmigaRuntimeGuard *, uint32_t pc, uint64_t cycle);
int amiga_runtime_guard_unsupported(AmigaRuntimeGuard *, uint32_t pc,
                                    uint32_t target, uint64_t cycle);
#endif
