#include "frame_capture.h"
#include <stdio.h>
#include <stdlib.h>

static void write_data(const NativeFrontend *game,const NativeFrameCapture *capture,const char *suffix) {
    char path[4096];
    int length=capture->count==1
        ? snprintf(path,sizeof path,"%s.%s.dat",capture->prefix,suffix)
        : snprintf(path,sizeof path,"%s.%u.%s.dat",capture->prefix,
                   capture->iteration+capture->captured,suffix);
    if(length<0 || length>=(int)sizeof path) {
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
    if(capture->complete || capture->replay->iteration!=capture->iteration+capture->captured) return;
    if(boundary==NATIVE_FRAME_INPUT_BEGIN) {
        write_data(game,capture,"entry");
        if(capture->entry_only) capture->complete=++capture->captured==capture->count;
    } else if(capture->entry_only) {
        return;
    } else if(boundary==NATIVE_FRAME_BODY_BEGIN) {
        if(capture->begun) {fputs("Native frame body began twice\n",stderr);abort();}
        capture->begun=1;capture->before_tick=game->ticks;capture->saved_tick=saved_tick;
        write_data(game,capture,"before");
    } else if(capture->begun) {
        capture->owner_exit=boundary==NATIVE_FRAME_OWNER_EXIT;
        capture->after_tick=game->ticks;
        write_data(game,capture,"after");capture->begun=0;
        capture->complete=++capture->captured==capture->count;
    }
}
