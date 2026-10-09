/* Observe actual scene constructors and delayed menu transitions. */
#include "native/frontend.h"
#include "native/model.h"
#include "../../port/native/frame_capture.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    NativeFrameCapture entry;
    NativeReplay clock;
    const char *prefix;
    char path[4096];
    unsigned count,tick,key_count,selected_mode;
    uint8_t keys[256];
    uint16_t retained;
    gaddr stage;
    int startup;
} SortRun;

static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,
                    uint16_t saved_tick,void *context) {
    SortRun *run=context;
    run->clock.iteration=game->update_iterations;
    if(boundary==NATIVE_FRAME_INPUT_BEGIN || boundary==NATIVE_STARTUP_SCENE_BEGIN) {
        const gaddr stage=rd_u32(STAGE_CALLBACK);
        if(boundary!=NATIVE_STARTUP_SCENE_BEGIN && stage!=0xc0f920 && stage!=0xc0f992 &&
           (stage!=0xc0fece || rd_s16(POST_INPUT_COUNTDOWN)>0)) return;
        if(run->entry.begun || game->input_count>256) abort();
        const int length=snprintf(run->path,sizeof run->path,"%s.%u",run->prefix,run->count);
        if(length<0 || length>=(int)sizeof run->path) abort();
        run->entry=(NativeFrameCapture){.replay=&run->clock,.prefix=run->path,
            .iteration=game->update_iterations,.count=1};
        run->stage=stage;run->tick=game->ticks;
        run->startup=boundary==NATIVE_STARTUP_SCENE_BEGIN;
        run->selected_mode=rd_u8(MODE_SELECT);
        run->retained=native_model_retained_result();
        run->key_count=game->input_count;
        for(unsigned i=0;i<run->key_count;++i)
            run->keys[i]=game->input_keys[(game->input_read+i)%256];
        native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,saved_tick,&run->entry);
    } else if((boundary==NATIVE_FRAME_BODY_BEGIN || boundary==NATIVE_STARTUP_SCENE_END) && run->entry.begun) {
        native_frame_capture(game,NATIVE_FRAME_BODY_END,saved_tick,&run->entry);
        printf("{\"entry\":%u,\"stage\":\"%06X\",\"startup\":%s,\"original_caller\":\"%06X\",\"selected_mode\":%u,\"tick\":%u,\"retained_before\":%u,\"retained_after\":%u,\"keys\":[",
            run->count++,run->stage,run->startup?"true":"false",run->startup?0xc0f812u:0xc0f5f8u,
            run->selected_mode,run->tick,run->retained,native_model_retained_result());
        for(unsigned i=0;i<run->key_count;++i) printf("%s%u",i?",":"",run->keys[i]);
        puts("]}");
    }
}

int main(int argc,char **argv) {
    if(argc!=5) return 1;
    const unsigned mode=(unsigned)atoi(argv[4]);
    if((mode<2 || mode>9) && mode!=125 && mode!=127) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);
    char error[256];int result=1;
    if(!game) return 1;
    SortRun run={.prefix=argv[3]};
    if(!native_frontend_open_observed(game,argv[1],argv[2],error,sizeof error,observe,&run)) {
        fprintf(stderr,"%s\n",error);goto done;
    }
    const unsigned mission_times[]={1800,3000,4500,6500,8000};
    const int mission_keys[]={32,54,282+(int)mode-3,13,13};
    const unsigned menu_times[]={1800,3000,5000,6500,11000};
    const int menu_keys[]={32,mode==127?49:mode==125?52:mode==9?53:51,13,13,27};
    const int mission=mode>=3 && mode<=8;
    const unsigned *times=mission?mission_times:menu_times;
    const int *keys=mission?mission_keys:menu_keys;
    const unsigned count=mode==127?2u:5u,end=mode==2?18000u:10000u;
    int16_t samples[1920];
    while(game->ticks<end) {
        for(unsigned i=0;i<count;++i) {
            if(game->ticks==times[i]) native_frontend_event(game,keys[i],1);
            if(game->ticks==times[i]+2) native_frontend_event(game,keys[i],0);
        }
        native_frontend_tick(game);
        native_audio_render(&game->audio,samples,960,48000);
    }
    if(!run.count || run.entry.begun) {fputs("No complete scene-sort interval\n",stderr);goto done;}
    result=0;
done:
    native_frontend_close(game);free(game);return result;
}
