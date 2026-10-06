/* Complete C2B564 and its actual children, against native grid/marker rendering. */
#define FA18_HUD_ORACLE_LIBRARY
#include "native_hud_oracle.c"
#include "../../port/game/native/frame_labels.c"
#define draw_clipped_segment host_draw_clipped_segment
#include "../../port/game/native/frame_markers.c"

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);unsigned visible=0;
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    for(unsigned test=0;test<385;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        /* The final case consumes the runner's actual map/flight state. */
        if(test<384) {
        wr_u8(ORIGIN_GATE_MODE,test<128 && (test&64)?0:1);
        wr_u8(ORIGIN_ENABLE,test<128 && (test&32)?0:1);
        wr_u8(MODE_SELECT,test&16?3:1);wr_u8(0xc457b5,test&8?1:0);
        wr_u8(0xc45857,(uint8_t[]){0,1,3,255}[(test>>1)&3]);
        wr_u16(UPDATE_TICK,(uint16_t)test);wr_u16(0xc459ba,test&4?512:0);
        wr_u8(0xc458af,test&2?255:0);wr_u16(LINE_LAST_ROW,199);
        wr_u32(0xc45c3e,(test&3)*0x10000u);wr_u32(0xc45c42,0);
        wr_u32(0xc45c46,(test&3)*0x20000u);
        for(unsigned i=0;i<9;++i) wr_u16(VIEW_ANGLE_MATRIX+2*i,i%4==0?256:0);
        if(test>=128) {
            static const int16_t matrices[][9]={
                {0,0,256,0,256,0,-256,0,0},
                {256,0,0,0,0,256,0,-256,0},
                {-256,0,0,0,256,0,0,0,-256},
                {0,256,0,-256,0,0,0,0,256}
            };
            for(unsigned i=0;i<9;++i) wr_s16(VIEW_ANGLE_MATRIX+2*i,matrices[(test>>6)&3][i]);
            wr_u32(0xc45c42,(uint32_t)(int32_t)(test&1?-64:64)<<16);
            wr_u32(0xc45c3e,(uint32_t)(int32_t)(test&4?-31:31)<<16);
        }
        for(unsigned i=0;i<16;++i) {
            gaddr r=CONTROL_RECORDS+512*i;
            wr_u16(r,i<(test>=128?16u:3u)?i==2?0x48:0x40:0);
            wr_u8(r+3,test&4?128:0);wr_u8(r+32,test&2?64:0);
            wr_u8(r+98,i%3==1?0x20:i%3==2?0x30:0x10);
            wr_u16(r+104,(uint16_t)((test*1800+i*900)%30000));
            wr_u8(r+112,(uint8_t)(test*17));wr_u8(r+113,test&1?255:3);
            wr_u32(r+20,((uint32_t)(i*128)<<16));
            wr_u32(r+24,0);wr_u32(r+28,((uint32_t)(512+i*128)<<16));
        }
        }
        memcpy(before,m,sizeof *m);
        native_frame_grid_and_markers();
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        if(!hud_original(0xc2b564)) return 1;
        unsigned differences=0;
        for(unsigned i=0;i<0xffc00;++i) {
            if(i>=GRID_FRAME-10 && i<GRID_FRAME) continue;
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                if(differences<12) fprintf(stderr,"grid-marker case %u at %06X: source %02X native %02X\n",
                    test,i<0x80000?i:i-0x80000+0xc00000,actual,expected[i]);
                ++differences;
            }
        }
        if(differences) {fprintf(stderr,"%u grid-marker differences\n",differences);return 1;}
        for(unsigned plane=0;plane<4;++plane) {
            gaddr a=rd_u32(rd_u32(PAGE_PLANE_TABLE)+4*plane);
            const uint8_t *old=a<0x80000?before->chip+a:before->slow+a-0xc00000;
            const uint8_t *now=a<0x80000?m->chip+a:m->slow+a-0xc00000;
            if(memcmp(old,now,8000)) {++visible;break;}
        }
    }
    if(!visible) {fputs("grid-marker fixtures drew no visible pixels\n",stderr);return 1;}
    printf("385 full grid-marker cases match original non-stack RAM; %u draw visible pixels\n",visible);
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
