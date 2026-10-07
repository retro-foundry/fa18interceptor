/* SDL key queue -> playable host boundary -> real Free Flight input/body. */
#include "native/frontend.h"
#include "native/menu.h"
#include "../../port/native/host_input.h"
#include "../../port/native/frame_capture.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    NativeFrameCapture entry,body;
    NativeReplay clock;
    const char *prefix;
    char entry_path[4096],body_path[4096];
    unsigned entries,bodies,pending_bodies,views,left_seen,right_seen,releases;
    int left_axis,right_axis;
} HostKeysRun;

static int key_event(NativeFrontend *game,int key,int down,unsigned modifiers,int repeat) {
    SDL_Event event={0};
    event.type=down?SDL_KEYDOWN:SDL_KEYUP;
    event.key.state=down?SDL_PRESSED:SDL_RELEASED;
    event.key.repeat=(uint8_t)repeat;
    event.key.keysym.sym=key;event.key.keysym.mod=(uint16_t)modifiers;
    event.key.keysym.scancode=SDL_GetScancodeFromKey(key);
    if(SDL_PushEvent(&event)!=1) return 0;
    while(SDL_PollEvent(&event))
        if(event.type==SDL_KEYDOWN || event.type==SDL_KEYUP)
            native_host_keyboard_event(game,&event.key);
    return 1;
}
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,uint16_t saved_tick,void *context) {
    HostKeysRun *run=context;
    run->clock.iteration=game->update_iterations;
    if(boundary==NATIVE_FRAME_INPUT_BEGIN && game->ticks>=6000 && game->input_count) {
        snprintf(run->entry_path,sizeof run->entry_path,"%s.entry.%u",run->prefix,run->entries);
        run->entry=(NativeFrameCapture){.replay=&run->clock,.prefix=run->entry_path,
            .iteration=game->update_iterations,.count=1};
        native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,saved_tick,&run->entry);
        printf("{\"entry\":%u,\"tick\":%u,\"keys\":[",run->entries++,game->ticks);
        for(unsigned i=0;i<game->input_count;++i)
            printf("%s%u",i?",":"",game->input_keys[(game->input_read+i)%256]);
        puts("]}");run->pending_bodies=2;
    }
    if(boundary==NATIVE_FRAME_BODY_BEGIN) {
        if(run->entry.begun) native_frame_capture(game,NATIVE_FRAME_BODY_END,saved_tick,&run->entry);
        if(game->ticks>=6000) {
            const uint8_t rudder=rd_u8(COMMAND_TRIM_INPUT);
            const int axis=rd_s8(CONTROL_RECORDS+41);
            if(rudder==0x80) {++run->left_seen;if(axis<run->left_axis) run->left_axis=axis;}
            if(rudder==0x40) {++run->right_seen;if(axis>run->right_axis) run->right_axis=axis;}
            if(!rudder && game->ticks>6200) ++run->releases;
            run->views|=1u<<rd_u8(VIEW_MODE);
        }
        if(run->pending_bodies) {
            --run->pending_bodies;
            snprintf(run->body_path,sizeof run->body_path,"%s.%u",run->prefix,run->bodies);
            run->body=(NativeFrameCapture){.replay=&run->clock,.prefix=run->body_path,
                .iteration=game->update_iterations,.count=1};
        }
    }
    if(!run->body.prefix || run->body.complete) return;
    native_frame_capture(game,boundary,saved_tick,&run->body);
    if(run->body.complete) {
        printf("{\"capture\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u}\n",
            run->bodies++,run->body.before_tick,run->body.after_tick,run->body.saved_tick);
    }
}
static int mapping(void) {
    static const int keys[]={SDLK_COMMA,SDLK_PERIOD,SDLK_KP_0,SDLK_KP_1,SDLK_KP_2,
        SDLK_KP_3,SDLK_KP_4,SDLK_KP_5,SDLK_KP_6,SDLK_KP_7,SDLK_KP_8,SDLK_KP_9,
        SDLK_KP_PERIOD,SDLK_KP_DIVIDE,SDLK_KP_MULTIPLY,SDLK_KP_MINUS,SDLK_KP_PLUS,SDLK_KP_ENTER};
    static const uint8_t raw[]={0x38,0x39,0x0f,0x1d,0x1e,0x1f,0x2d,0x2e,0x2f,
        0x3d,0x3e,0x3f,0x3c,0x5c,0x5d,0x4a,0x5e,0x43};
    for(unsigned i=0;i<sizeof keys/sizeof *keys;++i)
        if(native_menu_raw_key(keys[i],1)!=raw[i] || native_menu_raw_key(keys[i],0)!=(raw[i]|0x80u)) return 0;
    for(unsigned i=2;i<12;++i)
        if(native_menu_raw_key(256+(int)i-2,1)!=raw[i]) return 0;
    return native_menu_raw_key(SDLK_UNKNOWN,1)==0xff;
}
int main(int argc,char **argv) {
    if(argc!=4 || !mapping()) return 1;
    SDL_SetMainReady();
    if(SDL_Init(SDL_INIT_EVENTS)) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);char error[256];int result=1;
    HostKeysRun run={.prefix=argv[3]};
    if(!game) goto done;
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    game->observe_frame=observe;game->frame_context=&run;
    const unsigned startup[]={1800,3000,4100,5000,5400};
    const int menu_keys[]={SDLK_SPACE,SDLK_2,SDLK_RETURN,SDLK_2,SDLK_1};
    const int keypad[]={SDLK_KP_2,SDLK_KP_4,SDLK_KP_6,SDLK_KP_1,SDLK_KP_7,
        SDLK_KP_8,SDLK_KP_9,SDLK_KP_5,SDLK_KP_3,SDLK_KP_PERIOD,SDLK_KP_0};
    while(game->ticks<7500) {
        for(unsigned i=0;i<sizeof startup/sizeof *startup;++i) {
            if(game->ticks==startup[i] && !key_event(game,menu_keys[i],1,0,0)) goto done;
            if(game->ticks==startup[i]+2 && !key_event(game,menu_keys[i],0,0,0)) goto done;
        }
        if(game->ticks==6000) {
            if(rd_u32(STAGE_CALLBACK)!=0xc10dae || !key_event(game,SDLK_COMMA,1,0,1) || game->input_count) goto done;
        }
        if(game->ticks==6200 && !key_event(game,SDLK_COMMA,1,0,0)) goto done;
        if(game->ticks==6280 && !key_event(game,SDLK_COMMA,0,0,0)) goto done;
        if(game->ticks==6360 && !key_event(game,SDLK_PERIOD,1,0,0)) goto done;
        if(game->ticks==6440 && !key_event(game,SDLK_PERIOD,0,0,0)) goto done;
        for(unsigned i=0;i<sizeof keypad/sizeof *keypad;++i) {
            const unsigned tick=6520+80*i,modifiers=(i&1)?KMOD_NUM:0;
            if(game->ticks==tick && !key_event(game,keypad[i],1,modifiers,0)) goto done;
            if(game->ticks==tick+8 && !key_event(game,keypad[i],0,modifiers,0)) goto done;
        }
        native_frontend_tick(game);
    }
    const unsigned expected_views=(1u<<0)|(1u<<5)|(1u<<6)|(1u<<12)|(1u<<13);
    if(run.entries!=26 || run.bodies!=52 || !run.left_seen || !run.right_seen ||
        !run.releases || run.left_axis>=0 || run.right_axis<=0 ||
        (run.views&expected_views)!=expected_views || game->input_count || rd_u8(COMMAND_TRIM_INPUT)) {
        fprintf(stderr,"Host controls failed: entries=%u bodies=%u rudder=%u/%u axis=%d/%d views=%X\n",
            run.entries,run.bodies,run.left_seen,run.right_seen,run.left_axis,run.right_axis,run.views);goto done;
    }
    result=0;
done:
    if(game) {native_frontend_close(game);free(game);}SDL_Quit();return result;
}
