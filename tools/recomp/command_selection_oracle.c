/* Complete command-selection proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc1ad74u;

static unsigned char visited[0x600];
static int in_selection(uint32_t pc) {
    return selected_entry==0xc1ac28u ? pc>=0xc1ac28u && pc<0xc1ad70u
                                  : pc>=0xc1ad74u && pc<0xc1b126u;
}
static int source_call(uint32_t ret,uint32_t sp) {
    unsigned count;
    (void)ret; (void)sp;
    for(count=0;count<1000;++count) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if(!in_selection(pc)) return FA18_RET;
        visited[pc-0xc1ac28u]=1;
        opcode=m68k_read_memory_16(pc);
        REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return FA18_EXIT_INTERP;
}
static uint32_t expected_sp=0xc7ff04u;
static void fixture(unsigned scenario) {
    unsigned i,profile=(scenario>>8)&31u;
    uint8_t detail=0,counter=1,mode=1,recorder=0,modifier=0;
    uint16_t first,second;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    wr_u8(COMMAND_RETURN_STATE,0); wr_u8(CONTEXT_GATE,0);
    wr_u8(COMMAND_ENABLE_GATE,0); wr_u8(COMMAND_MODE_GATE,1);
    wr_u8(COMMAND_BLOCK_FLAGS,0); wr_u8(MESSAGE_STATE_C,1);
    switch(profile) {
    case 1: modifier=1; break;
    case 2: counter=0; break;
    case 3: counter=0xff; break;
    case 4: counter=0x80; break;
    case 5: counter=0x81; break;
    case 6: detail=6; break;
    case 7: detail=3; break;
    case 8: detail=0x80; break;
    case 9: wr_u8(COMMAND_RETURN_STATE,1); break;
    case 10: wr_u8(CONTEXT_GATE,1); break;
    case 11: wr_u8(CONTEXT_GATE,2); break;
    case 12: wr_u8(COMMAND_ENABLE_GATE,1); break;
    case 13: wr_u8(COMMAND_MODE_GATE,0); break;
    case 14: mode=0; break;
    case 15: mode=2; break;
    case 16: recorder=0x80; break;
    case 17: recorder=3; break;
    case 18: recorder=1; break;
    case 19: recorder=2; break;
    case 20: recorder=5; break;
    case 21: wr_u8(COMMAND_BLOCK_FLAGS,0x0f); break;
    case 22: detail=1; break;
    case 23: detail=5; break;
    case 24: recorder=4; break;
    case 25: wr_u8(MESSAGE_STATE_C,2); break;
    case 26: wr_u8(COMMAND_ENABLE_GATE,1); break;
    case 27: wr_u8(COMMAND_RETURN_STATE,0x80); break;
    case 28: recorder=0xff; break;
    case 29: recorder=0x7f; break;
    case 30: modifier=0xff; break;
    case 31: counter=0x7f; break;
    }
    wr_u8(COMMAND_EVENT_COUNTER,counter); wr_u8(MODE_SELECT,mode);
    wr_u8(ORIGIN_DETAIL_MODE,detail); wr_u8(ORIGIN_ENABLE,(uint8_t)random_value());
    wr_u8(RECORDER_MODE,recorder); wr_u8(KEY_STATE,modifier);
    wr_u8(KEY_STATE+1,(uint8_t)random_value()); wr_u8(KEY_STATE+2,(uint8_t)random_value());
    if(selected_entry==0xc1ac28u) {
        unsigned word_profile=(scenario>>5)&63u,bit=word_profile&15u;
        uint16_t mask=(uint16_t)(1u<<(bit<8?bit+8:bit-8));
        first=word_profile<16?mask:0;
        second=word_profile>=16 && word_profile<32?mask:(uint16_t)random_value();
        if(word_profile>=32 && word_profile<36) { first=second=0; recorder=(uint8_t)(word_profile-32); }
        if(word_profile>=36) {
            first=(scenario&0x800u)?(uint16_t)random_value():0;
            recorder=(scenario&0x1000u)?1:0;
        }
        wr_u16(RECORD_WORD_A,first); wr_u16(RECORD_WORD_B,second);
        wr_u8(RECORDER_MODE,recorder);
    }
    REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    wr_u32(REG_A[7]+4,(random_value()&0xffffff00u)|(scenario&0xffu));
    m68k_set_reg(M68K_REG_SR,0x2700u|((scenario>>8)&31u)); REG_PC=selected_entry;
    fa18_recomp_abort=0; fa18_next_event=INT64_MAX; SET_CYCLES(100000000);
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
        fprintf(stderr,"command-selection oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr,selected_pc; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"command-selection oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR); selected_pc=REG_PC;
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        if(selected_entry==0xc1ac28u) glue_select_pending_command();
        else if(selected_entry==0xc1ad74u) glue_select_keyboard_command();
        else return 1;
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"command-selection oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=selected_pc || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"command-selection oracle: case %u action PC source %06X C %06X; SR source %04X C %04X\n",
                    scenario,selected_pc,REG_PC,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"command-selection oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("command-selection oracle %06X: %u selection prefixes matched all registers, PC, full SR and all RAM; %u selection boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",0xc1ac28u+i); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
