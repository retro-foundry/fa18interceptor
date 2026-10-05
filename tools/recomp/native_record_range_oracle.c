/* Original CPU and captured RAM are used only for this differential proof. */
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_record_range_source.h"
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/scene_component_magnitude.c"
#include "../../port/native_record_range.c"

static unsigned range_case,source_tones,native_tones;
static int tone(void *context,FA18NativeRecordRange *s,unsigned program) {
    (void)context; if(program!=4) return 0; ++native_tones;
    *s->selected_record=(uint16_t)((range_case%15+1)*512);
    return 1;
}
static int original_range(void) {
    unsigned step,i;
    for(step=0;step<1000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(pc==0xc3316e) {
            if(REG_D[0]!=4 || rd_u32(REG_A[7])!=0xc2450c) return 0;
            ++source_tones;
            wr_u16(SELECTED_RECORD,(uint16_t)((range_case%15+1)*512));
            REG_PC=m68ki_pull_32(); continue;
        }
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected range PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    SceneState *native=malloc(sizeof *native);
    uint8_t table_bytes[65536],redraw; uint16_t magnitude,stride;
    PortFieldWindow table={.bytes=table_bytes,.byte_count=sizeof table_bytes,.origin=32768};
    FA18NativeRecordRangeOps ops={tone,NULL}; FA18NativeRecordRange range;
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,i,j;
    static const uint16_t distances[]={0,0x2ff,0x300,0x301,0xbff,0xc00,0xc01,
        0x17ff,0x1800,0x1801,0x1dff,0x1e00,0x1e01,0x36bf,0x36c0,0x36c1,0x7f00,0x7f01};
    selected_entry=0xc244e2;
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        for(j=0;j<scene_source_bytes[i].length;++j)
            if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
    for(range_case=0;range_case<cases;++range_case) {
        unsigned slot=(range_case/64)%16,target=(range_case%15)+1;
        uint32_t r=CONTROL_RECORDS+slot*512,t=CONTROL_RECORDS+target*512;
        memcpy(m,base,sizeof *m); scene_fixture(range_case); REG_A[1]=r;
        wr_u16(SELECTED_RECORD,range_case%17==0?0xffff:range_case%17==1?0:target*512);
        wr_u16(SCRIPT_RECORD,range_case&1?slot*512:0);
        wr_u16(r+0x6c,range_case%4==0?0x11ff:0x1200);
        wr_u8(r+0x7c,(range_case%3==0?0x70:0)); wr_u8(BAR_REDRAWS_F,range_case%5==0?1:0);
        wr_u16(r+0x4a,range_case%7==0?0x480:range_case%7==1?0x900:0);
        wr_u8(r+0x39,(uint8_t)((range_case%16)<<4)); wr_u8(r+0x7a,(uint8_t)(range_case%6));
        wr_u16(r+6,0); wr_u16(r+8,0); wr_u16(r+0xc,0); wr_u16(r+0xe,0); wr_u32(r+0x10,0);
        wr_u16(t+6,0); wr_u16(t+8,0); wr_u16(t+0xc,0); wr_u16(t+0xe,0);
        wr_u32(t+0x10,distances[range_case%(sizeof distances/sizeof distances[0])]);
        if(range_case%9==0) wr_u16(t+6,3);
        if(range_case%11==0) wr_u16(t+8,3);
        if(range_case%13==0) { wr_u16(t+6,0xffff); wr_u16(t+0xc,0xffff); }
        if(range_case%19==0) { wr_u32(r+0x10,0x80000000); wr_u32(t+0x10,0x7fffffff); }
        if(range_case%23==0) { wr_u16(t+0xc,0x100); wr_u16(t+0xe,0x200); }
        if(range_case%31==0) wr_u16(t+8,0xffff);
        if(range_case%37==0) wr_u32(t+0x10,0u-distances[range_case%(sizeof distances/sizeof distances[0])]);
        if(range_case%41==0) {
            for(i=0;i<FA18_SCENE_MAGNITUDE_WORDS;++i) wr_u16(MAGNITUDE_TABLE+2*i,0xffff);
            wr_u32(t+0x10,0x3000);
        }
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        for(i=0;i<sizeof table_bytes;++i) table_bytes[i]=rd_u8(MAGNITUDE_TABLE-32768+i);
        magnitude=rd_u16(MAGNITUDE); stride=rd_u16(SCRIPT_RECORD); redraw=rd_u8(BAR_REDRAWS_F);
        range=(FA18NativeRecordRange){.records=&native->bank,.table=&table,.ops=&ops,
            .selected_record=&native->selected,.current_stride=&stride,.magnitude=&magnitude,.bar_redraw_f=&redraw};
        memcpy(before,m,sizeof *m);
        if(!original_range()) { fprintf(stderr,"source range stopped case %u\n",range_case); return 1; }
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(!fa18_classify_native_selected_range(&range,slot)) {
            fprintf(stderr,"native range failed case %u\n",range_case); return 1;
        }
        store_scene(native); wr_u16(MAGNITUDE,magnitude); wr_u8(BAR_REDRAWS_F,redraw);
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if(a>=0xc7fd00 && a<0xc7ff00) continue;
            if(rd_u8(a)!=expected[i]) {
                fprintf(stderr,"range case %u byte %06X source %02X native %02X\n",range_case,a,expected[i],rd_u8(a)); return 1;
            }
        }
        if(!verify_record_owners(native)) return 1;
    }
    if(native_tones!=source_tones) return 1;
    printf("native selected range: %u complete calls match all game RAM; actual magnitude; %u sound contracts\n",cases,source_tones);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
