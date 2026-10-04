/* Complete HUD readout parent proof, including complete child-entry CPU/RAM contracts. */
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
static uint32_t selected_entry=0xc31f4cu;
extern void hud_readout_parents_contract_reset(unsigned scenario,int source);
extern void hud_readout_parents_contract_enter(uint32_t entry,uint32_t ret);
extern void hud_readout_parents_contract_finish(void);
static const uint32_t source_boundaries[]={0xC31C5Eu,0xC31C60u,0xC31C68u,0xC31C6Eu,0xC31C74u,0xC31C7Au,0xC31C7Cu,0xC31C80u,0xC31C84u,0xC31C86u,0xC31C8Au,0xC31C8Cu,0xC31C90u,0xC31C94u,0xC31C96u,0xC31C9Au,0xC31CA0u,0xC31CA6u,0xC31CA8u,0xC31CACu,0xC31CAEu,0xC31CB0u,0xC31CB2u,0xC31CB6u,0xC31CBCu,0xC31CC2u,0xC31CC6u,0xC31CCAu,0xC31CCCu,0xC31CCEu,0xC31CD0u,0xC31CD4u,0xC31CD8u,0xC31CDEu,0xC31CE4u,0xC31CE8u,0xC31CEAu,0xC31CECu,0xC31CF2u,0xC31CF8u,0xC31CFEu,0xC31D02u,0xC31D04u,0xC31D08u,0xC31D0Cu,0xC31D0Eu,0xC31D10u,0xC31D14u,0xC31D16u,0xC31D1Eu,0xC31D24u,0xC31D26u,0xC31D28u,0xC31D2Au,0xC31D2Eu,0xC31D30u,0xC31D34u,0xC31D38u,0xC31D3Au,0xC31D3Cu,0xC31D3Eu,0xC31D44u,0xC31D4Au,0xC31D4Eu,0xC31D50u,0xC31D54u,0xC31D58u,0xC31D5Au,0xC31D5Cu,0xC31D5Eu,0xC31D62u,0xC31D64u,0xC31D6Au,0xC31D70u,0xC31D78u,0xC31D82u,0xC31D88u,0xC31D8Au,0xC31D8Eu,0xC31D92u,0xC31D98u,0xC31D9Cu,0xC31DA0u,0xC31DA4u,0xC31DA6u,0xC31DACu,0xC31DB0u,0xC31DB4u,0xC31DB8u,0xC31DBCu,0xC31DBEu,0xC31DC2u,0xC31DC8u,0xC31DCCu,0xC31DD0u,0xC31DD6u,0xC31DDAu,0xC31DDEu,0xC31DE2u,0xC31DE8u,0xC31DEEu,0xC31DF4u,0xC31DF8u,0xC31DFAu,0xC31DFCu,0xC31DFEu,0xC31E04u,0xC31E06u,0xC31E08u,0xC31E0Cu,0xC31E0Eu,0xC31E14u,0xC31E16u,0xC31E1Cu,0xC31E20u,0xC31E22u,0xC31E26u,0xC31E28u,0xC31E2Cu,0xC31E2Eu,0xC31E30u,0xC31E34u,0xC31E36u,0xC31E3Au,0xC31E3Eu,0xC31E40u,0xC31E42u,0xC31E44u,0xC31E4Au,0xC31E50u,0xC31E52u,0xC31E54u,0xC31E56u,0xC31E58u,0xC31E5Cu,0xC31E6Cu,0xC31E72u,0xC31E78u,0xC31E7Cu,0xC31E80u,0xC31E84u,0xC31E86u,0xC31E8Au,0xC31E8Cu,0xC31E90u,0xC31E94u,0xC31E98u,0xC31E9Cu,0xC31EA0u,0xC31EA2u,0xC31EA6u,0xC31EAEu,0xC31EB2u,0xC31EB4u,0xC31EB6u,0xC31EBCu,0xC31EC0u,0xC31EC6u,0xC31EC8u,0xC31ED0u,0xC31ED2u,0xC31ED8u,0xC31EDEu,0xC31EE2u,0xC31EE4u,0xC31EE6u,0xC31EEAu,0xC31EF0u,0xC31EF2u,0xC31EF8u,0xC31EFEu,0xC31F02u,0xC31F08u,0xC31F0Cu,0xC31F10u,0xC31F16u,0xC31F1Cu,0xC31F1Eu,0xC31F20u,0xC31F24u,0xC31F28u,0xC31F2Cu,0xC31F30u,0xC31F34u,0xC31F38u,0xC31F3Cu,0xC31F40u,0xC31F44u,0xC31F48u,0xC31F4Cu,0xC31F52u,0xC31F58u,0xC31F5Au,0xC31F60u,0xC31F62u,0xC31F66u,0xC31F68u,0xC31F6Au,0xC31F70u,0xC31F72u,0xC31F78u,0xC31F7Au,0xC31F82u,0xC31F84u,0xC31F86u,0xC31F8Cu,0xC31F90u,0xC31F92u,0xC31F94u,0xC31F96u,0xC31F9Au,0xC31F9Cu,0xC31FA2u,0xC31FA8u,0xC31FAAu,0xC31FB0u,0xC31FB4u,0xC31FBAu,0xC31FBCu,0xC31FC0u,0xC31FC4u,0xC31FC8u,0xC31FCAu,0xC31FCEu,0xC31FD2u,0xC31FD6u,0xC31FDAu,0xC31FE0u,0xC31FE2u,0xC31FE6u,0xC31FEAu,0xC31FEEu,0xC31FF0u,0xC31FF6u,0xC31FFCu,0xC32002u,0xC32006u,0xC3200Au,0xC3200Eu,0xC32012u,0xC32016u,0xC3201Au,0xC32020u,0xC32022u,0xC32028u,0xC3202Eu,0xC32030u,0xC32036u,0xC32038u,0xC3203Eu,0xC32044u,0xC32048u,0xC3204Au,0xC3204Cu,0xC3204Eu,0xC32050u,0xC32052u,0xC32054u,0xC3205Au,0xC3205Cu,0xC32062u,0xC32064u,0xC3206Cu,0xC3206Eu,0xC32070u,0xC32076u,0xC32078u,0xC3207Eu,0xC32080u,0xC32082u,0xC32084u,0xC3208Cu,0xC3208Eu,0xC32090u,0xC32092u,0xC3209Cu,0xC320A2u,0xC320A4u,0xC320A6u,0xC320ACu,0xC320B6u,0xC320BCu,0xC320C2u,0xC320C4u,0xC320C8u,0xC320CEu,0xC320D2u,0xC320D8u,0xC320DAu,0xC320DEu,0xC320E2u,0xC320E4u,0xC320E8u,0xC320ECu,0xC320F0u,0xC320F6u,0xC320F8u,0xC320FCu,0xC32100u,0xC32104u,0xC32106u,0xC3210Cu,0xC32112u,0xC32116u,0xC3211Au,0xC3211Eu,0xC32122u,0xC32126u,0xC3212Au,0xC32130u,0xC32136u,0xC3213Au,0xC3213Cu,0xC32142u,0xC32146u,0xC32148u,0xC3214Au,0xC3214Cu,0xC32152u,0xC32158u,0xC3215Au,0xC3215Eu,0xC32164u,0xC32168u,0xC3216Cu,0xC32170u,0xC32174u,0xC32178u,0xC3217Eu,0xC32184u,0xC32188u,0xC3218Au,0xC3218Cu,0xC3218Eu,0xC32190u,0xC32192u,0xC32196u,0xC3219Cu,0xC321A0u,0xC321A2u,0xC321A4u,0xC321A6u,0xC321ACu,0xC321B2u,0xC321B4u,0xC321B8u,0xC321BEu,0xC321C2u,0xC321C6u,0xC321CAu,0xC321CEu,0xC321D2u,0xC321D8u,0xC321DEu,0xC321E2u,0xC321E8u,0xC321EAu,0xC321EEu,0xC321F2u,0xC321F8u,0xC321FAu,0xC32200u,0xC32202u,0xC32208u,0xC3220Au,0xC32210u,0xC32218u,0xC3221Eu,0xC32220u,0xC32222u,0xC32228u,0xC3222Eu,0xC32230u,0xC32234u,0xC3223Au,0xC3223Eu,0xC32242u,0xC32246u,0xC3224Au,0xC3224Eu,0xC32252u,0xC32256u,0xC3225Au,0xC3225Eu,0xC32260u,0xC32266u,0xC3226Cu,0xC32270u,0xC32276u,0xC32278u,0xC3227Cu,0xC32280u,0xC32286u,0xC32288u,0xC3228Eu,0xC32290u,0xC32296u,0xC32298u,0xC3229Eu,0xC322A6u,0xC322ACu,0xC322AEu,0xC322B0u,0xC322B6u,0xC322BCu,0xC322BEu,0xC322C2u,0xC322C8u,0xC322CCu,0xC322D0u,0xC322D4u,0xC322D8u,0xC322DCu,0xC322E0u,0xC322E4u,0xC322E8u,0xC322ECu,0xC3271Au,0xC3271Cu,0xC32720u,0xC32722u,0xC32724u,0xC32736u,0xC3273Au,0xC3273Cu,0xC32740u,0xC32742u,0xC32748u,0xC3274Eu,0xC32750u,0xC32752u,0xC32758u,0xC3275Au,0xC3275Eu,0xC32762u,0xC32766u,0xC32768u,0xC3276Au,0xC3276Cu,0xC3276Eu,0xC32772u,0xC32774u,0xC32776u,0xC32778u,0xC3277Au,0xC3277Eu,0xC32780u,0xC32786u,0xC3278Au,0xC32794u,0xC32796u,0xC3279Au,0xC327A0u,0xC327A4u,0xC327A6u,0xC327A8u,0xC327AAu,0xC327ACu,0xC327AEu,0xC327B0u,0xC327B2u,0xC327B4u,0xC327B8u,0xC327BAu,0xC327BCu,0xC327BEu,0xC327C0u,0xC327C2u,0xC327C6u,0xC327CAu,0xC327CCu,0xC327D2u,0xC327D6u,0xC327D8u,0xC327DAu,0xC327DCu,0xC327DEu,0xC327E0u,0xC327E4u,0xC327E6u,0xC327E8u,0xC327EAu,0xC327EEu,0xC327F0u,0xC327F4u,0xC327F6u,0xC327FEu,0xC32804u,0xC328A6u,0xC328A8u,0xC328AEu,0xC328B0u,0xC328B6u,0xC328BCu,0xC328C2u,0xC328C4u,0xC328C8u,0xC328CEu,0xC328D2u,0xC328D6u,0xC328DAu,0xC328DEu,0xC328E0u,0xC328E4u,0xC328E6u,0xC328EAu,0xC328F0u,0xC328F6u,0xC328F8u,0xC328FCu,0xC328FEu,0xC32900u,0xC32904u,0xC32906u,0xC3290Au,0xC32910u,0xC32916u,0xC3291Au,0xC3291Eu,0xC32920u,0xC32922u,0xC32926u,0xC32928u,0xC3292Cu,0xC32930u,0xC32936u,0xC3293Cu,0xC32940u,0xC32942u,0xC32944u,0xC32948u,0xC3294Eu,0xC32954u,0xC3295Au,0xC3295Cu,0xC32960u,0xC32964u,0xC32968u,0xC3296Cu,0xC3296Eu,0xC32970u,0xC32972u,0xC32978u,0xC3297Eu,0xC32982u,0xC32984u,0xC3298Au,0xC3298Cu,0xC3298Eu,0xC32992u,0xC32998u,0xC3299Eu,0xC329A0u,0xC329A4u,0xC329AAu,0xC329B0u,0xC329B4u,0xC329B6u,0xC329BAu,0xC329BEu,0xC329C2u,0xC329C6u,0xC329C8u,0xC329CAu,0xC329D0u,0xC329D6u,0xC329DAu,0xC329DEu,0xC329E4u,0xC329EAu,0xC329EEu,0xC329F0u,0xC329F4u,0xC329F8u,0xC329FCu,0xC32A00u,0xC32A02u,0xC32A08u,0xC32A0Eu,0xC32A10u,0xC32A14u,0xC32A1Au,0xC32A1Eu,0xC32A20u,0xC32A24u,0xC32A28u,0xC32A2Cu,0xC32A2Eu,0xC32A32u,0xC32A34u,0xC32A3Au,0xC32A40u,0xC33F54u,0xC33F56u,0xC33F5Au,0xC33F60u,0xC33F62u,0xC33F64u,0xC33F6Au,0xC33F6Eu,0xC33FA2u,0xC33FA4u,0xC33FA6u,0xC33FA8u,0xC33FAAu,0xC33FB0u};
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
                hud_readout_parents_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
