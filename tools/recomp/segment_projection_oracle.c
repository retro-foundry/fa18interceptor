/* Complete segment projection proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc1ff9cu;
static const uint32_t source_boundaries[]={0xC1FF9Cu,0xC1FFA2u,0xC1FFA4u,0xC1FFAEu,0xC1FFB0u,0xC1FFB2u,0xC1FFB4u,0xC1FFBAu,0xC1FFC4u,0xC1FFCAu,0xC1FFD0u,0xC1FFD2u,0xC1FFD6u,0xC1FFDAu,0xC1FFDCu,0xC1FFDEu,0xC1FFE2u,0xC1FFE6u,0xC1FFECu,0xC1FFEEu,0xC1FFF0u,0xC1FFF2u,0xC1FFF6u,0xC1FFF8u,0xC1FFFCu,0xC1FFFEu,0xC20000u,0xC2EA5Au,0xC2EA5Eu,0xC2EA64u,0xC2EA66u,0xC2EA68u,0xC2EA6Au,0xC2EA6Cu,0xC2EA6Eu,0xC2EA70u,0xC2EA72u,0xC2EA74u,0xC2EA76u,0xC2EA78u,0xC2EA7Au,0xC2EA7Cu,0xC2EA7Eu,0xC2EA80u,0xC2EA82u,0xC2EA84u,0xC2EA86u,0xC2EA88u,0xC2EA8Au,0xC2EA8Cu,0xC2EA8Eu,0xC2EA90u,0xC2EA92u,0xC2EA94u,0xC2EA96u,0xC2EA98u,0xC2EA9Au,0xC2EA9Cu,0xC2EA9Eu,0xC2EAA0u,0xC2EAA2u,0xC2EAA4u,0xC2EAA6u,0xC2EAA8u,0xC2EAAAu,0xC2EAACu,0xC2EAAEu,0xC2EAB0u,0xC2EAB2u,0xC2EAB4u,0xC2EAB6u,0xC2EAB8u,0xC2EABAu,0xC2EABCu,0xC2EABEu,0xC2EAC0u,0xC2EAC2u,0xC2EAC4u,0xC2EAC6u,0xC2EAC8u,0xC2EACCu,0xC2EAD0u,0xC2EAD4u,0xC2EADAu,0xC2EADCu,0xC2EADEu,0xC2EAE0u,0xC2EAE2u,0xC2EAE4u,0xC2EAE6u,0xC2EAE8u,0xC2EAEAu,0xC2EAECu,0xC2EAEEu,0xC2EAF0u,0xC2EAF2u,0xC2EAF4u,0xC2EAF6u,0xC2EAF8u,0xC2EAFAu,0xC2EAFCu,0xC2EAFEu,0xC2EB00u,0xC2EB02u,0xC2EB04u,0xC2EB06u,0xC2EB08u,0xC2EB0Au,0xC2EB0Cu,0xC2EB0Eu,0xC2EB10u,0xC2EB12u,0xC2EB14u,0xC2EB16u,0xC2EB18u,0xC2EB1Au,0xC2EB1Cu,0xC2EB1Eu,0xC2EB20u,0xC2EB22u,0xC2EB24u,0xC2EB26u,0xC2EB28u,0xC2EB2Au,0xC2EB2Cu,0xC2EB2Eu,0xC2EB30u,0xC2EB32u,0xC2EB34u,0xC2EB36u,0xC2EB38u,0xC2EB3Au,0xC2EB3Cu,0xC2EB3Eu,0xC2EB40u,0xC2EB42u,0xC2EB44u,0xC2EB48u,0xC2EB4Cu,0xC2EB50u,0xC2EB56u,0xC2EB58u,0xC2EB5Au,0xC2EB5Cu,0xC2EB5Eu,0xC2EB60u,0xC2EB62u,0xC2EB64u,0xC2EB66u,0xC2EB68u,0xC2EB6Au,0xC2EB6Cu,0xC2EB6Eu,0xC2EB70u,0xC2EB72u,0xC2EB74u,0xC2EB76u,0xC2EB78u,0xC2EB7Au,0xC2EB7Cu,0xC2EB7Eu,0xC2EB80u,0xC2EB82u,0xC2EB84u,0xC2EB86u,0xC2EB88u,0xC2EB8Au,0xC2EB8Cu,0xC2EB8Eu,0xC2EB90u,0xC2EB92u,0xC2EB94u,0xC2EB96u,0xC2EB98u,0xC2EB9Au,0xC2EB9Cu,0xC2EB9Eu,0xC2EBA0u,0xC2EBA2u,0xC2EBA4u,0xC2EBA6u,0xC2EBA8u,0xC2EBAAu,0xC2EBACu,0xC2EBAEu,0xC2EBB0u,0xC2EBB2u,0xC2EBB4u,0xC2EBB6u,0xC2EBB8u,0xC2EBBAu,0xC2EBBEu,0xC2EBC2u,0xC2EBC6u,0xC2EBCCu,0xC2EBCEu,0xC2EBD0u,0xC2EBD2u,0xC2EBD4u,0xC2EBD6u,0xC2EBD8u,0xC2EBDAu,0xC2EBDCu,0xC2EBDEu,0xC2EBE0u,0xC2EBE2u,0xC2EBE4u,0xC2EBE6u,0xC2EBE8u,0xC2EBEAu,0xC2EBECu,0xC2EBEEu,0xC2EBF0u,0xC2EBF2u,0xC2EBF4u,0xC2EBF6u,0xC2EBF8u,0xC2EBFAu,0xC2EBFCu,0xC2EBFEu,0xC2EC00u,0xC2EC02u,0xC2EC04u,0xC2EC06u,0xC2EC08u,0xC2EC0Au,0xC2EC0Cu,0xC2EC0Eu,0xC2EC10u,0xC2EC12u,0xC2EC14u,0xC2EC16u,0xC2EC18u,0xC2EC1Au,0xC2EC1Cu,0xC2EC1Eu,0xC2EC20u,0xC2EC22u,0xC2EC24u,0xC2EC26u,0xC2EC28u,0xC2EC2Au,0xC2EC2Cu,0xC2EC2Eu,0xC2EC30u,0xC2EC32u,0xC2EC34u,0xC2EC36u,0xC2EC3Eu,0xC2EC40u,0xC2EC42u,0xC2EC44u,0xC2EC46u,0xC2EC48u,0xC2EC4Au,0xC2EC4Cu,0xC2EC4Eu,0xC2EC50u,0xC2EC52u,0xC2EC54u,0xC2EC56u,0xC2EC58u,0xC2EC5Au,0xC2EC5Eu,0xC2EC60u,0xC2EC62u,0xC2EC66u,0xC2ED6Cu,0xC2ED6Eu,0xC2ED70u,0xC2ED76u,0xC2ED78u,0xC2ED7Au,0xC2ED7Cu,0xC2ED7Eu,0xC2ED80u,0xC2ED82u,0xC2ED84u,0xC2ED86u,0xC2ED88u,0xC2ED8Au,0xC2ED8Cu,0xC2ED8Eu,0xC2ED90u,0xC2ED92u,0xC2ED94u,0xC2ED96u,0xC2ED9Au,0xC2ED9Cu,0xC2EDA0u,0xC2EDA2u,0xC2EDA4u,0xC2EDA6u,0xC2EDAAu,0xC2EDACu,0xC2EDB0u,0xC2EDB4u,0xC2EDB6u,0xC2EDBAu,0xC2EDBCu,0xC2EDBEu,0xC2EDC0u,0xC2EDC4u,0xC2EDC6u,0xC2EDCAu,0xC2EDCEu,0xC2EDD0u,0xC2EDD4u,0xC2EDD6u,0xC2EDD8u,0xC2EDDAu,0xC2EDDCu,0xC2EDDEu,0xC2EDE0u,0xC2EDE2u,0xC2EDE4u,0xC2EDE6u,0xC2EDE8u,0xC2EDEAu,0xC2EDECu,0xC2EDEEu,0xC2EDF0u,0xC2EDF2u,0xC2EDF4u,0xC2EDF6u,0xC2EDFAu,0xC2EDFCu,0xC2EE00u,0xC2EE02u,0xC2EE04u,0xC2EE06u,0xC2EE0Au,0xC2EE0Cu,0xC2EE10u,0xC2EE14u,0xC2EE16u,0xC2EE1Au,0xC2EE1Cu,0xC2EE1Eu,0xC2EE20u,0xC2EE24u,0xC2EE26u,0xC2EE2Au,0xC2EE2Eu,0xC2EE30u,0xC2EE34u,0xC2EE36u,0xC2EE3Cu,0xC2EE3Eu,0xC2EE40u,0xC2EE42u,0xC2EE44u,0xC2EE46u,0xC2EE48u,0xC2EE4Au,0xC2EE4Eu,0xC2EE54u,0xC2EE5Au,0xC2EE60u,0xC2EE64u,0xC2EE66u,0xC2EE68u,0xC2EE6Cu,0xC2EE70u,0xC2EE72u,0xC2EE74u,0xC2EE78u,0xC2EE7Cu,0xC2EE82u,0xC2EE84u,0xC2EE86u,0xC2EE88u,0xC2EE8Au,0xC2EE8Eu,0xC2EE90u,0xC2EE94u,0xC2EE96u,0xC2EE98u,0xC2EE9Au,0xC2EE9Eu,0xC2EEA2u,0xC2EEA6u,0xC2EEA8u,0xC2EEAAu,0xC2EEACu,0xC2EEB0u,0xC2EEB4u,0xC2EEBAu,0xC2EEBCu,0xC2EEBEu,0xC2EEC0u,0xC2EEC2u,0xC2EEC6u,0xC2EECAu,0xC2EECCu,0xC2EECEu,0xC2EED0u,0xC2EED2u,0xC2EED4u,0xC2EED8u,0xC2EEDCu,0xC2EEE0u,0xC2EEE2u,0xC2EEE4u,0xC2EEE8u,0xC2EEECu,0xC2EEEEu,0xC2EEF4u,0xC2EEF8u,0xC2EEFAu,0xC2EEFEu,0xC2EF00u,0xC2EF02u,0xC2EF06u,0xC2EF0Au,0xC2EF0Eu,0xC2EF12u,0xC2EF14u,0xC2EF18u,0xC2EF1Cu,0xC2EF20u,0xC2EF22u,0xC2EF26u,0xC2EF2Au,0xC2EF2Cu,0xC2EF32u,0xC2EF36u,0xC2EF38u,0xC2EF3Au,0xC2EF3Cu,0xC2EF40u,0xC2EF44u,0xC2EF46u,0xC2EF48u,0xC2EF4Cu,0xC2EF50u,0xC2EF54u,0xC2EF58u,0xC2EF5Au,0xC2EF5Cu,0xC2EF60u,0xC2EF64u,0xC2EF66u,0xC2EF6Au,0xC2EF6Eu,0xC2EF70u,0xC2EF76u,0xC2EF78u,0xC2EF7Au,0xC2EF7Cu,0xC2EF7Eu,0xC2EF82u,0xC2EF84u,0xC2EF88u,0xC2EF8Au,0xC2EF8Cu,0xC2EF8Eu,0xC2EF92u,0xC2EF96u,0xC2EF9Au,0xC2EF9Cu,0xC2EF9Eu,0xC2EFA2u,0xC2EFA6u,0xC2EFA8u,0xC2EFAEu,0xC2EFB0u,0xC2EFB2u,0xC2EFB4u,0xC2EFB6u,0xC2EFBAu,0xC2EFBEu,0xC2EFC0u,0xC2EFC2u,0xC2EFC4u,0xC2EFC6u,0xC2EFC8u,0xC2EFCAu,0xC2EFCEu,0xC2EFD2u,0xC2EFD4u,0xC2EFD6u,0xC2EFD8u,0xC2EFDCu,0xC2EFDEu,0xC2EFE4u,0xC2EFE6u,0xC2EFE8u,0xC2EFEAu,0xC2EFECu,0xC2EFEEu,0xC2EFF0u,0xC2EFF4u,0xC2EFF6u,0xC2EFF8u,0xC2EFFAu,0xC2EFFCu,0xC2F000u,0xC2F004u,0xC2F006u,0xC2F008u,0xC2F00Cu,0xC2F00Eu,0xC2F014u,0xC2F016u,0xC2F018u,0xC2F01Au,0xC2F01Cu,0xC2F01Eu,0xC2F022u,0xC2F024u,0xC2F026u,0xC2F028u,0xC2F02Cu,0xC2F02Eu,0xC2F030u,0xC2F032u,0xC2F034u,0xC2F036u,0xC2F038u,0xC2F03Au,0xC2F042u,0xC2F044u,0xC2F046u,0xC2F04Au,0xC2F04Cu,0xC2F050u,0xC2F052u,0xC2F056u,0xC2F058u,0xC2F05Cu,0xC2F05Eu,0xC2F062u,0xC2F064u,0xC2F068u,0xC2F06Au,0xC2F06Eu,0xC2F070u,0xC2F074u,0xC2F076u,0xC2F078u,0xC2F07Au,0xC2F07Eu,0xC2F080u,0xC2F088u,0xC2F08Eu,0xC2F090u,0xC2F092u,0xC2F094u,0xC2F09Cu,0xC2F0A0u,0xC2F0A4u,0xC2F0A8u,0xC2F0AEu,0xC2F0B2u,0xC2F0B4u,0xC2F0B6u,0xC2F0B8u,0xC2F0BAu,0xC2F0BEu,0xC2F0C0u,0xC2F0C4u,0xC2F0C6u,0xC2F0CAu,0xC2F0D0u,0xC2F0D2u,0xC2F0D4u,0xC2F0D6u,0xC2F0D8u,0xC2F0DAu,0xC2F0DCu,0xC2F0DEu,0xC2F0E0u,0xC2F0E2u,0xC2F0E4u,0xC2F0E6u,0xC2F0E8u,0xC2F0EAu,0xC2F0ECu,0xC2F0EEu,0xC2F0F0u,0xC2F0F4u,0xC2F0F8u,0xC2F0FEu,0xC2F100u,0xC2F102u,0xC2F104u,0xC2F106u,0xC2F108u,0xC2F10Au,0xC2F10Cu,0xC2F10Eu,0xC2F110u,0xC2F112u,0xC2F114u,0xC2F116u,0xC2F118u,0xC2F11Au,0xC2F11Cu,0xC2F11Eu,0xC2F120u,0xC2F122u,0xC2F124u,0xC2F128u,0xC2F12Cu,0xC2F132u,0xC2F134u,0xC2F136u,0xC2F138u,0xC2F13Au,0xC2F13Cu,0xC2F13Eu,0xC2F140u,0xC2F142u,0xC2F144u,0xC2F146u,0xC2F148u,0xC2F14Au,0xC2F14Cu,0xC2F14Eu,0xC2F150u,0xC2F152u,0xC2F156u,0xC2F15Au,0xC2F160u,0xC2F162u,0xC2F164u,0xC2F166u,0xC2F168u,0xC2F16Au,0xC2F16Cu,0xC2F16Eu,0xC2F170u,0xC2F172u,0xC2F174u,0xC2F176u,0xC2F178u,0xC2F17Au,0xC2F17Cu,0xC2F17Eu,0xC2F180u,0xC2F182u,0xC2F184u,0xC2F186u,0xC2F18Eu,0xC2F190u,0xC2F192u,0xC2F194u,0xC2F196u,0xC2F198u,0xC2F19Au,0xC2F19Cu,0xC2F19Eu,0xC2F1A0u,0xC2F1A2u,0xC2F1A4u,0xC2F1A6u,0xC2F1A8u,0xC2F1AAu,0xC2F1AEu,0xC2F1B0u,0xC2F1B2u,0xC2F1B6u};
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
#include "segment_projection_fixture.h"
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
        fprintf(stderr,"segment projection oracle: %s\n",error); return 1;
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
            fprintf(stderr,"segment projection oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_render_entry_controlled_begin(selected_entry,scenario);
        fa18_render_entry_helpers_fixture_begin("C");
        switch(selected_entry) {
        case 0xc1ff9cu: glue_C1FF9C(); break;
        case 0xc1ffa4u: glue_C1FFA4(); break;
        case 0xc2ed70u: glue_C2ED70(); break;
        case 0xc2ee4au: glue_C2EE4A(); break;
        case 0xc2f0c6u: glue_C2F0C6(); break;
        case 0xc2f0f4u: glue_C2F0F4(); break;
        case 0xc2f128u: glue_C2F128(); break;
        case 0xc2f156u: glue_C2F156(); break;
        case 0xc2ea5au: glue_C2EA5A(); break;
        case 0xc2ead0u: glue_C2EAD0(); break;
        case 0xc2eb4cu: glue_C2EB4C(); break;
        case 0xc2ebc2u: glue_C2EBC2(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"segment projection oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"segment projection oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"segment projection oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("segment projection oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
