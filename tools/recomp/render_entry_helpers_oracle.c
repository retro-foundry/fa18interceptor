/* Complete render entry helper proof, including cold internal paths and real children. */
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
extern void fa18_render_entry_helpers_fixture_begin(const char *phase);
extern unsigned fa18_hud_hardware_count(void);
extern void fa18_hud_hardware_begin(void);
extern void fa18_render_entry_controlled_begin(uint32_t entry,unsigned scenario);
extern void fa18_hud_hardware_reference(void);
extern int fa18_hud_hardware_check(void);
extern int64_t fa18_next_event;
extern void fa18_render_entry_source_child(uint32_t ret,uint32_t sp);
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
static uint32_t selected_entry=0xc2f688u;
static const uint32_t source_boundaries[]={0xC2F622u,0xC2F624u,0xC2F63Au,0xC2F640u,0xC2F642u,0xC2F646u,0xC2F648u,0xC2F64Eu,0xC2F650u,0xC2F652u,0xC2F658u,0xC2F65Eu,0xC2F664u,0xC2F668u,0xC2F66Au,0xC2F66Cu,0xC2F688u,0xC2F68Au,0xC2F68Cu,0xC2F692u,0xC2F696u,0xC2F698u,0xC2F69Au,0xC2F69Eu,0xC2F6A0u,0xC2F6A4u,0xC2F6A6u,0xC2F6AAu,0xC2F6ACu,0xC2F6AEu,0xC2F6B0u,0xC2F6B2u,0xC2F6B4u,0xC2F6B6u,0xC2F6B8u,0xC2F6BCu,0xC2F6BEu,0xC2F6C0u,0xC2F6C4u,0xC2F6C6u,0xC2F6C8u,0xC2F6CAu,0xC2F6CCu,0xC2F6CEu,0xC2F6D0u,0xC2F6D2u,0xC2F6D4u,0xC2F6D6u,0xC2F6D8u,0xC2F6E0u,0xC2F6E2u,0xC2F6E6u,0xC2F6E8u,0xC2F6F0u,0xC2F6F2u,0xC2F6F6u,0xC2F6F8u,0xC2F700u,0xC2F702u,0xC2F706u,0xC2F708u,0xC2F710u,0xC2F712u,0xC2F716u,0xC2F718u,0xC2F71Eu,0xC2F720u,0xC2F726u,0xC2F72Eu,0xC2F730u,0xC2F732u,0xC2F734u,0xC2F73Cu,0xC2F73Eu,0xC2F740u,0xC2F742u,0xC2F74Au,0xC2F74Cu,0xC2F74Eu,0xC2F750u,0xC2F758u,0xC2F75Au,0xC2F75Cu,0xC2F75Eu,0xC2F760u,0xC2F762u,0xC2F764u,0xC2F826u,0xC2F828u,0xC2F82Au,0xC2F82Cu,0xC2F82Eu,0xC2F83Au,0xC2F83Cu,0xC2F83Eu,0xC2F840u,0xC2F842u,0xC2F844u,0xC2F846u,0xC2F848u,0xC2F84Au,0xC2F84Cu,0xC2F84Eu,0xC2F850u,0xC2F852u,0xC2F854u,0xC2F856u,0xC2F858u,0xC2F85Au,0xC2F85Cu,0xC2F85Eu,0xC2F860u,0xC2F862u,0xC2F864u,0xC2F866u,0xC2F868u,0xC2F86Au,0xC2F86Cu,0xC2F86Eu,0xC2F870u,0xC2F872u,0xC2F874u,0xC2F876u,0xC2F878u,0xC2F87Au,0xC2F87Cu,0xC2F87Eu,0xC2F880u,0xC2F882u,0xC2F884u,0xC2F886u,0xC2F888u,0xC2F88Au,0xC2F88Cu,0xC2F88Eu,0xC2F890u,0xC2F892u,0xC2F894u,0xC2F896u,0xC2F898u,0xC2F89Au,0xC2F89Cu,0xC2F89Eu,0xC2F8A0u,0xC2F8A2u,0xC2F8A4u,0xC2F8A6u,0xC2F8A8u,0xC2F8AAu,0xC2F8ACu,0xC2F8AEu,0xC2F8B0u,0xC2F8B2u,0xC2F8B4u,0xC2F8B6u,0xC2F8B8u,0xC2F8BAu,0xC2F8BCu,0xC2F8BEu,0xC2F8C0u,0xC2F8C2u,0xC2F8C4u,0xC2F8C6u,0xC2F8C8u,0xC2F8CAu,0xC2F8CCu,0xC2F8CEu,0xC2F8D0u,0xC2F8D2u,0xC2F8D6u,0xC2F8D8u,0xC2F8DCu,0xC2F8DEu,0xC2F8E2u,0xC2F8E4u,0xC2F8E8u,0xC2F8EAu,0xC2F8ECu,0xC2F8F0u,0xC2F8F2u,0xC2F8F6u,0xC2F8F8u,0xC2F8FCu,0xC2F8FEu,0xC2F902u,0xC2F904u,0xC2F906u,0xC2F90Au,0xC2F90Cu,0xC2F910u,0xC2F912u,0xC2F916u,0xC2F918u,0xC2F91Cu,0xC2F91Eu,0xC2F920u,0xC2F924u,0xC2F926u,0xC2F92Au,0xC2F92Cu,0xC2F930u,0xC2F932u,0xC2F936u,0xC2F938u,0xC2F93Au,0xC2F93Eu,0xC2F940u,0xC2F944u,0xC2F946u,0xC2F94Au,0xC2F94Cu,0xC2F950u,0xC2F952u,0xC2F954u,0xC2F958u,0xC2F95Au,0xC2F95Eu,0xC2F960u,0xC2F964u,0xC2F966u,0xC2F96Au,0xC2F96Cu,0xC2F96Eu,0xC2F972u,0xC2F974u,0xC2F978u,0xC2F97Au,0xC2F97Eu,0xC2F980u,0xC2F984u,0xC2F986u,0xC2F988u,0xC2F98Cu,0xC2F98Eu,0xC2F992u,0xC2F994u,0xC2F998u,0xC2F99Au,0xC2F99Eu,0xC2F9A0u,0xC2F9A2u,0xC2F9A6u,0xC2F9A8u,0xC2F9ACu,0xC2F9AEu,0xC2F9B2u,0xC2F9B4u,0xC2F9B8u,0xC2F9BAu,0xC2F9BCu,0xC2F9C0u,0xC2F9C2u,0xC2F9C6u,0xC2F9C8u,0xC2F9CCu,0xC2F9CEu,0xC2F9D2u,0xC2F9D4u,0xC2F9D6u,0xC2F9DAu,0xC2F9DCu,0xC2F9E0u,0xC2F9E2u,0xC2F9E6u,0xC2F9E8u,0xC2F9ECu,0xC2F9EEu,0xC2F9F0u,0xC2F9F4u,0xC2F9F6u,0xC2F9FAu,0xC2F9FCu,0xC2FA00u,0xC2FA02u,0xC2FA06u,0xC2FA08u,0xC2FA0Au,0xC2FA0Eu,0xC2FA10u,0xC2FA14u,0xC2FA16u,0xC2FA1Au,0xC2FA1Cu,0xC2FA20u,0xC2FA22u,0xC2FA24u,0xC2FA28u,0xC2FA2Au,0xC2FA2Eu,0xC2FA30u,0xC2FA34u,0xC2FA36u,0xC2FA3Au,0xC2FA3Cu,0xC2FA3Eu,0xC2FA42u,0xC2FA44u,0xC2FA48u,0xC2FA4Au,0xC2FA4Eu,0xC2FA50u,0xC2FA54u,0xC2FA56u,0xC2FA58u,0xC2FA5Cu,0xC2FA5Eu,0xC2FA62u,0xC2FA64u,0xC2FA68u,0xC2FA6Au,0xC2FA6Eu,0xC301F6u,0xC301FCu,0xC30202u,0xC30204u,0xC30206u,0xC3020Au,0xC3020Cu,0xC3020Eu,0xC30210u,0xC30212u,0xC30214u,0xC30216u,0xC30218u,0xC3021Au,0xC3021Cu,0xC3021Eu,0xC30220u,0xC30222u,0xC30224u,0xC30226u,0xC30228u,0xC3022Au,0xC3022Cu,0xC3022Eu,0xC30230u,0xC30232u,0xC30234u,0xC30236u,0xC30238u,0xC3023Au,0xC3023Cu,0xC3023Eu,0xC30240u,0xC30242u,0xC30244u,0xC30246u,0xC30248u,0xC3024Au,0xC3024Cu,0xC3024Eu,0xC30250u,0xC30252u,0xC30254u,0xC30256u,0xC30258u,0xC3025Au,0xC3025Cu,0xC3025Eu,0xC30260u,0xC30262u,0xC30264u,0xC30266u,0xC30268u,0xC3026Cu,0xC3026Eu,0xC30272u,0xC30274u,0xC30276u,0xC30278u,0xC3027Au,0xC3027Cu,0xC30280u,0xC30282u,0xC30284u,0xC30286u,0xC30288u,0xC3028Au,0xC3028Eu,0xC30290u,0xC30292u,0xC30294u,0xC30296u,0xC30298u,0xC3029Cu,0xC3029Eu,0xC302A4u,0xC302AAu,0xC302ACu,0xC302B6u,0xC302BAu,0xC302C0u,0xC302C2u,0xC302C4u,0xC302C6u,0xC302C8u,0xC302CAu,0xC302CCu,0xC302CEu,0xC302D2u,0xC302D4u,0xC302D6u,0xC302DAu,0xC302DCu,0xC302DEu,0xC302E0u,0xC302E2u,0xC302E4u,0xC302E6u,0xC302EAu,0xC302ECu,0xC302EEu,0xC302F0u,0xC302F2u,0xC302F4u,0xC302FCu,0xC30306u,0xC3030Cu,0xC30312u,0xC30314u,0xC30316u,0xC3031Cu,0xC30320u,0xC30322u,0xC30324u,0xC30328u,0xC3032Au,0xC3032Cu,0xC3032Eu,0xC30330u,0xC30332u,0xC3033Au,0xC3033Cu,0xC30340u,0xC30342u,0xC3034Au,0xC3034Cu,0xC3034Eu,0xC30350u,0xC30352u,0xC30354u,0xC30356u,0xC30358u,0xC3035Cu,0xC30362u,0xC30364u,0xC30366u,0xC30368u,0xC3036Au,0xC3036Cu,0xC3036Eu,0xC30370u,0xC30372u,0xC30374u,0xC30376u,0xC30378u,0xC3037Au,0xC3037Cu,0xC3037Eu,0xC30380u,0xC30382u,0xC30384u,0xC30386u,0xC30388u,0xC3038Au,0xC3038Cu,0xC3038Eu,0xC30390u,0xC30392u,0xC30394u,0xC3039Au,0xC3039Cu,0xC303A2u,0xC303A4u,0xC303AAu,0xC303ACu,0xC303B2u,0xC303B8u,0xC303BAu,0xC303BCu,0xC303BEu,0xC303C0u,0xC303C2u,0xC303C4u,0xC303CAu,0xC303CCu,0xC303D2u,0xC303D8u,0xC303DAu,0xC303DCu,0xC303DEu,0xC303E0u,0xC303E6u,0xC303ECu,0xC303F0u,0xC303F4u,0xC303F8u,0xC303FCu,0xC30400u,0xC30404u,0xC30408u,0xC3040Au,0xC330FEu,0xC33100u,0xC33102u,0xC33104u,0xC33106u,0xC33108u,0xC3310Au,0xC3310Cu,0xC3310Eu,0xC33110u,0xC33112u,0xC33114u,0xC33116u,0xC3311Au,0xC3311Eu,0xC33120u,0xC33122u,0xC33124u,0xC33126u,0xC33128u,0xC3312Au,0xC3312Cu,0xC3312Eu,0xC33130u,0xC33132u,0xC33134u,0xC33138u,0xC3313Cu,0xC3313Eu,0xC33140u,0xC33142u,0xC33144u,0xC33146u,0xC33148u,0xC3314Au,0xC3314Cu,0xC3314Eu,0xC33150u,0xC33152u,0xC33154u,0xC33156u,0xC3315Au,0xC3315Eu,0xC33160u,0xC33162u,0xC33164u,0xC33166u,0xC33168u};
static unsigned char visited[FA18_SLOW_SIZE/2];
static int source_owned(uint32_t pc) { unsigned i; for(i=0;i<sizeof source_boundaries/sizeof source_boundaries[0];++i) if(source_boundaries[i]==pc) return 1; return 0; }
static unsigned source_slot(uint32_t pc) { return (pc-FA18_SLOW_BASE)/2; }
static uint32_t source_pc(unsigned slot) { return FA18_SLOW_BASE+2*slot; }
static int source_call(uint32_t ret,uint32_t sp) {
    unsigned dispatch;
    uint32_t child_ret=0,child_sp=0;
    for(dispatch=0;dispatch<1000000;++dispatch) {
        fa18_next_event=INT64_MAX;
        int lo=0,hi=fa18_recomp_entry_count,result;
        if(REG_PC==ret && REG_A[7]==sp) return FA18_RET;
        if(source_owned(REG_PC)) {
            uint32_t pc=REG_PC;
            uint16_t opcode=m68k_read_memory_16(pc);
            visited[source_slot(pc)]=1;
            child_ret=0;
            REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
            m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
            if((opcode&0xff00u)==0x6100u||(opcode&0xffc0u)==0x4e80u)
                fa18_render_entry_source_child(rd_u32(REG_A[7]),REG_A[7]+4);
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
#define HP_ORIGINAL_CHILDREN 1
#include "render_entry_helpers_fixture.h"
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
        fprintf(stderr,"render entry helper oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_render_entry_controlled_begin(selected_entry,scenario); fa18_write_log_active=2;
        fa18_render_entry_helpers_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"render entry helper oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_render_entry_controlled_begin(selected_entry,scenario);
        fa18_render_entry_helpers_fixture_begin("C");
        switch(selected_entry) {
        case 0xc2f688u: glue_C2F688(); break;
        case 0xc2f63au: glue_C2F63A(); break;
        case 0xc2f64eu: glue_C2F64E(); break;
        case 0xc301f6u: glue_C301F6(); break;
        case 0xc330feu: glue_C330FE(); break;
        case 0xc2f826u: glue_C2F826(); break;
        case 0xc2f83au: glue_C2F83A(); break;
        case 0xc2f844u: glue_C2F844(); break;
        case 0xc2f84eu: glue_C2F84E(); break;
        case 0xc2f858u: glue_C2F858(); break;
        case 0xc2f862u: glue_C2F862(); break;
        case 0xc2f86cu: glue_C2F86C(); break;
        case 0xc2f876u: glue_C2F876(); break;
        case 0xc2f880u: glue_C2F880(); break;
        case 0xc2f88au: glue_C2F88A(); break;
        case 0xc2f894u: glue_C2F894(); break;
        case 0xc2f89eu: glue_C2F89E(); break;
        case 0xc2f8a8u: glue_C2F8A8(); break;
        case 0xc2f8b2u: glue_C2F8B2(); break;
        case 0xc2f8bcu: glue_C2F8BC(); break;
        case 0xc2f8c6u: glue_C2F8C6(); break;
        case 0xc2f8d0u: glue_C2F8D0(); break;
        case 0xc2f8eau: glue_C2F8EA(); break;
        case 0xc2f904u: glue_C2F904(); break;
        case 0xc2f91eu: glue_C2F91E(); break;
        case 0xc2f938u: glue_C2F938(); break;
        case 0xc2f952u: glue_C2F952(); break;
        case 0xc2f96cu: glue_C2F96C(); break;
        case 0xc2f986u: glue_C2F986(); break;
        case 0xc2f9a0u: glue_C2F9A0(); break;
        case 0xc2f9bau: glue_C2F9BA(); break;
        case 0xc2f9d4u: glue_C2F9D4(); break;
        case 0xc2f9eeu: glue_C2F9EE(); break;
        case 0xc2fa08u: glue_C2FA08(); break;
        case 0xc2fa22u: glue_C2FA22(); break;
        case 0xc2fa3cu: glue_C2FA3C(); break;
        case 0xc2fa56u: glue_C2FA56(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"render entry helper oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"render entry helper oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"render entry helper oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("render entry helper oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
