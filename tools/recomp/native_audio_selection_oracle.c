/* Complete original menu/selection closures; CPU state is validation only. */
#define main reference_publication_validation_main
#include "command_publication_oracle.c"
#undef main
#include "../../port/voice_selection.c"
#include "../../port/audio_selection.c"
#include "../../build/recomp/native_audio_selection_source.h"

enum { TEST_VOICES=6, SOUNDS=37, ACK_LIMIT=8,
       ABI_STACK_FIRST=0xc7fd00, ABI_STACK_END=0xc7ff00 };
typedef struct {
    FA18CommandAudio audio;
    PortVoice voices[TEST_VOICES],*sounds[SOUNDS];
} SelectionState;
typedef struct { unsigned channel; uint16_t mask; } SelectionAck;
static SelectionAck selection_acks[ACK_LIMIT];
static uint8_t selection_ram[ACK_LIMIT][FA18_CHIP_SIZE+FA18_SLOW_SIZE];
static unsigned source_acks,port_acks,total_acks,test_scenario;
static uint8_t selection_visited[sizeof selection_source_bytes/sizeof selection_source_bytes[0]];

static uint32_t selection_voice_address(unsigned i) { return 0xc61000u+0x80u*i; }
static PortVoice *selection_resolve(SelectionState *s,uint32_t address) {
    unsigned i;
    if(!address) return NULL;
    for(i=0;i<TEST_VOICES;++i) if(address==selection_voice_address(i)) return &s->voices[i];
    abort();
}
static uint32_t selection_export(const SelectionState *s,const PortVoice *voice) {
    unsigned i;
    if(!voice) return 0;
    for(i=0;i<TEST_VOICES;++i) if(voice==&s->voices[i]) return selection_voice_address(i);
    abort();
}
static void selection_store(const SelectionState *s) {
    unsigned i;
    wr_u8(VOLUME_FADING,s->audio.volume_fading);
    wr_u8(SOUND_FLAGS,s->audio.sound_flags); wr_u8(SOUND_FLAGS-1,s->audio.effect_flags);
    for(i=0;i<TEST_VOICES;++i) wr_u32(selection_voice_address(i)+12,s->voices[i].volume);
    for(i=0;i<SOUNDS;++i) wr_u32(SOUND_VOICES+4*i,selection_export(s,s->sounds[i]));
    for(i=0;i<4;++i) {
        wr_u32(VOICE_SLOTS+4*i,selection_export(s,s->audio.slots[i]));
        wr_u16(rd_u32(VOICE_TABLE+4*i)+0x14,s->audio.interrupt_masks[i]);
    }
}
static int selection_equal_ram(const uint8_t *expected) {
    unsigned i;
    if(memcmp(expected,fa18_machine->chip,FA18_CHIP_SIZE)) return 0;
    for(i=0;i<FA18_SLOW_SIZE;++i) {
        uint32_t a=FA18_SLOW_BASE+i;
        if(a>=ABI_STACK_FIRST && a<ABI_STACK_END) continue;
        if(fa18_machine->slow[i]!=expected[FA18_CHIP_SIZE+i]) {
            fprintf(stderr,"RAM %06X source %02X native %02X\n",a,expected[FA18_CHIP_SIZE+i],fa18_machine->slow[i]);
            return 0;
        }
    }
    return 1;
}
/* Stress shared ownership at the real host acknowledgement boundary. These
 * supplied host effects are applied in both runs after the original write;
 * no CPU frame, result output, event or stack local is synthesized. */
