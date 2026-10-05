/* Complete original owner with actual placement/in-sight instructions.
 * CPU, ROM and captured RAM exist only in validation. */
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_record_view_source.h"
#define FA18_SCENE_PLAYER_HELPERS_ONLY
#include "native_scene_player_oracle.c"
#include "../../port/native_record_view.c"

static unsigned view_case,source_norms,native_norms,source_faults,native_faults;
static int32_t norm_input[3];
static int16_t norm_word(unsigned i) {
    static const int16_t words[]={0,-192,192,-32768,32767};
    return words[(view_case/16+i)%5];
}
static int normalize(void *context,FA18NativeRecordView *s,int16_t scale,const int32_t components[3],int16_t output[3],uint32_t *axis) {
    unsigned i; (void)context; (void)s;
    if(scale!=192 || !axis) return 0;
    for(i=0;i<3;++i) { if(components[i]!=norm_input[i]) return 0; output[i]=norm_word(i); }
    ++native_norms; return 1;
}
static int view_fault(void *context,FA18NativeRecordView *s,uint32_t *axis) {
    (void)context; if(!axis) return 0;
    if(*s->error_word!=0x34 && *s->error_word!=0x35) return 0;
    ++native_faults; *s->pending=(uint8_t)(view_case&1); return 1;
}
static int original_view(void) {
    unsigned step,i;
    for(step=0;step<5000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(pc==0xc2574a) {
            if((uint16_t)REG_D[0]!=192 || rd_u32(REG_A[7])!=0xc243a2) return 0;
            for(i=0;i<3;++i) {
                norm_input[i]=(int32_t)REG_D[5+i]; REG_D[5+i]=(uint32_t)(int32_t)norm_word(i);
                wr_u16(NORMALIZED+2*i,(uint16_t)norm_word(i));
            }
            ++source_norms; REG_PC=m68ki_pull_32(); continue;
        }
        if(pc==0xc06c02) {
            if(rd_u16(ERROR_CODE)!=0x34 && rd_u16(ERROR_CODE)!=0x35) return 0;
            ++source_faults; wr_u8(FIRE_RECORD_PENDING,(uint8_t)(view_case&1));
            REG_PC=m68ki_pull_32(); continue;
        }
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            if(scene_source_bytes[i].pc==pc) break;
        if(i==sizeof scene_source_bytes/sizeof scene_source_bytes[0]) {
            fprintf(stderr,"unexpected view PC %06X\n",pc); return 0;
        }
        scene_seen[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
enum { VIEW_TABLE=0xc295e0, STATUS_TABLE=0xc2bb98, LIST_BASE=0xc60000, ZONE_BASE=0xc61000 };
static void view_fixture(unsigned scenario,unsigned slot,unsigned viewer) {
    uint32_t r=CONTROL_RECORDS+512*slot,v=CONTROL_RECORDS+512*viewer;
    static const uint16_t ranges[]={0,0x300,0x301,0xc00,0xc01,0x1e00,0x1e01,0x3000,0x3001};
    static const uint16_t angles[]={0,0xc0,0xff40,0x4200,0x4100,0x4300,0x7fff,0x8000};
    unsigned i,route=scenario%16;
    wr_u8(POST_INPUT_EVENT,route==0); wr_u8(MODE_SELECT,scenario%3?5:2);
    wr_u8(SCENE_DISPATCH_LIMIT,(uint8_t)((scenario/16)%7-2));
    wr_u8(FIRE_RECORD_PENDING,scenario&1); wr_u16(SCRIPT_RECORD,(uint16_t)(slot*512));
    wr_u16(STREAM_MODE,(uint16_t)slot); wr_u16(STREAM_SKIP,(uint16_t)(scenario%3));
    wr_u16(SELECTED_RECORD,scenario%3==0?0xffff:scenario%3==1?slot*512:viewer*512);
    wr_u16(r,route==1?8:route==2?10:route==3?11:route==4?0x1000:route==7?8:route==8?0x88:0);
    wr_u8(r+0x62,route==8?(scenario/16%2?0x15:0x11):0);
    wr_u16(r+2,route==5 || route==6?1:0);
    wr_u8(r+5,route==7 || route==8 || route==9?8:route==10?6:1);
    wr_u8(r+0x20,route==11?2:0); wr_u8(r+4,0x20);
    wr_u8(r+0x38,route==12?0xff:(uint8_t)viewer);
    wr_u8(r+0x39,route==13 || route==14?0x10:0); wr_u8(r+0x3a,0);
    wr_u8(r+0x64,(uint8_t)(scenario/16%4)); wr_u8(r+0x7a,(uint8_t)(scenario%6));
    wr_u8(r+0x5d,route==4 && scenario/16%2?0:1);
    wr_u16(r+0x4a,route==9?0x481:0);
    wr_u16(r+0x6c,angles[(scenario/16)%8]); wr_u16(v+0x6c,angles[(scenario/128)%8]);
    wr_u16(v+0x4a,ranges[(scenario/16)%9]);
    if(route!=1 && route!=2 && route!=3) wr_u16(v,route==4 || route==15?0:0x40);
    for(i=0;i<3;++i) {
        wr_u32(r+0x14+4*i,random_value()); wr_u32(v+0x14+4*i,random_value());
    }
    if(scenario%3==0) for(i=0;i<3;++i) wr_u32(r+0x14+4*i,rd_u32(CONTROL_RECORDS+0x14+4*i));
    for(i=0;i<9;++i) { wr_u16(r+0x92+2*i,(uint16_t)random_value()); wr_u16(v+0x92+2*i,(uint16_t)random_value()); }
    if(route==14) for(i=0;i<3;++i) { wr_u16(r+0x96+6*i,0x4000); wr_u16(v+0x96+6*i,0x4000); }
    wr_u8(v+0x28,(uint8_t)(scenario%2?0x80:0x15)); wr_u8(v+0x2a,0x15);
    wr_u16(VIEW_TABLE,0x200); wr_u16(VIEW_TABLE+0x200,3); wr_u16(VIEW_TABLE+0x202,4);
    wr_u16(VIEW_TABLE+0x204,5); wr_u16(VIEW_TABLE+0x206,6); wr_u16(VIEW_TABLE+0x208,7);
    for(i=0;i<4;++i) wr_u16(r+0x2c+2*i,(uint16_t)(3+i));
    wr_u16(VIEW_TABLE+0x20a,route==8?0xffff:8);
    for(i=1;i<5;++i) wr_u16(VIEW_TABLE+0x20a+2*i,(uint16_t)(8+i));
    if(route==9) wr_u16(r+0x4a,0);
    if(route==9) { wr_u16(r+0x2c,0x7777); wr_u16(VIEW_TABLE+0x20a,0xffff); }
    for(i=0;i<0x200;++i) wr_u8(STATUS_TABLE+i,(uint8_t)random_value());
    wr_u32(POST_INPUT_RECORD_LIST,LIST_BASE); wr_u16(LIST_BASE,scenario%2?0xffff:1);
    wr_u16(LIST_BASE+4,(uint16_t)(route==4?slot+1:slot)); wr_u16(LIST_BASE+6,0); wr_u16(LIST_BASE+10,0xffff);
    wr_u32(0xc29720,ZONE_BASE); wr_u16(ZONE_BASE+10,1);
    wr_u16(ZONE_BASE+14,(uint16_t)slot); wr_u16(ZONE_BASE+16,0); wr_u16(ZONE_BASE+20,0xffff);
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE); SceneState *native=malloc(sizeof *native);
    uint8_t parameters[65536],status[1024],list[32],zone[32];
    uint16_t stride,slot_word,tick,error_word; uint8_t event,mode,limit,pending,flag,created,admitted;
    int16_t normalized[3]; FA18NativeRecordViewAssets assets; PortFieldWindow zone_window;
    FA18NativeRecordViewOps ops={normalize,view_fault,NULL}; FA18NativeRecordView view;
    FA18NativeRecordViewWork work; unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,i,j;
    selected_entry=0xc23ca6;
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        for(j=0;j<scene_source_bytes[i].length;++j)
            if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) return 1;
    for(view_case=0;view_case<cases;++view_case) {
        unsigned slot=(view_case/256)%16,viewer=(slot+1+(view_case/32)%15)%16; uint32_t axis,companion;
        if(view_case%29==0) viewer=slot;
        memcpy(m,base,sizeof *m); scene_fixture(view_case); view_fixture(view_case,slot,viewer);
        REG_A[1]=CONTROL_RECORDS+slot*512; REG_A[2]=CONTROL_RECORDS+viewer*512; REG_A[3]=CONTROL_RECORDS+viewer*512;
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        for(i=0;i<sizeof parameters;++i) parameters[i]=rd_u8(VIEW_TABLE-32768+i);
        for(i=0;i<sizeof status;++i) status[i]=rd_u8(STATUS_TABLE-512+i);
        for(i=0;i<sizeof list;++i) { list[i]=rd_u8(LIST_BASE+i); zone[i]=rd_u8(ZONE_BASE+i); }
        zone_window=(PortFieldWindow){.bytes=zone,.byte_count=sizeof zone};
        assets=(FA18NativeRecordViewAssets){
            .parameters={.bytes=parameters,.byte_count=sizeof parameters,.origin=32768},
            .status={.bytes=status,.byte_count=sizeof status,.origin=512},
            .primary_list={.bytes=list,.byte_count=sizeof list},.zones=&zone_window,.zone_count=1};
        stride=rd_u16(SCRIPT_RECORD); slot_word=rd_u16(STREAM_MODE); tick=rd_u16(STREAM_SKIP); error_word=rd_u16(ERROR_CODE);
        event=rd_u8(POST_INPUT_EVENT); mode=rd_u8(MODE_SELECT); limit=rd_u8(SCENE_DISPATCH_LIMIT);
        pending=rd_u8(FIRE_RECORD_PENDING); flag=rd_u8(RECORD_VIEW_FLAG);
        created=rd_u8(SCENE_DISPATCH_CREATED); admitted=rd_u8(SCENE_DISPATCH_ADMITTED);
        for(i=0;i<3;++i) normalized[i]=rd_s16(NORMALIZED+2*i);
        view=(FA18NativeRecordView){.records=&native->bank,.assets=&assets,.ops=&ops,
            .selected_record=&native->selected,.current_stride=&stride,.current_slot=&slot_word,.tick_word=&tick,.error_word=&error_word,
            .post_input_event=&event,.mode=&mode,.limit=&limit,.pending=&pending,.view_flag=&flag,.created=&created,.admitted=&admitted,.normalized=normalized};
        work=(FA18NativeRecordViewWork){native->bank.records+viewer,REG_D[4],native->bank.records+viewer};
        memcpy(before,m,sizeof *m);
        if(!original_view()) { fprintf(stderr,"source view stopped case %u\n",view_case); return 1; }
        axis=REG_D[4]; companion=REG_A[2];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(!fa18_update_native_record_view(&view,slot,&work)) { fprintf(stderr,"native view failed case %u\n",view_case); return 1; }
        if(companion!=CONTROL_RECORDS+512*(unsigned)(work.companion-native->bank.records)) return 1;
        if(work.carried_axis!=axis) { fprintf(stderr,"view axis case %u expected %08X native %08X\n",view_case,axis,work.carried_axis); return 1; }
        store_scene(native); wr_u16(ERROR_CODE,error_word); wr_u8(FIRE_RECORD_PENDING,pending); wr_u8(RECORD_VIEW_FLAG,flag);
        wr_u8(SCENE_DISPATCH_CREATED,created); wr_u8(SCENE_DISPATCH_ADMITTED,admitted);
        for(i=0;i<3;++i) wr_u16(NORMALIZED+2*i,(uint16_t)normalized[i]);
        for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
            uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
            if(a>=0xc7fd00 && a<0xc7ff00) continue;
            if(rd_u8(a)!=expected[i]) {
                fprintf(stderr,"view case %u byte %06X source %02X native %02X\n",view_case,a,expected[i],rd_u8(a)); return 1;
            }
        }
        memcpy(m->chip,expected,FA18_CHIP_SIZE); memcpy(m->slow,expected+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        if(!verify_record_owners(native)) return 1;
    }
    if(source_norms!=native_norms || source_faults!=native_faults) return 1;
    printf("native record view: %u calls match all game RAM; %u normalization/%u fault contracts\n",cases,source_norms,source_faults);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
