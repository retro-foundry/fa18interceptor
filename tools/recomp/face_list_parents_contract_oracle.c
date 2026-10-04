/* Complete face-list/edge parent proof, including complete child-entry CPU/RAM contracts. */
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
static uint32_t selected_entry=0xc1ff0au;
extern void face_list_parents_contract_reset(unsigned scenario,int source);
extern void face_list_parents_contract_enter(uint32_t entry,uint32_t ret);
extern void face_list_parents_contract_finish(void);
static const uint32_t source_boundaries[]={0xC1FF0Au,0xC1FF10u,0xC1FF16u,0xC1FF1Au,0xC1FF1Eu,0xC1FF22u,0xC1FF26u,0xC1FF2Au,0xC1FF2Eu,0xC1FF32u,0xC1FF34u,0xC1FF36u,0xC1FF3Cu,0xC1FF3Eu,0xC1FF40u,0xC1FF44u,0xC20002u,0xC20008u,0xC2000Eu,0xC20014u,0xC20018u,0xC2001Cu,0xC20020u,0xC20024u,0xC20028u,0xC2002Cu,0xC20030u,0xC20036u,0xC20038u,0xC2003Au,0xC2003Cu,0xC2003Eu,0xC20040u,0xC20046u,0xC20048u,0xC2004Au,0xC2004Cu,0xC2004Eu,0xC20050u,0xC20052u,0xC20054u,0xC20056u,0xC2005Au,0xC2005Cu,0xC20062u,0xC20064u,0xC20066u,0xC2006Cu,0xC2006Eu,0xC20070u,0xC20074u,0xC20076u,0xC20078u,0xC2007Au,0xC2007Cu,0xC20080u,0xC20082u,0xC20084u,0xC20086u,0xC20088u,0xC2008Au,0xC2008Eu,0xC20090u,0xC20092u,0xC20094u,0xC20096u,0xC20098u,0xC2009Cu,0xC200A0u,0xC200A2u,0xC200A4u,0xC200A6u,0xC200A8u,0xC200AEu,0xC200B0u,0xC200B2u,0xC200B4u,0xC200B8u,0xC200BEu,0xC200C0u,0xC200C2u,0xC200C6u,0xC200C8u,0xC200CAu,0xC200CCu,0xC200D0u,0xC200D2u,0xC200D4u,0xC200D8u,0xC200DAu,0xC200DCu,0xC200E2u,0xC200E6u,0xC200ECu,0xC200F0u,0xC200F2u,0xC200F4u,0xC200F6u,0xC200FAu,0xC200FEu,0xC20100u,0xC20104u,0xC20108u,0xC2010Eu,0xC20110u,0xC20112u,0xC20114u,0xC20116u,0xC20118u,0xC2011Eu,0xC20124u,0xC20126u,0xC20128u,0xC2012Cu,0xC20130u,0xC20132u,0xC20134u,0xC20138u,0xC2013Cu,0xC20140u,0xC20142u,0xC20144u,0xC20148u,0xC2014Cu,0xC20150u,0xC20152u,0xC20154u,0xC20158u,0xC2015Cu,0xC20160u,0xC20164u,0xC20166u,0xC2016Cu,0xC2016Eu,0xC20170u,0xC20172u,0xC20178u,0xC2017Au,0xC2017Cu,0xC20180u,0xC20182u,0xC20184u,0xC20186u,0xC20188u,0xC2018Eu,0xC20190u,0xC20196u,0xC2019Au,0xC2019Cu,0xC2019Eu,0xC20826u,0xC20828u,0xC2082Au,0xC20830u,0xC20834u,0xC20836u,0xC20840u,0xC20842u,0xC20848u,0xC2084Au,0xC2084Cu,0xC2084Eu,0xC20850u,0xC20852u,0xC20854u,0xC20856u,0xC20858u,0xC2085Cu,0xC20862u,0xC20866u,0xC2086Au,0xC20870u,0xC20872u,0xC20876u,0xC2087Cu,0xC20880u,0xC20886u,0xC2088Au,0xC2088Cu,0xC2088Eu,0xC20890u,0xC20892u,0xC20894u,0xC20896u,0xC20898u,0xC2089Au,0xC208A0u,0xC208AAu,0xC208ACu,0xC208B2u,0xC208B6u,0xC208B8u,0xC208BCu,0xC208BEu,0xC208C2u,0xC208C4u,0xC208C8u,0xC208CAu,0xC208CCu,0xC208CEu,0xC208D0u,0xC208D2u,0xC20A40u,0xC20A4Au,0xC20A50u,0xC20A52u,0xC20A5Au,0xC20A5Eu,0xC20A62u,0xC20A66u,0xC20A68u,0xC20A70u,0xC20A76u,0xC20A78u,0xC20A7Cu,0xC20A80u,0xC20A84u,0xC20A88u,0xC20A8Cu,0xC20A8Eu,0xC20A92u,0xC20A96u,0xC20A98u,0xC20A9Cu,0xC20AA0u,0xC20AA6u,0xC20AA8u,0xC20AAAu,0xC20AACu,0xC20AB0u,0xC20AB2u,0xC20AB4u,0xC20AB6u,0xC20ABAu,0xC20ABEu,0xC20AC2u,0xC20AC8u,0xC20ACCu,0xC20ACEu,0xC20AD0u,0xC20AD2u,0xC20AD6u,0xC20ADAu,0xC20ADEu,0xC20AE4u,0xC20AE8u,0xC20AEEu,0xC20AF4u,0xC20AF6u,0xC20AF8u,0xC20AFAu,0xC20AFCu,0xC20B02u,0xC20B06u,0xC20B0Au,0xC20B0Eu,0xC20B14u,0xC20B1Au,0xC20B1Eu,0xC20B20u,0xC20B22u,0xC20B24u,0xC20B26u,0xC20B28u,0xC20B2Au,0xC20B2Cu,0xC20B2Eu,0xC20B30u,0xC20B34u,0xC20B38u,0xC20B3Cu,0xC20B40u,0xC20B44u,0xC20B48u,0xC20B4Eu,0xC20B50u,0xC20B52u,0xC20B54u,0xC20B56u,0xC20B58u,0xC20B5Au,0xC20B5Cu,0xC20B5Eu,0xC20B60u,0xC20B64u,0xC20B68u,0xC20B6Cu,0xC20B70u,0xC20B74u,0xC20B7Au,0xC20B7Eu,0xC20B82u,0xC20B86u,0xC20B8Au,0xC20B8Cu,0xC20B8Eu,0xC20B90u,0xC20B92u,0xC20B98u,0xC20B9Cu,0xC20BA0u,0xC20BA4u,0xC20BAAu,0xC20BB0u,0xC20BB6u,0xC20BB8u,0xC20BBAu,0xC20BBCu,0xC20BBEu,0xC20BC0u,0xC20BC2u,0xC20BC4u,0xC20BC6u,0xC20BC8u,0xC20BCCu,0xC20BD0u,0xC20BD4u,0xC20BD8u,0xC20BDCu,0xC20BE2u,0xC20BE8u,0xC20BEAu,0xC20BECu,0xC20BEEu,0xC20BF0u,0xC20BF2u,0xC20BF4u,0xC20BF6u,0xC20BF8u,0xC20BFAu,0xC20BFEu,0xC20C02u,0xC20C06u,0xC20C0Au,0xC20C10u,0xC20C14u,0xC20C18u,0xC20C1Cu,0xC20C20u,0xC20C22u,0xC20C26u,0xC20C30u,0xC20C36u,0xC20C38u,0xC20C3Cu,0xC20C44u,0xC20C48u,0xC20C4Cu,0xC20C50u,0xC20C52u,0xC20C5Au,0xC20C60u,0xC20C62u,0xC20C66u,0xC20C6Au,0xC20C6Eu,0xC20C72u,0xC20C76u,0xC20C7Au,0xC20C7Eu,0xC20C82u,0xC20C88u,0xC20C8Cu,0xC20C8Eu,0xC20C92u,0xC20C96u,0xC20C9Cu,0xC20CA0u,0xC20CA6u,0xC20CACu,0xC20CAEu,0xC20CB0u,0xC20CB4u,0xC20CB6u,0xC20CB8u,0xC20CBAu,0xC20CBCu,0xC20CC2u,0xC20CC6u,0xC20CCAu,0xC20CCEu,0xC20CD4u,0xC20CDAu,0xC20CDEu,0xC20CE0u,0xC20CE2u,0xC20CE4u,0xC20CE6u,0xC20CE8u,0xC20CEAu,0xC20CECu,0xC20CEEu,0xC20CF0u,0xC20CF4u,0xC20CF8u,0xC20CFCu,0xC20D00u,0xC20D04u,0xC20D08u,0xC20D0Cu,0xC20D10u,0xC20D14u,0xC20D1Au,0xC20D1Cu,0xC20D1Eu,0xC20D20u,0xC20D22u,0xC20D24u,0xC20D26u,0xC20D28u,0xC20D2Au,0xC20D2Cu,0xC20D30u,0xC20D34u,0xC20D38u,0xC20D3Cu,0xC20D40u,0xC20D46u,0xC20D4Au,0xC20D4Eu,0xC20D52u,0xC20D56u,0xC20D5Au,0xC20D5Eu,0xC20D62u,0xC20D66u,0xC21060u,0xC21064u,0xC2106Cu,0xC21070u,0xC21074u,0xC21076u,0xC21078u,0xC21080u,0xC21084u,0xC2108Eu,0xC21090u,0xC21092u,0xC21096u,0xC2109Cu,0xC210A2u,0xC210A6u,0xC210A8u,0xC210AAu,0xC210ACu,0xC210B0u,0xC210B2u,0xC210B4u,0xC210B6u,0xC210BAu,0xC210BCu,0xC210BEu,0xC210C0u,0xC210C4u,0xC210C6u,0xC210C8u,0xC210CAu,0xC210CCu,0xC210CEu,0xC210D4u,0xC210D8u,0xC210DAu,0xC210DCu,0xC210E0u,0xC210E4u,0xC219AEu,0xC219B4u,0xC219B6u,0xC219B8u,0xC219BEu,0xC219C4u,0xC219C6u,0xC219C8u,0xC219CAu,0xC219CCu,0xC219D0u,0xC219D2u,0xC219D4u,0xC219D6u,0xC219DCu,0xC219DEu,0xC219E0u,0xC219E2u,0xC219E8u,0xC219EAu,0xC219ECu,0xC219EEu,0xC219F0u,0xC219F2u,0xC219F4u,0xC219FAu,0xC21A00u,0xC21A06u,0xC21A0Au,0xC21A0Eu,0xC21A10u,0xC21A12u,0xC21A14u,0xC21A16u,0xC21A18u,0xC21A1Au,0xC21A1Cu,0xC21A1Eu,0xC21C2Eu,0xC21C34u,0xC21C3Au,0xC21C3Eu,0xC21C40u,0xC21C44u,0xC21C4Au,0xC21C4Cu,0xC21C52u,0xC21C54u,0xC21C56u,0xC21C58u,0xC21C5Au,0xC21C5Cu,0xC21C5Eu,0xC21C60u,0xC21C62u,0xC21C64u,0xC21C6Au,0xC21C6Cu,0xC21C6Eu,0xC21C70u,0xC21C72u,0xC21C74u,0xC21C76u,0xC21C78u,0xC21C7Au,0xC21C7Cu,0xC21C82u,0xC21C84u};
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
                face_list_parents_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
