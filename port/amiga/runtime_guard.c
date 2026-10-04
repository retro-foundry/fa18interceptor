#include "runtime_guard.h"
#include <string.h>

int amiga_runtime_guard_init(AmigaRuntimeGuard *g, const AmigaForbiddenRange *ranges, size_t count) {
    if (!g || !ranges || !count || count>sizeof g->ranges/sizeof g->ranges[0]) return 0;
    for (size_t i=0; i<count; ++i)
        if (ranges[i].low>=ranges[i].high || ranges[i].high>0x1000000u) return 0;
    AmigaRuntimeGuard next={0};
    memcpy(next.ranges,ranges,count*sizeof *ranges);
    next.range_count=count; next.enabled=1; *g=next;
    return 1;
}
int amiga_runtime_guard_contains(const AmigaRuntimeGuard *g, uint32_t address, unsigned size) {
    /* The CPU adapter supplies an already masked 24-bit bus address. */
    if (!g || !size) return 0;
    uint64_t end=(uint64_t)address+size;
    for (size_t i=0; i<g->range_count; ++i)
        if (address<g->ranges[i].high && end>g->ranges[i].low) return 1;
    return 0;
}
static int fault(AmigaRuntimeGuard *g, AmigaRuntimeFaultKind kind, uint32_t pc,
                 uint32_t target, uint64_t cycle) {
    if (g->fault.kind==AMIGA_RUNTIME_NO_FAULT) {
        g->fault.kind=kind; g->fault.pc=pc; g->fault.target=target;
        g->fault.cycle=cycle; g->fault.service=g->service;
    }
    return 0;
}
int amiga_runtime_guard_read(AmigaRuntimeGuard *g, uint32_t pc, uint32_t address,
                             unsigned size, uint64_t cycle) {
    if (!g) return 0;
    if (!g->enabled) return 1;
    if (!size || address>0xFFFFFFu) return 0;
    if (!amiga_runtime_guard_contains(g,address,size)) return g->fault.kind==AMIGA_RUNTIME_NO_FAULT;
    if (g->rom_reads!=UINT64_MAX) ++g->rom_reads;
    return fault(g,AMIGA_RUNTIME_ROM_READ,pc,address,cycle);
}
int amiga_runtime_guard_fetch(AmigaRuntimeGuard *g, uint32_t pc, uint64_t cycle) {
    if (!g) return 0;
    if (!g->enabled) return 1;
    if (!amiga_runtime_guard_contains(g,pc,2)) return g->fault.kind==AMIGA_RUNTIME_NO_FAULT;
    if (g->rom_instruction_fetches!=UINT64_MAX) ++g->rom_instruction_fetches;
    return fault(g,AMIGA_RUNTIME_ROM_FETCH,pc,pc,cycle);
}
int amiga_runtime_guard_unsupported(AmigaRuntimeGuard *g, uint32_t pc,
                                    uint32_t target, uint64_t cycle) {
    if (!g) return 0;
    if (!g->enabled) return 1;
    if (g->unsupported_services!=UINT64_MAX) ++g->unsupported_services;
    return fault(g,AMIGA_RUNTIME_UNSUPPORTED_SERVICE,pc,target,cycle);
}
