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
void control_readouts_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void control_readouts_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"control/readouts contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void control_readouts_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc13176,0xc12ed0},
        {0xc13176,0xc1301a},
        {0xc131be,0xc12d40},
        {0xc13396,0xc1326a},
        {0xc13396,0xc1327e},
        {0xc13396,0xc132ea},
        {0xc133b2,0xc13116},
        {0xc133b2,0xc13128},
        {0xc17b08,0xc12c7c},
        {0xc17c62,0xc12f84},
        {0xc17c62,0xc12ff8},
        {0xc17cf6,0xc12e5a},
        {0xc17cf6,0xc12e84},
        {0xc17cf6,0xc12eae},
        {0xc17cf6,0xc12f10},
        {0xc17cf6,0xc12fa2},
        {0xc17cf6,0xc12fce},
        {0xc17cf6,0xc1305a},
        {0xc17cf6,0xc131b8},
        {0xc17d6e,0xc13088},
        {0xc17d6e,0xc130a6},
        {0xc17d6e,0xc130de},
        {0xc17d6e,0xc1310c},
        {0xc17daa,0xc12f30},
        {0xc17e4a,0xc13120},
        {0xc17e4a,0xc13132},
        {0xc17ef2,0xc12a56},
        {0xc17ef2,0xc12aaa},
        {0xc17ef2,0xc12ae6},
        {0xc17ef2,0xc12b2c},
        {0xc17ef2,0xc12b6a},
        {0xc17ef2,0xc12bac},
        {0xc17ef2,0xc12be8},
        {0xc17ef2,0xc12c26},
        {0xc17ef2,0xc12c62},
        {0xc17f8c,0xc12cc4},
        {0xc17f8c,0xc12ce8},
        {0xc18096,0xc129d2},
        {0xc18096,0xc129e0},
        {0xc180fc,0xc12a04},
        {0xc18108,0xc13192},
        {0xc52ec8,0xc12e10},
        {0xc52ec8,0xc13320},
    };
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"control/readouts contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"control/readouts child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i]) fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(entry==sites[i].entry && ret==sites[i].ret) break;
    if(i==sizeof sites/sizeof sites[0]) abort();
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    { static const uint16_t words[]={0,1,0xffff,2,0xfffe,7,8,9,63,120,127,0xff80,0x7fff,0x8000,0x7ffe,0x8001};
      REG_D[0]=(REG_D[0]&0xffff0000u)|words[(scenario/32+ordinal)%16]; }
    /* Keep the actual call stack/frame; vary all child working registers,
     * full flags and a RAM result independently between successive calls. */
    for(i=0;i<6;++i)REG_A[i]=0xc64000u+0x100u*i;
    wr_u32(0xc66000u+ordinal*4,value); m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&27u)|(((scenario>>((ordinal+2)%5))&1u)<<2)); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; control_readouts_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
