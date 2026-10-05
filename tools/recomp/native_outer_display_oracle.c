/* Original CPU and packed addresses exist only in this validation executable. */
#define main publication_validation_main
#include "command_publication_oracle.c"
#undef main
#include "../../port/voice_program.c"
#include "../../port/audio_update.c"
#define signed_byte queue_signed_byte
#include "../../port/command_queue.c"
#undef signed_byte
#include "../../port/viewport_transition.c"
#include "../../port/input_callback.c"
#include "../../port/input_palette.c"
#include "../../port/outer_display.c"
#include "../../port/amiga/host_graphics.h"
#include "../../build/recomp/native_outer_display_source.h"

enum { PAIR_BASE=0xc182ae, STATIC_WORDS=0xc084d0, DYNAMIC_WORDS=0xc60600,
       STACK_FIRST=0xc7fd00, STACK_END=0xc7ff00, MAX_EVENTS=1024 };
typedef struct {
    FA18NativeInputDisplay display;
    FA18ViewportModeState viewport;
    FA18NativeOuterDisplay outer;
    uint8_t activity;
    uint16_t status,static_words[32],dynamic_words[2][32];
    FA18NativeInputDisplayPair pairs[8];
    AmigaRgb4CopperList objects[12];
    AmigaRgb4HardwareList hardware_lists[12];
    uint8_t instructions[12][36],hardware[12][24],colors[64];
    AmigaRgb4Palette color_map;
    FA18NativeInputPalette palette;
} NativeOuter;
typedef struct { unsigned kind; uint32_t address; uint16_t words[32]; uint8_t ram[4096]; } OuterEvent;
static OuterEvent outer_events[MAX_EVENTS];
static unsigned outer_visited[sizeof outer_source_bytes/sizeof outer_source_bytes[0]];
static unsigned outer_case,source_events,native_events,total_outer_events;
static int outer_real_palette;
static uint32_t object_address(unsigned i) { return 0xc65000+0x100*i; }
static void *object_pointer(NativeOuter *s,uint32_t a) {
    if(a<0xc65000 || a>=0xc65c00 || ((a-0xc65000)&255)) abort();
    return &s->objects[(a-0xc65000)/256];
}
static uint32_t object_value(NativeOuter *s,void *p) {
    unsigned i;
    for(i=0;i<12;++i) if(p==&s->objects[i]) return object_address(i);
    abort();
}
static uint32_t palette_value(NativeOuter *s,const uint16_t *p) {
    if(p==s->static_words) return STATIC_WORDS;
    if(p==s->dynamic_words[0]) return DYNAMIC_WORDS;
    if(p==s->dynamic_words[1]) return DYNAMIC_WORDS+64;
    abort();
}
static void store_outer(NativeOuter *s) {
    unsigned i;
    wr_u16(DRAW_PAGE,s->display.draw_page); wr_u8(0xc45899,s->activity);
    wr_u8(TABLE_CLEAR_MODE,s->viewport.state); wr_u16(0xc458d2,s->status);
    wr_u32(LONG_TABLE,palette_value(s,s->display.stable_palette));
    wr_u32(0xc1821c,object_value(s,s->display.saved_pair.view));
    wr_u32(0xc18232,object_value(s,s->display.saved_pair.display_list));
    for(i=0;i<8;++i) wr_u32(PAIR_BASE+4*i,object_value(s,s->pairs[i].view));
    for(i=6;i<8;++i) wr_u32(PAIR_BASE+4*i+8,object_value(s,s->pairs[i].display_list));
    for(i=0;i<32;++i) {
        wr_u16(STATIC_WORDS+2*i,s->static_words[i]);
        wr_u16(DYNAMIC_WORDS+2*i,s->dynamic_words[0][i]);
        wr_u16(DYNAMIC_WORDS+64+2*i,s->dynamic_words[1][i]);
    }
    if(outer_real_palette) {
        memcpy(fa18_machine->slow+0xc66100-FA18_SLOW_BASE,s->colors,64);
        for(i=0;i<12;++i) {
            memcpy(fa18_machine->slow+object_address(i)+0x40-FA18_SLOW_BASE,s->instructions[i],36);
            memcpy(fa18_machine->slow+object_address(i)+0x80-FA18_SLOW_BASE,s->hardware[i],24);
        }
    }
}
static void snapshot_outer(OuterEvent *e,unsigned kind,uint32_t address) {
    static const uint32_t ranges[][2]={
        {0xc1821c,4},{0xc18232,4},{PAIR_BASE,40},{LONG_TABLE,4},{DRAW_PAGE,2},
        {0xc45899,1},{TABLE_CLEAR_MODE,1},{0xc458d2,2},{STATIC_WORDS,64},
        {DYNAMIC_WORDS,128},{0xc66100,64},{0xc65000,3072}
    };
    unsigned i,j,at=0;
    memset(e,0,sizeof *e); e->kind=kind; e->address=address;
    if(kind>=9) for(i=0;i<32;++i) e->words[i]=rd_u16(address+2*i);
    for(i=0;i<sizeof ranges/sizeof ranges[0];++i)
        for(j=0;j<ranges[i][1];++j) e->ram[at++]=rd_u8(ranges[i][0]+j);
}
/* Synchronization contracts may change actual game owners, not frame locals.
 * The first activity wait can cancel/change its sign; final waits can restart
 * or wrap its decrement. Initial counts of 127 exercise the whole loop. */
