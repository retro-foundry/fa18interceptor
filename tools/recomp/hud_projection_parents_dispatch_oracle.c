/* Complete HUD projection parent proof, including cold internal paths and real children. */
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
extern void fa18_hud_projection_parents_fixture_begin(const char *phase);
extern int fa18_structural_port_matched(uint32_t entry);
extern int fa18_structural_port_unused(uint32_t entry);
extern int fa18_ports_enter(int function,int label,int via_call);
extern unsigned fa18_hud_hardware_count(void);
extern void fa18_hud_hardware_begin(void);
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
static uint32_t selected_entry=0xc0daeeu;
static const uint32_t source_boundaries[]={0xC0CFFAu,0xC0CFFEu,0xC0D002u,0xC0D004u,0xC0D006u,0xC0D008u,0xC0D00Cu,0xC0D00Eu,0xC0D010u,0xC0D012u,0xC0D014u,0xC0D016u,0xC0D018u,0xC0D01Au,0xC0D01Cu,0xC0D01Eu,0xC0D020u,0xC0D022u,0xC0D024u,0xC0D02Au,0xC0D02Cu,0xC0D030u,0xC0D032u,0xC0D034u,0xC0D036u,0xC0D03Au,0xC0D03Cu,0xC0D040u,0xC0D046u,0xC0DAEEu,0xC0DAF2u,0xC0DAF6u,0xC0DAFAu,0xC0DB00u,0xC0DB02u,0xC0DB04u,0xC0DB06u,0xC0DB08u,0xC0DB0Au,0xC0DB0Cu,0xC0DB0Eu,0xC0DB10u,0xC0DB12u,0xC0DB14u,0xC0DB16u,0xC0DB18u,0xC0DB1Au,0xC0DB1Cu,0xC0DB1Eu,0xC0DB20u,0xC0DB22u,0xC0DB24u,0xC0DB26u,0xC0DB28u,0xC0DB2Au,0xC0DB2Cu,0xC0DB2Eu,0xC0DB30u,0xC0DB32u,0xC0DB3Au,0xC0DB40u,0xC32662u,0xC32668u,0xC3266Cu,0xC32672u,0xC32674u,0xC32678u,0xC32794u,0xC32796u,0xC3279Au,0xC327A0u,0xC327A4u,0xC327A6u,0xC327A8u,0xC327AAu,0xC327ACu,0xC327AEu,0xC327B0u,0xC327B2u,0xC327B4u,0xC327B8u,0xC327BAu,0xC327BCu,0xC327BEu,0xC327C0u,0xC327C2u,0xC327C6u,0xC327CAu,0xC327CCu,0xC327D2u,0xC327D6u,0xC327D8u,0xC327DAu,0xC327DCu,0xC327DEu,0xC327E0u,0xC327E4u,0xC327E6u,0xC327E8u,0xC327EAu,0xC327EEu,0xC327F0u,0xC327F4u,0xC327F6u,0xC327FEu,0xC32804u,0xC332B4u,0xC332BAu,0xC332BCu,0xC332C6u,0xC332CCu,0xC332CEu,0xC332D2u,0xC332D6u,0xC332DAu,0xC332DEu,0xC332E4u,0xC332EAu,0xC332ECu,0xC332F2u,0xC332F6u,0xC332FAu,0xC33B36u,0xC33B38u,0xC33B40u,0xC33B42u,0xC33B48u,0xC33B4Eu,0xC33B52u,0xC33B56u,0xC33B5Au,0xC33B5Cu,0xC33B62u,0xC33B66u,0xC33B6Au,0xC33B6Eu,0xC33B74u,0xC33B76u,0xC33B7Cu,0xC33B80u,0xC33B88u,0xC33B8Cu,0xC33B92u,0xC33B94u,0xC33B9Au,0xC33B9Cu,0xC33B9Eu,0xC33BA2u,0xC33BA4u,0xC33BAAu,0xC33BB0u,0xC33BB2u,0xC33BB4u,0xC33BB8u,0xC33BBAu,0xC33BC0u,0xC33BC2u,0xC33BC8u,0xC33BCAu,0xC33BD0u,0xC33BD2u,0xC33BDCu,0xC33BE4u,0xC33BEAu,0xC33BECu,0xC33BF2u,0xC33BF4u,0xC33BFAu,0xC33C04u,0xC33C0Cu,0xC33C0Eu,0xC33C12u,0xC33C18u,0xC33C20u,0xC33C22u,0xC33C26u,0xC33C2Cu,0xC33C30u,0xC33C36u,0xC33C3Eu,0xC33C42u,0xC33C48u,0xC33C4Cu,0xC33C54u,0xC33C5Au,0xC33C60u,0xC33C66u,0xC33C6Au,0xC33C6Cu,0xC33C70u,0xC33C74u,0xC33C7Au,0xC33C82u,0xC33C86u,0xC33C88u,0xC33C8Cu,0xC33C90u,0xC33C92u,0xC33C98u,0xC33C9Au,0xC33CA0u,0xC33CA2u,0xC33CA8u,0xC33CAAu,0xC33CACu,0xC33CB2u,0xC33CB8u,0xC33CBEu,0xC33CC4u,0xC33CCAu,0xC33CCCu,0xC33CD0u,0xC33CD2u,0xC33CD8u,0xC33CDEu,0xC33CE6u,0xC33CEAu,0xC33CEEu,0xC33CF2u,0xC33CF4u,0xC33CF6u,0xC33CF8u,0xC33CFEu,0xC33D00u,0xC33D02u,0xC33D04u,0xC33D06u,0xC33D08u,0xC33D0Au,0xC33D0Cu,0xC33D0Eu,0xC33D10u,0xC33D12u,0xC33D14u,0xC33D16u,0xC33D18u,0xC33D1Au,0xC33D1Cu,0xC33D1Eu,0xC33D20u,0xC33D22u,0xC33D24u,0xC33D26u,0xC33D28u,0xC33D2Au,0xC33D2Cu,0xC33D2Eu,0xC33D30u,0xC33D32u,0xC33D34u,0xC33D3Au,0xC33D42u,0xC33D46u,0xC33D4Au,0xC33D4Cu,0xC33D50u,0xC33D52u,0xC33D54u,0xC33D58u,0xC33D5Cu,0xC33D5Eu,0xC33D60u,0xC33D62u,0xC33D66u,0xC33D6Au,0xC33D6Cu,0xC33D6Eu,0xC33D72u,0xC33D76u,0xC33D78u,0xC33D7Au,0xC33D7Cu,0xC33D7Eu,0xC33D82u,0xC33D86u,0xC33D8Au,0xC33D92u,0xC33D9Au,0xC33DA2u};
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
#define HP_ORIGINAL_CHILDREN 1
#include "hud_projection_parents_fixture.h"
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
        fprintf(stderr,"HUD projection parent dispatch oracle: %s\n",error); return 1;
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
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_hud_hardware_begin(); saved_cycles=GET_CYCLES(); fa18_write_log_active=2;
        fa18_hud_projection_parents_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"HUD projection parent dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        source_hardware=fa18_write_log_hardware!=0; hardware_cases+=source_hardware; matched_cases+=!source_hardware;
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_hud_hardware_begin(); SET_CYCLES(saved_cycles);
        fa18_write_log_active=0;
        fa18_ports_init(tested_mode,selection);
        fa18_hud_projection_parents_fixture_begin("dispatch");
        {
            uint32_t previous=REG_PPC;
            int result;
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            if(fa18_ports_enter_source_only(0,&result)) { fputs("non-call source entry was accepted\n",stderr); return 1; }
            REG_PPC=previous;
            result=fa18_recomp_call_dynamic();
            if(tested_mode==FA18_PORTS_ON && (result!=FA18_EXIT_DISPATCH || fa18_ports_active_steps()!=1)) {
                fputs("HUD projection parent ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"HUD projection parent dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"HUD projection parent comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/hud_projection_parents_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"HUD projection parent dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"HUD projection parent dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"HUD projection parent dispatch oracle: case %u byte %06X source %02X C %02X\n",
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
      printf("HUD projection parent dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
