/* Native callback versus original instructions; addresses exist only in validation. */
#define main reference_publication_validation_main
#include "command_publication_oracle.c"
#undef main
#include "../../port/voice_program.c"
#include "../../port/audio_update.c"
#define signed_byte queue_signed_byte
#include "../../port/command_queue.c"
#undef signed_byte
#include "../../port/viewport_transition.c"
#include "../../port/input_callback.c"
#include "../../build/recomp/native_input_callback_source.h"

enum { PALETTE_FIRST=0xc07710, PALETTE_WORDS=4112, PAIR_FIRST=0xc182ae,
       STACK_FIRST=0xc7fd00, STACK_END=0xc7ff00 };
typedef struct {
    FA18CommandInput commands;
    FA18CommandAudio audio;
    FA18ViewportModeState mode;
    FA18NativeInputCallbackState input;
    FA18NativeInputDisplay display;
    FA18ViewportTransitionOps ops;
    uint16_t palettes[PALETTE_WORDS],stable[16];
    const uint16_t *modes[256];
    FA18NativeInputDisplayPair pairs[8];
    int objects[12];
    uint32_t stable_address;
} NativeInput;
typedef struct { FA18ViewportPalettePhase phase; uint32_t address; uint16_t words[16]; } PaletteEvent;
static PaletteEvent events[2];
static uint8_t event_ram[2][FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static unsigned input_visited[sizeof input_source_bytes/sizeof input_source_bytes[0]];
static unsigned source_count,native_count,total_events,case_number;
static uint16_t sampled_pair;

static void *resolve_object(NativeInput *s,uint32_t address) {
    if(address<0xc65000 || address>=0xc65030 || ((address-0xc65000)&3)) abort();
    return &s->objects[(address-0xc65000)/4];
}
static uint32_t export_object(NativeInput *s,void *object) {
    unsigned i;
    for(i=0;i<12;++i) if(object==&s->objects[i]) return 0xc65000+4*i;
    abort();
}
static void store_input(NativeInput *s) {
    unsigned i;
    wr_u16(0xc1ac06,s->input.counter_x); wr_u16(0xc1ac08,s->input.counter_y);
    wr_u16(0xc45776,(uint16_t)s->input.mouse_x); wr_u16(0xc45778,(uint16_t)s->commands.indexed.throttle);
    wr_u16(0xc45774,(uint16_t)s->input.ticks); wr_u8(PLAYER_READY,s->commands.indexed.player_ready);
    wr_u8(VIEWPORT_MODE,s->mode.current); wr_u8(VIEWPORT_TARGET,s->mode.target);
    wr_u8(0xc458a3,s->mode.countdown); wr_u8(TABLE_CLEAR_MODE,s->mode.state);
    wr_u16(DRAW_PAGE,s->display.draw_page);
    wr_u32(0xc1821c,export_object(s,s->display.saved_pair.view));
    wr_u32(0xc18232,export_object(s,s->display.saved_pair.palette));
    wr_u32(MASTER_VOLUME,s->audio.master_volume); wr_u32(MASTER_VOLUME_TARGET,s->audio.master_volume_target);
    wr_u8(VOLUME_FADING,s->audio.volume_fading);
    for(i=0;i<8;++i) wr_u32(PAIR_FIRST+4*i,export_object(s,s->pairs[i].view));
    for(i=6;i<8;++i) wr_u32(PAIR_FIRST+4*i+8,export_object(s,s->pairs[i].palette));
    for(i=0;i<PALETTE_WORDS;++i) wr_u16(PALETTE_FIRST+2*i,s->palettes[i]);
    for(i=0;i<16;++i) wr_u16(s->stable_address+2*i,s->display.stable_palette[i]);
}
static int equal_input_ram(const uint8_t *expected) {
    unsigned i;
    if(!memcmp(expected,fa18_machine->chip,FA18_CHIP_SIZE) &&
       !memcmp(expected+FA18_CHIP_SIZE,fa18_machine->slow,STACK_FIRST-FA18_SLOW_BASE) &&
       !memcmp(expected+FA18_CHIP_SIZE+STACK_END-FA18_SLOW_BASE,
               fa18_machine->slow+STACK_END-FA18_SLOW_BASE,FA18_SLOW_SIZE-(STACK_END-FA18_SLOW_BASE))) return 1;
    for(i=0;i<FA18_CHIP_SIZE+FA18_SLOW_SIZE;++i) {
        uint32_t a=i<FA18_CHIP_SIZE?i:FA18_SLOW_BASE+i-FA18_CHIP_SIZE;
        uint8_t got=i<FA18_CHIP_SIZE?fa18_machine->chip[i]:fa18_machine->slow[i-FA18_CHIP_SIZE];
        if(a>=STACK_FIRST && a<STACK_END) continue;
        if(got!=expected[i]) { fprintf(stderr,"case %u RAM %06X original %02X native %02X\n",case_number,a,expected[i],got); return 0; }
    }
    return 0;
}
/* This required host-service contract only changes ordinary shared owners. */
static void palette_effect(NativeInput *s,FA18ViewportPalettePhase phase,unsigned ordinal) {
    uint16_t draw=(uint16_t)((case_number>>4)&1);
    unsigned i;
    if(case_number&512) draw=(uint16_t)((case_number>>4)%8-3);
    if(phase==FA18_PALETTE_SECOND) draw=(uint16_t)(draw^1);
    if(s) s->display.draw_page=draw; else wr_u16(DRAW_PAGE,draw);
    if((case_number&256) && phase==FA18_PALETTE_SECOND) {
        if(s) s->mode.current=s->mode.target; else wr_u8(VIEWPORT_MODE,rd_u8(VIEWPORT_TARGET));
    }
    if(case_number&1024) {
        uint8_t fading=(uint8_t)(ordinal+1);
        if(s) s->audio.volume_fading=fading; else wr_u8(VOLUME_FADING,fading);
    }
    if((case_number&128) && phase==FA18_PALETTE_SECOND) {
        if(s) for(i=0;i<8;++i) {
            s->pairs[i].view=&s->objects[(i+3)%12];
            s->pairs[i].palette=&s->objects[(i+5)%12];
        }
        else for(i=0;i<10;++i) wr_u32(PAIR_FIRST+4*i,0xc65000+4*((i+3)%12));
    }
    if((case_number&64) && phase==FA18_PALETTE_FIRST) {
        uint32_t address=events[ordinal].address;
        if(s) s->palettes[(address-PALETTE_FIRST)/2]^=0xabcd;
        else wr_u16(address,(uint16_t)(rd_u16(address)^0xabcd));
    }
}
static int palette_load(void *context,FA18ViewportPalettePhase phase,const uint16_t *words) {
    NativeInput *s=context;
    uint32_t address;
    unsigned i;
    if(words==s->display.stable_palette) address=s->stable_address;
    else {
        for(i=0;i<256;++i) if(words==s->modes[i]) break;
        if(i==256) abort();
        address=0xc08510+(15-(int)i+128)*32;
    }
    store_input(s);
    if(native_count>=source_count || events[native_count].phase!=phase ||
       events[native_count].address!=address || memcmp(events[native_count].words,words,32) ||
       !equal_input_ram(event_ram[native_count])) {
        fprintf(stderr,"case %u palette boundary %u differs (phase %u address %06X)\n",case_number,native_count,phase,address); exit(1);
    }
    palette_effect(s,phase,native_count++);
    return 1;
}
static void load_input(NativeInput *s) {
    unsigned i;
    memset(s,0,sizeof *s);
    s->input.commands=&s->commands; s->input.audio=&s->audio; s->input.viewport=&s->mode;
    s->input.counter_x=rd_u16(0xc1ac06); s->input.counter_y=rd_u16(0xc1ac08);
    s->input.mouse_x=signed_word(rd_u16(0xc45776)); s->commands.indexed.throttle=signed_word(rd_u16(0xc45778));
    s->input.ticks=signed_word(rd_u16(0xc45774)); s->commands.indexed.player_ready=rd_u8(PLAYER_READY);
    s->input.min_x=signed_word(rd_u16(0xc081ac)); s->input.min_y=signed_word(rd_u16(0xc081ae));
    s->input.max_x=signed_word(rd_u16(0xc081b0)); s->input.max_y=signed_word(rd_u16(0xc081b2));
    s->mode=(FA18ViewportModeState){rd_u8(VIEWPORT_MODE),rd_u8(VIEWPORT_TARGET),rd_u8(0xc458a3),rd_u8(TABLE_CLEAR_MODE)};
    s->audio.master_volume=rd_u32(MASTER_VOLUME); s->audio.master_volume_target=rd_u32(MASTER_VOLUME_TARGET);
    s->audio.volume_fading=rd_u8(VOLUME_FADING);
    for(i=0;i<PALETTE_WORDS;++i) s->palettes[i]=rd_u16(PALETTE_FIRST+2*i);
    for(i=0;i<256;++i) s->modes[i]=s->palettes+(255-i)*16;
    s->stable_address=rd_u32(LONG_TABLE);
    for(i=0;i<16;++i) s->stable[i]=rd_u16(s->stable_address+2*i);
    s->display.stable_palette=s->stable_address==0xc60600?s->stable:
        s->palettes+(s->stable_address-PALETTE_FIRST)/2;
    for(i=0;i<8;++i) s->pairs[i]=(FA18NativeInputDisplayPair){resolve_object(s,rd_u32(PAIR_FIRST+4*i)),resolve_object(s,rd_u32(PAIR_FIRST+4*i+8))};
    s->display.saved_pair=(FA18NativeInputDisplayPair){resolve_object(s,rd_u32(0xc1821c)),resolve_object(s,rd_u32(0xc18232))};
    s->display.pairs=s->pairs; s->display.pair_count=8; s->display.first_pair=-3;
    s->display.mode_palettes=s->modes; s->display.mode_count=256; s->display.first_mode=-128;
    s->display.draw_page=rd_u16(DRAW_PAGE); s->display.load_palette=palette_load; s->display.context=s;
    if(!fa18_prepare_native_input_display(&s->display,&s->ops)) abort();
}
static void input_fixture(unsigned scenario) {
    static const uint8_t bytes[]={0,1,2,3,15,0x7f,0x80,0xff};
    static const uint16_t previous[]={0,1,127,128,129,255,0x7fff,0x8000};
    static const uint16_t positions[]={0,1,127,128,0x7fff,0x8000,0xffff,0xff80};
    static const uint32_t levels[]={0,0x4000,0x1f0000,0x3f0000,0x400000,0xffff0000,0x80000000,0x80000010,0x7ffff000,0x7fffffff,0xffffffff,0x3effff};
    unsigned i;
    uint8_t mode=bytes[(scenario/2)%8],target=(scenario&1)?mode:bytes[(scenario/2+1)%8];
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    sampled_pair=(uint16_t)((unsigned)bytes[(scenario/8)%8]<<8|bytes[scenario%8]);
    fa18_machine->joy0dat=sampled_pair; fa18_machine->mouse_x=sampled_pair&255; fa18_machine->mouse_y=sampled_pair>>8;
    fa18_machine->mouse_dx=fa18_machine->mouse_dy=0;
    wr_u16(0xc1ac06,previous[(scenario/8)%8]); wr_u16(0xc1ac08,previous[scenario%8]);
    wr_u16(0xc45776,positions[(scenario/4)%8]); wr_u16(0xc45778,positions[(scenario/3)%8]);
    wr_u16(0xc45774,(uint16_t)random_value()); wr_u8(PLAYER_READY,(uint8_t)(scenario&1));
    wr_u16(0xc081ac,(scenario&2)?0x8000:0); wr_u16(0xc081b0,(scenario&4)?0xffff:0x7fff);
    wr_u16(0xc081ae,(scenario&8)?0x7fff:0xff80); wr_u16(0xc081b2,(scenario&16)?0x8000:127);
    wr_u8(VIEWPORT_MODE,mode); wr_u8(VIEWPORT_TARGET,target);
    wr_u8(0xc458a3,bytes[(scenario/16)%8]); wr_u8(TABLE_CLEAR_MODE,(uint8_t)(scenario&2));
    for(i=0;i<10;++i) wr_u32(PAIR_FIRST+4*i,0xc65000+4*i);
    wr_u32(0xc1821c,0xc65028); wr_u32(0xc18232,0xc6502c);
    wr_u16(DRAW_PAGE,(uint16_t)(scenario&1));
    wr_u32(LONG_TABLE,(scenario&2048)?0xc08510+(15-signed_byte(target))*32+2:0xc60600);
    wr_u8(VOLUME_FADING,(scenario&64)?0xff:0);
    wr_u32(MASTER_VOLUME,levels[scenario%12]); wr_u32(MASTER_VOLUME_TARGET,levels[(scenario/12)%12]);
    REG_A[7]=0xc7ff00; wr_u32(REG_A[7],0xc70000); REG_PC=0xc1718e;
    m68k_set_reg(M68K_REG_SR,0x2700|(scenario&31)); SET_CYCLES(1000000000); fa18_next_event=INT64_MAX;
}
static int original_input(void) {
    unsigned step,i;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return REG_D[0]==0;
        if(pc==0xc53ec0) {
            uint32_t ret=rd_u32(REG_A[7]),address=rd_u32(REG_A[7]+8);
            FA18ViewportPalettePhase phase=ret==0xc17360?FA18_PALETTE_FIRST:ret==0xc173ae?FA18_PALETTE_SECOND:FA18_PALETTE_STABLE;
            if(source_count>=2 || rd_u32(REG_A[7]+12)!=16 || rd_u32(REG_A[7]+4)!=0xc1822a ||
               (ret!=0xc17360 && ret!=0xc173ae && ret!=0xc17442)) return 0;
            events[source_count].phase=phase; events[source_count].address=address;
            for(i=0;i<16;++i) events[source_count].words[i]=rd_u16(address+2*i);
            memcpy(event_ram[source_count],fa18_machine->chip,FA18_CHIP_SIZE);
            memcpy(event_ram[source_count]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
            palette_effect(NULL,phase,source_count++);
            REG_PC=m68ki_pull_32(); continue;
        }
        for(i=0;i<sizeof input_source_bytes/sizeof input_source_bytes[0];++i) if(input_source_bytes[i].pc==pc) break;
        if(i==sizeof input_source_bytes/sizeof input_source_bytes[0]) { fprintf(stderr,"unexpected original PC %06X\n",pc); return 0; }
        input_visited[i]=1;
        opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size),*rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,i,r;
    char error[256];
    if(!state || !rom || !m || !base || !before || !expected || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof input_source_bytes/sizeof input_source_bytes[0];++i)
        for(r=0;r<input_source_bytes[i].length;++r)
            if(rd_u8(input_source_bytes[i].pc+r)!=input_source_bytes[i].bytes[r]) return 1;
    for(case_number=0;case_number<cases;++case_number) {
        NativeInput native;
        memcpy(m,base,sizeof *m); input_fixture(case_number); load_input(&native);
        source_count=native_count=0; memcpy(before,m,sizeof *m);
        if(!original_input()) { fprintf(stderr,"original input case %u failed at %06X\n",case_number,REG_PC); return 1; }
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(!fa18_advance_native_input_callback(&native.input,sampled_pair,&native.ops)) return 1;
        store_input(&native);
        if(source_count!=native_count || !equal_input_ram(expected)) return 1;
        total_events+=native_count;
    }
    printf("native input callback C1718E: %u original calls matched RAM outside CPU ABI stack, return and %u ordered palette boundaries\n",cases,total_events);
    printf("visited:"); for(i=0;i<sizeof input_visited/sizeof input_visited[0];++i) if(input_visited[i]) printf(" %06X",input_source_bytes[i].pc); putchar('\n');
    free(expected); free(before); free(base); free(m); return 0;
}
