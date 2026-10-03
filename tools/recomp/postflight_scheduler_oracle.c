/* Complete postflight-scheduler proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc09e06u;

static unsigned char visited[0xb00];
static int source_call(uint32_t ret,uint32_t sp) {
    unsigned dispatch;
    uint32_t child_ret=0,child_sp=0;
    for(dispatch=0;dispatch<1000000;++dispatch) {
        int lo=0,hi=fa18_recomp_entry_count,result;
        if(REG_PC==ret && REG_A[7]==sp) return FA18_RET;
        if(REG_PC>=0xc09e06u && REG_PC<0xc0a42cu) {
            uint32_t pc=REG_PC;
            uint16_t opcode=m68k_read_memory_16(pc);
            visited[pc-0xc09e06u]=1;
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
    static const uint8_t phases[]={0,1,2,0x7f,0x80,0xfc,0xfd,0xfe,0xff};
    static const uint8_t counts[]={0,1,2,3,4,5,0x7f,0x80,0xff};
    static const uint8_t modes[]={0,2,3,4,5,6,7,8,9,125,126,0x80,0xff};
    static const uint16_t gates[]={0,1,0x6f,0x70,0x7fff,0x8000,0xffff};
    static const uint32_t distances[]={0,0xffff,0x10000,0x10001,0x18000,0x24000,0x30000,0x30001,0x7fffffff,0x80000000,0xffffffff};
    unsigned i,j;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[1]=CONTROL_RECORDS+0x800u; REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    wr_u8(PLAYER_PHASE,phases[(scenario/32u)%9]);
    wr_u8(SEQUENCE_PHASE,(scenario&64u)?3:(uint8_t)random_value());
    wr_u8(PLAYER_FLAGS_F,(uint8_t)random_value());
    wr_u8(POST_INPUT_EVENT,counts[(scenario/288u)%9]);
    wr_u8(0xc458a6u,modes[(scenario/32u)%13]);
    wr_u8(0xc458cdu,(scenario&32u)?0x40:0);
    wr_u8(0xc45790u,(scenario&64u)?1:0);
    wr_u8(0xc458a7u,(uint8_t)(scenario/1024u));
    wr_u8(CONTEXT_SELECT,(scenario&512u)?1:0);
    wr_u8(0xc457beu,(uint8_t)random_value());
    wr_u8(VIEW_MODE,(uint8_t)random_value());
    wr_u8(SCENE_DISPATCH_ADMITTED,(uint8_t)random_value());
    wr_u8(SCENE_DISPATCH_AUX,(uint8_t)random_value());
    wr_u16(SCENE_DISPATCH_GATE,gates[(scenario/64u)%7]);
    wr_u16(0xc458c2u,(random_value()&1u)?0x800:0);
    wr_u8(0xc45848u,(scenario&32u)?3:1);
    for(i=0;i<16;++i) {
        gaddr r=CONTROL_RECORDS+i*0x200u;
        wr_u8(r,(uint8_t)random_value()); wr_u8(r+1,(uint8_t)random_value());
        wr_u8(r+3,(uint8_t)random_value()); wr_u8(r+4,(uint8_t)random_value());
        wr_u16(r+6,gates[(scenario/128u+i)%7]);
        wr_u16(r+12,(scenario&128u)?0:1);
        wr_u8(r+0x20,(uint8_t)random_value()); wr_u8(r+0x21,(uint8_t)random_value());
        wr_u8(r+0x3a,(uint8_t)(i%8));
        wr_u16(r+0x4c,gates[(scenario/64u+i)%7]);
        wr_u16(r+0x6c,gates[(scenario/256u+i)%7]);
        wr_u16(r+0x6e,(scenario&128u)?0:1);
        for(j=0;j<3;++j) wr_u32(r+0x14+4*j,distances[(scenario/32u+i+j)%11]);
    }
    if(scenario&256u) {
        /* Decouple active-mode fixtures from phase/countdown bit patterns. */
        wr_u8(PLAYER_PHASE,0);
        wr_u16(CONTROL_RECORDS+2,(scenario&512u)?0xc080:0);
        wr_u16(CONTROL_RECORDS+0x6e,(scenario&128u)?0:1);
        if(selected_entry==0xc0a002u && (scenario&1024u)) {
            gaddr first=CONTROL_RECORDS+0x800,second=CONTROL_RECORDS+0xc00;
            wr_u8(first+1,0x48); wr_u8(second+1,0x48); wr_u16(first+6,0x6f);
            for(i=0;i<3;++i) {
                uint32_t position=random_value();
                wr_u32(CONTROL_RECORDS+0x14+4*i,position);
                wr_u32(first+0x14+4*i,position+(scenario&2048u?0x30001u:0));
            }
            wr_u16(SCENE_DISPATCH_GATE,gates[(scenario/32u)%7]);
        }
        if(selected_entry==0xc0a15cu && (scenario&1024u)) {
            gaddr target=CONTROL_RECORDS+0x800u;
            uint32_t x=random_value(),z=random_value();
            wr_u16(SCHEDULE_TARGET,0x800);
            wr_u8(target,0x80); wr_u16(target+0x4c,0);
            wr_u32(CONTROL_RECORDS+0x1614,x); wr_u32(CONTROL_RECORDS+0x161c,z);
            wr_u32(target+0x14,x+distances[(scenario/32u)%11]);
            wr_u32(target+0x1c,z+distances[(scenario/352u)%11]);
        }
    }
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
        fprintf(stderr,"postflight-scheduler oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"postflight-scheduler oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        switch(selected_entry) {
        case 0xc09e06u: glue_C09E06(); break;
        case 0xc09e98u: glue_C09E98(); break;
        case 0xc09ec4u: glue_C09EC4(); break;
        case 0xc0a002u: glue_C0A002(); break;
        case 0xc0a12eu: glue_C0A12E(); break;
        case 0xc0a15cu: glue_C0A15C(); break;
        case 0xc0a1e0u: glue_C0A1E0(); break;
        case 0xc0a2f0u: glue_C0A2F0(); break;
        case 0xc0a334u: glue_C0A334(); break;
        case 0xc0a364u: glue_C0A364(); break;
        case 0xc0a3eau: glue_C0A3EA(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"postflight-scheduler oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"postflight-scheduler oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"postflight-scheduler oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("postflight-scheduler oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",0xc09e06u+i); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
