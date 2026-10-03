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
extern void flight_record_actions_fault_boundary_observed(void);
static uint32_t scramble(uint32_t v) { v^=v<<13; v^=v>>17; v^=v<<5; return v; }
void flight_record_actions_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void flight_record_actions_contract_finish(void) {
    if(!source_phase && ordinal!=count) {
        fprintf(stderr,"flight record actions contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort();
    }
}
void flight_record_actions_contract_enter(uint32_t entry,uint32_t ret) {
    Boundary boundary={0}; uint32_t value; unsigned i;
    if(ordinal>=64) { fprintf(stderr,"flight record actions contract case %u: too many children\n",scenario); abort(); }
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
        fprintf(stderr,"flight record actions child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",
            scenario,ordinal,entry,boundaries[ordinal].sr,boundary.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i])
            fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    if(entry==0xc06c02u && ret==0xc257eau) { ++ordinal; flight_record_actions_fault_boundary_observed(); abort(); }
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<14;++i) { value=scramble(value); REG_DA[i]=value; }
    for(i=8;i<14;++i) REG_DA[i]=boundary.regs[i];
    if(ret==0xc25816u) {
        REG_D[0]=boundary.regs[0]; REG_D[1]=(scenario&16)?0x10001u:256u;
        REG_D[5]=boundary.regs[5]; REG_D[6]=boundary.regs[6]; REG_D[7]=boundary.regs[7];
    }
    if(ret==0xc23426u || ret==0xc23518u) {
        wr_u8(0xc45799u,2); wr_u16(0xc4fda2u,0); wr_u32(0xc2367eu,0xc61400u); wr_u8(0xc63002u,42);
    }
    if(ret==0xc23300u) { REG_D[0]=0x7fffffu; REG_D[1]=0xffff8000u; REG_D[2]=0xffff0000u; }
    if(ret==0xc236feu || ret==0xc2370au) { REG_A[1]=0xc61000u; REG_A[2]=0xc60e00u; }
    if(ret==0xc2370au) {
        static const uint8_t classes[]={0,16,48,49};
        wr_u8(rd_u32(REG_A[7]+4)+98,classes[(scenario>>2)%4]);
    }
    m68k_set_reg(M68K_REG_SR,0x2700u|(value&31u));
    /* Return changed registers and CCR, preserving the actual caller frame.
     * Parent input comparisons retain all child-entry RAM, including PEA. */
    REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,
    uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited;
    m68ki_push_32(ret); REG_PC=entry; flight_record_actions_contract_enter(entry,ret);
    return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) {
    return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL);
}
