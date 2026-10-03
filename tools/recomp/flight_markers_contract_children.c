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
void flight_markers_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void flight_markers_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"marker contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void flight_markers_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc2eca8,0xc2b52e},{0xc32a44,0xc2b554},{0xc2affa,0xc2b5d8},{0xc2affa,0xc2b5ea},
        {0xc2ee4a,0xc2b5fc},{0xc32a44,0xc2b64c},{0xc2affa,0xc2b69e},{0xc2affa,0xc2b6b0},
        {0xc2ee4a,0xc2b6c2},{0xc32a44,0xc2b710},{0xc2affa,0xc2b776},{0xc2b928,0xc2b794},
        {0xc2b952,0xc2b7d0},{0xc2ec90,0xc2b93e},{0xc2affa,0xc2b970},{0xc2ec90,0xc2b97e},{0xc2fa7e,0xc2bae6}
    };
    static const uint16_t projected[]={160,0xffff,20,300,14,186,0x8000,0x7fff};
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"marker contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"marker child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i]) fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(entry==sites[i].entry && ret==sites[i].ret) break;
    if(i==sizeof sites/sizeof sites[0]) abort();
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    SET_W(REG_D[0],projected[(scenario>>3)&7]); SET_W(REG_D[1],projected[(scenario>>6)&7]);
    if(entry==0xc2ee4au) SET_W(REG_D[0],(scenario&32)?0:1);
    REG_A[0]=b.regs[8]; REG_A[1]=0xc60820u; REG_A[2]=0xffff8123u; REG_A[3]=0xc62020u;
    REG_A[4]=0xc63020u; REG_A[5]=entry==0xc2affau?b.regs[13]+6:0xc63040u;
    if(entry==0xc2eca8u) { wr_u16(0xc45958u,(uint16_t)REG_D[0]); wr_u16(0xc4595au,(uint16_t)REG_D[1]); }
    if(entry==0xc2ee4au) { wr_u32(0xc4b390u,((uint32_t)projected[(scenario>>8)&7]<<16)|projected[(scenario>>10)&7]); wr_u32(0xc4b394u,((uint32_t)projected[(scenario>>11)&7]<<16)|projected[(scenario>>12)&7]); }
    wr_u32(0xc65000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; flight_markers_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
