/* Complete context-publication proof, including cold internal paths and real children. */
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
extern void context_publication_contract_reset(unsigned,int);
extern void context_publication_contract_enter(uint32_t,uint32_t);
extern void context_publication_contract_finish(void);
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
static uint32_t selected_entry=0xc1bee8u;
extern int glue_publish_command_event(void);

static unsigned char visited[0x1800];
static int source_owned(uint32_t pc) {
    return (pc>=0xc1b77cu && pc<0xc1c2b8u) || (pc>=0xc083a6u && pc<0xc083e2u) ||
           (pc>=0xc09dd0u && pc<0xc09e06u);
}
static unsigned source_slot(uint32_t pc) {
    if(pc>=0xc1b77cu) return pc-0xc1b77cu;
    return pc>=0xc09dd0u?0x1000u+pc-0xc09dd0u:0x1100u+pc-0xc083a6u;
}
static uint32_t source_pc(unsigned slot) {
    if(slot>=0x1100u) return 0xc083a6u+slot-0x1100u;
    if(slot>=0x1000u) return 0xc09dd0u+slot-0x1000u;
    return 0xc1b77cu+slot;
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
            if(pc==0xc1bf10u || pc==0xc1b9dau || pc==0xc1ba86u ||
               pc==0xc1bf68u || pc==0xc09deau)
                context_publication_contract_enter(REG_PC,rd_u32(REG_A[7]));
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
    static const uint8_t indices[]={0,1,9,10,0x7f,0x80,0xff,0xc2,0xb8,14};
    static const uint8_t counts[]={0,1,9,10,0x7f,0x80,0xff};
    static const uint16_t offsets[]={0,0x200,0x7fff,0x8000,0xffff,0x1e00};
    static const gaddr flags[]={KEY_TAKEN,KEY_COUNT,KEY_WRITE,KEY_TRANSLATED_WRITE,VIEW_SIDE,CONTEXT_SELECT,0xc61000u};
    static const uint16_t record_indices[]={0,1,2,4,15,16,63,64,0x7fff,0x8000,0xffff};
    unsigned i,profile=scenario/32u;
    uint16_t index,offset,chosen=offsets[profile%6u];
    gaddr record;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[0]=flags[profile%7u]; REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    SET_CYCLES(100000000);
    REG_D[0]=(REG_D[0]&0xffffff00u)|(scenario&0xffu);
    index=record_indices[(profile/2u)%11u]; REG_D[1]=(REG_D[1]&0xffff0000u)|index;
    wr_u8(KEY_TAKEN,(scenario&256u)?(uint8_t)(scenario|1u):0);
    wr_u8(KEY_COUNT,counts[(scenario/512u)%7u]);
    wr_u8(KEY_WRITE,indices[(scenario/32u)%10u]);
    wr_u8(KEY_TRANSLATED_WRITE,indices[(scenario/64u)%10u]);
    for(i=0;i<3;++i) wr_u8(KEY_STATE+i,(uint8_t)random_value());
    wr_u8(FIRE_STATE,(profile&1u)?0:0xff);
    wr_u8(CONTEXT_SELECT,(profile&2u)?1:0);
    wr_u8(CONTEXT_PUBLISH_RETURN_MODE,(uint8_t)random_value());
    wr_u8(REG_A[0],(profile&4u)?0:1);
    offset=(uint16_t)(index<<9); record=CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)offset;
    wr_u16(record+0x68,(uint16_t)random_value());
    wr_u8(record+0x62,(profile&4u)?0x30:0x20);
    wr_u16(LINE_LAST_ROW,(profile&8u)?0xa7:0x90);
    wr_u8(VIEW_MODE,(uint8_t)random_value());
    wr_u16(CHOSEN_RECORD,chosen);
    wr_u16(SELECTED_RECORD,(profile&1u)?chosen:0xffff);
    record=CONTROL_RECORDS+(gaddr)(int32_t)(int16_t)chosen;
    wr_u8(record+3,(profile&2u)?0x80:0);
    wr_u8(record+0x7c,(profile&4u)?0x80:0);
    wr_u32(WARNING_CAUSES,random_value());
    REG_A[7]=0xc7ff00u; wr_u32(REG_A[7],0xc70000u);
    m68k_set_reg(M68K_REG_SR,0x2700u|(scenario&31u)); REG_PC=selected_entry;
    fa18_recomp_abort=0; fa18_next_event=INT64_MAX;
    if(selected_entry==0xc083b6u) {
        /* Shared body entry has the source MOVEM save frame already present. */
        REG_A[7]-=12;
        wr_u32(REG_A[7],random_value()); wr_u32(REG_A[7]+4,random_value());
        wr_u32(REG_A[7]+8,random_value());
        REG_D[7]=(REG_D[7]&0xffffff00u)|(scenario&1u);
    }
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
        fprintf(stderr,"context-publication child-contract oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        context_publication_contract_reset(scenario,1);
        if(source_call(0xc70000u,expected_sp)!=FA18_RET) {
            fprintf(stderr,"context-publication child-contract oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        context_publication_contract_reset(scenario,0);
        switch(selected_entry) {
        case 0xc1b7a6u: glue_C1B7A6(); break;
        case 0xc1bee8u: glue_C1BEE8(); break;
        case 0xc1c214u: glue_C1C214(); break;
        case 0xc083a6u: glue_C083A6(); break;
        case 0xc083b6u: glue_selected_record_request_body(); break;
        case 0xc09dd0u: glue_C09DD0(); break;
        default: return 1;
        }
        context_publication_contract_finish();
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"context-publication child-contract oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"context-publication child-contract oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"context-publication child-contract oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    { unsigned i,count=0;
      for(i=0;i<sizeof visited;++i) if(visited[i]) ++count;
      printf("context-publication child-contract oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM; %u parent boundaries observed\n",selected_entry,cases,count);
      printf("visited:"); for(i=0;i<sizeof visited;++i) if(visited[i]) printf(" %06X",source_pc(i)); putchar('\n');
    }
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
