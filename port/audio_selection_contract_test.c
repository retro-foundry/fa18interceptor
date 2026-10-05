#include "audio_selection.h"
#include "audio_update.h"
#include <assert.h>

typedef struct { unsigned count,channels[8]; } Acks;
static void acknowledge(void *context,unsigned channel,uint16_t mask) {
    Acks *acks=context;
    assert(acks->count<8 && mask==(uint16_t)(0x80u<<channel));
    acks->channels[acks->count++]=channel;
}
static int output(void *context,FA18AudioParameter parameter,uint16_t value) {
    unsigned *calls=context;
    assert(value==(parameter==FA18_AUDIO_PERIOD?300:63));
    ++*calls;
    return 1;
}
int main(void) {
    FA18CommandAudio audio={0};
    PortVoice a={0},b={0},*voices[37]={0};
    FA18AudioUpdate update={0};
    Acks acks={0}; unsigned i,outputs=0;
    audio.acknowledge=acknowledge; audio.acknowledge_context=&acks;
    for(i=0;i<4;++i) audio.interrupt_masks[i]=(uint16_t)(0x80u<<i);
    voices[13]=voices[35]=&a; voices[14]=voices[36]=&b;
    audio.sound_flags=0x80; audio.effect_flags=4;
    assert(fa18_start_native_menu_sound(&audio,voices,37,50));
    assert(audio.volume_fading==2 && acks.count==8 && audio.slots[0]==&a && audio.slots[1]==&b);
    assert(a.volume==(63u<<16) && b.volume==(63u<<16));
    for(i=0;i<4;++i) assert(acks.channels[i]==i);
    assert(acks.channels[4]==0 && acks.channels[5]==0 && acks.channels[6]==1 && acks.channels[7]==1);
    assert(fa18_start_native_menu_sound(&audio,NULL,0,0) && acks.count==8);
    /* Actual selector -> native update shares the selected voice objects. */
    a.period=b.period=300u<<16;
    audio.master_volume=63u<<16; update.audio=&audio;
    for(i=0;i<4;++i) update.channels[i]=(FA18AudioUpdateChannel){&audio.slots[i],output,&outputs};
    assert(fa18_update_native_audio(&update) && outputs==4);
    audio.volume_fading=0; audio.sound_flags=0; acks.count=0;
    assert(fa18_start_native_menu_sound(&audio,voices,37,0xffff0001));
    assert(acks.count==4 && a.volume==0x10000 && b.volume==0x10000 && audio.volume_fading==2);
    audio.volume_fading=0; audio.effect_flags=0; acks.count=0;
    assert(fa18_start_native_menu_sound(&audio,NULL,0,0) && acks.count==4 && !audio.volume_fading);
    for(i=0;i<4;++i) assert(!audio.slots[i]);
    audio.effect_flags=4; voices[35]=voices[36]=NULL; audio.acknowledge=NULL; acks.count=0;
    assert(fa18_start_native_menu_sound(&audio,voices,37,0) && audio.volume_fading==2 && !acks.count);
    audio.volume_fading=0; voices[35]=&a;
    assert(!fa18_start_native_menu_sound(&audio,voices,37,0) && !audio.volume_fading);
    assert(!fa18_select_native_sound(&audio,voices,37,37,0,0));
    assert(!fa18_release_native_audio_channel(&audio,4));
    return 0;
}
