/* Full C2B3C2 with original scene data and the actual native projection/text. */
#define FA18_HUD_ORACLE_LIBRARY
#include "native_hud_oracle.c"
#include "../../port/game/native/frame_labels.c"

static uint32_t scene_axis(gaddr row,unsigned axis) {
    gaddr terrain=0xc1d7e2u+(gaddr)(int32_t)(int16_t)(rd_s16(row+4)*4);
    uint32_t cell=(uint32_t)(int32_t)rd_s16(row+2*axis)<<24;
    uint32_t tile=(uint32_t)(int32_t)rd_s16(terrain+2*axis)<<10;
    uint32_t local=(uint32_t)(int32_t)rd_s16(row+6+2*axis)<<10;
    uint32_t fine=(uint32_t)(int32_t)rd_s16(row+10+2*axis)<<4;
    return cell+tile+local+fine;
}

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);unsigned visible=0,numbers=0;
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    for(unsigned test=0;test<256;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        wr_u8(MODE_SELECT,test&128?3:1);
        wr_u8(ORIGIN_DETAIL_MODE,(uint8_t[]){4,5,6,7}[(test>>4)&3]);
        wr_u32(POSITION_BIAS,test&64?0xfe800000:0xfe7fffff);
        wr_u8(ORIGIN_ENABLE,test&1?1:0);wr_u8(ORIGIN_GATE_A,test&2?1:0);
        wr_u8(0xc45848,(uint8_t)(test%5));wr_u8(0xc45857,test&4?1:0);
        wr_u8(0xc45883,test&8?1:0);wr_u16(VIEW_RECORD,0);
        gaddr origin=rd_u8(ORIGIN_ENABLE)?0xc45c3e:CONTROL_RECORDS+20;
        uint32_t wx=scene_axis(0xc42a02,0),wz=scene_axis(0xc42a02,1);
        int16_t dx=(int16_t)((test&15)*64-448),dy=(int16_t)((test&7)*64-192);
        wr_u32(origin,wx+((uint32_t)(int32_t)dx<<14));
        wr_u32(origin+4,(uint32_t)(int32_t)dy<<14);
        wr_u32(origin+8,wz-((uint32_t)512<<14));
        for(unsigned i=0;i<9;++i) wr_u16(VIEW_ANGLE_MATRIX+2*i,i%4==0?256:0);
        wr_u16(LINE_LAST_ROW,199);wr_u32(PROJECTED_PAIR,0xffffffff);
        memcpy(before,m,sizeof *m);
        native_frame_scene_labels();
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        if(!hud_original(0xc2b3c2)) return 1;
        unsigned differences=0;
        for(unsigned i=0;i<0xffc00;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                if(differences<8) fprintf(stderr,"scene-label case %u at %06X: source %02X native %02X\n",
                    test,i<0x80000?i:i-0x80000+0xc00000,actual,expected[i]);
                ++differences;
            }
        }
        if(differences) return 1;
        if(memcmp(before->slow+TEXT_LINE-0xc00000,m->slow+TEXT_LINE-0xc00000,4)) ++numbers;
        for(unsigned plane=0;plane<4;++plane) {
            gaddr a=rd_u32(rd_u32(PAGE_PLANE_TABLE)+4*plane);
            const uint8_t *old=a<0x80000?before->chip+a:before->slow+a-0xc00000;
            const uint8_t *now=a<0x80000?m->chip+a:m->slow+a-0xc00000;
            if(memcmp(old,now,8000)) {++visible;break;}
        }
    }
    if(!visible || !numbers) {fputs("scene label fixtures did not draw points and numbers\n",stderr);return 1;}
    printf("256 scene-label cases match original non-stack RAM; %u draw visible pixels, %u publish numbers\n",visible,numbers);
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
