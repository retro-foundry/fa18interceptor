/* Complete input-display-setup proof, including complete child-entry CPU/RAM contracts. */
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
static uint32_t selected_entry=0xc1612cu;
extern void input_display_setup_contract_reset(unsigned scenario,int source);
extern void input_display_setup_contract_enter(uint32_t entry,uint32_t ret);
extern void input_display_setup_contract_finish(void);
static const uint32_t source_boundaries[]={0xC1612Cu,0xC16130u,0xC16136u,0xC1613Cu,0xC1613Eu,0xC16144u,0xC16148u,0xC1614Au,0xC1614Cu,0xC1614Eu,0xC16154u,0xC1615Au,0xC1615Eu,0xC16160u,0xC16162u,0xC16164u,0xC1616Au,0xC16170u,0xC16176u,0xC1617Cu,0xC1617Eu,0xC16184u,0xC16188u,0xC1618Eu,0xC16194u,0xC16196u,0xC1619Cu,0xC161A2u,0xC161A4u,0xC161A8u,0xC161AAu,0xC161ACu,0xC161B2u,0xC161B8u,0xC161BEu,0xC161C2u,0xC161C8u,0xC161CEu,0xC161D0u,0xC161D6u,0xC161DCu,0xC161DEu,0xC161E0u,0xC161E2u,0xC161E8u,0xC161EEu,0xC161F4u,0xC161F8u,0xC161FEu,0xC16204u,0xC16206u,0xC1620Cu,0xC16212u,0xC16214u,0xC1621Au,0xC1621Cu,0xC16222u,0xC16226u,0xC1622Cu,0xC1622Eu,0xC16234u,0xC16236u,0xC1623Cu,0xC16242u,0xC16246u,0xC16248u,0xC1624Eu,0xC16254u,0xC16256u,0xC16258u,0xC1625Au,0xC16260u,0xC16266u,0xC1626Cu,0xC16270u,0xC16276u,0xC16278u,0xC1627Au,0xC16280u,0xC16282u,0xC16D4Cu,0xC16D50u,0xC16D5Au,0xC16D5Cu,0xC16D5Eu,0xC16D60u,0xC16D66u,0xC16D68u,0xC16D6Eu,0xC16D70u,0xC16D72u,0xC16D74u,0xC16D76u,0xC16D7Cu,0xC16D7Eu,0xC16D84u,0xC16D8Au,0xC16D8Cu,0xC16D92u,0xC16D94u,0xC16D96u,0xC16D9Cu,0xC16DA2u,0xC16DA4u,0xC16DA6u,0xC16DA8u,0xC16DAEu,0xC16DB0u,0xC16DB2u,0xC16DB8u,0xC16DBAu,0xC16DBCu,0xC16DC2u,0xC16DC8u,0xC16DCCu,0xC16DD0u,0xC16DD2u,0xC16DD4u,0xC16DDAu,0xC16DE0u,0xC16DE2u,0xC16DE8u,0xC16DEEu,0xC16DF0u,0xC16DF2u,0xC16DF4u,0xC16DFAu,0xC16DFCu,0xC16E06u,0xC16E08u,0xC16E0Au,0xC16E0Eu,0xC16E10u,0xC16E12u,0xC16E14u,0xC16E1Au,0xC16E20u,0xC16E22u,0xC16E28u,0xC16E2Eu,0xC16E30u,0xC16E32u,0xC16E34u,0xC16E3Au,0xC16E3Cu,0xC16E40u,0xC16E42u,0xC16E44u,0xC16E4Au,0xC16E50u,0xC16E52u,0xC16E58u,0xC16E5Eu,0xC16E60u,0xC16E62u,0xC16E64u,0xC16E6Au,0xC16E6Cu,0xC16E72u,0xC16E78u,0xC16E7Eu,0xC16E86u,0xC16E88u,0xC16E8Eu,0xC16E92u,0xC16E96u,0xC16E9Cu,0xC16EA0u,0xC16EA2u,0xC16EA8u,0xC16EAAu,0xC16EACu,0xC16FF4u,0xC16FF8u,0xC16FFEu,0xC17004u,0xC17006u,0xC1700Cu,0xC17010u,0xC17016u,0xC1701Eu,0xC17022u,0xC17028u,0xC1702Au,0xC17030u,0xC17036u,0xC17038u,0xC1703Eu,0xC17044u,0xC17046u,0xC1704Cu,0xC17052u,0xC17054u,0xC1705Au,0xC1705Eu,0xC17060u,0xC17062u,0xC17064u,0xC17066u,0xC1706Au,0xC17070u,0xC17076u,0xC17078u,0xC1707Eu,0xC17082u,0xC17086u,0xC1708Cu,0xC17090u,0xC17096u,0xC1709Au,0xC1709Cu,0xC170A0u,0xC170A4u,0xC170A6u,0xC170ACu,0xC170AEu,0xC170B0u,0xC1787Au,0xC1787Eu,0xC17882u,0xC17884u,0xC17886u,0xC1788Cu,0xC17892u,0xC17894u,0xC17896u,0xC17898u,0xC1789Eu,0xC178A0u,0xC178A2u,0xC178A4u,0xC178AAu,0xC178B0u,0xC178B2u,0xC178B4u,0xC178B6u,0xC178BCu,0xC178C0u,0xC178C2u,0xC178C4u,0xC178C6u,0xC178C8u,0xC178CAu,0xC178D0u,0xC178D2u,0xC178D4u,0xC178D6u,0xC178D8u,0xC178DEu,0xC178E2u,0xC178E8u,0xC178ECu,0xC178F2u,0xC178F6u,0xC178FCu,0xC17902u,0xC17908u,0xC1790Cu,0xC17914u,0xC17916u,0xC17918u,0xC1791Au,0xC1791Cu,0xC17922u,0xC17924u,0xC17926u,0xC17928u,0xC1792Eu,0xC17932u,0xC17934u,0xC1793Au,0xC1793Cu,0xC17942u,0xC1794Au,0xC1794Cu,0xC17950u,0xC17952u,0xC17954u,0xC1795Au,0xC17960u,0xC17962u,0xC17964u,0xC17966u,0xC1796Cu,0xC17970u,0xC17972u,0xC17978u,0xC1797Au,0xC17980u,0xC17982u,0xC17986u,0xC1798Eu,0xC17996u,0xC17998u,0xC1799Au,0xC179A0u,0xC179A6u,0xC179A8u,0xC179AAu,0xC179ACu,0xC179B4u,0xC179BAu,0xC179BCu,0xC179BEu,0xC179C0u,0xC179C2u,0xC179C4u,0xC179CAu,0xC179CCu,0xC179CEu,0xC179D0u,0xC179D6u,0xC179DEu,0xC179E6u,0xC179EEu,0xC179F0u,0xC179F2u,0xC179F8u,0xC179FEu,0xC17A00u,0xC17A02u,0xC17A04u,0xC17A06u,0xC17A0Cu,0xC17A10u,0xC17A18u,0xC17A1Eu,0xC17A20u,0xC17A22u,0xC17A24u,0xC17A26u,0xC17A28u,0xC17A2Eu,0xC17A30u,0xC17A32u,0xC17A34u,0xC17A3Au,0xC17A42u,0xC17A4Au,0xC17A4Cu,0xC17A52u,0xC17A58u,0xC17A5Au,0xC17A5Cu,0xC17A5Eu,0xC17A60u,0xC17A62u,0xC17A64u,0xC17A6Au,0xC17A6Cu,0xC17A6Eu,0xC17A70u,0xC17A72u,0xC17A78u,0xC17A7Cu,0xC17A82u,0xC17A86u,0xC17A8Eu,0xC17A96u,0xC17A98u,0xC17A9Au,0xC17AA0u,0xC17AA6u,0xC17AA8u,0xC17AAAu,0xC17AACu,0xC17AAEu,0xC17AB0u,0xC17AB2u,0xC17AB4u,0xC17ABAu,0xC17ABCu,0xC17ABEu,0xC17AC0u,0xC17AC6u,0xC17AC8u,0xC17ACCu,0xC17AD2u,0xC17AD6u,0xC17ADCu,0xC17AE2u,0xC17AE6u,0xC17AECu,0xC17AF2u,0xC17AF8u,0xC17AFCu,0xC17B04u,0xC17B06u};
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
                input_display_setup_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
    static const uint8_t activity[]={0,1,2,3,7,0x80,0xff,0x81};
    static const uint8_t errors[]={0,1,0x7f,0x80,0xff,0x81,2,0xfe};
    unsigned i,profile=scenario/32u;
    for(i=0;i<15;++i) REG_DA[i]=random_value(); REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    for(i=0;i<512;++i) wr_u32(0xc60400u+4*i,random_value());
    wr_u32(0xc1abceu,0xc60400u); wr_u32(0xc1abecu,0xc60500u); wr_u32(0xc1abe8u,0xc60600u);
    wr_u8(0xc6041fu,errors[profile%8u]);
    for(i=0;i<13;++i) wr_u32(0xc0a438u+4*i,0xc60800u+64*i);
    if(profile&32u) wr_u32(0xc0a450u,0); if(profile&64u) wr_u32(0xc0a440u,0);
    wr_u8(0xc45b5au,(uint8_t)random_value()); wr_u8(0xc45b5bu,(uint8_t)random_value());
    wr_u8(0xc45899u,activity[profile%8u]); wr_u8(TABLE_CLEAR_MODE,errors[(profile/8u)%8u]);
    wr_u16(0xc458d2u,(profile&16u)?0x0100u:0);
    wr_u16(DRAW_PAGE,(uint16_t)(profile&1u));
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; expected_sp=REG_A[7]+4; wr_u32(REG_A[7],0xc70000u);
    wr_u32(REG_A[7]+4,random_value());
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
        fprintf(stderr,"input-display-setup oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        input_display_setup_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"input-display-setup oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        input_display_setup_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc16d4cu: glue_C16D4C(); break;
        case 0xc16ff4u: glue_C16FF4(); break;
        case 0xc17066u: glue_C17066(); break;
        case 0xc1787au: glue_C1787A(); break;
        case 0xc1612cu: glue_C1612C(); break;
        default: return 1;
        }
        input_display_setup_contract_finish();
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"input-display-setup oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"input-display-setup oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"input-display-setup oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("input-display-setup oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
