#ifndef FA18_NATIVE_AUDIO_H
#define FA18_NATIVE_AUDIO_H
#include "../audio.h"

typedef struct {
    VoiceOutput channels[4];
    unsigned ticks, publications;
    struct {
        VoiceSample current, next;
        uint32_t cursor;
        uint64_t phase;
        uint16_t period;
        int playing;
    } streams[4];
    unsigned pending, sample_requests, sample_frames, nonzero_frames;
} NativeAudio;

/* Select the current frontend, as with the existing native data binding. */
void native_audio_bind(NativeAudio *audio);
void native_audio_request_channel(int channel);
void native_audio_render(NativeAudio *audio,int16_t *stereo,unsigned frames,unsigned rate);

/* C5002A registers C50158 on PAL vertical blank at priority -120.
 * Run after C1718E (priority zero), including suspended game updates. */
void native_audio_tick(NativeAudio *audio);
#endif
