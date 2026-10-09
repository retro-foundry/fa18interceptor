#include "audio.h"
#include "../globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static NativeAudio *active_audio;
int native_pcm_filter_begin(NativePcmFilter *filter,unsigned rate) {
    /* Original rc_calculate_a0, double M_PI and SoftFloat tangent/rounding:
     * 6200/20000 Hz fixed poles, followed by the three 7000 Hz LED poles.
     * Nine significant decimal digits preserve the exact source float bits.
     * Initialization never computes transcendental functions in gameplay. */
    NativePcmFilter fresh={0};fresh.rate=rate;
    if(rate==44100) {
        fresh.fixed_first=0.48603487f;fresh.fixed_second=0.931495488f;fresh.led=0.521334589f;
    } else if(rate==48000) {
        fresh.fixed_first=0.462153852f;fresh.fixed_second=0.881853998f;fresh.led=0.49654904f;
    } else return 0;
    *filter=fresh;return 1;
}
void native_pcm_filter_process(NativePcmFilter *filter,int16_t *stereo,unsigned frames,unsigned rate) {
    if(!filter->rate || filter->rate!=rate) {
        fputs("native PCM filter requires its configured output rate\n",stderr);abort();
    }
    for(unsigned frame=0;frame<frames;++frame) for(unsigned channel=0;channel<2;++channel) {
        NativePcmFilterChannel *history=&filter->channels[channel];
        const int input=stereo[2*frame+channel];
        /* audio.c:filter FILTER_MODEL_A500, led_filter_on=1. Retain float
         * evaluation order and the original double denormal offset. */
        history->rc1=(float)(filter->fixed_first*input+(1.0f-filter->fixed_first)*history->rc1+1E-10);
        history->rc2=filter->fixed_second*history->rc1+(1.0f-filter->fixed_second)*history->rc2;
        history->rc3=filter->led*history->rc2+(1-filter->led)*history->rc3;
        history->rc4=filter->led*history->rc3+(1-filter->led)*history->rc4;
        history->rc5=filter->led*history->rc4+(1-filter->led)*history->rc5;
        int output=(int)history->rc5;
        if(output>32767) output=32767;else if(output<-32768) output=-32768;
        stereo[2*frame+channel]=(int16_t)output;
    }
}
void native_audio_bind(NativeAudio *audio) { active_audio=audio; }
void native_audio_request_channel(int channel) {
    if(channel<0 || channel>=4 || !active_audio) {
        fprintf(stderr,"native sample request without a channel owner: %d\n",channel);
        abort();
    }
    /* C50134-C50150 silences and stops an empty slot before the caller
     * starts another voice. Preserve that stop even when requests share a
     * host block, so the new pitch cannot be applied to the old music buffer. */
    if(!rd_u32(VOICE_SLOTS+4u*(unsigned)channel)) {
        memset(&active_audio->streams[channel],0,sizeof active_audio->streams[channel]);
        active_audio->channels[channel]=(VoiceOutput){PAULA_MIN_PERIOD,0};
        if(active_audio->observe) {
            NativeAudioEvent event={0};event.kind=NATIVE_AUDIO_STOP;
            event.channel=(unsigned)channel;event.tick=active_audio->ticks;
            event.output_frame=active_audio->sample_frames;
            active_audio->observe(active_audio->observe_context,&event);
        }
    }
    active_audio->pending |= 1u << channel;
}

static void service(NativeAudio *audio,unsigned channel,uint64_t output_frame) {
    const gaddr voice=audio->observe?rd_u32(rd_u32(rd_u32(VOICE_TABLE+4*channel)+4)):0;
    VoiceSample sample=request_voice_sample(channel);
    ++audio->sample_requests;
    audio->channels[channel]=sample.output;
    if(!sample.active) {
        audio->streams[channel].playing=0;
        if(audio->observe) {
            NativeAudioEvent event={NATIVE_AUDIO_REQUEST,channel,audio->ticks,output_frame,voice,sample,{0}};
            audio->observe(audio->observe_context,&event);
        }
        return;
    }
    if(!audio->resolve) {
        fputs("native PCM request without a sample-buffer owner\n",stderr);abort();
    }
    NativePcmBuffer buffer={audio->resolve(audio->sample_context,sample.samples&~1u,sample.bytes),sample.bytes};
    if(!buffer.data) { fputs("native PCM sample-buffer owner returned no bytes\n",stderr);abort(); }
    if(audio->observe) {
        NativeAudioEvent event={NATIVE_AUDIO_REQUEST,channel,audio->ticks,output_frame,voice,sample,buffer};
        audio->observe(audio->observe_context,&event);
    }
    if(!audio->streams[channel].playing) {
        audio->streams[channel].current=buffer;
        audio->streams[channel].cursor=0;
        audio->streams[channel].phase=0;
        audio->streams[channel].playing=1;
        /* Original startup requests its next buffer as the initial buffer
         * is fetched (reference audio.c state 1 -> 5). Preserve this priming
         * request rather than counting it as a played loop. */
        service(audio,channel,output_frame);
        audio->streams[channel].period=(uint16_t)audio->channels[channel].period;
    } else audio->streams[channel].next=buffer;
}

