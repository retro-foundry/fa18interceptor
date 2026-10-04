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
void segment_projection_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void segment_projection_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"segment projection contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void segment_projection_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2ed70,0xc1fff8},
        {0xc2ee4a,0xc1fff8},
        {0xc2fa7e,0xc2ee3c},
        {0xc2f0c6,0xc2ee78},
        {0xc2f0f4,0xc2ee8e},
        {0xc2f0f4,0xc2eeb0},
        {0xc2f0c6,0xc2eec6},
        {0xc2f156,0xc2eeec},
        {0xc2f128,0xc2ef0a},
        {0xc2f128,0xc2ef2a},
        {0xc2f156,0xc2ef50},
        {0xc2f128,0xc2ef6e},
        {0xc2f156,0xc2ef82},
        {0xc2f156,0xc2efa6},
        {0xc2f128,0xc2efba},
        {0xc2f0f4,0xc2efdc},
        {0xc2f0c6,0xc2eff4},
        {0xc2f0c6,0xc2f00c},
        {0xc2f0f4,0xc2f02c},
        {0xc2fa7e,0xc2f08e},
    };
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"segment projection contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"segment projection child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i]) fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(entry==sites[i].entry && ret==sites[i].ret) break;
    if(i==sizeof sites/sizeof sites[0]) abort();
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    /* Returned cursors are live original RAM; modify the point pair, clip
     * result, D2/D6 and all unsaved registers before the next parent test. */
    REG_A[0]=0xc64040u;REG_A[1]=0xc63000u;REG_A[2]=0xc64020u;
    REG_A[3]=0xc48390u;REG_A[4]=0xc65000u;REG_A[5]=0xc65020u;
    for(i=0;i<6;++i)wr_u16(REG_A[1]+2*i,(uint16_t)(i%3==2?200:((scenario/32+i*47)%501)-250));
    for(i=0;i<3;++i)wr_u16(0xc45ac6u+2*i,(uint16_t)(i==2?((scenario&128)?-100:((scenario&256)?0:100)):((scenario/32+i*13)%201)-100));
    if((scenario/32)%16==0){wr_u16(0xc45ac6u,100);wr_u16(0xc45ac8u,100);wr_u16(0xc45acau,100);}
    if(ret!=0xc1fff8u && entry!=0xc2fa7eu){
        for(i=3;i<=5;++i)REG_D[i]=(REG_D[i]&0xffff0000u)|(uint16_t)(i==5?((scenario&64)?-100:100):((scenario/32+i*79)%801)-400);
        REG_D[2]=(REG_D[2]&0xffff0000u)|(uint16_t)((scenario/32)%201-100);
        REG_D[6]=(REG_D[6]&0xffff0000u)|200;
    }
    /* Independent successive Z results cover opposite-plane and other-axis
     * decisions in the same call. All remaining CCR bits still vary. */
    wr_u32(0xc66000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&27u)|(((scenario>>((ordinal+2)%5))&1u)<<2)); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; segment_projection_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
