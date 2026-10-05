/* CPU-backed oracle only: the native publisher/formatter use ordinary state. */
#define FA18_SELECTION_SOURCE_HEADER "../../build/recomp/native_postflight_text_source.h"
#define FA18_AUDIO_SELECTION_HELPERS_ONLY
#include "native_audio_selection_oracle.c"
#include "../../port/hex_field.c"
#include "../../port/postflight_text.c"
#include "../../port/menu_record.c"

enum { SEED_BANK=0xc08510, DYNAMIC_A=0xc63000, DYNAMIC_B=0xc64000,
       TEXT_DESCRIPTOR=0xc3f040, HEX_BUFFER=0xc65000, HEX_BYTES=512 };
typedef struct {
    SelectionState sounds;
    FA18CommandInput commands;
    FA18FlightCommandState flight;
    FA18NativeInputDisplay display;
    FA18NativePostflightText publisher;
    uint16_t countdown,checksums[3],seed[256],dynamic[2][32];
    FA18StageCallback stage;
    uint8_t text[64],hex[HEX_BYTES];
} TextState;
static uint8_t bootstrap_before[FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static unsigned bootstrap_original_count,bootstrap_native_count;

static int text_equal_ram(const uint8_t *expected) {
    unsigned i;
    if(selected_entry!=0xc0f56a) return selection_equal_ram(expected);
    if(memcmp(expected,fa18_machine->chip,FA18_CHIP_SIZE)) return 0;
    for(i=0;i<FA18_SLOW_SIZE;++i) {
        uint32_t a=FA18_SLOW_BASE+i;
        /* This child rewrites its by-value argument slots as local cursors.
         * Native C arguments use the host ABI; exclude those slots explicitly. */
        if(a>=ABI_STACK_FIRST && a<0xc7ff10) continue;
        if(fa18_machine->slow[i]!=expected[FA18_CHIP_SIZE+i]) {
            fprintf(stderr,"hex RAM %06X source %02X native %02X\n",a,expected[FA18_CHIP_SIZE+i],fa18_machine->slow[i]);
            return 0;
        }
    }
    return 1;
}

static uint32_t text_palette_export(const TextState *s,const uint16_t *palette) {
    unsigned i;
    for(i=0;i<256;++i) if(palette==s->seed+i) return SEED_BANK+2*i;
    if(palette==s->dynamic[0]) return DYNAMIC_A;
    if(palette==s->dynamic[1]) return DYNAMIC_B;
    abort();
}
static uint16_t *text_palette_resolve(TextState *s,uint32_t address) {
    if(address>=SEED_BANK && address<SEED_BANK+512 && !(address&1)) return s->seed+(address-SEED_BANK)/2;
    if(address==DYNAMIC_A) return s->dynamic[0];
    if(address==DYNAMIC_B) return s->dynamic[1];
    abort();
}
static uint32_t text_stage_export(FA18StageCallback stage) {
    switch(stage) {
    case FA18_STAGE_C0F812: return 0xc0f812;
    case FA18_STAGE_C11446: return 0xc11446;
    case FA18_STAGE_C113E4: return 0xc113e4;
    default: abort();
    }
}
static void text_store(const TextState *s) {
    unsigned i;
    selection_store(&s->sounds);
    wr_u8(MODE_SELECT,s->commands.indexed.mode); wr_u8(ORIGIN_GATE_MODE,s->flight.pause);
    wr_u8(MENU_TRANSITION_FLAG,s->publisher.transition); wr_u16(POST_INPUT_COUNTDOWN,s->countdown);
    wr_u32(STAGE_CALLBACK,text_stage_export(s->stage));
    wr_u32(LONG_TABLE,text_palette_export(s,s->display.stable_palette));
    for(i=0;i<3;++i) wr_u16(0xc4564c+4*i,s->checksums[i]);
    for(i=0;i<256;++i) wr_u16(SEED_BANK+2*i,s->seed[i]);
    for(i=0;i<32;++i) {
        wr_u16(DYNAMIC_A+2*i,s->dynamic[0][i]); wr_u16(DYNAMIC_B+2*i,s->dynamic[1][i]);
    }
    for(i=0;i<sizeof s->text;++i) wr_u8(TEXT_DESCRIPTOR+i,s->text[i]);
    for(i=0;i<HEX_BYTES;++i) wr_u8(HEX_BUFFER+i,s->hex[i]);
}
static void text_acknowledge(void *context,unsigned channel,uint16_t mask) {
    TextState *s=context;
    text_store(s);
    selection_native_ack(&s->sounds,channel,mask);
}
/* Explicit bootstrap contract: its ordinary native graph is still open.
 * Mutate only shared fields, never CPU frames/locals or captured outputs. */
static uint32_t bootstrap_destination(unsigned scenario) {
    if(scenario&8) return SEED_BANK+2*((scenario&16)?1:16);
    return (scenario&4)?DYNAMIC_B:DYNAMIC_A;
}
static void bootstrap_effect(TextState *s) {
    uint32_t destination=bootstrap_destination(test_scenario);
    if(s) {
        s->display.stable_palette=text_palette_resolve(s,destination);
        s->commands.indexed.mode=0x7f; s->publisher.transition=1;
        s->countdown=5;
        if(test_scenario&64) s->checksums[1]=0;
    } else {
        wr_u32(LONG_TABLE,destination); wr_u8(MODE_SELECT,0x7f);
        wr_u8(MENU_TRANSITION_FLAG,1); wr_u16(POST_INPUT_COUNTDOWN,5);
        if(test_scenario&64) wr_u16(0xc45650,0);
    }
}
static int text_bootstrap(void *context) {
    TextState *s=context;
    text_store(s);
    if(bootstrap_native_count++ || !bootstrap_original_count || !selection_equal_ram(bootstrap_before)) {
        fprintf(stderr,"bootstrap entry state differs\n"); exit(1);
    }
    bootstrap_effect(s);
    return 1;
}
static void text_load(TextState *s) {
    unsigned i;
    memset(s,0,sizeof *s); selection_load(&s->sounds);
    s->sounds.audio.acknowledge=text_acknowledge; s->sounds.audio.acknowledge_context=s;
    s->commands.indexed.mode=rd_u8(MODE_SELECT); s->flight.commands=&s->commands;
    s->flight.pause=rd_u8(ORIGIN_GATE_MODE); s->countdown=rd_u16(POST_INPUT_COUNTDOWN);
    s->stage=FA18_STAGE_C0F812;
    s->publisher=(FA18NativePostflightText){.display=&s->display,.flight=&s->flight,
        .countdown=&s->countdown,.callback=&s->stage,.transition=rd_u8(MENU_TRANSITION_FLAG),
        .palette_seed=s->seed,.text_descriptor=s->text,.text_bytes=sizeof s->text,
        .audio=&s->sounds.audio,.sounds=s->sounds.sounds,.sound_count=SOUNDS};
    for(i=0;i<3;++i) {
        s->checksums[i]=rd_u16(0xc4564c+4*i); s->publisher.checksums[i]=&s->checksums[i];
    }
    for(i=0;i<256;++i) s->seed[i]=rd_u16(SEED_BANK+2*i);
    for(i=0;i<32;++i) {
        s->dynamic[0][i]=rd_u16(DYNAMIC_A+2*i); s->dynamic[1][i]=rd_u16(DYNAMIC_B+2*i);
    }
    s->display.stable_palette=text_palette_resolve(s,rd_u32(LONG_TABLE));
    for(i=0;i<sizeof s->text;++i) s->text[i]=rd_u8(TEXT_DESCRIPTOR+i);
    for(i=0;i<HEX_BYTES;++i) s->hex[i]=rd_u8(HEX_BUFFER+i);
}
static uint32_t text_hex_value(unsigned scenario) {
    static const uint32_t values[]={0,1,15,16,0x1234,0xabcd,0xdeadbeef,0xffffffff,0x80000000};
    return values[(scenario/256)%9];
}
static void text_fixture(unsigned scenario) {
    static const uint16_t sentinels[]={0xc560,0x7e70,0x4de8};
    unsigned i;
    selection_fixture(scenario);
    wr_u32(LONG_TABLE,DYNAMIC_A); wr_u32(STAGE_CALLBACK,0xc0f812);
    wr_u8(MODE_SELECT,(uint8_t)random_value()); wr_u8(ORIGIN_GATE_MODE,(uint8_t)random_value());
    wr_u8(MENU_TRANSITION_FLAG,(uint8_t)random_value()); wr_u16(POST_INPUT_COUNTDOWN,(uint16_t)random_value());
    for(i=0;i<3;++i) wr_u16(0xc4564c+4*i,(scenario&(1u<<i))?sentinels[i]:
                            (scenario&32)?0:(uint16_t)selection_volume(scenario+i));
    for(i=0;i<256;++i) wr_u16(SEED_BANK+2*i,(uint16_t)random_value());
    for(i=0;i<32;++i) { wr_u16(DYNAMIC_A+2*i,(uint16_t)random_value()); wr_u16(DYNAMIC_B+2*i,(uint16_t)random_value()); }
    for(i=0;i<64;++i) wr_u8(TEXT_DESCRIPTOR+i,(uint8_t)random_value());
    for(i=0;i<HEX_BYTES;++i) wr_u8(HEX_BUFFER+i,((scenario/256)%3)?(uint8_t)random_value():'0');
    if(selected_entry==0xc0f56a) {
        wr_u32(REG_A[7]+4,HEX_BUFFER+256);
        wr_u32(REG_A[7]+8,text_hex_value(scenario));
        wr_u32(REG_A[7]+12,(random_value()&0xffffff00u)|(uint8_t)scenario);
    }
}
static int text_original(void) {
    unsigned step,i;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        if(pc==0xc08f26) {
            if(bootstrap_original_count++) return 0;
            memcpy(bootstrap_before,fa18_machine->chip,FA18_CHIP_SIZE);
            memcpy(bootstrap_before+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
            bootstrap_effect(NULL);
            /* Actual child's incidental return registers are ignored by the
             * publisher. This contract deliberately changes the volatile set. */
            REG_D[0]=0xffffffff; REG_D[1]=0x12345678; REG_A[0]=0xc68000; REG_A[1]=0xc68100;
            REG_PC=rd_u32(REG_A[7]); REG_A[7]+=4;
            continue;
        }
        for(i=0;i<sizeof selection_source_bytes/sizeof selection_source_bytes[0];++i)
            if(selection_source_bytes[i].pc==pc) break;
        if(i==sizeof selection_source_bytes/sizeof selection_source_bytes[0]) {
            fprintf(stderr,"unexpected text instruction %06X\n",pc); return 0;
        }
        selection_visited[i]=1;
        if(pc==0xc4ffc0) {
            if(source_acks>=ACK_LIMIT) return 0;
            selection_acks[source_acks]=(SelectionAck){REG_D[0]/4,rd_u16(REG_A[0]+0x14)};
            memcpy(selection_ram[source_acks],fa18_machine->chip,FA18_CHIP_SIZE);
            memcpy(selection_ram[source_acks]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
        }
        opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
        if(pc==0xc4ffc0) selection_host_effects(NULL,source_acks++);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,scenario,i,r,bootstrap_calls=0;
    char error[256];
    selected_entry=argc>2?(uint32_t)strtoul(argv[2],NULL,16):0xc0f812;
    if(!state || !rom || !m || !base || !before || !expected || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof selection_source_bytes/sizeof selection_source_bytes[0];++i)
        for(r=0;r<selection_source_bytes[i].length;++r)
            if(rd_u8(selection_source_bytes[i].pc+r)!=selection_source_bytes[i].bytes[r]) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        TextState native;
        FA18NativePostflightTextOps ops={text_bootstrap,&native};
        uint16_t original_intreq,original_custom;
        int ok;
        test_scenario=scenario;
        memcpy(m,base,sizeof *m); text_fixture(scenario); text_load(&native);
        source_acks=port_acks=bootstrap_original_count=bootstrap_native_count=0;
        memcpy(before,m,sizeof *m);
        if(!text_original()) { fprintf(stderr,"original text failed case %u\n",scenario); return 1; }
        original_intreq=m->intreq; original_custom=m->custom[0x9c/2];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(selected_entry==0xc0f56a) ok=fa18_format_native_hex_field(native.hex,HEX_BYTES,256,text_hex_value(scenario),(uint8_t)scenario);
        else ok=fa18_publish_native_postflight_text(&native.publisher,&ops);
        text_store(&native);
        if(!ok || source_acks!=port_acks || bootstrap_original_count!=bootstrap_native_count ||
           !text_equal_ram(expected) || original_intreq!=m->intreq || original_custom!=m->custom[0x9c/2]) {
            fprintf(stderr,"native text %06X case %u failed\n",selected_entry,scenario); return 1;
        }
        total_acks+=port_acks; bootstrap_calls+=bootstrap_native_count;
    }
    printf("native text %06X: %u complete calls, full RAM, %u ordered acknowledgements and %u contracted bootstrap calls match\n",selected_entry,cases,total_acks,bootstrap_calls);
    printf("visited:");
    for(i=0;i<sizeof selection_source_bytes/sizeof selection_source_bytes[0];++i)
        if(selection_visited[i]) printf(" %06X",selection_source_bytes[i].pc);
    puts("");
    free(expected); free(before); free(base); free(m);
    return 0;
}
