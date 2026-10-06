#ifndef FA18_NATIVE_AUDIO_H
#define FA18_NATIVE_AUDIO_H
#include "../audio.h"

typedef struct {
    VoiceOutput channels[4];
    unsigned ticks, publications;
} NativeAudio;

/* C5002A registers C50158 on PAL vertical blank at priority -120.
 * Run after C1718E (priority zero), including suspended game updates. */
void native_audio_tick(NativeAudio *audio);
#endif
