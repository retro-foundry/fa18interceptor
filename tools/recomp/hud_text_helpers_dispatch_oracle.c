/* Complete HUD text helper proof, including cold internal paths and real children. */
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
extern void fa18_hud_text_helpers_fixture_begin(const char *phase);
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
static uint32_t selected_entry=0xc31c20u;
static const uint32_t source_boundaries[]={0xC31C20u,0xC31C26u,0xC31C28u,0xC31C2Au,0xC31C2Cu,0xC31C2Eu,0xC31C30u,0xC31C38u,0xC31C3Au,0xC31C40u,0xC31C42u,0xC31C44u,0xC31C46u,0xC31C48u,0xC31C4Cu,0xC31C50u,0xC31C52u,0xC31C54u,0xC31C56u,0xC31C5Au,0xC31C5Cu,0xC3271Au,0xC3271Cu,0xC32720u,0xC32722u,0xC32724u,0xC32726u,0xC32728u,0xC3272Cu,0xC3272Eu,0xC32730u,0xC32736u,0xC3273Au,0xC32740u,0xC32742u,0xC32748u,0xC3274Eu,0xC32750u,0xC32752u,0xC32758u,0xC3275Au,0xC3275Eu,0xC32762u,0xC32766u,0xC32768u,0xC3276Au,0xC3276Cu,0xC3276Eu,0xC32772u,0xC32774u,0xC32776u,0xC32778u,0xC3277Au,0xC3277Eu,0xC32780u,0xC32786u,0xC3278Au,0xC32794u,0xC32796u,0xC3279Au,0xC327A0u,0xC327A4u,0xC327A6u,0xC327A8u,0xC327AAu,0xC327ACu,0xC327AEu,0xC327B0u,0xC327B2u,0xC327B4u,0xC327B8u,0xC327BAu,0xC327BCu,0xC327BEu,0xC327C0u,0xC327C2u,0xC327C6u,0xC327CAu,0xC327CCu,0xC327D2u,0xC327D6u,0xC327D8u,0xC327DAu,0xC327DCu,0xC327DEu,0xC327E0u,0xC327E4u,0xC327E6u,0xC327E8u,0xC327EAu,0xC327EEu,0xC327F0u,0xC327F4u,0xC327F6u,0xC327FEu,0xC32804u,0xC32AA4u,0xC32AA6u,0xC32AACu,0xC32AB2u,0xC32AB4u,0xC32ABAu,0xC32AC0u,0xC32AD0u,0xC32AD6u,0xC32AD8u,0xC32ADAu,0xC32ADEu,0xC32AE2u,0xC32AE4u,0xC32AE6u,0xC32AEAu,0xC32AECu,0xC32AEEu,0xC32AF0u,0xC32AF4u,0xC32AF6u,0xC32AFCu,0xC32AFEu,0xC32B00u,0xC32B02u,0xC32B06u,0xC32B0Cu,0xC32B0Eu,0xC32B10u,0xC32B12u,0xC32B14u,0xC32B18u,0xC32B1Cu,0xC32B1Eu,0xC32B20u,0xC32B22u,0xC32B24u,0xC32B26u,0xC32B2Au,0xC32B2Eu,0xC32B32u,0xC32B34u,0xC32B36u,0xC32B38u,0xC32B3Cu,0xC32B40u,0xC32B42u,0xC32B48u,0xC32B4Cu,0xC32B4Eu,0xC32B50u,0xC32B52u,0xC32B56u,0xC32B58u,0xC32B5Au,0xC32B5Eu,0xC32B66u,0xC32B68u,0xC32B6Cu,0xC32B6Eu,0xC32B72u,0xC32B76u,0xC32B78u,0xC32B7Cu,0xC32B84u,0xC32B86u,0xC32B8Au,0xC32B8Cu,0xC32B90u,0xC32B94u,0xC32B96u,0xC32B9Au,0xC32BA2u,0xC32BA4u,0xC32BA8u,0xC32BAAu,0xC32BAEu,0xC32BB2u,0xC32BB4u,0xC32BB8u,0xC32BC0u,0xC32BC2u,0xC32BC6u,0xC32BC8u,0xC32BCCu,0xC32BD0u};
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
#include "hud_text_helpers_fixture.h"
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
        fprintf(stderr,"HUD text helper dispatch oracle: %s\n",error); return 1;
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
        fa18_hud_text_helpers_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"HUD text helper dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        source_hardware=fa18_write_log_hardware!=0; hardware_cases+=source_hardware; matched_cases+=!source_hardware;
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_hud_hardware_begin(); SET_CYCLES(saved_cycles);
        fa18_write_log_active=0;
        fa18_ports_init(tested_mode,selection);
        fa18_hud_text_helpers_fixture_begin("dispatch");
        {
            uint32_t previous=REG_PPC;
            int result;
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            if(fa18_ports_enter_source_only(0,&result)) { fputs("non-call source entry was accepted\n",stderr); return 1; }
            REG_PPC=previous;
            result=fa18_recomp_call_dynamic();
            if(tested_mode==FA18_PORTS_ON && (result!=FA18_EXIT_DISPATCH || fa18_ports_active_steps()!=1)) {
                fputs("HUD text helper ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"HUD text helper dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"HUD text helper comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/hud_text_helpers_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"HUD text helper dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"HUD text helper dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"HUD text helper dispatch oracle: case %u byte %06X source %02X C %02X\n",
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
      printf("HUD text helper dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
