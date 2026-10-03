/* Complete menu-context-finish proof, including complete child-entry CPU/RAM contracts. */
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
static uint32_t selected_entry=0xc10a24u;
extern void menu_context_finish_contract_reset(unsigned scenario,int source);
extern void menu_context_finish_contract_enter(uint32_t entry,uint32_t ret);
extern void menu_context_finish_contract_finish(void);
static const uint32_t source_boundaries[]={0xC09192u,0xC09194u,0xC09196u,0xC0919Au,0xC0919Eu,0xC091A6u,0xC10A24u,0xC10A2Au,0xC10A2Cu,0xC10A2Eu,0xC10A32u,0xC10A34u,0xC10A3Au,0xC10A3Cu,0xC10A42u,0xC10A46u,0xC10A48u,0xC10A4Eu,0xC10A50u,0xC10A56u,0xC10A58u,0xC10A5Eu,0xC10A66u,0xC10A6Cu,0xC10A6Eu,0xC10A70u,0xC10A74u,0xC10A7Au,0xC10A82u,0xC10A88u,0xC10A90u,0xC10A92u,0xC10A96u,0xC10A9Cu,0xC10A9Eu,0xC10AA4u,0xC10AA6u,0xC10AA8u,0xC10AB0u,0xC10AB2u,0xC10AB4u,0xC10ABAu,0xC10AC0u,0xC10AC2u,0xC10AC4u,0xC10AC6u,0xC10ACCu,0xC10AD4u,0xC10ADAu,0xC10ADEu,0xC10AE4u,0xC10AE6u,0xC10AECu,0xC10AEEu,0xC10AF0u,0xC10AF8u,0xC10B00u,0xC10B04u,0xC10B06u,0xC10B0Cu,0xC10B12u,0xC10B16u,0xC10B1Cu,0xC10B1Eu,0xC10B24u,0xC10B26u,0xC10B28u,0xC10B30u,0xC10B32u,0xC10B38u,0xC10B3Au,0xC10B40u,0xC10B48u,0xC10B4Eu,0xC10B54u,0xC10B58u,0xC10B5Au,0xC10B5Eu,0xC10B60u,0xC10B68u,0xC10B6Au,0xC10B6Cu,0xC10B6Eu,0xC10B74u,0xC10B76u,0xC10B78u,0xC10B7Au,0xC10B7Cu,0xC10B82u,0xC10B84u,0xC10B88u,0xC10B8Eu,0xC10C08u,0xC10C0Eu,0xC10C10u,0xC10C12u,0xC10C1Au,0xC10C1Cu,0xC10C22u,0xC10C28u,0xC10C2Eu,0xC10C36u,0xC10C3Au,0xC10C40u,0xC10C42u,0xC10C48u,0xC10C4Au,0xC10C4Cu,0xC10C54u,0xC10C5Cu,0xC10C66u,0xC10C68u,0xC10C6Cu,0xC10C72u,0xC10C74u,0xC10C78u,0xC10C7Eu,0xC10C84u,0xC10C8Cu,0xC10C90u,0xC10C94u,0xC10C96u,0xC10C9Au,0xC10C9Cu,0xC10CA2u,0xC10CA4u,0xC10CA6u,0xC10CAAu,0xC10CAEu,0xC10CB2u,0xC10CB8u,0xC10CBAu,0xC10CC0u,0xC10CC4u,0xC10CC6u,0xC10CCAu,0xC10CCEu,0xC10CD2u,0xC10CD4u,0xC10CD8u,0xC10CDCu,0xC10CE0u,0xC10CE2u,0xC10CEAu,0xC10CEEu,0xC10CF0u,0xC10CF4u,0xC10CFAu,0xC10CFCu,0xC10CFEu,0xC10D04u,0xC10D06u,0xC10D0Au,0xC10D0Cu,0xC10D12u,0xC10D18u,0xC10D1Cu,0xC10D22u,0xC10D28u,0xC10D2Cu,0xC10D32u,0xC10D38u,0xC10D3Cu,0xC10D42u,0xC10D48u,0xC10D4Au,0xC10D4Cu,0xC10D54u,0xC10D5Au,0xC10D60u,0xC10D66u,0xC10D70u,0xC10D76u,0xC10D7Eu,0xC10D82u,0xC10D88u,0xC10D8Au,0xC10D90u,0xC10D92u,0xC10D94u,0xC10D9Cu,0xC10DA2u,0xC10DA6u,0xC10DACu,0xC10DAEu,0xC10DB2u,0xC10DBAu,0xC10DC0u,0xC10DC4u,0xC10DC8u,0xC10DCEu,0xC10DD2u,0xC10DD4u,0xC10DD8u,0xC10DDEu,0xC10DE2u,0xC10DE8u,0xC10DECu,0xC10DEEu,0xC10DF4u,0xC10DF8u,0xC10DFAu,0xC10E00u,0xC10E06u,0xC10E0Au,0xC10E10u,0xC10E16u,0xC10E1Au,0xC10E20u,0xC10E26u,0xC10E2Au,0xC10E30u,0xC10E36u,0xC10E38u,0xC10E3Au,0xC10E42u,0xC10E48u,0xC10E50u,0xC10E56u,0xC10E58u,0xC10E5Au,0xC10E5Cu,0xC10E5Eu,0xC10E64u,0xC10E66u,0xC10E70u,0xC10E74u,0xC10E7Au,0xC10E7Eu,0xC10E84u,0xC10E8Au,0xC10E90u,0xC10E92u,0xC10E96u,0xC10E9Cu,0xC10E9Eu,0xC10EA0u,0xC10EA8u,0xC10EB0u,0xC10EB6u,0xC10EC0u,0xC10ECAu,0xC10ECEu,0xC10ED2u,0xC10ED6u,0xC10EDAu,0xC10EDEu,0xC10EE2u,0xC10EE8u,0xC10EECu,0xC10EF2u,0xC10EF6u,0xC10EFAu,0xC10EFCu,0xC10F04u,0xC10F06u,0xC10F0Cu,0xC10F0Eu,0xC10F16u,0xC10F18u,0xC10F1Eu,0xC10F20u,0xC10F28u,0xC10F30u,0xC10F34u,0xC10F36u,0xC10F3Cu,0xC10F44u,0xC10F4Cu,0xC10F52u,0xC10F58u,0xC10F5Cu,0xC10F5Eu,0xC10F64u,0xC10F6Au,0xC10F70u,0xC10F76u,0xC10F78u,0xC10F7Au,0xC10F82u,0xC10F8Au,0xC10F8Cu,0xC10F92u,0xC10F9Au,0xC10FA0u,0xC10FA6u,0xC10FA8u,0xC10FAEu,0xC10FB4u,0xC10FBAu,0xC10FBEu,0xC10FC4u,0xC10FC8u,0xC10FCEu,0xC10FD2u,0xC10FD4u,0xC10FDAu,0xC10FE2u,0xC10FECu,0xC10FEEu,0xC10FF4u,0xC10FF6u,0xC11000u,0xC11002u,0xC11008u,0xC1100Au,0xC11014u,0xC11016u,0xC1101Cu,0xC1101Eu,0xC11020u,0xC11026u,0xC11028u,0xC1102Au,0xC11030u,0xC11034u,0xC11036u,0xC1103Eu,0xC11040u,0xC11048u,0xC1104Au,0xC11A26u,0xC11A2Cu,0xC11A32u,0xC11A38u,0xC11A3Au,0xC11A3Cu,0xC11A44u,0xC11A48u,0xC11A4Eu,0xC11A50u,0xC11A56u,0xC11A58u,0xC11A5Au,0xC11A62u,0xC11A68u,0xC11A70u,0xC11A76u,0xC11A78u,0xC11A7Au,0xC11A84u,0xC11A86u,0xC11A8Cu,0xC11A92u,0xC11A98u,0xC11A9Au,0xC11AA0u,0xC11AA6u,0xC11AACu,0xC11AAEu,0xC11AB4u,0xC11ABAu,0xC11AC0u,0xC11ACAu,0xC16D04u,0xC16D0Cu,0xC16D12u,0xC16D14u,0xC16D1Au,0xC16D20u,0xC16D28u,0xC16D2Eu,0xC16D34u,0xC16D36u,0xC16D40u,0xC16D4Au,0xC2506Cu,0xC2506Eu,0xC25070u,0xC25076u,0xC2507Cu,0xC2507Eu,0xC25080u,0xC25084u,0xC25088u,0xC2508Au,0xC2508Cu,0xC25090u,0xC25094u,0xC25096u,0xC2509Au,0xC2509Eu,0xC250A0u,0xC250A6u,0xC250A8u,0xC250AAu,0xC250ACu,0xC250AEu,0xC250B2u,0xC250B8u,0xC250BAu,0xC250BCu,0xC250C2u,0xC250C4u,0xC250C6u,0xC250CCu,0xC250CEu,0xC250D0u,0xC250D2u,0xC250D4u,0xC250D8u,0xC250DEu,0xC250E2u,0xC250E8u,0xC250EAu,0xC250EEu,0xC250F0u,0xC250F6u,0xC250FAu,0xC25100u,0xC25104u,0xC2510Au,0xC2510Cu,0xC25112u,0xC25118u,0xC2511Cu,0xC25120u,0xC25122u,0xC25124u,0xC2512Au,0xC25132u,0xC25136u,0xC25138u,0xC2513Au,0xC25144u,0xC25146u,0xC2514Cu,0xC2514Eu,0xC25152u,0xC25156u,0xC25158u,0xC2515Au,0xC2515Cu,0xC25160u,0xC25164u,0xC25166u,0xC25168u,0xC2516Cu,0xC25170u,0xC25172u,0xC25174u};
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
                menu_context_finish_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
    static const uint8_t phases[]={0,1,0xff,0xfe,0xfd,0xfc,0xfb,0xf0,0xef,0x80};
    unsigned i,profile=scenario/32u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[1]=CONTROL_RECORDS+512*(profile%16u); REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    wr_u16(POST_INPUT_COUNTDOWN,words[(profile/14u)%5u]);
    wr_u8(MODE_SELECT,modes[profile%14u]); wr_u8(KEY_TAKEN,(profile/3u)%4u);
    wr_u8(CONTEXT_SMOOTH,(profile&16u)?1:0); wr_u8(SCENE_POSE_ENTRY,(profile&32u)?3:4);
    wr_u8(COMMAND_ENABLE_GATE,(profile&4u)?1:0); wr_u8(CONTEXT_SELECT,((profile/3u)&1u)?1:0);
    wr_u8(POST_INPUT_EVENT,(profile&8u)?0x80:0); wr_u8(CONTEXT_STATE,(profile&32u)?6:3);
    wr_u8(MESSAGE_STATE_C,(profile&2u)?0xff:((profile&4u)?1:0));
    wr_u8(MESSAGE_STATE_B,(profile&16u)?1:0); wr_u8(SOUND_FLAGS,(profile&4u)?0x10:0);
    wr_u8(VIEWPORT_MODE,(uint8_t)(profile%16u)); wr_u8(VIEWPORT_TARGET,(profile&8u)?(uint8_t)(profile%16u):15);
    wr_u8(PLAYER_PHASE,phases[profile%10u]); wr_u8(SEQUENCE_PHASE,(profile&32u)?0xff:0);
    wr_u8(PLAYER_FLAGS_E,(profile&16u)?6:0x80); wr_u8(MENU_CONTEXT_FLAG,(profile&16u)?1:0);
    wr_u8(MENU_CONTEXT_SAVED_SELECT,(profile&64u)?1:0); wr_u8(COMMAND_BLOCK_FLAGS,(profile&32u)?1:0);
    wr_u16(CONTROL_RECORDS,(profile&64u)?0x200:0); wr_u8(CONTROL_RECORDS+4,(profile&8u)?8:0);
    wr_u16(TARGET_RECORD,(profile&1u)?1:0); wr_u16(VIEW_RECORD,(profile&4u)?512:0);
    wr_u32(MENU_TIME_PENDING,random_value()); wr_u32(MENU_TIME_TOTAL,random_value());
    wr_u32(MENU_TIME_OPTIONAL,(profile&4u)?random_value():0); wr_u32(MENU_TIME_SAVED,random_value());
    wr_u32(MODE_TABLE,0xc60000u); wr_u16(0xc60010u,words[(profile/5u)%5u]);
    wr_u32(POST_INPUT_RECORD_LIST,0xc60400u);
    for(i=0;i<16;++i) {
        gaddr record=CONTROL_RECORDS+512*i;
        wr_u16(0xc60400u+10*i,0); wr_u16(0xc60404u+10*i,(uint16_t)i);
        wr_u8(record+0x62u,((profile+i)%4u)==0?0x15:((profile+i)%4u)==1?0x30:0x11);
        wr_u8(record+1u,((profile+i)%3u)?0x40:0);
    }
    for(i=0;i<16;++i) {
        gaddr record=CONTROL_RECORDS+512*i; unsigned j;
        for(j=0;j<9;++j) wr_u16(record+RECORD_INVERSE+2*j,(j==0 || j==4 || j==8)?16:0);
        wr_u32(record+0x14u,(profile&1u)?524288u:((profile&2u)?0u-524288u:8388608u));
        wr_u32(record+0x18u,0); wr_u32(record+0x1cu,8388608u);
    }
    wr_u32(CONTROL_RECORDS+0x14u,0); wr_u32(CONTROL_RECORDS+0x1cu,0);
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
        fprintf(stderr,"menu-context-finish oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        menu_context_finish_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"menu-context-finish oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        menu_context_finish_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc10a24u: glue_C10A24(); break;
        case 0xc10c08u: glue_C10C08(); break;
        case 0xc10c68u: glue_C10C68(); break;
        case 0xc10ab2u: glue_C10AB2(); break;
        case 0xc10ae6u: glue_C10AE6(); break;
        case 0xc10b1eu: glue_C10B1E(); break;
        case 0xc10cfeu: glue_C10CFE(); break;
        case 0xc10d8au: glue_C10D8A(); break;
        case 0xc10daeu: glue_C10DAE(); break;
        case 0xc11a26u: glue_C11A26(); break;
        case 0xc11a50u: glue_C11A50(); break;
        case 0xc09192u: glue_C09192(); break;
        case 0xc16d04u: glue_C16D04(); break;
        case 0xc25070u: glue_C25070(); break;
        default: return 1;
        }
        menu_context_finish_contract_finish();
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"menu-context-finish oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"menu-context-finish oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"menu-context-finish oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("menu-context-finish oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