static void outer_effect(NativeOuter *s,unsigned kind) {
    unsigned i;
    if(kind==FA18_DISPLAY_WAIT_PUBLICATION && (outer_case&8)) {
        uint16_t page=(uint16_t)((outer_case/16)%8-3);
        if(s) s->display.draw_page=page; else wr_u16(DRAW_PAGE,page);
    }
    if(kind==8 && (outer_case&32)) { /* LoadView */
        if(s) s->display.draw_page=0xffff; else wr_u16(DRAW_PAGE,0xffff);
    }
    if(kind==FA18_DISPLAY_WAIT_ACTIVITY && (outer_case&64)) {
        uint8_t count=(outer_case&128)?128:2;
        if(s) s->activity=count; else wr_u8(0xc45899,count);
    }
    if(kind==FA18_DISPLAY_WAIT_STATIC_SECOND && (outer_case&256)) {
        if(s) s->display.stable_palette=s->dynamic_words[1]; else wr_u32(LONG_TABLE,DYNAMIC_WORDS+64);
    }
    if(kind==FA18_DISPLAY_WAIT_DYNAMIC_SECOND && (outer_case&512) &&
       (s?native_events:source_events)<=10) {
        uint8_t count=(outer_case&1024)?0:128;
        if(s) s->activity=count; else wr_u8(0xc45899,count);
    }
    if(kind==FA18_DISPLAY_WAIT_CLEAR && (outer_case&256)) {
        if(s) s->display.stable_palette=s->dynamic_words[1]; else wr_u32(LONG_TABLE,DYNAMIC_WORDS+64);
    }
    if(kind==FA18_DISPLAY_WAIT_STATIC_FIRST && (outer_case&16)) {
        for(i=0;i<10;++i) {
            void *object=s?&s->objects[(i+2)%12]:NULL;
            if(s) {
                if(i<8) s->pairs[i].view=object;
                if(i>=2) s->pairs[i-2].display_list=object;
            } else wr_u32(PAIR_BASE+4*i,object_address((i+2)%12));
        }
    }
    if(kind==FA18_DISPLAY_WAIT_DYNAMIC_FIRST && (outer_case&4)) {
        if(s) s->viewport.state=255; else wr_u8(TABLE_CLEAR_MODE,255);
    }
}
static int check_boundary(NativeOuter *s,unsigned kind,uint32_t address) {
    OuterEvent actual;
    store_outer(s); snapshot_outer(&actual,kind,address);
    if(native_events>=source_events || memcmp(&actual,&outer_events[native_events],sizeof actual)) {
        fprintf(stderr,"outer case %u boundary %u differs (kind %u address %06X)\n",outer_case,native_events,kind,address); exit(1);
    }
    ++native_events; outer_effect(s,kind); return 1;
}
static int native_wait(void *context,FA18NativeDisplayWait phase) {
    return check_boundary(context,(unsigned)phase,0);
}
static int native_view(void *context,FA18NativeInputDisplay *display) {
    NativeOuter *s=context;
    if(display!=&s->display) abort();
    return check_boundary(s,8,0);
}
static int native_palette(void *context,FA18NativeDisplayPalettePhase phase,const uint16_t *words,size_t count) {
    NativeOuter *s=context;
    if(count!=32 || !check_boundary(s,9+(unsigned)phase,palette_value(s,words))) return 0;
    return !outer_real_palette || fa18_load_native_display_palette(&s->palette,words,count);
}
static unsigned source_kind(uint32_t pc,uint32_t ret) {
    if(pc==0xc53f30 && ret==0xc1617c) return 8;
    if(pc==0xc53f44 && ret==0xc1619c) return FA18_DISPLAY_WAIT_BLIT;
    if(pc==0xc53ec0) {
        if(ret==0xc161be) return 9+FA18_DISPLAY_PALETTE_STATIC;
        if(ret==0xc161f4) return 9+FA18_DISPLAY_PALETTE_DYNAMIC;
        if(ret==0xc1626c) return 9+FA18_DISPLAY_PALETTE_CLEAR;
    }
    if(pc==0xc53f88) {
        switch(ret) {
        case 0xc1613c: return FA18_DISPLAY_WAIT_PUBLICATION;
        case 0xc16194: return FA18_DISPLAY_WAIT_ACTIVITY;
        case 0xc161ce: return FA18_DISPLAY_WAIT_STATIC_FIRST;
        case 0xc161dc: return FA18_DISPLAY_WAIT_STATIC_SECOND;
        case 0xc16204: return FA18_DISPLAY_WAIT_DYNAMIC_FIRST;
        case 0xc16212: return FA18_DISPLAY_WAIT_DYNAMIC_SECOND;
        case 0xc16254: return FA18_DISPLAY_WAIT_CLEAR;
        }
    }
    abort();
}
static int original_outer(void) {
    unsigned step,i;
    for(step=0;step<100000;++step) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04)
            return (uint16_t)REG_D[1]==rd_u16(DRAW_PAGE) &&
                (uint16_t)REG_D[0]==(uint16_t)(1u-rd_u16(DRAW_PAGE)) && (REG_D[1]>>16)==0;
        if(pc==0xc53f88 || pc==0xc53f30 || pc==0xc53f44 || pc==0xc53ec0) {
            unsigned kind=source_kind(pc,rd_u32(REG_A[7]));
            uint32_t address=pc==0xc53ec0?rd_u32(REG_A[7]+8):0;
            if(source_events>=MAX_EVENTS || (pc!=0xc53f44 && rd_u32(REG_A[7]+4)!=(pc==0xc53f30?0xc18218u:0xc1822au)) ||
               (pc==0xc53ec0 && rd_u32(REG_A[7]+12)!=32)) return 0;
            snapshot_outer(&outer_events[source_events++],kind,address); outer_effect(NULL,kind);
            if(outer_real_palette && pc==0xc53ec0) {
                AmigaGuestBank banks[]={{0,FA18_CHIP_SIZE,0,fa18_machine->chip},
                    {FA18_SLOW_BASE,FA18_SLOW_SIZE,0,fa18_machine->slow}};
                static AmigaHostCompat host;
                host.memory=(AmigaGuestMemory){banks,2};
                if(!amiga_host_load_rgb4(&host,0xc1822a,address,32)) return 0;
            }
            REG_PC=m68ki_pull_32(); continue;
        }
        for(i=0;i<sizeof outer_source_bytes/sizeof outer_source_bytes[0];++i) if(outer_source_bytes[i].pc==pc) break;
        if(i==sizeof outer_source_bytes/sizeof outer_source_bytes[0]) return 0;
        outer_visited[i]=1; opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
