#include "audio.h"
#include "../globals.h"
#include <stdio.h>
#include <stdlib.h>

static NativeAudio *active_audio;
void native_audio_bind(NativeAudio *audio) { active_audio=audio; }
void native_audio_request_channel(int channel) {
    if(channel<0 || channel>=4 || !active_audio) {
        fprintf(stderr,"native sample request without a channel owner: %d\n",channel);
        abort();
    }
    active_audio->pending |= 1u << channel;
}

static void service(NativeAudio *audio,unsigned channel) {
    VoiceSample sample=request_voice_sample(channel);
    ++audio->sample_requests;
    audio->channels[channel]=sample.output;
    if(!sample.active) {
        audio->streams[channel].playing=0;
        return;
    }
    if(!audio->streams[channel].playing) {
        audio->streams[channel].current=sample;
        audio->streams[channel].cursor=0;
        audio->streams[channel].phase=0;
        audio->streams[channel].playing=1;
        /* Original startup requests its next buffer as the initial buffer
         * is fetched (reference audio.c state 1 -> 5). Preserve this priming
         * request rather than counting it as a played loop. */
        service(audio,channel);
        audio->streams[channel].period=(uint16_t)audio->channels[channel].period;
    } else audio->streams[channel].next=sample;
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
    for(unsigned c=0;c<4;++c) if(audio->pending & (1u<<c)) service(audio,c);
    audio->pending=0;
    /* Signed source PCM, PAL sample clock 3546895 / period. Integer hold
     * resampling keeps phase across host blocks and observes live pitch.
     * Channel pairs 0/3 left and 1/2 right are the original stereo wiring. */
    for(unsigned frame=0;frame<frames;++frame) {
        int left=0,right=0;
        for(unsigned c=0;c<4;++c) {
            if(!audio->streams[c].playing) continue;
            int value=(int8_t)rd_u8((audio->streams[c].current.samples&~1u)+audio->streams[c].cursor);
            unsigned volume=(uint16_t)audio->channels[c].volume;
            volume=volume&64u?64u:volume&63u;
            int output=value*(int)volume*2; /* Two full-scale channels fit s16. */
            if(c==0 || c==3) left+=output; else right+=output;
            audio->streams[c].phase+=3546895u;
            uint64_t duration=(uint64_t)audio->streams[c].period*rate;
            if(!duration) duration=65536ull*rate;
            while(audio->streams[c].playing && audio->streams[c].phase>=duration) {
                audio->streams[c].phase-=duration;
                if(++audio->streams[c].cursor==audio->streams[c].current.bytes) {
                    audio->streams[c].current=audio->streams[c].next;
                    audio->streams[c].cursor=0;
                    service(audio,c);
                }
                /* Period writes take effect at the next source byte. */
                audio->streams[c].period=(uint16_t)audio->channels[c].period;
                duration=(uint64_t)audio->streams[c].period*rate;
                if(!duration) duration=65536ull*rate;
            }
        }
        if(left || right) ++audio->nonzero_frames;
        stereo[2*frame]=(int16_t)left;stereo[2*frame+1]=(int16_t)right;
    }
    audio->sample_frames+=frames;
}
