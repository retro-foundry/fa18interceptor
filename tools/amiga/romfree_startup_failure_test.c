/* Original ADF startup-error unwind, without modifying guest bytes or PC.
 * Include the production loader for the standalone GNU fixture builder. */
#include "../../port/romfree/profile.c"
#include "recomp_ports.h"
#define REQUIRE(x) do { if (!(x)) { fprintf(stderr,"startup failure line %d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(int argc,char **argv) {
    FA18Machine *m=calloc(1,sizeof *m); FA18RomFreeProfile p={0}; char error[256];
    if (argc!=4 || (strcmp(argv[3],"dos") && strcmp(argv[3],"intuition"))) {
        fprintf(stderr,"usage: %s ORIGINAL.adf SAVE_DIR dos|intuition\n",argv[0]); return 2;
    }
    int workbench=!strcmp(argv[3],"intuition");
    REQUIRE(m && fa18_romfree_load(&p,m,argv[1],argv[2],1,error,sizeof error));
    fa18_ports_init(FA18_PORTS_OFF,NULL);
    uint32_t saved[15];
    for (unsigned i=0;i<15;++i) saved[i]=m68k_get_reg(NULL,(m68k_register_t)(M68K_REG_D0+i));
    uint32_t sp=m68k_get_reg(NULL,M68K_REG_A7);
    /* The second case permits DOS startup/Workbench message reception, then
     * denies the game's Intuition library allocation. Only host bookkeeping
     * and a normally constructed library are added, never captured OS state. */
    if (workbench) REQUIRE(amiga_host_library(p.compat,"dos.library",0));
    /* Occupy host-managed Fast RAM so the original DOS OpenLibrary fails.
     * The game, services, registers and loaded executable remain untouched. */
    unsigned allocations=0; uint32_t largest;
    while ((largest=amiga_host_available(p.compat,0x20004))!=0) {
        REQUIRE(++allocations<1024 && amiga_host_alloc(p.compat,largest,4));
    }
    unsigned frames=0;
    while (frames<10 && !p.compat->exited) { fa18_machine_run_frame(m); ++frames; }
    REQUIRE(p.compat->exited && p.compat->exit_code==(workbench?0:100));
    REQUIRE(p.compat->alert_count==(uint64_t)(workbench?0:1));
    REQUIRE(p.compat->last_alert==(workbench?0:0x00030007));
    if (workbench) {
        uint32_t message=amiga_be32(amiga_guest_range(&p.compat->memory,0xC07F70,4));
        /* This host-owned startup message has no reply port. Original
         * ReplyMsg marks it NT_FREEMSG (6), rather than queuing NT_REPLYMSG. */
        REQUIRE(message && *amiga_guest_range(&p.compat->memory,message+8,1)==6);
    }
    REQUIRE(m68k_get_reg(NULL,M68K_REG_PC)==0xFF446E && m68k_get_reg(NULL,M68K_REG_A7)==sp+4);
    for (unsigned i=1;i<15;++i) if (i!=7)
        REQUIRE(m68k_get_reg(NULL,(m68k_register_t)(M68K_REG_D0+i))==saved[i]);
    REQUIRE(!m->runtime_guard.rom_reads && !m->runtime_guard.rom_instruction_fetches && !m->runtime_guard.unsupported_services);
    REQUIRE(fa18_romfree_close(&p)); free(m);
    printf("Original startup failure: %u frame(s), %s allocation failure, %s, restored registers/stack, exit %d, zero ROM/fault counters\n",
           frames,argv[3],workbench?"Workbench message replied":"Alert",workbench?0:100);
    return 0;
}
