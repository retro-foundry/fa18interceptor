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
extern void flight_dynamics_publish_class(uint32_t address);
static uint32_t scramble(uint32_t v) { v^=v<<13; v^=v>>17; v^=v<<5; return v; }
void flight_dynamics_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
    flight_dynamics_publish_class(0);
}
void flight_dynamics_contract_finish(void) {
    if(!source_phase && ordinal!=count) {
        fprintf(stderr,"flight dynamics contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort();
    }
}
void flight_dynamics_contract_enter(uint32_t entry,uint32_t ret) {
    Boundary boundary={0}; uint32_t value; unsigned i;
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2d970,0xc25b3a},{0xc28e28,0xc25bac},{0xc2c392,0xc25c70},{0xc1b27e,0xc25c76},
        {0xc25704,0xc25d20},{0xc25704,0xc25d54},{0xc13d84,0xc25d84},{0xc2d408,0xc25da4},
        {0xc149be,0xc25e2c},{0xc26ebe,0xc26014},{0xc17f8c,0xc260fe},{0xc25704,0xc2616a},
        {0xc06c02,0xc261d6},{0xc26322,0xc26246},{0xc2b05a,0xc2624e},{0xc26352,0xc2625e},{0xc2651e,0xc26282},
        {0xc26cc0,0xc26af2},{0xc26d8a,0xc26af8},{0xc28b16,0xc289da},{0xc28b16,0xc289f6},
        {0xc28f16,0xc28a6a},{0xc28f16,0xc28a98},{0xc28f16,0xc28d58},{0xc2d954,0xc28e02}
    };
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(sites[i].entry==entry && sites[i].ret==ret) break;
    if(i==sizeof sites/sizeof sites[0]) { fputs("unsealed flight-dynamics child site\n",stderr); abort(); }
    if(ordinal>=64) { fprintf(stderr,"flight dynamics contract case %u: too many children\n",scenario); abort(); }
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
        fprintf(stderr,"flight dynamics child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",
            scenario,ordinal,entry,boundaries[ordinal].sr,boundary.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i])
            fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    REG_A[0]=0xc60420u; REG_A[1]=boundary.regs[9];
    REG_A[2]=boundary.regs[10]; REG_A[3]=boundary.regs[11];
    REG_A[4]=boundary.regs[12]+4; REG_A[5]=boundary.regs[13];
    /* Values are returned independently of preserved cursors/frame. Loop
     * counters and pointer-like scalars obey each original call contract. */
    if(entry==0xc28b16u) REG_D[7]=boundary.regs[7];
    if(entry==0xc28f16u) { REG_D[0]=boundary.regs[0]; REG_D[1]=boundary.regs[1]; }
    if(ret==0xc28d58u) REG_A[2]=boundary.regs[10]+2;
    if(entry==0xc2d954u) REG_A[2]=0xc63202u;
    if(ret==0xc28a6au || ret==0xc28a98u) REG_D[7]=boundary.regs[7];
    if(entry==0xc2d954u) REG_D[0]=boundary.regs[0];
    if(entry==0xc26cc0u || entry==0xc26d8au) REG_D[7]=(scenario&4096)?1:0;
    if(entry==0xc26ebeu) {
        static const uint32_t codes[]={0,16,32,64,48,80,96,0xffff};
        REG_D[0]=codes[(scenario>>6)&7];
        if(scenario&8192) value|=4; else value&=~4u;
        if(scenario%32==12) { wr_u16(REG_A[1],rd_u16(REG_A[1])|0x400); wr_u16(REG_A[1]+76,20); value&=~4u; }
        if(scenario%32==14) {
            REG_D[0]=32; value&=~4u; wr_u8(REG_A[1]+98,0x30);
            flight_dynamics_publish_class(REG_A[1]+98);
        }
    }
    wr_u32(0xc66000u+ordinal*4,value);
    m68k_set_reg(M68K_REG_SR,0x2700u|(value&31u));
    /* Return changed registers and CCR, preserving the actual caller frame.
     * Parent input comparisons retain all child-entry RAM, including PEA. */
    REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,
    uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited;
    m68ki_push_32(ret); REG_PC=entry; flight_dynamics_contract_enter(entry,ret);
    return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) {
    return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL);
}
