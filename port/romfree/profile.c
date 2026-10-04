/* Interceptor-specific placement and process profile. Reusable disk loading,
 * relocation, guest structures and service semantics live in port/amiga.
 * The compiled manifest contains verified addresses/sizes/identifiers only. */
#include "profile.h"
#include "placement.h"
#include "../amiga/hunk_loader.h"
#include "../amiga/exec_bootstrap.h"
#include "../amiga/abi_13.h"
#include "../os/service_dispatch_adapter.h"
#include "../os/host_compat_adapter.h"
#include "recomp_runtime.h"
#include "m68kcpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int fail(char *error,size_t size,const char *why) {
    if (error && size) snprintf(error,size,"ROM-free launch: %s",why); return 0;
}
void fa18_romfree_close(FA18RomFreeProfile *p) {
    if (p) {
        fa18_os_host_compat_detach(); amiga_host_close(p->compat); free(p->compat);
        amiga_hunks_free(&p->image); amiga_ofs_close(&p->adf); memset(p,0,sizeof *p);
    }
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
    p->compat=calloc(1,sizeof *p->compat);
    AmigaHostRegion reserved[sizeof fa18_placements/sizeof fa18_placements[0]+4];
    size_t reserved_count=0;
    reserved[reserved_count++]=(AmigaHostRegion){0,0x1000,0};
    reserved[reserved_count++]=(AmigaHostRegion){0xC00000,0x4C2,0};
    reserved[reserved_count++]=(AmigaHostRegion){0xC54028,0x10DC,0};
    reserved[reserved_count++]=(AmigaHostRegion){0xC7E000,0x2000,0};
    for (unsigned i=0;i<p->image.count;++i)
        reserved[reserved_count++]=(AmigaHostRegion){fa18_placements[i].payload_base-8,fa18_placements[i].allocation_size,0};
    if (!p->compat || !amiga_host_init(p->compat,&memory,&p->adf,save_directory,reserved,reserved_count)) {
        fa18_romfree_close(p); return fail(error,error_size,"cannot initialize host compatibility memory");
    }
    p->compat->libraries[AMIGA_HOST_EXEC]=process.exec_base;
    /* Non-CLI process handoff with one executable argument. The embedding
     * host owns this process, so no desktop reply port is required. */
    uint32_t message=amiga_host_alloc(p->compat,48,0x10004),argument=message+40;
    uint8_t *msg=amiga_guest_range(&memory,message,48);
    uint32_t list=process.task+AMIGA_PROCESS_MSG_PORT+AMIGA_PORT_MESSAGES;
    if (!message || !msg) { fa18_romfree_close(p); return fail(error,error_size,"cannot allocate startup message"); }
    amiga_store_be32(msg,list+4); amiga_store_be32(msg+4,list); msg[8]=5; msg[19]=40;
    amiga_store_be32(msg+20,process.task); amiga_store_be32(msg+24,p->segment_list);
    amiga_store_be32(msg+28,1); amiga_store_be32(msg+36,argument);
    amiga_store_be32(msg+40,amiga_host_lock(p->compat,"")); amiga_store_be32(msg+44,process.task_name_address);
    amiga_store_be32(amiga_guest_range(&memory,list,4),message);
    amiga_store_be32(amiga_guest_range(&memory,list+8,4),message);
    amiga_store_be32(amiga_guest_range(&memory,process.task+AMIGA_TASK_SIGNALS_RECEIVED,4),0x100);
    /* Explicit server lists and CPU exception/IRQ identifiers. Only pointers
     * and empty packed structures are installed, never captured OS code/data. */
    for (unsigned vector=2;vector<64;++vector)
        amiga_store_be32(amiga_guest_range(&memory,vector*4,4),0xEF4000+vector*2);
    amiga_store_be32(amiga_guest_range(&memory,0x20,4),0xFC090E);
    static const uint32_t irq_roots[]={0xFC0C8E,0xFC0CE2,0xFC0D14,0xFC0D6C,0xFC0DFA,0xFC0E40,0xFC0E86};
    for (unsigned level=0;level<7;++level)
        amiga_store_be32(amiga_guest_range(&memory,0x64+level*4,4),irq_roots[level]);
    uint32_t interrupt_lists=amiga_host_alloc(p->compat,16*24,0x10004);
    if (!interrupt_lists) { fa18_romfree_close(p); return fail(error,error_size,"cannot initialize interrupt server lists"); }
    for (unsigned bit=0;bit<16;++bit) {
        uint32_t head=interrupt_lists+bit*24;
        uint8_t *h=amiga_guest_range(&memory,head,24);
        amiga_store_be32(h,head+4); amiga_store_be32(h+8,head);
        h[18]=(uint8_t)((1u<<bit)>>8); h[19]=(uint8_t)(1u<<bit);
        uint8_t *v=amiga_guest_range(&memory,process.exec_base+AMIGA_EXEC_INT_VECTORS+bit*12,12);
        amiga_store_be32(v,head); amiga_store_be32(v+4,bit==2?0xFC13BC:0xFC1338);
    }
    if (!fa18_os_host_compat_install(p->compat,fa18_exec_vectors,sizeof fa18_exec_vectors/sizeof fa18_exec_vectors[0]) ||
        !fa18_machine_prepare_run(m,error,error_size)) {
        fa18_romfree_close(p); return fail(error,error_size,"cannot install service profile or prepare machine DMA");
    }
    p->save_directory=save_directory;
    return 1;
}
