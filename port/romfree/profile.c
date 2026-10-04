/* Interceptor-specific placement and process profile. Reusable disk loading,
 * relocation, guest structures and service semantics live in port/amiga.
 * The compiled manifest contains verified addresses/sizes/identifiers only. */
#include "profile.h"
#include "placement.h"
#include "../amiga/hunk_loader.h"
#include "../amiga/exec_bootstrap.h"
#include "../os/service_dispatch_adapter.h"
#include "recomp_runtime.h"
#include "m68kcpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int fail(char *error,size_t size,const char *why) {
    if (error && size) snprintf(error,size,"ROM-free launch: %s",why); return 0;
}
static int pending_wrapper(void *context) {
    (void)context;
    amiga_runtime_guard_unsupported(&fa18_machine->runtime_guard,REG_PPC,REG_PC,(uint64_t)fa18_machine_now());
    fa18_machine_runtime_fault(); return 0;
}
static const AmigaService pending_ram_services[]={
    {0xC06550,0xC06552,0xC06550,"exec.AllocMem.wrapper",1,pending_wrapper,NULL},
    {0xC0655A,0xC0655C,0xC0655A,"exec.OpenLibrary.wrapper",1,pending_wrapper,NULL},
    {0xC06564,0xC06566,0xC06564,"exec.OpenDevice.wrapper",1,pending_wrapper,NULL},
    {0xC0656E,0xC06570,0xC0656E,"exec.CloseLibrary.wrapper",1,pending_wrapper,NULL},
    {0xC06578,0xC0657A,0xC06578,"exec.CloseDevice.wrapper",1,pending_wrapper,NULL},
    {0xC06582,0xC06584,0xC06582,"exec.RemLibrary.wrapper",1,pending_wrapper,NULL},
    {0xC0658C,0xC0658E,0xC0658C,"exec.RemDevice.wrapper",1,pending_wrapper,NULL}
};
void fa18_romfree_close(FA18RomFreeProfile *p) {
    if (p) { amiga_hunks_free(&p->image); amiga_ofs_close(&p->adf); memset(p,0,sizeof *p); }
}
int fa18_romfree_load(FA18RomFreeProfile *p,FA18Machine *m,const char *adf_path,
                      const char *save_directory,int use_recomp,char *error,size_t error_size) {
    if (!p || !m || !adf_path || !save_directory || !*save_directory)
        return fail(error,error_size,"missing ADF, save directory or destination");
    *p=(FA18RomFreeProfile){0};
    if (!amiga_ofs_open(&p->adf,adf_path)) return fail(error,error_size,"cannot open original OFS ADF");
    size_t size=0; uint8_t *exe=amiga_ofs_read(&p->adf,"F-18 Interceptor",&size);
    uint32_t signature=2166136261u;
    for (size_t i=0;exe && i<size;++i) signature=(signature^exe[i])*16777619u;
    if (!exe || size!=331232 || signature!=0xE1811C45u) {
        free(exe); fa18_romfree_close(p); return fail(error,error_size,"ADF executable does not match the verified game version");
    }
    int parsed=amiga_hunks_parse(&p->image,exe,size); free(exe);
    if (!parsed || p->image.count!=sizeof fa18_placements/sizeof fa18_placements[0]) {
        fa18_romfree_close(p); return fail(error,error_size,"invalid original Hunk image or placement count");
    }
    FA18MachineStartup startup={0};
    startup.pc=fa18_placements[0].payload_base; startup.usp=0xC550DC; startup.isp=0xC80000;
    /* No desktop, boot-screen Copper list or unimplemented device driver is
     * synthesized. These devices remain inactive until their startup/services
     * are implemented. Full cold-launch behavior is not yet accepted. */
    if (!fa18_machine_init(m,&startup,error,error_size)) { fa18_romfree_close(p); return 0; }
    AmigaGuestBank banks[]={{0,FA18_CHIP_SIZE,AMIGA_MEMORY_CHIP,m->chip},
        {FA18_SLOW_BASE,FA18_SLOW_SIZE,AMIGA_MEMORY_FAST,m->slow}};
    AmigaGuestMemory memory={banks,2};
    /* The process begins exactly after the final disk segment allocation.
     * This profile's other owned OS region is below the first Slow-RAM hunk. */
    for (unsigned i=0;i<p->image.count;++i) {
        uint32_t begin=fa18_placements[i].payload_base-8,end=begin+fa18_placements[i].allocation_size;
        if ((begin<0xC004C2 && end>0xC00000) || (begin<0xC55104 && end>0xC54028)) {
            fa18_romfree_close(p); return fail(error,error_size,"game placement overlaps initial OS/process storage");
        }
    }
    if (!amiga_hunks_install(&p->image,fa18_placements,p->image.count,&memory,&p->segment_list,error,error_size)) {
        fa18_romfree_close(p); return 0;
    }
    AmigaExecBootstrap process={
        .exec_base=0xC00276,.task=0xC54028,.stack_lower=0xC540E4,.stack_upper=0xC550E4,
        .initial_sp=0xC550DC,.return_pc=0xFF446E,
        .signal_allocated=0xFFFF,.trap_allocated=0x80000000,.task_name_address=0xC550E4,
        .negative_size=0x276,.positive_size=0x24C,.version=34,.revision=2,.process_size=0xBC,
        .process_signal_bit=8,.task_name="F-18 Interceptor",
        .vectors=fa18_exec_vectors,.vector_count=sizeof fa18_exec_vectors/sizeof fa18_exec_vectors[0]
    };
    if (!amiga_exec_bootstrap(&memory,&process,error,error_size)) { fa18_romfree_close(p); return 0; }
    fa18_recomp_init(use_recomp);
    FA18RecompCodeRange code[sizeof fa18_placements/sizeof fa18_placements[0]];
    size_t code_count=0;
    for (unsigned i=0;i<p->image.count;++i)
        if (p->image.segments[i].kind!=AMIGA_HUNK_BSS && p->image.segments[i].size)
            code[code_count++]=(FA18RecompCodeRange){fa18_placements[i].payload_base,
                fa18_placements[i].payload_base+p->image.segments[i].size};
    if (!fa18_recomp_restrict_code(code,code_count)) {
        fa18_romfree_close(p); return fail(error,error_size,"cannot restrict translations to original executable storage");
    }
    /* Only the already proved implementations. No signature/opcode/operand
     * reads from ROM are performed to enable them in a clean profile. */
    for (unsigned i=0;i<FA18_SERVICE_COUNT;++i) fa18_service_enable(i,1);
    if (!fa18_services_install_extra(pending_ram_services,sizeof pending_ram_services/sizeof pending_ram_services[0]) ||
        !fa18_machine_prepare_run(m,error,error_size)) {
        fa18_romfree_close(p); return fail(error,error_size,"cannot install service profile or prepare machine DMA");
    }
    p->save_directory=save_directory;
    return 1;
}
