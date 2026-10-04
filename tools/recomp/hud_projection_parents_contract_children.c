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
void hud_projection_parents_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void hud_projection_parents_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"HUD projection parent contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void hud_projection_parents_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc06c02,0xc32804},
        {0xc1d974,0xc0d02a},
        {0xc2ec90,0xc33d3a},
        {0xc2ec9c,0xc0d046},
        {0xc2ec9c,0xc0db40},
        {0xc2f5c0,0xc33c18},
        {0xc31c60,0xc332e4},
        {0xc31d16,0xc33cc4},
        {0xc31d64,0xc332f2},
        {0xc31e6c,0xc33bea},
        {0xc32806,0xc327ee},
        {0xc332fe,0xc332d2},
        {0xc33370,0xc332f6},
        {0xc33b38,0xc332fa},
        {0xc33cd2,0xc33cd0},
        {0xc33dc8,0xc332de},
        {0xc34146,0xc332d6},
        {0xc342d0,0xc332da},
        {0xc345a0,0xc33c42},
        {0xc347f2,0xc33c86},
    };
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"HUD projection parent contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"HUD projection parent child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
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
    SET_W(REG_D[2],(scenario>>3)&3u); SET_W(REG_D[5],scenario&1u); SET_W(REG_D[6],0x0041u);
    SET_W(REG_D[7],(scenario&128)?0:1);
    REG_A[0]=0xc61004u; REG_A[1]=0xc46384u; REG_A[2]=b.regs[10];
    REG_A[3]=0xc63020u; REG_A[4]=0x10020u; REG_A[5]=0xc65000u;
    wr_u8(0xc457a1u,(scenario&128)?0:1);
    wr_u8(0xc457aeu,(scenario&512)?1:0);
    wr_u32(0xc45b22u,(scenario&128)?0x00000001u:0x001abcf0u);
    wr_u32(0xc66000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; hud_projection_parents_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
