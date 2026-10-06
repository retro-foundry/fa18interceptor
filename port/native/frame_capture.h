#ifndef FA18_NATIVE_FRAME_CAPTURE_H
#define FA18_NATIVE_FRAME_CAPTURE_H
#include "replay.h"
typedef struct {
    NativeReplay *replay;
    const char *prefix;
    unsigned iteration,before_tick,after_tick;
    uint16_t saved_tick;
    int begun,complete;
} NativeFrameCapture;
void native_frame_capture(NativeFrontend *game,enum NativeFrameBoundary boundary,
                          uint16_t saved_tick,void *context);
#endif
