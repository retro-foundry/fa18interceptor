/* Exercise the exact native frame helpers against their original caller ranges. */
#define FA18_HUD_ORACLE_LIBRARY
#include "native_hud_oracle.c"
#define select_draw_page host_select_draw_page
#define draw_page_debug_mark host_draw_page_debug_mark
#include "../../port/game/render_page.c"
#include "../../port/game/native/frame_tail.c"

static int source_range(gaddr start,gaddr end) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[4]=rd_u16(LINE_LAST_ROW);
    REG_A[6]=0xc7ff80;REG_A[7]=0xc7ff00;
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=start;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<200000;++step) {
        if(REG_PC==end && REG_A[7]==0xc7ff00) {wait_blitter();return 1;}
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        int cycles=GET_CYCLES();
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
        fa18_machine->cycle+=cycles-GET_CYCLES();
    }
    fprintf(stderr,"source frame tail stopped at %06X\n",REG_PC);return 0;
}

int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    for(unsigned group=0;group<2;++group) {
        unsigned cases=group?256:64;
        for(unsigned test=0;test<cases;++test) {
            memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
            if(!group) {
                unsigned target=(test>>2)&15;
                wr_u16(TARGET_RECORD,(uint16_t)target);wr_u16(VIEW_RECORD,(uint16_t)(target<<9));
                wr_u16(CONTROL_RECORDS+(target<<9),test&1?0x40:0);
                wr_u8(CONTEXT_SELECT,test&2?1:0);wr_u8(UPDATE_MASK,0x24);
                wr_u8(VIEW_MODE,7);wr_u16(SPAN_ORIGIN,12);wr_u16(SPAN_ORIGIN_Y,192);
                wr_u8(KEY_TAKEN,0);wr_u8(KEY_COUNT,0);wr_u8(KEY_WRITE,0);wr_u8(KEY_TRANSLATED_WRITE,0);
            } else {
                const int16_t values[]={0,1,9,10,999,9999,32767,(int16_t)0x8000};
                wr_u8(UPDATE_TAIL_CONDITION,test&1?1:0);wr_u8(UPDATE_ACTIVE,test&2?1:0);
                wr_u8(0xc457b3,test&4?1:0);wr_u16(DRAW_PAGE,test&8?1:0);select_draw_page();
                wr_u16(0xc45ae6,(uint16_t)values[(test>>5)&7]);
                wr_u16(0xc45776,(uint16_t)values[(test>>4)&7]);
                wr_u16(0xc45778,(uint16_t)-values[(test>>4)&7]);
                wr_u16(LINE_LAST_ROW,test&16?9:199);
                wr_u16(UPDATE_STAGE_MARKER,0x7777);
            }
            memcpy(before,m,sizeof *m);
            if(group) native_frame_debug_overlay();else native_frame_selection_cleanup();
            memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
            memcpy(m,before,sizeof *m);
            if(!source_range(group?0xc0f386:0xc0f2dc,group?0xc0f3ba:0xc0f2f0)) return 1;
            unsigned differences=0;
            for(unsigned i=0;i<0xffc00;++i) {
                uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
                if(actual!=expected[i]) {
                    if(differences<8) fprintf(stderr,"frame-tail group %u case %u at %06X: source %02X native %02X\n",
                        group,test,i<0x80000?i:i-0x80000+0xc00000,actual,expected[i]);
                    ++differences;
                }
            }
            if(differences) return 1;
        }
    }
    puts("64 selection-cleanup and 256 gated-overlay cases match all original non-stack RAM");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
