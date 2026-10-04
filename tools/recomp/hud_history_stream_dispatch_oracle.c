/* Complete HUD history/stream parent proof, including cold internal paths and real children. */
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
extern void fa18_hud_history_stream_fixture_begin(const char *phase);
extern int fa18_structural_port_matched(uint32_t entry);
extern int fa18_structural_port_unused(uint32_t entry);
extern int fa18_ports_enter(int function,int label,int via_call);
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
static uint32_t selected_entry=0xc0d04cu;
static const uint32_t source_boundaries[]={0xC0CF98u,0xC0CF9Eu,0xC0CFA0u,0xC0CFA6u,0xC0CFA8u,0xC0CFACu,0xC0CFB0u,0xC0CFB4u,0xC0D048u,0xC0D04Au,0xC0D04Cu,0xC0D052u,0xC0D058u,0xC0D05Au,0xC0D05Eu,0xC0D064u,0xC0D068u,0xC0D06Eu,0xC0D070u,0xC0D072u,0xC0D07Au,0xC0D07Cu,0xC0D082u,0xC0D086u,0xC0D08Cu,0xC0D090u,0xC0D098u,0xC0D09Au,0xC0D09Eu,0xC0D0A4u,0xC0D0AAu,0xC0D0B0u,0xC0D0B6u,0xC0D0BCu,0xC0D0C2u,0xC0D0C4u,0xC0D0C6u,0xC0D0C8u,0xC0D0CAu,0xC0D0CCu,0xC0D0CEu,0xC0D0D0u,0xC0D0D2u,0xC0D0D4u,0xC0D0D6u,0xC0D0D8u,0xC0D0DAu,0xC0D0DCu,0xC0D0E0u,0xC0D0E6u,0xC0D0ECu,0xC0D0F4u,0xC0D0F6u,0xC0D0F8u,0xC0D0FCu,0xC0D0FEu,0xC0D104u,0xC0D108u,0xC0D10Au,0xC0D110u,0xC0D116u,0xC0D11Cu,0xC0D120u,0xC0D122u,0xC0D124u,0xC0D126u,0xC0D128u,0xC0D12Au,0xC0D12Cu,0xC0D132u,0xC0D134u,0xC0D138u,0xC0D13Eu,0xC0D144u,0xC0D14Au,0xC0D14Cu,0xC0D14Eu,0xC0D150u,0xC0D152u,0xC0D154u,0xC0D156u,0xC0D158u,0xC0D15Au,0xC0D15Cu,0xC0D15Eu,0xC0D164u,0xC0D166u,0xC0D168u,0xC0D16Au,0xC0D16Cu,0xC0D16Eu,0xC0D170u,0xC0D172u,0xC0D178u,0xC0D17Au,0xC0D17Cu,0xC0D17Eu,0xC0D180u,0xC0D182u,0xC0D184u,0xC0D186u,0xC0D188u,0xC0D18Au,0xC0D18Cu,0xC0D18Eu,0xC0D190u,0xC0D192u,0xC0D198u,0xC0D19Au,0xC0D1A0u,0xC0D1A2u,0xC0D1AAu,0xC0D1ACu,0xC0D1B4u,0xC0D1B6u,0xC0D1BCu,0xC0D1BEu,0xC0D1C6u,0xC0D1C8u,0xC0D1D0u,0xC0D1D4u,0xC0D1D8u,0xC0D1DAu,0xC0D1DCu,0xC0D1DEu,0xC0D1E4u,0xC0D1E8u,0xC0D1EAu,0xC0D1ECu,0xC0D1EEu,0xC0D1F0u,0xC0D1F2u,0xC0D1F4u,0xC0D1FAu,0xC0D1FCu,0xC0D1FEu,0xC0D200u,0xC0D202u,0xC0D204u,0xC0D206u,0xC0D208u,0xC0D20Au,0xC0D20Eu,0xC0D210u,0xC0D214u,0xC0D218u,0xC0D21Cu,0xC0D222u,0xC0D226u,0xC0D22Au,0xC0D230u,0xC0D232u,0xC0D234u,0xC0D236u,0xC0D238u,0xC0D23Au,0xC0D23Cu,0xC0D23Eu,0xC0D240u,0xC0D242u,0xC0D244u,0xC0D246u,0xC0D248u,0xC0D24Au,0xC0D24Cu,0xC0D24Eu,0xC0D250u,0xC0D252u,0xC0D254u,0xC0D256u,0xC0D25Cu,0xC0D260u,0xC0D264u,0xC0D26Au,0xC0D26Cu,0xC0D26Eu,0xC0D270u,0xC0D272u,0xC0D274u,0xC0D276u,0xC0D278u,0xC0D27Au,0xC0D27Cu,0xC0D27Eu,0xC0D280u,0xC0D282u,0xC0D284u,0xC0D286u,0xC0D288u,0xC0D28Au,0xC0D28Cu,0xC0D28Eu,0xC0D290u,0xC0D296u,0xC0D29Au,0xC0D29Eu,0xC0D2A4u,0xC0D2A6u,0xC0D2A8u,0xC0D2AAu,0xC0D2ACu,0xC0D2AEu,0xC0D2B0u,0xC0D2B2u,0xC0D2B4u,0xC0D2B6u,0xC0D2B8u,0xC0D2BAu,0xC0D2BCu,0xC0D2BEu,0xC0D2C0u,0xC0D2C2u,0xC0D2C4u,0xC0D2C6u,0xC0D2C8u,0xC0D2CAu,0xC0D2D0u,0xC0D2D4u,0xC0D2D8u,0xC0D2DEu,0xC0D2E2u,0xC0D2E4u,0xC0D2E6u,0xC0D2E8u,0xC0D2EAu,0xC0D2F0u,0xC0D2F4u,0xC0D2F8u,0xC0D2FAu,0xC0D2FEu,0xC0D300u,0xC0D306u,0xC0D308u,0xC0D30Cu,0xC0D30Eu,0xC0D312u,0xC0D318u,0xC0D31Au,0xC0D31Eu,0xC0D320u,0xC0D324u,0xC0D328u,0xC0D32Cu,0xC0D330u,0xC0D332u,0xC1FE24u,0xC1FE2Au,0xC1FE2Cu,0xC1FE30u,0xC1FE36u,0xC1FE3Au,0xC1FE40u,0xC1FE44u,0xC1FE46u,0xC1FE4Cu,0xC1FE4Eu,0xC1FE52u,0xC1FE58u,0xC1FE5Cu,0xC1FE62u,0xC1FE66u,0xC33370u,0xC33378u,0xC3337Eu,0xC33384u,0xC3338Au,0xC3338Cu,0xC33392u,0xC33398u,0xC3339Au,0xC333A0u,0xC333A2u,0xC333A6u,0xC333A8u,0xC333AAu,0xC333ACu,0xC333AEu,0xC333B0u,0xC333B2u,0xC333B8u,0xC333BCu,0xC333C2u,0xC333C6u,0xC333C8u,0xC333CAu,0xC333D0u,0xC333D6u,0xC333DAu,0xC333DCu,0xC333E0u,0xC333E4u,0xC333E6u,0xC333E8u,0xC333EEu,0xC333F2u,0xC333F8u,0xC333FCu,0xC33400u,0xC33404u,0xC33408u,0xC3340Au,0xC3340Eu,0xC33414u,0xC3341Au,0xC3341Eu,0xC33420u,0xC33422u,0xC33428u,0xC3342Eu,0xC33434u,0xC33436u,0xC3343Au,0xC3343Eu,0xC33442u,0xC33444u,0xC33448u,0xC3344Au,0xC3344Eu,0xC33454u,0xC33456u,0xC3345Cu,0xC33466u,0xC3346Cu,0xC33472u,0xC33476u,0xC33478u,0xC3347Eu,0xC33484u,0xC3348Au,0xC33490u,0xC33496u,0xC3349Cu,0xC3349Eu,0xC334A2u,0xC334A6u,0xC334ACu,0xC334AEu,0xC334B2u,0xC334B8u,0xC334BEu,0xC334C0u,0xC334C2u,0xC334C4u,0xC334C8u,0xC334CAu,0xC334D0u,0xC334D2u,0xC334D4u,0xC334D6u,0xC334D8u,0xC334DAu,0xC334DCu,0xC334DEu,0xC334E4u,0xC334E6u,0xC334E8u,0xC334ECu,0xC334F6u,0xC334F8u,0xC334FAu,0xC33500u,0xC33502u,0xC33504u,0xC3350Au,0xC33510u,0xC33512u,0xC33516u,0xC33518u,0xC3351Au,0xC3351Eu,0xC33524u,0xC33526u,0xC33528u,0xC3352Eu,0xC33534u,0xC33538u,0xC3353Au,0xC3353Cu,0xC3353Eu,0xC33542u,0xC33544u,0xC33546u,0xC33548u,0xC3354Au,0xC3354Cu,0xC33552u,0xC33558u,0xC3355Cu,0xC33562u,0xC33568u,0xC3356Cu,0xC33570u,0xC33572u,0xC33574u,0xC33578u,0xC3357Eu,0xC33580u,0xC33582u,0xC33588u,0xC3358Eu,0xC33592u,0xC33594u,0xC33596u,0xC33598u,0xC3359Cu,0xC3359Eu,0xC335A0u,0xC335A2u,0xC335A6u,0xC335AAu,0xC335AEu,0xC335B2u,0xC335B6u,0xC335BCu,0xC335C2u,0xC335C4u,0xC335CAu,0xC335CCu,0xC335D0u,0xC335D2u,0xC335D4u,0xC335DAu,0xC335DEu,0xC335E4u,0xC335E8u,0xC335EAu,0xC335ECu,0xC335EEu,0xC335F2u,0xC335F4u,0xC335FAu,0xC33600u,0xC33604u,0xC33606u,0xC3360Au,0xC3360Eu,0xC33610u,0xC33612u,0xC33618u,0xC3361Cu,0xC33622u,0xC33626u,0xC3362Au,0xC3362Eu,0xC33632u,0xC33634u,0xC33638u,0xC3363Eu,0xC33644u,0xC33646u,0xC3364Au,0xC3364Cu,0xC3364Eu,0xC33654u,0xC3365Au,0xC33660u,0xC33662u,0xC33666u,0xC3366Au,0xC3366Eu,0xC33670u,0xC33674u,0xC33676u,0xC3367Au,0xC33680u,0xC33682u,0xC33688u,0xC33692u,0xC33698u,0xC3369Eu,0xC336A2u,0xC336A4u,0xC336AAu,0xC336B0u,0xC336B6u,0xC336BCu,0xC336C2u,0xC336C8u,0xC336CAu,0xC336CEu,0xC336D0u,0xC336D4u,0xC336DAu,0xC336E0u,0xC336E2u,0xC336E4u,0xC336E6u,0xC336E8u,0xC336EAu,0xC336ECu,0xC336F2u,0xC336F4u,0xC336F8u,0xC336FAu,0xC33704u,0xC3370Au,0xC3370Cu,0xC33712u,0xC33718u,0xC3371Au,0xC33720u,0xC33726u,0xC3372Au,0xC3372Cu,0xC3372Eu,0xC33730u,0xC33732u,0xC33736u,0xC33738u,0xC3373Au,0xC3373Cu,0xC33742u,0xC33748u,0xC3374Cu,0xC33752u,0xC33758u,0xC3375Au,0xC33760u,0xC33766u,0xC3376Au,0xC3376Cu,0xC3376Eu,0xC33770u,0xC33772u,0xC33776u,0xC33778u,0xC3377Au,0xC3377Eu,0xC33782u,0xC33786u,0xC3378Au,0xC3378Eu,0xC33792u,0xC33798u,0xC3379Cu,0xC3379Eu,0xC337A2u,0xC337A4u,0xC337A8u,0xC337AEu,0xC337B4u,0xC337B8u,0xC337BCu,0xC337C2u,0xC337C4u,0xC337CAu,0xC337CEu,0xC337D4u,0xC337D6u,0xC337DCu,0xC337E0u,0xC337E4u,0xC337EAu,0xC337ECu,0xC337EEu,0xC337F4u,0xC337F6u,0xC337FCu,0xC33802u,0xC33808u,0xC3380Cu,0xC3380Eu,0xC33810u,0xC33812u,0xC33818u,0xC3381Eu,0xC33824u,0xC33826u,0xC3382Au,0xC3382Eu,0xC33832u,0xC33834u,0xC3383Au,0xC33844u,0xC3384Au,0xC33850u,0xC33854u,0xC33856u,0xC3385Cu,0xC3385Eu,0xC33860u,0xC33866u,0xC3386Cu,0xC33872u,0xC33878u,0xC3387Eu,0xC33884u,0xC33886u,0xC3388Au,0xC3388Cu,0xC3388Eu,0xC33892u,0xC33898u,0xC3389Au,0xC338A0u,0xC338A2u,0xC338A4u,0xC338A8u,0xC338AAu,0xC338B4u,0xC338BAu,0xC338BCu,0xC338C0u,0xC338C4u,0xC338C6u,0xC338CCu,0xC338D2u,0xC338D6u,0xC338D8u,0xC338DAu,0xC338DCu,0xC338E6u,0xC338E8u,0xC338EEu,0xC338F4u,0xC338F6u,0xC338F8u,0xC338FCu,0xC338FEu,0xC33900u,0xC33902u,0xC33908u,0xC3390Eu,0xC33910u,0xC3391Au,0xC3391Eu,0xC33922u,0xC33924u,0xC3392Au,0xC33930u,0xC33934u,0xC33936u,0xC33938u,0xC3393Au,0xC33940u,0xC33942u,0xC33944u,0xC33948u,0xC3394Au,0xC3394Cu,0xC33952u,0xC33958u,0xC3395Cu};
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
#define HP_ORIGINAL_CHILDREN 1
#include "hud_history_stream_fixture.h"
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
    unsigned hardware_writes=0;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"HUD history/stream parent dispatch oracle: %s\n",error); return 1;
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
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_hud_hardware_begin(); saved_cycles=GET_CYCLES(); fa18_write_log_active=2;
        fa18_hud_history_stream_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"HUD history/stream parent dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        source_hardware=fa18_write_log_hardware!=0; hardware_cases+=source_hardware; matched_cases+=!source_hardware;
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_hud_hardware_begin(); SET_CYCLES(saved_cycles);
        fa18_write_log_active=0;
        fa18_ports_init(tested_mode,selection);
        fa18_hud_history_stream_fixture_begin("dispatch");
        {
            uint32_t previous=REG_PPC;
            int result;
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            if(fa18_ports_enter_source_only(0,&result)) { fputs("non-call source entry was accepted\n",stderr); return 1; }
            REG_PPC=previous;
            result=fa18_recomp_call_dynamic();
            if(tested_mode==FA18_PORTS_ON && (result!=FA18_EXIT_DISPATCH || fa18_ports_active_steps()!=1)) {
                fputs("HUD history/stream parent ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"HUD history/stream parent dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"HUD history/stream parent comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/hud_history_stream_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"HUD history/stream parent dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"HUD history/stream parent dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"HUD history/stream parent dispatch oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    {
        int result;
        unsigned guard;
        for(guard=0;guard<3;++guard) {
            int function=-1,label=-1,i;
            memcpy(m,base,sizeof *m); fixture(0);
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            fa18_ports_init(guard==0?FA18_PORTS_OFF:tested_mode,guard==1?"FFFFFE":selection);
            for(i=0;i<fa18_recomp_function_count;++i)
                if(fa18_recomp_functions[i].entry==selected_entry) function=i;
            for(i=0;i<fa18_recomp_entry_count;++i)
                if(fa18_recomp_entries[i].pc==selected_entry) label=(int)fa18_recomp_entries[i].label;
            if(function>=0) {
                if(label<0) { fputs("missing translated entry label\n",stderr); return 1; }
                result=guard==2?fa18_ports_enter(function,label,0):fa18_recomp_call_dynamic();
                /* A generated child can leave its caller for runtime
                 * completion. It must never start a native step here. */
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
      printf("HUD history/stream parent dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
