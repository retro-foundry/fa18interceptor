/* Original one-instruction behavior vs C, with ROM physically cleared and
 * every ROM alias guarded on the C side. No recorded OS state ships at runtime. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
#include "m68kops.h"
#include "bus.h"
#include "machine.h"
#include "memory.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "exec_glue.h"
#include "exec_task_lookup.h"
#include "graphics_glue.h"
#include "graphics_wait_bovp.h"
#include "graphics_blitter_ownership.h"
#include "potgo_glue.h"
#include "../../build/amiga/service_phase_cases.h"
extern int64_t fa18_cycle_origin,fa18_next_event;
extern int fa18_write_log_active;
void amiga_phase_observe_begin(void);
void amiga_phase_observe_reference(void);
int amiga_phase_observe_compare(void);
static uint8_t *read_file(const char *path,size_t *size) {
    FILE *f=fopen(path,"rb"); long n; uint8_t *p;
    if (!f) return NULL;
    if (fseek(f,0,SEEK_END) || (n=ftell(f))<0 || fseek(f,0,SEEK_SET)) { fclose(f); return NULL; }
    p=malloc(n?n:1);
    if (!p || fread(p,1,(size_t)n,f)!=(size_t)n) { free(p); fclose(f); return NULL; }
    fclose(f); *size=(size_t)n; return p;
}
static uint32_t seed=0x31415926u;
static uint32_t random_word(void) {
    seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed;
}
static unsigned dma_offset;
static void reset_bus(void) {
    fa18_bus_reset();
    if (fa18_bus_timing) {
        FA18Machine *m=fa18_machine;
        m->dmacon=0x0300;
        m->custom[0x100>>1]=0x5000;
        m->custom[0x08E>>1]=0x2081; m->custom[0x090>>1]=0x2CC1;
        m->custom[0x092>>1]=0x0038; m->custom[0x094>>1]=0x00D0;
        fa18_bus_line(m,80,fa18_machine_now()-dma_offset);
    }
}
static void fixture(uint32_t pc,unsigned scenario) {
    static const uint16_t limits[]={0,1,0x7FFF,0x8000,0xFFFF,0xFFFE,0x00FF,0x0100};
    fa18_write_log_active=0;
    for (unsigned i=0;i<16;++i) REG_DA[i]=random_word();
    for (unsigned i=0;i<8;++i) {
        REG_D[i]=(REG_D[i]&0xFFFF0000u)|limits[(scenario/32+i)%8];
        REG_A[i]=0xC61000u+i*0x400u;
        for (unsigned j=0;j<0x200;j+=4) wr_u32(REG_A[i]+j,random_word());
    }
    REG_A[7]=0xC7FC00u;
    for (unsigned i=0;i<64;++i) wr_u32(REG_A[7]+4*i,random_word());
    wr_u32(REG_A[7],0xC10000u); wr_u32(REG_A[7]+4,REG_A[0]);
    wr_u32(REG_A[6]+0x22,REG_A[1]);
    wr_u16(REG_A[6]+0xAA,limits[(scenario/32)%8]);
    wr_u8(REG_A[6]+0x126,(uint8_t)limits[(scenario/32)%8]);
    wr_u16(REG_A[0]+0x1A,limits[(scenario/32+1)%8]);
    wr_u16(REG_A[0]+0x1E,limits[(scenario/32+2)%8]);
    wr_u16(REG_A[1]+0xC,limits[(scenario/32+3)%8]);
    wr_u16(REG_A[6]+0xD4,limits[(scenario/32+4)%8]);
    /* Exec lists use their tail's zero successor, never a NULL head. */
    uint32_t list=REG_A[0]+0x14,node=REG_A[2],tail=list+4;
    wr_u32(list,(scenario/32)&1?node:tail);
    wr_u32(tail,0); wr_u32(list+8,(scenario/32)&1?node:list);
    wr_u32(node,tail); wr_u32(node+4,list);
    /* Mid-body GetMsg phases take A0=list and A1=current node/sentinel. */
    if (pc>=0xFC1BEEu && pc<=0xFC1C16u) REG_A[0]=list;
    if (pc>=0xFC1BFCu && pc<=0xFC1C16u) {
        REG_A[1]=(scenario/32)&1?node:tail;
        REG_D[0]=(scenario/32)&1?tail:0;
    }
    if (pc==0xFC1C04u) REG_A[1]=tail;
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31));
    REG_PC=pc;
    SET_CYCLES(100000000);
    fa18_cycle_origin=100000000; fa18_next_event=INT64_MAX;
    dma_offset=40+(scenario%32)*4;
    reset_bus(); fa18_write_log_active=1;
}
int main(int argc,char **argv) {
    size_t ns,nr; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=read_file("local/system/kick13.rom",&nr);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *ram=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),*cpu=malloc(m68k_context_size());
    unsigned count=argc>1?(unsigned)strtoul(argv[1],NULL,10):256,matched=0;
    if (!state || !rom || !m || !base || !before || !ram || !cpu || !count) return 1;
    if (!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) { fprintf(stderr,"%s\n",error); return 1; }
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL);
    memcpy(base,m,sizeof *m); free(state); free(rom);
    for (unsigned bus=0;bus<2;++bus) {
        fa18_bus_timing=(int)bus;
        for (unsigned k=0;k<sizeof service_phase_cases/sizeof service_phase_cases[0];++k) {
            uint32_t pc=service_phase_cases[k].pc;
            for (unsigned n=0;n<count;++n) {
                uint32_t regs[16],want_pc,want_sr; int want_cycles;
                memcpy(m,base,sizeof *m); fixture(pc,n);
                memcpy(before,m,sizeof *m); m68k_get_context(cpu);
                uint16_t op=fa18_bus_read16(pc);
                amiga_phase_observe_begin();
                fa18_bus_begin(pc); fa18_bus_fetch(pc);
                REG_PPC=pc; REG_IR=op; REG_PC=pc+2;
                m68ki_instruction_jump_table[op](); USE_CYCLES(CYC_INSTRUCTION[op]);
                fa18_bus_finish(REG_PC);
                memcpy(regs,REG_DA,sizeof regs); want_pc=REG_PC;
                want_sr=m68k_get_reg(NULL,M68K_REG_SR); want_cycles=GET_CYCLES();
                memcpy(ram,m->chip,FA18_CHIP_SIZE); memcpy(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
                amiga_phase_observe_reference();
                memcpy(m,before,sizeof *m); m68k_set_context(cpu);
                SET_CYCLES(100000000);
                memset(m->rom,0,sizeof m->rom); memset(m->rtarea,0,sizeof m->rtarea);
                fa18_machine_require_romfree(m);
                m->runtime_guard.service=(AmigaServiceContext){0xC10000u,pc,service_phase_cases[k].name};
                reset_bus(); amiga_phase_observe_begin();
                int handled=service_phase_cases[k].step();
                fa18_bus_finish(REG_PC);
                if (!handled || REG_IR!=op || want_pc!=REG_PC || want_sr!=m68k_get_reg(NULL,M68K_REG_SR) ||
                    want_cycles!=GET_CYCLES() || memcmp(regs,REG_DA,sizeof regs) ||
                    memcmp(ram,m->chip,FA18_CHIP_SIZE) || memcmp(ram+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE) ||
                    !amiga_phase_observe_compare()) {
                    fprintf(stderr,"service phase %s %06X fixture %u bus=%u source PC/SR/cycles=%06X/%04X/%d C=%06X/%04X/%d opcode=%04X/%04X\n",
                        service_phase_cases[k].name,pc,n,bus,want_pc,want_sr,want_cycles,REG_PC,
                        m68k_get_reg(NULL,M68K_REG_SR),GET_CYCLES(),op,REG_IR);
                    return 1;
                }
                if (m->runtime_guard.rom_reads || m->runtime_guard.rom_instruction_fetches ||
                    m->runtime_guard.unsupported_services) return 1;
                ++matched;
            }
        }
    }
    printf("service phases: %zu PCs, %u CPU/DMA fixtures match registers, full SR, RAM, ordered accesses and cycles; zero ROM accesses\n",
        sizeof service_phase_cases/sizeof service_phase_cases[0],matched);
    free(m); free(base); free(before); free(ram); free(cpu); return 0;
}
