/* Actual menu/mission paths, sharing the playable runtime's objects.
 * Capture the first body at each source stage and each playback stream. */
#include "native/frontend.h"
#include "../../port/native/frame_capture.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    unsigned eject,ejection;
    unsigned weapon,launch_bodies,launched,removed;
    unsigned projectile_states[3];
    uint16_t stock,ammo,counters[3];
    int weapon_baseline;
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
        if(run->weapon && stage==0xc10dae && !run->weapon_baseline) {
            run->weapon_baseline=1;
            run->stock=rd_u8(CONTROL_RECORDS+95);
            run->ammo=rd_u16(CONTROL_RECORDS+96);
            gaddr log=rd_u32(MODE_TABLE);
            run->counters[0]=rd_u16(log+58);
            run->counters[1]=rd_u16(log+62);
            run->counters[2]=rd_u16(log+66);
        }
        if(run->weapon && game->ticks>=12000) {
            if(run->launch_bodies) {--run->launch_bodies;sample|=8;}
            for(unsigned slot=1;slot<=3;++slot) {
                const gaddr record=CONTROL_RECORDS+slot*CONTROL_RECORD_BYTES;
                if(rd_u8(record+98)>1) continue;
                const int16_t lifetime=rd_s16(record+76);
                const unsigned state=(rd_u8(record+1)&0x40)?
                    (lifetime<0?1u:lifetime==0?2u:lifetime==1?4u:lifetime<12?8u:
                     lifetime<50?16u:lifetime<100?32u:64u):128u;
                if(state!=128) run->launched|=1u<<slot;
                else if(run->launched&(1u<<slot)) run->removed|=1u<<slot;
                if(!(run->projectile_states[slot-1]&state)) {
                    run->projectile_states[slot-1]|=state;sample|=8;
                }
            }
        }
        if(run->eject && rd_u8(BAR_E_FLAG)) {
            run->ejection|=1;
            if(!rd_u16(CONTEXT_RECORD)) sample|=8; /* Sound and the first clone body. */
            else {
                run->ejection|=2;
                const gaddr record=CONTROL_RECORDS+(gaddr)(int32_t)rd_s16(CONTEXT_RECORD);
                const int16_t lifetime=rd_s16(record+0x4c);
                const unsigned bit=4u<<(lifetime<0?0:lifetime>=5?6:(unsigned)lifetime+1);
                if(!(run->ejection&bit)) {run->ejection|=bit;sample|=8;}
            }
        }
        if(run->mode==125 || run->mode==6 || run->mode==3 || run->mode==4 || run->mode==5 || run->mode==7 || run->mode==8) {
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
    if(argc<4 || argc>7) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);char error[256];int result=1;
    ModeRun run={.prefix=argv[3],.mode=argc>=5?(unsigned)atoi(argv[4]):2};
    unsigned aircraft=argc>=6?(unsigned)atoi(argv[5]):1;
    if(argc==7) {
        if(run.mode!=8) return 1;
        if(!strcmp(argv[6],"eject")) run.eject=1;
        else if(!strncmp(argv[6],"weapon",6) && strlen(argv[6])==7 && argv[6][6]>='1' && argv[6][6]<='3')
            run.weapon=(unsigned)(argv[6][6]-'0');
        else return 1;
    }
    if((run.mode!=2 && run.mode!=3 && run.mode!=4 && run.mode!=5 && run.mode!=6 && run.mode!=7 && run.mode!=8 && run.mode!=125) ||
       aircraft<1 || aircraft>2) return 1;
    if(!game) return 1;
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    if(run.mode==7 || run.mode==8) {
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
    const int mission=(run.mode>=3 && run.mode<=5) || run.mode==7 || run.mode==8;
    const unsigned *input_times=mission?mission_times:times;
    const int *input_keys=mission?mission_keys:keys;
    unsigned input_count=run.mode==3?6u:mission?5u:run.mode==125?7u:4u;
    while(game->ticks<((mission || run.mode==125)?18000u:10000u)) {
        if(run.weapon) {
            for(unsigned i=0;i<run.weapon;++i) {
                if(game->ticks==11000+20*i) native_frontend_event(game,13,1);
                if(game->ticks==11002+20*i) native_frontend_event(game,13,0);
            }
            for(unsigned i=0;i<2;++i) {
                if(game->ticks==12000+1000*i) {
                    native_frontend_event(game,32,1);run.launch_bodies=4;
                }
                if(game->ticks==12100+1000*i) native_frontend_event(game,32,0);
            }
        }
        if(run.eject) {
            if(game->ticks==12000) native_frontend_event(game,304,1);
            if(game->ticks==12002) native_frontend_event(game,'E',1);
            if(game->ticks==12004) native_frontend_event(game,'E',0);
            if(game->ticks==12006) native_frontend_event(game,304,0);
        }
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
       (run.eject && (!run.returned || run.ejection!=0x1ff || game->scene_frames<512 ||
        rd_u8(MODE_SELECT)!=0 || rd_u32(STAGE_CALLBACK)!=0xc0fcb4)) ||
       ((!run.eject && (run.mode==4 || run.mode==5 || run.mode==7 || run.mode==8)) && (game->scene_frames<2000 || (run.samples&7)!=7 ||
        rd_u8(RECORDER_MODE)!=0 || rd_u8(POSTFLIGHT_FAILURE_INPUT)!=0x11 ||
        rd_u32(STAGE_CALLBACK)!=0xc10dae))) {
        fprintf(stderr,"Mode %u failed: returned=%d captures=%u scene=%u postflight=%u\n",
            run.mode,run.returned,run.captures,game->scene_frames,game->postflight_callbacks);goto done;
    }
    if(run.weapon) {
        gaddr log=rd_u32(MODE_TABLE);
        const unsigned consumed=(uint16_t)(run.ammo-rd_u16(CONTROL_RECORDS+96));
        const unsigned gun_shots=(uint16_t)(rd_u16(log+58)-run.counters[0]);
        const unsigned first=(uint16_t)(rd_u16(log+62)-run.counters[1]);
        const unsigned second=(uint16_t)(rd_u16(log+66)-run.counters[2]);
        const unsigned stock=rd_u8(CONTROL_RECORDS+95);
        if(!run.weapon_baseline || (run.weapon==3?
           (!consumed || consumed!=gun_shots || first || second || stock!=run.stock):
           (!run.launched || !run.removed || consumed || gun_shots ||
            first!=(run.weapon==1?2u:0u) || second!=(run.weapon==2?2u:0u) ||
            stock!=(unsigned)(run.stock-(run.weapon==1?2u:32u))))) {
            fprintf(stderr,"Weapon %u failed: stock=%u ammo=%u shots=%u/%u/%u launched=%u removed=%u\n",
                run.weapon,stock,consumed,gun_shots,first,second,run.launched,run.removed);goto done;
        }
    }
    result=0;
done:
    if(game) {native_frontend_close(game);free(game);}return result;
}
