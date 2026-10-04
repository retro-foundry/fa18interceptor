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
void hud_history_stream_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void hud_history_stream_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"HUD history/stream parent contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void hud_history_stream_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc0cffa,0xc0cfb0},
        {0xc1d974,0xc0d1e4},
        {0xc259c2,0xc33490},
        {0xc259c2,0xc336bc},
        {0xc259c2,0xc33878},
        {0xc25a08,0xc333d6},
        {0xc25a08,0xc3342e},
        {0xc25a08,0xc3346c},
        {0xc25a08,0xc33600},
        {0xc25a08,0xc3365a},
        {0xc25a08,0xc33698},
        {0xc25a08,0xc3381e},
        {0xc25a08,0xc3384a},
        {0xc2ce82,0xc0d1fa},
        {0xc2ec94,0xc1fe40},
        {0xc2ec9c,0xc0d25c},
        {0xc2ec9c,0xc0d296},
        {0xc2ec9c,0xc0d2d0},
        {0xc2ec9c,0xc0d2f0},
        {0xc2eca4,0xc1fe62},
        {0xc2f5c0,0xc33524},
        {0xc2f5c0,0xc3357e},
        {0xc2f5c0,0xc337c2},
        {0xc2f5c0,0xc337ea},
        {0xc2f5d4,0xc334d0},
        {0xc2f5d4,0xc337ca},
        {0xc2f5d4,0xc337f4},
        {0xc2f5f4,0xc337fc},
        {0xc2f60a,0xc337b4},
        {0xc2f60a,0xc337dc},
        {0xc32aa4,0xc333ee},
        {0xc32aa4,0xc33618},
        {0xc32ab4,0xc33414},
        {0xc32ab4,0xc3363e},
        {0xc33ad6,0xc335b6},
        {0xc33b06,0xc3378e},
        {0xc33f54,0xc338a8},
        {0xc33f54,0xc338fc},
        {0xc33f54,0xc33948},
        {0xc33f70,0xc336f8},
        {0xc33f70,0xc33736},
        {0xc33f70,0xc33776},
        {0xc33f8a,0xc334ec},
        {0xc33f8a,0xc33542},
        {0xc33f8a,0xc3359c},
        {0xc33fb4,0xc335ae},
        {0xc33fb4,0xc33786},
        {0xc34066,0xc3395c},
    };
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"HUD history/stream parent contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"HUD history/stream parent child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i]) fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(entry==sites[i].entry && ret==sites[i].ret) break;
    if(i==sizeof sites/sizeof sites[0]) abort();
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    /* Source parents receive changed full data registers and valid cursors.
     * Keep only source drawing dimensions and history shift inputs bounded. */
    SET_W(REG_D[0],(scenario>>5)&3u);SET_W(REG_D[2],(scenario&64)?0:0xffceu);
    SET_W(REG_D[5],scenario&1u);SET_W(REG_D[6],(scenario&128)?0x8000u:0x0041u);SET_W(REG_D[7],(scenario>>5)%3u*4u);
    if(entry==0xc25a08u || entry==0xc259c2u) REG_D[2]=(scenario&64)?0:0xffffffceu;
    if(entry==0xc1d974u) { static const uint16_t lengths[]={0,1,8,127,128,0xffff,0x8000,0x7fff};SET_W(REG_D[1],lengths[(scenario>>5)&7u]); }
    REG_A[0]=0xc64008u;REG_A[1]=0xc46384u;REG_A[2]=0xc64000u;
    REG_A[3]=0xc63020u;REG_A[4]=0x10020u;REG_A[5]=0xc65000u;
    if(entry==0xc25a08u) { static const uint32_t packed[]={0,0x1,0x20,0x21,0x40,0x41,0x70,0x71,0x3500,0x3599,0x3600,0x3601,0x3700,0x9999,0xffffffffu,0x80000000u};wr_u32(0xc45b22u,packed[(scenario>>5)&15u]); }
    if(entry==0xc259c2u) wr_u32(0xc45b1eu,(scenario>>5)%101u);
    wr_u32(0xc66000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; hud_history_stream_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
