/* Real keyboard-driven Free Flight, sharing every runtime object with the
 * playable runner. Capture launch and expiry bodies for original comparison. */
#include "native/frontend.h"
#include "../../port/native/frame_capture.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    NativeFrameCapture capture;
    NativeReplay clock;
    const char *prefix;
    char path[4096];
    unsigned captures,launches,expiry;
} CountermeasureFixture;
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,uint16_t saved_tick,void *context) {
    CountermeasureFixture *fixture=context;
    fixture->clock.iteration=game->update_iterations;
    if(boundary==NATIVE_FRAME_BODY_BEGIN && (!fixture->capture.begun)) {
        int16_t timer=-1;
        for(unsigned i=0;i<10;++i) {
            const gaddr record=0xc45c72u+128*i;
            if(rd_u16(record+38)&1) timer=rd_s16(record+40);
        }
        const int launch=rd_u8(MISSION_FLAGS_A)!=0;
        const int expiry=timer==1;
        if((launch || expiry) && fixture->captures<4) {
            snprintf(fixture->path,sizeof fixture->path,"%s.%u",fixture->prefix,fixture->captures);
            fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                .iteration=game->update_iterations,.count=1};
            fixture->launches+=launch;fixture->expiry+=expiry;
        }
    }
    if(!fixture->capture.prefix || fixture->capture.complete) return;
    native_frame_capture(game,boundary,saved_tick,&fixture->capture);
    if(fixture->capture.complete) {
        printf("{\"capture\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u}\n",
            fixture->captures,fixture->capture.before_tick,fixture->capture.after_tick,fixture->capture.saved_tick);
        ++fixture->captures;
    }
}
int main(int argc,char **argv) {
    if(argc!=4) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);char error[256];int result=1;
    CountermeasureFixture fixture={.prefix=argv[3]};
    if(!game) return 1;
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    game->observe_frame=observe;game->frame_context=&fixture;
    const unsigned times[]={1800,3000,4100,5000,5400,6200,6900};
    const int keys[]={32,50,13,50,49,'F','C'};
    while(game->ticks<7800) {
        for(unsigned i=0;i<sizeof times/sizeof times[0];++i) {
            if(game->ticks==times[i]) native_frontend_event(game,keys[i],1);
            if(game->ticks==times[i]+2) native_frontend_event(game,keys[i],0);
        }
        native_frontend_tick(game);
    }
    if(fixture.captures!=4 || fixture.launches!=2 || fixture.expiry!=2 ||
       rd_u8(MISSION_LEVEL_A)!=15 || rd_u8(MISSION_LEVEL_B)!=15 || rd_u32(STAGE_CALLBACK)!=0xc10dae) {
        fprintf(stderr,"Countermeasure integration failed: captures=%u launch=%u expiry=%u stocks=%u/%u\n",
            fixture.captures,fixture.launches,fixture.expiry,rd_u8(MISSION_LEVEL_A),rd_u8(MISSION_LEVEL_B));goto done;
    }
    result=0;
done:
    if(game) {native_frontend_close(game);free(game);}return result;
}
