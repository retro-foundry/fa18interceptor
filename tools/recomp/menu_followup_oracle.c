/* Complete menu-followup proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc1029eu;
static const uint32_t source_boundaries[]={0xC1029Eu,0xC102A4u,0xC102AAu,0xC102ACu,0xC102AEu,0xC102B0u,0xC102B6u,0xC102BEu,0xC102C4u,0xC102CCu,0xC102D0u,0xC102D6u,0xC10418u,0xC1041Eu,0xC10424u,0xC10426u,0xC10428u,0xC1042Au,0xC10430u,0xC10438u,0xC1043Eu,0xC10444u,0xC1044Cu,0xC10450u,0xC10456u,0xC10458u,0xC1045Eu,0xC10464u,0xC10466u,0xC1046Cu,0xC10472u,0xC10474u,0xC10476u,0xC1047Cu,0xC1047Eu,0xC10486u,0xC1048Au,0xC10492u,0xC10496u,0xC1049Cu,0xC1049Eu,0xC104A4u,0xC104A6u,0xC104A8u,0xC104AEu,0xC104B6u,0xC104BAu,0xC104C0u,0xC10678u,0xC1067Cu,0xC10682u,0xC10684u,0xC10688u,0xC1068Eu,0xC10694u,0xC10696u,0xC1069Eu,0xC106A6u,0xC106AEu,0xC106B2u,0xC106B8u,0xC106BAu,0xC106C0u,0xC106C2u,0xC106CAu,0xC106D0u,0xC106D4u,0xC106D6u,0xC106D8u,0xC106DCu,0xC106DEu,0xC106E2u,0xC106E4u,0xC106E6u,0xC106E8u,0xC106ECu,0xC106F0u,0xC106F4u,0xC106F6u,0xC106FAu,0xC106FEu,0xC10702u,0xC10708u,0xC1070Cu,0xC10710u,0xC10714u,0xC10718u,0xC1071Au,0xC10720u,0xC10724u,0xC1072Au,0xC1072Cu,0xC1643Au,0xC1643Eu,0xC16444u,0xC16446u,0xC1644Au,0xC16450u,0xC16456u,0xC1645Cu,0xC16462u,0xC16464u,0xC16468u,0xC1646Au,0xC16470u,0xC16472u,0xC16478u,0xC1647Au,0xC1647Cu,0xC1647Eu,0xC16480u,0xC16482u,0xC16488u,0xC1648Eu,0xC16494u,0xC16496u,0xC16498u,0xC1649Cu,0xC164A2u,0xC164A4u,0xC164A8u,0xC164AAu,0xC164B0u,0xC164B2u,0xC164B4u,0xC164B6u,0xC164BCu,0xC164BEu,0xC164C0u,0xC164C2u,0xC164C6u,0xC164CAu,0xC164D0u,0xC164D4u,0xC164D6u,0xC164DAu,0xC164E0u,0xC164E2u,0xC164E6u,0xC164ECu,0xC164EEu,0xC164F0u,0xC164F6u,0xC164F8u,0xC16500u,0xC16502u,0xC16504u,0xC16506u,0xC16508u,0xC1650Eu,0xC16510u};
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
    static const uint8_t modes[]={0,1,2,3,4,8,9,0x7f,0x80,0xff};
    unsigned i,profile=scenario/32u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    wr_u8(VIEWPORT_MODE,modes[profile%10u]);
    wr_u8(VIEWPORT_TARGET,(profile&1u)?modes[profile%10u]:modes[(profile+1u)%10u]);
    wr_u16(POST_INPUT_COUNTDOWN,words[(profile/2u)%5u]);
    wr_u8(MODE_SELECT,modes[(profile/3u)%10u]);
    wr_u8(MODE_MESSAGES_OFF,(profile&8u)?0xff:0);
    wr_u8(KEY_TAKEN,(profile&16u)?1:0);
    wr_u8(SEQUENCE_FLAG,(profile&4u)?1:0);
    wr_u16(MENU_TABLE_STATUS,(profile&1u)?words[(profile/2u)%5u]:0);
    /* The captured OS cannot return from the file path. Prove its original
       status gate here; the separate child-contract oracle covers that path. */
    if(selected_entry==0xc1643au)
        wr_u16(MENU_TABLE_STATUS,words[1u+profile%4u]);
    wr_u16(MENU_FILE_READY,(profile&8u)?0xffff:0);
    wr_u32(MODE_TABLE,0xc60000u);
    for(i=0;i<128;++i) wr_u32(0xc60000u+4*i,random_value());
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX;
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
        fprintf(stderr,"menu-followup oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"menu-followup oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        switch(selected_entry) {
        case 0xc1029eu: glue_C1029E(); break;
        case 0xc10418u: glue_C10418(); break;
        case 0xc10458u: glue_C10458(); break;
        case 0xc10678u: glue_C10678(); break;
        case 0xc1643au: glue_C1643A(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"menu-followup oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"menu-followup oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"menu-followup oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("menu-followup oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
