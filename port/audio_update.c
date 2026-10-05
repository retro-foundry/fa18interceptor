#include "audio_update.h"

static int32_t signed_long(uint32_t bits) {
    return bits<0x80000000u?(int32_t)bits:(int32_t)((int64_t)bits-0x100000000LL);
}

static int signed_high_word(uint32_t bits) {
    unsigned word=bits>>16;
    return word<0x8000u?(int)word:(int)word-0x10000;
}

int fa18_import_voice_operations(const uint32_t *selectors,size_t count,
                                  PortVoiceOperation *operations) {
    size_t i;
    if(!selectors || !operations || !count || count>UINT32_MAX/8u) return 0;
    for(i=0;i<count;++i) {
        switch(selectors[i]) {
        case 0x08: operations[i]=PORT_VOICE_SET_PERIOD; break;
        case 0x0c: operations[i]=PORT_VOICE_SET_VOLUME; break;
        case 0x18: operations[i]=PORT_VOICE_SET_PERIOD_SLIDE; break;
        case 0x1c: operations[i]=PORT_VOICE_SET_VOLUME_SLIDE; break;
        case 0x24: operations[i]=PORT_VOICE_SET_LOOP0; break;
        case 0x28: operations[i]=PORT_VOICE_SET_LOOP1; break;
        case 0x2c: operations[i]=PORT_VOICE_WAIT; break;
        case 0x40: operations[i]=PORT_VOICE_LOOP0; break;
        case 0x44: operations[i]=PORT_VOICE_LOOP1; break;
        default: return 0;
        }
    }
    return 1;
}

typedef struct { FA18CommandAudio *audio; unsigned channel; } VoiceEnd;
static void acknowledge_end(void *context) {
    VoiceEnd *end=context;
    end->audio->acknowledge(end->audio->acknowledge_context,end->channel,
                            end->audio->interrupt_masks[end->channel]);
}

int fa18_step_native_voice_program(FA18CommandAudio *a,FA18CommandVoice *v,
                                    FA18CommandVoice **slot,unsigned channel) {
    VoiceEnd end={a,channel};
    if(!a || channel>=4 || !a->acknowledge) return 0;
    return port_step_voice_program(v,slot,acknowledge_end,&end)==PORT_VOICE_OK;
}

int fa18_output_native_voice(FA18CommandAudio *a,const FA18CommandVoice *v,
                              const FA18AudioUpdateChannel *channel) {
    int period,volume,master;
    if(!a || !v || !channel || !channel->output) return 0;
    period=signed_high_word(v->period);
    if(period<124) period=124;
    if(!channel->output(channel->context,FA18_AUDIO_PERIOD,(uint16_t)period)) return 0;
    volume=(int)((v->volume>>16)&63u); master=signed_high_word(a->master_volume);
    if(volume>master) volume=master;
    return channel->output(channel->context,FA18_AUDIO_VOLUME,(uint16_t)volume);
}

int fa18_update_native_audio(FA18AudioUpdate *s) {
    unsigned channel;
    if(!s || !s->audio || !s->audio->acknowledge) return 0;
    for(channel=0;channel<4;++channel) {
        if(!s->channels[channel].slot || !s->channels[channel].output) return 0;
    }
    for(channel=0;channel<4;++channel) {
        FA18CommandVoice *v=*s->channels[channel].slot;
        if(!v) continue;
        if(!fa18_step_native_voice_program(s->audio,v,s->channels[channel].slot,channel) ||
           !fa18_output_native_voice(s->audio,v,&s->channels[channel])) return 0;
        /* Keep the original voice even if program termination cleared its slot. */
        port_advance_voice_slides(v);
    }
    return 1;
}

int fa18_fade_native_master_volume(FA18CommandAudio *a) {
    int32_t level,target;
    uint32_t result;
    if(!a) return 0;
    if(!a->volume_fading || a->master_volume==a->master_volume_target) return 1;
    level=signed_long(a->master_volume); target=signed_long(a->master_volume_target);
    if(level>target) {
        result=a->master_volume-0x4000u;
        /* Source BLT immediately follows SUBI: account for signed overflow. */
        if((int64_t)level-0x4000<0) result=0;
    } else {
        result=a->master_volume+0x4000u;
        if(signed_long(result)>0x3f0000) result=0x3f0000;
    }
    a->master_volume=result;
    return 1;
}
