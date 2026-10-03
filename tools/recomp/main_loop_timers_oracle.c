/* Complete main-loop-timers proof, including cold internal paths and real children. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
#include "m68kops.h"
#include "machine.h"
#include "bus.h"
#include "recomp_runtime.h"
#include "recomp_ports.h"
#include "memory.h"
#include "ports_glue.h"
#include "globals.h"

extern int fa18_write_log_active;
extern void fa18_structural_reset_write_log(void);
extern void fa18_main_loop_timers_fixture_begin(const char *phase);
extern int64_t fa18_next_event;
static uint32_t seed=0xc0f5f8u;
static uint32_t random_value(void) {
    seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed;
}
static uint8_t *read_file(const char *path,size_t *size) {
    FILE *file=fopen(path,"rb"); long length; uint8_t *bytes;
    if(!file) return NULL;
    if(fseek(file,0,SEEK_END) || (length=ftell(file))<0 || fseek(file,0,SEEK_SET)) return NULL;
    bytes=malloc((size_t)length);
    if(!bytes || fread(bytes,1,(size_t)length,file)!=(size_t)length || fclose(file)) return NULL;
    *size=(size_t)length; return bytes;
}
static uint32_t selected_entry=0xc25312u;
static const uint32_t source_boundaries[]={0xC2527Au,0xC2527Cu,0xC25282u,0xC25284u,0xC25286u,0xC2528Cu,0xC2528Eu,0xC25292u,0xC25296u,0xC25298u,0xC2529Au,0xC2529Cu,0xC2529Eu,0xC252A0u,0xC252A2u,0xC252A8u,0xC252AAu,0xC252ACu,0xC252B0u,0xC252B4u,0xC252B6u,0xC252B8u,0xC252BAu,0xC252BCu,0xC252C0u,0xC252C4u,0xC252C6u,0xC252C8u,0xC252CAu,0xC252CCu,0xC252D0u,0xC252D4u,0xC252D6u,0xC252D8u,0xC252DAu,0xC252DCu,0xC252E0u,0xC252E4u,0xC252E6u,0xC252E8u,0xC252EAu,0xC252ECu,0xC252EEu,0xC252F0u,0xC252F2u,0xC252F6u,0xC252F8u,0xC252FCu,0xC25300u,0xC25304u,0xC2530Au,0xC2530Eu,0xC25312u,0xC25318u,0xC2531Eu,0xC25322u,0xC25328u,0xC2532Cu,0xC2532Eu,0xC25336u,0xC2533Cu,0xC25342u,0xC25346u,0xC2534Cu,0xC2534Eu,0xC25352u,0xC25358u,0xC2535Eu,0xC25362u,0xC25364u,0xC25366u,0xC2536Cu,0xC25372u,0xC2537Au,0xC2537Cu,0xC25382u,0xC25388u,0xC2538Eu,0xC25394u,0xC25396u,0xC2539Eu,0xC253A4u,0xC253AAu,0xC253B0u,0xC253B2u,0xC253B8u,0xC253BEu,0xC253C4u,0xC253C6u,0xC253C8u,0xC253CCu,0xC253CEu,0xC253D0u,0xC253D2u,0xC253D8u,0xC253DCu,0xC253E2u,0xC253E6u,0xC253ECu,0xC253F0u,0xC253F6u,0xC253FCu,0xC253FEu,0xC25400u,0xC25404u,0xC25406u,0xC25408u,0xC2540Au,0xC2540Cu,0xC2540Eu,0xC25410u,0xC25416u,0xC25420u,0xC25426u,0xC2542Cu,0xC2542Eu,0xC25434u,0xC25436u,0xC2543Au,0xC25440u,0xC25446u,0xC2544Au,0xC2544Cu,0xC2544Eu,0xC25454u,0xC2545Au,0xC2545Cu,0xC25462u,0xC25466u,0xC25468u,0xC2546Au,0xC2546Eu,0xC25474u,0xC25476u,0xC25478u,0xC2547Cu,0xC2547Eu,0xC25480u,0xC2548Au,0xC25490u,0xC25492u,0xC25498u,0xC2549Eu,0xC254A0u,0xC254A6u,0xC254A8u,0xC254AEu,0xC254B0u,0xC254B6u,0xC254B8u,0xC254BEu,0xC254C4u,0xC254C6u,0xC254CCu,0xC254D2u,0xC254DCu,0xC254E6u};
static unsigned char visited[FA18_SLOW_SIZE/2];
static int source_owned(uint32_t pc) { unsigned i; for(i=0;i<sizeof source_boundaries/sizeof source_boundaries[0];++i) if(source_boundaries[i]==pc) return 1; return 0; }
static unsigned source_slot(uint32_t pc) { return (pc-FA18_SLOW_BASE)/2; }
static uint32_t source_pc(unsigned slot) { return FA18_SLOW_BASE+2*slot; }
static int source_call(uint32_t ret,uint32_t sp) {
    unsigned dispatch;
    uint32_t child_ret=0,child_sp=0;
    for(dispatch=0;dispatch<1000000;++dispatch) {
        int lo=0,hi=fa18_recomp_entry_count,result;
        if(REG_PC==ret && REG_A[7]==sp) return FA18_RET;
        if(source_owned(REG_PC)) {
            uint32_t pc=REG_PC;
            uint16_t opcode=m68k_read_memory_16(pc);
            visited[source_slot(pc)]=1;
            child_ret=0;
            REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
            m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
            continue;
        }
        if(!child_ret) { child_ret=rd_u32(REG_A[7]); child_sp=REG_A[7]+4; }
        while(lo<hi) {
            int mid=lo+(hi-lo)/2;
            if(fa18_recomp_entries[mid].pc<REG_PC) lo=mid+1; else hi=mid;
        }
        if(lo==fa18_recomp_entry_count || fa18_recomp_entries[lo].pc!=REG_PC)
            result=fa18_recomp_resume(child_ret,child_sp);
        else result=fa18_recomp_functions[fa18_recomp_entries[lo].function].fn((int)fa18_recomp_entries[lo].label);
        if(result==FA18_EXIT_INTERP) return result;
    }
    return FA18_EXIT_INTERP;
}
static uint32_t expected_sp=0xc7ff04u;
static void fixture(unsigned scenario) {
    static const uint8_t bytes[]={0,1,0x7f,0x80,0xff,0x81,2,15};
    static const uint32_t fractions[]={0,1,999,1000,125000,999999,0x7fffffff,0x80000000};
    static const uint32_t readouts[]={1,12,13,30,32767,32768,0x80000001,0xffffffff};
    unsigned i,profile=scenario/32u;
    for(i=0;i<15;++i) REG_DA[i]=random_value(); REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    wr_u16(0xc44880u,0x100); wr_u16(0xc44882u,0x140); wr_u16(0xc44884u,0xffff);
    for(i=0;i<2;++i) {
        gaddr a=0xc44980u+64*i;
        wr_u16(a,(uint16_t)random_value()); if(rd_u16(a)==0xffff) wr_u16(a,1);
        wr_u16(a+2,(uint16_t)random_value()); wr_u16(a+4,(uint16_t)random_value()); wr_u16(a+6,(uint16_t)random_value());
        wr_u16(a+20,0xffff);
    }
    for(i=0;i<8;++i) wr_u16(0xc60400u+2*i,(uint16_t)random_value()); wr_u16(0xc60414u,0xffff);
    wr_u32(0xc45b02u,(profile&1u)?100:0xffffffffu);
    wr_u32(0xc45af2u,(profile&2u)?100:101); wr_u32(0xc45af6u,fractions[(profile>>2u)%8u]);
    wr_u32(0xc45b06u,fractions[(profile>>5u)%8u]);
    wr_u32(0xc45b10u,random_value()); wr_u32(0xc45b14u,random_value()); wr_u16(0xc45b0eu,(uint16_t)random_value());
    wr_u16(0xc458ceu,(uint16_t)((profile&16u)?0x140:0));
    wr_u32(0xc45afau,(profile&8u)?0xffffffffu:100); wr_u32(0xc45afeu,0);
    wr_u32(0xc45b0au,readouts[(profile>>2u)%8u]);
    if(getenv("FA18_TIMER_ZERO_DIVISOR")) { wr_u32(0xc45afau,0); wr_u32(0xc45b0au,0); }
    wr_u16(0xc45ae8u,(uint16_t)random_value()); wr_u16(0xc458e0u,(uint16_t)random_value());
    for(i=0;i<8;++i) wr_u8(0xc45884u+i,bytes[(profile+i)%8u]);
    wr_u8(0xc45888u,(uint8_t)((profile>>1u)&1u)); wr_u8(0xc45889u,bytes[(profile>>2u)%8u]);
    wr_u16(0xc458dau,(uint16_t)random_value()); wr_u8(0xc458beu,(uint8_t)(profile%8u));
    for(i=0;i<8;++i) wr_u16(0xc2502eu+2*i,(uint16_t)((profile&64u)?250:0));
    if(selected_entry==0xc25312u) { wr_u32(0xc45b02u,0xffffffffu); wr_u32(0xc45afau,0xffffffffu); }
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    wr_u32(REG_A[7]+4,random_value());
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *reference=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    void *cpu=malloc(m68k_context_size()); char error[256];
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"main-loop-timers oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=2;
        fa18_main_loop_timers_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"main-loop-timers oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        fa18_main_loop_timers_fixture_begin("C");
        switch(selected_entry) {
        case 0xc2527cu: glue_C2527C(); break;
        case 0xc25312u: glue_C25312(); break;
        case 0xc2548au: glue_C2548A(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"main-loop-timers oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"main-loop-timers oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"main-loop-timers oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("main-loop-timers oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
