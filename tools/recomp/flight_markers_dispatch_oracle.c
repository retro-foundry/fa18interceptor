/* Complete flight-markers proof, including cold internal paths and real children. */
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
extern void fa18_flight_markers_fixture_begin(const char *phase);
extern int fa18_structural_port_matched(uint32_t entry);
extern int fa18_structural_port_unused(uint32_t entry);
extern int fa18_ports_enter(int function,int label,int via_call);
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
static uint32_t selected_entry=0xc2affau;
static const uint32_t source_boundaries[]={0xC2AFFAu,0xC2AFFCu,0xC2AFFEu,0xC2B000u,0xC2B002u,0xC2B004u,0xC2B00Au,0xC2B00Cu,0xC2B00Eu,0xC2B010u,0xC2B012u,0xC2B014u,0xC2B016u,0xC2B018u,0xC2B01Au,0xC2B01Cu,0xC2B01Eu,0xC2B020u,0xC2B022u,0xC2B024u,0xC2B026u,0xC2B028u,0xC2B02Au,0xC2B02Cu,0xC2B02Eu,0xC2B030u,0xC2B032u,0xC2B034u,0xC2B036u,0xC2B038u,0xC2B03Au,0xC2B03Cu,0xC2B03Eu,0xC2B040u,0xC2B3C0u,0xC2B3C2u,0xC2B3CAu,0xC2B3CCu,0xC2B3D4u,0xC2B3D6u,0xC2B3E0u,0xC2B3E2u,0xC2B3E8u,0xC2B3EEu,0xC2B3F0u,0xC2B3F6u,0xC2B3F8u,0xC2B3FEu,0xC2B404u,0xC2B408u,0xC2B40Eu,0xC2B416u,0xC2B41Eu,0xC2B420u,0xC2B426u,0xC2B428u,0xC2B42Eu,0xC2B434u,0xC2B436u,0xC2B43Eu,0xC2B440u,0xC2B446u,0xC2B448u,0xC2B44Eu,0xC2B452u,0xC2B454u,0xC2B458u,0xC2B45Au,0xC2B45Eu,0xC2B460u,0xC2B464u,0xC2B466u,0xC2B468u,0xC2B46Cu,0xC2B46Eu,0xC2B470u,0xC2B474u,0xC2B476u,0xC2B478u,0xC2B47Eu,0xC2B480u,0xC2B486u,0xC2B48Au,0xC2B48Eu,0xC2B492u,0xC2B496u,0xC2B498u,0xC2B49Au,0xC2B49Cu,0xC2B49Eu,0xC2B4A0u,0xC2B4A6u,0xC2B4ACu,0xC2B4AEu,0xC2B4B0u,0xC2B4B2u,0xC2B4B4u,0xC2B4B6u,0xC2B4B8u,0xC2B4BCu,0xC2B4BEu,0xC2B4C0u,0xC2B4C2u,0xC2B4C4u,0xC2B4C6u,0xC2B4C8u,0xC2B4CAu,0xC2B4CCu,0xC2B4CEu,0xC2B4D0u,0xC2B4D4u,0xC2B4D6u,0xC2B4D8u,0xC2B4DAu,0xC2B4DCu,0xC2B4DEu,0xC2B4E0u,0xC2B4E2u,0xC2B4E4u,0xC2B4E6u,0xC2B4ECu,0xC2B4EEu,0xC2B4F0u,0xC2B4F2u,0xC2B4F4u,0xC2B4F6u,0xC2B4F8u,0xC2B4FAu,0xC2B4FCu,0xC2B4FEu,0xC2B500u,0xC2B502u,0xC2B504u,0xC2B506u,0xC2B508u,0xC2B50Au,0xC2B50Cu,0xC2B50Eu,0xC2B510u,0xC2B512u,0xC2B514u,0xC2B516u,0xC2B518u,0xC2B51Au,0xC2B51Cu,0xC2B520u,0xC2B528u,0xC2B52Eu,0xC2B534u,0xC2B536u,0xC2B53Cu,0xC2B53Eu,0xC2B544u,0xC2B546u,0xC2B54Eu,0xC2B554u,0xC2B558u,0xC2B55Eu,0xC2B562u,0xC2B564u,0xC2B56Au,0xC2B56Cu,0xC2B572u,0xC2B574u,0xC2B578u,0xC2B582u,0xC2B58Au,0xC2B592u,0xC2B594u,0xC2B596u,0xC2B598u,0xC2B59Cu,0xC2B59Eu,0xC2B5A0u,0xC2B5A2u,0xC2B5A4u,0xC2B5A6u,0xC2B5A8u,0xC2B5AAu,0xC2B5AEu,0xC2B5B4u,0xC2B5BAu,0xC2B5C0u,0xC2B5C6u,0xC2B5CAu,0xC2B5CEu,0xC2B5D2u,0xC2B5D8u,0xC2B5DCu,0xC2B5E0u,0xC2B5E4u,0xC2B5EAu,0xC2B5EEu,0xC2B5F6u,0xC2B5FCu,0xC2B5FEu,0xC2B602u,0xC2B608u,0xC2B60Au,0xC2B60Eu,0xC2B610u,0xC2B612u,0xC2B616u,0xC2B618u,0xC2B61Eu,0xC2B620u,0xC2B622u,0xC2B626u,0xC2B628u,0xC2B62Cu,0xC2B62Eu,0xC2B632u,0xC2B634u,0xC2B638u,0xC2B63Au,0xC2B63Eu,0xC2B646u,0xC2B64Cu,0xC2B650u,0xC2B656u,0xC2B65Cu,0xC2B65Eu,0xC2B662u,0xC2B666u,0xC2B66Au,0xC2B670u,0xC2B676u,0xC2B67Cu,0xC2B67Eu,0xC2B680u,0xC2B684u,0xC2B686u,0xC2B68Cu,0xC2B690u,0xC2B694u,0xC2B698u,0xC2B69Eu,0xC2B6A2u,0xC2B6A6u,0xC2B6AAu,0xC2B6B0u,0xC2B6B4u,0xC2B6BCu,0xC2B6C2u,0xC2B6C4u,0xC2B6C8u,0xC2B6CEu,0xC2B6D0u,0xC2B6D4u,0xC2B6D6u,0xC2B6D8u,0xC2B6DCu,0xC2B6DEu,0xC2B6E4u,0xC2B6E6u,0xC2B6E8u,0xC2B6EAu,0xC2B6EEu,0xC2B6F0u,0xC2B6F2u,0xC2B6F6u,0xC2B6F8u,0xC2B6FCu,0xC2B6FEu,0xC2B700u,0xC2B702u,0xC2B70Au,0xC2B710u,0xC2B714u,0xC2B71Au,0xC2B720u,0xC2B722u,0xC2B726u,0xC2B72Au,0xC2B72Eu,0xC2B730u,0xC2B732u,0xC2B738u,0xC2B73Au,0xC2B73Eu,0xC2B740u,0xC2B744u,0xC2B748u,0xC2B74Cu,0xC2B74Eu,0xC2B754u,0xC2B758u,0xC2B75Eu,0xC2B760u,0xC2B762u,0xC2B768u,0xC2B76Au,0xC2B76Cu,0xC2B770u,0xC2B776u,0xC2B778u,0xC2B77Au,0xC2B782u,0xC2B786u,0xC2B78Au,0xC2B78Eu,0xC2B790u,0xC2B794u,0xC2B796u,0xC2B79Eu,0xC2B7A0u,0xC2B7A6u,0xC2B7A8u,0xC2B7ACu,0xC2B7B0u,0xC2B7B2u,0xC2B7BAu,0xC2B7BCu,0xC2B7C4u,0xC2B7C6u,0xC2B7C8u,0xC2B7CAu,0xC2B7CCu,0xC2B7D0u,0xC2B7D2u,0xC2B7D4u,0xC2B7D8u,0xC2B7DCu,0xC2B7E0u,0xC2B7E2u,0xC2B928u,0xC2B92Eu,0xC2B930u,0xC2B938u,0xC2B93Eu,0xC2B940u,0xC2B948u,0xC2B94Cu,0xC2B950u,0xC2B952u,0xC2B95Au,0xC2B95Eu,0xC2B962u,0xC2B964u,0xC2B968u,0xC2B96Au,0xC2B970u,0xC2B978u,0xC2B97Eu,0xC2B980u,0xC2B986u,0xC2B988u,0xC2B98Au,0xC2B990u,0xC2B996u,0xC2B998u,0xC2B99Eu,0xC2B9A0u,0xC2B9A4u,0xC2B9A6u,0xC2B9AAu,0xC2B9ACu,0xC2B9B0u,0xC2B9B2u,0xC2B9B6u,0xC2B9B8u,0xC2B9C0u,0xC2B9C6u,0xC2B9CAu,0xC2B9D0u,0xC2B9D6u,0xC2B9DAu,0xC2B9DCu,0xC2B9DEu,0xC2B9E0u,0xC2B9E4u,0xC2B9E8u,0xC2B9ECu,0xC2B9EEu,0xC2B9F2u,0xC2B9F4u,0xC2B9F6u,0xC2B9FAu,0xC2B9FCu,0xC2BA00u,0xC2BA02u,0xC2BA04u,0xC2BA08u,0xC2BA0Au,0xC2BA0Eu,0xC2BA10u,0xC2BA14u,0xC2BA16u,0xC2BA1Au,0xC2BA1Cu,0xC2BA1Eu,0xC2BA20u,0xC2BA22u,0xC2BA26u,0xC2BA28u,0xC2BA2Au,0xC2BA2Eu,0xC2BA30u,0xC2BA34u,0xC2BA36u,0xC2BA3Au,0xC2BA3Cu,0xC2BA40u,0xC2BA42u,0xC2BA44u,0xC2BA46u,0xC2BA4Au,0xC2BA4Eu,0xC2BA52u,0xC2BA56u,0xC2BA58u,0xC2BA5Au,0xC2BA5Cu,0xC2BA60u,0xC2BA62u,0xC2BA66u,0xC2BA68u,0xC2BA6Au,0xC2BA6Cu,0xC2BA6Eu,0xC2BA70u,0xC2BA74u,0xC2BA76u,0xC2BA78u,0xC2BA7Cu,0xC2BA80u,0xC2BA82u,0xC2BA84u,0xC2BA88u,0xC2BA8Eu,0xC2BA90u,0xC2BA98u,0xC2BA9Au,0xC2BA9Cu,0xC2BA9Eu,0xC2BAA2u,0xC2BAA6u,0xC2BAA8u,0xC2BAAAu,0xC2BAAEu,0xC2BAB0u,0xC2BAB2u,0xC2BAB4u,0xC2BAB6u,0xC2BAB8u,0xC2BABAu,0xC2BABCu,0xC2BABEu,0xC2BAC0u,0xC2BAC2u,0xC2BAC4u,0xC2BAC6u,0xC2BAC8u,0xC2BACCu,0xC2BACEu,0xC2BAD0u,0xC2BAD2u,0xC2BAD4u,0xC2BAD6u,0xC2BAD8u,0xC2BADCu,0xC2BADEu,0xC2BAE0u,0xC2BAE6u,0xC2BAE8u,0xC2BAEAu,0xC2BAECu,0xC2BAEEu};
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
#include "flight_markers_fixture.h"
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
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"flight-markers dispatch oracle: %s\n",error); return 1;
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
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); saved_cycles=GET_CYCLES(); fa18_write_log_active=2;
        fa18_flight_markers_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"flight-markers dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        source_hardware=fa18_write_log_hardware!=0; hardware_cases+=source_hardware; matched_cases+=!source_hardware;
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(saved_cycles);
        fa18_write_log_active=0;
        fa18_ports_init(tested_mode,selection);
        fa18_flight_markers_fixture_begin("dispatch");
        {
            uint32_t previous=REG_PPC;
            int result;
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            if(fa18_ports_enter_source_only(0,&result)) { fputs("non-call source entry was accepted\n",stderr); return 1; }
            REG_PPC=previous;
            result=fa18_recomp_call_dynamic();
            if(tested_mode==FA18_PORTS_ON && (result!=FA18_EXIT_DISPATCH || fa18_ports_active_steps()!=1)) {
                fputs("flight-markers ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"flight-markers dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"flight-markers comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/flight_markers_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"flight-markers dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"flight-markers dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"flight-markers dispatch oracle: case %u byte %06X source %02X C %02X\n",
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
                if(result!=FA18_RET || fa18_ports_active_steps()) {
                    fputs("guarded translated entry started a native continuation\n",stderr); return 1;
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
      printf("flight-markers dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
