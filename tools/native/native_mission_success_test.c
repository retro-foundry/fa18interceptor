/* Validation pilot for normally reached mission objectives and landing.
 * Only keyboard events are delivered; all flight and outcome RAM is read-only. */
#include "native/frontend.h"
#include "globals.h"
#include "mission_pilot.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *prefix;
    uint8_t *before, *entry_before;
    unsigned body, captures, entries, first_tick, entry_tick, iteration, window, combat_window, formation_window;
    unsigned entry_keys[256], key_count;
    unsigned mode, formation_length;
    int force_return, sequence, following_result, restarted;
    uint16_t saved_tick, contact, completions, speed;
    uint16_t radar_hits, gun_hits;
    int16_t proximity_gate;
    uint8_t enemy_expiries;
    uint8_t phase, region, confirmation, message_b, message_c, entry_phase, previous_entry_phase;
    gaddr stage, previous_stage, entry_stage, previous_entry_stage;
    int begun, keep_entry, airborne, landed;
} Observation;

static void copy_ram(uint8_t *destination,const NativeFrontend *game) {
    memcpy(destination,game->storage.buffers,0x80000);
    memcpy(destination+0x80000,game->storage.source,0x80000);
}
static void snapshot(const Observation *run,const char *tag,unsigned index,
                     const char *suffix,const uint8_t *data) {
    char path[4096];
    int size=snprintf(path,sizeof path,"%s.%s.%u.%s.dat",run->prefix,tag,index,suffix);
    if(size<0 || size>=(int)sizeof path) abort();
    FILE *file=fopen(path,"wb");
    if(!file) {perror(path);abort();}
    int written=fwrite(data,1,0x100000,file)==0x100000;
    if(fclose(file) || !written) {fprintf(stderr,"Cannot write %s\n",path);abort();}
}
static void capture_budget(const Observation *run) {
    /* At most 480 MiB before comparison; passing cases are then removed. */
    if(run->captures+run->entries>=240) {
        fputs("Mission capture budget exhausted\n",stderr);abort();
    }
}
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,
                    uint16_t saved_tick,void *context) {
    Observation *run=context;
    if(boundary==NATIVE_FRAME_INPUT_BEGIN) {
        run->entry_stage=rd_u32(STAGE_CALLBACK);run->entry_phase=rd_u8(PLAYER_PHASE);
        run->entry_tick=game->ticks;run->key_count=game->input_count;
        run->keep_entry=run->entry_stage!=run->previous_entry_stage ||
            run->entry_phase!=run->previous_entry_phase || run->entry_stage==0xc110a4 ||
            (run->following_result && (run->key_count ||
             (run->entry_phase==0xfc && rd_u8(MESSAGE_STATE_B) && rd_u8(MESSAGE_STATE_C))));
        if(run->keep_entry) {
            copy_ram(run->entry_before,game);
            for(unsigned i=0;i<run->key_count;++i)
                run->entry_keys[i]=game->input_keys[(game->input_read+i)&255u];
        }
        run->previous_entry_stage=run->entry_stage;
        run->previous_entry_phase=run->entry_phase;
        return;
    }
    if(boundary==NATIVE_FRAME_BODY_BEGIN) {
        if(run->begun) abort();
        run->begun=1;++run->body;
        run->first_tick=game->ticks;run->saved_tick=saved_tick;
        run->iteration=game->update_iterations;run->stage=rd_u32(STAGE_CALLBACK);
        run->phase=rd_u8(PLAYER_PHASE);run->contact=rd_u16(CONTROL_RECORDS+2);
        run->region=rd_u8(CONTROL_RECORDS+4);run->speed=rd_u16(CONTROL_RECORDS+110);
        run->confirmation=rd_u8(PLAYER_FLAGS_F);
        run->message_b=rd_u8(MESSAGE_STATE_B);run->message_c=rd_u8(MESSAGE_STATE_C);
        run->completions=rd_u16(rd_u32(MODE_TABLE)+56);
        run->radar_hits=rd_u16(rd_u32(MODE_TABLE)+68);
        run->gun_hits=rd_u16(rd_u32(MODE_TABLE)+60);
        run->enemy_expiries=rd_u8(SCENE_DISPATCH_AUX);
        run->proximity_gate=rd_s16(SCENE_DISPATCH_GATE);
        copy_ram(run->before,game);
        if(run->keep_entry) {
            capture_budget(run);
            snapshot(run,"entry",run->entries,"before",run->entry_before);
            snapshot(run,"entry",run->entries,"after",run->before);
            printf("{\"entry\":%u,\"iteration\":%u,\"stage\":\"%06X\",\"tick\":%u,\"keys\":[",
                run->entries++,run->iteration,run->entry_stage,run->entry_tick);
            for(unsigned i=0;i<run->key_count;++i) printf("%s%u",i?",":"",run->entry_keys[i]);
            printf("],\"phase_before\":%u,\"phase_after\":%u,\"completions\":%u}\n",
                run->entry_phase,run->phase,run->completions);
            run->keep_entry=0;
        }
        if(run->following_result && run->entry_stage==0xc0f992 && run->stage==0xc0fcb4) run->restarted=1;
        return;
    }
    /* Menu ticks can publish BODY_END without entering a flight body. */
    if(!run->begun) return;
    run->begun=0;
    const uint16_t contact=rd_u16(CONTROL_RECORDS+2),speed=rd_u16(CONTROL_RECORDS+110);
    const uint8_t phase=rd_u8(PLAYER_PHASE),region=rd_u8(CONTROL_RECORDS+4);
    const uint8_t confirmation=rd_u8(PLAYER_FLAGS_F);
    const uint8_t message_b=rd_u8(MESSAGE_STATE_B),message_c=rd_u8(MESSAGE_STATE_C);
    const uint16_t completions=rd_u16(rd_u32(MODE_TABLE)+56);
    const uint16_t radar_hits=rd_u16(rd_u32(MODE_TABLE)+68);
    const uint16_t gun_hits=rd_u16(rd_u32(MODE_TABLE)+60);
    const uint8_t enemy_expiries=rd_u8(SCENE_DISPATCH_AUX);
    const int16_t proximity_gate=rd_s16(SCENE_DISPATCH_GATE);
    if(run->force_return && run->proximity_gate>=0 && proximity_gate<run->proximity_gate) {
        const int limit=rd_s8(SCENE_DISPATCH_LIMIT);
        printf("{\"proximity\":true,\"body\":%u,\"before_tick\":%u,\"after_tick\":%u,"
               "\"gate_before\":%d,\"gate_after\":%d,\"distance_limit\":%u,"
               "\"player_fixed\":[%d,%d,%d],\"stolen_fixed\":[%d,%d,%d]}\n",
               run->body,run->first_tick,game->ticks,run->proximity_gate,proximity_gate,
               limit>=3?0x18000u:limit>=2?0x24000u:0x30000u,
               rd_s32(CONTROL_RECORDS+20),rd_s32(CONTROL_RECORDS+24),rd_s32(CONTROL_RECORDS+28),
               rd_s32(CONTROL_RECORDS+0x814),rd_s32(CONTROL_RECORDS+0x818),rd_s32(CONTROL_RECORDS+0x81c));
    }
    if(run->force_return && ((run->proximity_gate==200 && proximity_gate<200 && proximity_gate>=0) ||
                            (run->proximity_gate>=0 && proximity_gate<0)))
        run->formation_window=run->formation_length;
    if((run->mode==4 || run->mode==5) && (radar_hits!=run->radar_hits || gun_hits!=run->gun_hits))
        run->combat_window=20; /* Include the original 15-tick expiry and its boundary. */
    if(rd_u8(MODE_SELECT)==run->mode && run->stage==0xc10dae && !(contact&0x80)) run->airborne=1;
    const int touchdown=run->airborne && !run->landed && (contact&0x80);
    if(touchdown) {run->landed=1;run->window=run->sequence?64u:96u;}
    /* C0A3EA admits carrier contact for pose three, runway contact otherwise. */
    const unsigned region_mask=rd_u8(SCENE_POSE_ENTRY)==3?0xc0u:4u;
    const int ready=run->landed && (region&region_mask) && !speed;
    const int ready_before=run->landed && (run->region&region_mask) && !run->speed;
    const int keep=run->stage!=run->previous_stage || run->body%(run->mode==3?512:1024)==0 ||
        run->window || run->combat_window || run->formation_window || phase!=run->phase || completions!=run->completions ||
        ((contact^run->contact)&(run->force_return?0x84:0x80)) || confirmation!=run->confirmation ||
        (run->landed && ((region^run->region)&region_mask)) || (ready && !ready_before) ||
        radar_hits!=run->radar_hits || gun_hits!=run->gun_hits ||
        (run->following_result && (message_b!=run->message_b || message_c!=run->message_c)) ||
        ((run->mode==4 || run->mode==5) && enemy_expiries!=run->enemy_expiries);
    if(radar_hits!=run->radar_hits || gun_hits!=run->gun_hits) {
        printf("{\"weapon_hit\":true,\"body\":%u,\"gun_before\":%u,\"gun_after\":%u,"
               "\"radar_before\":%u,\"radar_after\":%u,\"records\":[",
            run->body,run->gun_hits,gun_hits,run->radar_hits,radar_hits);
        for(unsigned slot=4;slot<=10;slot+=2) {
            const gaddr record=CONTROL_RECORDS+512*slot;
            const uint8_t *before=run->before+0x80000+record-0xc00000;
            printf("%s{\"slot\":%u,\"flags_before\":%u,\"flags_after\":%u,\"lifetime\":%d}",
                slot==4?"":",",slot,before[0]*256u+before[1],rd_u16(record),rd_s16(record+76));
        }
        puts("]}");
    }
    if(keep) {
        capture_budget(run);
        snapshot(run,"body",run->captures,"before",run->before);
        /* The entry buffer is free until the next INPUT_BEGIN. */
        copy_ram(run->entry_before,game);
        snapshot(run,"body",run->captures,"after",run->entry_before);
        printf("{\"capture\":%u,\"body\":%u,\"iteration\":%u,\"stage\":\"%06X\","
               "\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"owner_exit\":%s,"
               "\"phase_before\":%u,\"phase_after\":%u,\"contact_before\":%u,\"contact_after\":%u,"
               "\"confirmation_before\":%u,\"confirmation_after\":%u,\"selected\":%d,"
               "\"view_record\":%u,\"region_before\":%u,\"region_after\":%u,\"speed\":%u,"
               "\"completions_before\":%u,\"completions_after\":%u,"
               "\"touchdown\":%s,\"landing_window\":%s,\"ready\":%s,"
               "\"combat_window\":%s,\"enemy_expiries_before\":%u,\"enemy_expiries_after\":%u,"
               "\"proximity_gate_before\":%d,\"proximity_gate_after\":%d,\"formation_window\":%s,"
               "\"message_b_before\":%u,\"message_b_after\":%u,\"message_c_before\":%u,\"message_c_after\":%u}\n",
               run->captures++,run->body,run->iteration,run->stage,run->first_tick,game->ticks,
               run->saved_tick,boundary==NATIVE_FRAME_OWNER_EXIT?"true":"false",
               run->phase,phase,run->contact,contact,run->confirmation,confirmation,
               rd_s16(SELECTED_RECORD),rd_u16(VIEW_RECORD),run->region,region,speed,
               run->completions,completions,touchdown?"true":"false",
               run->window?"true":"false",ready?"true":"false",run->combat_window?"true":"false",
               run->enemy_expiries,enemy_expiries,run->proximity_gate,proximity_gate,
               run->formation_window?"true":"false",run->message_b,message_b,run->message_c,message_c);
    }
    if(run->window) --run->window;
    if(run->combat_window) --run->combat_window;
    if(run->formation_window) --run->formation_window;
    run->previous_stage=run->stage;
}

