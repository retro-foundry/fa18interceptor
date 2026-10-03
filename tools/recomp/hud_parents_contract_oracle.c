/* Complete HUD parent proof, including complete child-entry CPU/RAM contracts. */
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
static uint32_t selected_entry=0xc30764u;
extern void hud_parents_contract_reset(unsigned scenario,int source);
extern void hud_parents_contract_enter(uint32_t entry,uint32_t ret);
extern void hud_parents_contract_finish(void);
static const uint32_t source_boundaries[]={0xC30762u,0xC30764u,0xC3076Au,0xC3076Cu,0xC30772u,0xC30778u,0xC3077Eu,0xC30782u,0xC30786u,0xC3078Cu,0xC30790u,0xC30794u,0xC3079Au,0xC3079Cu,0xC307A2u,0xC307A4u,0xC307A8u,0xC307ACu,0xC307B0u,0xC307B6u,0xC307BAu,0xC307BEu,0xC307C0u,0xC307C2u,0xC307C4u,0xC307C6u,0xC307C8u,0xC307CAu,0xC307CCu,0xC307CEu,0xC307D0u,0xC307D2u,0xC307D4u,0xC307DAu,0xC307E0u,0xC307E6u,0xC307ECu,0xC307F0u,0xC307F2u,0xC307F4u,0xC307F8u,0xC307FCu,0xC30800u,0xC30804u,0xC30808u,0xC3080Eu,0xC30812u,0xC30818u,0xC3081Cu,0xC3081Eu,0xC30820u,0xC30822u,0xC30824u,0xC30826u,0xC30828u,0xC3082Au,0xC30830u,0xC30836u,0xC30838u,0xC3083Cu,0xC3083Eu,0xC30842u,0xC30844u,0xC30846u,0xC30848u,0xC3084Au,0xC3084Eu,0xC30850u,0xC30852u,0xC30854u,0xC30858u,0xC3085Au,0xC3085Eu,0xC30860u,0xC30864u,0xC30866u,0xC30868u,0xC3086Au,0xC3086Eu,0xC30874u,0xC30876u,0xC3087Cu,0xC3087Eu,0xC30880u,0xC30886u,0xC30888u,0xC3088Au,0xC3088Cu,0xC3088Eu,0xC30890u,0xC30892u,0xC30896u,0xC30898u,0xC3089Au,0xC308A0u,0xC308A6u,0xC308AAu,0xC308AEu,0xC308B2u,0xC308B6u,0xC308BAu,0xC308BEu,0xC308C2u,0xC308C6u,0xC308CAu,0xC308CEu,0xC308D2u,0xC308D6u,0xC309B6u,0xC309BAu,0xC309BEu,0xC309C2u,0xC309C4u,0xC309C6u,0xC309CCu,0xC309D0u,0xC309D4u,0xC309D8u,0xC309DCu,0xC309E0u,0xC30B5Cu,0xC30B62u,0xC30B64u,0xC30B6Au,0xC30B6Eu,0xC30B74u,0xC30B76u,0xC30B7Au,0xC30B7Cu,0xC30B80u,0xC30B86u,0xC30B88u,0xC30B8Cu,0xC30B8Eu,0xC30B98u,0xC30B9Eu,0xC30BA4u,0xC30BAAu,0xC30BACu,0xC30BB4u,0xC30BB6u,0xC30BBEu,0xC30BC4u,0xC30BCAu,0xC30BCCu,0xC30BD2u,0xC30BD8u,0xC30BDCu,0xC30BE0u,0xC30BE4u,0xC30BE8u,0xC30BEEu,0xC30BF2u,0xC30BF4u,0xC30BF8u,0xC30BFEu,0xC30C04u,0xC30C08u,0xC30C0Cu,0xC30C0Eu,0xC30C12u,0xC30C14u,0xC30C18u,0xC30C1Cu,0xC30C20u,0xC30C26u,0xC30C28u,0xC30C2Eu,0xC30C34u,0xC30C38u,0xC30C3Cu,0xC30C40u,0xC30C44u,0xC30C4Au,0xC30C4Eu,0xC30C50u,0xC30C54u,0xC30C5Au,0xC30C5Cu,0xC30C60u,0xC30C62u,0xC30C66u,0xC30C6Au,0xC30C6Eu,0xC30C70u,0xC30C76u,0xC30C78u,0xC30C7Eu,0xC30C84u,0xC30C88u,0xC30C8Cu,0xC30C90u,0xC30C94u,0xC30C9Au,0xC30C9Eu,0xC30CA0u,0xC30CA4u,0xC30CAAu,0xC30CACu,0xC30CB4u,0xC30CB6u,0xC30CBAu,0xC30CBCu,0xC30CC0u,0xC30CC4u,0xC30CCAu,0xC30CCEu,0xC30CD0u,0xC30CD2u,0xC30CD6u,0xC30CD8u,0xC30CDAu,0xC30CDEu,0xC30CE0u,0xC30CE2u,0xC30CE4u,0xC30CE6u,0xC30CE8u,0xC30CEEu,0xC30CF4u,0xC30CF8u,0xC30CFEu,0xC30D04u,0xC30D08u,0xC30D0Cu,0xC30D10u,0xC30D14u,0xC30D18u,0xC30D1Cu,0xC30D20u,0xC30D32u,0xC30D34u,0xC30D3Au,0xC30D3Eu,0xC30D42u,0xC30D46u,0xC30D4Au,0xC30D50u,0xC30D54u,0xC30D56u,0xC30D5Au,0xC30D60u,0xC30D62u,0xC30D6Au,0xC30D6Cu,0xC30D70u,0xC30D72u,0xC30D76u,0xC30D7Au,0xC30D7Eu,0xC30D84u,0xC30D86u,0xC30D8Cu,0xC30D92u,0xC30D98u,0xC30D9Cu,0xC30DA0u,0xC30DA4u,0xC30DA6u,0xC30DA8u,0xC30DAAu,0xC30DACu,0xC30DAEu,0xC30DB4u,0xC30DBAu,0xC30DBEu,0xC30DC2u,0xC30DC6u,0xC30DCAu,0xC30DD0u,0xC30DD6u,0xC30DDAu,0xC30DDEu,0xC30DE0u,0xC30DE2u,0xC30DE4u,0xC30DE6u,0xC30DE8u,0xC30DEAu,0xC30DECu,0xC30DEEu,0xC30DF0u,0xC30DF2u,0xC30DF4u,0xC30DF6u,0xC30DFAu,0xC30E00u,0xC30E06u,0xC30E0Au,0xC30E10u,0xC30E16u,0xC30E1Cu,0xC30E20u,0xC30E24u,0xC30E26u,0xC30E28u,0xC30E2Cu,0xC30E30u,0xC30E34u,0xC30E38u,0xC30E3Cu,0xC30E40u,0xC30E44u,0xC30E4Au,0xC30E50u,0xC30E56u,0xC30E58u,0xC30E5Cu,0xC30E60u,0xC30E64u,0xC30E68u,0xC30E6Eu,0xC30E70u,0xC30E74u,0xC30E76u,0xC30E7Cu,0xC30E7Eu,0xC30E82u,0xC30E84u,0xC30E8Au,0xC30E90u,0xC30E9Au,0xC30EA2u,0xC30EA8u,0xC30F76u,0xC30F78u,0xC30F7Cu,0xC30F82u,0xC30F88u,0xC30F8Au,0xC30F90u,0xC30F92u,0xC30F94u,0xC30F96u,0xC30F9Eu,0xC30FA0u,0xC30FA2u,0xC30FAAu,0xC30FAEu,0xC30FB0u,0xC30FB2u,0xC30FB8u,0xC30FC0u,0xC30FC6u,0xC30FCCu,0xC30FD2u,0xC30FD6u,0xC30FDAu,0xC30FDEu,0xC30FE2u,0xC30FE8u,0xC30FECu,0xC30FEEu,0xC30FF0u,0xC30FF4u,0xC30FF6u,0xC30FF8u,0xC30FFAu,0xC31000u,0xC31002u,0xC31004u,0xC31008u,0xC3100Au,0xC3100Cu,0xC3100Eu,0xC31010u,0xC31014u,0xC31016u,0xC3101Au,0xC3101Cu,0xC3101Eu,0xC31024u,0xC31026u,0xC31028u,0xC3102Au,0xC3102Eu,0xC31030u,0xC31034u,0xC3103Au,0xC3103Eu,0xC31042u,0xC31048u,0xC3104Cu,0xC3104Eu,0xC31052u,0xC31056u,0xC3105Au,0xC3105Cu,0xC31060u,0xC31064u,0xC31068u,0xC3106Cu,0xC31070u,0xC31074u,0xC3107Eu,0xC31082u,0xC31088u,0xC3108Au,0xC3108Eu,0xC31090u,0xC31094u,0xC31096u,0xC31098u,0xC3109Au,0xC310A2u,0xC310A8u,0xC31128u,0xC3112Au,0xC31134u,0xC3113Cu,0xC3113Eu,0xC31142u,0xC31144u,0xC31148u,0xC3114Eu,0xC31152u,0xC31156u,0xC3115Cu,0xC3115Eu,0xC31160u,0xC31166u,0xC3116Eu,0xC31170u,0xC31174u,0xC31176u,0xC3117Au,0xC31180u,0xC31182u,0xC31188u,0xC3118Au,0xC31190u,0xC31198u,0xC3119Au,0xC3119Eu,0xC311A0u,0xC311A4u,0xC311AAu,0xC311ACu,0xC311AEu,0xC311B4u,0xC311B6u,0xC311BCu,0xC311C4u,0xC311C6u,0xC311CAu,0xC311CCu,0xC311D0u,0xC311D6u,0xC311D8u,0xC311DEu,0xC311E0u,0xC311E6u,0xC311ECu,0xC311F2u,0xC311F6u,0xC311FCu,0xC311FEu,0xC31206u,0xC31208u,0xC3120Cu,0xC31212u,0xC31214u,0xC3121Au,0xC3121Cu,0xC31222u,0xC31A64u,0xC31A6Au,0xC31A6Cu,0xC31A72u,0xC31A78u,0xC31A7Eu,0xC31A82u,0xC31A86u,0xC31A8Au,0xC31A8Cu,0xC31A8Eu,0xC31A90u,0xC31A94u,0xC31A96u,0xC31A98u,0xC31A9Au,0xC31A9Cu,0xC31AA2u,0xC31AA8u,0xC31AACu,0xC31AB2u,0xC31AB6u,0xC31ABAu,0xC31ABEu,0xC31AC2u,0xC31AC4u,0xC31AC8u,0xC31ACAu,0xC31ACCu,0xC31AD2u,0xC31AD4u,0xC31ADAu,0xC31AE0u,0xC31AE4u,0xC31AE6u,0xC31AE8u,0xC31AEAu,0xC31AEEu,0xC31AF0u,0xC31AF2u,0xC31AF4u,0xC31AF6u,0xC31AFEu,0xC31B06u,0xC31B08u,0xC31B0Cu,0xC31B10u,0xC31B18u,0xC31B1Au,0xC31B1Eu,0xC31B22u,0xC31B2Au,0xC31B2Eu,0xC31B32u,0xC31B36u,0xC31B40u,0xC31B46u,0xC31B4Au,0xC31B50u,0xC31B56u,0xC31B5Au,0xC31B60u,0xC31B64u,0xC31B68u,0xC31B6Cu,0xC31B6Eu,0xC3273Cu,0xC32740u,0xC32742u,0xC32748u,0xC3274Eu,0xC32750u,0xC32752u,0xC32758u,0xC3275Au,0xC3275Eu,0xC32762u,0xC32766u,0xC32768u,0xC3276Au,0xC3276Cu,0xC3276Eu,0xC32772u,0xC32774u,0xC32776u,0xC32778u,0xC3277Au,0xC3277Eu,0xC32780u,0xC32786u,0xC3278Au,0xC32794u,0xC32796u,0xC3279Au,0xC327A0u,0xC327A4u,0xC327A6u,0xC327A8u,0xC327AAu,0xC327ACu,0xC327AEu,0xC327B0u,0xC327B2u,0xC327B4u,0xC327B8u,0xC327BAu,0xC327BCu,0xC327BEu,0xC327C0u,0xC327C2u,0xC327C6u,0xC327CAu,0xC327CCu,0xC327D2u,0xC327D6u,0xC327D8u,0xC327DAu,0xC327DCu,0xC327DEu,0xC327E0u,0xC327E4u,0xC327E6u,0xC327E8u,0xC327EAu,0xC327EEu,0xC327F0u,0xC327F4u,0xC327F6u,0xC327FEu,0xC32804u};
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
                hud_parents_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
#include "hud_parents_fixture.h"
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
        fprintf(stderr,"HUD parent oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_hud_hardware_begin(); fa18_write_log_active=2;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        hud_parents_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"HUD parent oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_hud_hardware_begin();
        hud_parents_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        switch(selected_entry) {
        case 0xc30764u: glue_C30764(); break;
        case 0xc309b6u: glue_C309B6(); break;
        case 0xc30b5cu: glue_C30B5C(); break;
        case 0xc30d34u: glue_C30D34(); break;
        case 0xc30f78u: glue_C30F78(); break;
        case 0xc3112au: glue_C3112A(); break;
        case 0xc31a64u: glue_C31A64(); break;
        case 0xc31accu: glue_C31ACC(); break;
        default: return 1;
        }
        hud_parents_contract_finish();
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"HUD parent oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"HUD parent oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"HUD parent oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("HUD parent oracle %06X: %u %s matched all registers, PC, full SR and all RAM; %u source boundaries observed\n",selected_entry,cases,"complete calls",count);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
