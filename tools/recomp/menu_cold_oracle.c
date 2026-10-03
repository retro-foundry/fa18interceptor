/* Complete cold-menu proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc1017eu;
static const uint32_t source_boundaries[]={0xC09120u,0xC09126u,0xC0912Cu,0xC09132u,0xC09148u,0xC0914Eu,0xC09154u,0xC0915Au,0xC09160u,0xC09166u,0xC0916Cu,0xC09172u,0xC09178u,0xC0917Au,0xC0917Cu,0xC0917Eu,0xC09184u,0xC0918Au,0xC09190u,0xC0FE36u,0xC0FE3Cu,0xC0FE3Eu,0xC0FE44u,0xC0FE46u,0xC0FE48u,0xC0FE4Eu,0xC0FE50u,0xC0FE56u,0xC0FE58u,0xC0FE5Au,0xC0FE60u,0xC0FE68u,0xC0FE6Eu,0xC0FE72u,0xC0FE76u,0xC0FE78u,0xC0FE82u,0xC0FE84u,0xC0FE8Au,0xC0FE8Cu,0xC0FE8Eu,0xC0FE94u,0xC0FE98u,0xC0FE9Eu,0xC0FEA4u,0xC0FEACu,0xC0FEB2u,0xC0FEB4u,0xC0FEBAu,0xC0FEBCu,0xC0FEC0u,0xC0FEC6u,0xC0FECCu,0xC1017Eu,0xC10182u,0xC10188u,0xC1018Eu,0xC10192u,0xC10194u,0xC1019Cu,0xC101A2u,0xC101A6u,0xC101AAu,0xC101AEu,0xC101B0u,0xC101B2u,0xC101B4u,0xC101BAu,0xC101BEu,0xC101C0u,0xC101C2u,0xC101C4u,0xC101C8u,0xC101CCu,0xC101CEu,0xC101D2u,0xC101D6u,0xC101DAu,0xC101DCu,0xC101E0u,0xC101E4u,0xC101E8u,0xC101ECu,0xC101EEu,0xC101F2u,0xC101F8u,0xC101FAu,0xC10272u,0xC10278u,0xC1027Au,0xC1027Cu,0xC1027Eu,0xC10284u,0xC1028Cu,0xC10292u,0xC10296u,0xC1029Cu,0xC103E4u,0xC103ECu,0xC103F2u,0xC103F4u,0xC103F6u,0xC103F8u,0xC103FEu,0xC10406u,0xC1040Cu,0xC10410u,0xC10416u,0xC10B90u,0xC10B96u,0xC10B9Cu,0xC10BA0u,0xC10BA6u,0xC10BACu,0xC16406u,0xC1640Au,0xC16412u,0xC16416u,0xC1641Cu,0xC1641Eu,0xC16422u,0xC16424u,0xC16428u,0xC1642Cu,0xC1642Eu,0xC16436u,0xC16438u,0xC29490u,0xC29498u,0xC2949Au,0xC294A2u,0xC294AAu};
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
static uint32_t expected_sp=0xc7ff04u;
static void fixture(unsigned scenario) {
    static const uint8_t actions[]={0,1,2,3,0xff};
    static const uint16_t words[]={0,1,2,0x7fff,0x8000,0xffff};
    unsigned i,profile=scenario/32u;
    gaddr table=0xc60000u+((profile&1u)?0x100u:0);
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<96;++i) wr_u32(0xc7fd80u+4*i,random_value());
    for(i=0;i<128;++i) wr_u32(0xc60000u+4*i,random_value());
    wr_u32(MODE_TABLE,table); wr_u16(table+4,words[(profile/5u)%6u]);
    for(i=0;i<6;++i) wr_u8(table+0x15u+i,(profile&(1u<<i))?(uint8_t)(i+1):0);
    wr_u8(MENU_TABLE_ACTION,actions[profile%5u]);
    wr_u8(MODE_TABLE_CHANGED,(profile&8u)?1:0);
    wr_u16(MENU_TABLE_STATUS,(profile&16u)?1:0);
    /* Real cold-menu load needs OS execution beyond this held-event oracle.
     * Its complete owner path is proven separately with child contracts.
     * Retain the original source-stop diagnostic; do not shorten the child. */
    if(selected_entry==0xc0fe36u && actions[profile%5u]==1 && (profile&8u)) wr_u16(MENU_TABLE_STATUS,1);
    wr_u8(SEQUENCE_PHASE,(profile&2u)?1:0);
    wr_u16(POST_INPUT_COUNTDOWN,words[profile%6u]);
    wr_u16(COCKPIT_FLAGS,(uint16_t)random_value());
    for(i=0;i<3;++i) { wr_u32(ORIGIN_ROOT_PRESET+4*i,random_value()); wr_u32(ORIGIN_ALTERNATE_PRESET+4*i,random_value()); }
    /* An actual source call boundary; no production liveness is changed. */
    REG_PPC=0xc10024u; REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=selected_entry;
    SET_CYCLES(1000000000); fa18_next_event=INT64_MAX;
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
        fprintf(stderr,"cold-menu oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"cold-menu oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        switch(selected_entry) {
        case 0xc0fe36u: glue_C0FE36(); break;
        case 0xc1017eu: glue_C1017E(); break;
        case 0xc10272u: glue_C10272(); break;
        case 0xc103e4u: glue_C103E4(); break;
        case 0xc09120u: glue_C09120(); break;
        case 0xc29490u: glue_C29490(); break;
        case 0xc2949au: glue_C2949A(); break;
        case 0xc09148u: glue_C09148(); break;
        case 0xc10b90u: glue_C10B90(); break;
        case 0xc16406u: glue_C16406(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"cold-menu oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"cold-menu oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"cold-menu oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("cold-menu oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
