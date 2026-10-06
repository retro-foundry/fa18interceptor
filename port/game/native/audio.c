#include "audio.h"
#include <stdio.h>
#include <stdlib.h>

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
