/* Complete display-record selection proof, including complete child-entry CPU/RAM contracts. */
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
extern unsigned fa18_hud_hardware_count(void);
extern void fa18_hud_hardware_begin(void);
extern void fa18_render_entry_controlled_begin(uint32_t entry,unsigned scenario);
extern void fa18_hud_hardware_reference(void);
extern int fa18_hud_hardware_check(void);
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
static uint32_t selected_entry=0xc0d74au;
extern void display_record_selection_contract_reset(unsigned scenario,int source);
extern void display_record_selection_contract_enter(uint32_t entry,uint32_t ret);
extern void display_record_selection_contract_finish(void);
static const uint32_t source_boundaries[]={0xC0D74Au,0xC0D750u,0xC0D752u,0xC0D758u,0xC0D75Au,0xC0D75Eu,0xC0D762u,0xC0D768u,0xC0D76Cu,0xC0D770u,0xC0D772u,0xC0D778u,0xC0D77Au,0xC0D77Cu,0xC0D77Eu,0xC0D780u,0xC0D782u,0xC0D784u,0xC0D786u,0xC0D788u,0xC0D78Au,0xC0D78Cu,0xC0D78Eu,0xC0D790u,0xC0D792u,0xC0D794u,0xC0D796u,0xC0D798u,0xC0D79Au,0xC0D79Cu,0xC0D79Eu,0xC0D7A0u,0xC0D7A2u,0xC0D7A4u,0xC0D7A6u,0xC0D7A8u,0xC0D7AAu,0xC0D7ACu,0xC0D7AEu,0xC0D7B0u,0xC0D7B2u,0xC0D7B4u,0xC0D7B6u,0xC0D7B8u,0xC0D7BAu,0xC0D7BCu,0xC0D7BEu,0xC0D7C0u,0xC0D7C2u,0xC0D7C4u,0xC0D7C8u,0xC0D7CAu,0xC0D7CCu,0xC0D7D2u,0xC0D7D4u,0xC0D7D6u,0xC0D7D8u,0xC0D7DAu,0xC0D7E0u,0xC0D7E6u,0xC0D7EAu,0xC0D7ECu,0xC0D7F0u,0xC0D7F2u,0xC0D7F6u,0xC0D7FAu,0xC0D7FEu,0xC0D802u,0xC0D804u,0xC0D808u,0xC0D80Cu,0xC0D810u,0xC0D814u,0xC0D816u,0xC0D81Au,0xC0D81Eu,0xC0D822u,0xC0D826u,0xC0D82Au,0xC0D82Cu,0xC0D830u,0xC0D832u,0xC0D836u,0xC0D83Au,0xC0D83Eu,0xC0D842u,0xC0D844u,0xC0D848u,0xC0D84Cu,0xC0D850u,0xC0D854u,0xC0D858u,0xC0D85Cu,0xC0D860u,0xC0D862u,0xC0D866u,0xC0D86Au,0xC0D86Eu,0xC0D872u,0xC0D878u,0xC0D87Cu,0xC0D882u,0xC0D884u,0xC0D88Au,0xC0D88Eu,0xC0D890u,0xC0D894u,0xC0D898u,0xC0D89Cu,0xC0D8A2u,0xC0D8A6u,0xC0D8AAu,0xC0D8AEu,0xC0D8B2u,0xC0D8B6u,0xC0D8BAu,0xC0D8BEu,0xC0D8C4u,0xC0D8C8u,0xC0D8CEu,0xC0D8D2u,0xC0D8D4u,0xC0D8D8u,0xC0D8DCu,0xC0D8E0u,0xC0D8E4u,0xC0D8EAu,0xC0D8EEu,0xC0D8F0u,0xC0D8F4u,0xC0D8F8u,0xC0D8FCu,0xC0D900u,0xC0D904u,0xC0D908u,0xC0D90Cu,0xC0D912u,0xC0D916u,0xC0D91Cu,0xC0D920u,0xC0D922u,0xC0D926u,0xC0D92Au,0xC0D92Eu,0xC0D932u,0xC0D936u,0xC0D93Au,0xC0D93Eu,0xC0D942u,0xC0D946u,0xC0D94Cu,0xC0D950u,0xC0D952u,0xC0D956u,0xC0D95Au,0xC0D960u,0xC0D964u,0xC0D96Au,0xC0D96Eu,0xC0D970u,0xC0D974u,0xC0D978u,0xC0D97Cu,0xC0D980u,0xC0D986u,0xC0D98Au,0xC0D98Cu,0xC0D990u,0xC0D994u,0xC0D998u,0xC0D99Cu,0xC0D9A0u,0xC0D9A4u,0xC0D9A8u,0xC0D9AEu,0xC0D9B2u,0xC0D9BAu,0xC0D9BCu,0xC0D9C0u,0xC0D9C4u,0xC0D9C8u,0xC0D9CEu,0xC0D9D2u,0xC0D9D6u,0xC0D9DAu,0xC0D9DEu,0xC0D9E2u,0xC0D9E6u,0xC0D9EAu,0xC0D9F0u,0xC0D9F4u,0xC0D9FAu,0xC0D9FEu,0xC0DA00u,0xC0DA04u,0xC0DA08u,0xC0DA0Cu,0xC0DA10u,0xC0DA14u,0xC0DA18u,0xC0DA1Cu,0xC0DA20u,0xC0DA24u,0xC0DA2Au,0xC0DA2Eu,0xC0DA30u,0xC0DA34u,0xC0DA38u,0xC0DA3Eu,0xC0DA42u,0xC0DA46u,0xC0DA4Au,0xC0DA4Eu,0xC0DA52u,0xC0DA58u,0xC0DA5Au,0xC0DA60u,0xC0DA62u,0xC0DA68u,0xC0DA6Cu,0xC0DA6Eu,0xC0DA70u,0xC0DA78u,0xC0DA80u,0xC0DA86u,0xC0DA8Eu,0xC0DA90u,0xC0DA92u,0xC0DA94u,0xC0DA9Au,0xC0DA9Cu,0xC0DA9Eu,0xC0DAA0u,0xC0DAA2u,0xC0DAA4u,0xC0DAAAu,0xC0DAAEu,0xC0DAB0u,0xC0DAB4u,0xC0DAB8u,0xC0DABAu,0xC0DABEu,0xC0DAC0u,0xC0DAC2u,0xC0DAC6u,0xC0DACAu,0xC0DACCu,0xC0DACEu,0xC0DAD0u,0xC0DAD2u,0xC0DAD4u,0xC0DAD8u,0xC0DADAu,0xC0DADCu,0xC0DAE0u,0xC0DAE4u,0xC0DAE6u,0xC0DAE8u,0xC0DAECu};
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
            if((opcode&0xff00u)==0x6100u || (opcode&0xffc0u)==0x4e80u)
                display_record_selection_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
#include "display_record_selection_fixture.h"
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *reference=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    void *cpu=malloc(m68k_context_size()); char error[256];
    unsigned hardware_writes=0;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"display-record selection oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_render_entry_controlled_begin(selected_entry,scenario); fa18_write_log_active=2;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        display_record_selection_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"display-record selection oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_render_entry_controlled_begin(selected_entry,scenario);
        display_record_selection_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc0d74au: glue_C0D74A(); break;
        case 0xc0d752u: glue_C0D752(); break;
        case 0xc0daa0u: glue_C0DAA0(); break;
        case 0xc0dad0u: glue_C0DAD0(); break;
        case 0xc0dad4u: glue_C0DAD4(); break;
        case 0xc0dadcu: glue_C0DADC(); break;
        case 0xc0dae6u: glue_C0DAE6(); break;
        default:return 1;
        }
        display_record_selection_contract_finish();
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"display-record selection oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"display-record selection oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"display-record selection oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("display-record selection oracle %06X: %u %s matched all registers, PC, full SR and all RAM; %u source boundaries observed\n",selected_entry,cases,"complete calls",count);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
