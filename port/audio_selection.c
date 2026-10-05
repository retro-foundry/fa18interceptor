#include "audio_selection.h"
#include "voice_selection.h"

static void selection_acknowledge(void *context,unsigned channel) {
    FA18CommandAudio *a=context;
    a->acknowledge(a->acknowledge_context,channel,a->interrupt_masks[channel]);
}

static PortVoiceSelection selection(FA18CommandAudio *a,
                                    PortVoice *const *voices,size_t count) {
    return (PortVoiceSelection){voices,count,a->slots,4,
                               a->acknowledge?selection_acknowledge:NULL,a};
}

int fa18_release_native_audio_channel(FA18CommandAudio *a,unsigned channel) {
    PortVoiceSelection s;
    if(!a) return 0;
    s=selection(a,NULL,0);
    return port_release_voice_channel(&s,channel)==PORT_VOICE_SELECTION_OK;
}

int fa18_select_native_sound(FA18CommandAudio *a,PortVoice *const *voices,
                              size_t count,size_t sound,unsigned channel,uint32_t volume) {
    PortVoiceSelection s;
    if(!a) return 0;
    s=selection(a,voices,count);
    return port_select_voice(&s,sound,channel,volume<<16)==PORT_VOICE_SELECTION_OK;
}

static int release_all(FA18CommandAudio *a) {
    unsigned channel;
    for(channel=0;channel<4;++channel)
        if(!fa18_release_native_audio_channel(a,channel)) return 0;
    return 1;
}

int fa18_start_native_menu_sound(FA18CommandAudio *a,PortVoice *const *voices,
                                  size_t count,uint32_t volume) {
    if(!a) return 0;
    if(a->volume_fading) return 1;
    if(a->sound_flags&0x80u) {
        if(!release_all(a) || !fa18_select_native_sound(a,voices,count,13,0,63) ||
           !fa18_select_native_sound(a,voices,count,14,1,63)) return 0;
    } else if(a->effect_flags&4u) {
        if(!fa18_select_native_sound(a,voices,count,35,0,volume) ||
           !fa18_select_native_sound(a,voices,count,36,1,volume)) return 0;
    } else return release_all(a);
    a->volume_fading=2;
    return 1;
}
