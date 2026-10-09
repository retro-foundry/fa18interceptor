#ifndef FA18_NATIVE_AUDIO_TRACE_H
#define FA18_NATIVE_AUDIO_TRACE_H
#include "../game/native/frontend.h"
#include <stdio.h>
typedef struct {
    FILE *file;
    char file_buffer[4096];
    size_t bytes,budget;
    unsigned requests,stops,boundaries;
    int failed;
} NativeAudioTrace;
int native_audio_trace_open(NativeAudioTrace *trace,const char *path,size_t budget);
void native_audio_trace_event(void *context,const NativeAudioEvent *event);
int native_audio_trace_boundary(NativeAudioTrace *trace,NativeFrontend *game,unsigned iteration);
int native_audio_trace_close(NativeAudioTrace *trace);
#endif
