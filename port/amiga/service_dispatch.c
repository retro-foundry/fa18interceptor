#include "service_dispatch.h"
int amiga_services_step(const AmigaService *services,size_t count,uint32_t pc,
                        uint32_t caller,AmigaRuntimeGuard *guard) {
    if ((!services && count) || !guard) return -1;
    const AmigaService *match=NULL;
    for (size_t i=0;i<count;++i) {
        const AmigaService *s=&services[i];
        if (!s->enabled) continue;
        if (!s->step || !s->name || s->start>=s->end || s->end>0x1000000u ||
            s->entry<s->start || s->entry>=s->end) return -1;
        if (pc<s->start || pc>=s->end) continue;
        if (match) return -1;
        match=s;
    }
    if (!match) return 0;
    AmigaServiceContext previous=guard->service;
    if (previous.entry!=match->entry || pc==match->entry) guard->service.caller=caller;
    guard->service.entry=match->entry; guard->service.name=match->name;
    int handled=match->step(match->context);
    if (!handled) guard->service=previous;
    return handled==0 || handled==1?handled:-1;
}
