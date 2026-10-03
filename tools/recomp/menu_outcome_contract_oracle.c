/* Complete menu-outcome proof, including complete child-entry CPU/RAM contracts. */
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
static uint32_t selected_entry=0xc104c2u;
extern void menu_outcome_contract_reset(unsigned scenario,int source);
extern void menu_outcome_contract_enter(uint32_t entry,uint32_t ret);
extern void menu_outcome_contract_finish(void);
static const uint32_t source_boundaries[]={0xC104C2u,0xC104C6u,0xC104CAu,0xC104CCu,0xC104D2u,0xC104D8u,0xC104DEu,0xC104E0u,0xC104E6u,0xC104ECu,0xC104EEu,0xC104F0u,0xC104F6u,0xC104F8u,0xC10500u,0xC10504u,0xC1050Cu,0xC10510u,0xC10516u,0xC1051Au,0xC10520u,0xC10522u,0xC10524u,0xC1052Au,0xC10530u,0xC10532u,0xC10536u,0xC1053Au,0xC1053Cu,0xC10540u,0xC10542u,0xC1054Au,0xC1054Cu,0xC10550u,0xC10556u,0xC1055Au,0xC1055Cu,0xC1055Eu,0xC10560u,0xC10562u,0xC10564u,0xC10568u,0xC1056Au,0xC1056Cu,0xC1056Eu,0xC10570u,0xC10576u,0xC1057Au,0xC1057Cu,0xC1057Eu,0xC10582u,0xC10584u,0xC10586u,0xC10588u,0xC1058Au,0xC10590u,0xC10594u,0xC10598u,0xC1059Eu,0xC105A2u,0xC105A4u,0xC105A6u,0xC105ACu,0xC105AEu,0xC105B0u,0xC105B6u,0xC105B8u,0xC105BEu,0xC105C4u,0xC105C6u,0xC105CCu,0xC105D4u,0xC105D8u,0xC105E0u,0xC105E8u,0xC105ECu,0xC105F2u,0xC105F4u,0xC105FAu,0xC105FCu,0xC105FEu,0xC10600u,0xC10606u,0xC1060Cu,0xC10614u,0xC1061Au,0xC1061Eu,0xC10624u,0xC10626u,0xC1062Cu,0xC1062Eu,0xC10630u,0xC10638u,0xC10640u,0xC10644u,0xC1064Au,0xC1072Eu,0xC10734u,0xC10736u,0xC10738u,0xC10740u,0xC10746u,0xC1074Eu,0xC10752u,0xC10758u,0xC1075Au,0xC10760u,0xC10762u,0xC10768u,0xC1076Au,0xC10772u,0xC10776u,0xC1077Eu,0xC10782u,0xC10788u,0xC1078Au,0xC1078Eu,0xC10794u,0xC10796u,0xC1079Au,0xC107A0u,0xC107A2u,0xC107A8u,0xC107ACu,0xC107AEu,0xC107B2u,0xC107B6u,0xC107BCu,0xC107C0u,0xC107C2u,0xC107C6u,0xC107C8u,0xC107D0u,0xC107D2u,0xC107D6u,0xC107D8u,0xC107DAu,0xC107DCu,0xC107DEu,0xC107E0u,0xC107E6u,0xC107EAu,0xC107F0u,0xC107F4u,0xC107F6u,0xC107FCu,0xC10800u,0xC10808u,0xC1080Cu,0xC1080Eu,0xC10812u,0xC10814u,0xC1081Au,0xC1081Eu,0xC10822u,0xC10824u,0xC1082Cu,0xC10834u,0xC1083Cu,0xC10844u,0xC10848u,0xC1084Cu,0xC10852u,0xC10856u,0xC1085Au,0xC10860u,0xC10862u,0xC10866u,0xC1086Cu,0xC1086Eu,0xC10876u,0xC1087Eu,0xC10882u,0xC10888u,0xC1088Au,0xC10890u,0xC10892u,0xC10894u,0xC1089Au,0xC108A2u,0xC108A6u,0xC108AEu,0xC108B4u,0xC108B6u,0xC108BCu,0xC108BEu,0xC108C0u,0xC108C4u,0xC108CAu,0xC108CCu,0xC108D0u,0xC108D6u,0xC108D8u,0xC108DAu,0xC108E0u,0xC108E2u,0xC108E4u,0xC108EAu,0xC108F2u,0xC108F6u,0xC108FCu,0xC29368u,0xC29370u,0xC29376u,0xC2937Eu,0xC29384u,0xC2938Au,0xC2938Cu,0xC2938Eu,0xC29392u,0xC29396u,0xC29398u,0xC2939Au,0xC2939Eu,0xC293A2u,0xC293A6u,0xC293A8u,0xC293AEu,0xC293B0u,0xC293B6u,0xC293BCu,0xC293BEu,0xC293C0u,0xC293C6u,0xC293C8u,0xC293D0u,0xC293D6u,0xC293DCu,0xC293DEu,0xC293E4u,0xC293ECu,0xC293F0u,0xC293F4u,0xC293FAu,0xC293FCu,0xC293FEu,0xC29400u,0xC29408u};
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
                menu_outcome_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
    static const uint8_t modes[]={0,1,2,3,4,5,6,7,8,9,0x7d,0x7f,0x80,0xff};
    static const uint8_t requests[]={0,1,0x7f,0x80,0xff};
    static const uint8_t states[]={0,1,2,3,0x7f,0x80,0xff};
    static const uint8_t attempts[]={0,1,2,0x7f,0x80,0xff};
    unsigned i,profile=scenario/32u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    wr_u16(POST_INPUT_COUNTDOWN,words[(profile/14u)%5u]);
    wr_u8(MODE_SELECT,modes[profile%14u]);
    wr_u8(MODE_MESSAGES_OFF,(uint8_t)random_value());
    wr_u8(KEY_TAKEN,(profile&128u)?1:0);
    wr_u8(SEQUENCE_FLAG,(profile&64u)?1:0);
    wr_u8(CONTEXT_REQUEST,requests[(profile/7u)%5u]);
    wr_u8(CONTEXT_SELECT,(uint8_t)((profile/3u)&1u));
    wr_u8(MESSAGE_STATE_C,states[(profile/2u)%7u]);
    wr_u8(ATTEMPTS_LEFT,attempts[(profile/5u)%6u]);
    wr_u32(MODE_TABLE,0xc60000u);
    for(i=0;i<128;++i) { wr_u32(0xc60000u+4*i,random_value()); wr_u32(0xc60100u+4*i,random_value()); }
    for(i=3;i<=8;++i) wr_u8(0xc60012u+i,((profile/2u)&1u)?0xff:0);
    wr_u32(ORIGIN_RECORD_LIST,0xc60400u);
    for(i=0;i<16;++i) {
        gaddr record=CONTROL_RECORDS+512*i;
        wr_u16(0xc60400u+10*i,0); wr_u16(0xc60404u+10*i,(uint16_t)i);
        wr_u8(record+0x62u,((profile+i)%3u)?0x11:0x30);
        wr_u8(record+1u,(uint8_t)(((profile+i)%4u)?0x40:0)|((profile&16u)?8u:0));
    }
    wr_u16(0xc60400u+10*(profile%17u),0xffffu);
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
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
        fprintf(stderr,"menu-outcome oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        menu_outcome_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"menu-outcome oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        menu_outcome_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc104c2u: glue_C104C2(); break;
        case 0xc105f4u: glue_C105F4(); break;
        case 0xc1072eu: glue_C1072E(); break;
        case 0xc1078au: glue_C1078A(); break;
        case 0xc105a6u: glue_C105A6(); break;
        case 0xc10626u: glue_C10626(); break;
        case 0xc1075au: glue_C1075A(); break;
        case 0xc108dau: glue_C108DA(); break;
        case 0xc29368u: glue_C29368(); break;
        default: return 1;
        }
        menu_outcome_contract_finish();
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"menu-outcome oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"menu-outcome oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"menu-outcome oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("menu-outcome oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