#include "hud_readout_parents_fixture.h"
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
        fprintf(stderr,"HUD readout parent oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_hud_hardware_begin(); fa18_write_log_active=2;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        hud_readout_parents_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"HUD readout parent oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_hud_hardware_begin();
        hud_readout_parents_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc31f4cu: glue_C31F4C(); break;
        case 0xc3201au: glue_C3201A(); break;
        case 0xc3212au: glue_C3212A(); break;
        case 0xc32178u: glue_C32178(); break;
        case 0xc321d2u: glue_C321D2(); break;
        case 0xc32260u: glue_C32260(); break;
        case 0xc31eb6u: glue_C31EB6(); break;
        case 0xc31c60u: glue_C31C60(); break;
        case 0xc31d16u: glue_C31D16(); break;
        case 0xc31e6cu: glue_C31E6C(); break;
        case 0xc31d64u: glue_C31D64(); break;
        case 0xc33f54u: glue_C33F54(); break;
        case 0xc328a8u: glue_C328A8(); break;
        default: return 1;
        }
        hud_readout_parents_contract_finish();
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"HUD readout parent oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"HUD readout parent oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"HUD readout parent oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("HUD readout parent oracle %06X: %u %s matched all registers, PC, full SR and all RAM; %u source boundaries observed\n",selected_entry,cases,"complete calls",count);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
