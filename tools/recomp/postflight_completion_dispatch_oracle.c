/* Complete postflight-completion proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc11788u;
static const uint32_t source_boundaries[]={0xC091E6u,0xC091EAu,0xC091F0u,0xC091F2u,0xC091F6u,0xC091F8u,0xC091FAu,0xC091FCu,0xC091FEu,0xC09202u,0xC09206u,0xC09208u,0xC0920Au,0xC0920Cu,0xC0920Eu,0xC09210u,0xC09214u,0xC09218u,0xC0921Cu,0xC0921Eu,0xC09220u,0xC09222u,0xC09226u,0xC0922Au,0xC0922Eu,0xC09230u,0xC09232u,0xC09234u,0xC09236u,0xC09238u,0xC0923Cu,0xC09240u,0xC09244u,0xC09248u,0xC0F946u,0xC0F94Cu,0xC0F94Eu,0xC0F950u,0xC0F956u,0xC0F95Cu,0xC0F95Eu,0xC0F960u,0xC0F968u,0xC0F96Cu,0xC0F972u,0xC0F974u,0xC0F97Au,0xC0F97Cu,0xC0F97Eu,0xC0F986u,0xC0F98Au,0xC0F990u,0xC1104Cu,0xC11052u,0xC11054u,0xC11056u,0xC11058u,0xC1105Eu,0xC11066u,0xC1106Cu,0xC11076u,0xC11788u,0xC1178Eu,0xC11790u,0xC11792u,0xC11798u,0xC1179Au,0xC1179Cu,0xC117A2u,0xC117A6u,0xC117ACu,0xC117B0u,0xC117B2u,0xC117B8u,0xC117BCu,0xC117C2u,0xC117C6u,0xC117CCu,0xC117D2u,0xC117DAu,0xC117DCu,0xC117E2u,0xC117E8u,0xC117ECu,0xC117F2u,0xC117F8u,0xC117FAu,0xC11800u,0xC11802u,0xC11804u,0xC1180Au,0xC11812u,0xC11818u,0xC1181Cu,0xC11822u,0xC11824u,0xC11828u,0xC1182Eu,0xC11830u,0xC11832u,0xC11838u,0xC1183Eu,0xC11842u,0xC11848u,0xC1184Eu,0xC11850u,0xC11852u,0xC11858u,0xC1185Eu,0xC11866u,0xC1186Au,0xC11870u,0xC11872u,0xC11878u,0xC1187Au,0xC1187Cu,0xC11882u,0xC11886u,0xC1188Cu,0xC11894u,0xC1189Eu,0xC118A0u,0xC118A6u,0xC118A8u,0xC118AAu,0xC118B0u,0xC118B4u,0xC118B6u,0xC118BCu,0xC118C0u,0xC118C2u,0xC118CAu,0xC118CCu,0xC118D4u,0xC118DAu,0xC118DEu,0xC118E4u,0xC118E6u,0xC118ECu,0xC118EEu,0xC118F0u,0xC118FAu,0xC118FCu,0xC11902u,0xC11904u,0xC11906u,0xC1190Cu,0xC11910u,0xC11916u,0xC1191Au,0xC11920u,0xC11928u,0xC1192Cu,0xC11932u,0xC11934u,0xC1193Au,0xC1193Cu,0xC1193Eu,0xC11940u,0xC11946u,0xC1194Cu,0xC11956u,0xC11958u,0xC1195Eu,0xC11960u,0xC11962u,0xC11968u,0xC1196Cu,0xC11972u,0xC11976u,0xC1197Cu,0xC11984u,0xC11988u,0xC1198Eu,0xC11990u,0xC11996u,0xC1199Au,0xC1199Cu,0xC1199Eu,0xC119A4u,0xC119AAu,0xC119B4u,0xC119B6u,0xC119BCu,0xC119BEu,0xC119C0u,0xC119C8u,0xC119CCu,0xC119D2u,0xC119D4u,0xC119DAu,0xC119DCu,0xC119DEu,0xC119E0u,0xC119E6u,0xC119ECu,0xC119F2u,0xC119F4u,0xC119FCu,0xC11A04u,0xC11A0Cu,0xC11A0Eu,0xC11A14u,0xC11A1Au,0xC11A24u};
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
    static const uint16_t words[]={0,1,0x7fff,0x8000,0xffff};
    static const uint8_t states[]={0,1,2,0x7f,0x80,0xff};
    static const uint8_t phases[]={0,1,2,3,0x7f,0x80,0xff};
    static const uint16_t bounds[]={0,1,0x7fff,0x8000,0xffff,0x4000,0xc000};
    unsigned i,profile=scenario;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    wr_u16(POST_INPUT_COUNTDOWN,words[(profile/8u)%5u]);
    wr_u8(CONTEXT_SELECT,(profile&1u)?1:0);
    wr_u8(PLAYER_FLAGS_A,(profile&2u)?0xff:0);
    wr_u16(CONTROL_RECORDS,(profile&4u)?0x400:0);
    wr_u8(POSTFLIGHT_RESET_REMAINING,states[(profile/3u)%6u]);
    wr_u8(SCENE_POSE_ENTRY,(uint8_t)(profile%4u));
    wr_u8(VIEWPORT_MODE,states[(profile/2u)%6u]);
    wr_u8(VIEWPORT_TARGET,(profile&8u)?states[(profile/2u)%6u]:15);
    wr_u8(MESSAGE_STATE_C,states[(profile/3u)%6u]);
    wr_u8(SEQUENCE_PHASE,phases[(profile/5u)%7u]);
    wr_u8(SEQUENCE_FLAG,(uint8_t)random_value());
    wr_u8(CONTEXT_REQUEST,(profile&16u)?0xff:0);
    wr_u8(POSTFLIGHT_FAILURE_INPUT,(profile&32u)?0x10:0xff);
    wr_u16(COCKPIT_FLAGS,(uint16_t)random_value());
    wr_u16(PLAYER_STATUS_D4,(uint16_t)random_value());
    wr_u32(LONG_TABLE,0xc60400u);
    for(i=0;i<9;++i) wr_u16(CONTROL_RECORDS+RECORD_INVERSE+2*i,
        (profile%7u)==0?0x8000u:bounds[(profile+i)%7u]);
    for(i=3;i<6;++i) {
        uint16_t low=(profile%7u)==0?0x8000u:bounds[(profile+i)%7u];
        REG_D[i]=(REG_D[i]&0xffff0000u)|low;
    }
    for(i=0;i<3;++i) wr_u32(CONTROL_RECORDS+RECORD_POSITION+4*i,random_value());
    REG_PPC=0xc10024u;
    /* Exercise saved-stack aliasing of the root position as well as the
     * usual game stack. The original MOVEM writes precede position reads. */
    REG_A[7]=(selected_entry==0xc091e6u && (profile&16u))?CONTROL_RECORDS+0x20u:0xc7ff00u;
    expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
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
        fprintf(stderr,"postflight-completion dispatch oracle: %s\n",error); return 1;
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
            fprintf(stderr,"postflight-completion dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
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
                fputs("postflight-completion ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"postflight-completion dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"postflight-completion comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/postflight_completion_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"C case %u D0=%08X sample=%08X request=%08X cycle=%lld credit=%d\n",scenario,REG_D[0],rd_u32(READOUT_SAMPLE),rd_u32(MENU_TIME_REQUEST+32),(long long)m->cycle,GET_CYCLES());
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"postflight-completion dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"postflight-completion dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"postflight-completion dispatch oracle: case %u byte %06X source %02X C %02X\n",
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
      printf("postflight-completion dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