static void publish(void *context, gaddr destination, VoiceOutput output) {
    NativeAudio *audio=context;
    /* Disk hunk 74's four descriptors identify outputs DFF0A0..DFF0D0.
     * Resolve that asset identity to ordinary host channel state. There is
     * no register bank, DMA engine, interrupt controller or bus here. */
    for(unsigned channel=0;channel<4;++channel) {
        if(destination==0xdff0a0u+16*channel) {
            audio->channels[channel]=output;
            ++audio->publications;
            return;
        }
    }
    fprintf(stderr,"native voice output has unknown descriptor: %08X\n",destination);
    abort();
}

void native_audio_tick(NativeAudio *audio) {
    advance_voice_channels(publish,audio);
    ++audio->ticks;
}

void native_audio_render(NativeAudio *audio,int16_t *stereo,unsigned frames,unsigned rate) {
    if(!rate) { fputs("native sample rate must be nonzero\n",stderr);abort(); }
    for(unsigned c=0;c<4;++c) if(audio->pending & (1u<<c)) service(audio,c,audio->sample_frames);
    audio->pending=0;
    /* Source reference audio.c anti_prehandler/samplexx_anti_handler average
     * the signed, volume-scaled signal over each output interval. Integrate
     * exactly in rational PAL clock units, including byte/buffer handoffs;
     * truncate each channel before the original stereo sum and x2 gain.
     * Channel pairs 0/3 left and 1/2 right are the original stereo wiring. */
    for(unsigned frame=0;frame<frames;++frame) {
        int left=0,right=0;
        for(unsigned c=0;c<4;++c) {
            if(!audio->streams[c].playing) continue;
            uint64_t remaining=3546895u;
            int64_t area=0;
            while(remaining && audio->streams[c].playing) {
                uint64_t duration=(uint64_t)audio->streams[c].period*rate;
                if(!duration) duration=65536ull*rate;
                uint64_t amount=duration-audio->streams[c].phase;
                if(amount>remaining) amount=remaining;
                unsigned volume=(uint16_t)audio->channels[c].volume;
                volume=volume&64u?64u:volume&63u;
                const int value=audio->streams[c].current.data[audio->streams[c].cursor];
                area+=(int64_t)value*(int)volume*(int64_t)amount;
                remaining-=amount;audio->streams[c].phase+=amount;
                if(audio->streams[c].phase<duration) continue;
                audio->streams[c].phase=0;
                if(++audio->streams[c].cursor==audio->streams[c].current.bytes) {
                    audio->streams[c].current=audio->streams[c].next;
                    audio->streams[c].cursor=0;
                    service(audio,c,(uint64_t)audio->sample_frames+frame+1);
                }
                /* Period writes take effect at the next source byte. */
                audio->streams[c].period=(uint16_t)audio->channels[c].period;
            }
            /* Preserve the existing stopped stream's carried byte phase;
             * the unplayed remainder contributes silence to this average. */
            if(!audio->streams[c].playing) audio->streams[c].phase+=remaining;
            int output=(int)(area/3546895)*2;
            if(c==0 || c==3) left+=output; else right+=output;
        }
        stereo[2*frame]=(int16_t)left;stereo[2*frame+1]=(int16_t)right;
    }
    if(audio->output_filter.rate) native_pcm_filter_process(&audio->output_filter,stereo,frames,rate);
    for(unsigned frame=0;frame<frames;++frame)
        if(stereo[2*frame] || stereo[2*frame+1]) ++audio->nonzero_frames;
    audio->sample_frames+=frames;
}
