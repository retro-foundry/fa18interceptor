/* Reference-only body exports at actual instruction boundaries. Replace
 * port/machine/bus.c in a diagnostic build. All memory reads use storage;
 * original bus operations, clocks, registers and controls are unchanged. */
#define fa18_bus_begin original_bus_begin
#include "../../port/machine/bus.c"
#undef fa18_bus_begin
#include "../../port/recomp/loop_input.h"

static void export_frame_body(const char *prefix,const char *suffix) {
    char path[4096];
    FILE *out;
    if(snprintf(path,sizeof path,"%s.%s.dat",prefix,suffix)>=(int)sizeof path) abort();
    out=fopen(path,"wb");
    if(!out || fwrite(fa18_machine->chip,1,FA18_CHIP_SIZE,out)!=FA18_CHIP_SIZE ||
       fwrite(fa18_machine->slow,1,FA18_SLOW_SIZE,out)!=FA18_SLOW_SIZE || fclose(out)) abort();
    if(snprintf(path,sizeof path,"%s.%s.json",prefix,suffix)>=(int)sizeof path) abort();
    out=fopen(path,"w");
    if(!out) abort();
    if(fprintf(out,"{\"iteration\":%ld,\"frame\":%llu,\"pc\":%u,\"ppc\":%u,\"registers\":[",
        fa18_loop_iterations(),(unsigned long long)fa18_machine->frame,REG_PC,REG_PPC)<0) abort();
    for(unsigned i=0;i<16;++i) if(fprintf(out,"%s%u",i?",":"",REG_DA[i])<0) abort();
    if(fprintf(out,"],\"sr\":%u}\n",m68k_get_reg(NULL,M68K_REG_SR))<0 || fclose(out)) abort();
}

void fa18_bus_begin(uint32_t pc) {
    static int initialized;
    static long target;
    static long count=1,last_iteration;
    static uint32_t owner,owner_return;
    static const char *prefix;
    static unsigned exported;
    if(!initialized) {
        const char *at=getenv("FA18_ORIGINAL_BODY_ITERATION");
        prefix=getenv("FA18_ORIGINAL_BODY_PREFIX");
        if(!at || !prefix || !(target=strtol(at,NULL,10))) abort();
        const char *range=getenv("FA18_ORIGINAL_BODY_COUNT");
        if(range) count=strtol(range,NULL,10);
        if(count<1 || count>128) abort();
        const char *entry=getenv("FA18_ORIGINAL_BODY_OWNER");
        const char *after=getenv("FA18_ORIGINAL_BODY_OWNER_RETURN");
        if(!!entry!=!!after) abort();
        if(entry) {owner=(uint32_t)strtoul(entry,NULL,16);owner_return=(uint32_t)strtoul(after,NULL,16);}
        initialized=1;
    }
    const long iteration=fa18_loop_iterations();
    if(iteration>=target && iteration-target<count) {
        char range_prefix[4096];
        const char *destination=prefix;
        if(iteration!=last_iteration) {exported=0;last_iteration=iteration;}
        if(count>1) {
            if(snprintf(range_prefix,sizeof range_prefix,"%s.%ld",prefix,iteration)>=(int)sizeof range_prefix) abort();
            destination=range_prefix;
        }
        if(pc==0xc0efeau && !(exported&1)) {export_frame_body(destination,"before");exported|=1;}
        if(pc==0xc0f3c0u && !(exported&2)) {export_frame_body(destination,"after");exported|=2;}
        if(owner && pc==owner && !(exported&4)) {export_frame_body(destination,"owner");exported|=4;}
        if(owner_return && pc==owner_return && !(exported&8)) {export_frame_body(destination,"owner-after");exported|=8;}
    }
    original_bus_begin(pc);
}
