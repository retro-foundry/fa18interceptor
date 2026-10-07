/* Real keyboard-driven Free Flight, sharing every runtime object with the
 * playable runner. Capture launch and expiry bodies for original comparison. */
#include "native/frontend.h"
#include "native/control_effects.h"
#include "native/input.h"
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
static int fd_input(NativeFrontend *game,CountermeasureFixture *fixture,unsigned variant) {
    const uint8_t raw=(uint8_t)(0x50+variant*9);
    wr_u8(RECORDER_MODE,0xfd);wr_u8(MODE_SELECT,1);
    wr_u8(ORIGIN_DETAIL_MODE,0);wr_u8(COMMAND_EVENT_COUNTER,1);
    wr_u8(KEY_STATE,0);wr_u8(KEY_STATE+1,0);wr_u8(KEY_TAKEN,0);
    wr_u32(EXTERNAL_INPUT_HANDLE,0x6400);wr_u32(KEYBOARD_INPUT_HANDLE,0x6500);
    game->joystick_directions=0;game->mouse_buttons=0;
    snprintf(fixture->path,sizeof fixture->path,"%s.fd.%u",fixture->prefix,variant);
    NativeFrameCapture capture={.replay=&fixture->clock,.prefix=fixture->path,
        .iteration=fixture->clock.iteration,.count=1};
    /* These files bracket C0F3C4 alone, with a controlled recorder mode. */
    native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,0,&capture);
    native_input_enqueue_raw(game,raw);native_input_process(game);
    native_frame_capture(game,NATIVE_FRAME_BODY_END,0,&capture);
    printf("{\"fd_input\":%u,\"raw\":%u}\n",variant,raw);
    return capture.complete && game->input_count==0;
}
static int collision_parent(NativeFrontend *game,CountermeasureFixture *fixture,unsigned variant) {
    const gaddr record=0xc45c72u,target=CONTROL_RECORDS+14*512;
    for(unsigned i=0;i<0x500;++i) wr_u8(record+i,0);
    wr_u8(MISSION_FLAGS_A,0);wr_u8(0xc46201u,0);
    wr_u8(0xc457bdu,0);wr_u8(0xc457aeu,0);
    wr_u8(0xc4585eu,1);wr_u32(0xc459c6u,0x6500);
    wr_u16(0x6500,0x0e10);wr_u32(0x6502,0x6600);wr_u32(0x6604,0x6700);
    wr_u16(0x6700,0);wr_u8(0x6707,variant&1?16:0);
    wr_u16(target,rd_u16(target)|0x40);wr_u8(target+123,0);
    wr_u16(record+48,rd_u16(target+6));wr_u16(record+50,rd_u16(target+8));
    wr_u32(record,(uint32_t)(int32_t)rd_s16(target+12)<<8);
    wr_u32(record+4,(rd_u32(target+16)<<8)+(variant>=2?0x10000:0));
    wr_u32(record+8,(uint32_t)(int32_t)rd_s16(target+14)<<8);
    wr_u16(record+38,0x401);wr_u16(record+40,10);
    snprintf(fixture->path,sizeof fixture->path,"%s.collision.%u",fixture->prefix,variant);
    NativeFrameCapture capture={.replay=&fixture->clock,.prefix=fixture->path,
        .iteration=fixture->clock.iteration,.count=1};
    /* These files bracket C1518C alone; they are not complete frame bodies. */
    native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,0,&capture);
    native_control_effects();
    native_frame_capture(game,NATIVE_FRAME_BODY_END,0,&capture);
    printf("{\"control_parent\":%u,\"collision_hit\":%s}\n",variant,
           rd_u16(record+38)&0x10?"true":"false");
    return capture.complete;
}
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
    for(unsigned i=0;i<2;++i) if(!fd_input(game,&fixture,i)) goto done;
    for(unsigned i=0;i<4;++i) if(!collision_parent(game,&fixture,i)) goto done;
    result=0;
done:
    if(game) {native_frontend_close(game);free(game);}return result;
}
