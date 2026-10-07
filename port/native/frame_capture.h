#ifndef FA18_NATIVE_FRAME_CAPTURE_H
#define FA18_NATIVE_FRAME_CAPTURE_H
#include "replay.h"
typedef struct {
    NativeReplay *replay;
    const char *prefix;
    unsigned iteration,count,captured,before_tick,after_tick;
    uint16_t saved_tick;
    int begun,complete,owner_exit,entry_only;
} NativeFrameCapture;
void native_frame_capture(NativeFrontend *game,enum NativeFrameBoundary boundary,
                          uint16_t saved_tick,void *context);
#endif
