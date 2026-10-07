/* Controlled source postflight conditions after normal mission startup.
 * Only this validation entry seeds terminal flags/positions. The frame body
 * and subsequent callbacks use the playable runner's shared runtime objects. */
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
    const char *prefix,*kind;
    char path[4096];
    char entry_path[4096];
    unsigned seeded,captures,mode,stages[32],stage_count;
    unsigned entry_stages[32],entry_count,entry_exports;
    unsigned phase,event,restored;
} ScheduleFixture;
static void terminal_conditions(ScheduleFixture *fixture) {
    const gaddr first=CONTROL_RECORDS+4*512,second=CONTROL_RECORDS+6*512;
    wr_u8(SCHEDULE_STATUS,rd_u8(SCHEDULE_STATUS)|0x40);
    wr_u8(SCHEDULE_BLOCKED,0);wr_u8(PLAYER_PHASE,0);wr_u8(SEQUENCE_PHASE,0);
    /* C22C80/C23A7E suspend motion while C09E06's terminal countdown runs. */
    wr_u8(POST_INPUT_EVENT,1);
    if(!strcmp(fixture->kind,"four")) {
        wr_u8(first+1,rd_u8(first+1)|0x40);
        wr_u8(first+3,rd_u8(first+3)|0x80);wr_u16(first+108,0);
        wr_u8(CONTROL_RECORDS+8*512+1,rd_u8(CONTROL_RECORDS+8*512+1)&~0x40u);
        wr_u8(CONTROL_RECORDS+10*512+1,rd_u8(CONTROL_RECORDS+10*512+1)&~0x40u);
    } else if(!strcmp(fixture->kind,"five")) {
        /* C0A002's close escort condition uses these actual world positions.
         * The fixture copies the captured escort position to the player and
         * expires the source proximity gate; it supplies no replacement AI. */
        wr_u8(first+1,rd_u8(first+1)|0x48);wr_u8(second+1,rd_u8(second+1)|0x40);
        for(unsigned i=0;i<3;++i) wr_u32(CONTROL_RECORDS+20+4*i,rd_u32(first+20+4*i));
        wr_u16(SCENE_DISPATCH_GATE,0xffff);
    } else {
        /* C0A3EA: ready player, gear/status flags and zero final speed. */
        wr_u8(PLAYER_PHASE,1);wr_u8(CONTROL_RECORDS+0x21,rd_u8(CONTROL_RECORDS+0x21)|1);
        wr_u8(CONTROL_RECORDS+3,rd_u8(CONTROL_RECORDS+3)|0x80);
        wr_u8(CONTROL_RECORDS+4,rd_u8(CONTROL_RECORDS+4)|4);
        wr_u8(0xc45848u,0);wr_u16(CONTROL_RECORDS+110,0);
    }
}
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,
                    uint16_t saved_tick,void *context) {
    ScheduleFixture *fixture=context;
    fixture->clock.iteration=game->update_iterations;
    if(boundary==NATIVE_FRAME_INPUT_BEGIN && fixture->seeded) {
        const unsigned stage=rd_u32(STAGE_CALLBACK);
        const unsigned key=stage|((rd_s16(POST_INPUT_COUNTDOWN)<=0)?0x1000000u:0);
        unsigned found=0;
        while(found<fixture->entry_count && fixture->entry_stages[found]!=key) ++found;
        if(found==fixture->entry_count || game->input_count) {
            if(found==fixture->entry_count) {
                if(found==32) abort();fixture->entry_stages[fixture->entry_count++]=key;
            }
            snprintf(fixture->entry_path,sizeof fixture->entry_path,"%s.entry.%u",fixture->prefix,fixture->entry_exports);
            fixture->entry=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->entry_path,
                .iteration=game->update_iterations,.count=1};
            native_frame_capture(game,NATIVE_FRAME_BODY_BEGIN,saved_tick,&fixture->entry);
            printf("{\"entry\":%u,\"stage\":\"%06X\",\"tick\":%u,\"keys\":[",fixture->entry_exports++,stage,game->ticks);
            for(unsigned i=0;i<game->input_count;++i)
                printf("%s%u",i?",":"",game->input_keys[(game->input_read+i)%256]);
            puts("]}");
        }
    }
    if(boundary==NATIVE_FRAME_BODY_BEGIN && fixture->entry.begun)
        native_frame_capture(game,NATIVE_FRAME_BODY_END,saved_tick,&fixture->entry);
    if(boundary==NATIVE_FRAME_BODY_BEGIN && rd_u8(MODE_SELECT)==fixture->mode && game->ticks>=10000) {
        const unsigned stage=rd_u32(STAGE_CALLBACK);
        if(!fixture->seeded && stage==0xc10dae) {
            terminal_conditions(fixture);fixture->seeded=1;
        }
        if(fixture->seeded) {
            unsigned found=0;
            while(found<fixture->stage_count && fixture->stages[found]!=stage) ++found;
            if(found==fixture->stage_count || fixture->captures<8) {
                if(found==fixture->stage_count) {
                    if(found==32) abort();fixture->stages[fixture->stage_count++]=stage;
                }
                snprintf(fixture->path,sizeof fixture->path,"%s.%u",fixture->prefix,fixture->captures);
                fixture->capture=(NativeFrameCapture){.replay=&fixture->clock,.prefix=fixture->path,
                    .iteration=game->update_iterations,.count=1};
                printf("{\"capture\":%u,\"stage\":\"%06X\",",fixture->captures,stage);
            }
        }
    }
    if(!fixture->capture.prefix || fixture->capture.complete) return;
    native_frame_capture(game,boundary,saved_tick,&fixture->capture);
    if(fixture->capture.complete) {
        printf("\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"owner_exit\":%s}\n",
            fixture->capture.before_tick,fixture->capture.after_tick,fixture->capture.saved_tick,
            fixture->capture.owner_exit?"true":"false");
        ++fixture->captures;
        if(!fixture->phase && rd_u8(SEQUENCE_PHASE)==3) {
            fixture->phase=rd_u8(PLAYER_PHASE);fixture->event=rd_u16(PHASE_WORD);
        }
        if(!strcmp(fixture->kind,"five") && rd_s16(SCENE_DISPATCH_GATE)<0) {
            const gaddr first=CONTROL_RECORDS+4*512,second=CONTROL_RECORDS+6*512;
            /* C0A12E reloads each five-word view entry; compare its source table. */
            for(unsigned i=0;i<2;++i) {
                const gaddr record=i?second:first;
                gaddr table=VIEW_PARAMETER_TABLE+(gaddr)(int32_t)rd_s16(VIEW_PARAMETER_TABLE+2*rd_u8(record+58));
                int equal=1;
                for(unsigned k=0;k<4;++k) equal&=rd_u16(record+44+2*k)==rd_u16(table+2*k);
                equal&=rd_u32(record+52)==(uint32_t)(int32_t)rd_s16(table+8);
                if(equal) fixture->restored|=1u<<i;
            }
        }
    }
}
int main(int argc,char **argv) {
    if(argc!=5) return 1;
    ScheduleFixture fixture={.prefix=argv[3],.kind=argv[4],.mode=!strcmp(argv[4],"five")?5:4};
    if(strcmp(fixture.kind,"four") && strcmp(fixture.kind,"five") && strcmp(fixture.kind,"ready")) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);char error[256];int result=1;
    if(!game) return 1;
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    game->observe_frame=observe;game->frame_context=&fixture;
    const unsigned times[]={1800,3000,4500,6500,8000};
    const int keys[]={32,54,282+(int)fixture.mode-3,13,13};
    while(game->ticks<16000) {
        for(unsigned i=0;i<5;++i) {
            if(game->ticks==times[i]) native_frontend_event(game,keys[i],1);
            if(game->ticks==times[i]+2) native_frontend_event(game,keys[i],0);
        }
        native_frontend_tick(game);
    }
    if(!fixture.seeded || fixture.captures<8 || (!strcmp(fixture.kind,"five")?
       fixture.restored!=3:fixture.phase!=(!strcmp(fixture.kind,"four")?0xffu:0xfcu)) ||
       (!strcmp(fixture.kind,"four") && fixture.event!=4)) {
        fprintf(stderr,"Postflight %s failed: seeded=%u captures=%u phase=%X event=%X restored=%u\n",
            fixture.kind,fixture.seeded,fixture.captures,fixture.phase,fixture.event,fixture.restored);goto done;
    }
    result=0;
    if(!strcmp(fixture.kind,"ready")) {
        uint8_t saved[78];FILE *file=fopen(game->config_path,"rb");
        if(!file) {fprintf(stderr,"Postflight log was not saved\n");result=1;}
        else {
            int valid=fread(saved,1,78,file)==78 && fgetc(file)==EOF;
            if(fclose(file)) valid=0;
            if(!valid || memcmp(saved,native_storage_range(rd_u32(MODE_TABLE),78),78)) {
                fprintf(stderr,"Postflight saved log differs from the result table\n");result=1;
            }
        }
    }
done:
    if(game) {native_frontend_close(game);free(game);}return result;
}
