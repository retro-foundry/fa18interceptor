/* Complete controlled child-entry CPU/SR/RAM contracts; actual children are
 * checked independently. No source parent bytes or production bus are changed. */
#include "glue.h"
#include "glue_child_call.h"
#include "globals.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct { uint32_t entry,ret,regs[16],sr; } Boundary;
static Boundary boundaries[256];
static uint8_t ram[256][FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static unsigned scenario,ordinal,count;
static int source_phase;
static uint32_t scramble(uint32_t v) { v^=v<<13; v^=v>>17; v^=v<<5; return v; }
void render_leaf_helpers_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void render_leaf_helpers_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"render leaf helper contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void render_leaf_helpers_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc0d74a,0xc2ff20},
        {0xc0d752,0xc2fef2},
        {0xc2f5f4,0xc2f618},
        {0xc2f5f4,0xc2f620},
        {0xc2f5f4,0xc302d2},
        {0xc2f60a,0xc302da},
        {0xc2f66e,0xc3028e},
        {0xc2f688,0xc2f5ee},
        {0xc2fa7e,0xc302ba},
        {0xc301f6,0xc2fef8},
        {0xc30466,0xc2ff0c},
        {0xc305aa,0xc30328},
        {0xc305aa,0xc30340},
    };
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"render leaf helper contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"render leaf helper child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i]) fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(entry==sites[i].entry && ret==sites[i].ret) break;
    if(i==sizeof sites/sizeof sites[0]) abort();
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    /* Changed child outputs stay on valid original RAM cursors. Frame and
     * stack remain source-owned; all full returned data words are changed. */
    REG_A[0]=0xc4bf94u;REG_A[1]=0xc63000u;REG_A[2]=0xc64020u;
    REG_A[3]=0xc48390u;REG_A[4]=0xc45bc6u;REG_A[5]=0xc65000u;
    /* Every unsaved return changes; source-owned stack/frame remain.
     * Edge callbacks return a valid counter and bounded saved coordinates. */
    if(ret==0xc30328u||ret==0xc30340u){REG_A[2]=0xc64020u;REG_A[3]=0xc660f0u;wr_u16(REG_A[3],(uint16_t)(ordinal<2?scenario%3u:0));}
    wr_u32(0xc66000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; render_leaf_helpers_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
