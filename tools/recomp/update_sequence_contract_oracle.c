/* Complete parent CPU/RAM and child-boundary proof under explicitly controlled
 * child contracts. Actual original-child bodies are independently checked by
 * update_sequence_oracle.c and recorded whole-call/live gates. */
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
static uint32_t selected_entry=0xc0f3c4u;
static unsigned char visited[0x600];
extern void update_contract_enter(uint32_t entry,uint32_t ret);
extern void update_contract_reset(unsigned scenario,int source);
extern void update_contract_finish(void);
static uint32_t source_end(void) {
    return selected_entry==0xc0efd4u?0xc0f3c4u:
           selected_entry==0xc0f3c4u?0xc0f4a6u:0xc0d74au;
}
static int source_call(uint32_t ret,uint32_t sp) {
    unsigned dispatch;
    uint32_t child_ret=0,child_sp=0;
    for(dispatch=0;dispatch<1000000;++dispatch) {
        int lo=0,hi=fa18_recomp_entry_count,result;
        if(REG_PC==ret && REG_A[7]==sp) return FA18_RET;
        if(REG_PC>=selected_entry && REG_PC<source_end()) {
            uint32_t pc=REG_PC;
            uint16_t opcode=m68k_read_memory_16(pc);
            visited[pc-selected_entry]=1;
            child_ret=0;
            REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
            m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
            continue;
        }
        update_contract_enter(REG_PC,rd_u32(REG_A[7]));

    }
    return FA18_EXIT_INTERP;
}
static uint32_t expected_sp=0xc7ff04u;
static void fixture(unsigned scenario) {
    scenario*=0x9e3779b9u;
    static const uint8_t modes[]={0,1,2,3,4,0x7f,0x80,0xff};
    static const uint8_t bytes[]={0,1,0x7f,0x80,0xff};
    static const uint32_t positions[]={0xf7ffffffu,0xf8000000u,0xf8000001u,
        0xffff7fffu,0xffff8000u,0xffff8001u,0,0x7fffffffu,0x80000000u};
    static const uint16_t indices[]={0,1,0x7fffu,0x8000u,0xffffu};
    unsigned i;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    wr_u8(RECORDER_MODE,modes[(scenario/32u)%8u]);
    wr_u16(RECORD_WORD_A,(scenario&1024u)?1:0); wr_u16(RECORD_WORD_B,(scenario&2048u)?2:0);
    wr_u16(PENDING_COMMAND_WORD_A,(uint16_t)random_value());
    wr_u16(PENDING_COMMAND_WORD_B,(uint16_t)random_value());
    wr_u16(INPUT_STATE_WORD,(scenario&32u)?(uint16_t)random_value():0);
    wr_u16(0xc08182u,0); /* no latched key in the sealed empty queue */
    wr_u16(UPDATE_DISPLAY_FLAGS,(uint16_t)((scenario&32u)?0x2000u:0));
    if(selected_entry==0xc0efd4u) {
        wr_u16(UPDATE_DISPLAY_FLAGS,(scenario&65536u)?0x2000u:0);
        wr_u8(UPDATE_ACTIVE,bytes[(scenario/64u)%5u]);
        wr_u16(UPDATE_TICK,(uint16_t)((scenario/32u)%32u));
        wr_u8(UPDATE_ACTIVITY,bytes[(scenario/128u)%5u]);
        wr_u8(UPDATE_HUD_MODE,(scenario&64u)?1:0);
        wr_u8(ORIGIN_ENABLE,(scenario&4096u)?1:0);
        wr_u8(ORIGIN_GATE_MODE,(scenario&128u)?1:0);
        wr_u8(ORIGIN_DETAIL_MODE,(scenario&256u)?1:0);
        wr_u8(ORIGIN_GATE_A,(scenario&512u)?1:0);
        wr_u8(UPDATE_MAP_OVERRIDE,(scenario&1024u)?1:0);
        wr_u8(UPDATE_TAIL_CONDITION,(scenario&2048u)?1:0);
        wr_u32(POSITION_BIAS,positions[(scenario/32u)%9u]);
        uint16_t index=indices[(scenario/1024u)%5u];
        gaddr record=CONTROL_RECORDS+((uint32_t)(int32_t)(int16_t)index<<9);
        wr_u16(TARGET_RECORD,index);
        wr_u8(record+0x62u,(scenario&8192u)?0x30:0x11);
        wr_u8(UPDATE_MAP_FLAGS,(scenario&16384u)?1:0);
        wr_u8(MODE_SELECT,(scenario&32768u)?2:1);
    }
    if(selected_entry==0xc0f3c4u && (scenario&256u)) {
        wr_u16(0xc08182u,1); wr_u16(0xc1abc8u,(scenario&512u)?0xa5:0x25);
    }
    REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    expected_sp=0xc7ff04u;
    if(selected_entry==0xc0d730u && (scenario&32u)) {
        /* The alternate original child returns through its owner's LINK frame. */
        REG_A[6]=0xc7ff10u; wr_u32(REG_A[6],0xc62080u);
        wr_u32(REG_A[6]+4,0xc70000u); expected_sp=REG_A[6]+8;
    }
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
        fprintf(stderr,"update child-contract oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1; update_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"update child-contract oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu); update_contract_reset(scenario,0);
        switch(selected_entry) {
        case 0xc0efd4u: glue_C0EFD4(); break;
        case 0xc0f3c4u: glue_C0F3C4(); break;
        case 0xc0d730u: glue_C0D730(); break;
        default: return 1;
        }
        update_contract_finish(); fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"update child-contract oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"update child-contract oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"update child-contract oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("update child-contract oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",selected_entry+i); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
