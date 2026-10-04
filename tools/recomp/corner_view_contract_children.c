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
int corner_view_loop_fixture_mode;
static uint32_t scramble(uint32_t v) { v^=v<<13; v^=v>>17; v^=v<<5; return v; }
void corner_view_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void corner_view_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"corner/view contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void corner_view_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc1d974,0xc2d3fa},
        {0xc2ce82,0xc2cd06},
        {0xc2ce82,0xc2ce3c},
        {0xc2d16c,0xc2d11c},
        {0xc2d16c,0xc2d13a},
        {0xc2d16c,0xc2d160},
        {0xc2d3a4,0xc2d0ba},
        {0xc2ea5a,0xc2e7d4},
        {0xc2ea5a,0xc2e830},
        {0xc2ea5a,0xc2e984},
        {0xc2ea5a,0xc2e9a8},
        {0xc2ead0,0xc2e7ea},
        {0xc2ead0,0xc2e81a},
        {0xc2ead0,0xc2e96c},
        {0xc2ead0,0xc2e9c8},
        {0xc2eb4c,0xc2e874},
        {0xc2eb4c,0xc2e8a0},
        {0xc2eb4c,0xc2e8f0},
        {0xc2eb4c,0xc2e948},
        {0xc2ebc2,0xc2e856},
        {0xc2ebc2,0xc2e8c6},
        {0xc2ebc2,0xc2e904},
        {0xc2ebc2,0xc2e934},
        {0xc2eca8,0xc2cd12},
        {0xc2ed70,0xc2ce52},
    };
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"corner/view contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"corner/view child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i]) fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(entry==sites[i].entry && ret==sites[i].ret) break;
    if(i==sizeof sites/sizeof sites[0]) abort();
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    /* Preserve a returning stack/frame and bounded cursor domain; vary
     * returned working words, clip bytes and independent successive Z. */
    REG_A[0]=0xc64000u;REG_A[3]=0xc65000u;REG_A[4]=0xc64080u;REG_A[5]=0xc640a0u;
    if(ret>=0xc2e7d4u&&ret<=0xc2e9c8u){
        REG_D[0]=b.regs[0];REG_D[1]=(REG_D[1]&0xffff0000u)|((scenario+ordinal)%8)*16u;
        REG_A[0]=0xc4b990u;REG_A[1]=0xc4b390u;REG_A[3]=0xc4b990u+8*((uint16_t)REG_D[0]);
        REG_D[2]=(REG_D[2]&0xffff0000u)|(uint16_t)((int)((scenario/32+ordinal*17)%201)-100);
        REG_D[3]=(REG_D[3]&0xffff0000u)|(uint16_t)((int)((scenario/32+ordinal*43)%801)-400);
        REG_D[4]=(REG_D[4]&0xffff0000u)|(uint16_t)((int)((scenario/32+ordinal*79)%801)-400);
        REG_D[5]=(REG_D[5]&0xffff0000u)|(uint16_t)((scenario&64)?-100:100);
        REG_D[6]=(REG_D[6]&0xffff0000u)|200u;
    }
    for(i=0;i<3;++i)wr_u16(0xc45ac6u+2*i,(uint16_t)(i==2?100:((int)((scenario/32+i*71)%1001)-500)));
    if(((scenario>>((ordinal+2)%5))&1u)==0)wr_u16(0xc45acau,(uint16_t)((scenario&128)?-100:100));
    if((scenario/32)%16==0){wr_u16(0xc45ac6u,100);wr_u16(0xc45ac8u,100);wr_u16(0xc45acau,100);}
    if(corner_view_loop_fixture_mode)wr_u16(0xc45acau,(uint16_t)((scenario&32)?-100:0));
    wr_u32(0xc66000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&27u)|(corner_view_loop_fixture_mode?4u:(((scenario>>((ordinal+2)%5))&1u)<<2))); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; corner_view_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
