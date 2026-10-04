/* Cold segments: execute original bytes against shared production helpers. */
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
extern void glue_render_leaf_extent_segment(uint32_t);
extern void glue_render_leaf_first_plane_wait(void);

extern int fa18_write_log_active;
extern void fa18_structural_reset_write_log(void);
extern unsigned fa18_hud_hardware_count(void);
extern void fa18_hud_hardware_begin(void);
extern void fa18_render_leaf_controlled_begin(uint32_t entry,unsigned scenario);
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
static uint32_t selected_entry=0xc30260u;
extern void render_leaf_helpers_contract_reset(unsigned scenario,int source);
extern void render_leaf_helpers_contract_enter(uint32_t entry,uint32_t ret);
extern void render_leaf_helpers_contract_finish(void);
static const uint32_t source_boundaries[]={0xc30260u,0xc30262u,0xc30264u,0xc30266u,0xc30274u,0xc30276u,0xc30278u,0xc3027au,0xc30290u,0xc30292u,0xc30294u,0xc30296u,0xc302deu,0xc302e0u,0xc302e2u,0xc302e4u,0xc2fbbau,0xc2fbc0u,0xc2fbc2u,0xc2fbc4u,0xc2fbc6u};
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
                render_leaf_helpers_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
#include "render_leaf_helpers_fixture.h"
static uint32_t segment_end(void) {
 switch(selected_entry) {case 0xc30260u:return 0xc30268u;case 0xc30274u:return 0xc3027cu;case 0xc30290u:return 0xc30298u;case 0xc302deu:return 0xc302e6u;case 0xc2fbbau:return 0xc2fbc8u;default:return 0;}
}
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
        fprintf(stderr,"render leaf helper oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        fa18_structural_reset_write_log();
        memcpy(m,base,sizeof *m); fixture(scenario);
        expected_sp=REG_A[7];REG_A[0]=0xdff000u;
        for(i=0;i<8;++i)REG_D[i]=random_value();
        REG_D[0]=(REG_D[0]&0xffff0000u)|(uint16_t)((scenario>>5)*97u);
        REG_D[1]=(REG_D[1]&0xffff0000u)|(uint16_t)((scenario>>5)*251u);
        REG_D[2]=(REG_D[2]&0xffff0000u)|(uint16_t)((scenario>>5)*65521u);
        REG_D[3]=(REG_D[3]&0xffff0000u)|(uint16_t)((scenario>>5)*491u);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_render_leaf_controlled_begin(selected_entry==0xc2fbbau?0xc2fa78u:selected_entry,scenario); fa18_write_log_active=2;
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u source\n",scenario);
        render_leaf_helpers_contract_reset(scenario,1);
        if(source_call(segment_end(),expected_sp)!=FA18_RET) {
            fprintf(stderr,"render leaf helper oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        fa18_hud_hardware_reference(); hardware_writes+=fa18_hud_hardware_count();
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); fa18_render_leaf_controlled_begin(selected_entry==0xc2fbbau?0xc2fa78u:selected_entry,scenario);
        render_leaf_helpers_contract_reset(scenario,0);
        if(getenv("FA18_ORACLE_TRACE")) fprintf(stderr,"case %u C\n",scenario);
        if(selected_entry==0xc2fbbau)glue_render_leaf_first_plane_wait();
        else glue_render_leaf_extent_segment(selected_entry);
        render_leaf_helpers_contract_finish();
        fa18_write_log_active=0;
        if(!fa18_hud_hardware_check()) { fprintf(stderr,"case %u ordered Custom writes/terminal hardware differ\n",scenario); return 1; }
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"render leaf helper oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=segment_end() || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"render leaf helper oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"render leaf helper oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("render leaf helper oracle %06X: %u %s matched all registers, PC, full SR and all RAM; %u source boundaries observed\n",selected_entry,cases,"production segment calls",count);
      printf("hardware: %u ordered Custom writes validated; terminal registers, effective flags, blit counters and data latches matched\n",hardware_writes);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
