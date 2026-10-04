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
void hud_render_parents_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void hud_render_parents_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"ground/HUD render parent contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void hud_render_parents_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc06c02,0xc32804},
        {0xc2469e,0xc099a2},
        {0xc2469e,0xc099ee},
        {0xc2469e,0xc09a60},
        {0xc2f5c0,0xc30146},
        {0xc2f5c0,0xc30162},
        {0xc2f5c0,0xc3407a},
        {0xc2f5c0,0xc34166},
        {0xc2f5d4,0xc3016a},
        {0xc2f5d4,0xc30172},
        {0xc2f5d4,0xc3017a},
        {0xc2f5d4,0xc30184},
        {0xc2f5d4,0xc3018a},
        {0xc2f5d4,0xc30190},
        {0xc2f5d4,0xc30198},
        {0xc2f5d4,0xc34084},
        {0xc2f5d4,0xc340c8},
        {0xc2f5d4,0xc3412c},
        {0xc2f5d4,0xc34170},
        {0xc2f5d4,0xc34178},
        {0xc2f5f4,0xc3014e},
        {0xc2f5f4,0xc340c0},
        {0xc2f5f4,0xc340d4},
        {0xc2f5f4,0xc34124},
        {0xc2f5f4,0xc34138},
        {0xc2f5f4,0xc34180},
        {0xc2f5f4,0xc3460c},
        {0xc2f5f4,0xc34636},
        {0xc2f5f4,0xc34660},
        {0xc2f5f4,0xc3468a},
        {0xc2f5f4,0xc346fe},
        {0xc2f5f4,0xc3474a},
        {0xc2f5f4,0xc34798},
        {0xc2f5f4,0xc347e6},
        {0xc2f5f4,0xc34942},
        {0xc2f60a,0xc33330},
        {0xc2f60a,0xc33340},
        {0xc2f60a,0xc33350},
        {0xc2f60a,0xc33362},
        {0xc2f60a,0xc3336e},
        {0xc2f66e,0xc348ac},
        {0xc2fa78,0xc3013a},
        {0xc2fa7e,0xc341b2},
        {0xc2fa7e,0xc341ec},
        {0xc2fa7e,0xc3421e},
        {0xc2fa7e,0xc342bc},
        {0xc2fa7e,0xc3451a},
        {0xc3019c,0xc30108},
        {0xc310e2,0xc3007e},
        {0xc31e6c,0xc33f52},
        {0xc32662,0xc325ea},
        {0xc32662,0xc325fc},
        {0xc32662,0xc3260e},
        {0xc32662,0xc3265c},
        {0xc3267a,0xc3249a},
        {0xc3267a,0xc324cc},
        {0xc3267a,0xc3250c},
        {0xc32806,0xc327ee},
        {0xc33da4,0xc33edc},
        {0xc348b2,0xc33f44},
    };
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"ground/HUD render parent contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"ground/HUD render parent child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
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
    /* Keep source text DBRA and DMA descriptors bounded; all upper words
     * and the unsaved pointer returns still vary. */
    if(ret==0xc327eeu || ret==0xc32804u)REG_D[0]=(REG_D[0]&0xffff0000u)|(scenario%4u);
    if(ret==0xc3007eu){REG_D[1]=0;REG_D[5]=(REG_D[5]&0xffff0000u)|(scenario&1u);REG_D[6]=(REG_D[6]&0xffff0000u)|0x42u;REG_D[7]=(REG_D[7]&0xffff0000u)|((scenario>>1)&1u);}
    if(ret==0xc3265cu){wr_u8(0xc45785u,(scenario>>3)&1u);wr_u8(0xc45793u,(scenario>>4)&1u);}
    wr_u32(0xc66000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; hud_render_parents_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
