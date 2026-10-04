#include <assert.h>
#include <string.h>
#include "runtime_guard.h"
int main(void) {
    AmigaRuntimeGuard g={0},before;
    AmigaForbiddenRange ranges[]={{0xF80000u,0x1000000u},{0xF00000u,0xF10000u}};
    assert(amiga_runtime_guard_init(&g,ranges,2));
    g.service=(AmigaServiceContext){0x1000,0xFC1428,"Disable"};
    assert(amiga_runtime_guard_read(&g,0x1000,0x2000,4,100));
    assert(amiga_runtime_guard_fetch(&g,0x1000,104));
    assert(!amiga_runtime_guard_read(&g,0x1000,0xF7FFFF,2,108));
    assert(g.rom_reads==1 && !g.rom_instruction_fetches && !g.unsupported_services);
    assert(g.fault.kind==AMIGA_RUNTIME_ROM_READ && g.fault.target==0xF7FFFF &&
           g.fault.cycle==108 && g.fault.service.caller==0x1000);
    assert(!amiga_runtime_guard_read(&g,0x1000,0x2000,2,112)); /* Fault remains fatal. */
    before=g; ranges[0].high=0x1000001;
    assert(!amiga_runtime_guard_init(&g,ranges,2) && !memcmp(&g,&before,sizeof g));
    ranges[0].high=0x1000000;
    assert(amiga_runtime_guard_init(&g,ranges,2));
    assert(!amiga_runtime_guard_fetch(&g,0xFC1428,200));
    assert(g.rom_instruction_fetches==1 && !g.rom_reads);
    assert(amiga_runtime_guard_init(&g,ranges,2));
    assert(!amiga_runtime_guard_unsupported(&g,0x1000,0xFC0000,300));
    assert(g.unsupported_services==1 && !g.rom_reads && !g.rom_instruction_fetches);
    assert(g.fault.kind==AMIGA_RUNTIME_UNSUPPORTED_SERVICE);
    assert(amiga_runtime_guard_init(&g,ranges,2));
    assert(!amiga_runtime_guard_read(&g,0x1000,0xF00000,1,400)); /* Expansion boot ROM. */
    assert(g.rom_reads==1);
    return 0;
}