#include "face_list_parents_fixture.h"
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *reference=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    void *cpu=malloc(m68k_context_size()); char error[256];
    unsigned hardware_writes=0;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"face-list/edge parent oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_hud_hardware_begin(); fa18_write_log_active=2;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        face_list_parents_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"face-list/edge parent oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_hud_hardware_begin();
        face_list_parents_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc1ff0au: glue_C1FF0A(); break;
        case 0xc2005cu: glue_C2005C(); break;
        case 0xc20100u: glue_C20100(); break;
        case 0xc21060u: glue_C21060(); break;
        case 0xc20c38u: glue_C20C38(); break;
        case 0xc20c22u: glue_C20C22(); break;
        case 0xc20a52u: glue_C20A52(); break;
        case 0xc20a40u: glue_C20A40(); break;
        case 0xc20002u: glue_C20002(); break;
        case 0xc2084au: glue_C2084A(); break;
        case 0xc2082au: glue_C2082A(); break;
        case 0xc219aeu: glue_C219AE(); break;
        case 0xc21c4cu: glue_C21C4C(); break;
        case 0xc21c2eu: glue_C21C2E(); break;
        default: return 1;
        }
        face_list_parents_contract_finish();
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"face-list/edge parent oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"face-list/edge parent oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"face-list/edge parent oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("face-list/edge parent oracle %06X: %u %s matched all registers, PC, full SR and all RAM; %u source boundaries observed\n",selected_entry,cases,"complete calls",count);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
