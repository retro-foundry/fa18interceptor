/* Reusable Exec Alert ABI contract: synthetic RAM, no game or SDK data. */
#include "../../port/machine/startup.h"
#include "../../port/os/host_compat_adapter.h"
#include "../../port/os/service_dispatch_adapter.h"
#include "../../port/amiga/hunk.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "m68kcpu.h"
#include <stdio.h>
#include <stdlib.h>
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"alert contract line %d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(void) {
    FA18Machine *m=calloc(1,sizeof *m); AmigaHostCompat *c=calloc(1,sizeof *c);
    FA18MachineStartup startup={0}; char error[256];
    REQUIRE(m && c); startup.pc=0x1800; startup.usp=0x1600; startup.isp=0xC80000;
    fa18_recomp_init(0); fa18_ports_init(FA18_PORTS_OFF,NULL);
    REQUIRE(fa18_machine_init(m,&startup,error,sizeof error));
    AmigaGuestBank banks[]={{0,FA18_CHIP_SIZE,AMIGA_MEMORY_CHIP,m->chip},
                           {FA18_SLOW_BASE,FA18_SLOW_SIZE,AMIGA_MEMORY_FAST,m->slow}};
    AmigaGuestMemory memory={banks,2}; AmigaHostRegion reserved={0,0x4000,0};
    AmigaOfs empty_disk={0};
    REQUIRE(amiga_host_init(c,&memory,&empty_disk,"host-alert-test-saves",&reserved,1));
    c->libraries[0]=0x2000; AmigaLibraryVector vector={108,0xFC0000};
    REQUIRE(fa18_os_host_compat_install(c,&vector,1));
    for (unsigned i=0;i<16;++i) REG_DA[i]=0x10203040u+i;
    REG_A[7]=0x1600; amiga_store_be32(m->chip+0x1600,0x1800);
    REG_D[7]=0x00030007; REG_PC=AMIGA_HOST_SERVICE_BASE+36; SET_CYCLES(1024);
    uint16_t sr=m68ki_get_sr();
    REQUIRE(fa18_services_step());
    REQUIRE(REG_PC==0x1800 && REG_A[7]==0x1604 && m68ki_get_sr()==sr);
    for (unsigned i=0;i<15;++i) REQUIRE(REG_DA[i]==(i==7?0x00030007u:0x10203040u+i));
    REQUIRE(!c->exited && c->alert_count==1 && c->last_alert==0x00030007);
    REG_A[7]=0x1600; REG_D[7]=0x80030007; REG_PC=AMIGA_HOST_SERVICE_BASE+36; SET_CYCLES(1024);
    REQUIRE(fa18_services_step());
    REQUIRE(c->exited && c->exit_code==100 && c->alert_count==2 && c->last_alert==0x80030007);
    REQUIRE(REG_A[7]==0x1600 && (CPU_STOPPED&STOP_LEVEL_STOP));
    REQUIRE(!m->runtime_guard.rom_reads && !m->runtime_guard.rom_instruction_fetches && !m->runtime_guard.unsupported_services);
    fa18_os_host_compat_detach(); REQUIRE(amiga_host_close(c)); free(c); free(m);
    puts("Exec Alert: D7 ABI, recoverable return, guest state, dead-end termination and zero ROM/fault counters pass");
    return 0;
}
