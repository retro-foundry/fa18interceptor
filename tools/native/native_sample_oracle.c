/* Full C500D8 sample request, with original output and repetition/chaining. */
#define main records_fixture_main
#include "native_records_oracle.c"
#undef main
#include "audio.h"
#include "../../../fa18-interceptor-decomp/port/game/native/audio.c"

typedef struct { FA18Machine *machine; unsigned resolutions; } SampleOwner;
static const int8_t *sample_bytes(void *context,gaddr address,uint32_t bytes) {
    SampleOwner *owner=context;
    ++owner->resolutions;
    if(address<0x80000u && bytes<=0x80000u-address)
        return (const int8_t *)owner->machine->chip+address;
    if(address>=0xc00000u && address<0xc80000u && bytes<=0xc80000u-address)
        return (const int8_t *)owner->machine->slow+(address-0xc00000u);
    return NULL;
}
static int source_request(unsigned channel) {
    memset(REG_DA,0,sizeof REG_DA);REG_A[7]=0xc7ff00;
    wr_u32(REG_A[7],0xc70000);REG_A[1]=rd_u32(VOICE_TABLE+4*channel);
    REG_PC=0xc500d8;m68k_set_reg(M68K_REG_SR,0x2700);
    fa18_next_event=INT64_MAX;SET_CYCLES(100000000);
    for(unsigned step=0;step<10000;++step) {
        if(REG_PC==0xc70000 && REG_A[7]==0xc7ff04) return 1;
        uint16_t opcode=rd_u16(REG_PC);REG_PPC=REG_PC;REG_IR=opcode;REG_PC+=2;
        m68ki_instruction_jump_table[opcode]();USE_CYCLES(CYC_INSTRUCTION[opcode]);
    }
    fprintf(stderr,"source sample request did not return at %06X\n",REG_PC);return 0;
}
int main(int argc,char **argv) {
    size_t ns=0,nr=0,nd=0;char error[256];
    uint8_t *state=file_bytes("captures/native/demo01/state.bin",&ns);
    uint8_t *rom=file_bytes("local/system/kick13.rom",&nr);
    uint8_t *data=argc==2?file_bytes(argv[1],&nd):NULL;
    FA18Machine *m=calloc(1,sizeof *m),*before=malloc(sizeof *before);
    uint8_t *expected=malloc(0x100000);
    if(!state||!rom||!data||nd!=0x100000||!m||!before||!expected) return 1;
    if(!fa18_machine_load_state(m,state,ns,rom,nr,error,sizeof error)) return 1;
    fa18_recomp_init(1);fa18_ports_init(FA18_PORTS_OFF,NULL);fa18_bus_timing=0;
    static const uint32_t lengths[]={0,1,2,3,32,568,0x80000020,0x80010036};
    static const uint32_t repeats[]={0,1,2,13,0xffffffff,0xfffffffe,0x80000000,0x7fffffff};
    for(unsigned test=0;test<256;++test) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        unsigned channel=test%4;gaddr record=rd_u32(VOICE_TABLE+4*channel);
        gaddr slot=rd_u32(record+4),voice=0x6000;
        memset(m->chip+voice,0,64);wr_u32(slot,test&128?0:voice);
        wr_u32(voice,0x7040+(test&1));wr_u32(voice+4,lengths[(test>>1)%8]);
        wr_u32(voice+16,repeats[(test>>4)%8]);wr_u32(voice+20,test&1?2:0);
        wr_u32(voice+32,test&2?0x6080:0);
        wr_u32(voice+8,(uint32_t[]){0,0x7c0000,0x136ffff,0x80000000}[(test>>2)%4]);
        wr_u32(voice+12,(uint32_t[]){0,0x3f0000,0x400000,0xffff0000}[(test>>3)%4]);
        wr_u32(MASTER_VOLUME,test&4?0x1f0000:0xffff0000);
        memcpy(before,m,sizeof *m);if(!source_request(channel)) return 1;
        uint32_t pointer=(uint32_t)m->custom[(0xa0+16*channel)/2]<<16|m->custom[(0xa2+16*channel)/2];
        uint16_t words=m->custom[(0xa4+16*channel)/2];
        VoiceOutput output={(int16_t)m->custom[(0xa6+16*channel)/2],(int16_t)m->custom[(0xa8+16*channel)/2]};
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);VoiceSample sample=request_voice_sample(channel);
        for(unsigned i=0;i<0xffc00;++i) {
            uint8_t actual=i<0x80000?m->chip[i]:m->slow[i-0x80000];
            if(actual!=expected[i]) {
                fprintf(stderr,"sample case %u at %06X: %02X != %02X\n",test,
                    i<0x80000?i:i-0x80000+0xc00000,actual,expected[i]);return 1;
            }
        }
        if(sample.active!=(test<128) || memcmp(&sample.output,&output,sizeof output) ||
           (sample.active && (sample.samples!=pointer || sample.bytes!=(words?2u*words:131072u)))) {
            fprintf(stderr,"sample case %u has different output payload\n",test);return 1;
        }
    }
    /* An actual loaded square wave: verify signed PCM, stereo routing and
     * exact rational PAL averaging against a periodic prefix-area formula.
     * Host chunk size must not change output, phase, requests or game RAM. */
    for(unsigned channel=0;channel<4;++channel) {
        memcpy(m->chip,data,0x80000);memcpy(m->slow,data+0x80000,0x80000);
        for(unsigned c=0;c<4;++c) wr_u32(VOICE_SLOTS+4*c,0);
        gaddr voice=rd_u32(SOUND_VOICES+4*SOUND_PROGRAMMED),samples=rd_u32(voice);
        wr_u32(voice+8,300u<<16);wr_u32(voice+12,21u<<16);wr_u32(voice+16,0xffffffff);
        wr_u32(VOICE_SLOTS+4*channel,voice);wr_u32(MASTER_VOLUME,0x3f0000);
        SampleOwner whole_owner={m,0},split_owner={m,0};
        NativeAudio whole={.resolve=sample_bytes,.sample_context=&whole_owner},
            split={.resolve=sample_bytes,.sample_context=&split_owner};
        int16_t first[1940],second[1940];
        memcpy(before,m,sizeof *m);native_audio_bind(&whole);native_audio_request_channel((int)channel);
        native_audio_render(&whole,first,970,48000);
        int64_t prefix[33]={0};
        for(unsigned i=0;i<32;++i) prefix[i+1]=prefix[i]+(int8_t)rd_u8(samples+i)*21;
        const uint64_t byte_time=300u*48000u,loop_time=32*byte_time;
        for(unsigned frame=0;frame<970;++frame) {
            int64_t areas[2];
            for(unsigned edge=0;edge<2;++edge) {
                uint64_t at=(uint64_t)(frame+edge)*3546895,remainder=at%loop_time;
                unsigned index=(unsigned)(remainder/byte_time);
                areas[edge]=(int64_t)(at/loop_time)*prefix[32]*(int64_t)byte_time+
                    prefix[index]*(int64_t)byte_time+
                    (int8_t)rd_u8(samples+index)*21*(int64_t)(remainder%byte_time);
            }
            int16_t value=(int16_t)((areas[1]-areas[0])/3546895*2);
            unsigned side=channel==0 || channel==3?0:1;
            if(first[2*frame+side]!=value || first[2*frame+1-side]!=0) {
                fprintf(stderr,"PCM routing/pitch differs at channel %u frame %u\n",channel,frame);return 1;
            }
        }
        memcpy(expected,m->chip,0x80000);memcpy(expected+0x80000,m->slow,0x80000);
        memcpy(m,before,sizeof *m);native_audio_bind(&split);native_audio_request_channel((int)channel);
        for(unsigned frame=0;frame<970;) {
            unsigned count=frame%53+1;if(count>970-frame) count=970-frame;
            native_audio_render(&split,second+2*frame,count,48000);frame+=count;
        }
        if(memcmp(first,second,sizeof first) || whole.sample_requests!=split.sample_requests ||
           whole_owner.resolutions!=whole.sample_requests || split_owner.resolutions!=split.sample_requests ||
           whole.streams[channel].cursor!=split.streams[channel].cursor ||
           whole.streams[channel].phase!=split.streams[channel].phase ||
           memcmp(m->chip,expected,0x80000) || memcmp(m->slow,expected+0x80000,0x80000)) {
            fputs("Host PCM block partition changed playback or game state\n",stderr);return 1;
        }
    }
    native_audio_bind(NULL);
    puts("256 complete C500D8 sample requests match original non-stack RAM and output payloads");
    puts("Four disk-backed PCM streams preserve PAL pitch, signed samples, stereo routing and block partitioning");
    free(expected);free(before);free(m);free(data);free(state);free(rom);return 0;
}
