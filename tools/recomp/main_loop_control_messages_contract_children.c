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
void main_loop_control_messages_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void main_loop_control_messages_contract_finish(void) {
    if(!source_phase && ordinal!=count) {
        fprintf(stderr,"main loop control messages contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort();
    }
}
void main_loop_control_messages_contract_enter(uint32_t entry,uint32_t ret) {
    Boundary boundary={0}; uint32_t value; unsigned i;
    if(ordinal>=64) { fprintf(stderr,"main loop control messages contract case %u: too many children\n",scenario); abort(); }
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
        fprintf(stderr,"main loop control messages child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",
            scenario,ordinal,entry,boundaries[ordinal].sr,boundary.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i])
            fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<14;++i) { value=scramble(value); REG_DA[i]=value; }
    {
        unsigned profile=scenario;
        /* Cursors remain in fixture storage; other registers and all CCR bits
         * are changed at actual call boundaries. Production children are intact. */
        for(i=8;i<14;++i) REG_DA[i]=boundary.regs[i];
        if(entry==0xc330feu || entry==0xc3316au) {
            REG_D[3]=boundary.regs[3]; REG_D[4]=boundary.regs[4]; REG_D[5]=boundary.regs[5]; REG_D[6]=boundary.regs[6];
            if(entry==0xc330feu && (profile&64u)) {
                REG_D[5]+=4; REG_D[3]^=0x0100; REG_D[6]^=15;
                REG_A[5]=0xc60a00u;
            }
        }
        if(ret==0xc151a4u) wr_u8(0xc46201u,rd_u8(0xc46201u)^(profile&256u?0x10:0));
        if(ret==0xc152dcu) wr_u8(0xc4588bu,(uint8_t)((profile&16u)?1:2));
        if(ret==0xc15350u || ret==0xc15362u || ret==0xc15374u) wr_u16(0xc461e4u,(uint16_t)((profile&8u)?2:0x8001));
        if(ret==0xc153aau && (profile&512u)) {
            wr_u32(REG_A[6]-20,0xc60840u); wr_u16(0xc60840u,(uint16_t)(profile&1u));
        }
        if(ret==0xc3307au && (profile&128u)) wr_u32(0xc60a04u,0x12000u);
        if(ret==0xc330ceu && (profile&256u)) wr_u8(0xc457deu,(uint8_t)((profile>>1u)&1u));
    }
    m68k_set_reg(M68K_REG_SR,0x2700u|(value&31u));
    /* Return changed registers and CCR, preserving the actual caller frame.
     * Parent input comparisons retain all child-entry RAM, including PEA. */
    REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,
    uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited;
    m68ki_push_32(ret); REG_PC=entry; main_loop_control_messages_contract_enter(entry,ret);
    return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) {
    return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL);
}
