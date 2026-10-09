/* Validation only: ordinarily replay qualification/menu keys, then choose
 * patrol/landing keys on the same controller clock as the original recorder.
 * No flight, eligibility, grade, clock or position fields are written. */
#include "native/frontend.h"
#include "globals.h"
#include "mission_pilot.h"
#include "../../port/native/replay.h"
#include <stdlib.h>

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
    unsigned target_release,gear_release;
    int airborne,gear_up,gear_down,landed;
} Input;

static void release(unsigned stamp,unsigned *last,Input *input,int key) {
    if(stamp && stamp!=*last && input->observation.ticks>=stamp+2) {
        mission_pilot_event(&input->pilot,&input->observation,key,0);*last=stamp;
    }
}
static void update(NativeFrontend *game,void *context) {
    Input *input=context;
    if(input->replay.iteration<input->replay.end) {
        native_replay_update(game,&input->replay);return;
    }
    ++input->replay.iteration;
    input->observation.ticks=40000u+4u*rd_u16(UPDATE_TICK);
    input->observation.scene_frames=rd_u16(UPDATE_TICK);
    release(input->pilot.target_press,&input->target_release,input,116);
    release(input->pilot.gear_key_tick,&input->gear_release,input,103);
    mission_pilot_tick(&input->pilot,&input->observation);
    if(input->pilot.started && !(rd_u16(CONTROL_RECORDS+2)&0x80)) {
        input->airborne=1;
        if(rd_u8(COMMAND_BLOCK_FLAGS)&0x80) input->gear_up=1;
        else if(input->gear_up) input->gear_down=1;
    }
    if(input->airborne && (rd_u16(CONTROL_RECORDS+2)&0x80)) input->landed=1;
}
int main(int argc,char **argv) {
    if(argc!=5) return 2;
    live=calloc(1,sizeof *live);
    Input *input=calloc(1,sizeof *input);
    char error[256];int result=1;
    if(!live || !input) goto done;
    physical=fopen(argv[4],"w");input->pilot.keys=tmpfile();
    if(!physical || !input->pilot.keys) goto done;
    fputs("E9K_INPUT_V1\nF 1800 K 32 0 0 1\nF 1802 K 32 0 0 0\n",physical);
    if(!native_frontend_open(live,argv[1],argv[2],error,sizeof error) ||
       !native_replay_load(&input->replay,argv[3],error,sizeof error)) {
        fprintf(stderr,"%s\n",error);goto done;
    }
    input->pilot.mode=3;input->pilot.manage_gear=1;
    input->pilot.patrol_flight=input->pilot.complete_flight=input->pilot.campaign_flight=1;
    live->begin_update=update;live->update_context=input;
    while(live->ticks<100000 && !live->postflight_resets) {
        if(live->ticks==1800) native_frontend_event(live,32,1);
        if(live->ticks==1802) native_frontend_event(live,32,0);
        native_frontend_tick(live);
        int16_t samples[960*2];
        native_audio_render(&live->audio,samples,960,48000);
        if(input->pilot.started && rd_u16(rd_u32(MODE_TABLE)+56)>input->pilot.completions) break;
        if(input->landed && !rd_u16(CONTROL_RECORDS+110) && rd_u8(PLAYER_PHASE)!=0xfc) break;
    }
    const unsigned completions=rd_u16(rd_u32(MODE_TABLE)+56);
    printf("{\"patrol_input_result\":true,\"tick\":%u,\"iteration\":%u,\"game_tick\":%u,"
           "\"phase\":%u,\"completions\":%u,\"grade\":%u,\"objective\":%u,"
           "\"airborne\":%d,\"gear_up\":%d,\"gear_down\":%d,\"landed\":%d,"
           "\"region\":%u,\"speed\":%u,\"resets\":%u,\"xyz\":[%d,%d,%d]}\n",
        live->ticks,input->replay.iteration,rd_u16(UPDATE_TICK),rd_u8(PLAYER_PHASE),completions,
        rd_u8(rd_u32(MODE_TABLE)+21),input->pilot.objective,input->airborne,input->gear_up,input->gear_down,
        input->landed,rd_u8(CONTROL_RECORDS+4),rd_u16(CONTROL_RECORDS+110),live->postflight_resets,
        rd_s32(CONTROL_RECORDS+20)/256,rd_s32(CONTROL_RECORDS+24)/256,rd_s32(CONTROL_RECORDS+28)/256);
    result=!(completions==1 && rd_u8(rd_u32(MODE_TABLE)+21)==1 && input->airborne &&
        input->pilot.objective && input->landed && input->gear_up && input->gear_down && !live->postflight_resets);
done:
    if(physical && fclose(physical)) result=1;
    if(input) {
        if(input->pilot.keys) fclose(input->pilot.keys);
        native_replay_close(&input->replay);
    }
    if(live) native_frontend_close(live);
    free(input);free(live);return result;
}