static void selection_host_effects(SelectionState *s,unsigned ordinal) {
    unsigned sound=selected_entry==0xc17b2c?test_scenario%SOUNDS:
                      ((test_scenario/4)&0x80u?13:35);
    if((test_scenario&0x10) && (ordinal==0 || ordinal==4)) {
        uint32_t address=selection_voice_address((ordinal+test_scenario)%TEST_VOICES);
        if(s) s->sounds[sound]=selection_resolve(s,address);
        else wr_u32(SOUND_VOICES+4*sound,address);
    }
    if((test_scenario&0x20) && ordinal==0) {
        if(s) {
            s->audio.volume_fading=0xa5;
            s->audio.effect_flags^=4;
            s->audio.interrupt_masks[1]^=0x8000;
        } else {
            wr_u8(VOLUME_FADING,0xa5); wr_u8(SOUND_FLAGS-1,rd_u8(SOUND_FLAGS-1)^4);
            gaddr descriptor=rd_u32(VOICE_TABLE+4);
            wr_u16(descriptor+0x14,rd_u16(descriptor+0x14)^0x8000);
        }
    }
}
static void selection_native_ack(void *context,unsigned channel,uint16_t mask) {
    SelectionState *s=context;
    selection_store(s);
    if(port_acks>=source_acks || channel!=selection_acks[port_acks].channel ||
       mask!=selection_acks[port_acks].mask || !selection_equal_ram(selection_ram[port_acks])) {
        fprintf(stderr,"selection acknowledgement %u channel %u mask %04X differs\n",port_acks,channel,mask);
        exit(1);
    }
    fa18_custom_write(fa18_machine,0x9c,mask);
    selection_host_effects(s,port_acks++);
}
static void selection_load(SelectionState *s) {
    unsigned i;
    memset(s,0,sizeof *s);
    s->audio.volume_fading=rd_u8(VOLUME_FADING);
    s->audio.sound_flags=rd_u8(SOUND_FLAGS); s->audio.effect_flags=rd_u8(SOUND_FLAGS-1);
    for(i=0;i<TEST_VOICES;++i) s->voices[i].volume=rd_u32(selection_voice_address(i)+12);
    for(i=0;i<SOUNDS;++i) s->sounds[i]=selection_resolve(s,rd_u32(SOUND_VOICES+4*i));
    for(i=0;i<4;++i) {
        s->audio.slots[i]=selection_resolve(s,rd_u32(VOICE_SLOTS+4*i));
        s->audio.interrupt_masks[i]=rd_u16(rd_u32(VOICE_TABLE+4*i)+0x14);
    }
    s->audio.acknowledge=selection_native_ack; s->audio.acknowledge_context=s;
}
static uint32_t selection_volume(unsigned scenario) {
    static const uint32_t volumes[]={0,1,50,63,0x7fff,0x8000,0xffff,0x10000,0xffffffff,0x80000000};
    return volumes[scenario%10];
}
static void selection_fixture(unsigned scenario) {
    static const uint8_t fading[]={0,1,0x80,0xff};
    unsigned i;
    for(i=0;i<15;++i) REG_DA[i]=random_value();
    REG_A[7]=0xc7ff00; REG_PC=selected_entry; SET_CYCLES(100000000);
    wr_u32(REG_A[7],0xc70000); m68k_set_reg(M68K_REG_SR,0x2700|(scenario&31));
    fa18_next_event=INT64_MAX;
    wr_u32(REG_A[7]+4,selected_entry==0xc17b2c?scenario%SOUNDS:selection_volume(scenario));
    wr_u32(REG_A[7]+8,(scenario/SOUNDS)%4); wr_u32(REG_A[7]+12,selection_volume(scenario));
    wr_u8(VOLUME_FADING,(scenario%4)?0:fading[(scenario/4)%4]);
    wr_u8(SOUND_FLAGS,(uint8_t)(scenario/4)); wr_u8(SOUND_FLAGS-1,(uint8_t)(scenario/8));
    for(i=0;i<TEST_VOICES;++i) wr_u32(selection_voice_address(i)+12,random_value());
    for(i=0;i<SOUNDS;++i) wr_u32(SOUND_VOICES+4*i,((scenario+i)%4)?selection_voice_address((scenario+i)%TEST_VOICES):0);
    for(i=0;i<4;++i) {
        uint32_t descriptor=0xc62000+0x20*i;
        wr_u32(VOICE_TABLE+4*i,descriptor);
        wr_u16(descriptor+0x14,(uint16_t)((0x80u<<i)|((scenario&64)?0x8000:0)));
        wr_u32(VOICE_SLOTS+4*i,(scenario&1)?0:selection_voice_address(i));
    }
}
static int selection_original(void) {
    unsigned step,i;
    for(step=0;step<10000;++step) {
        uint32_t pc=REG_PC;
        uint16_t opcode;
        if(pc==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        for(i=0;i<sizeof selection_source_bytes/sizeof selection_source_bytes[0];++i)
            if(selection_source_bytes[i].pc==pc) break;
        if(i==sizeof selection_source_bytes/sizeof selection_source_bytes[0]) {
            fprintf(stderr,"unexpected original selection instruction %06X\n",pc); return 0;
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
    unsigned cases=argc>1?(unsigned)strtoul(argv[1],NULL,10):4096,scenario,i,r;
    char error[256];
    selected_entry=argc>2?(uint32_t)strtoul(argv[2],NULL,16):0xc17b96;
    if(!state || !rom || !m || !base || !before || !expected || !cases) return 1;
    if(!fa18_machine_load_state(m,state,state_size,rom,rom_size,error,sizeof error)) return 1;
    free(state); free(rom); fa18_bus_timing=0; memcpy(base,m,sizeof *m);
    for(i=0;i<sizeof selection_source_bytes/sizeof selection_source_bytes[0];++i)
        for(r=0;r<selection_source_bytes[i].length;++r)
            if(rd_u8(selection_source_bytes[i].pc+r)!=selection_source_bytes[i].bytes[r]) return 1;
    for(scenario=0;scenario<cases;++scenario) {
        SelectionState native;
        uint16_t original_intreq,original_custom;
        int ok;
        test_scenario=scenario;
        memcpy(m,base,sizeof *m); selection_fixture(scenario); selection_load(&native);
        source_acks=port_acks=0; memcpy(before,m,sizeof *m);
        if(!selection_original()) { fprintf(stderr,"source selection failed case %u\n",scenario); return 1; }
        original_intreq=m->intreq; original_custom=m->custom[0x9c/2];
        memcpy(expected,m->chip,FA18_CHIP_SIZE); memcpy(expected+FA18_CHIP_SIZE,m->slow,FA18_SLOW_SIZE);
        memcpy(m,before,sizeof *m);
        if(selected_entry==0xc17b2c)
            ok=fa18_select_native_sound(&native.audio,native.sounds,SOUNDS,scenario%SOUNDS,
                                         (scenario/SOUNDS)%4,selection_volume(scenario));
        else ok=fa18_start_native_menu_sound(&native.audio,native.sounds,SOUNDS,selection_volume(scenario));
        selection_store(&native);
        if(!ok || source_acks!=port_acks || !selection_equal_ram(expected) ||
           original_intreq!=m->intreq || original_custom!=m->custom[0x9c/2]) {
            fprintf(stderr,"native selection %06X case %u failed\n",selected_entry,scenario); return 1;
        }
        total_acks+=port_acks;
    }
    printf("native audio selection %06X: %u complete calls, full RAM and %u ordered acknowledgements match\n",selected_entry,cases,total_acks);
    printf("visited:");
    for(i=0;i<sizeof selection_source_bytes/sizeof selection_source_bytes[0];++i)
        if(selection_visited[i]) printf(" %06X",selection_source_bytes[i].pc);
    puts("");
    free(expected); free(before); free(base); free(m);
    return 0;
}
