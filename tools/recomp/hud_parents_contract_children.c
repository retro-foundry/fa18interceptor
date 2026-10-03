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
void hud_parents_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void hud_parents_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"HUD parent contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void hud_parents_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc06c02,0xc32804},
        {0xc25a08,0xc31aa8},
        {0xc25a08,0xc31b56},
        {0xc2f5c0,0xc31b46},
        {0xc2f63a,0xc3115c},
        {0xc2f64e,0xc31166},
        {0xc2f64e,0xc31188},
        {0xc2f64e,0xc31190},
        {0xc2f64e,0xc311b4},
        {0xc2f64e,0xc311bc},
        {0xc2f64e,0xc311de},
        {0xc2f64e,0xc311e6},
        {0xc2f64e,0xc3121a},
        {0xc2f64e,0xc31222},
        {0xc2fa78,0xc30bc4},
        {0xc2fa78,0xc30ea8},
        {0xc2fa78,0xc310a8},
        {0xc308d8,0xc308c6},
        {0xc308d8,0xc308ce},
        {0xc308d8,0xc308d6},
        {0xc308e2,0xc308be},
        {0xc308f4,0xc30800},
        {0xc308f4,0xc30804},
        {0xc308f4,0xc30808},
        {0xc30904,0xc307fc},
        {0xc30cc4,0xc30c20},
        {0xc30cc4,0xc30c6e},
        {0xc30cc4,0xc30d7e},
        {0xc30eaa,0xc309e0},
        {0xc310aa,0xc30f7c},
        {0xc310e2,0xc307ba},
        {0xc310e2,0xc30bf2},
        {0xc310e2,0xc30c4e},
        {0xc310e2,0xc30c9e},
        {0xc310e2,0xc30d54},
        {0xc310e2,0xc30dda},
        {0xc310e2,0xc30fec},
        {0xc32806,0xc327ee},
        {0xc53f44,0xc307da},
        {0xc53f44,0xc308a0},
        {0xc53f44,0xc30cf4},
        {0xc53f44,0xc30e06},
        {0xc53f44,0xc3103a},
    };
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"HUD parent contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"HUD parent child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i]) fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(entry==sites[i].entry && ret==sites[i].ret) break;
    if(i==sizeof sites/sizeof sites[0]) abort();
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    /* Change all full data outputs and valid cursors. Test-only returns. */
    /* Preserve only the original glyph/fault loop word, while changing its
     * high half. Other data outputs change within valid blit dimensions. */
    if(entry==0xc32806u || entry==0xc06c02u) SET_W(REG_D[0],b.regs[0]);
    SET_W(REG_D[5],scenario&1u); SET_W(REG_D[6],0x0041u);
    SET_W(REG_D[7],(scenario&128)?0:1);
    REG_A[0]=0xc61004u; REG_A[1]=b.regs[9]; REG_A[2]=b.regs[10];
    REG_A[3]=0xc63020u; REG_A[4]=0x10020u; REG_A[5]=0xc64000u;
    wr_u32(0xc45b22u,(scenario&128)?0x00000001u:0x001abcf0u);
    wr_u32(0xc66000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; hud_parents_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
