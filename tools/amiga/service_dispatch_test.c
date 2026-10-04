#include <assert.h>
#include <string.h>
#include "service_dispatch.h"
typedef struct {
    uint32_t pc;
    unsigned events[8],count;
} Timeline;
static int outer(void *context) {
    Timeline *t=context;
    if (t->pc==0x2000) { t->events[t->count++]=1; t->pc=0x3000; return 1; }
    if (t->pc==0x2002) { t->events[t->count++]=4; t->pc=0x1000; return 1; }
    return 0;
}
static int inner(void *context) {
    Timeline *t=context; t->events[t->count++]=2; t->pc=0x2002; return 1;
}
static int interrupt(void *context) {
    Timeline *t=context; t->events[t->count++]=3; t->pc=0x2002; return 1;
}
int main(void) {
    Timeline t={0x2000,{0},0};
    AmigaRuntimeGuard g={0};
    const AmigaForbiddenRange range={0xF80000,0x1000000};
    assert(amiga_runtime_guard_init(&g,&range,1));
    AmigaService services[]={
        {0x2000,0x2006,0x2000,"outer",1,outer,&t},
        {0x3000,0x3002,0x3000,"inner",1,inner,&t},
        {0x4000,0x4002,0x4000,"interrupt",1,interrupt,&t}
    };
    assert(amiga_services_step(services,3,t.pc,0x1800,&g)==1);
    assert(t.pc==0x3000 && g.service.caller==0x1800 && g.service.entry==0x2000);
    assert(amiga_services_step(services,3,t.pc,0x2000,&g)==1);
    /* A machine event can run a guest interrupt between service phases. */
    t.pc=0x4000;
    assert(amiga_services_step(services,3,t.pc,0x2002,&g)==1);
    assert(amiga_services_step(services,3,t.pc,0x4000,&g)==1);
    const unsigned expected[]={1,2,3,4};
    assert(t.pc==0x1000 && t.count==4 && !memcmp(t.events,expected,sizeof expected));
    AmigaServiceContext before=g.service;
    t.pc=0x2004; /* Sparse phases restore the context on rejection. */
    assert(!amiga_services_step(services,3,t.pc,0x1000,&g) && t.count==4);
    assert(g.service.entry==before.entry && g.service.caller==before.caller);
    services[0].enabled=0; t.pc=0x2000;
    assert(!amiga_services_step(services,3,t.pc,0x1000,&g) && t.count==4);
    services[0].enabled=1; services[1]=services[0];
    assert(amiga_services_step(services,3,t.pc,0x1000,&g)==-1 && t.count==4);
    services[1].enabled=0; services[2].end=0x1000001;
    assert(amiga_services_step(services,3,t.pc,0x1000,&g)==-1 && t.count==4);
    return 0;
}
