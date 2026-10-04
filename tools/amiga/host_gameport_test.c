/* CPU/chipset adapter contract, with synthetic RAM only: no ADF, SDK, ROM,
 * savestate or game state. C16EAE consumes codes 68/E8 from a 22-byte event. */
#include "../../port/machine/startup.h"
#include "../../port/os/host_compat_adapter.h"
#include "../../port/os/service_dispatch_adapter.h"
#include "../../port/amiga/abi_13.h"
#include "../../port/amiga/hunk.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "m68kcpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"gameport contract failed line %d: %s\n",__LINE__,#x); return 1; } } while (0)
enum { REQUEST=0x1000,PORT=0x1100,TASK=0x1200,EVENT=0x1400,STACK=0x1600,RETURN=0x1800 };
static int call(unsigned lvo) {
    REG_PC=AMIGA_HOST_SERVICE_BASE+(lvo/6)*2; REG_A[1]=REQUEST; REG_A[7]=STACK;
    amiga_store_be32(fa18_machine->chip+STACK,RETURN); SET_CYCLES(1024);
    return fa18_services_step();
}
static int counted_service(void *context) { ++*(unsigned *)context; return 1; }
int main(void) {
    FA18Machine *m=calloc(1,sizeof *m); AmigaHostCompat *c=calloc(1,sizeof *c);
    FA18MachineStartup startup={0}; char error[256];
    REQUIRE(m && c); startup.pc=RETURN; startup.usp=STACK; startup.isp=0xC80000;
    fa18_recomp_init(0); fa18_ports_init(FA18_PORTS_OFF,NULL);
    REQUIRE(fa18_machine_init(m,&startup,error,sizeof error));
    AmigaGuestBank banks[]={{0,FA18_CHIP_SIZE,AMIGA_MEMORY_CHIP,m->chip},
                           {FA18_SLOW_BASE,FA18_SLOW_SIZE,AMIGA_MEMORY_FAST,m->slow}};
    AmigaGuestMemory memory={banks,2}; AmigaHostRegion reserved={0,0x4000,0};
    AmigaOfs empty_disk={0}; /* No file operations in this fixture. */
    REQUIRE(amiga_host_init(c,&memory,&empty_disk,"host-gameport-test-saves",&reserved,1));
    c->libraries[0]=0x2000;
    AmigaLibraryVector vector={30,0xFC0000};
    REQUIRE(fa18_os_host_compat_install(c,&vector,1));
    uint32_t device=amiga_host_library(c,"gameport.device",0); REQUIRE(device);
    uint8_t *io=m->chip+REQUEST,*port=m->chip+PORT;
    port[15]=3; amiga_store_be32(port+16,TASK);
    amiga_store_be32(port+AMIGA_PORT_MESSAGES,PORT+AMIGA_PORT_MESSAGES+4);
    amiga_store_be32(port+AMIGA_PORT_MESSAGES+8,PORT+AMIGA_PORT_MESSAGES);
    amiga_store_be32(io+14,PORT); amiga_store_be32(io+20,device); amiga_store_be32(io+24,1);
    amiga_store_be16(io+28,9); amiga_store_be32(io+36,22); amiga_store_be32(io+40,EVENT);
    c->gameport_type[1]=2; amiga_store_be16(c->gameport_trigger[1],3);
    REQUIRE(call(462) && REG_PC==RETURN && c->pending_count==1 && io[8]==5);
    fa18_os_host_tick(0); REQUIRE(c->pending_count==1);
    REQUIRE(call(474) && c->wait_kind==3 && (CPU_STOPPED&STOP_LEVEL_STOP));
    m->cycle=2*7093790+709379; fa18_machine_button(m,2,1);
    fa18_os_host_tick(m->cycle);
    REQUIRE(!c->pending_count && !c->wait_kind && !(CPU_STOPPED&STOP_LEVEL_STOP));
    REQUIRE(io[8]==7 && !io[31] && amiga_be32(io+32)==22);
    REQUIRE(amiga_be32(port+AMIGA_PORT_MESSAGES)==REQUEST);
    REQUIRE(amiga_be32(m->chip+TASK+AMIGA_TASK_SIGNALS_RECEIVED)==8);
    REQUIRE(amiga_be16(m->chip+EVENT+6)==0x68 && amiga_be32(m->chip+EVENT+14)==2 && amiga_be32(m->chip+EVENT+18)==100000);
    REQUIRE(call(474) && REG_PC==RETURN && REG_D[0]==0);
    REQUIRE(amiga_be32(port+AMIGA_PORT_MESSAGES)==PORT+AMIGA_PORT_MESSAGES+4);
    REQUIRE(call(462) && c->pending_count==1);
    fa18_machine_button(m,0,1); fa18_os_host_tick(m->cycle); /* unit 0 must not complete unit 1 */
    REQUIRE(c->pending_count==1);
    fa18_machine_button(m,2,0); fa18_os_host_tick(m->cycle);
    REQUIRE(!c->pending_count && amiga_be16(m->chip+EVENT+6)==0xE8);
    REQUIRE(call(474)); REQUIRE(call(462) && c->pending_count==1);
    REQUIRE(call(480) && !c->pending_count && io[8]==7 && (int8_t)io[31]==-2);
    REQUIRE(call(474) && (int32_t)REG_D[0]==-2);
    /* Registry bounds must follow arbitrary RAM services, replacements,
     * failed installations and reset, not assume OS services live in ROM. */
    unsigned calls=0;
    AmigaService extra={0x100,0x104,0x100,"fixture.ram",1,counted_service,&calls};
    REQUIRE(fa18_services_install_extra(&extra,1));
    REG_PC=0x100; REQUIRE(fa18_services_step()==1 && calls==1);
    REG_PC=0x102; REQUIRE(fa18_services_step()==1 && calls==2);
    REG_PC=0x104; REQUIRE(!fa18_services_step() && calls==2);
    extra.start=extra.entry=0xC00100; extra.end=0xC00104;
    REQUIRE(fa18_services_install_extra(&extra,1));
    REG_PC=0x100; REQUIRE(!fa18_services_step());
    REG_PC=0xC00100; REQUIRE(fa18_services_step()==1 && calls==3);
    extra.end=extra.start;
    REQUIRE(!fa18_services_install_extra(&extra,1));
    REQUIRE(fa18_services_step()==1 && calls==4); /* failed replacement is atomic */
    fa18_service_enable(FA18_SERVICE_EXEC_LISTS,1);
    REQUIRE(fa18_services_step()==1 && calls==5); /* enable/disable retains extra range */
    fa18_service_enable(FA18_SERVICE_EXEC_LISTS,0);
    REQUIRE(fa18_services_step()==1 && calls==6);
    fa18_services_reset(); REQUIRE(!fa18_services_step());
    REQUIRE(!m->runtime_guard.rom_reads && !m->runtime_guard.rom_instruction_fetches && !m->runtime_guard.unsupported_services);
    fa18_os_host_compat_detach(); REQUIRE(amiga_host_close(c)); free(c); free(m);
    puts("Gameport SendIO/WaitIO, button edges, reply/signals, unit isolation and AbortIO pass; zero ROM/fault counters"); return 0;
}
