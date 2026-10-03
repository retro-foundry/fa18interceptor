/* Test-only child contracts. Original parent instructions are unmodified.
 * Real-child and recorded body checks are separate proof layers. */
#include "glue.h"
#include "glue_child_call.h"
#include "globals.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { uint32_t entry,ret,regs[16],sr; } Boundary;
static Boundary boundaries[64];
static uint8_t ram[64][FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static unsigned scenario,ordinal,count;
static int source_phase;
static uint32_t scramble(uint32_t v) { v^=v<<13; v^=v>>17; v^=v<<5; return v; }
void flight_geometry_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void flight_geometry_contract_finish(void) {
    if(!source_phase && ordinal!=count) {
        fprintf(stderr,"flight geometry contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort();
    }
}
void flight_geometry_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc06c02,0xc28e24},{0xc28f16,0xc28f08},{0xc27456,0xc27184},{0xc27456,0xc27414}
    };
    Boundary boundary={0}; uint32_t value; unsigned i;
    if(ordinal>=64) { fprintf(stderr,"flight geometry contract case %u: too many children\n",scenario); abort(); }
    boundary.entry=entry; boundary.ret=ret;
    memcpy(boundary.regs,REG_DA,sizeof boundary.regs);
    boundary.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=boundary; ++count;
        memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&boundary,&boundaries[ordinal],sizeof boundary) ||
        memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"flight geometry child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",
            scenario,ordinal,entry,boundaries[ordinal].sr,boundary.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i])
            fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(entry==sites[i].entry && ret==sites[i].ret) break;
    if(i==sizeof sites/sizeof sites[0]) abort();
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    REG_A[0]=entry==0xc27456u?boundary.regs[8]:0xc60420u;
    REG_A[1]=0xc60820u; REG_A[2]=0xffff8123u; REG_A[3]=boundary.regs[11];
    REG_A[4]=boundary.regs[12]+4; REG_A[5]=0xc64200u;
    if(scenario&128) { value|=4u; wr_u16(REG_A[4],0xffff); } else value&=~4u;
    /* Explicit controlled RAM return: restart the first detail pass with an
     * exhausted polygon ring. This exercises the parent's velocity-clear arm.
     * The original face child is not claimed to publish these values. */
    if(ret==0xc27414u && (scenario&8192) && (scenario&31)==17 && !ordinal) {
        value|=4u; wr_u16(REG_A[6]-68,0xffff); wr_u16(0xc64000u,0xffff);
    }
    wr_u32(0xc65000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(value&31u));
    /* Return changed registers and CCR, preserving the actual caller frame.
     * Parent input comparisons retain all child-entry RAM, including PEA. */
    REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,
    uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited;
    m68ki_push_32(ret); REG_PC=entry; flight_geometry_contract_enter(entry,ret);
    return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) {
    return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL);
}
