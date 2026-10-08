/* Complete view/projection/cached-sort calls against original bytes.
 * CPU, ROM and controlled caller variants belong only to this oracle. */
#define main records_reference_main
#include "native_records_oracle.c"
#undef main
#include "view.h"

static int same_ram(const uint8_t *expected,unsigned test) {
    for(unsigned i=0;i<0xff000;++i) {
        const uint8_t actual=i<0x80000?fa18_machine->chip[i]:fa18_machine->slow[i-0x80000];
        if(actual!=expected[i]) {
            fprintf(stderr,"View case %u RAM %06X source=%02X native=%02X\n",test,
                i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 0;
        }
    }
    return 1;
}

static int source_call(gaddr entry) {
    REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);REG_PC=entry;
    m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    return original();
}

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*saved=malloc(sizeof *saved),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!saved||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) return 1;
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);memcpy(saved,m,sizeof *saved);
    unsigned positive=0,negative=0;
    for(unsigned test=0;test<640;++test) {
        memcpy(m,saved,sizeof *saved);
        const int matrix_only=test<128;
        const gaddr matrix=VIEW_ANGLE_MATRIX;
        if(matrix_only) {
            for(unsigned i=0;i<9;++i) wr_u16(matrix+2*i,(uint16_t)(test*997+i*7919));
            for(unsigned i=0;i<3;++i) wr_u16(MATRIX_ROW_SCALES+2*i,
                (uint16_t[]){0,1,128,256,0x7fff,0x8000,0xffff,0x1234}[(test+i)%8]);
        } else {
            const unsigned profile=test-128;
            const gaddr record=CONTROL_RECORDS+(profile&64?0x1000u:0);
            const unsigned route=profile/128;
            wr_u8(CONTEXT_SELECT,0xff);wr_u8(CONTEXT_STARTED,route==0?0:route==1?1:0xff);
            wr_u8(CONTEXT_STATE,(uint8_t[]){0,1,6,8}[(profile/32)%4]);
            wr_u8(CONTEXT_SMOOTH,profile&16?1:0);wr_u8(TRACK_STARTED,profile&8?1:0);
            wr_u8(POST_INPUT_EVENT,route==3?1:0);wr_u8(TARGET_ENABLED,1);
            wr_u16(VIEW_RECORD,(uint16_t)(record-CONTROL_RECORDS));
            wr_u16(VIEW_PAN,(uint16_t)((profile%8)*3600+profile%3));
            wr_u16(VIEW_ROTATE,(uint16_t)(((profile/8)%8)*3600+profile%3));
            wr_u8(record+0x62,(uint8_t[]){0x10,0x11,0x14,0x30}[(profile/16)%4]);
            set_record_orientation(record,(uint16_t)(profile*53%28800),
                (uint16_t)(profile*193%28800),(uint16_t)(profile*71%28800));
            wr_u8(PAUSE_A,profile&4?1:0);wr_u8(STICK_X,profile&2?8:1);wr_u8(STICK_Y,profile&1?0x20:0);
            wr_u8(SORT_LISTS_ON,1);wr_u8(SORT_LIST_NEXT,1);wr_u8(SORT_LIST_COUNT,1);
            wr_u32(SORT_LISTS,0xc6e000);wr_u16(SORT_LISTS+4,4);
            for(unsigned i=0;i<4;++i) {
                const gaddr entry=0xc6e000+24*i;
                for(unsigned k=0;k<24;++k) wr_u8(entry+k,0);
                wr_u16(entry,(uint16_t)(profile&1?0x40:0));
                wr_u16(entry+16,(uint16_t)(17+i));
                wr_u32(entry+12,0x79130000u+i*0x20131);
            }
        }
        wr_u8(0xc7ff54,0);wr_u16(0xc7fe9c,0x6bad);
        memcpy(before,m,sizeof *before);
        memset(REG_DA,0,sizeof REG_DA);REG_D[3]=0x53a90123;
        REG_A[6]=0xc7ff80;REG_A[1]=matrix;
        if(!source_call(matrix_only?0xc2e5ac:0xc2d9ba)) return 1;
        const uint32_t scaled=REG_D[3];
        uint16_t retained=0;
        if(!matrix_only) {
            if(!source_call(0xc1c54e)) return 1;
            if((REG_D[3]>>16)!=(scaled>>16)) {
                fprintf(stderr,"Projection changed the view factor in case %u\n",test);return 1;
            }
            if(!source_call(0xc1e328)) return 1;
            retained=rd_u16(0xc7fe9c);
        }
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *before);
        const uint32_t factor=(uint32_t)(matrix_only?scale_matrix_rows(matrix,MATRIX_ROW_SCALES):aim_view_depth_factor());
        if(matrix_only) {
            if(factor!=scaled) {
                fprintf(stderr,"Scaled case %u source=%08X native=%08X\n",test,scaled,factor);return 1;
            }
        } else {
            seed_projection();
            DisplaySortResult result={.planar_factor=factor,.has_factor=1};
            sort_display_list_retained(0,&result);
            if((factor>>16)!=(scaled>>16) || !result.has_output || result.retained_word!=retained) {
                fprintf(stderr,"View case %u factor source=%08X native=%08X retained source=%04X native=%04X\n",
                    test,scaled,factor,retained,result.retained_word);return 1;
            }
            if(factor>>16) ++negative;else ++positive;
        }
        if(!same_ram(expected,test)) return 1;
    }
    printf("128 complete scaled-matrix and 512 view/projection/cached-sort cases match original output and non-stack RAM; factors positive=%u negative=%u\n",
        positive,negative);
    free(expected);free(before);free(saved);free(m);free(data);free(rom);free(state);return 0;
}
