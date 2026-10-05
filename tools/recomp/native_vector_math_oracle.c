/* CPU, ROM and captured RAM are validation only. The native owner is pure C. */
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_vector_math_source.h"
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/scene_component_magnitude.c"
#include "../../port/native_vector_math.c"
#include "native_vector_math_fixture.h"
#include "../../port/disk.h"

static int vector_original_assets(void) {
    FA18Disk disk; FA18Hunks hunks; FA18LoadedSceneMagnitudeTable loaded;
    PortFieldWindow window; size_t size=0; uint8_t *file; uint16_t word; unsigned i;
    if(!fa18_disk_open(&disk,"FA-18 Interceptor (1988)(Electronic Arts)[cr A-Ha].adf")) return 0;
    file=fa18_disk_read(&disk,"F-18 Interceptor",&size);
    if(!file || !fa18_hunks_load(&hunks,file,size)) return 0;
    if(fa18_load_scene_magnitude_window(&hunks,&window)!=0 ||
       fa18_load_scene_magnitude_table(&hunks,&loaded)!=0 ||
       window.bytes!=hunks.segments[FA18_SCENE_MAGNITUDE_HUNK].data ||
       window.byte_count!=hunks.segments[FA18_SCENE_MAGNITUDE_HUNK].size ||
       window.origin!=FA18_SCENE_MAGNITUDE_OFFSET) return 0;
    for(i=0;i<FA18_SCENE_MAGNITUDE_WORDS;++i)
        if(!port_field_window_u16(&window,2*i,&word) || word!=loaded.words[i] ||
           word!=rd_u16(MAGNITUDE_TABLE+2*i)) {
            fprintf(stderr,"magnitude asset word %u differs\n",i); return 0;
        }
    if(!port_field_window_u16(&window,-2,&word) ||
       word!=fa18_be16(window.bytes+window.origin-2)) return 0;
    puts("magnitude assets: all 258 original Hunk-8 words match sealed source; full hunk window and preceding word bound");
    fa18_hunks_free(&hunks); free(file); fa18_disk_close(&disk); return 1;
}

static int original_vector(void) {
    unsigned step,i;
    for(step=0;step<5000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        /* Exact invariant: zero shifted left remains zero and this signed
         * comparison keeps returning to the same upward search forever. */
        if(pc==0xc2578a && !REG_D[0] && (int32_t)REG_D[1]>=0) return 2;
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected vector PC %06X\n",pc); return 0;
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
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),table_bytes[65536];
    PortFieldWindow table={.bytes=table_bytes,.byte_count=sizeof table_bytes,.origin=32768};
    FA18NativeVectorMath math; uint16_t magnitude; int16_t normalized[3];
    static const uint32_t entries[]={0xc2574a,0xc25754,0xc1d974};
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,entry,scenario,i,j,completed=0,loops=0;
    if(!state || !rom || !m || !base || !before || !expected || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    if(!vector_original_assets()) { fprintf(stderr,"original magnitude assets differ\n"); return 1; }
    for(entry=0;entry<3;++entry) for(scenario=0;scenario<cases;++scenario) {
        uint32_t scale_word,axis,source_axis,full,source_full; int32_t input[3]; int source_ok,ok;
        selected_entry=entries[entry]; memcpy(m,base,sizeof *m); scene_fixture(scenario);
        vector_fixture(scenario,entry,&scale_word,input);
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            for(j=0;j<scene_source_bytes[i].length;++j)
                if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
        for(i=0;i<sizeof table_bytes;++i) table_bytes[i]=rd_u8(MAGNITUDE_TABLE-32768+i);
        magnitude=rd_u16(MAGNITUDE); for(i=0;i<3;++i) normalized[i]=rd_s16(NORMALIZED+2*i);
        math=(FA18NativeVectorMath){&table,&magnitude,normalized}; axis=REG_D[4];
        memcpy(before,m,sizeof *m); source_ok=original_vector();
        if(!source_ok) { fprintf(stderr,"source vector stopped %06X case %u\n",selected_entry,scenario); return 1; }
        source_axis=REG_D[4]; source_full=REG_D[1];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(entry<2) ok=entry==0?fa18_normalize_native_vector(&math,(int16_t)scale_word,input,&axis):
            fa18_normalize_native_vector_with_direction(&math,(int16_t)scale_word,(int16_t)(scale_word>>16),input,&axis);
        else {
            ok=fa18_scene_component_magnitude_window_value(&table,(int16_t)input[0],(int16_t)input[1],
                (int16_t)input[2],&full,&axis)==0;
            if(ok) magnitude=(uint16_t)full;
            if(ok && full!=source_full) { fprintf(stderr,"magnitude full case %u %08X/%08X input %04X %04X %04X axis %08X/%08X\n",scenario,source_full,full,(uint16_t)input[0],(uint16_t)input[1],(uint16_t)input[2],source_axis,axis); return 1; }
        }
        if(ok!=(source_ok==1) || axis!=source_axis) {
            fprintf(stderr,"vector %06X case %u completion %d/%d axis %08X/%08X\n",selected_entry,scenario,source_ok,ok,source_axis,axis); return 1;
        }
        wr_u16(MAGNITUDE,magnitude); for(i=0;i<3;++i) wr_u16(NORMALIZED+2*i,(uint16_t)normalized[i]);
        if(memcmp(m->chip,expected,FA18_CHIP_SIZE) || memcmp(m->slow,expected+FA18_CHIP_SIZE,0x7fd00) ||
           memcmp(m->slow+0x7ff00,expected+FA18_CHIP_SIZE+0x7ff00,FA18_SLOW_SIZE-0x7ff00)) {
            for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
                uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
                if(a>=0xc7fd00 && a<0xc7ff00) continue;
                if(rd_u8(a)!=expected[i]) { fprintf(stderr,"vector %06X case %u RAM %06X %02X/%02X\n",selected_entry,scenario,a,expected[i],rd_u8(a)); return 1; }
            }
        }
        if(source_ok==1) ++completed; else ++loops;
    }
    printf("native vector math: %u calls match all game RAM, normalized words, full magnitude and carried axis; %u complete/%u proven source loops; no child contracts\n",cases*3,completed,loops);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(expected); free(before); free(base); free(m); return 0;
}
