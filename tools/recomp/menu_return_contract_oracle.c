/* Complete menu-return proof, including complete child-entry CPU/RAM contracts. */
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
static uint32_t selected_entry=0xc1064cu;
extern void menu_return_contract_reset(unsigned scenario,int source);
extern void menu_return_contract_enter(uint32_t entry,uint32_t ret);
extern void menu_return_contract_finish(void);
static const uint32_t source_boundaries[]={0xC0FB70u,0xC0FB76u,0xC0FB78u,0xC0FB7Au,0xC0FB80u,0xC0FB82u,0xC0FB84u,0xC0FB8Au,0xC0FB92u,0xC0FB96u,0xC0FB9Cu,0xC0FB9Eu,0xC0FBA6u,0xC0FBAAu,0xC0FBAEu,0xC0FBB4u,0xC0FBB6u,0xC0FBBCu,0xC0FBBEu,0xC0FBC0u,0xC0FBC6u,0xC0FBC8u,0xC0FBD0u,0xC0FBD4u,0xC0FBD8u,0xC0FBDEu,0xC101FCu,0xC10202u,0xC10204u,0xC10206u,0xC10208u,0xC1020Eu,0xC10216u,0xC1021Cu,0xC10220u,0xC10226u,0xC10228u,0xC1022Eu,0xC10234u,0xC10236u,0xC10238u,0xC10240u,0xC10242u,0xC10248u,0xC10250u,0xC10258u,0xC1025Eu,0xC10266u,0xC1026Au,0xC10270u,0xC102D8u,0xC102DAu,0xC102E0u,0xC102E8u,0xC102EEu,0xC102F6u,0xC102FAu,0xC10300u,0xC10302u,0xC10306u,0xC1030Eu,0xC10314u,0xC10316u,0xC10318u,0xC1031Au,0xC10320u,0xC10322u,0xC10324u,0xC1032Au,0xC10330u,0xC10332u,0xC10334u,0xC10338u,0xC1033Cu,0xC10340u,0xC10342u,0xC10346u,0xC1034Au,0xC1034Eu,0xC10354u,0xC10358u,0xC1035Eu,0xC10360u,0xC10362u,0xC10368u,0xC1036Au,0xC1036Cu,0xC10372u,0xC10374u,0xC1037Cu,0xC10384u,0xC10388u,0xC1038Au,0xC10390u,0xC10396u,0xC1039Cu,0xC103A2u,0xC103A6u,0xC103A8u,0xC103B0u,0xC103B4u,0xC103BAu,0xC103BCu,0xC103C4u,0xC103CAu,0xC103D0u,0xC103D8u,0xC103DCu,0xC103E2u,0xC1064Cu,0xC10652u,0xC10654u,0xC10656u,0xC10658u,0xC1065Eu,0xC10664u,0xC1066Cu,0xC10670u,0xC10676u,0xC108FEu,0xC10900u,0xC10906u,0xC10908u,0xC1090Au,0xC1090Eu,0xC10910u,0xC10916u,0xC10918u,0xC1091Au,0xC10922u,0xC10926u,0xC1092Eu,0xC10936u,0xC1093Au,0xC10940u,0xC10942u,0xC10948u,0xC1094Au,0xC1094Cu,0xC10950u,0xC10952u,0xC10958u,0xC1095Au,0xC1095Cu,0xC10964u,0xC10968u,0xC1096Eu,0xC10970u,0xC10976u,0xC10978u,0xC1097Au,0xC1097Eu,0xC10980u,0xC10986u,0xC10988u,0xC1098Au,0xC1098Cu,0xC10992u,0xC10998u,0xC109A0u,0xC109A4u,0xC109AAu,0xC109ACu,0xC109B2u,0xC109B4u,0xC109B6u,0xC109BAu,0xC109BCu,0xC109C2u,0xC109C4u,0xC109C6u,0xC109CCu,0xC109D2u,0xC109D6u,0xC109D8u,0xC109E0u,0xC109E6u,0xC109E8u,0xC109EAu,0xC109F0u,0xC109F2u,0xC109FAu,0xC10A02u,0xC10A0Au,0xC10A0Cu,0xC10A12u,0xC10A18u,0xC10A1Cu,0xC10A22u,0xC10BAEu,0xC10BB6u,0xC10BB8u,0xC10BC0u,0xC10BC6u,0xC10BCAu,0xC10BCCu,0xC10BD2u,0xC10BD8u,0xC10BDAu,0xC10BE0u,0xC10BE6u,0xC10BE8u,0xC10BEEu,0xC10BF4u,0xC10BFCu,0xC10C00u,0xC10C06u};
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
                menu_return_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
    static const uint8_t modes[]={0,1,2,3,0x7d,0x7f,0x80,0xff};
    static const uint8_t keys[]={0,1,2,0x7f,0x80,0xff};
    static const uint8_t states[]={0,1,2,3,4,5,6,0x7f,0x80,0xff};
    static const uint8_t messages[]={0,1,2,0x7f,0x80,0xff};
    unsigned i,profile=scenario/32u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    wr_u16(POST_INPUT_COUNTDOWN,words[(profile/8u)%5u]);
    wr_u8(MODE_SELECT,modes[profile%8u]);
    wr_u8(KEY_TAKEN,keys[(profile/5u)%6u]);
    wr_u8(SEQUENCE_FLAG,(profile&16u)?0xff:0);
    wr_u8(CONTEXT_STATE,states[(profile/7u)%10u]);
    wr_u8(MESSAGE_STATE_C,messages[(profile/3u)%6u]);
    wr_u8(VIEWPORT_MODE,modes[(profile/2u)%8u]);
    wr_u8(VIEWPORT_TARGET,(profile&8u)?modes[(profile/2u)%8u]:15);
    wr_u8(POST_INPUT_AUX,(uint8_t)random_value());
    wr_u8(CONTEXT_SMOOTH,(uint8_t)random_value());
    wr_u8(CONTEXT_GATE,(uint8_t)random_value());
    wr_u8(CONTEXT_AUX,(uint8_t)random_value());
    wr_u8(CONTEXT_STARTED,(uint8_t)random_value());
    wr_u8(MENU_TRANSITION_FLAG,(uint8_t)random_value());
    wr_u8(MESSAGE_STATE_B,(uint8_t)random_value());
    wr_u16(MENU_RETURN_WORD,(uint16_t)random_value());
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
        fprintf(stderr,"menu-return oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        menu_return_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"menu-return oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        menu_return_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc1064cu: glue_C1064C(); break;
        case 0xc108feu: glue_C108FE(); break;
        case 0xc10900u: glue_C10900(); break;
        case 0xc10970u: glue_C10970(); break;
        case 0xc102d8u: glue_C102D8(); break;
        case 0xc0fb70u: glue_C0FB70(); break;
        case 0xc0fbb6u: glue_C0FBB6(); break;
        case 0xc101fcu: glue_C101FC(); break;
        case 0xc10228u: glue_C10228(); break;
        case 0xc10942u: glue_C10942(); break;
        case 0xc109acu: glue_C109AC(); break;
        case 0xc10302u: glue_C10302(); break;
        case 0xc10baeu: glue_C10BAE(); break;
        case 0xc10362u: glue_C10362(); break;
        default: return 1;
        }
        menu_return_contract_finish();
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"menu-return oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"menu-return oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"menu-return oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("menu-return oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
