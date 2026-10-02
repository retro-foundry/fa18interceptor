/* Complete selector-origin proof, including cold internal paths and real children. */
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
static uint32_t selected_entry=0xc29042u;
static void fixture(unsigned scenario) {
    static const uint32_t limits[]={0,1,0x23f,0x240,0x241,0x27ff,0x2800,0x2801,
        0x7fff,0x8000,0x8001,0xcfff,0xd000,0xd001,0x2ffff,0x30000,0x30001,
        0x5ffff,0x60000,0x60001,0xfffff,0x100000,0x100001,0x1fffff,0x200000,
        0x200001,0x7fffff,0x800000,0x800001,0xcfffff,0xd00000,0xd00001,
        0x1bfffff,0x1c00000,0x1c00001,0x7fffffff,0x80000000u,0xffffffffu};
    static const uint8_t enables[]={0xff,1,2,3,16,17,32,63,64,65,127};
    static const uint8_t types[]={0x11,0x14,0x30,0x17};
    static const uint8_t details[]={0,1,5,0xff};
    static const uint16_t angle[]={0,1,0x1c1f,0x1c20,0x5460,0x5461,0x7fff,0x8000,0xffff};
    static const uint8_t counters[]={0,1,2,5,0x7f,0x80,0xff};
    static const uint8_t auxiliary_flags[]={0,1,0x7f,0x80,0xff};
    static const uint32_t auxiliary_delta[]={0,0xffefffffu,0xfff00000u,0xfff00001u,0x80000000u,0x7fffffffu};
    static const uint16_t floor_words[]={0,1,0x7ff8,0x7ff9,0x7fff,0x8000,0xfff8,0xfff9,0xffff};
    uint32_t magnitude=limits[(scenario/9u)%38u];
    unsigned i,lane=scenario%16u,mode=(scenario/16u)%9u;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[6]=0xc62080u;
    for(i=0;i<32;++i) wr_u32(0xc62040u+4*i,random_value());
    wr_u8(ORIGIN_ENABLE,1); wr_u8(ORIGIN_GATE_B,1); wr_u8(ORIGIN_GATE_A,0);
    wr_u16(ORIGIN_RECORD_OFFSET,(uint16_t)((scenario/64u)%4u)*512u);
    wr_u8(ORIGIN_GATE_MODE,0); wr_u8(ORIGIN_DETAIL_MODE,2);
    wr_u8(ORIGIN_ADJUSTMENT_MODE,(uint8_t)mode);
    wr_u8(ORIGIN_VARIANT_SELECTOR,(uint8_t)((scenario/144u)%4u));
    wr_u8(ORIGIN_DETAIL_COUNTER,counters[(scenario/144u)%7u]);
    wr_u8(ORIGIN_AUXILIARY_FLAG,auxiliary_flags[(scenario/288u)%5u]);
    for(i=0;i<3;++i) {
        uint32_t origin=random_value(),delta=(i==scenario%3u)?magnitude:0;
        if((scenario/512u)&1u) delta=0u-delta;
        wr_u32(SELECTOR_ORIGIN+4*i,origin);
        wr_u32(ORIGIN_CANDIDATE_TRIPLE+4*i,origin+delta);
        wr_u32(ORIGIN_SMOOTHED_DELTA+4*i,(scenario&0x100u)?random_value():0);
    }
    if(mode==7) wr_u32(ORIGIN_AUXILIARY_DELTA,auxiliary_delta[(scenario/432u)%6u]);
    if(lane<3) {
        if(lane==0) wr_u8(ORIGIN_ENABLE,0);
        else if(lane==1) wr_u8(ORIGIN_GATE_B,0);
        else wr_u8(ORIGIN_GATE_A,1);
    } else if(lane==3) {
        wr_u8(ORIGIN_GATE_MODE,1); wr_u8(ORIGIN_DETAIL_MODE,0);
    } else if(lane<8) {
        gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(ORIGIN_RECORD_OFFSET);
        wr_u8(ORIGIN_DETAIL_MODE,details[(scenario/16u)%4u]);
        wr_u8(ORIGIN_ENABLE,enables[(scenario/64u)%11u]);
        wr_u8(ORIGIN_DETAIL_INDEX,(uint8_t)((scenario/704u)%10u));
        wr_u8(record+0x62u,types[(scenario/32u)%4u]);
        wr_u16(record+0x68u,angle[(scenario/128u)%9u]);
        wr_u16(ORIGIN_ANGLE_HISTORY,angle[(scenario/1152u)%9u]);
        wr_u8(record+4u,(uint8_t)((rd_u8(record+4u)&0x3fu)|(((scenario/256u)%4u)<<6)));
        wr_u16(record+0x4eu,floor_words[(scenario/256u)%9u]);
    } else if((scenario/256u)&1u) wr_u8(ORIGIN_DETAIL_MODE,(uint8_t)(6+scenario%3u));
    if(selected_entry==0xc29368u) {
        /* A real terminated list shape, using original control-record slots.
         * Record fields outside the source selection contract stay sealed. */
        wr_u32(ORIGIN_RECORD_LIST,0xc60000u);
        for(i=0;i<16;++i) {
            gaddr record=CONTROL_RECORDS+512*i;
            wr_u16(0xc60000u+10*i,0); wr_u16(0xc60004u+10*i,(uint16_t)i);
            wr_u8(record+0x62u,((scenario+i)%3u)?0x11:0x30);
            wr_u8(record+1u,(uint8_t)(((scenario+i)%4u)?0x40:0)|((scenario&16u)?8u:0));
        }
        wr_u16(0xc60000u+10*((scenario/32u)%17u),0xffffu);
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
        fprintf(stderr,"selector-origin oracle: %s\n",error); return 1;
    }
    free(state); free(rom); memcpy(base,m,sizeof *m);
    fa18_recomp_init(1); fa18_ports_init(FA18_PORTS_OFF,NULL); fa18_bus_timing=0;
    for(scenario=0;scenario<cases;++scenario) {
        uint32_t regs[16],sr; unsigned i;
        memcpy(m,base,sizeof *m); fixture(scenario);
        memcpy(before,m,sizeof *m); m68k_get_context(cpu); fa18_write_log_active=1;
        if(source_call(0xc70000u,0xc7ff04u)!=FA18_RET) {
            fprintf(stderr,"selector-origin oracle: case %u source did not return at %06X\n",scenario,REG_PC); return 1;
        }
        memcpy(regs,REG_DA,sizeof regs); sr=m68k_get_reg(NULL,M68K_REG_SR);
        memcpy(reference,m->chip,FA18_CHIP_SIZE);
        memcpy(reference+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); m68k_set_context(cpu);
        switch(selected_entry) {
        case 0xc29042u: glue_C29042(); break;
        case 0xc29368u: glue_origin_control_record(); break;
        case 0xc29490u: glue_origin_candidate_preset(ORIGIN_ALTERNATE_PRESET); break;
        case 0xc2949au: glue_origin_candidate_preset(ORIGIN_ROOT_PRESET); break;
        default: return 1;
        }
        fa18_write_log_active=0;
        for(i=0;i<16;++i) if(regs[i]!=REG_DA[i]) {
            fprintf(stderr,"selector-origin oracle: case %u %c%u source %08X C %08X\n",
                    scenario,i<8?'D':'A',i&7u,regs[i],REG_DA[i]); return 1;
        }
        if(REG_PC!=0xc70000u || sr!=m68k_get_reg(NULL,M68K_REG_SR)) {
            fprintf(stderr,"selector-origin oracle: case %u PC/SR source %04X C %04X\n",
                    scenario,sr,m68k_get_reg(NULL,M68K_REG_SR)); return 1;
        }
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            gaddr address=i<FA18_CHIP_SIZE?i:i-FA18_CHIP_SIZE+FA18_SLOW_BASE;
            uint8_t got=i<FA18_CHIP_SIZE?m->chip[i]:m->slow[i-FA18_CHIP_SIZE];
            if(got!=reference[i]) {
                fprintf(stderr,"selector-origin oracle: case %u byte %06X source %02X C %02X\n",
                        scenario,address,reference[i],got); return 1;
            }
        }
    }
    printf("selector-origin oracle %06X: %u complete calls matched all registers, PC, full SR and all RAM\n",selected_entry,cases);
    free(cpu); free(reference); free(before); free(base); free(m); return 0;
}
