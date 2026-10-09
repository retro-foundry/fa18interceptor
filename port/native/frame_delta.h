#ifndef FA18_FRAME_DELTA_H
#define FA18_FRAME_DELTA_H
#include "flight_trace.h"

/* Optional host diagnostics. Allocated and opened before the gameplay heap
 * lock; snapshots use fixed storage and never feed bytes back into the game. */
typedef struct {
    FILE *file;
    char file_buffer[4096];
    uint8_t previous[0x100000];
    size_t bytes,budget;
    unsigned snapshots;
    int failed;
} FA18FrameDelta;
int fa18_frame_delta_open(FA18FrameDelta *trace,const char *path,size_t budget);
int fa18_frame_delta_write(FA18FrameDelta *trace,unsigned iteration,unsigned frame,
    unsigned boundary,uint16_t saved_tick,FA18FlightTraceReader reader,void *context);
int fa18_frame_delta_close(FA18FrameDelta *trace);
#endif
