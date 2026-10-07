/* Native demo startup parents; executable bytes are validation-only. */
#define FA18_QUALIFICATION_ORACLE_LIBRARY
#include "native_qualification_oracle.c"
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns),*rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;native_clock_set(5000);
    for(unsigned test=0;test<32;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        /* Cold defaults must overwrite prior selectors. The saved level is
         * a word whose low byte is copied, including signed byte values. */
        wr_u16(rd_u32(MODE_TABLE)+2,(uint16_t)(0x1200u+test*17u));
        wr_u8(POSTFLIGHT_FAILURE_INPUT,(uint8_t)test);
        wr_u8(SCENE_DISPATCH_LIMIT,0x7f);wr_u8(SCENE_DISPATCH_LIMIT_PREVIOUS,0x80);
        wr_u8(VIEWPORT_MODE,2);wr_u8(VIEWPORT_TARGET,3);
        wr_u8(TABLE_CLEAR_MODE,2);wr_u8(COMMAND_EVENT_COUNTER,0x80);
        wr_u8(CONTEXT_GATE,0);wr_u8(0xc457d4u,0);wr_u16(0xc50412u,123);
        memcpy(before,m,sizeof *m);
        if(!original_parent(test&1?0xc08eb8:0xc08ee4)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        if(test&1) load_saved_scene_level(); else initialize_scene_startup_defaults();
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"cold scene case %u at %06X: source %02X native %02X\n",
                    test,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 1;
            }
        }
    }
    const gaddr entries[]={0xc0fece,0xc0fa04,0xc0fa4c,0xc0fa80,0xc10c68};
    for(unsigned test=0;test<15;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        unsigned owner=test%5,variant=test/5;gaddr entry=entries[owner];
        wr_u8(MODE_SELECT,owner<2?127:3);wr_u8(RECORDER_MODE,3);
        wr_u8(PLAYER_PHASE,0);wr_u8(SEQUENCE_PHASE,0);
        wr_u16(POST_INPUT_COUNTDOWN,variant==0?2:0xffff);
        wr_u8(VIEWPORT_MODE,0);wr_u8(VIEWPORT_TARGET,variant==2?15:0);
        memcpy(before,m,sizeof *m);
        if(!original_parent(entry)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);NativeFrontend game={.screen=NATIVE_SCENE_SETUP};
        stage(&game,entry);
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"demo case %u parent %06X address %06X: source %02X native %02X\n",
                    test,entry,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 1;
            }
        }
    }
    for(unsigned test=0;test<16;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        gaddr entry=test<8?0xc2374c:0xc2377e,record=CONTROL_RECORDS+512;
        wr_u16(CHOSEN_RECORD,1);wr_u16(SCRIPT_RECORD,512);wr_u16(UPDATE_TICK,0);
        wr_u8(CONTEXT_SELECT,0);wr_u8(0xc45789,1);wr_u8(0xc458a7,0);
        wr_u8(0xc458b5,0);wr_u8(SPACE_COMMAND_LATCH,1);wr_u8(FIRE_RECORD_PENDING,0);
        wr_u32(WARNING_CAUSES,rd_u32(WARNING_CAUSES)|0x4000u);
        wr_u8(CONTROL_RECORDS+0x5f,test%4==0?0:0x24);
        wr_u8(CONTROL_RECORDS+0x63,test%4==2?0x3d:0x1d);
        memcpy(before,m,sizeof *m);
        memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;REG_A[6]=0xc7ff70;
        REG_A[1]=record;REG_A[2]=CONTROL_RECORDS;wr_u32(REG_A[7],0xc70000);REG_PC=entry;
        m68k_set_reg(M68K_REG_SR,0x2700);fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
        if(!original()) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);
        FlightActionState work={0};work.record=record;work.source=CONTROL_RECORDS;
        const FlightActionHooks actions={.consume_values=action_child};
        if(test<8) try_primary_flight_record_action(work,&actions);
        else try_flight_record_action(work,&actions);
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"demo launch %u parent %06X address %06X: source %02X native %02X\n",
                    test,entry,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 1;
            }
        }
    }
    puts("32 cold defaults/level parents, 15 native demo startup parents and 16 primary/secondary launch cases match original non-stack RAM");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
