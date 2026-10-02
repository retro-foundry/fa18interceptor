/* Complete view-command proof, including cold internal paths and real children. */
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
#include "glue_view_commands.h"
#include "glue_command_selection.h"

extern int fa18_write_log_active;
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
static uint32_t selected_entry=0xc1b8f8u;
static enum CommandAction selected_action;
extern int glue_publish_command_event(void);

static unsigned char visited[0x1800];
static const uint32_t action_base=0xc1b126u;
static int source_parent_pc(uint32_t pc) {
    return (pc>=0xc1b77c && pc<0xc1bb7a) || (pc>=0xc1c23c && pc<0xc1c2b8);
}
static int source_call(uint32_t ret,uint32_t sp) {
    unsigned dispatch;
    uint32_t child_ret=0,child_sp=0;
    for(dispatch=0;dispatch<1000000;++dispatch) {
        int lo=0,hi=fa18_recomp_entry_count,result;
        if(REG_PC==ret && REG_A[7]==sp) return FA18_RET;
        if(source_parent_pc(REG_PC)) {
            uint32_t pc=REG_PC;
            uint16_t opcode=m68k_read_memory_16(pc);
            visited[pc-action_base]=1;
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
    static const uint8_t modes[]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,0x7f,0x80,0xff};
    static const uint16_t scales[]={0,0x1f,0x20,0x21,0x40,0x7f,0x80,0x81,0x7fff,0x8000,0xffff};
    static const uint32_t middles[]={0,0x01000000,0x02000000,0x07000000,0x08000000,0x09000000,0x7fffffff,0x80000000,0xff000000};
    unsigned i,profile=scenario/32u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_D[0]=(REG_D[0]&0xffffff00u)|(uint8_t)(profile*37u);
    REG_D[5]=(REG_D[5]&0xffffff00u)|((profile&1u)?1:0);
    REG_D[6]=(REG_D[6]&0xffffff00u)|((profile&1u)?(uint8_t)(profile|1u):0);
    wr_u8(ORIGIN_ENABLE,modes[profile%17u]);
    wr_u8(ORIGIN_DETAIL_MODE,(profile&2u)?1:0);
    wr_u8(ORIGIN_GATE_B,(profile&8u)?1:0);
    wr_u8(ORIGIN_GATE_MODE,(profile&4u)?1:0);
    wr_u32(SELECTOR_ORIGIN_MIDDLE,middles[profile%9u]);
    wr_u8(VIEW_MODE,modes[profile%17u]);
    wr_u16(VIEW_RECORD,(profile&4u)?0x200:0);
    wr_u8(CONTROL_RECORDS+((profile&4u)?0x200:0)+0x62,(profile&8u)?0x30:0x20);
    wr_u16(LINE_LAST_ROW,profile%3u==0?0xb3:profile%3u==1?0xa7:0x90);
    wr_u8(FIRE_STATE,(profile&16u)?1:0);
    wr_u16(ZOOM_SCALE,scales[profile%11u]);
    wr_u8(ZOOM_FLAGS,(profile&2u)?0x80:(profile&4u)?0x01:0);
    wr_u8(KEY_TAKEN,(profile&4u)?2:0);
    wr_u8(KEY_COUNT,profile%3u==0?10:0);
    wr_u8(KEY_WRITE,(profile&2u)?10:0);
    wr_u8(KEY_TRANSLATED_WRITE,(uint8_t)(profile%10u));
    for(i=0;i<3;++i) wr_u8(KEY_STATE+i,(uint8_t)random_value());
    REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=selected_entry;
    fa18_recomp_abort=0; fa18_next_event=INT64_MAX; SET_CYCLES(100000000);
}

int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *reference=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    void *cpu=malloc(m68k_context_size()); char error[256];
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):2048,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    for(selected_action=COMMAND_PENDING_EMPTY; selected_action<=COMMAND_INDEXED; ++selected_action)
        if(glue_command_action_pc(selected_action)==selected_entry) break;
    if(selected_action>COMMAND_INDEXED || !is_view_command(selected_action)) return 1;
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"view-command oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"view-command oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        { CommandRequest request={selected_action,REG_D[0],(uint8_t)REG_D[5],(uint8_t)REG_D[6],0,0};
          uint32_t event=glue_execute_view_command(&request);
          if(event!=REG_D[0]) {
              fprintf(stderr,"view-command oracle: case %u domain event %08X CPU event %08X\n",scenario,event,REG_D[0]); return 1;
          }
          glue_publish_command_event();
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"view-command oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"view-command oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"view-command oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("view-command oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",action_base+i); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
