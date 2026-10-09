/* Validation-only continuous tour. One frontend lifetime, ordinary keys only.
 * C110A4 earns qualification; C1643A saves results; C0F920 returns to menu.
 * MissionPilot reads flight state to choose keys and is never in fa18_native. */
#include "native/frontend.h"
#include "native/menu.h"
#include "globals.h"
#include "mission_pilot.h"
#include "../../port/native/replay.h"
#include "../../port/amiga/host_keys.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { NativeReplay replay; FILE *keys; unsigned previous_stage; int code_pressed; } QualificationInput;
typedef struct { int airborne, landed, wire, gear_raised, gear_lowered; } FlightEvidence;

static int failed(NativeFrontend *game,const char *operation) {
    fprintf(stderr,"Campaign %s failed at tick %u, screen %s, stage %06X, mode %u, phase %u, completions %u, resets %u\n",
        operation,game->ticks,native_frontend_screen(game),rd_u32(STAGE_CALLBACK),
        rd_u8(MODE_SELECT),rd_u8(PLAYER_PHASE),rd_u16(rd_u32(MODE_TABLE)+56),game->postflight_resets);
    fprintf(stderr,"Player fuel %u, speed %d, damage %u; scene difficulty %u, dispatch gate %d, aircraft expiries %u\n",
        rd_u32(CONTROL_RECORDS+114),rd_s16(CONTROL_RECORDS+110),rd_u8(CONTROL_RECORDS+60),
        rd_u8(SCENE_DISPATCH_LIMIT),rd_s16(SCENE_DISPATCH_GATE),rd_u8(SCENE_DISPATCH_AUX));
    return 0;
}
static void tick(NativeFrontend *game) {
    int16_t samples[960*2];
    native_frontend_tick(game);
    native_audio_render(&game->audio,samples,960,48000);
}
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,uint16_t saved,void *context) {
    FlightEvidence *flight=context;
    (void)game;(void)saved;
    if(boundary!=NATIVE_FRAME_BODY_END) return;
    unsigned contact=rd_u16(CONTROL_RECORDS+2);
    if(rd_u32(STAGE_CALLBACK)==0xc10dae && !(contact&0x80)) flight->airborne=1;
    if(flight->airborne && !(contact&0x80) && (rd_u8(COMMAND_BLOCK_FLAGS)&0x80)) flight->gear_raised=1;
    if(flight->gear_raised && !(contact&0x80) && !(rd_u8(COMMAND_BLOCK_FLAGS)&0x80)) flight->gear_lowered=1;
    if(flight->airborne && (contact&0x80)) flight->landed=1;
    if(flight->landed && (contact&0x4000)) flight->wire=1;
}
static void qualification_update(NativeFrontend *game,void *context) {
    QualificationInput *input=context;
    NativeReplay *replay=&input->replay;
    if(!replay->started) {
        if(game->screen!=NATIVE_MENU) return;
        replay->started=1;
    }
    ++replay->iteration;
    /* The supplied disk has an empty expected code. A live enlistment clears
     * CONTEXT_REQUEST, so its original mode-three prompt needs Return. */
    if((replay->iteration==2600 && rd_u8(0xc457e0)==3) || (replay->iteration==2602 && input->code_pressed)) {
        int down=replay->iteration==2600;
        if(down && (rd_u8(0xc457e0)!=3 || rd_u8(EXPECTED_CODE))) {
            failed(game,"empty original code prompt");abort();
        }
        fprintf(input->keys,"F %u K 13 0 0 %d\n",game->ticks-1,down);
        native_frontend_event(game,13,down);
        input->code_pressed=down;
    }
    if(getenv("FA18_MISSION_TRACE") && rd_u32(STAGE_CALLBACK)!=input->previous_stage) {
        input->previous_stage=rd_u32(STAGE_CALLBACK);
        fprintf(stderr,"Qualification iteration %u tick %u stage %06X phase %u\n",replay->iteration,game->ticks,input->previous_stage,rd_u8(PLAYER_PHASE));
    }
    while(replay->next<replay->count && replay->events[replay->next].iteration==replay->iteration) {
        NativeReplayEvent *event=&replay->events[replay->next++];
        int host=0;
        for(int key=1;key<=315;++key) if(amiga_host_legacy_raw_key(key)==event->key) {host=key;break;}
        if(!host) {fprintf(stderr,"Qualification raw key %u has no host identity\n",event->key);abort();}
        /* begin_update runs after the host tick increment. Record the event
         * at the preceding host boundary for the playable runner replay. */
        fprintf(input->keys,"F %u K %d 0 0 %u\n",game->ticks-1,host,event->down);
        native_frontend_raw_event(game,event->key,event->down);
    }
}
static int saved_log(NativeFrontend *game,unsigned mode) {
    uint8_t saved[78];FILE *file=fopen(game->config_path,"rb");
    if(!file) return failed(game,"read earned save");
    int ok=fread(saved,1,sizeof saved,file)==sizeof saved && fgetc(file)==EOF;
    if(fclose(file)) ok=0;
    for(unsigned i=0;ok && i<sizeof saved;++i) {
        /* C115BA advances the existing pilot's tour word on entry; its next
         * earned result persists it. Initial checkpoint retains the actual
         * enrolled file, while all result checkpoints require full equality. */
        if(!mode && (i==4 || i==5)) continue;
        ok=saved[i]==rd_u8(rd_u32(MODE_TABLE)+i);
    }
    if(ok && !mode) {
        unsigned tours=saved[4]*256u+saved[5],current=rd_u16(rd_u32(MODE_TABLE)+4);
        ok=current==tours || current==(uint16_t)(tours+1);
    }
    if(!ok) return failed(game,"persisted log equality");
    printf("{\"checkpoint\":true,\"mode\":%u,\"tick\":%u,\"stage\":\"%06X\",\"saved_config_hex\":\"",
        mode,game->ticks,rd_u32(STAGE_CALLBACK));
    for(unsigned i=0;i<sizeof saved;++i) printf("%02x",saved[i]);
    puts("\"}");fflush(stdout);
    return 1;
}
static int menu_return(NativeFrontend *game,MissionPilot *pilot) {
    unsigned start=game->ticks,escape=0;
    while(game->ticks<start+5000 && !game->postflight_resets) {
        if(!escape && rd_u8(PLAYER_PHASE)==4 && rd_s8(MESSAGE_STATE_C)<0) {
            escape=game->ticks;mission_pilot_event(pilot,game,304,1);mission_pilot_event(pilot,game,27,1);
        }
        if(escape && !game->input_count && game->shift_keys) {
            mission_pilot_event(pilot,game,27,0);mission_pilot_event(pilot,game,304,0);
        }
        tick(game);
        if(escape && game->ticks>escape+2 && game->screen==NATIVE_MENU && rd_u32(STAGE_CALLBACK)==0xc0fcb4) {
            if(rd_u8(MODE_SELECT) || game->input_count || rd_u8(PLAYER_PHASE) || rd_u8(RECORDER_MODE)) return failed(game,"menu reset");
            return 1;
        }
    }
    return failed(game,"result messages and menu return");
}
static int mission(NativeFrontend *game,FILE *keys,unsigned mode) {
    MissionPilot pilot={0};FlightEvidence flight={0};
    pilot.mode=mode;pilot.keys=keys;pilot.scene=game->scene_frames;
    pilot.complete_flight=mode!=3;pilot.escort_flight=mode==4;
    pilot.force_return=mode==5;pilot.rescue_flight=mode==6;
    pilot.cruise_flight=mode==7;pilot.new_final_flight=pilot.final_flight=pilot.final_sequence=mode==8;
    pilot.campaign_flight=mode!=3;
    pilot.manage_gear=1;
    game->frame_context=&flight;
    unsigned start=game->ticks,before=rd_u16(rd_u32(MODE_TABLE)+56);
    const unsigned times[]={100,1600,3600,5100,11600};
    const int commands[]={54,282+(int)mode-3,13,13,50};
    unsigned previous_stage=0;
    while(game->ticks<start+65000 && !game->postflight_resets) {
        if(getenv("FA18_MISSION_TRACE") && previous_stage!=rd_u32(STAGE_CALLBACK)) {
            previous_stage=rd_u32(STAGE_CALLBACK);
            fprintf(stderr,"Mission %u tick %u stage %06X player phase %u\n",mode,game->ticks,previous_stage,rd_u8(PLAYER_PHASE));
        }
        for(unsigned i=0;i<(mode==3?5u:4u);++i) {
            if(game->ticks==start+times[i]) mission_pilot_event(&pilot,game,commands[i],1);
            if(game->ticks==start+times[i]+2) mission_pilot_event(&pilot,game,commands[i],0);
        }
        if(pilot.target_press && game->ticks==pilot.target_press+2) mission_pilot_event(&pilot,game,116,0);
        if(pilot.gear_key_tick && game->ticks==pilot.gear_key_tick+2) mission_pilot_event(&pilot,game,103,0);
        if(pilot.weapon_press && game->ticks==pilot.weapon_press+2) mission_pilot_event(&pilot,game,13,0);
        if(pilot.defense_tick && game->ticks==pilot.defense_tick+2) mission_pilot_event(&pilot,game,pilot.defense_key,0);
        if(pilot.rescue_drop_tick && game->ticks==pilot.rescue_drop_tick+6) {
            mission_pilot_event(&pilot,game,304,1);mission_pilot_event(&pilot,game,102,1);
        }
        if(pilot.rescue_drop_tick && game->ticks==pilot.rescue_drop_tick+12) mission_pilot_event(&pilot,game,102,0);
        if(pilot.rescue_drop_tick && game->ticks==pilot.rescue_drop_tick+14) mission_pilot_event(&pilot,game,304,0);
        if((mode==4 || mode==5 || mode==7 || mode==8) && pilot.started) for(unsigned i=0;i<2;++i) {
            if(game->ticks==pilot.started+700+20*i) mission_pilot_event(&pilot,game,13,1);
            if(game->ticks==pilot.started+702+20*i) mission_pilot_event(&pilot,game,13,0);
        }
        mission_pilot_tick(&pilot,game);tick(game);
        if(pilot.started && rd_u16(rd_u32(MODE_TABLE)+56)!=before) break;
        if(pilot.started && !rd_u8(MODE_SELECT)) return failed(game,"unexpected menu before mission result");
        if(pilot.started && (rd_u8(PLAYER_PHASE)==0xfe || rd_u8(PLAYER_PHASE)==2))
            return failed(game,"original mission failure outcome");
    }
    gaddr log=rd_u32(MODE_TABLE);
    if(game->postflight_resets || !pilot.started || !pilot.objective || !flight.airborne || !flight.landed || !flight.gear_raised || !flight.gear_lowered ||
       (rd_u8(COMMAND_BLOCK_FLAGS)&0x80) || (mode!=3 && !flight.wire) ||
       rd_u8(MODE_SELECT)!=mode || rd_u8(PLAYER_PHASE)!=0xfc || rd_u16(CONTROL_RECORDS+110) ||
       rd_u16(log+56)!=before+1 || rd_u8(log+18+mode)!=1 || rd_u8(log+6)!=mode)
        return failed(game,"objective/wire/stop/result");
    const int held[]={pilot.rudder,pilot.pitch,pilot.roll,pilot.throttle,pilot.fire,pilot.hook};
    for(unsigned i=0;i<sizeof held/sizeof held[0];++i) if(held[i]) mission_pilot_event(&pilot,game,held[i],0);
    if(!menu_return(game,&pilot) || !saved_log(game,mode)) return 0;
    printf("{\"mission\":%u,\"airborne\":true,\"objective\":true,\"landed\":true,\"gear_raised_in_flight\":true,\"gear_lowered_for_landing\":true,\"wire\":%s,\"menu\":true,\"completions\":%u}\n",mode,flight.wire?"true":"false",before+1);
    return 1;
}
int main(int argc,char **argv) {
    if(argc!=6 && (argc!=7 || strcmp(argv[6],"saved-pilot"))) {fputs("Usage: campaign_session_test ADF save qualification-input enlist-input output-keys [saved-pilot]\n",stderr);return 1;}
    NativeFrontend *game=calloc(1,sizeof *game);QualificationInput input={0};
    MissionPilot keys={0};FlightEvidence qualification={0};char error[256];int result=1,opened=0;
    keys.keys=fopen(argv[5],"w");input.keys=keys.keys;
    if(!game || !keys.keys) goto done;
    fputs("E9K_INPUT_V1\n",keys.keys);
    if(!native_replay_load(&input.replay,argv[3],error,sizeof error) ||
       !native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    opened=1;
    if(argc==7) {
        while(game->ticks<1800) tick(game);
        mission_pilot_event(&keys,game,32,1);tick(game);tick(game);
        mission_pilot_event(&keys,game,32,0);
    } else {
    FILE *enlist=fopen(argv[4],"r");char line[128];
    if(!enlist) goto done;
    if(!fgets(line,sizeof line,enlist) || strcmp(line,"E9K_INPUT_V1\n")) {fclose(enlist);goto done;}
    unsigned when;int code,down;
    while(fgets(line,sizeof line,enlist)) {
        if(sscanf(line,"F %u K %d 0 0 %d",&when,&code,&down)!=3 || when<game->ticks) {fclose(enlist);goto done;}
        while(game->ticks<when) tick(game);
        mission_pilot_event(&keys,game,code,down);
    }
    if(ferror(enlist)) {fclose(enlist);goto done;}
    if(fclose(enlist)) goto done;
    }
    while(game->ticks<9000) tick(game);
    if(game->screen!=NATIVE_MENU || rd_u16(rd_u32(MODE_TABLE)) || rd_u16(rd_u32(MODE_TABLE)+56) ||
       !saved_log(game,0)) {failed(game,"normal enlistment");goto done;}
    game->observe_frame=observe;game->frame_context=&qualification;
    game->begin_update=qualification_update;game->update_context=&input;
    unsigned qualification_start=game->ticks;
    while(game->ticks<qualification_start+45000 && input.replay.iteration<input.replay.end) tick(game);
    game->begin_update=NULL;game->update_context=NULL;
    if(input.replay.iteration!=input.replay.end || input.replay.next!=input.replay.count ||
       !qualification.airborne || !qualification.landed || !rd_u16(rd_u32(MODE_TABLE)) ||
       game->postflight_resets || !saved_log(game,9)) {failed(game,"qualification");goto done;}
    /* The source qualification result restarts flight. Shift+Escape requests the
     * ordinary scene/menu return before selecting the first earned mission. */
    mission_pilot_event(&keys,game,304,1);
    mission_pilot_event(&keys,game,27,1);
    do {tick(game);} while(game->input_count);
    mission_pilot_event(&keys,game,27,0);
    mission_pilot_event(&keys,game,304,0);
    unsigned escape=game->ticks;
    while(game->ticks<escape+5000 && !(game->screen==NATIVE_MENU && rd_u32(STAGE_CALLBACK)==0xc0fcb4)) tick(game);
    if(game->screen!=NATIVE_MENU || rd_u8(MODE_SELECT) || game->input_count || game->postflight_resets) {failed(game,"qualification menu return");goto done;}
    for(unsigned mode=3;mode<=8;++mode) if(!mission(game,keys.keys,mode)) goto done;
    printf("{\"continuous_campaign\":true,\"frontend_opens\":1,\"ticks\":%u,\"scene_frames\":%u,\"completions\":6,\"crash_resets\":0,\"menu\":true}\n",game->ticks,game->scene_frames);
    result=0;
done:
    if(keys.keys && fclose(keys.keys)) result=1;
    if(opened) native_frontend_close(game);
    native_replay_close(&input.replay);free(game);return result;
}
