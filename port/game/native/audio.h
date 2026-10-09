#ifndef FA18_NATIVE_AUDIO_H
#define FA18_NATIVE_AUDIO_H
#include "../audio.h"

/* C500D8 supplies an address/length at a buffer request. Resolve that identity
 * once into an owned host span; PCM byte playback does not use game addresses. */
typedef const int8_t *(*NativePcmResolve)(void *context,gaddr address,uint32_t bytes);
typedef struct { const int8_t *data; uint32_t bytes; } NativePcmBuffer;
enum NativeAudioEventKind { NATIVE_AUDIO_REQUEST, NATIVE_AUDIO_STOP };
typedef struct {
    enum NativeAudioEventKind kind;
    unsigned channel,tick;
    uint64_t output_frame;
    gaddr voice;
    VoiceSample sample;
    NativePcmBuffer buffer;
} NativeAudioEvent;
typedef void (*NativeAudioObserver)(void *context,const NativeAudioEvent *event);

typedef struct {
    VoiceOutput channels[4];
    unsigned ticks, publications;
    struct {
        NativePcmBuffer current, next;
        uint32_t cursor;
        uint64_t phase;
        uint16_t period;
        int playing;
    } streams[4];
    NativePcmResolve resolve;
    void *sample_context;
    NativeAudioObserver observe;
    void *observe_context;
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
