/* Complete input-events proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc16eaeu;

static unsigned char visited[0x600];
extern void input_contract_enter(uint32_t entry,uint32_t ret);
extern void input_contract_reset(unsigned scenario,int source);
extern void input_contract_finish(void);
static uint32_t source_end(void) {
    switch(selected_entry) {
    case 0xc16eae: return 0xc16f1c;
    case 0xc16bf2: return 0xc16c3a;
    case 0xc16c56: return 0xc16cd8;
    case 0xc13d34: return 0xc13d84;
    default: abort();
    }
}
static int source_call(uint32_t ret,uint32_t sp) {
    unsigned dispatch;
    for(dispatch=0;dispatch<1000000;++dispatch) {
        if(REG_PC==ret && REG_A[7]==sp) return FA18_RET;
        if(REG_PC>=selected_entry && REG_PC<source_end()) {
            uint32_t pc=REG_PC;
            uint16_t opcode=m68k_read_memory_16(pc);
            visited[pc-selected_entry]=1;
            REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
            m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
            continue;
        }
        input_contract_enter(REG_PC,rd_u32(REG_A[7]));
    }
    return FA18_EXIT_INTERP;
}
static uint32_t expected_sp=0xc7ff04u;
static void fixture(unsigned scenario) {
    static const uint16_t codes[]={0,0x68,0xe8,0x67,0xffff,0x8000};
    static const uint8_t levels[]={0,0x77,0x78,0x79,0x7f,0x80,0xff};
    unsigned i;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    wr_u32(EXTERNAL_INPUT_DESCRIPTOR,0xc61000u);
    wr_u32(KEYBOARD_INPUT_DESCRIPTOR,0xc61200u);
    wr_u16(0xc61006u,codes[(scenario/64u)%6u]);
    wr_u16(0xc61206u,(uint16_t)(scenario/64u));
    wr_u16(RAW_KEY_LATCH,(scenario&32u)?(uint16_t)(scenario|1u):0);
    wr_u16(RAW_KEY_WORD,(uint16_t)random_value());
    wr_u32(MATRIX_SIDE_METRIC,(scenario&32u)?random_value():0);
    wr_u8(PLAYER_READY,(scenario&64u)?(uint8_t)(scenario|1u):0);
    wr_u16(INPUT_STATE_MIRROR,(uint16_t)(scenario/128u));
    wr_u16(BUTTON_COMMAND_FLAGS,(scenario&512u)?8:0);
    wr_u8(BUTTON_COMMAND_LEVEL,levels[(scenario/1024u)%7u]);
    wr_u8(FUNCTION_KEY_LEVEL,(uint8_t)random_value());
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
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,scenario;
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !reference || !cpu || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) {
        fprintf(stderr,"input-events child-contract oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1; input_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"input-events child-contract oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); input_contract_reset(scenario,0);
        switch(selected_entry) {
        case 0xc16eaeu: glue_C16EAE(); break;
        case 0xc16bf2u: glue_C16BF2(); break;
        case 0xc16c56u: glue_C16C56(); break;
        case 0xc13d34u: glue_C13D34(); break;
        default: return 1;
        }
        input_contract_finish(); fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"input-events child-contract oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"input-events child-contract oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"input-events child-contract oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("input-events child-contract oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",selected_entry+i); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
