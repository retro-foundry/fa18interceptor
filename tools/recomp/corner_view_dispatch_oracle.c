/* Complete corner/view proof, including cold internal paths and real children. */
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
extern void fa18_render_entry_helpers_fixture_begin(const char *phase);
extern int fa18_structural_port_matched(uint32_t entry);
extern int fa18_structural_port_unused(uint32_t entry);
extern int fa18_ports_enter(int function,int label,int via_call);
extern unsigned fa18_hud_hardware_count(void);
extern void fa18_hud_hardware_begin(void);
extern void fa18_render_entry_controlled_begin(uint32_t entry,unsigned scenario);
extern void fa18_hud_hardware_reference(void);
extern int fa18_hud_hardware_check(void);
extern void fa18_render_entry_hardware_details(void);
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
static uint32_t selected_entry=0xc2e758u;
static const uint32_t source_boundaries[]={0xC200F6u,0xC200FAu,0xC200FEu,0xC203CCu,0xC203CEu,0xC2058Eu,0xC20590u,0xC20826u,0xC20828u,0xC22C70u,0xC2CCA0u,0xC2CCA4u,0xC2CCA8u,0xC2CCAAu,0xC2CCB0u,0xC2CCB2u,0xC2CCBAu,0xC2CCBEu,0xC2CCC4u,0xC2CCC6u,0xC2CCC8u,0xC2CCCAu,0xC2CCCEu,0xC2CCD4u,0xC2CCD6u,0xC2CCD8u,0xC2CCDAu,0xC2CCDEu,0xC2CCE0u,0xC2CCE2u,0xC2CCE8u,0xC2CCECu,0xC2CCEEu,0xC2CCF0u,0xC2CCF6u,0xC2CCFAu,0xC2CD00u,0xC2CD02u,0xC2CD06u,0xC2CD08u,0xC2CD0Au,0xC2CD0Cu,0xC2CD12u,0xC2CD14u,0xC2CD18u,0xC2CD1Cu,0xC2CD1Eu,0xC2CD24u,0xC2CD26u,0xC2CD28u,0xC2CD2Cu,0xC2CD30u,0xC2CD32u,0xC2CD38u,0xC2CD3Au,0xC2CD42u,0xC2CD48u,0xC2CD4Cu,0xC2CD50u,0xC2CD56u,0xC2CD58u,0xC2CD5Au,0xC2CD5Cu,0xC2CD60u,0xC2CD66u,0xC2CD68u,0xC2CD6Au,0xC2CD6Cu,0xC2CD72u,0xC2CD74u,0xC2CD76u,0xC2CD78u,0xC2CD7Au,0xC2CD7Cu,0xC2CD7Eu,0xC2CD82u,0xC2CD86u,0xC2CD88u,0xC2CD8Au,0xC2CD8Cu,0xC2CD8Eu,0xC2CD90u,0xC2CD94u,0xC2CD98u,0xC2CD9Cu,0xC2CD9Eu,0xC2CDA4u,0xC2CDA6u,0xC2CDAAu,0xC2CDB0u,0xC2CDB2u,0xC2CDB4u,0xC2CDB6u,0xC2CDBAu,0xC2CDC0u,0xC2CDC2u,0xC2CDC4u,0xC2CDC6u,0xC2CDCAu,0xC2CDCCu,0xC2CDCEu,0xC2CDD4u,0xC2CDD8u,0xC2CDDAu,0xC2CDDCu,0xC2CDE2u,0xC2CDE6u,0xC2CDECu,0xC2CDEEu,0xC2CDF4u,0xC2CDFAu,0xC2CE02u,0xC2CE0Au,0xC2CE10u,0xC2CE18u,0xC2CE1Eu,0xC2CE22u,0xC2CE26u,0xC2CE2Cu,0xC2CE30u,0xC2CE32u,0xC2CE34u,0xC2CE38u,0xC2CE3Cu,0xC2CE3Eu,0xC2CE40u,0xC2CE42u,0xC2CE48u,0xC2CE4Au,0xC2CE4Cu,0xC2CE52u,0xC2CE54u,0xC2CE58u,0xC2CE5Au,0xC2CE5Cu,0xC2CE82u,0xC2CE88u,0xC2CE8Cu,0xC2CE8Eu,0xC2CE90u,0xC2CE92u,0xC2CE94u,0xC2CE96u,0xC2CE98u,0xC2CE9Au,0xC2CE9Eu,0xC2CEA0u,0xC2CEA2u,0xC2CEA4u,0xC2CEA6u,0xC2CEA8u,0xC2CEAAu,0xC2CEACu,0xC2CEAEu,0xC2CEB0u,0xC2CEB2u,0xC2CEB4u,0xC2CEB6u,0xC2CEBCu,0xC2D080u,0xC2D082u,0xC2D08Au,0xC2D092u,0xC2D098u,0xC2D09Au,0xC2D0A0u,0xC2D0A2u,0xC2D0A4u,0xC2D0AAu,0xC2D0ACu,0xC2D0B0u,0xC2D0B2u,0xC2D0B6u,0xC2D0BAu,0xC2D0BCu,0xC2D0C0u,0xC2D0C4u,0xC2D0C6u,0xC2D0CAu,0xC2D0CEu,0xC2D0D0u,0xC2D0D2u,0xC2D0D4u,0xC2D0D6u,0xC2D0DAu,0xC2D0DCu,0xC2D0E0u,0xC2D0E2u,0xC2D0E4u,0xC2D0E6u,0xC2D0ECu,0xC2D0F2u,0xC2D0F8u,0xC2D0FAu,0xC2D0FCu,0xC2D0FEu,0xC2D100u,0xC2D102u,0xC2D104u,0xC2D106u,0xC2D10Au,0xC2D10Eu,0xC2D110u,0xC2D112u,0xC2D114u,0xC2D118u,0xC2D11Cu,0xC2D120u,0xC2D124u,0xC2D128u,0xC2D12Au,0xC2D12Cu,0xC2D12Eu,0xC2D130u,0xC2D132u,0xC2D136u,0xC2D13Au,0xC2D13Eu,0xC2D142u,0xC2D146u,0xC2D148u,0xC2D14Au,0xC2D14Cu,0xC2D14Eu,0xC2D150u,0xC2D154u,0xC2D158u,0xC2D15Cu,0xC2D160u,0xC2D164u,0xC2D168u,0xC2D16Au,0xC2D3A4u,0xC2D3ACu,0xC2D3AEu,0xC2D3B0u,0xC2D3B2u,0xC2D3B4u,0xC2D3B6u,0xC2D3B8u,0xC2D3BAu,0xC2D3BCu,0xC2D3BEu,0xC2D3C0u,0xC2D3C6u,0xC2D3C8u,0xC2D3CAu,0xC2D3CCu,0xC2D3CEu,0xC2D3D0u,0xC2D3D2u,0xC2D3D4u,0xC2D3DAu,0xC2D3DCu,0xC2D3DEu,0xC2D3E0u,0xC2D3E2u,0xC2D3E4u,0xC2D3E6u,0xC2D3E8u,0xC2D3EAu,0xC2D3ECu,0xC2D3EEu,0xC2D3F0u,0xC2D3F2u,0xC2D3F4u,0xC2D3FAu,0xC2E758u,0xC2E75Cu,0xC2E760u,0xC2E766u,0xC2E76Cu,0xC2E76Eu,0xC2E774u,0xC2E778u,0xC2E77Au,0xC2E77Cu,0xC2E77Eu,0xC2E782u,0xC2E784u,0xC2E786u,0xC2E788u,0xC2E78Au,0xC2E78Cu,0xC2E78Eu,0xC2E790u,0xC2E792u,0xC2E796u,0xC2E798u,0xC2E79Au,0xC2E79Cu,0xC2E79Eu,0xC2E7A0u,0xC2E7A4u,0xC2E7A8u,0xC2E7AAu,0xC2E7AEu,0xC2E7B2u,0xC2E7B4u,0xC2E7BAu,0xC2E7BCu,0xC2E7BEu,0xC2E7C0u,0xC2E7C2u,0xC2E7C6u,0xC2E7CAu,0xC2E7CCu,0xC2E7D0u,0xC2E7D4u,0xC2E7D8u,0xC2E7DEu,0xC2E7E0u,0xC2E7E2u,0xC2E7E4u,0xC2E7E6u,0xC2E7EAu,0xC2E7ECu,0xC2E7F2u,0xC2E7F8u,0xC2E7FCu,0xC2E7FEu,0xC2E800u,0xC2E802u,0xC2E806u,0xC2E80Au,0xC2E80Eu,0xC2E810u,0xC2E812u,0xC2E816u,0xC2E81Au,0xC2E81Eu,0xC2E824u,0xC2E826u,0xC2E828u,0xC2E82Au,0xC2E82Cu,0xC2E830u,0xC2E834u,0xC2E836u,0xC2E838u,0xC2E83Au,0xC2E83Cu,0xC2E83Eu,0xC2E842u,0xC2E846u,0xC2E84Au,0xC2E84Cu,0xC2E84Eu,0xC2E852u,0xC2E856u,0xC2E858u,0xC2E85Eu,0xC2E862u,0xC2E864u,0xC2E868u,0xC2E86Au,0xC2E86Cu,0xC2E870u,0xC2E874u,0xC2E878u,0xC2E87Eu,0xC2E884u,0xC2E888u,0xC2E88Au,0xC2E88Eu,0xC2E892u,0xC2E896u,0xC2E898u,0xC2E89Cu,0xC2E8A0u,0xC2E8A2u,0xC2E8A8u,0xC2E8ACu,0xC2E8AEu,0xC2E8B0u,0xC2E8B2u,0xC2E8B6u,0xC2E8BAu,0xC2E8BCu,0xC2E8BEu,0xC2E8C2u,0xC2E8C6u,0xC2E8CAu,0xC2E8D0u,0xC2E8D6u,0xC2E8DAu,0xC2E8DCu,0xC2E8DEu,0xC2E8E2u,0xC2E8E6u,0xC2E8E8u,0xC2E8ECu,0xC2E8F0u,0xC2E8F2u,0xC2E8F8u,0xC2E8FAu,0xC2E8FCu,0xC2E8FEu,0xC2E900u,0xC2E904u,0xC2E906u,0xC2E90Cu,0xC2E912u,0xC2E916u,0xC2E918u,0xC2E91Au,0xC2E91Cu,0xC2E920u,0xC2E924u,0xC2E928u,0xC2E92Au,0xC2E92Cu,0xC2E930u,0xC2E934u,0xC2E936u,0xC2E93Cu,0xC2E93Eu,0xC2E940u,0xC2E942u,0xC2E944u,0xC2E948u,0xC2E94Cu,0xC2E94Eu,0xC2E950u,0xC2E952u,0xC2E954u,0xC2E956u,0xC2E95Au,0xC2E95Eu,0xC2E962u,0xC2E964u,0xC2E966u,0xC2E968u,0xC2E96Cu,0xC2E96Eu,0xC2E974u,0xC2E976u,0xC2E978u,0xC2E97Au,0xC2E97Cu,0xC2E97Eu,0xC2E980u,0xC2E984u,0xC2E986u,0xC2E98Cu,0xC2E992u,0xC2E994u,0xC2E996u,0xC2E998u,0xC2E99Cu,0xC2E9A0u,0xC2E9A2u,0xC2E9A4u,0xC2E9A8u,0xC2E9AAu,0xC2E9B0u,0xC2E9B2u,0xC2E9B4u,0xC2E9B6u,0xC2E9B8u,0xC2E9BAu,0xC2E9BEu,0xC2E9C0u,0xC2E9C2u,0xC2E9C4u,0xC2E9C8u,0xC2E9CAu,0xC2E9D0u,0xC2E9D6u,0xC2E9D8u,0xC2E9DAu,0xC2E9DCu,0xC2E9DEu,0xC2E9E0u,0xC2E9E2u,0xC2E9E6u,0xC2E9EAu,0xC2E9EEu,0xC2E9F0u,0xC2E9F6u,0xC2E9F8u,0xC2EA00u,0xC2EA02u,0xC2EA04u,0xC2EA08u,0xC2EA0Au,0xC2EA0Cu,0xC2EA0Eu,0xC2EA10u,0xC2EA14u,0xC2EA16u,0xC2EA1Au,0xC2EA1Cu,0xC2EA20u,0xC2EA22u,0xC2EA24u,0xC2EA26u,0xC2EA28u,0xC2EA2Cu,0xC2EA2Eu,0xC2EA32u,0xC2EA34u,0xC2EA38u,0xC2EA3Au,0xC2EA3Eu,0xC2EA42u,0xC2EA44u,0xC2EA46u,0xC2EA48u,0xC2EA4Au,0xC2EA4Cu,0xC2EA4Eu,0xC2EA52u,0xC2EA54u,0xC2EA58u};
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
        /* Match the runtime's instruction boundary, including actual due
         * blitter completion. Whole-call oracles deliberately hold events. */
        fa18_bus_finish(REG_PC);fa18_bus_instruction();
        if(fa18_machine_service())return FA18_EXIT_INTERP;
        fa18_bus_instruction();
        if(source_owned(REG_PC)) {
            uint32_t pc=REG_PC;
            uint16_t opcode=m68k_read_memory_16(pc);
            visited[source_slot(pc)]=1;
            child_ret=0;
            REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
            m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
            /* Dispatch uses the production generated-child continuation on
             * both sides. Independent whole-C proofs use original children. */
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
        /* Generated children yield at due hardware boundaries. Resume the
         * same event handoff as fa18_recomp_resume; the fixture holds service
         * at its next boundary while retaining the real device clock. */
        if(result==FA18_EXIT_INTERP && !fa18_machine_event_due()) return result;
    }
    return FA18_EXIT_INTERP;
}
#define HP_ORIGINAL_CHILDREN 1
#include "corner_view_fixture.h"
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
        fprintf(stderr,"corner/view dispatch oracle: %s\n",error); return 1;
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
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_render_entry_controlled_begin(selected_entry,scenario); saved_cycles=GET_CYCLES();
        /* Sandbox references suppress Custom writes by design. Its baseline
         * must use that same original write policy, especially for counted
         * busy reads; physical blit execution is proven by the whole oracles. */
        fa18_write_log_active=2;
        fa18_render_entry_helpers_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"corner/view dispatch oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        source_hardware=fa18_write_log_hardware!=0; hardware_cases+=source_hardware; matched_cases+=!source_hardware;
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_render_entry_controlled_begin(selected_entry,scenario); SET_CYCLES(saved_cycles);
        fa18_write_log_active=0;
        fa18_ports_init(tested_mode,selection);
        fa18_render_entry_helpers_fixture_begin("dispatch");
        {
            uint32_t previous=REG_PPC;
            int result;
            wr_u16(0xc70010u,0x4e71u); REG_PPC=0xc70010u;
            if(fa18_ports_enter_source_only(0,&result)) { fputs("non-call source entry was accepted\n",stderr); return 1; }
            REG_PPC=previous;
            result=fa18_recomp_call_dynamic();
            if(tested_mode==FA18_PORTS_ON && selected_entry!=0xc200f6u && (result!=FA18_EXIT_DISPATCH || fa18_ports_active_steps()!=1)) {
                fputs("corner/view ON entry did not start its native continuation\n",stderr); return 1;
            }
            if(result==FA18_EXIT_DISPATCH) result=fa18_recomp_resume(0xc70000u,expected_sp);
            if(result!=FA18_RET || fa18_ports_active_steps()) {
                fprintf(stderr,"corner/view dispatch case %u mode %u did not complete at %06X\n",scenario,tested_mode,REG_PC); return 1;
            }
            if(tested_mode!=FA18_PORTS_ON && !fa18_structural_port_classified(selected_entry,source_hardware)) {
                fprintf(stderr,"corner/view comparison case %u mode %u did not match\n",scenario,tested_mode);
                fa18_ports_report("build/recomp/corner_view_dispatch_failed_report.json"); return 1;
            }
        }
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fa18_render_entry_hardware_details();fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"corner/view dispatch oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"corner/view dispatch oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"corner/view dispatch oracle: case %u byte %06X source %02X C %02X\n",
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
                /* A source guard may stop at an existing generated cold
                 * boundary. This proves admission/counting, not a completed
                 * source call; native continuation must still be absent. */
                if((result!=FA18_RET && result!=FA18_EXIT_INTERP) || fa18_ports_active_steps()) {
                    fprintf(stderr,"guarded translated entry result %d guard %u PC %06X active %u\n",result,guard,REG_PC,(unsigned)fa18_ports_active_steps()); return 1;
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
      printf("corner/view dispatch oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("classification: %u hardware-bearing source calls, %u hardware-free source calls; reference modes require exact hardware or matched classification respectively\n",hardware_cases,matched_cases);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
