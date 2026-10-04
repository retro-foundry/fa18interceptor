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
void display_record_selection_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=0; source_phase=source; if(source) count=0;
}
void display_record_selection_contract_finish(void) {
    if(!source_phase && ordinal!=count) { fprintf(stderr,"display-record selection contract case %u: source children %u C children %u\n",scenario,count,ordinal); abort(); }
}
void display_record_selection_contract_enter(uint32_t entry,uint32_t ret) {
    static const struct { uint32_t entry,ret; } sites[]={
        {0xc0daa0,0xc0d894},
        {0xc0daa0,0xc0d8b2},
        {0xc0daa0,0xc0d8d8},
        {0xc0daa0,0xc0d8fc},
        {0xc0daa0,0xc0d926},
        {0xc0daa0,0xc0d93a},
        {0xc0daa0,0xc0d974},
        {0xc0daa0,0xc0d998},
        {0xc0daa0,0xc0d9c0},
        {0xc0daa0,0xc0d9de},
        {0xc0daa0,0xc0da04},
        {0xc0daa0,0xc0da18},
        {0xc0dad0,0xc0d8aa},
        {0xc0dad0,0xc0d8f4},
        {0xc0dad0,0xc0d932},
        {0xc0dad0,0xc0d946},
        {0xc0dad0,0xc0d980},
        {0xc0dad0,0xc0d9a4},
        {0xc0dad0,0xc0d9c8},
        {0xc0dad0,0xc0d9e6},
        {0xc0dad0,0xc0da0c},
        {0xc0dad0,0xc0da20},
        {0xc0dad0,0xc0da46},
        {0xc0dad4,0xc0d8a6},
        {0xc0dad4,0xc0d8dc},
        {0xc0dad4,0xc0d900},
        {0xc0dad4,0xc0d92e},
        {0xc0dad4,0xc0d942},
        {0xc0dad4,0xc0d990},
        {0xc0dad4,0xc0d9d6},
        {0xc0dad4,0xc0da08},
        {0xc0dad4,0xc0da1c},
        {0xc0dad4,0xc0da4a},
        {0xc0dadc,0xc0d898},
        {0xc0dadc,0xc0d8b6},
        {0xc0dadc,0xc0d8e0},
        {0xc0dadc,0xc0d904},
        {0xc0dadc,0xc0d92a},
        {0xc0dadc,0xc0d93e},
        {0xc0dadc,0xc0d978},
        {0xc0dadc,0xc0d99c},
        {0xc0dadc,0xc0d9d2},
        {0xc0dadc,0xc0da34},
        {0xc0dadc,0xc0da4e},
        {0xc0dae6,0xc0d89c},
        {0xc0dae6,0xc0d8ba},
        {0xc0dae6,0xc0d8e4},
        {0xc0dae6,0xc0d908},
        {0xc0dae6,0xc0d956},
        {0xc0dae6,0xc0d97c},
        {0xc0dae6,0xc0d9a0},
        {0xc0dae6,0xc0d9c4},
        {0xc0dae6,0xc0d9e2},
        {0xc0dae6,0xc0da10},
        {0xc0dae6,0xc0da24},
        {0xc0dae6,0xc0da52},
        {0xc2e758,0xc0d7e0},
    };
    Boundary b={0}; uint32_t value; unsigned i;
    if(ordinal>=256) { fprintf(stderr,"display-record selection contract case %u: too many children\n",scenario); abort(); }
    b.entry=entry; b.ret=ret; memcpy(b.regs,REG_DA,sizeof b.regs); b.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(source_phase) {
        boundaries[ordinal]=b; ++count; memcpy(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
    } else if(memcmp(&b,&boundaries[ordinal],sizeof b) || memcmp(ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
        memcmp(ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        fprintf(stderr,"display-record selection child contract: case %u child %u entry %06X CPU/RAM mismatch; SR %04X/%04X\n",scenario,ordinal,entry,boundaries[ordinal].sr,b.sr);
        for(i=0;i<16;++i) if(boundaries[ordinal].regs[i]!=REG_DA[i]) fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,boundaries[ordinal].regs[i],REG_DA[i]);
        exit(1);
    }
    for(i=0;i<sizeof sites/sizeof sites[0];++i) if(entry==sites[i].entry && ret==sites[i].ret) break;
    if(i==sizeof sites/sizeof sites[0]) abort();
    value=scramble(scenario*97u+ordinal*31u+entry);
    for(i=0;i<8;++i) { value=scramble(value); REG_D[i]=value; }
    /* The original child returns to the parent's saved frame. Independently
     * vary all working registers, bounded output cursors and source flags. */
    for(i=0;i<6;++i) {value=scramble(value);REG_A[i]=0xc64000u+((value&255u)<<2);}
    if(entry==0xc2e758u){
      unsigned n=scenario>>5,mask=n&15u;
      for(i=0;i<4;++i){wr_u16(0xc4e854u+2*i,(uint16_t)((mask&(1u<<i))?1:0));wr_u16(0xc4e85cu+2*i,(uint16_t)((n/16+i)%8));}
    }
    wr_u32(0xc66000u+ordinal*4,value);m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=m68ki_pull_32(); ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    (void)exit_pc; (void)exit_sp; (void)exited; m68ki_push_32(ret); REG_PC=entry; display_record_selection_contract_enter(entry,ret); return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) { return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL); }
