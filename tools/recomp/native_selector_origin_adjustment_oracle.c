/* CPU, ROM and captured RAM are validation only. The native owner is pure C. */
#ifndef FA18_SCENE_SOURCE_HEADER
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_selector_origin_adjustment_source.h"
#endif
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/scene_component_magnitude.c"
#include "../../port/native_vector_math.c"
#include "native_vector_math_fixture.h"
#include "../../port/terrain_selector_origin_adjustment.c"

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
#ifndef FA18_ORIGIN_MATH_MAIN
#define FA18_ORIGIN_MATH_MAIN main
#endif
int FA18_ORIGIN_MATH_MAIN(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE),table_bytes[65536];
    PortFieldWindow table={.bytes=table_bytes,.byte_count=sizeof table_bytes,.origin=32768};
    FA18NativeVectorMath math; uint16_t magnitude; int16_t normalized[3];
    static const uint32_t entries[]={0xc2574a,0xc25754,0xc1d974,0xc29548};
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,entry,scenario,i,j,completed=0,loops=0;
    if(!state || !rom || !m || !base || !before || !expected || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(entry=0;entry<4;++entry) for(scenario=0;scenario<cases;++scenario) {
        uint32_t scale_word,axis,source_axis,full,source_full; int32_t input[3],origin[3],smoothed[3],companion[3]; uint32_t amount=0; uint16_t shift=0; int source_ok,ok;
        selected_entry=entries[entry]; memcpy(m,base,sizeof *m); scene_fixture(scenario);
        vector_fixture(scenario,entry<3?entry:0,&scale_word,input);
        if(entry==3) {
            static const uint32_t amounts[]={0,1,0x47ff,0x4800,0x4801,0x12000,0x7fffffff,0x80000000,0xffffffff};
            amount=amounts[scenario%9]; shift=(uint16_t)(scenario%70);
            REG_D[3]=amount; REG_D[4]=(REG_D[4]&0xffff0000u)|shift;
            for(i=0;i<3;++i) {
                REG_D[5+i]=(uint32_t)input[i];
                wr_u32(ORIGIN_SMOOTHED_DELTA+4*i,scenario%3==0?0:random_value());
                wr_u32(SELECTOR_ORIGIN+4*i,random_value());
                wr_u32(ORIGIN_NEGATED_COMPANION+4*i,random_value());
                smoothed[i]=rd_s32(ORIGIN_SMOOTHED_DELTA+4*i);
                origin[i]=rd_s32(SELECTOR_ORIGIN+4*i);
                companion[i]=rd_s32(ORIGIN_NEGATED_COMPANION+4*i);
            }
        }
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
        else if(entry==2) {
            ok=fa18_scene_component_magnitude_window_value(&table,(int16_t)input[0],(int16_t)input[1],
                (int16_t)input[2],&full,&axis)==0;
            if(ok) magnitude=(uint16_t)full;
            if(ok && full!=source_full) { fprintf(stderr,"magnitude full case %u %08X/%08X input %04X %04X %04X axis %08X/%08X\n",scenario,source_full,full,(uint16_t)input[0],(uint16_t)input[1],(uint16_t)input[2],source_axis,axis); return 1; }
        }
        else {
            FA18TerrainSelectorOriginAdjustmentState adjustment={.magnitude=(int32_t)amount,
                .candidate=input,.smoothed_delta=smoothed,.origin=origin,.negated_companion=companion,
                .shift=shift,.vector_math=&math};
            ok=fa18_adjust_terrain_selector_origin(&adjustment)==0;
            for(i=0;i<3;++i) {
                wr_u32(ORIGIN_SMOOTHED_DELTA+4*i,(uint32_t)smoothed[i]);
                wr_u32(SELECTOR_ORIGIN+4*i,(uint32_t)origin[i]);
                wr_u32(ORIGIN_NEGATED_COMPANION+4*i,(uint32_t)companion[i]);
            }
        }
        if(ok!=(source_ok==1) || (entry<3 && axis!=source_axis)) {
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
    printf("native selector-origin adjustment: %u calls match all game RAM, shared math words and adjusted origins; core entries also match full magnitude/carried axis; %u complete/%u proven source loops; no child contracts\n",cases*4,completed,loops);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(expected); free(before); free(base); free(m); return 0;
}
