/* Complete audio update closures versus native code; validation only. */
#define main reference_publication_validation_main
#include "command_publication_oracle.c"
#undef main
#include "../../port/voice_program.c"
#include "../../port/audio_update.c"
#include "../../build/recomp/native_audio_update_source.h"

enum { VOICES=6, PROGRAMS=5, MAX_EVENTS=12, STACK_FIRST=0xc7fd00, STACK_END=0xc7ff00 };
static const uint32_t program_addresses[]={0xc50b78,0xc50bd8,0xc50c00,0xc50c30,0xc50c70};
static const unsigned program_lengths[]={12,5,6,8,12};
static const unsigned voice_offsets[]={8,12,0x34,0x2c,0x18,0x1c,0x24,0x28,0x38,0x3c};
typedef struct NativeAudio NativeAudio;
typedef struct { NativeAudio *state; unsigned target; } OutputChannel;
struct NativeAudio {
    FA18CommandAudio audio;
    FA18CommandVoice voices[VOICES];
    PortVoiceProgram programs[PROGRAMS];
    PortVoiceOperation operations[PROGRAMS][12];
    uint32_t values[PROGRAMS][12];
    FA18AudioUpdate update;
    OutputChannel outputs[4];
};
typedef struct { unsigned kind,channel; uint16_t value; } Event;
static Event expected_events[MAX_EVENTS];
static uint8_t event_ram[MAX_EVENTS][FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static unsigned source_count,native_count,total_events;
static unsigned char audio_visited[sizeof audio_source_bytes/sizeof audio_source_bytes[0]];
static uint32_t voice_address(unsigned i) { return 0xc61000u+0x80u*i; }
static uint32_t *voice_fields(FA18CommandVoice *v,unsigned i) {
    switch(i) {
    case 0: return &v->period; case 1: return &v->volume;
    case 2: return &v->position; case 3: return &v->delay;
    case 4: return &v->period_slide; case 5: return &v->volume_slide;
    case 6: return &v->loop_counters[0]; case 7: return &v->loop_counters[1];
    case 8: return &v->period_ticks; case 9: return &v->volume_ticks;
    }
    abort();
}
static FA18CommandVoice *resolve_voice(NativeAudio *s,uint32_t address) {
    unsigned i;
    if(!address) return NULL;
    for(i=0;i<VOICES;++i) if(address==voice_address(i)) return &s->voices[i];
    abort();
}
static uint32_t export_voice(const NativeAudio *s,const FA18CommandVoice *v) {
    unsigned i;
    if(!v) return 0;
    for(i=0;i<VOICES;++i) if(v==&s->voices[i]) return voice_address(i);
    abort();
}
static void store_audio(NativeAudio *s) {
    unsigned i,f;
    wr_u32(MASTER_VOLUME,s->audio.master_volume); wr_u32(MASTER_VOLUME_TARGET,s->audio.master_volume_target);
    wr_u8(VOLUME_FADING,s->audio.volume_fading);
    for(i=0;i<VOICES;++i) for(f=0;f<10;++f)
        wr_u32(voice_address(i)+voice_offsets[f],*voice_fields(&s->voices[i],f));
    for(i=0;i<4;++i) wr_u32(VOICE_SLOTS+4*i,export_voice(s,s->audio.slots[i]));
}
static int equal_audio_ram(const uint8_t *expected) {
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
static void compare_event(NativeAudio *s,unsigned kind,unsigned channel,uint16_t value) {
    store_audio(s);
    if(native_count>=source_count || expected_events[native_count].kind!=kind ||
       expected_events[native_count].channel!=channel || expected_events[native_count].value!=value ||
       !equal_audio_ram(event_ram[native_count])) {
        fprintf(stderr,"audio event %u kind %u channel %u value %04X differs\n",native_count,kind,channel,value);
        exit(1);
    }
    ++native_count;
}
static void audio_ack(void *context,unsigned channel,uint16_t mask) {
    NativeAudio *s=context;
    compare_event(s,2,channel,mask);
    fa18_custom_write(fa18_machine,0x9c,mask);
}
static int audio_output(void *context,FA18AudioParameter parameter,uint16_t value) {
    OutputChannel *o=context;
    compare_event(o->state,(unsigned)parameter,o->target,value);
    fa18_custom_write(fa18_machine,0xa0u+16*o->target+(parameter==FA18_AUDIO_PERIOD?6:8),value);
    return 1;
}
static void load_audio(NativeAudio *s) {
    unsigned i,f;
    memset(s,0,sizeof *s);
    for(i=0;i<PROGRAMS;++i) {
        uint32_t selectors[12];
        for(f=0;f<program_lengths[i];++f) {
            selectors[f]=rd_u32(program_addresses[i]+8*f);
            s->values[i][f]=rd_u32(program_addresses[i]+8*f+4);
        }
        if(!fa18_import_voice_operations(selectors,program_lengths[i],s->operations[i])) abort();
        s->programs[i]=(PortVoiceProgram){s->operations[i],{s->values[i],program_lengths[i]}};
    }
    for(i=0;i<VOICES;++i) {
        uint32_t a=voice_address(i),p=rd_u32(a+0x30);
        for(f=0;f<10;++f) *voice_fields(&s->voices[i],f)=rd_u32(a+voice_offsets[f]);
        if(p) {
            for(f=0;f<PROGRAMS;++f) if(p==program_addresses[f]) break;
            if(f==PROGRAMS) abort();
            s->voices[i].program=&s->programs[f];
        }
    }
    s->audio.master_volume=rd_u32(MASTER_VOLUME); s->audio.master_volume_target=rd_u32(MASTER_VOLUME_TARGET);
    s->audio.volume_fading=rd_u8(VOLUME_FADING);
    s->audio.acknowledge=audio_ack; s->audio.acknowledge_context=s;
    s->update.audio=&s->audio;
    for(i=0;i<4;++i) {
        uint32_t descriptor=rd_u32(VOICE_TABLE+4*i);
        uint32_t slot=rd_u32(descriptor+4),target=rd_u32(descriptor);
        if(slot<VOICE_SLOTS || slot>=VOICE_SLOTS+16 || (slot&3) ||
           target<0xdff0a0 || target>0xdff0d0 || ((target-0xdff0a0)&15)) abort();
        s->audio.slots[i]=resolve_voice(s,rd_u32(VOICE_SLOTS+4*i));
        s->audio.interrupt_masks[i]=rd_u16(descriptor+0x14);
        s->outputs[i]=(OutputChannel){s,(target-0xdff0a0)/16};
        s->update.channels[i]=(FA18AudioUpdateChannel){&s->audio.slots[(slot-VOICE_SLOTS)/4],audio_output,&s->outputs[i]};
    }
}
static uint32_t base_long(const FA18Machine *base,uint32_t address) {
    const uint8_t *p=base->slow+(address-FA18_SLOW_BASE);
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];
}
static void audio_fixture(unsigned scenario,const FA18Machine *base) {
    static const uint32_t levels[]={0,0x00004000,0x001f0000,0x003f0000,0x00400000,0xffff0000,
                                     0x80000000,0x80000010,0x7ffff000,0x7fffffff,0xffffffff,0x003effff};
    static const uint32_t delays[]={0,1,2,0xffffffff};
    static const uint32_t periods[]={0,0x007b0000,0x007c0000,0x7fff0000,0x80000000,0xffffffff,0xffff0000,0x007cffff};
    unsigned i,f;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    SET_CYCLES(100000000); REG_A[7]=0xc7ff00; REG_PC=selected_entry;
    wr_u32(REG_A[7],0xc70000); m68k_set_reg(M68K_REG_SR,0x2700|((scenario>>3)&31));
    fa18_next_event=INT64_MAX;
    for(i=0;i<PROGRAMS;++i) for(f=0;f<program_lengths[i];++f)
        wr_u32(program_addresses[i]+8*f+4,base_long(base,program_addresses[i]+8*f+4));
    /* Independent loop initialization/entry profiles; all paths reach a wait. */
    wr_u32(0xc50b7c,(scenario/16)%3); wr_u32(0xc50b84,(scenario/32)%3);
    wr_u32(0xc50c74,(scenario/64)%3);
    for(i=0;i<VOICES;++i) {
        unsigned p=(scenario+i)%PROGRAMS;
        uint32_t a=voice_address(i);
        wr_u32(a+8,periods[(scenario+i)%8]); wr_u32(a+12,random_value());
        wr_u32(a+0x30,program_addresses[p]); wr_u32(a+0x34,((scenario/8+i)%program_lengths[p])*8);
        wr_u32(a+0x2c,delays[(scenario/4+i)%4]);
        wr_u32(a+0x18,random_value()); wr_u32(a+0x1c,random_value());
        wr_u32(a+0x24,(scenario+i)%3); wr_u32(a+0x28,(scenario/3+i)%3);
        wr_u32(a+0x38,delays[(scenario/16+i)%4]); wr_u32(a+0x3c,delays[(scenario/64+i)%4]);
    }
    for(i=0;i<4;++i) {
        uint32_t descriptor=0xc62000+0x20*i;
        unsigned target=(scenario&2048)?3-i:i,slot=(scenario&1024)?0:(scenario&512)?(i+1)%4:i;
        wr_u32(VOICE_TABLE+4*i,descriptor); wr_u32(descriptor,0xdff0a0+16*target);
        wr_u32(descriptor+4,VOICE_SLOTS+4*slot);
        wr_u16(descriptor+0x14,(uint16_t)((0x80u<<i)|((scenario&256)?0x8000:0)));
        wr_u32(VOICE_SLOTS+4*i,(scenario&(1u<<i))?0:voice_address(i));
    }
    wr_u32(MASTER_VOLUME,levels[scenario%12]); wr_u32(MASTER_VOLUME_TARGET,levels[(scenario/12)%12]);
    wr_u8(VOLUME_FADING,(scenario&64)?0xff:0);
    REG_A[3]=voice_address(0); REG_A[2]=VOICE_SLOTS+4*(scenario%4); REG_D[3]=scenario%4;
    REG_A[0]=rd_u32(rd_u32(VOICE_TABLE+4*(scenario%4)));
}
static int original_audio(void) {
    unsigned step,i;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof audio_source_bytes/sizeof audio_source_bytes[0];++i)
            if(audio_source_bytes[i].pc==pc) break;
        if(i==sizeof audio_source_bytes/sizeof audio_source_bytes[0]) {
            fprintf(stderr,"unexpected original instruction %06X\n",pc); return 0;
        }
        audio_visited[i]=1;
        if(pc==0xc4ffc0 || pc==0xc501f0 || pc==0xc5020c) {
            Event event;
            if(source_count>=MAX_EVENTS) return 0;
            if(pc==0xc4ffc0) event=(Event){2,REG_D[0]/4,rd_u16(REG_A[0]+0x14)};
            else event=(Event){pc==0xc501f0?0:1,(REG_A[0]-0xdff0a0)/16,(uint16_t)REG_D[0]};
            expected_events[source_count]=event;
            memcpy(event_ram[source_count],fa18_machine->chip,FA18_CHIP_SIZE);
            memcpy(event_ram[source_count]+FA18_CHIP_SIZE,fa18_machine->slow,FA18_SLOW_SIZE);
            ++source_count;
        }
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
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,scenario,i,r;
    char error[256];
    if(argc>2) selected_entry=(uint32_t)strtoul(argv[2],NULL,16);
    if(!state || !rom || !m || !base || !before || !expected || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof audio_source_bytes/sizeof audio_source_bytes[0];++i)
        for(r=0;r<audio_source_bytes[i].length;++r)
            if(rd_u8(audio_source_bytes[i].pc+r)!=audio_source_bytes[i].bytes[r]) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        NativeAudio native;
        uint16_t custom[0x100],intreq;
        int ok=0;
        memcpy(m,base,sizeof *m); audio_fixture(scenario,base); load_audio(&native);
        source_count=native_count=0; memcpy(before,m,sizeof *m);
        if(!original_audio()) { fprintf(stderr,"original audio failed %06X case %u\n",selected_entry,scenario); return 1; }
        intreq=m->intreq; memcpy(custom,m->custom,sizeof custom);
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        switch(selected_entry) {
        case 0xc50158: ok=fa18_update_native_audio(&native.update); break;
        case 0xc501e0: ok=fa18_output_native_voice(&native.audio,&native.voices[0],&native.update.channels[scenario%4]); break;
        case 0xc50212: ok=fa18_step_native_voice_program(&native.audio,&native.voices[0],&native.audio.slots[scenario%4],scenario%4); break;
        case 0xc24fe8: ok=fa18_fade_native_master_volume(&native.audio); break;
        }
        store_audio(&native);
        if(!ok || source_count!=native_count || memcmp(custom,m->custom,sizeof custom) || intreq!=m->intreq || !equal_audio_ram(expected)) {
            fprintf(stderr,"native audio %06X case %u events %u/%u\n",selected_entry,scenario,source_count,native_count); return 1;
        }
        total_events+=native_count;
    }
    printf("native audio update %06X: %u original calls matched RAM outside CPU ABI stack, channel state and %u ordered output/acknowledgement boundaries\n",selected_entry,cases,total_events);
    printf("visited:"); for(i=0;i<sizeof audio_visited;++i) if(audio_visited[i]) printf(" %06X",audio_source_bytes[i].pc); putchar('\n');
    free(expected); free(before); free(base); free(m); return 0;
}
