/* Complete projection-readouts proof, including cold internal paths and real children. */
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
extern void fa18_projection_readouts_fixture_begin(const char *phase);
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
static uint32_t selected_entry=0xc2ec90u;
static const uint32_t source_boundaries[]={0xC2EC70u,0xC2EC78u,0xC2EC7Eu,0xC2EC80u,0xC2EC82u,0xC2EC84u,0xC2EC8Eu,0xC2EC90u,0xC2EC92u,0xC2EC94u,0xC2EC9Au,0xC2EC9Cu,0xC2EC9Eu,0xC2ECA4u,0xC2ECA6u,0xC2ECA8u,0xC2ECAAu,0xC2ECACu,0xC2ECAEu,0xC2ECB0u,0xC2ECB2u,0xC2ECB4u,0xC2ECB6u,0xC2ECB8u,0xC2ECBAu,0xC2ECBCu,0xC2ECBEu,0xC2ECC0u,0xC2ECC2u,0xC2ECC4u,0xC2ECC6u,0xC2ECCAu,0xC2ECCCu,0xC2ECD0u,0xC2ECD2u,0xC2ECD6u,0xC2ECD8u,0xC2ECDCu,0xC2ECDEu,0xC2ECE2u,0xC2ECE4u,0xC2ECE8u,0xC2ECEAu,0xC2ECEEu,0xC2ECF0u,0xC2ECF4u,0xC2ECF6u,0xC2ECF8u,0xC2ECFEu,0xC2ED00u,0xC2ED08u,0xC2ED0Au,0xC2ED0Cu,0xC2ED10u,0xC2ED12u,0xC2ED16u,0xC2ED18u,0xC2ED1Cu,0xC2ED1Eu,0xC2ED22u,0xC2ED24u,0xC2ED2Au,0xC2ED2Cu,0xC2ED32u,0xC2ED34u,0xC2ED3Au,0xC2ED3Cu,0xC2ED40u,0xC2ED42u,0xC2ED44u,0xC2ED46u,0xC2ED48u,0xC2ED4Au,0xC2ED4Cu,0xC2ED50u,0xC2ED52u,0xC2ED56u,0xC2ED58u,0xC2ED5Au,0xC2ED5Cu,0xC2ED5Eu,0xC2ED60u,0xC2ED62u,0xC2ED64u,0xC2ED66u,0xC2ED68u,0xC2ED6Au,0xC32A44u,0xC32A46u,0xC32A4Cu,0xC32A52u,0xC32A56u,0xC32A5Cu,0xC32A5Eu,0xC32A62u,0xC32A64u,0xC32A68u,0xC32A6Au,0xC32A6Cu,0xC32A6Eu,0xC32A70u,0xC32A72u,0xC32A74u,0xC32A76u,0xC32A78u,0xC32A7Au,0xC32A7Cu,0xC32A7Eu,0xC32A80u,0xC32A82u,0xC32A84u,0xC32A88u,0xC32A8Au,0xC32A8Cu,0xC32A8Eu,0xC32A90u,0xC32A92u,0xC32A94u,0xC32AC8u,0xC32ACAu,0xC32ACCu,0xC32ACEu,0xC32AD0u,0xC32AD6u,0xC32AD8u,0xC32ADAu,0xC32ADEu,0xC32AE2u,0xC32AE4u,0xC32AE6u,0xC32AEAu,0xC32AECu,0xC32AEEu,0xC32AF0u,0xC32AF4u,0xC32AF6u,0xC32AFCu,0xC32AFEu,0xC32B00u,0xC32B02u,0xC32B06u,0xC32B0Cu,0xC32B0Eu,0xC32B10u,0xC32B12u,0xC32B14u,0xC32B18u,0xC32B1Cu,0xC32B1Eu,0xC32B20u,0xC32B22u,0xC32B24u,0xC32B26u,0xC32B2Au,0xC32B2Eu,0xC32B32u,0xC32B34u,0xC32B36u,0xC32B38u,0xC32B3Cu,0xC32B40u,0xC32B42u,0xC32B48u,0xC32B4Cu,0xC32B4Eu,0xC32B50u,0xC32B52u,0xC32B56u,0xC32B58u,0xC32B5Au,0xC32B5Eu,0xC32B66u,0xC32B68u,0xC32B6Cu,0xC32B6Eu,0xC32B72u,0xC32B76u,0xC32B78u,0xC32B7Cu,0xC32B84u,0xC32B86u,0xC32B8Au,0xC32B8Cu,0xC32B90u,0xC32B94u,0xC32B96u,0xC32B9Au,0xC32BA2u,0xC32BA4u,0xC32BA8u,0xC32BAAu,0xC32BAEu,0xC32BB2u,0xC32BB4u,0xC32BB8u,0xC32BC0u,0xC32BC2u,0xC32BC6u,0xC32BC8u,0xC32BCCu,0xC32BD0u,0xC33F70u,0xC33F72u,0xC33F76u,0xC33F7Au,0xC33F7Cu,0xC33F7Eu,0xC33F84u,0xC33F88u,0xC33F8Au,0xC33F8Cu,0xC33F90u,0xC33F94u,0xC33F96u,0xC33F98u,0xC33F9Eu,0xC33FA2u,0xC33FA4u,0xC33FA6u,0xC33FA8u,0xC33FAAu,0xC33FB0u,0xC33FB2u,0xC33FB4u,0xC33FBAu,0xC33FBCu,0xC33FC0u,0xC33FC2u,0xC33FC6u,0xC33FCAu,0xC33FCEu,0xC33FD0u,0xC33FD6u,0xC33FD8u,0xC33FDEu,0xC33FE0u,0xC33FE6u,0xC33FEAu,0xC33FECu,0xC33FF0u,0xC33FF2u,0xC33FF4u,0xC33FFAu,0xC33FFCu,0xC34002u,0xC34004u,0xC3400Au,0xC3400Cu,0xC34010u,0xC34016u,0xC34018u,0xC3401Cu,0xC3401Eu,0xC34024u,0xC3402Au,0xC3402Eu,0xC34030u,0xC34034u,0xC34036u,0xC34038u,0xC3403Eu,0xC34040u,0xC34046u,0xC34048u,0xC3404Eu,0xC34050u,0xC34054u,0xC3405Au,0xC3405Cu,0xC34060u,0xC34062u,0xC34064u};
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
#define PR_ORIGINAL_CHILDREN 1
#include "projection_readouts_fixture.h"
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
        fprintf(stderr,"projection-readouts oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=2;
        fa18_projection_readouts_fixture_begin("original");
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"projection-readouts oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        fa18_projection_readouts_fixture_begin("C");
        switch(selected_entry) {
        case 0xc2ec90u: glue_C2EC90(); break;
        case 0xc2ec94u: glue_C2EC94(); break;
        case 0xc2ec9cu: glue_C2EC9C(); break;
        case 0xc2eca4u: glue_C2ECA4(); break;
        case 0xc2eca8u: glue_C2ECA8(); break;
        case 0xc2ecd2u: glue_readout_x_limit(); break;
        case 0xc2ece4u: glue_readout_y_limit(); break;
        case 0xc32a44u: glue_C32A44(); break;
        case 0xc32ac8u: glue_C32AC8(); break;
        case 0xc33f70u: glue_C33F70(); break;
        case 0xc33f8au: glue_C33F8A(); break;
        case 0xc33fb4u: glue_C33FB4(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"projection-readouts oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"projection-readouts oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"projection-readouts oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("projection-readouts oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
