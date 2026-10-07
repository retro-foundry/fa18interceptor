/* Actual menu/mission paths, sharing the playable runtime's objects.
 * Capture the first body at each source stage and each playback stream. */
#include "native/frontend.h"
#include "../../port/native/frame_capture.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    NativeFrameCapture capture;
    NativeFrameCapture entry;
    NativeReplay clock;
    const char *prefix;
    char path[4096];
    char entry_path[4096];
    gaddr stages[32];
    gaddr entry_stages[64];
    unsigned entry_count,entry_exports;
    unsigned stage_count,captures,streams;
    unsigned mode,samples;
    int entered,returned;
} ModeRun;
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,
                    uint16_t saved_tick,void *context) {
    ModeRun *run=context;
    run->clock.iteration=game->update_iterations;
    if(boundary==NATIVE_FRAME_INPUT_BEGIN && rd_u8(MODE_SELECT)==run->mode) {
        gaddr stage=rd_u32(STAGE_CALLBACK);
        gaddr key=stage|((rd_s16(POST_INPUT_COUNTDOWN)<=0)?0x1000000u:0);
        if(run->mode==125 && game->ticks>=11000) key|=0x2000000u;
        unsigned index=0;
        while(index<run->entry_count && run->entry_stages[index]!=key) ++index;
        if(index==run->entry_count || game->input_count) {
            if(index==run->entry_count) {
                if(index==64) abort();
                run->entry_stages[run->entry_count++]=key;
            }
            snprintf(run->entry_path,sizeof run->entry_path,"%s.entry.%u",run->prefix,run->entry_exports);
            run->entry=(NativeFrameCapture){.replay=&run->clock,.prefix=run->entry_path,
                .iteration=game->update_iterations,.count=1};
            native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,saved_tick,&run->entry);
            printf("{\"entry\":%u,\"stage\":\"%06X\",\"tick\":%u,\"keys\":[",run->entry_exports++,stage,game->ticks);
            for(unsigned i=0;i<game->input_count;++i)
                printf("%s%u",i?",":"",game->input_keys[(game->input_read+i)%256]);
            puts("]}");
        }
    }
    if(boundary==NATIVE_FRAME_BODY_BEGIN && run->entry.begun)
        native_frame_capture(game,NATIVE_FRAME_BODY_END,saved_tick,&run->entry);
    if(boundary==NATIVE_FRAME_BODY_BEGIN && rd_u8(MODE_SELECT)==run->mode) {
        run->entered=1;
        gaddr stage=rd_u32(STAGE_CALLBACK);
        gaddr key=stage|((run->mode==125 && game->ticks>=11000)?0x2000000u:0);
        unsigned index=0;
        while(index<run->stage_count && run->stages[index]!=key) ++index;
        const unsigned stream=rd_u8(0xc45799u);
        const unsigned bit=stream<8?1u<<stream:0;
        unsigned sample=0;
        if(run->mode==125 || run->mode==6 || run->mode==3 || run->mode==4 || run->mode==5 || run->mode==7) {
            const unsigned frames[]={128,run->mode==6?256u:512u,
                run->mode==6?384u:run->mode==3?768u:run->mode==125?1000u:2000u};
            for(unsigned i=0;i<3;++i)
                if(game->scene_frames>=frames[i] && !(run->samples&(1u<<i))) sample|=1u<<i;
        }
        if(index==run->stage_count || !(run->streams&bit) || sample) {
            if(index==run->stage_count) {
                if(index==32) abort();
                run->stages[run->stage_count++]=key;
            }
            run->streams|=bit;
            run->samples|=sample;
            snprintf(run->path,sizeof run->path,"%s.%u",run->prefix,run->captures);
            run->capture=(NativeFrameCapture){.replay=&run->clock,.prefix=run->path,
                .iteration=game->update_iterations,.count=1};
            printf("{\"capture\":%u,\"stage\":\"%06X\",\"stream\":%u,",run->captures,stage,stream);
        }
    }
    if(!run->capture.prefix || run->capture.complete) return;
    native_frame_capture(game,boundary,saved_tick,&run->capture);
    if(run->capture.complete) {
        printf("\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"owner_exit\":%s}\n",
            run->capture.before_tick,run->capture.after_tick,run->capture.saved_tick,
            run->capture.owner_exit?"true":"false");
        ++run->captures;
    }
}
int main(int argc,char **argv) {
    if(argc<4 || argc>6) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);char error[256];int result=1;
    ModeRun run={.prefix=argv[3],.mode=argc>=5?(unsigned)atoi(argv[4]):2};
    unsigned aircraft=argc==6?(unsigned)atoi(argv[5]):1;
    if((run.mode!=2 && run.mode!=3 && run.mode!=4 && run.mode!=5 && run.mode!=6 && run.mode!=7 && run.mode!=125) ||
       aircraft<1 || aircraft>2) return 1;
    if(!game) return 1;
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    if(run.mode==7) {
        /* A saved-pilot fixture unlocks the original availability byte.
         * Reopen through the normal loader before any gameplay/input; only
         * validation creates this fixture, never the playable runtime. */
        gaddr log=rd_u32(MODE_TABLE);
        if(!rd_u16(log)) goto done;
        wr_u8(log+0x12u+run.mode-1,1);
        native_frontend_save_log(game);
        native_frontend_close(game);
        if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    }
    game->observe_frame=observe;game->frame_context=&run;
    const unsigned times[]={1800,3000,5000,6500,11000,15000,16500};
    const int keys[]={32,run.mode==125?52:run.mode==6?55:51,13,13,27,13,13};
    const unsigned mission_times[]={1800,3000,4500,6500,8000,14500};
    const int mission_keys[]={32,54,282+(int)run.mode-3,13,13,48+(int)aircraft};
    const int mission=(run.mode>=3 && run.mode<=5) || run.mode==7;
    const unsigned *input_times=mission?mission_times:times;
    const int *input_keys=mission?mission_keys:keys;
    unsigned input_count=run.mode==3?6u:mission?5u:run.mode==125?7u:4u;
    while(game->ticks<((mission || run.mode==125)?18000u:10000u)) {
        for(unsigned i=0;i<input_count;++i) {
            if(game->ticks==input_times[i]) native_frontend_event(game,input_keys[i],1);
            if(game->ticks==input_times[i]+2) native_frontend_event(game,input_keys[i],0);
        }
        native_frontend_tick(game);
        if(run.entered && game->screen==NATIVE_MENU && rd_u32(STAGE_CALLBACK)==0xc0fcb4)
            run.returned=1;
    }
    if(run.captures<8 || (run.mode==2 &&
       (!run.returned || game->scene_frames<30 || !game->postflight_callbacks)) ||
       (run.mode==125 && (!run.returned || game->scene_frames<2000 || run.samples!=7 ||
        rd_u32(STAGE_CALLBACK)!=0xc10dae)) ||
       (run.mode==6 && (game->scene_frames<384 || run.samples!=7 ||
        rd_u32(STAGE_CALLBACK)!=0xc10dae)) ||
       (run.mode==3 && (game->scene_frames<768 || run.samples!=7 ||
        rd_u8(RECORDER_MODE)!=0 || rd_u8(0xc45849)!=0x12u-aircraft ||
        rd_u32(STAGE_CALLBACK)!=0xc10dae)) ||
       ((run.mode==4 || run.mode==5 || run.mode==7) && (game->scene_frames<2000 || run.samples!=7 ||
        rd_u8(RECORDER_MODE)!=0 || rd_u8(POSTFLIGHT_FAILURE_INPUT)!=0x11 ||
        rd_u32(STAGE_CALLBACK)!=0xc10dae))) {
        fprintf(stderr,"Mode %u failed: returned=%d captures=%u scene=%u postflight=%u\n",
            run.mode,run.returned,run.captures,game->scene_frames,game->postflight_callbacks);goto done;
    }
    result=0;
done:
    if(game) {native_frontend_close(game);free(game);}return result;
}
