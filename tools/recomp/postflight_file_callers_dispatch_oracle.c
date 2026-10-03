/* Complete postflight-file-callers proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc0f56au;
static const uint32_t source_boundaries[]={0xC0EF08u,0xC0EF0Cu,0xC0EF12u,0xC0EF14u,0xC0EF16u,0xC0EF1Cu,0xC0EF1Eu,0xC0EF24u,0xC0EF26u,0xC0EF2Cu,0xC0EF32u,0xC0EF34u,0xC0EF36u,0xC0EF3Cu,0xC0EF40u,0xC0EF46u,0xC0EF48u,0xC0EF4Eu,0xC0EF50u,0xC0EF54u,0xC0EF5Au,0xC0EF5Cu,0xC0EF60u,0xC0EF66u,0xC0EF68u,0xC0EF6Eu,0xC0EF72u,0xC0EF76u,0xC0EF78u,0xC0EF7Au,0xC0EF7Cu,0xC0EF80u,0xC0EF82u,0xC0EF8Au,0xC0EF8Cu,0xC0EF8Eu,0xC0EF92u,0xC0EF94u,0xC0EF9Au,0xC0EFA2u,0xC0EFA4u,0xC0EFA6u,0xC0EFAAu,0xC0EFACu,0xC0EFB0u,0xC0EFB6u,0xC0EFB8u,0xC0EFBAu,0xC0EFBCu,0xC0EFC2u,0xC0EFC8u,0xC0EFCAu,0xC0EFCEu,0xC0EFD0u,0xC0F56Au,0xC0F56Eu,0xC0F572u,0xC0F574u,0xC0F576u,0xC0F57Au,0xC0F57Eu,0xC0F582u,0xC0F586u,0xC0F588u,0xC0F58Cu,0xC0F592u,0xC0F598u,0xC0F59Cu,0xC0F5A0u,0xC0F5A2u,0xC0F5A6u,0xC0F5AAu,0xC0F5AEu,0xC0F5B2u,0xC0F5B6u,0xC0F5BAu,0xC0F5BCu,0xC0F5C0u,0xC0F5C2u,0xC0F5C6u,0xC0F5CAu,0xC0F5CEu,0xC0F5D2u,0xC0F5D6u,0xC0F5D8u,0xC0F5DCu,0xC0F5DEu,0xC0F5E2u,0xC0F5E4u,0xC0F5E8u,0xC0F5ECu,0xC0F5EEu,0xC0F5F2u,0xC0F5F4u,0xC0F5F6u,0xC162E4u,0xC162EAu,0xC162F0u,0xC162F6u,0xC162F8u,0xC162FAu,0xC162FEu,0xC16304u,0xC16306u,0xC16308u,0xC16310u,0xC16312u,0xC16314u,0xC1631Au,0xC1631Cu,0xC16320u,0xC16326u,0xC1632Cu,0xC16332u,0xC16334u,0xC16338u,0xC1633Au,0xC1633Cu,0xC1633Eu,0xC16340u,0xC16342u,0xC16348u,0xC1634Au,0xC1634Cu,0xC1634Eu,0xC16352u,0xC16356u,0xC1635Cu,0xC16360u,0xC16364u,0xC16366u,0xC16368u,0xC1636Au,0xC1636Cu,0xC1636Eu,0xC16376u,0xC1637Au,0xC16380u,0xC16382u,0xC16384u,0xC16386u,0xC1638Au,0xC16390u,0xC16396u,0xC1639Cu,0xC1639Eu,0xC163A2u,0xC163A4u,0xC163A6u,0xC163ACu,0xC163AEu,0xC163B0u,0xC163B2u,0xC163BAu,0xC163C0u,0xC163C2u,0xC163C4u,0xC163C6u,0xC163CAu,0xC163CEu,0xC163D4u,0xC163D8u,0xC163DCu,0xC163E0u,0xC163E6u,0xC163E8u,0xC163F0u,0xC163F2u,0xC163F4u,0xC163F6u,0xC163F8u,0xC16400u,0xC16402u,0xC16404u};
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
    static const uint8_t widths[]={0,1,2,3,4,7,8,9,15,16,31,63,127,128,129,255};
    static const uint32_t numbers[]={0,1,0x12345678,0xabcdef01,0xffffffff,0x80000000,0x0000000a,0x11111111};
    unsigned i,profile=scenario; uint32_t output;
    for(i=0;i<15;++i) REG_DA[i]=random_value(); REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    wr_u32(MODE_TABLE,0xc60400u); wr_u32(0xc4fdc4u,0xc60800u);
    for(i=0;i<128;++i) wr_u8(0xc60400u+i,(uint8_t)random_value());
    wr_u16(MENU_FILE_READY,(uint16_t)random_value()); wr_u16(MENU_TABLE_STATUS,0);
    for(i=0;i<512;++i) wr_u8(0xc60e00u+i,0x30);
    /* Include output overlapping the rewritten caller arguments. */
    output=(profile&128u)?0xc7ff08u:0xc60f00u;
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    wr_u32(REG_A[7]+4,output); wr_u32(REG_A[7]+8,numbers[(profile/16u)%8u]);
    wr_u32(REG_A[7]+12,random_value()); wr_u8(REG_A[7]+15,widths[profile%16u]);
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX; fa18_recomp_abort=0;
}
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
        fprintf(stderr,"postflight-file-callers dispatch oracle: %s\n",error); return 1;
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
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"postflight-file-callers dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"source case %u D0=%08X sample=%08X request=%08X cycle=%lld credit=%d\n",scenario,REG_D[0],rd_u32(READOUT_SAMPLE),rd_u32(MENU_TIME_REQUEST+32),(long long)m->cycle,GET_CYCLES());
        source_hardware=fa18_write_log_hardware!=0; hardware_cases+=source_hardware; matched_cases+=!source_hardware;
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); SET_CYCLES(saved_cycles);
        fa18_write_log_active=0;
        fa18_ports_init(tested_mode,selection);
        {
            uint32_t previous=REG_PPC;
            int result;
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            if(fa18_ports_enter_source_only(0,&result)) { fputs("non-call source entry was accepted\n",stderr); return 1; }
            REG_PPC=previous;
            result=fa18_recomp_call_dynamic();
            if(tested_mode==FA18_PORTS_ON && (result!=FA18_EXIT_DISPATCH || fa18_ports_active_steps()!=1)) {
                fputs("postflight-file-callers ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"postflight-file-callers dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"postflight-file-callers comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/postflight_file_callers_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"C case %u D0=%08X sample=%08X request=%08X cycle=%lld credit=%d\n",scenario,REG_D[0],rd_u32(READOUT_SAMPLE),rd_u32(MENU_TIME_REQUEST+32),(long long)m->cycle,GET_CYCLES());
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"postflight-file-callers dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"postflight-file-callers dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"postflight-file-callers dispatch oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    {
        int result;
        unsigned guard;
        for(guard=0;guard<3;++guard) {
            int function=-1,i;
            memcpy(m,base,sizeof *m); fixture(0);
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            fa18_ports_init(guard==0?FA18_PORTS_OFF:tested_mode,guard==1?"FFFFFE":selection);
            for(i=0;i<fa18_recomp_function_count;++i)
                if(fa18_recomp_functions[i].entry==selected_entry) function=i;
            if(function>=0) {
                result=guard==2?fa18_ports_enter(function,0,0):fa18_recomp_call_dynamic();
                /* The unported OS child can leave its generated caller for
                 * runtime completion. It must never start a native step here. */
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
      printf("postflight-file-callers dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
