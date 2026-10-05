/* Original instructions are a validation oracle, never a native dependency. */
#define main reference_publication_validation_main
#include "command_publication_oracle.c"
#undef main
#include "../../port/command_queue.c"
#include "../../port/command_effects.c"
#include "../../build/recomp/native_command_effects_source.h"

enum { VOICES=6, STACK_FIRST=0xc7fd00, STACK_END=0xc7ff00, MAX_ACKS=4 };
typedef struct {
    FA18CommandInput c;
    FA18FlightCommandState f;
    FA18ViewCommandState v;
    FA18ContextCommandState context;
    FA18CommandQueue q;
    FA18CommandEffects effects;
    FA18CommandAudio audio;
    FA18CommandVoice voices[VOICES];
    FA18FlightCommandRecord player,target;
    uint32_t programmed[12],sweep[3];
} NativeEffects;
typedef struct { unsigned channel; uint16_t mask; } Ack;
static Ack expected_acks[MAX_ACKS];
static uint8_t boundary_ram[MAX_ACKS][FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static unsigned expected_count,native_count,total_acks;
static unsigned char effect_visited[sizeof effect_source_bytes/sizeof effect_source_bytes[0]];

static uint32_t voice_address(unsigned i) { return 0xc61000u+0x80u*i; }
static FA18CommandVoice *resolve_voice(NativeEffects *s,uint32_t address) {
    unsigned i;
    if(!address) return NULL;
    for(i=0;i<VOICES;++i) if(address==voice_address(i)) return &s->voices[i];
    abort();
}
static uint32_t export_voice(const NativeEffects *s,const FA18CommandVoice *voice) {
    unsigned i;
    if(!voice) return 0;
    for(i=0;i<VOICES;++i) if(voice==&s->voices[i]) return voice_address(i);
    abort();
}
static void store_effects(const NativeEffects *s) {
    unsigned i;
    wr_u8(COMMAND_BLOCK_FLAGS,s->c.block_flags); wr_u8(MODE_SELECT,s->c.indexed.mode);
    wr_u8(COMMAND_WEAPON_PAUSE,s->f.weapon_pause);
    wr_u8(CONTROL_RECORDS+0x63,s->player.weapon_radar);
    wr_u16(CONTROL_RECORDS+0x802,s->target.secondary_flags);
    wr_u16(PENDING_COMMAND_WORD_A,s->f.emitted_requests); wr_u16(COMMAND_WORD,s->f.command_word);
    wr_u8(SPACE_COMMAND_LATCH,s->f.space_command_latch);
    wr_u8(COCKPIT_FLAGS,s->c.indexed.cockpit_high_byte); wr_u8(COCKPIT_FLAGS+1,s->c.indexed.cockpit_low_byte);
    wr_u16(MESSAGE_CODE,s->effects.message_code); wr_u16(MESSAGE_STATE,s->effects.message_state);
    wr_u8(ORIGIN_ENABLE,s->c.origin_mode); wr_u8(VOLUME_FADING,s->audio.volume_fading);
    wr_u8(TONE_MUTE,s->audio.tone_mute); wr_u8(SOUND_FLAGS-1,s->audio.effect_flags);
    wr_u8(0xc45797,s->audio.sound6_mode); wr_u8(FIRE_STATE,s->v.fire_state);
    wr_u32(RANDOM_SEED,s->audio.random_seed);
    for(i=0;i<12;++i) wr_u32(PROGRAM_4_VALUES+8*i,s->programmed[i]);
    for(i=0;i<3;++i) wr_u32(0xc50bdc+8*i,s->sweep[i]);
    for(i=0;i<VOICES;++i) {
        uint32_t a=voice_address(i);
        wr_u32(a+8,s->voices[i].period); wr_u32(a+12,s->voices[i].volume);
        wr_u32(a+0x34,s->voices[i].position); wr_u32(a+0x2c,s->voices[i].delay);
    }
    for(i=0;i<4;++i) wr_u32(VOICE_SLOTS+4*i,export_voice(s,s->audio.slots[i]));
}
static int equal_ram(const uint8_t *expected) {
    unsigned i;
    if(memcmp(expected,fa18_machine->chip,FA18_CHIP_SIZE)) return 0;
    for(i=0;i<FA18_SLOW_SIZE;++i) {
        uint32_t a=FA18_SLOW_BASE+i;
        if(a>=STACK_FIRST && a<STACK_END) continue;
        if(fa18_machine->slow[i]!=expected[FA18_CHIP_SIZE+i]) {
            fprintf(stderr,"RAM %06X original %02X native %02X\n",a,expected[FA18_CHIP_SIZE+i],fa18_machine->slow[i]);
            return 0;
        }
    }
    return 1;
}
static void native_ack(void *context,unsigned channel,uint16_t mask) {
    NativeEffects *s=context;
    store_effects(s);
    if(native_count>=expected_count || expected_acks[native_count].channel!=channel ||
       expected_acks[native_count].mask!=mask || !equal_ram(boundary_ram[native_count])) {
        fprintf(stderr,"acknowledgement %u channel %u mask %04X differs\n",native_count,channel,mask); exit(1);
    }
    ++native_count;
    /* Validation host backend reproduces the original acknowledgement only. */
    fa18_custom_write(fa18_machine,0x9c,mask);
}
static void load_effects(NativeEffects *s) {
    unsigned i;
    uint8_t neighbors[FA18_COMMAND_QUEUE_NEIGHBORS],keys[FA18_COMMAND_KEY_TABLE_SIZE];
    memset(s,0,sizeof *s);
    s->f.commands=&s->c; s->f.player=&s->player; s->f.target=&s->target;
    s->v.flight=&s->f; s->context.view=&s->v;
    for(i=0;i<sizeof neighbors;++i) neighbors[i]=rd_u8(KEY_RAW-128+i);
    for(i=0;i<sizeof keys;++i) keys[i]=rd_u8(KEY_TABLE+i);
    if(!fa18_initialize_command_queue(&s->q,&s->context,neighbors,sizeof neighbors,keys,sizeof keys)) abort();
    s->c.block_flags=rd_u8(COMMAND_BLOCK_FLAGS); s->c.indexed.mode=rd_u8(MODE_SELECT);
    s->c.indexed.cockpit_high_byte=rd_u8(COCKPIT_FLAGS); s->c.indexed.cockpit_low_byte=rd_u8(COCKPIT_FLAGS+1);
    s->f.emitted_requests=rd_u16(PENDING_COMMAND_WORD_A); s->f.command_word=rd_u16(COMMAND_WORD);
    s->player.weapon_radar=rd_u8(CONTROL_RECORDS+0x63); s->target.secondary_flags=rd_u16(CONTROL_RECORDS+0x802);
    s->effects.message_code=rd_u16(MESSAGE_CODE); s->effects.message_state=rd_u16(MESSAGE_STATE);
    s->v.fire_state=rd_u8(FIRE_STATE); s->audio.random_seed=rd_u32(RANDOM_SEED);
    s->audio.tone_mute=rd_u8(TONE_MUTE); s->audio.effect_flags=rd_u8(SOUND_FLAGS-1);
    for(i=0;i<VOICES;++i) {
        uint32_t a=voice_address(i);
        s->voices[i]=(FA18CommandVoice){rd_u32(a+8),rd_u32(a+12),rd_u32(a+0x34),rd_u32(a+0x2c)};
    }
    s->audio.programmed_voice=resolve_voice(s,rd_u32(SOUND_VOICES+4*4));
    s->audio.sweep_voice=resolve_voice(s,rd_u32(SOUND_VOICES+4*6));
    for(i=0;i<4;++i) {
        s->audio.slots[i]=resolve_voice(s,rd_u32(VOICE_SLOTS+4*i));
        s->audio.interrupt_masks[i]=rd_u16(rd_u32(VOICE_TABLE+4*i)+0x14);
    }
    for(i=0;i<12;++i) s->programmed[i]=rd_u32(PROGRAM_4_VALUES+8*i);
    for(i=0;i<3;++i) s->sweep[i]=rd_u32(0xc50bdc+8*i);
    s->audio.programmed=(FA18CommandSoundProgram){s->programmed,12};
    s->audio.sweep=(FA18CommandSoundProgram){s->sweep,3};
    s->audio.acknowledge=native_ack; s->audio.acknowledge_context=s;
    if(!fa18_initialize_command_effects(&s->effects,&s->context,&s->q,&s->audio)) abort();
}
static void effect_fixture(unsigned scenario) {
    static const uint8_t mute[]={0,1,0x7f,0x80,0xff};
    static const int32_t periods[]={0,1,28,-28,0x7fff,0x8000,0xffff,0x10000,INT32_MIN,INT32_MAX};
    static const int32_t ticks[]={0,1,-1,48,-48,100000,INT32_MIN,INT32_MAX};
    unsigned i;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    SET_CYCLES(100000000); REG_A[7]=0xc7ff00; REG_PC=selected_entry;
    wr_u32(REG_A[7],0xc70000); m68k_set_reg(M68K_REG_SR,0x2700|((scenario>>3)&31));
    fa18_next_event=INT64_MAX;
    /* Sweep arguments exercise signs, shift wrap and signed divide results. */
    wr_s32(REG_A[7]+4,periods[scenario%10]);
    wr_s32(REG_A[7]+8,ticks[(scenario/10)%8]);
    REG_D[0]=(random_value()&0xffff0000u)|(uint16_t)(scenario*37u);
    wr_u8(COMMAND_BLOCK_FLAGS,(scenario&8)?0:(uint8_t)(scenario>>4));
    wr_u8(MODE_SELECT,(scenario&64)?0x7d:1);
    wr_u8(COMMAND_WEAPON_PAUSE,(scenario&32)?0x80:0);
    wr_u8(CONTROL_RECORDS+0x63,(uint8_t)(scenario*16u));
    wr_u16(CONTROL_RECORDS+0x802,(uint16_t)random_value());
    wr_u16(PENDING_COMMAND_WORD_A,(uint16_t)random_value()); wr_u16(COMMAND_WORD,(uint16_t)random_value());
    wr_u8(SPACE_COMMAND_LATCH,(uint8_t)random_value());
    wr_u16(COCKPIT_FLAGS,(uint16_t)random_value()); wr_u16(MESSAGE_CODE,(uint16_t)random_value());
    wr_u16(MESSAGE_STATE,(uint16_t)random_value());
    wr_u8(ORIGIN_ENABLE,(scenario&128)?1:0); wr_u8(VOLUME_FADING,(scenario&64)?0x80:0);
    wr_u8(TONE_MUTE,mute[scenario%5]); wr_u8(SOUND_FLAGS-1,(scenario&64)?1:0);
    wr_u8(0xc45797,(uint8_t)random_value()); wr_u8(FIRE_STATE,(uint8_t)random_value());
    wr_u32(RANDOM_SEED,random_value());
    for(i=0;i<VOICES;++i) {
        uint32_t a=voice_address(i);
        wr_u32(a+8,random_value()); wr_u32(a+12,random_value());
        wr_u32(a+0x34,random_value()); wr_u32(a+0x2c,random_value());
    }
    wr_u32(SOUND_VOICES+4*4,(scenario&256)?0:voice_address(4));
    wr_u32(SOUND_VOICES+4*6,(scenario&512)?0:voice_address(5));
    for(i=0;i<4;++i) {
        uint32_t descriptor=0xc62000+0x20*i;
        wr_u32(VOICE_TABLE+4*i,descriptor);
        wr_u16(descriptor+0x14,(uint16_t)((0x80u<<i)|((scenario&1024)?0x8000:0)));
        wr_u32(VOICE_SLOTS+4*i,(scenario&1)?0:voice_address(i));
    }
    for(i=0;i<12;++i) wr_u32(PROGRAM_4_VALUES+8*i,random_value());
    for(i=0;i<3;++i) wr_u32(0xc50bdc+8*i,random_value());
}
static int original_effect(void) {
    unsigned step,i;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof effect_source_bytes/sizeof effect_source_bytes[0];++i)
            if(effect_source_bytes[i].pc==pc) break;
        if(i==sizeof effect_source_bytes/sizeof effect_source_bytes[0]) {
            fprintf(stderr,"unexpected original instruction %06X\n",pc); return 0;
        }
        effect_visited[i]=1;
        if(pc==0xc4ffc0) {
            if(expected_count>=MAX_ACKS) return 0;
            expected_acks[expected_count]=(Ack){REG_D[0]/4,rd_u16(REG_A[0]+0x14)};
            memcpy(boundary_ram[expected_count],fa18_machine->chip,FA18_CHIP_SIZE);
            memcpy(boundary_ram[expected_count]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
            ++expected_count;
        }
        opcode=rd_u16(pc); REG_PPC=pc; REG_IR=opcode; REG_PC=pc+2;
        m68ki_instruction_jump_table[opcode](); USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    return 0;
}
int main(int argc,char **argv) {
    size_t state_size=0,rom_size=0;
    uint8_t *state=read_file("captures/native/demo01/state.bin",&state_size);
    uint8_t *rom=read_file("local/system/kick13.rom",&rom_size);
    FA18Machine *m=calloc(1,sizeof *m),*base=malloc(sizeof *base),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(FA18_CHIP_SIZE+FA18_SLOW_SIZE);
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):2048,scenario,i,r;
    char error[256];
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !expected || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof effect_source_bytes/sizeof effect_source_bytes[0];++i)
        for(r=0;r<effect_source_bytes[i].length;++r)
            if(rd_u8(effect_source_bytes[i].pc+r)!=effect_source_bytes[i].bytes[r]) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        NativeEffects native;
        uint32_t input,event,original_event;
        uint16_t original_intreq,original_custom;
        int ok=0;
        memcpy(m,base,sizeof *m); effect_fixture(scenario); load_effects(&native);
        input=REG_D[0]; expected_count=native_count=0; memcpy(before,m,sizeof *m);
        if(!original_effect()) { fprintf(stderr,"original effect failed %06X case %u\n",selected_entry,scenario); return 1; }
        original_event=REG_D[0]; original_intreq=m->intreq; original_custom=m->custom[0x9c/2];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m); event=input;
        switch(selected_entry) {
        case 0xc0833e: ok=fa18_press_native_space_command(&native.f); break;
        case 0xc25704: ok=fa18_post_native_command_message(&native.effects,input,&event); break;
        case 0xc33186:
            if(native.c.origin_mode) { ok=1; break; }
            /* falls through */
        case 0xc3318e: ok=fa18_play_native_status_tone(&native.audio,&event); break;
        case 0xc0f4a6: ok=fa18_release_native_command_voices(&native.audio,&event); break;
        case 0xc17f8c: ok=fa18_start_native_sound6(&native.effects,input,
                            rd_s32(0xc7ff04),rd_s32(0xc7ff08),&event); break;
        }
        store_effects(&native);
        if(!ok || event!=original_event || expected_count!=native_count ||
           m->intreq!=original_intreq || m->custom[0x9c/2]!=original_custom || !equal_ram(expected)) {
            fprintf(stderr,"native effects %06X case %u event %08X/%08X acknowledgements %u/%u\n",
                    selected_entry,scenario,original_event,event,expected_count,native_count); return 1;
        }
        total_acks+=native_count;
    }
    printf("native command effects %06X: %u original calls matched RAM outside CPU ABI stack, full events and %u ordered audio acknowledgements\n",selected_entry,cases,total_acks);
    printf("visited:"); for(i=0;i<sizeof effect_visited;++i) if(effect_visited[i]) printf(" %06X",effect_source_bytes[i].pc); putchar('\n');
    free(expected); free(before); free(base); free(m); return 0;
}
