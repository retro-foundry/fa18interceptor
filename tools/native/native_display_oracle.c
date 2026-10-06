/* C1612C: original parent bytes versus resumable source composition.
 * External service contracts mutate the same globals on both paths. CPU and
 * exported RAM are validation inputs only; no native game service is replaced. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "input_display_setup.h"

enum { FRAME=0xc7fefc, MAX_EVENTS=1024 };
typedef struct {
    unsigned child,activity,clear,page,palette,view,list,local_page;
} Event;
static Event events[MAX_EVENTS];
static unsigned variant,ordinal,event_count,yields;
static int source_phase,waiting;
static int32_t display_child(void *context,enum InputDisplayChild child) {
    (void)context;
    Event event={0};
    event.child=child;event.activity=rd_u8(PLAYER_FLAGS_A);
    event.clear=rd_u8(TABLE_CLEAR_MODE);event.page=rd_u16(DRAW_PAGE);
    event.view=rd_u32(0xc1821c);event.list=rd_u32(0xc18232);
    event.local_page=rd_u16(FRAME-2);
    if(child==IDS_LOAD_STATIC_PALETTE) event.palette=0xc084d0;
    if(child==IDS_LOAD_DYNAMIC_PALETTE || child==IDS_LOAD_CLEAR_PALETTE)
        event.palette=rd_u32(LONG_TABLE);
    if(source_phase && event.palette &&
       (rd_u32(REG_A[7]+8)!=event.palette || rd_u32(REG_A[7]+12)!=32)) abort();
    if(ordinal>=MAX_EVENTS) abort();
    if(source_phase) events[event_count++]=event;
    else if(ordinal>=event_count || memcmp(&event,&events[ordinal],sizeof event)) {
        fprintf(stderr,"display case %u child %u (%u) boundary mismatch\n",variant,ordinal,child);abort();
    }
    ++ordinal;
    /* These model changes at service completion, including values the
     * original explicitly reloads after waits. They are test contracts. */
    if(child==IDS_WAIT_PUBLICATION && (variant&8)) wr_u16(DRAW_PAGE,(variant>>1)&1);
    if(child==IDS_LOAD_VIEW && (variant&16)) wr_u8(PLAYER_FLAGS_A,3);
    if(child==IDS_WAIT_BLIT && (variant&32)) wr_u8(PLAYER_FLAGS_A,0x80);
    if(child==IDS_LOAD_STATIC_PALETTE) wr_u32(LONG_TABLE,0x3100);
    if(child==IDS_WAIT_DYNAMIC_SECOND && ordinal<16 && (variant&64)) wr_u8(PLAYER_FLAGS_A,2);
    if(child==IDS_WAIT_CLEAR_PALETTE) wr_u32(LONG_TABLE,0x3140);
    if(child==IDS_LOAD_CLEAR_PALETTE || child==IDS_WAIT_DYNAMIC_SECOND)
        wr_u16(DRAW_PAGE,(variant>>2)&1);
    return 0;
}
static int await_child(void *context,enum InputDisplayChild child) {
    (void)context;(void)child;
    if(!waiting) {waiting=1;++yields;return 0;}
    waiting=0;return 1;
}
static int original_display(void) {
    static const gaddr returns[]={0xc1613c,0xc1617c,0xc16194,0xc1619c,
        0xc161be,0xc161ce,0xc161dc,0xc161f4,0xc16204,0xc16212,0xc16254,0xc1626c};
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;wr_u32(REG_A[7],0xc70000);
    m68k_set_reg(M68K_REG_SR,0x2700);REG_PC=0xc1612c;
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<200000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(REG_PC==0xc53f88 || REG_PC==0xc53f30 || REG_PC==0xc53f44 || REG_PC==0xc53ec0) {
            gaddr ret=rd_u32(REG_A[7]);unsigned i;
            for(i=0;i<12 && returns[i]!=ret;++i) {}
            if(i==12) abort();
            display_child(NULL,(enum InputDisplayChild)(IDS_WAIT_PUBLICATION+i));
            REG_PC=ret;REG_A[7]+=4;continue;
        }
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"original display failed at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state || !rom || !data || nd!=0x100000 || !m || !before || !expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) {fputs(error,stderr);return 1;}
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    unsigned total_events=0,total_yields=0;
    for(variant=0;variant<128;++variant) {
        static const uint8_t counts[]={0,1,2,7,127,128,255,129};
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        wr_u8(PLAYER_FLAGS_A,counts[variant&7]);wr_u8(TABLE_CLEAR_MODE,(uint8_t)(variant>>3));
        wr_u16(0xc458d2,variant&4?0x100:0);wr_u16(DRAW_PAGE,variant&1);
        wr_u32(0xc182ba,0x6000);wr_u32(0xc182be,0x6100);
        wr_u32(0xc182c2,0x6200);wr_u32(0xc182c6,0x6300);
        memcpy(before,m,sizeof *m);source_phase=1;ordinal=event_count=0;
        if(!original_display()) return 1;
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);source_phase=0;ordinal=yields=0;waiting=0;
        OuterDisplayState continuation={0};const InputDisplayHooks hooks={display_child,NULL,NULL};
        while(!advance_outer_display(FRAME,&hooks,&continuation,await_child)) {
            if(yields>2*MAX_EVENTS) return 1;
            /* A second poll before service completion may not run a child. */
            unsigned old=ordinal;
            waiting=0;
            if(advance_outer_display(FRAME,&hooks,&continuation,await_child) || ordinal!=old) return 1;
        }
        if(ordinal!=event_count || waiting || !yields) return 1;
        for(unsigned i=0;i<0xff000;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"display case %u %06X: source %02X native %02X\n",variant,
                    i<0x80000?i:i-0x80000+0xc00000,expected[i],actual);return 1;
            }
        }
        total_events+=event_count;total_yields+=yields;
    }
    printf("128 resumable display cases match original child order, publication/palette boundaries and non-stack RAM (%u children, %u suspended polls)\n",total_events,total_yields);
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
