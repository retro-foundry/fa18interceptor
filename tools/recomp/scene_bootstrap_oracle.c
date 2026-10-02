/* Complete C0F5F8 proof against original bytes, with real installed children. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "m68kcpu.h"
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
static int source_call(uint32_t ret,uint32_t sp) {
    unsigned dispatch;
    for(dispatch=0;dispatch<10000;++dispatch) {
        int lo=0,hi=fa18_recomp_entry_count,result;
        if(REG_PC==ret && REG_A[7]==sp) return FA18_RET;
        while(lo<hi) {
            int mid=lo+(hi-lo)/2;
            if(fa18_recomp_entries[mid].pc<REG_PC) lo=mid+1; else hi=mid;
        }
        if(lo==fa18_recomp_entry_count || fa18_recomp_entries[lo].pc!=REG_PC)
            return fa18_recomp_resume(ret,sp);
        result=fa18_recomp_functions[fa18_recomp_entries[lo].function].fn((int)fa18_recomp_entries[lo].label);
        if(result==FA18_EXIT_INTERP) return result;
    }
    return FA18_EXIT_INTERP;
}
static uint32_t selected_entry=0xc1c860u;
static void fixture(unsigned scenario) {
    static const uint32_t guards[]={0,0xf7ffffffu,0xf8000000u,0xf8000001u,0x80000000u,0x7fffffffu,0xffffffffu};
    static const uint8_t modes[]={0,1,2,3,9,127};
    static const uint8_t recorder_modes[]={0,1,2,3,4,127,128,255};
    unsigned i,j;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=scenario&0x100u?0xc7ff1cu:0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    wr_u32(POSITION_BIAS,guards[(scenario/16u)%7u]);
    wr_u8(UPDATE_MASK,(uint8_t)scenario);
    wr_u8(CONTEXT_SELECT,scenario&0x10u?1:0);
    wr_u32(0xc45c3eu,random_value()); wr_u32(0xc45c46u,random_value());
    wr_u16(VIEW_RECORD,(uint16_t)((scenario%16u)*512u));
    for(i=0;i<16;++i) {
        gaddr record=CONTROL_RECORDS+i*512u;
        wr_u16(record+6,(uint16_t)(((scenario+i)%32u)*4u+(scenario&3u)));
        wr_u16(record+8,(uint16_t)(((scenario+i*3u)%32u)*4u+((scenario>>2)&3u)));
        for(j=0;j<9;++j) wr_u16(record+0xa4u+j*2u,(uint16_t)random_value());
    }
    wr_u16(STREAM_SKIP,(uint16_t)(scenario>>5));
    wr_u8(VIEW_MODE,scenario&0x40u?1:0); wr_u8(FIXED_READOUTS,scenario&0x80u?1:0);
    wr_u8(0xc45859u,(uint8_t)(scenario>>7));
    wr_u8(MODE_SELECT,modes[(scenario/4u)%6u]);
    wr_u8(RECORDER_MODE,recorder_modes[(scenario/4u)%8u]);
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
        fprintf(stderr,"scene-bootstrap oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(source_call(0xc70000u,0xc7ff04u)!=FA18_RET) {
            fprintf(stderr,"scene-bootstrap oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        switch(selected_entry) {
        case 0xc1c860u: glue_C1C860(); break;
        case 0xc08f26u: glue_C08F26(); break;
        case 0xc0f920u: glue_C0F920(); break;
        case 0xc0f992u: glue_C0F992(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"scene-bootstrap oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"scene-bootstrap oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"scene-bootstrap oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    printf("scene-bootstrap oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM\n",selected_entry,cases);
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
