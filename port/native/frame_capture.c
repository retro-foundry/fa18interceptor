#include "frame_capture.h"
#include <stdio.h>
#include <stdlib.h>

static void write_data(const NativeFrontend *game,const char *prefix,const char *suffix) {
    char path[4096];
    if(snprintf(path,sizeof path,"%s.%s.dat",prefix,suffix)>=(int)sizeof path) {
        fputs("Native frame capture path is too long\n",stderr);abort();
    }
    FILE *file=fopen(path,"wb");
    if(!file) {perror(path);abort();}
    int written=fwrite(game->storage.buffers,1,0x80000,file)==0x80000 &&
                fwrite(game->storage.source,1,0x80000,file)==0x80000;
    if(fclose(file) || !written) {
        fprintf(stderr,"Cannot write native frame capture: %s\n",path);abort();
    }
}
void native_frame_capture(NativeFrontend *game,enum NativeFrameBoundary boundary,
                          uint16_t saved_tick,void *context) {
    NativeFrameCapture *capture=context;
    if(capture->complete || capture->replay->iteration!=capture->iteration) return;
    if(boundary==NATIVE_FRAME_BODY_BEGIN) {
        if(capture->begun) {fputs("Native frame body began twice\n",stderr);abort();}
        capture->begun=1;capture->before_tick=game->ticks;capture->saved_tick=saved_tick;
        write_data(game,capture->prefix,"before");
    } else if(capture->begun) {
        capture->after_tick=game->ticks;
        write_data(game,capture->prefix,"after");capture->complete=1;
    }
}
