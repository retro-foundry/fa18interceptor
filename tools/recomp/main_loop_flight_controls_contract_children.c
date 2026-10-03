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
void main_loop_flight_controls_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void main_loop_flight_controls_contract_finish(void) {
    if(!source_phase && ordinal!=count) {
        fprintf(stderr,"main loop flight controls contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort();
    }
}
void main_loop_flight_controls_contract_enter(uint32_t entry,uint32_t ret) {
    Boundary boundary={0}; uint32_t value; unsigned i;
    if(ordinal>=64) { fprintf(stderr,"main loop flight controls contract case %u: too many children\n",scenario); abort(); }
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
        fprintf(stderr,"main loop flight controls child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",
            scenario,ordinal,entry,boundaries[ordinal].sr,boundary.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i])
            fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<14;++i) { value=scramble(value); REG_DA[i]=value; }
    {
        static const uint16_t lengths[]={0,0x100,0x300,0x400,0xc00,0x1800,0x1e00,0x36c0,0x4000,0x7fff,0x8000,0xffff};
        unsigned profile=scenario;
        for(i=8;i<14;++i) REG_DA[i]=boundary.regs[i];
        if(ret==0xc14a5au) { wr_u16(0xc45a4cu,(uint16_t)(profile%128u)); wr_u16(0xc45a4eu,lengths[(profile>>2u)%12u]); wr_u16(0xc45a50u,(uint16_t)(profile%256u)); }
        if(ret==0xc14ad2u || ret==0xc14aecu) REG_D[0]=(uint32_t)(int32_t)(int16_t)lengths[(profile>>1u)%12u];
        if(ret==0xc14b2au) REG_D[0]=(profile&8192u)?0:(uint32_t)(int32_t)(int16_t)lengths[(profile>>3u)%12u];
        if(ret==0xc14e1au || ret==0xc15098u) wr_u32(0xc45af2u,profile*37u);
        if(ret==0xc245f6u) REG_D[1]=lengths[profile%12u];
        if(ret==0xc25784u) {
            REG_D[0]=boundary.regs[0]; REG_D[1]=(profile&16u)?0x10001u:256u;
            REG_D[5]=boundary.regs[5]; REG_D[6]=boundary.regs[6]; REG_D[7]=boundary.regs[7];
        }
        if(ret==0xc243a2u) { REG_D[5]=(profile&1u)?192u:0xffffff40u; REG_D[6]=0; REG_D[7]=0; }
        if(ret==0xc23b2cu) wr_u16(REG_A[1]+74,lengths[(profile>>3u)%12u]);
        if(ret==0xc23b2cu && (profile&8192u) && (profile%16u==0 || profile%16u==9)) wr_u16(REG_A[1]+74,0x100);
        if(ret==0xc24180u) { REG_D[0]=0x123456; REG_D[1]=0x234567; REG_D[2]=0x345678; }
        if(ret==0xc241b4u && (profile&64u)) wr_u8(REG_A[1]+4,rd_u8(REG_A[1]+4)|32);
    }
    m68k_set_reg(M68K_REG_SR,0x2700u|(value&31u));
    /* Return changed registers and CCR, preserving the actual caller frame.
     * Parent input comparisons retain all child-entry RAM, including PEA. */
    REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,
    uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited;
    m68ki_push_32(ret); REG_PC=entry; main_loop_flight_controls_contract_enter(entry,ret);
    return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) {
    return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL);
}
