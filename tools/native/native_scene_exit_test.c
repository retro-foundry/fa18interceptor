/* The actual native frontend must propagate C0DA38's enclosing-frame exit.
 * Validation supplies the alternate-path flag; disk/input create game state. */
#include "native/frontend.h"
#include "../../port/native/frame_capture.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    NativeFrameCapture capture;
    unsigned hud_frames,control_frames,scene_frames,glyphs;
} ExitFixture;
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,uint16_t saved_tick,void *context) {
    ExitFixture *fixture=context;
    if(boundary==NATIVE_FRAME_BODY_BEGIN && fixture->capture.replay->iteration==fixture->capture.iteration) {
        wr_u16(UPDATE_DISPLAY_FLAGS,rd_u16(UPDATE_DISPLAY_FLAGS)|0x2000u);
        /* C32CEE would decrement this source message delay. It must be skipped. */
        wr_u16(0xc45744u,10);wr_u16(0xc4574au,0);wr_u8(0xc457c6u,0);wr_u8(0xc45871u,0);
        fixture->hud_frames=game->hud_frames;fixture->control_frames=game->control_frames;
        fixture->scene_frames=game->scene_frames;fixture->glyphs=game->glyphs;
    }
    native_frame_capture(game,boundary,saved_tick,&fixture->capture);
}
int main(int argc,char **argv) {
    if(argc!=5) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);NativeReplay replay={0};
    ExitFixture fixture={0};char error[256];int result=1;
    if(!game) return 1;
    if(!native_replay_load(&replay,argv[2],error,sizeof error) ||
       !native_frontend_open(game,argv[1],argv[3],error,sizeof error)) {
        fprintf(stderr,"%s\n",error);goto done;
    }
    fixture.capture=(NativeFrameCapture){.replay=&replay,.prefix=argv[4],.iteration=2365,.count=1};
    game->begin_update=native_replay_update;game->update_context=&replay;
    game->observe_frame=observe;game->frame_context=&fixture;
    while(game->ticks<12000 && !fixture.capture.complete) {
        if(game->ticks==1800) native_frontend_event(game,32,1);
        if(game->ticks==1802) native_frontend_event(game,32,0);
        native_frontend_tick(game);
    }
    if(!fixture.capture.complete || !fixture.capture.owner_exit || game->flight_timer_pending ||
       game->hud_frames!=fixture.hud_frames || game->control_frames!=fixture.control_frames ||
       game->scene_frames!=fixture.scene_frames || game->glyphs!=fixture.glyphs ||
       rd_u16(UPDATE_STAGE_MARKER)!=0x60 || rd_u16(UPDATE_TICK)!=fixture.capture.saved_tick ||
       rd_u16(0xc45744u)!=10 || rd_u16(CORNER_RECORDS)!=4 || !game->display_pending) {
        fputs("Native alternate selection did not exit before HUD/timer/counter/message and resume display\n",stderr);goto done;
    }
    printf("{\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"owner_exit\":true}\n",
           fixture.capture.before_tick,fixture.capture.after_tick,fixture.capture.saved_tick);
    result=0;
done:
    native_replay_close(&replay);native_frontend_close(game);free(game);return result;
}
