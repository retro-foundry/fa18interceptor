/* Validation only: ordinarily replay qualification/menu keys, then choose
 * patrol or escort keys on the same controller clock as the original recorder.
 * No flight, eligibility, grade, clock or position fields are written. */
#include "native/frontend.h"
#include "globals.h"
#include "mission_pilot.h"
#include "../../port/native/replay.h"
#include "../../port/native/flight_trace.h"
#include <stdlib.h>
#include <string.h>

static NativeFrontend *live;
static FILE *physical;
static void pilot_event(NativeFrontend *observation,int key,int down) {
    (void)observation;
    native_frontend_event(live,key,down);
    fprintf(physical,"F %u K %d 0 0 %d\n",live->ticks-1,key,down);
}
#define native_frontend_event pilot_event
#include "mission_pilot.c"
#undef native_frontend_event

typedef struct {
    NativeReplay replay;
    MissionPilot pilot;
    NativeFrontend observation;
    FA18FlightTrace trace;
    unsigned target_release,gear_release,weapon_release,defense_release;
    unsigned weapon_initial_pressed,weapon_initial_released;
    unsigned prefix_native_end,earned_tick,escape_tick;
    int airborne,gear_up,gear_down,landed,wire,earned,level_final;
} Input;

static const uint8_t *trace_bytes(void *context,uint32_t address,size_t size) {
    NativeStorage *storage=context;
    if(address<0x80000u && size<=0x80000u-address) return storage->buffers+address;
    if(address>=0xc00000u && address<0xc80000u && size<=0xc80000u-address)
        return storage->source+address-0xc00000u;
    return NULL;
}
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,uint16_t saved,void *context) {
    Input *input=context;(void)saved;
    if(input->trace.file && boundary==NATIVE_FRAME_INPUT_BEGIN && input->replay.iteration)
        fa18_flight_trace_write(&input->trace,input->replay.iteration,game->ticks,
            320,200,trace_bytes,&game->storage);
    if(boundary==NATIVE_FRAME_BODY_END && input->pilot.started &&
       input->landed && (rd_u16(CONTROL_RECORDS+2)&0x4000)) input->wire=1;
}
static void release(unsigned stamp,unsigned *last,Input *input,int key) {
    if(stamp && stamp!=*last && input->observation.ticks>=stamp+2) {
        mission_pilot_event(&input->pilot,&input->observation,key,0);*last=stamp;
    }
}
static void update(NativeFrontend *game,void *context) {
    Input *input=context;
    if(input->replay.anchor_count?!input->replay.anchor_complete:input->replay.iteration<input->replay.end) {
        native_replay_update(game,&input->replay);return;
    }
    if(!input->prefix_native_end) input->prefix_native_end=input->replay.iteration;
    ++input->replay.iteration;
    if(input->earned) {
        /* Same ordinary result-menu controls as the continuous-tour test.
         * Waiting on the live message owner does not rewrite its timers. */
        if(!input->escape_tick && rd_u8(PLAYER_PHASE)==4 && rd_s8(MESSAGE_STATE_C)<0) {
            input->escape_tick=game->ticks;pilot_event(NULL,304,1);pilot_event(NULL,27,1);
        }
        if(input->escape_tick && !game->input_count && game->shift_keys) {
            pilot_event(NULL,27,0);pilot_event(NULL,304,0);
        }
        return;
    }
    input->observation.ticks=40000u+4u*rd_u16(UPDATE_TICK);
    input->observation.scene_frames=rd_u16(UPDATE_TICK);
    release(input->pilot.target_press,&input->target_release,input,116);
    release(input->pilot.gear_key_tick,&input->gear_release,input,103);
    if(input->pilot.mode==4) {
        release(input->pilot.weapon_press,&input->weapon_release,input,13);
        release(input->pilot.defense_tick,&input->defense_release,input,input->pilot.defense_key);
        if(input->pilot.started) for(unsigned i=0;i<2;++i) {
            const unsigned bit=1u<<i,when=input->pilot.started+700+20*i;
            if(input->observation.ticks>=when && !(input->weapon_initial_pressed&bit)) {
                mission_pilot_event(&input->pilot,&input->observation,13,1);input->weapon_initial_pressed|=bit;
            }
            if(input->observation.ticks>=when+2 && (input->weapon_initial_pressed&bit) && !(input->weapon_initial_released&bit)) {
                mission_pilot_event(&input->pilot,&input->observation,13,0);input->weapon_initial_released|=bit;
            }
        }
    }
    const int previous[]={input->pilot.rudder,input->pilot.pitch,input->pilot.roll};
    /* Optional validation input: keep the wire's existing lateral approach,
     * but aim at its observed aircraft touchdown height. The test pilot's
     * final point subtracts 200 and adds before_home*.04; undo those terms
     * in its temporary goal, then restore that goal after selecting keys.
     * This never writes game coordinates, geometry, contact or outcome RAM. */
    const double home_height=input->pilot.home[1];
    const int level_height=input->level_final && input->pilot.objective && input->pilot.phase>=2;
    if(level_height) {
        const double before_home=(input->pilot.home[0]-rd_s32(CONTROL_RECORDS+20)/256.0)*input->pilot.forward[0]+
            (input->pilot.home[2]-rd_s32(CONTROL_RECORDS+28)/256.0)*input->pilot.forward[2];
        input->pilot.home[1]+=200-fmax(before_home,0)*0.04;
    }
    mission_pilot_tick(&input->pilot,&input->observation);
    if(level_height) input->pilot.home[1]=home_height;
    if(input->pilot.mode==4 && rd_u32(STAGE_CALLBACK)==0xc10dae) {
        const int steering[]={input->pilot.rudder,input->pilot.pitch,input->pilot.roll};
        for(unsigned i=0;i<3;++i) if(steering[i] && steering[i]==previous[i])
            mission_pilot_event(&input->pilot,&input->observation,steering[i],1);
    }
    if(input->pilot.started && !(rd_u16(CONTROL_RECORDS+2)&0x80)) {
        input->airborne=1;
        if(rd_u8(COMMAND_BLOCK_FLAGS)&0x80) input->gear_up=1;
        else if(input->gear_up) input->gear_down=1;
    }
    if(input->airborne && (rd_u16(CONTROL_RECORDS+2)&0x80)) input->landed=1;
}
int main(int argc,char **argv) {
    if(argc<5) return 2;
    const char *anchors=NULL,*trace_path=NULL,*data_path=NULL;int level_final=0;
    for(int i=5;i<argc;++i) {
        if(i+1<argc && !strcmp(argv[i],"--escort-anchors")) anchors=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--flight-trace")) trace_path=argv[++i];
        else if(i+1<argc && !strcmp(argv[i],"--data-out")) data_path=argv[++i];
        else if(!strcmp(argv[i],"--level-final")) level_final=1;
        else return 2;
    }
    if(level_final && !anchors) return 2;
    live=calloc(1,sizeof *live);
    Input *input=calloc(1,sizeof *input);
    char error[256];int result=1;
    if(!live || !input) goto done;
    physical=fopen(argv[4],"w");input->pilot.keys=tmpfile();
    if(!physical || !input->pilot.keys) goto done;
    fputs("E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n",physical);
    if(!native_frontend_open(live,argv[1],argv[2],error,sizeof error) ||
       !native_replay_load(&input->replay,argv[3],error,sizeof error) ||
       (anchors && !native_replay_load_anchors(&input->replay,anchors,error,sizeof error))) {
        fprintf(stderr,"%s\n",error);goto done;
    }
    if(trace_path && !fa18_flight_trace_open(&input->trace,trace_path,512u*1024*1024)) goto done;
    input->pilot.mode=anchors?4:3;input->pilot.manage_gear=1;
    input->level_final=level_final;
    input->pilot.complete_flight=input->pilot.campaign_flight=1;
    input->pilot.patrol_flight=!anchors;input->pilot.escort_flight=anchors!=NULL;
    input->pilot.wait_for_approach_height=input->pilot.approach_at_standoff_height=input->pilot.carrier_wire_return=anchors!=NULL;
    live->begin_update=update;live->update_context=input;
    live->observe_frame=observe;live->frame_context=input;
    while(live->ticks<100000 && !live->postflight_resets && !input->replay.anchor_failed && !input->trace.failed) {
        if(live->ticks==1800) native_frontend_event(live,32,1);
        if(live->ticks==1802) native_frontend_event(live,32,0);
        native_frontend_tick(live);
        int16_t samples[960*2];
        native_audio_render(&live->audio,samples,960,48000);
        if(input->pilot.started && !input->earned && rd_u16(rd_u32(MODE_TABLE)+56)>input->pilot.completions) {
            if(!anchors) break;
            input->earned=1;input->earned_tick=live->ticks;
            const int held[]={input->pilot.rudder,input->pilot.pitch,input->pilot.roll,input->pilot.throttle,input->pilot.fire,input->pilot.hook};
            for(unsigned i=0;i<sizeof held/sizeof held[0];++i) if(held[i]) {
                /* Outside begin_update: this release belongs to the current
                 * host boundary, not the preceding recorded boundary. */
                native_frontend_event(live,held[i],0);
                fprintf(physical,"F %u K %d 0 0 0\n",live->ticks,held[i]);
            }
        }
        if(input->earned && input->escape_tick && live->ticks>input->escape_tick+2 &&
           live->screen==NATIVE_MENU && rd_u32(STAGE_CALLBACK)==0xc0fcb4) break;
        if(!input->earned && input->landed && !rd_u16(CONTROL_RECORDS+110) && rd_u8(PLAYER_PHASE)!=0xfc) break;
        if(input->pilot.started && (rd_u8(PLAYER_PHASE)==0xfe || rd_u8(PLAYER_PHASE)==2)) break;
    }
    const unsigned completions=rd_u16(rd_u32(MODE_TABLE)+56);
    printf("{\"patrol_input_result\":true,\"tick\":%u,\"iteration\":%u,\"game_tick\":%u,"
           "\"phase\":%u,\"completions\":%u,\"grade\":%u,\"objective\":%u,"
           "\"airborne\":%d,\"gear_up\":%d,\"gear_down\":%d,\"landed\":%d,"
           "\"region\":%u,\"speed\":%u,\"resets\":%u,\"xyz\":[%d,%d,%d],"
           "\"mode\":%u,\"wire\":%d,\"menu\":%d,\"earned_tick\":%u,\"prefix_native_end\":%u,"
           "\"prefix_source_end\":%u,\"prefix_keys\":%zu,\"anchor_failed\":%d}\n",
        live->ticks,input->replay.iteration,rd_u16(UPDATE_TICK),rd_u8(PLAYER_PHASE),completions,
        rd_u8(rd_u32(MODE_TABLE)+18+input->pilot.mode),input->pilot.objective,input->airborne,input->gear_up,input->gear_down,
        input->landed,rd_u8(CONTROL_RECORDS+4),rd_u16(CONTROL_RECORDS+110),live->postflight_resets,
        rd_s32(CONTROL_RECORDS+20)/256,rd_s32(CONTROL_RECORDS+24)/256,rd_s32(CONTROL_RECORDS+28)/256,
        input->pilot.mode,input->wire,live->screen==NATIVE_MENU && rd_u32(STAGE_CALLBACK)==0xc0fcb4,
        input->earned_tick,input->prefix_native_end,input->replay.end,input->replay.next,input->replay.anchor_failed);
    result=!(completions==input->pilot.completions+1 && rd_u8(rd_u32(MODE_TABLE)+18+input->pilot.mode)==1 && input->airborne &&
        input->pilot.objective && input->landed && input->gear_up && input->gear_down && !live->postflight_resets &&
        !input->replay.anchor_failed && (!anchors || (input->wire && live->screen==NATIVE_MENU &&
        rd_u32(STAGE_CALLBACK)==0xc0fcb4 && !live->input_count && !rd_u8(MODE_SELECT) && !rd_u8(PLAYER_PHASE))));
    if(data_path) {
        FILE *file=fopen(data_path,"wb");
        if(!file) result=1;
        else {
            int ok=fwrite(live->storage.buffers,1,0x80000,file)==0x80000 &&
                fwrite(live->storage.source,1,0x80000,file)==0x80000;
            if(fclose(file) || !ok) result=1;
        }
    }
done:
    if(physical && fclose(physical)) result=1;
    if(input) {
        if(!fa18_flight_trace_close(&input->trace)) result=1;
        if(input->pilot.keys) fclose(input->pilot.keys);
        native_replay_close(&input->replay);
    }
    if(live) native_frontend_close(live);
    free(input);free(live);return result;
}