int main(int argc,char **argv) {
    /* Mission success gates and bounded normal-input diagnostics. */
    if(argc!=5 && argc!=6) {
        fputs("Usage: mission_success_test ADF fresh-save keys capture-prefix [3|4-mission|5|5-formation|5-mission|5-success|3-sequence|5-sequence]\n",stderr);
        return 1;
    }
    NativeFrontend *game=calloc(1,sizeof *game);MissionPilot pilot={.mode=3};Observation run={0};
    char error[256];int result=1;
    if(argc==6) {
        run.sequence=!strcmp(argv[5],"3-sequence") || !strcmp(argv[5],"5-sequence");
        pilot.complete_flight=!strcmp(argv[5],"4-mission") || !strcmp(argv[5],"5-mission") || !strcmp(argv[5],"5-success") || !strcmp(argv[5],"5-sequence");
        pilot.force_return=!strcmp(argv[5],"5-formation") || !strcmp(argv[5],"5-mission") ||
            !strcmp(argv[5],"5-success") || !strcmp(argv[5],"5-sequence");
        pilot.mode=pilot.force_return?5u:(unsigned)atoi(argv[5]);
    }
    if(pilot.mode!=3 && pilot.mode!=4 && pilot.mode!=5) {
        fprintf(stderr,"Unsupported mission mode: %s\n",argv[5]);
        goto done;
    }
    run.mode=pilot.mode;
    run.force_return=pilot.force_return;
    /* Sequence coverage reserves 64 landing bodies and the later callbacks
     * inside the same 480 MiB cap; success gates retain their longer windows. */
    run.formation_length=pilot.complete_flight?16u:32u;
    if(argc==6 && (!strcmp(argv[5],"5-success") || !strcmp(argv[5],"5-sequence"))) run.formation_length=4;
    run.prefix=argv[4];run.before=malloc(0x100000);run.entry_before=malloc(0x100000);
    if(!game || !run.before || !run.entry_before) goto done;
    pilot.keys=fopen(argv[3],"w");
    if(!pilot.keys) goto done;
    fputs("E9K_INPUT_V1\n",pilot.keys);
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    game->observe_frame=observe;game->frame_context=&run;
    const unsigned times[]={1800,3000,4500,6500,8000,14500};
    const int keys[]={32,54,282+(int)pilot.mode-3,13,13,50};
    const char *end=getenv("FA18_MISSION_END_TICK");
    const unsigned end_tick=end?(unsigned)strtoul(end,NULL,10):pilot.mode==3?32000u:60000u;
    while(game->ticks<end_tick) {
        for(unsigned i=0;i<(pilot.mode==3?6u:5u);++i) {
            if(game->ticks==times[i]) mission_pilot_event(&pilot,game,keys[i],1);
            if(game->ticks==times[i]+2) mission_pilot_event(&pilot,game,keys[i],0);
        }
        if(pilot.target_press && game->ticks==pilot.target_press+2) mission_pilot_event(&pilot,game,116,0);
        if((pilot.mode==4 || pilot.mode==5) && pilot.started) for(unsigned i=0;i<(pilot.complete_flight?2u:3u);++i) {
            if(game->ticks==pilot.started+700+20*i) mission_pilot_event(&pilot,game,13,1);
            if(game->ticks==pilot.started+702+20*i) mission_pilot_event(&pilot,game,13,0);
        }
        mission_pilot_tick(&pilot,game);native_frontend_tick(game);
        if(game->postflight_resets) break;
        if(pilot.started && rd_u16(rd_u32(MODE_TABLE)+56)!=pilot.completions) break;
    }
    const unsigned mode=rd_u8(MODE_SELECT),phase=rd_u8(PLAYER_PHASE),ticks=game->ticks;
    const unsigned resets=game->postflight_resets,region=rd_u8(CONTROL_RECORDS+4);
    const unsigned speed=rd_u16(CONTROL_RECORDS+110),contact=rd_u16(CONTROL_RECORDS+2);
    const gaddr log=rd_u32(MODE_TABLE),stage=rd_u32(STAGE_CALLBACK);
    const unsigned completions=rd_u16(log+56),grade=rd_u8(log+18+pilot.mode);
    if(pilot.mode==4 || pilot.mode==5)
        printf("{\"diagnostic_end\":true,\"mode\":%u,\"phase\":%u,\"tick\":%u,\"crash_resets\":%u,"
               "\"completions_before\":%u,\"completions_after\":%u,\"stage\":\"%06X\"}\n",
            mode,phase,ticks,resets,pilot.completions,completions,stage);
    const unsigned region_mask=rd_u8(SCENE_POSE_ENTRY)==3?0xc0u:4u;
    if(!run.airborne || !run.landed || !pilot.objective || !pilot.started || resets ||
       mode!=pilot.mode || phase!=0xfc || !(contact&0x80) || !(region&region_mask) || speed ||
       completions!=pilot.completions+1 || rd_u8(log+6)!=mode || rd_u8(log+7)!=pilot.grade ||
       grade!=(pilot.grade<3?pilot.grade+1:3)) {
        fprintf(stderr,"Mission incomplete: tick %u, mode %u, phase %u, completions %u -> %u, resets %u\n",
            ticks,mode,phase,pilot.completions,completions,resets);goto done;
    }
    uint8_t saved[78];char path[4096];
    if(snprintf(path,sizeof path,"%s/config",argv[2])>=(int)sizeof path) goto done;
    FILE *file=fopen(path,"rb");
    if(!file) {perror(path);goto done;}
    int read=fread(saved,1,sizeof saved,file)==sizeof saved && fgetc(file)==EOF;
    if(fclose(file) || !read) goto done;
    for(unsigned i=0;i<sizeof saved;++i) if(saved[i]!=rd_u8(log+i)) goto done;
    if(run.sequence) {
        /* Flight is complete. Release the pilot's held keys through the same
         * host input path before the result messages and C0F992 restart. */
        const int held[]={pilot.rudder,pilot.pitch,pilot.roll,pilot.throttle,pilot.fire,pilot.hook};
        unsigned restart_key_tick=0;int restart_key_released=0;
        run.following_result=1;
        for(unsigned i=0;i<sizeof held/sizeof held[0];++i)
            if(held[i]) mission_pilot_event(&pilot,game,held[i],0);
        while(game->ticks<ticks+5000u && !game->postflight_resets) {
            /* Successful mission messages return FC to phase four, leaving
             * the aircraft parked. Normal Escape (C1C224, phase one) requests
             * the viewport/bootstrap restart through C0F5F8. */
            if(!restart_key_tick && rd_u8(PLAYER_PHASE)==4 && rd_s8(MESSAGE_STATE_C)<0) {
                restart_key_tick=game->ticks;
                printf("{\"result_messages_finished\":true,\"tick\":%u,\"phase\":4,"
                       "\"message_state\":%d,\"restart_key\":27}\n",game->ticks,rd_s8(MESSAGE_STATE_C));
                mission_pilot_event(&pilot,game,27,1);
            }
            if(restart_key_tick && !restart_key_released && game->ticks>=restart_key_tick+2) {
                mission_pilot_event(&pilot,game,27,0);restart_key_released=1;
            }
            native_frontend_tick(game);
            if(run.restarted && rd_u32(STAGE_CALLBACK)==0xc0fcb4) break;
        }
        if(!run.restarted || !restart_key_released || game->postflight_resets || game->input_count ||
           game->screen!=NATIVE_MENU || rd_u32(STAGE_CALLBACK)!=0xc0fcb4 ||
           rd_u8(PLAYER_PHASE) || rd_u8(RECORDER_MODE)) {
            fprintf(stderr,"Mission result restart incomplete: tick %u, stage %06X, phase %u\n",
                game->ticks,rd_u32(STAGE_CALLBACK),rd_u8(PLAYER_PHASE));goto done;
        }
        for(unsigned i=0;i<sizeof saved;++i) if(saved[i]!=rd_u8(rd_u32(MODE_TABLE)+i)) {
            fprintf(stderr,"Mission log changed during restart at byte %u\n",i);goto done;
        }
        printf("{\"sequence\":true,\"restarted\":%s,\"ticks\":%u,\"stage\":\"%06X\","
               "\"mode\":%u,\"phase\":%u,\"queued\":%u,\"crash_resets\":%u,"
               "\"completions\":%u,\"grade\":%u,\"menu\":true,\"log_unchanged\":true}\n",
               run.restarted?"true":"false",game->ticks,rd_u32(STAGE_CALLBACK),rd_u8(MODE_SELECT),
               rd_u8(PLAYER_PHASE),game->input_count,game->postflight_resets,
               rd_u16(rd_u32(MODE_TABLE)+56),rd_u8(rd_u32(MODE_TABLE)+18+pilot.mode));
    }
    native_frontend_close(game);
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {
        fprintf(stderr,"Reload failed: %s\n",error);goto done;
    }
    const gaddr loaded=rd_u32(MODE_TABLE);
    for(unsigned i=0;i<sizeof saved;++i) if(saved[i]!=rd_u8(loaded+i)) goto done;
    printf("{\"finished\":true,\"airborne\":true,\"landed\":true,\"objective\":true,"
           "\"mode\":%u,\"stage\":\"%06X\",\"phase\":%u,\"ticks\":%u,\"target\":%u,"
           "\"completions_before\":%u,\"completions_after\":%u,\"grade_before\":%u,\"grade_after\":%u,"
           "\"region\":%u,\"speed\":%u,\"crash_resets\":%u,\"reloaded\":true}\n",
           mode,stage,phase,ticks,pilot.target,pilot.completions,completions,pilot.grade,grade,
           region,speed,resets);
    result=0;
done:
    if(pilot.keys) fclose(pilot.keys);
    if(game) native_frontend_close(game);
    free(run.entry_before);free(run.before);free(game);return result;
}
