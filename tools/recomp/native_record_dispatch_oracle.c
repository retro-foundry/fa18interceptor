/* Source CPU/ROM exist only in this independent differential proof. */
#define FA18_SCENE_SOURCE_HEADER "../../build/recomp/native_record_dispatch_source.h"
#define FA18_PUBLICATION_HELPERS_ONLY
#include "native_context_publication_oracle.c"
#include "../../port/native_record_view.c"
#include "../../port/scene_component_magnitude.c"
#include "../../port/native_record_range.c"
#include "../../port/native_record_dispatch.c"
#include "../../port/native_vector_math.c"
#include "native_vector_math_fixture.h"
static unsigned view_case,source_norms,source_faults;
static int original_view(void) {
    unsigned step,i;
    for(step=0;step<5000;++step) {
        uint32_t pc=REG_PC; uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(pc==0xc2574a) ++source_norms;
        if(pc==0xc2578a && !REG_D[0] && (int32_t)REG_D[1]>=0) return 2;
        if(pc==0xc06c02) {
            if(rd_u16(ERROR_CODE)!=0x34 && rd_u16(ERROR_CODE)!=0x35) return 0;
            ++source_faults; /* Execute the sealed release RTS below. */
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
enum { DISPATCH_CONTROL_TABLE=0xc45c72, DISPATCH_CELL_ONLY=0xc45788, DISPATCH_TRACK_ENABLE=0xc4578e,
 DISPATCH_SEQUENCE_PHASE=0xc4582a, DISPATCH_SEQUENCE_STEP=0xc4582c, DISPATCH_DETAIL_CLEAR=0xc457d9,
 DISPATCH_CONTROL_CHOICE=0xc4588c };
static void dispatch_fixture(unsigned scenario,unsigned slot,unsigned companion,unsigned entry) {
    uint32_t r=CONTROL_RECORDS+512*slot; unsigned i,route=scenario%32;
    static const uint16_t lives[]={0,1,2,3,4,5,11,12,19,20,49,50,51,239,240,241,0x7fff,0x8000,0xffff};
    static const uint16_t flags[]={0,64,66,74,0x440,0x8040,0x1040,0x1048,0x1000,0x10c0};
    static const uint8_t kinds[]={0,1,0x10,0x14,0x15,0x20,0x30,0x31,0x40};
    /* Explicit bounded test data for signed control choices, including negative rows. */
    for(i=0;i<16384;++i) wr_u8(DISPATCH_CONTROL_TABLE-8192+i,(uint8_t)random_value());
    for(i=0;i<256;++i) {
        uint32_t at=DISPATCH_CONTROL_TABLE-8192+64*i;
        wr_u16(at+48,(uint16_t)(scenario%3)); wr_u16(at+50,(uint16_t)(scenario%5));
        wr_u32(at,(scenario%7==0?0x80000000u:scenario*256u));
        wr_u32(at+4,scenario*128u); wr_u32(at+8,scenario*64u);
    }
    view_fixture(scenario,slot,companion);
    if(entry==5 || entry==7) { wr_u16(VIEW_RECORD,(uint16_t)(companion*512)); return; }
    wr_u8(POST_INPUT_EVENT,route==0); wr_u8(DISPATCH_CELL_ONLY,route==1);
    wr_u8(DISPATCH_TRACK_ENABLE,(scenario/32)%3!=0); wr_u8(DISPATCH_SEQUENCE_PHASE,route%4==0?1:0);
    wr_u16(VIEW_RECORD,(uint16_t)(companion*512));
    wr_u16(TARGET_RECORD,(uint16_t)(scenario%3==0?slot:companion));
    wr_u8(DISPATCH_CONTROL_CHOICE,scenario%3==0?0:(uint8_t)scenario);
    wr_u8(DISPATCH_CONTROL_CHOICE+1,scenario%3==0?0:(uint8_t)(scenario/3));
    wr_u16(r,flags[scenario/32%10]); wr_u8(r+0x62,kinds[scenario%9]);
    wr_u16(r+0x4c,lives[scenario/9%19]); wr_u8(r+0x38,(uint8_t)(route%3==0?255:companion));
    wr_u16(r+2,route%4==0?0x100:route%4==1?0x80:route%4==2?1:0);
    wr_u8(r+0x20,route%7==0?2:0); wr_u8(r+0x64,(uint8_t)(scenario%4*32));
    if(entry==1) { wr_u8(r+0x62,0); REG_D[0]=0; }
    if(!(rd_u8(r+0x62)&0xf0)) wr_u8(r+0x38,(uint8_t)companion);
    if(entry==2) REG_D[1]=(REG_D[1]&0xffffff00u)|(uint8_t)scenario;
    /* Force local range combinations without altering the source table. */
    if(entry==3 || (scenario%5==0 && entry==0)) {
        wr_u16(r+6,0); wr_u16(r+8,0); wr_u16(r+0xc,0); wr_u16(r+0xe,0); wr_u32(r+0x10,0);
        wr_u16(r+0x2c,route==2?0xffff:route==3?3:0); wr_u16(r+0x2e,route==4?3:0);
        wr_u16(r+0x30,(uint16_t)(scenario%31*256)); wr_u16(r+0x32,(uint16_t)(scenario%23*128));
        wr_u32(r+0x34,(uint32_t)(scenario%37*96));
    }
    if(entry>=6 && entry<=9 && entry!=7) {
        static const uint8_t counts[]={0,1,9,10,0x7f,0x80,0xff};
        wr_u8(KEY_TAKEN,scenario&256?1:0); wr_u8(KEY_COUNT,counts[scenario/512%7]);
        wr_u8(KEY_WRITE,(uint8_t)(scenario/32)); wr_u8(KEY_TRANSLATED_WRITE,(uint8_t)(scenario/16));
        wr_u8(CONTEXT_SELECT,scenario%3==0?1:0);
        wr_u8(CONTEXT_PUBLISH_RETURN_MODE,(uint8_t)random_value());
        wr_u8(r+0x62,scenario%3==1?0x30:0x20); wr_u8(VIEW_MODE,(uint8_t)(scenario/7));
        wr_u8(REDRAW_KEEP_STATE,(uint8_t)(scenario&1)); REG_D[1]=slot;
    }
    wr_u8(0xc457b5,(uint8_t)(scenario&1));
    if(entry==3) {
        static const uint16_t ranges[]={0,0x480,0x900,0x7fff,0x8000};
        wr_u16(r+0x4a,ranges[scenario/32%5]); wr_u8(r+0x39,(uint8_t)(scenario%16*16));
    }
    if(entry==4) { wr_u8(KEY_COUNT,(uint8_t)(scenario%12)); wr_u8(KEY_TAKEN,scenario%3==0); }
    for(i=0;i<16;++i) if(i!=slot) wr_u8(CONTROL_RECORDS+512*i+0x62,0x20);

    if(entry==0 && scenario%64>=1 && scenario%64<=23) {
        unsigned arm=scenario%64; static const uint16_t speeds[]={0,0xff,0x100,0x1180,0x1190,0x1201,0x1800};
        static const uint16_t distances[]={0,0x17f,0x180,0x240,0x241,0x300,0x301,0x360,0x361,0x5ff,0x600,0x900};
        uint16_t d=distances[(arm-8)%12];
        wr_u8(POST_INPUT_EVENT,0); wr_u8(DISPATCH_TRACK_ENABLE,1);
        wr_u16(r,0x1040); wr_u16(r+2,arm<8 && (scenario/128)%2?0x80:0);
        wr_u8(r+0x62,arm<8?0x14:0x10); wr_u8(r+0x38,arm<8?255:(uint8_t)companion);
        wr_u8(r+0x20,0); wr_u8(r+4,arm%2?0:0x20); wr_u8(r+5,arm<8?6:(uint8_t)((arm/2)%4));
        wr_u16(r+0x6c,arm<8?speeds[arm%7]:0x900); wr_u16(r+0x4a,d);
        wr_u8(r+0x64,(uint8_t)(scenario/64%4*32)); wr_u8(r+0x39,0);
        wr_u16(CONTROL_RECORDS+512*companion,0x40);
        wr_u16(CONTROL_RECORDS+512*companion+0x6c,scenario/64%2?0x3000:0x900);
        wr_u16(r+6,0); wr_u16(r+8,0); wr_u16(r+0xc,0); wr_u16(r+0xe,0); wr_u32(r+0x10,arm<8?scenario/64%3*0x6c0:0);
        for(i=0;i<4;++i) wr_u16(r+0x2c+2*i,0);
        wr_u32(r+0x34,arm<8?0:d);
        if(arm<8) wr_u8(r+0x7c,scenario/64%2?0x80:0);
        if(arm==17 || arm==19) wr_u8(r+5,arm==17?6:2);
        if(arm==23) { wr_u8(r+0x20,2); wr_u16(r+0x6c,scenario/64%2?0x1190:0x1201); }
    }

    if(entry==0 && scenario%64>=24 && scenario%64<=27) {
        static const uint16_t life[]={0xffff,3,4,20};
        wr_u8(POST_INPUT_EVENT,0); wr_u8(DISPATCH_CELL_ONLY,0); wr_u8(0xc457b5,1);
        wr_u16(TARGET_RECORD,(uint16_t)companion); wr_u16(r,0x40); wr_u8(r+0x62,0x30);
        wr_u16(r+0x4c,life[scenario%64-24]); wr_u32(r+0x18,0);
        if(slot) wr_u16(CONTROL_RECORDS,0);
    }
    if((entry==1 && scenario%64>=24 && scenario%64<=29) || entry==10) {
        wr_u16(r,scenario%3==0?0x80:0x40); wr_u16(r+0x4c,100);
        wr_u8(DISPATCH_CELL_ONLY,0); wr_u8(POST_INPUT_EVENT,0); wr_u8(r+0x38,0);
        wr_u8(r+0x62,scenario%64%2?1:0);
        wr_u8(DISPATCH_CONTROL_CHOICE,scenario%64%3?1:0);
        wr_u8(DISPATCH_CONTROL_CHOICE+1,scenario%64%3?1:0);
    }
    if(entry==2 && scenario%64<6) {
        REG_D[1]=(REG_D[1]&0xffffff00u)|1;
        wr_u16(DISPATCH_CONTROL_TABLE+48,0); wr_u16(DISPATCH_CONTROL_TABLE+50,0);
        for(i=0;i<3;++i) { wr_u32(DISPATCH_CONTROL_TABLE+4*i,0); wr_u32(r+0x14+4*i,0); }
        if(scenario%64==1) wr_u32(r+0x1c,0x5000);
        if(scenario%64==2) wr_u32(r+0x1c,0xffffb000);
    }
    if(entry==3 && scenario%64<3) {
        wr_u16(r+0x4a,0); wr_u8(r+0x39,0);
        for(i=0;i<4;++i) { wr_u16(r+0x2c+2*i,0); }
        wr_u16(r+6,0); wr_u16(r+8,0); wr_u16(r+0xc,0); wr_u16(r+0xe,0);
        wr_u32(r+0x10,scenario%64==0?0x80000000u:0);
        wr_u32(r+0x34,scenario%64==0?0x7fffffffu:0x1000);
        if(scenario%64==1) for(i=0;i<FA18_SCENE_MAGNITUDE_WORDS;++i) wr_u16(MAGNITUDE_TABLE+2*i,0xffff);
    }

}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0; char error[256];
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE); SceneState *native=malloc(sizeof *native);
    uint8_t parameters[65536],status[1024],list[32],zone[32],table_bytes[65536],controls[16384];
    PortFieldWindow table={.bytes=table_bytes,.byte_count=sizeof table_bytes,.origin=32768};
    PortFieldByte *control_fields=calloc(16384,sizeof *control_fields);
    PortFieldWindow control_window={.bytes=controls,.before=control_fields,.before_count=8192,.after=control_fields+8192,.after_count=8192};
    uint16_t stride,slot_word,tick,error_word,magnitude,marker,target;
    uint8_t event,mode,limit,pending,flag,created,admitted,redraw,cell,track,step,clear,choices[2];
    int16_t normalized[3]; FA18NativeRecordViewAssets assets; PortFieldWindow zone_window;
    FA18NativeRecordView view;
    FA18NativeRecordViewWork work; FA18NativeRecordRange range; FA18NativeContextPublication publication;
    FA18NativeVectorMath math; FA18NativeRecordDispatch dispatch; FA18ViewSpanOffsets spans;
    static const uint32_t entries[]={0xc23a7e,0xc23f4a,0xc24458,0xc24568,0xc1b7a6,0xc23ca6,0xc1bee8,0xc23ff8,0xc1ba86,0xc1b906,0xc243f2,0xc2574a,0xc25754};
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):8192,i,j,entry;
    if(!state || !rom || !m || !base || !before || !expected || !native || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(entry=argc>2?(unsigned)strtoul(argv[2],NULL,10):0;entry<(argc>2?(unsigned)strtoul(argv[2],NULL,10)+1:13);++entry) for(view_case=0;view_case<cases;++view_case) {
        unsigned slot=(view_case/256)%16,viewer=(slot+1+(view_case/32)%15)%16;
        uint32_t axis,source_event,output_event,source_companion; uint8_t choice; int decision=-1,source_decision,ok,source_result; uint32_t scale_word=0; int32_t vector[3];
        selected_entry=entries[entry]; memcpy(m,base,sizeof *m); scene_fixture(view_case);
        dispatch_fixture(view_case,slot,viewer,entry);
        if(entry>=11) vector_fixture(view_case,entry-11,&scale_word,vector);
        REG_A[1]=CONTROL_RECORDS+slot*512; REG_A[2]=CONTROL_RECORDS+viewer*512; REG_A[3]=REG_A[2];
        for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
            for(j=0;j<scene_source_bytes[i].length;++j)
                if(rd_u8(scene_source_bytes[i].pc+j)!=scene_source_bytes[i].bytes[j]) {
                    fprintf(stderr,"fixture altered source %06X\n",scene_source_bytes[i].pc+j); return 1;
                }
        if(!load_scene(native) || !verify_record_owners(native)) return 1;
        load_publication(native);
        for(i=0;i<sizeof parameters;++i) parameters[i]=rd_u8(VIEW_TABLE-32768+i);
        for(i=0;i<sizeof status;++i) status[i]=rd_u8(STATUS_TABLE-512+i);
        for(i=0;i<sizeof table_bytes;++i) table_bytes[i]=rd_u8(MAGNITUDE_TABLE-32768+i);
        for(i=0;i<sizeof controls;++i) controls[i]=rd_u8(DISPATCH_CONTROL_TABLE-8192+i);
        for(i=0;i<sizeof list;++i) { list[i]=rd_u8(LIST_BASE+i); zone[i]=rd_u8(ZONE_BASE+i); }
        for(i=0;i<256;++i) spans.values[i]=rd_s8(0xc1bad4u-128+i);
        zone_window=(PortFieldWindow){.bytes=zone,.byte_count=sizeof zone};
        assets=(FA18NativeRecordViewAssets){
            .parameters={.bytes=parameters,.byte_count=sizeof parameters,.origin=32768},
            .status={.bytes=status,.byte_count=sizeof status,.origin=512},
            .primary_list={.bytes=list,.byte_count=sizeof list},.zones=&zone_window,.zone_count=1};
        stride=rd_u16(SCRIPT_RECORD); slot_word=rd_u16(STREAM_MODE); tick=rd_u16(STREAM_SKIP); error_word=rd_u16(ERROR_CODE);
        event=rd_u8(POST_INPUT_EVENT); mode=rd_u8(MODE_SELECT); limit=rd_u8(SCENE_DISPATCH_LIMIT);
        pending=rd_u8(FIRE_RECORD_PENDING); flag=rd_u8(RECORD_VIEW_FLAG);
        created=rd_u8(SCENE_DISPATCH_CREATED); admitted=rd_u8(SCENE_DISPATCH_ADMITTED);
        magnitude=rd_u16(MAGNITUDE); redraw=rd_u8(BAR_REDRAWS_F); marker=rd_u16(SELECTION_MARKER); target=rd_u16(TARGET_RECORD);
        step=rd_u8(DISPATCH_SEQUENCE_STEP);
        choices[0]=rd_u8(DISPATCH_CONTROL_CHOICE); choices[1]=rd_u8(DISPATCH_CONTROL_CHOICE+1);
        if(!fa18_bind_command_queue_byte(&native->queue,128+DISPATCH_CELL_ONLY-KEY_RAW,&cell) ||
           !fa18_bind_command_queue_byte(&native->queue,128+DISPATCH_TRACK_ENABLE-KEY_RAW,&track) ||
           !fa18_bind_command_queue_byte(&native->queue,128+DISPATCH_DETAIL_CLEAR-KEY_RAW,&clear) ||
           !fa18_bind_command_queue_byte(&native->queue,128+DISPATCH_SEQUENCE_PHASE-KEY_RAW,&native->flight.sequence_phase) ||
           !fa18_bind_command_queue_byte(&native->queue,128+DISPATCH_SEQUENCE_STEP-KEY_RAW,&step) ||
           !fa18_bind_command_queue_byte(&native->queue,128+FIRE_RECORD_PENDING-KEY_RAW,&pending) ||
           !fa18_bind_command_queue_byte(&native->queue,128+RECORD_VIEW_FLAG-KEY_RAW,&flag) ||
           !fa18_bind_command_queue_byte(&native->queue,128+BAR_REDRAWS_F-KEY_RAW,&redraw)) return 1;
        for(i=0;i<3;++i) normalized[i]=rd_s16(NORMALIZED+2*i);
        math=(FA18NativeVectorMath){&table,&magnitude,normalized};
        view=(FA18NativeRecordView){.records=&native->bank,.assets=&assets,.vector_math=&math,
            .selected_record=&native->selected,.current_stride=&stride,.current_slot=&slot_word,.tick_word=&tick,.error_word=&error_word,
            .post_input_event=&native->commands.indexed.origin_gate_a,.mode=&mode,.limit=&limit,.pending=&pending,.view_flag=&flag,.created=&created,.admitted=&admitted,.normalized=normalized};
        (void)event;
        work=(FA18NativeRecordViewWork){native->bank.records+viewer,REG_D[4],native->bank.records+viewer};
        range=(FA18NativeRecordRange){.records=&native->bank,.table=&table,.selected_record=&native->selected,
            .current_stride=&stride,.magnitude=&magnitude,.bar_redraw_f=&redraw};
        publication=(FA18NativeContextPublication){&native->bank,&native->context,&native->queue,&spans,&marker,&target};
        for(i=0;i<16384;++i) {
            uint32_t a=DISPATCH_CONTROL_TABLE-8192+i;
            control_fields[i]=(PortFieldByte){.byte=controls+i};
            if(a>=CONTROL_RECORDS && a<CONTROL_RECORDS+8192) {
                unsigned n=(a-CONTROL_RECORDS)/512, at=(a-CONTROL_RECORDS)%512;
                control_fields[i]=at<FA18_NATIVE_SCENE_MAPPED_BYTES?native->bank.records[n].fields[at]:
                    (PortFieldByte){.byte=native->bank.records[n].unported+at};
            } else if(a>=KEY_RAW-128 && a<KEY_RAW-128+FA18_COMMAND_QUEUE_NEIGHBORS)
                control_fields[i]=native->queue.slots[a-KEY_RAW+128];
        }
        dispatch=(FA18NativeRecordDispatch){.view=&view,.view_work=&work,.range=&range,.publication=&publication,
            .selected_controls=&control_window,.cell_only=&cell,.track_enable=&track,.sequence_phase=&native->flight.sequence_phase,
            .sequence_step=&step,.detail_clear=&clear,.scene_redraw=&native->view.update_mask,
            .control_choice={choices,choices+1},.target_slot=&target};
        choice=(uint8_t)REG_D[1]; output_event=REG_D[0]; memcpy(before,m,sizeof *m);
        source_result=original_view();
        if(!source_result) { fprintf(stderr,"source dispatch stopped %06X case %u\n",selected_entry,view_case); return 1; }
        source_companion=REG_A[2]; axis=REG_D[4]; source_event=REG_D[0]; source_decision=(uint8_t)REG_D[0];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        ok=entry==0?fa18_dispatch_native_record(&dispatch,slot,viewer,&decision):
            entry==1?fa18_dispatch_native_unclassified_record(&dispatch,slot,&decision):
            entry==2?fa18_select_native_control_target(&dispatch,slot,choice):
            entry==3?fa18_classify_native_view_range(&range,slot,&work.carried_axis):
            entry==4?fa18_publish_native_context_detail(&publication,output_event,&work.carried_axis,&output_event):
            entry==5?fa18_update_native_record_view(&view,slot,&work):
            entry==6?fa18_publish_native_context_record(&publication,output_event,(int16_t)slot,&work.carried_axis,&output_event):
            entry==7?fa18_link_native_record_view(&view,slot,&work):
            entry==8?fa18_publish_native_view_key(&publication,output_event,&work.carried_axis,&output_event):
            entry==9?fa18_publish_native_zero_view(&publication,output_event,&work.carried_axis,&output_event):
            entry==10?fa18_track_native_record_target(&dispatch,slot):
            entry==11?fa18_normalize_native_vector(&math,(int16_t)scale_word,vector,&work.carried_axis):
                fa18_normalize_native_vector_with_direction(&math,(int16_t)scale_word,(int16_t)(scale_word>>16),vector,&work.carried_axis);
        if(source_companion!=CONTROL_RECORDS+512*(unsigned)(work.companion-native->bank.records)) {
            fprintf(stderr,"companion %06X case %u expected %06X slot %u\n",selected_entry,view_case,source_companion,(unsigned)(work.companion-native->bank.records)); return 1;
        }
        if(ok!=(source_result==1) || work.carried_axis!=axis || (entry<2 && decision!=source_decision) || ((entry==4 || entry==6 || entry==8 || entry==9) && output_event!=source_event)) {
            fprintf(stderr,"dispatch %06X case %u ok %d decision %d/%d axis %08X/%08X\n",selected_entry,view_case,ok,source_decision,decision,axis,work.carried_axis); return 1;
        }
        store_publication(native); wr_u16(ERROR_CODE,error_word); wr_u8(FIRE_RECORD_PENDING,pending); wr_u8(RECORD_VIEW_FLAG,flag);
        wr_u8(SCENE_DISPATCH_CREATED,created); wr_u8(SCENE_DISPATCH_ADMITTED,admitted);
        wr_u16(MAGNITUDE,magnitude); wr_u8(BAR_REDRAWS_F,redraw); wr_u16(SELECTION_MARKER,marker); wr_u16(TARGET_RECORD,target);
        wr_u8(DISPATCH_SEQUENCE_PHASE,native->flight.sequence_phase); wr_u8(DISPATCH_SEQUENCE_STEP,step);
        for(i=0;i<3;++i) wr_u16(NORMALIZED+2*i,(uint16_t)normalized[i]);
        if(memcmp(m->chip,expected,FA18_CHIP_SIZE) || memcmp(m->slow,expected+FA18_CHIP_SIZE,0x7fd00) ||
           memcmp(m->slow+0x7ff00,expected+FA18_CHIP_SIZE+0x7ff00,FA18_SLOW_SIZE-0x7ff00)) {
            for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
                uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
                if(a>=0xc7fd00 && a<0xc7ff00) continue;
                if(rd_u8(a)!=expected[i]) {
                    fprintf(stderr,"dispatch %06X case %u byte %06X source %02X native %02X\n",selected_entry,view_case,a,expected[i],rd_u8(a)); return 1;
                }
            }
        }
        memcpy(m->chip,expected,FA18_CHIP_SIZE); memcpy(m->slow,expected+FA18_CHIP_SIZE,FA18_SLOW_SIZE);
        if(!verify_record_owners(native)) return 1;
    }
    printf("native record dispatch: %u calls match game RAM, independent typed records, carried axis and decisions; %u actual normalizations/%u actual release fault returns; no child contracts\n",cases*(argc>2?1:13),source_norms,source_faults);
    printf("visited:"); for(i=0;i<sizeof scene_source_bytes/sizeof scene_source_bytes[0];++i)
        if(scene_seen[i]) printf(" %06X",scene_source_bytes[i].pc);
    puts(""); free(native); free(expected); free(before); free(base); free(m); return 0;
}
