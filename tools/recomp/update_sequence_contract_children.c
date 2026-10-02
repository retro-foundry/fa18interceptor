/* Structural child-contract oracle ONLY. This replaces glue_child_call.c in
 * the test executable. It does not claim to execute original child bodies;
 * update_sequence_oracle.c and sealed replay prove those independently. */
#include "glue.h"
#include "glue_child_call.h"
#include "globals.h"
#include "machine.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned scenario,ordinal,waits,polls,reference_count;
static int reference_phase;
typedef struct { uint32_t entry,ret,regs[16],sr; } Boundary;
static Boundary boundaries[128];
static uint8_t *boundary_ram[128];
static uint32_t scramble(uint32_t value) {
    value^=value<<13; value^=value>>17; value^=value<<5; return value;
}
void update_contract_reset(unsigned value,int source) {
    scenario=value; ordinal=waits=polls=0; reference_phase=source;
    if(source) reference_count=0;
}
void update_contract_finish(void) {
    if(!reference_phase && ordinal!=reference_count) {
        fprintf(stderr,"child-contract oracle: case %u call count %u/%u\n",scenario,reference_count,ordinal); exit(1);
    }
}
void update_contract_enter(uint32_t entry,uint32_t ret) {
    Boundary boundary={0}; unsigned i; uint32_t value;
    if(ordinal>=128) abort();
    boundary.entry=entry; boundary.ret=ret; memcpy(boundary.regs,REG_DA,sizeof boundary.regs);
    boundary.sr=m68k_get_reg(NULL,M68K_REG_SR);
    if(reference_phase) {
        if(!boundary_ram[ordinal]) boundary_ram[ordinal]=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
        if(!boundary_ram[ordinal]) abort();
        memcpy(boundary_ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE);
        memcpy(boundary_ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
        boundaries[ordinal]=boundary; ++reference_count;
    } else if(memcmp(&boundary,&boundaries[ordinal],sizeof boundary) ||
              memcmp(boundary_ram[ordinal],fa18_machine->chip,FA18_CHIP_SIZE) ||
              memcmp(boundary_ram[ordinal]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE)) {
        Boundary *old=&boundaries[ordinal];
        fprintf(stderr,"child-contract oracle: case %u child %u CPU/RAM mismatch; entry %06X/%06X ret %06X/%06X SR %04X/%04X\n",
            scenario,ordinal,old->entry,entry,old->ret,ret,old->sr,boundary.sr);
        for(i=0;i<16;++i) if(old->regs[i]!=boundary.regs[i]) fprintf(stderr," %c%u %08X/%08X\n",i<8?'D':'A',i&7,old->regs[i],boundary.regs[i]);
        exit(1);
    }
    value=scramble(0x0efd4001u+scenario*97u+ordinal*31u+entry);
    for(i=0;i<14;++i) { value=scramble(value); REG_DA[i]=value; }
    m68k_set_reg(M68K_REG_SR,0x2700u|(value&31u));
    /* Saved scheduling tick is deliberately distinct from its live global. */
    wr_u16(UPDATE_TICK,(uint16_t)value);
    if(entry==0xc265e8u) REG_D[0]=(scenario&32u)?0x80000000u:0;
    if(entry==0xc1ac28u) {
        if(++waits==2) { wr_u16(RECORD_WORD_A,0); wr_u16(RECORD_WORD_B,0); }
        wr_u8(RECORDER_MODE,(uint8_t)value);
    }
    if(entry==0xc1715cu) REG_D[0]=(scenario&32u)?0x12348000u:0;
    if(entry==0xc16c56u) {
        static const uint32_t keys[]={0x30000025u,0x0000ffa5u,0x123456ffu};
        REG_D[0]=keys[(scenario&64u)?polls++:2];
    }
    if(entry==0xc1ad74u) wr_u8(RECORDER_MODE,(uint8_t)value);
    if(entry==0xc0d730u && (rd_u16(UPDATE_DISPLAY_FLAGS)&0x2000u)) {
        REG_A[7]=REG_A[6]; REG_A[6]=m68ki_pull_32(); REG_PC=m68ki_pull_32();
    } else if(entry==0xc0da38u) {
        REG_A[7]=REG_A[6]; REG_A[6]=m68ki_pull_32(); REG_PC=m68ki_pull_32();
    } else REG_PC=m68ki_pull_32();
    ++ordinal;
}
int32_t glue_complete_child_or_frame_exit(uint32_t entry,uint32_t ret,
                                         uint32_t exit_pc,uint32_t exit_sp,int *exited) {
    m68ki_push_32(ret); REG_PC=entry; update_contract_enter(entry,ret);
    if(exited && REG_PC==exit_pc && REG_A[7]==exit_sp) *exited=1;
    return (int32_t)REG_D[0];
}
int32_t glue_complete_child(uint32_t entry,uint32_t ret) {
    return glue_complete_child_or_frame_exit(entry,ret,0,0,NULL);
}
