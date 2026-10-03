/* Complete menu-transition proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc0fcb4u;
static unsigned char visited[0x900];
static int source_owned(uint32_t pc) {
    return (pc>=0xc0fcb4u && pc<0xc1017eu) || (pc>=0xc17c2au && pc<0xc17c62u) ||
           (pc>=0xc24e8au && pc<0xc24fa4u);
}
static unsigned source_slot(uint32_t pc) {
    if(pc>=0xc24e8au) return 0x600u+pc-0xc24e8au;
    if(pc>=0xc17c2au) return 0x500u+pc-0xc17c2au;
    return pc-0xc0fcb4u;
}
static uint32_t source_pc(unsigned slot) {
    if(slot>=0x600u) return 0xc24e8au+slot-0x600u;
    if(slot>=0x500u) return 0xc17c2au+slot-0x500u;
    return 0xc0fcb4u+slot;
}
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
    static const uint8_t modes[]={0,1,2,3,4,5,6,7,8,9,125,127,128,254,255};
    static const uint16_t delays[]={0,1,0x7fff,0x8000,0xffff};
    static const uint32_t times[]={0,1,3599,3600,0x7fff*3600u,0x8000*3600u,
                                  65535u*3600u,65536u*3600u,0x7fffffffu,0x80000000u,0xffffffffu};
    unsigned i,profile=scenario/32u;
    gaddr record=0xc60000u+(profile%4u)*0x100u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    wr_u8(MODE_SELECT,modes[profile%15u]);
    wr_u8(SEQUENCE_PHASE,(profile/15u)%3u?0:0xff);
    wr_u16(0xc45772u,(profile&1u)?6:7);
    wr_u8(SEQUENCE_FLAG,(profile&2u)?0:1);
    wr_u8(0xc45792u,(profile&4u)?0:1);
    wr_u8(SOUND_FLAGS,(profile&8u)?0x90:0);
    wr_u8(0xc45b5au,(profile&1u)?4:0);
    wr_u16(POST_INPUT_COUNTDOWN,delays[(profile/15u)%5u]);
    wr_u8(SCENE_POSE_ENTRY,(profile&1u)?3:2);
    wr_u16(SECONDARY_REQUEST_FLAGS,(uint16_t)random_value());
    wr_u32(MODE_TABLE,record); wr_u32(record+8,times[profile%11u]);
    for(i=0;i<10;++i) wr_u16(record+0x36+2*i,(uint16_t)random_value());
    REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    wr_u32(REG_A[7]+4,random_value());
    if(selected_entry==0xc0ffe2u || selected_entry==0xc1000au) {
        /* Table-arm entry into the parent's already active LINK -2 frame. */
        REG_A[6]=REG_A[7]-4; REG_A[7]=REG_A[6]-2;
        wr_u32(REG_A[6],0xc62080u); wr_u32(REG_A[6]+4,0xc70000u);
        wr_u16(REG_A[6]-2,(uint16_t)random_value());
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
        fprintf(stderr,"menu-transition oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"menu-transition oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        switch(selected_entry) {
        case 0xc0fcb4u: glue_C0FCB4(); break;
        case 0xc0feceu: glue_C0FECE(); break;
        case 0xc0ffe2u: glue_C0FFE2(); break;
        case 0xc1000au: glue_C1000A(); break;
        case 0xc17c2au: glue_C17C2A(); break;
        case 0xc24e8au: glue_C24E8A(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"menu-transition oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"menu-transition oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"menu-transition oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("menu-transition oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
