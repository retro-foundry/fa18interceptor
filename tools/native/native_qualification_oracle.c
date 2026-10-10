/* Validate actual native qualification startup/briefing composition. */
/* Separate runtime translation units use overlapping phase enum spellings. */
#define MC_BYTE_TEST COLD_BYTE_TEST
#define MC_BYTE_STORE COLD_BYTE_STORE
#define MC_WORD_STORE COLD_WORD_STORE
#define MC_CALLBACK COLD_CALLBACK
#define MC_QUEUE_NEXT COLD_QUEUE_NEXT
#include "menu_cold.h"
#undef MC_BYTE_TEST
#undef MC_BYTE_STORE
#undef MC_WORD_STORE
#undef MC_CALLBACK
#undef MC_QUEUE_NEXT
#define MH_DIVIDE HEADING_DIVIDE
#include "target_heading.h"
#undef MH_DIVIDE
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "../../../fa18-interceptor-decomp/port/game/native/flight.c"
#include "../../../fa18-interceptor-decomp/port/game/native/setup.c"

/* These runtime boundaries are outside the parent cases below. A reached
 * boundary fails the test; none supplies substitute game behavior. */
void native_frontend_clear_text(void) { abort(); }
#ifndef FA18_QUALIFICATION_SAVE_BACKEND
void native_frontend_save_log(NativeFrontend *game) { (void)game;abort(); }
#endif
void native_input_process(NativeFrontend *game) { (void)game;abort(); }
uint32_t native_scene_project(void) { abort(); }
int native_scene_draw(NativeFrontend *game) { (void)game;abort(); }
NativeInputReturn native_hud_draw(uint16_t tick) { (void)tick;abort(); }
NativeInputReturn native_frame_selection_cleanup(NativeInputReturn prior) { (void)prior;abort(); }
NativeInputReturn native_frame_debug_overlay(NativeInputReturn prior) { (void)prior;abort(); }
NativeInputReturn native_frame_scene_labels(NativeInputReturn prior) { (void)prior;abort(); }
NativeInputReturn native_frame_grid_and_markers(NativeInputReturn prior) { (void)prior;abort(); }

static int original_parent(gaddr entry) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;
    wr_u32(REG_A[7],0xc70000);REG_PC=entry;
    /* These callbacks inherit C0EFD4's valid frame. C1C860 writes/reads its
     * -$2C flag there; A6=0 would discard that write as unmapped test memory. */
    REG_A[6]=0xc7ff70;
    m68k_set_reg(M68K_REG_SR,0x2700);
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned steps=0;steps<2000000;++steps) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(REG_PC==0xc53c78) {
            native_clock_request();REG_PC=rd_u32(REG_A[7]);REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"qualification source did not return at %06X\n",REG_PC);return 0;
}
#ifndef FA18_QUALIFICATION_ORACLE_LIBRARY
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) { fputs(error,stderr);return 1; }
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    native_clock_set(7000);
    const gaddr entries[]={0xc0fece,0xc0fb70,0xc0fbb6,0xc10c08,0xc09e06};
    for(unsigned test=0;test<20;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        unsigned owner=test%5,variant=test/5;gaddr entry=entries[owner];
        wr_u8(MODE_SELECT,9);wr_u16(POST_INPUT_COUNTDOWN,variant==0?2:0xffff);
        wr_u8(SEQUENCE_FLAG,variant&1);wr_u8(KEY_TAKEN,variant&2);
        if(owner==4) {
            wr_u8(PLAYER_PHASE,0);wr_u8(SEQUENCE_PHASE,0);
            wr_u8(CONTROL_RECORDS+1,rd_u8(CONTROL_RECORDS+1)|0x40);
            wr_u16(CONTROL_RECORDS+2,0xc080);
            wr_u16(CONTROL_RECORDS+0x6e,variant==3?1:0);
        }
        memcpy(before,m,sizeof *m);
        if(!original_parent(entry)) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);NativeFrontend game={0};
        if(owner==4) {
            const PostflightScheduleHooks hooks={schedule_child,NULL,NULL};
            schedule_postflight(POSTFLIGHT_DISPATCH,0,CONTROL_RECORDS,&hooks);
        } else stage(&game,entry);
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"qualification case %u parent %06X address %06X: source %02X native %02X\n",
                    test,entry,i<0x80000?i:0xc00000+i-0x80000,expected[i],actual);return 1;
            }
        }
    }
    puts("20 native qualification startup, briefing, context and landing-schedule cases match original non-stack RAM");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
#endif
