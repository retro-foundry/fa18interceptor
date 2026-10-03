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
void input_display_setup_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void input_display_setup_contract_finish(void) {
    if(!source_phase && ordinal!=count) {
        fprintf(stderr,"input display setup contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort();
    }
}
void input_display_setup_contract_enter(uint32_t entry,uint32_t ret) {
    Boundary boundary={0}; uint32_t value; unsigned i;
    if(ordinal>=64) { fprintf(stderr,"input display setup contract case %u: too many children\n",scenario); abort(); }
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
        fprintf(stderr,"input display setup child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",
            scenario,ordinal,entry,boundaries[ordinal].sr,boundary.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i])
            fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<14;++i) { value=scramble(value); REG_DA[i]=value; }
    {
        unsigned profile=scenario/32u;
        static const uint8_t errors[]={0,1,0x7f,0x80,0xff,0x81,2,0xfe};
        /* Branch results and changed globals come from the same test contract
         * for original parent instructions and readable C. No production child
         * or hardware result is substituted. */
        if(ret==0xc16d66u) REG_D[0]=(profile&1u)?0:0xc60500u;
        if(ret==0xc16d8au) { REG_D[0]=(profile&2u)?0:0xc60400u; wr_u32(0xc1abecu,0xc60540u); }
        if(ret==0xc16dc8u) REG_D[0]=(profile&4u)?0xffff0001u:0x80000000u;
        if(ret==0xc16e0eu) REG_D[0]=(profile&8u)?0xffffffffu:0;
        if(ret==0xc16e40u) REG_D[0]=(profile&16u)?0x80000000u:0;
        if(entry==0xc50de8u) wr_u32(0xc1abceu,0xc60480u);
        if(ret==0xc17036u) wr_u32(0xc1abecu,0xc60540u);
        if(ret==0xc17044u) wr_u32(0xc1abecu,0xc60580u);
        if(ret==0xc17052u) { wr_u32(0xc1abceu,0xc60700u); wr_u8(0xc6071fu,errors[profile%8u]); }
        if(entry==0xc5058eu || entry==0xc50614u || entry==0xc5046cu) REG_D[0]=(profile&(1u<<(ordinal%8u)))?0:1;
        if(ret==0xc1793au) wr_u32(0xc0a448u,0xc60c00u);
        if(ret==0xc17978u) wr_u32(0xc0a450u,0xc60c40u);
        if(ret==0xc1613cu) wr_u16(DRAW_PAGE,(uint16_t)((profile>>1u)&1u));
        if(ret==0xc1617cu && (profile&32u)) wr_u8(0xc45899u,(uint8_t)(profile&3u));
        if(ret==0xc1619cu && (profile&64u)) wr_u8(0xc45899u,(uint8_t)(profile&3u));
        if(ret==0xc161beu) wr_u32(0xc45660u,0xc60700u);
        if(ret==0xc16212u && (profile&128u)) wr_u8(0xc45899u,1);
        if(ret==0xc16254u) wr_u32(0xc45660u,0xc60740u);
        if(ret==0xc1626cu || ret==0xc16212u) wr_u16(DRAW_PAGE,(uint16_t)((profile>>2u)&1u));
    }
    m68k_set_reg(M68K_REG_SR,0x2700u|(value&31u));
    /* Return changed registers and CCR, preserving the actual caller frame.
     * Parent input comparisons retain all child-entry RAM, including PEA. */
    REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,
    uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited;
    m68ki_push_32(ret); REG_PC=entry; input_display_setup_contract_enter(entry,ret);
    return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) {
    return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL);
}
