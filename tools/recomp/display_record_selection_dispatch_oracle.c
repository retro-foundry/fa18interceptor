/* Complete display-record selection proof, including cold internal paths and real children. */
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

extern int fa18_write_log_active,fa18_write_log_hardware;
extern int fa18_structural_port_classified(uint32_t entry,int hardware);
extern void fa18_structural_reset_write_log(void);
extern void fa18_render_entry_helpers_fixture_begin(const char *phase);
extern int fa18_structural_port_matched(uint32_t entry);
extern int fa18_structural_port_unused(uint32_t entry);
extern int fa18_ports_enter(int function,int label,int via_call);
extern unsigned fa18_hud_hardware_count(void);
extern void fa18_hud_hardware_begin(void);
extern void fa18_render_entry_controlled_begin(uint32_t entry,unsigned scenario);
extern void fa18_hud_hardware_reference(void);
extern int fa18_hud_hardware_check(void);
extern void fa18_render_entry_hardware_details(void);
extern int64_t fa18_next_event;
extern void fa18_render_entry_source_child(uint32_t ret,uint32_t sp);
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
        /* Match the runtime's instruction boundary, including actual due
         * blitter completion. Whole-call oracles deliberately hold events. */
        fa18_bus_finish(REG_PC);fa18_bus_instruction();
        if(fa18_machine_service())return FA18_EXIT_INTERP;
        fa18_bus_instruction();
        if(source_owned(REG_PC)) {
            uint32_t pc=REG_PC;
            uint16_t opcode=m68k_read_memory_16(pc);
            visited[source_slot(pc)]=1;
            child_ret=0;
            REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
            m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
            /* Dispatch uses the production generated-child continuation on
             * both sides. Independent whole-C proofs use original children. */
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
        /* Generated children yield at due hardware boundaries. Resume the
         * same event handoff as fa18_recomp_resume; the fixture holds service
         * at its next boundary while retaining the real device clock. */
        if(result==FA18_EXIT_INTERP && !fa18_machine_event_due()) return result;
    }
    return FA18_EXIT_INTERP;
}
#define HP_ORIGINAL_CHILDREN 1
#include "display_record_selection_fixture.h"
int main(int argc,char **argv) {
    FA18PortMode tested_mode=argc>3?(FA18PortMode)strtoul(argv[3],NULL,10):FA18_PORTS_ON;
    char selection[16];
    if(tested_mode<FA18_PORTS_ON || tested_mode>FA18_PORTS_SANDBOX) return 1;
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *reference=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    void *cpu=malloc(m68k_context_size()); char error[256];
    unsigned hardware_cases=0,matched_cases=0;
    unsigned hardware_writes=0;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"display-record selection dispatch oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    snprintf(selection,sizeof selection,"%06X",selected_entry);
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i; int saved_cycles,source_hardware;
        fa18_ports_init(FA18_PORTS_OFF,NULL);
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        wr_u16(0xc70010u,0x4e71u);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_render_entry_controlled_begin(selected_entry,scenario); saved_cycles=GET_CYCLES();
        /* Sandbox references suppress Custom writes by design. Its baseline
         * must use that same original write policy, especially for counted
         * busy reads; physical blit execution is proven by the whole oracles. */
        fa18_write_log_active=2;
        fa18_render_entry_helpers_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"display-record selection dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        source_hardware=fa18_write_log_hardware!=0; hardware_cases+=source_hardware; matched_cases+=!source_hardware;
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_render_entry_controlled_begin(selected_entry,scenario); SET_CYCLES(saved_cycles);
        fa18_write_log_active=0;
        fa18_ports_init(tested_mode,selection);
        fa18_render_entry_helpers_fixture_begin("dispatch");
        {
            uint32_t previous=REG_PPC;
            int result;
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            if(fa18_ports_enter_source_only(0,&result)) { fputs("non-call source entry was accepted\n",stderr); return 1; }
            REG_PPC=previous;
            result=fa18_recomp_call_dynamic();
            if(tested_mode==FA18_PORTS_ON && (result!=FA18_EXIT_DISPATCH || fa18_ports_active_steps()!=1)) {
                fputs("display-record selection ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"display-record selection dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"display-record selection comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/display_record_selection_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fa18_render_entry_hardware_details();fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"display-record selection dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"display-record selection dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"display-record selection dispatch oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    {
        int result;
        unsigned guard;
        for(guard=0;guard<3;++guard) {
            int function=-1,label=-1,i;
            memcpy(m,base,sizeof *m); fixture(0);
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            fa18_ports_init(guard==0?FA18_PORTS_OFF:tested_mode,guard==1?"FFFFFE":selection);
            for(i=0;i<fa18_recomp_function_count;++i)
                if(fa18_recomp_functions[i].entry==selected_entry) function=i;
            for(i=0;i<fa18_recomp_entry_count;++i)
                if(fa18_recomp_entries[i].pc==selected_entry) label=(int)fa18_recomp_entries[i].label;
            if(function>=0) {
                if(label<0) { fputs("missing translated entry label\n",stderr); return 1; }
                result=guard==2?fa18_ports_enter(function,label,0):fa18_recomp_call_dynamic();
                /* A generated child can leave its caller for runtime
                 * completion. It must never start a native step here. */
                if(result==FA18_EXIT_DISPATCH && !fa18_ports_active_steps())
                    result=fa18_recomp_resume(0xc70000u,expected_sp);
                /* A source guard may stop at an existing generated cold
                 * boundary. This proves admission/counting, not a completed
                 * source call; native continuation must still be absent. */
                if((result!=FA18_RET && result!=FA18_EXIT_INTERP) || fa18_ports_active_steps()) {
                    fprintf(stderr,"guarded translated entry result %d guard %u PC %06X active %u\n",result,guard,REG_PC,(unsigned)fa18_ports_active_steps()); return 1;
                }
            } else if(fa18_ports_enter_source_only(guard!=2,&result)) {
                fputs("guarded source entry was accepted\n",stderr); return 1;
            }
            if(!fa18_structural_port_unused(selected_entry)) {
                fputs("guarded entry counted a port call\n",stderr); return 1;
            }
        }
        fa18_ports_init(tested_mode,selection);
        REG_PC=selected_entry;
        {
            uint32_t sp=REG_A[7];
            wr_u16(selected_entry,rd_u16(selected_entry));
            result=fa18_recomp_call_dynamic();
            if(result!=FA18_EXIT_DISPATCH || REG_PC!=selected_entry || REG_A[7]!=sp || fa18_ports_active_steps()) {
                fputs("changed source still dispatched\n",stderr); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("display-record selection dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
