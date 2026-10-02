/* Complete C22C80/C1C63E proof against original bytes and real children. */
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
static uint32_t selected_entry=0xc22c80u;
static void fixture(unsigned scenario) {
    static const uint32_t signed_values[]={0,0xffffffffu,0x80000000u,0x7fffffffu,
        0x9fff,0xa000,0xa001,0xffff6001u,0xffff6000u,0xffff5fffu,0xf8000000u};
    static const uint16_t words[]={0,1,0x7fff,0x8000,0xffff};
    unsigned i;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    for(i=0;i<16;++i) {
        wr_u16(WORKSPACE_RECORDS+4+i*32u,(uint16_t)random_value());
        /* Exercise each bit cleared by the parent without changing the
         * source-owned record type, live pointers or pose dimensions. */
        wr_u16(CONTROL_RECORDS+i*512u+2,
               (uint16_t)((rd_u16(CONTROL_RECORDS+i*512u+2)&0xfffeu)|((scenario>>((i%7)+1))&1u)));
    }
    if(scenario&0x100u) {
        /* Valid source records, including currently dormant slots: enable
         * one slot at a time, or all, while retaining every type/pointer. */
        for(i=1;i<16;++i) {
            uint8_t active=((scenario/2u)%16u==i || (scenario&0x200u))?0x40u:0;
            wr_u8(CONTROL_RECORDS+i*512u+1,
                  (uint8_t)((rd_u8(CONTROL_RECORDS+i*512u+1)&~0x44u)|active));
        }
    }
    wr_u8(0xc457bau,(uint8_t)((scenario>>4)&1u));
    wr_u8(0xc457bbu,(uint8_t)((scenario>>5)&1u));
    wr_u8(POST_INPUT_EVENT,(uint8_t)(scenario&1u));
    wr_u16(0xc458dau,(uint16_t)(scenario%16u));
    wr_u8(0xc4584fu,(uint8_t)(scenario/16u)); wr_u8(0xc4584eu,(uint8_t)(255u-scenario/16u));
    wr_u16(CONTROL_RECORDS+0x4cu,words[(scenario/32u)%5u]);
    wr_u16(CONTROL_RECORDS,(uint16_t)((rd_u16(CONTROL_RECORDS)&~2u)|((scenario>>6)&2u)));
    if(selected_entry==0xc1c63eu) {
        wr_u8(CONTEXT_SELECT,(uint8_t)((scenario>>1)&1u));
        wr_u8(0xc45854u,(uint8_t)(scenario&7u)); wr_u8(0xc45855u,(uint8_t)((scenario>>3)&7u));
        wr_u8(0xc45786u,(uint8_t)((scenario>>6)&1u));
        wr_u32(POSITION_BIAS,signed_values[(scenario/4u)%11u]);
        wr_u32(0xc45a6eu,signed_values[(scenario/44u)%11u]);
        wr_u16(0xc45a5eu,(uint16_t)random_value());
        wr_u8(UPDATE_MASK,(uint8_t)random_value());
        /* Signed selector offsets for valid source-owned records. */
        wr_u16(VIEW_RECORD,(uint16_t)((scenario%4u)*512u));
        wr_u8(0xc458aeu,(uint8_t)(scenario%3u));
        wr_u32(0xc45a78u,(scenario&0x10u)?0xfffff000u:0xfffff001u);
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
        fprintf(stderr,"record-update-stage oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(source_call(0xc70000u,0xc7ff04u)!=FA18_RET) {
            fprintf(stderr,"record-update-stage oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        switch(selected_entry) {
        case 0xc22c80u: glue_C22C80(); break;
        case 0xc1c63eu: glue_C1C63E(); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"record-update-stage oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"record-update-stage oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"record-update-stage oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    printf("record-update-stage oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM\n",selected_entry,cases);
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