static void outer_fixture(void) {
    static const uint8_t counts[]={0,1,2,3,0,128,255,0};
    static const uint8_t states[]={0,1,2,3,127,128,255};
    unsigned i;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    wr_u8(0xc45899,(outer_case%256==0)?127:counts[outer_case%8]);
    wr_u8(TABLE_CLEAR_MODE,states[(outer_case/8)%7]); wr_u16(0xc458d2,(uint16_t)((outer_case&2)?0x100:0));
    wr_u16(DRAW_PAGE,(uint16_t)((outer_case/16)%8-3));
    wr_u32(LONG_TABLE,(outer_case&2048)?STATIC_WORDS:DYNAMIC_WORDS);
    for(i=0;i<32;++i) {
        wr_u16(STATIC_WORDS+2*i,(uint16_t)random_value()); wr_u16(DYNAMIC_WORDS+2*i,(uint16_t)random_value());
        wr_u16(DYNAMIC_WORDS+64+2*i,(uint16_t)random_value());
    }
    for(i=0;i<10;++i) wr_u32(PAIR_BASE+4*i,object_address(i));
    wr_u32(0xc1821c,object_address(10)); wr_u32(0xc18232,object_address(11));
    if(outer_real_palette) {
        static const uint16_t regs[]={0x180,0x19e,0x1a0,0x1be,0x100,0x180};
        for(i=0;i<12;++i) {
            unsigned j; uint32_t a=object_address(i);
            wr_u32(a+12,a+0x40); wr_u32(a+20,a+0x80); wr_u16(a+28,6);
            for(j=0;j<6;++j) {
                wr_u16(a+0x40+6*j,(uint16_t)(j==3 && (outer_case&16))); wr_u16(a+0x42+6*j,regs[j]);
                wr_u16(a+0x44+6*j,(uint16_t)random_value()); wr_u32(a+0x80+4*j,random_value());
            }
        }
        wr_u32(0xc1822e,0xc66000); wr_u16(0xc66002,(uint16_t)((outer_case&1)?16:32)); wr_u32(0xc66004,0xc66100);
        for(i=0;i<32;++i) wr_u16(0xc66100+2*i,(uint16_t)random_value());
    }
    REG_A[7]=0xc7ff00; wr_u32(REG_A[7],0xc70000); REG_PC=0xc1612c;
    m68k_set_reg(M68K_REG_SR,0x2700|(outer_case&31)); SET_CYCLES(1000000000); fa18_next_event=INT64_MAX;
}
static void import_outer(NativeOuter *s) {
    unsigned i;
    memset(s,0,sizeof *s);
    s->activity=rd_u8(0xc45899); s->viewport.state=rd_u8(TABLE_CLEAR_MODE); s->status=rd_u16(0xc458d2);
    s->display.draw_page=rd_u16(DRAW_PAGE); s->display.pairs=s->pairs; s->display.pair_count=8; s->display.first_pair=-3;
    for(i=0;i<8;++i) s->pairs[i]=(FA18NativeInputDisplayPair){object_pointer(s,rd_u32(PAIR_BASE+4*i)),object_pointer(s,rd_u32(PAIR_BASE+4*i+8))};
    s->display.saved_pair=(FA18NativeInputDisplayPair){object_pointer(s,rd_u32(0xc1821c)),object_pointer(s,rd_u32(0xc18232))};
    for(i=0;i<32;++i) {
        s->static_words[i]=rd_u16(STATIC_WORDS+2*i); s->dynamic_words[0][i]=rd_u16(DYNAMIC_WORDS+2*i);
        s->dynamic_words[1][i]=rd_u16(DYNAMIC_WORDS+64+2*i);
    }
    s->display.stable_palette=rd_u32(LONG_TABLE)==STATIC_WORDS?s->static_words:s->dynamic_words[0];
    s->outer=(FA18NativeOuterDisplay){&s->display,&s->viewport,&s->activity,&s->status,s->static_words};
    if(outer_real_palette) {
        for(i=0;i<12;++i) {
            memcpy(s->instructions[i],fa18_machine->slow+object_address(i)+0x40-FA18_SLOW_BASE,36);
            memcpy(s->hardware[i],fa18_machine->slow+object_address(i)+0x80-FA18_SLOW_BASE,24);
            s->hardware_lists[i]=(AmigaRgb4HardwareList){s->hardware[i],24};
            s->objects[i]=(AmigaRgb4CopperList){s->instructions[i],36,6,amiga_rgb4_write_hardware,&s->hardware_lists[i]};
        }
        memcpy(s->colors,fa18_machine->slow+0xc66100-FA18_SLOW_BASE,64);
        s->color_map=(AmigaRgb4Palette){s->colors,(outer_case&1)?32:64};
        if(!fa18_bind_native_input_palette(&s->palette,&s->display,&s->color_map)) abort();
    }
}
static int same_outer_ram(const uint8_t *expected) {
    unsigned i;
    if(!memcmp(expected,fa18_machine->chip,FA18_CHIP_SIZE) &&
       !memcmp(expected+FA18_CHIP_SIZE,fa18_machine->slow,STACK_FIRST-FA18_SLOW_BASE) &&
       !memcmp(expected+FA18_CHIP_SIZE+STACK_END-FA18_SLOW_BASE,fa18_machine->slow+STACK_END-FA18_SLOW_BASE,
                 FA18_SLOW_SIZE-(STACK_END-FA18_SLOW_BASE))) return 1;
    for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
        uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
        uint8_t got=i<FA18_CHIP_SIZE?fa18_machine->chip[i]:fa18_machine->slow[i-FA18_CHIP_SIZE];
        if(a>=STACK_FIRST && a<STACK_END) continue;
        if(got!=expected[i]) { fprintf(stderr,"outer case %u RAM %06X original %02X native %02X\n",outer_case,a,expected[i],got); return 0; }
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size),*rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,i,j;
    char error[256];
    outer_real_palette=argc>2 && !strcmp(argv[2],"--real-palette");
    if(!state || !rom || !m || !base || !before || !expected || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof outer_source_bytes/sizeof outer_source_bytes[0];++i)
        for(j=0;j<outer_source_bytes[i].length;++j) if(rd_u8(outer_source_bytes[i].pc+j)!=outer_source_bytes[i].bytes[j]) return 1;
    for(outer_case=0;outer_case<cases;++outer_case) {
        NativeOuter native;
        FA18NativeOuterDisplayOps ops={native_wait,native_view,native_palette,&native};
        memcpy(m,base,sizeof *m); outer_fixture(); import_outer(&native);
        source_events=native_events=0; memcpy(before,m,sizeof *m);
        if(!original_outer()) { fprintf(stderr,"original outer case %u failed at %06X\n",outer_case,REG_PC); return 1; }
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(!fa18_synchronize_native_outer_display(&native.outer,&ops)) return 1;
        store_outer(&native);
        if(source_events!=native_events || !same_outer_ram(expected)) return 1;
        total_outer_events+=native_events;
    }
    printf("native outer display C1612C%s: %u original calls matched RAM outside CPU ABI stack, source toggle outputs and %u ordered service boundaries\n",outer_real_palette?" with actual RGB4":"",cases,total_outer_events);
    printf("visited:"); for(i=0;i<sizeof outer_visited/sizeof outer_visited[0];++i) if(outer_visited[i]) printf(" %06X",outer_source_bytes[i].pc); putchar('\n');
    free(expected); free(before); free(base); free(m); return 0;
}
