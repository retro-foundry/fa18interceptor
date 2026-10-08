/* Actual menu/mission paths, sharing the playable runtime's objects.
 * Capture the first body at each source stage and each playback stream. */
#include "native/frontend.h"
#include "../../port/native/frame_capture.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gun_discovery.h"

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
    unsigned body_serial,hit_serial;
    unsigned previous_stream,stream_wraps;
    unsigned eject,ejection;
    unsigned callback,callback_bodies,callback_events;
    unsigned smoothing_cancel,cancel_phase,cancelled;
    unsigned weapon,launch_bodies,launched,removed;
    unsigned hit_probe;
    unsigned gun_approach,gun_bodies;
    GunDiscovery gun;
    unsigned kill_probe,kill_started,kill_accounted,kill_inactive,kill_bodies;
    gaddr kill_record;
    uint8_t kill_baseline,kill_count;
    uint16_t kill_last_flags;
    int16_t kill_last_lifetime;
    uint8_t *hit_before;
    unsigned hit_tracking,hit_captured,hit_before_tick,hit_saved_tick;
    gaddr hit_log;
    gaddr hit_stage;
    uint16_t hit_before_count;
    unsigned projectile_states[3];
    uint16_t stock,ammo,counters[3];
    int weapon_baseline;
    unsigned weapon_checked;
    uint16_t weapon_stock,weapon_ammo,weapon_counters[3];
    unsigned flight,region_mask,occupied_mask,spawns,zone_exits,pending_exits,npc_missiles;
    uint16_t hit_baseline[3],hit_counts[3];
    uint32_t record_states[16];
    uint16_t guidance_cases[16][512];
    unsigned guidance_case_counts[16];
    uint32_t record_cases[16][32];
    unsigned record_case_counts[16];
    uint32_t initial_position[3];
    uint32_t aircraft_positions[16][3];
    unsigned aircraft_moved;
    unsigned outcome,failure_seen,reset_states,outcome_transitions,outcome_controls;
    unsigned last_outcome_state,outcome_baseline;
    uint16_t initial_losses;
    int flight_baseline,moved;
    int entered,returned;
} ModeRun;
static int inside_region(gaddr region,gaddr record) {
    const int16_t x=rd_s16(record+6),z=rd_s16(record+8);
    return x>=rd_s16(region) && x<=rd_s16(region+2) &&
           z>=rd_s16(region+4) && z<=rd_s16(region+6);
}
static unsigned hit_counter_offset(unsigned weapon) {
    return weapon==3?60u:60u+4u*weapon; /* C266AE gun; C26EBE missile counters. */
}
static void write_hit_snapshot(const ModeRun *run,const char *suffix,const uint8_t *data) {
    char path[4096];
    const int length=snprintf(path,sizeof path,"%s.hit.%s.dat",run->prefix,suffix);
    if(length<0 || length>=(int)sizeof path) abort();
    FILE *file=fopen(path,"wb");
    if(!file) {perror(path);abort();}
    const int written=fwrite(data,1,0x100000,file)==0x100000;
    if(fclose(file) || !written) {fprintf(stderr,"Cannot write hit capture: %s\n",path);abort();}
}
static void observe(NativeFrontend *game,enum NativeFrameBoundary boundary,
                    uint16_t saved_tick,void *context) {
    ModeRun *run=context;
    run->clock.iteration=game->update_iterations;
    if(boundary==NATIVE_FRAME_BODY_BEGIN) ++run->body_serial;
    if(boundary==NATIVE_FRAME_BODY_BEGIN && run->weapon==3) gun_discovery_trace(&run->gun,game->ticks);
    /* A single bounded in-memory before-state while selected player ordnance
     * is active. Export only the actual collision body, never seed gameplay. */
    if(run->hit_probe && !run->hit_captured && boundary==NATIVE_FRAME_BODY_BEGIN) {
        run->hit_tracking=0;
        if(run->weapon!=3) for(unsigned slot=1;slot<=3;++slot) {
            const gaddr projectile=CONTROL_RECORDS+slot*CONTROL_RECORD_BYTES;
            if((rd_u8(projectile+1)&0x48u)==0x48u && !rd_u8(projectile+94) &&
               rd_u8(projectile+98)==2u-run->weapon) run->hit_tracking=1;
        }
        if(run->weapon==3) {
            for(unsigned slot=0;slot<20;++slot)
                if(rd_u16(0xc45c72u+64*slot+38)&1u) run->hit_tracking=1;
        }
        if(run->hit_tracking) {
            memcpy(run->hit_before,game->storage.buffers,0x80000);
            memcpy(run->hit_before+0x80000,game->storage.source,0x80000);
            run->hit_before_tick=game->ticks;run->hit_saved_tick=saved_tick;
            run->hit_serial=run->body_serial;
            run->hit_stage=rd_u32(STAGE_CALLBACK);
            run->hit_log=rd_u32(MODE_TABLE);
            run->hit_before_count=rd_u16(run->hit_log+hit_counter_offset(run->weapon));
        }
    }
    if(boundary==NATIVE_FRAME_INPUT_BEGIN && rd_u8(MODE_SELECT)==run->mode) {
        gaddr stage=rd_u32(STAGE_CALLBACK);
        if(run->smoothing_cancel && !run->cancel_phase && stage==0xc10a24) {
            /* Controlled cancel marker at a naturally reached source stage.
             * Preflight normally gates keypad input here. Only this validation
             * entry selects its rare cancel condition; no return is seeded. */
            wr_u8(KEY_TAKEN,2);
            run->cancel_phase=1;run->callback_bodies=2;
        } else if(run->smoothing_cancel && run->cancel_phase==1) {
            run->cancel_phase=2;
        }
        gaddr key=stage|((rd_s16(POST_INPUT_COUNTDOWN)<=0)?0x1000000u:0);
        if(run->mode==125 && game->ticks>=11000) key|=0x2000000u;
        unsigned index=0;
        while(index<run->entry_count && run->entry_stages[index]!=key) ++index;
        const unsigned outcome_state=stage^((unsigned)rd_u8(POSTFLIGHT_RESET_REMAINING)<<24);
        const int outcome_changed=run->outcome && run->outcome_baseline && outcome_state!=run->last_outcome_state;
        if(index==run->entry_count || game->input_count || outcome_changed) {
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
    if(boundary==NATIVE_FRAME_BODY_BEGIN && (rd_u8(MODE_SELECT)==run->mode ||
       (run->flight==2 && run->entered))) {
        run->entered=1;
        gaddr stage=rd_u32(STAGE_CALLBACK);
        if(run->smoothing_cancel && run->cancel_phase==1) {
            if(stage!=0xc10c68 || rd_u16(POST_INPUT_COUNTDOWN)!=5 ||
               !rd_u8(POST_INPUT_AUX) || rd_u8(CONTEXT_SMOOTH) ||
               !rd_u8(CONTEXT_STARTED) || !rd_u8(MENU_TRANSITION_FLAG) ||
               rd_u8(CONTEXT_GATE) || rd_u8(CONTEXT_AUX) || rd_u8(POST_INPUT_EVENT)) {
                fprintf(stderr,"Smoothing cancel did not execute the source return/reset path: stage=%06X key=%u countdown=%u aux=%u smooth=%u started=%u transition=%u gate=%u context=%u event=%u\n",
                    stage,rd_u8(KEY_TAKEN),rd_u16(POST_INPUT_COUNTDOWN),rd_u8(POST_INPUT_AUX),
                    rd_u8(CONTEXT_SMOOTH),rd_u8(CONTEXT_STARTED),rd_u8(MENU_TRANSITION_FLAG),
                    rd_u8(CONTEXT_GATE),rd_u8(CONTEXT_AUX),rd_u8(POST_INPUT_EVENT));abort();
            }
            run->cancelled=1;
        }
        if(run->smoothing_cancel && run->cancel_phase==2 && stage==0xc10dae)
            run->cancelled|=2;
        gaddr key=stage|((run->mode==125 && game->ticks>=11000)?0x2000000u:0);
        unsigned index=0;
        while(index<run->stage_count && run->stages[index]!=key) ++index;
        const unsigned stream=rd_u8(0xc45799u);
        const unsigned bit=stream<8?1u<<stream:0;
        unsigned sample=0;
        if(run->gun_approach && run->hit_tracking && run->gun_bodies<32) {
            ++run->gun_bodies;sample|=8;
        }
        if(run->kill_probe && run->kill_started && !run->kill_inactive) {
            if(run->kill_bodies>=768) {
                fputs("Enemy destruction exceeded the bounded 768-body capture window\n",stderr);abort();
            }
            sample|=8;
        }
        if(run->mode==2) {
            /* Compare the complete seventh-stream interval, including its
             * actual C233AA -> C28722 reset and C23578 wrap to stream one. */
            if(stream==7) sample|=8;
            if(run->previous_stream==7 && stream==1) {++run->stream_wraps;sample|=8;}
            run->previous_stream=stream;
        }
        if(run->outcome) {
            const unsigned remaining=rd_u8(POSTFLIGHT_RESET_REMAINING);
            const unsigned state=stage^(remaining<<24);
            if(stage==0xc10dae && !run->outcome_baseline) {
                run->outcome_baseline=1;
                run->initial_losses=rd_u16(rd_u32(MODE_TABLE)+0x10);
            }
            if(run->outcome_baseline) {
                if(remaining<=3) run->reset_states|=1u<<remaining;
                if(state!=run->last_outcome_state) {
                    run->last_outcome_state=state;sample|=8;
                    ++run->outcome_transitions;
                    printf("{\"outcome_transition\":true,\"tick\":%u,\"stage\":\"%06X\",\"resets_remaining\":%u,\"player_phase\":%u,\"sequence_phase\":%u}\n",
                        game->ticks,stage,remaining,rd_u8(PLAYER_PHASE),rd_u8(SEQUENCE_PHASE));
                }
                if(stage==0xc118a0 || stage==0xc118e6) run->failure_seen=1;
            }
        }
        if(run->callback_bodies) {--run->callback_bodies;sample|=8;}
        /* Keep sampling active postflight bodies after the flight baseline;
         * these still run record dynamics beyond stage C10DAE. */
        if(run->flight && (stage==0xc10dae || (run->flight==2 && run->flight_baseline &&
           rd_u8(MODE_SELECT)==run->mode && rd_u8(POST_INPUT_AUX)))) {
            if(!run->flight_baseline) {
                run->flight_baseline=1;
                for(unsigned i=0;i<3;++i)
                    run->hit_baseline[i]=rd_u16(rd_u32(MODE_TABLE)+60+4*i);
                for(unsigned i=0;i<3;++i) run->initial_position[i]=rd_u32(CONTROL_RECORDS+20+4*i);
                for(unsigned slot=0;slot<16;++slot)
                    for(unsigned i=0;i<3;++i)
                        run->aircraft_positions[slot][i]=rd_u32(CONTROL_RECORDS+slot*CONTROL_RECORD_BYTES+20+4*i);
            }
            for(unsigned i=0;i<3;++i) {
                const uint16_t hits=(uint16_t)(rd_u16(rd_u32(MODE_TABLE)+60+4*i)-run->hit_baseline[i]);
                if(hits!=run->hit_counts[i]) {run->hit_counts[i]=hits;sample|=8;}
            }
            for(unsigned i=0;i<3;++i)
                if(rd_u32(CONTROL_RECORDS+20+4*i)!=run->initial_position[i]) run->moved=1;
            unsigned mask=0,exits=0;
            for(unsigned i=0;i<8 && rd_s16(0xc29720u+4*i)>=0;++i)
                if(inside_region(rd_u32(0xc29720u+4*i),CONTROL_RECORDS)) mask|=1u<<i;
            const unsigned occupied=rd_u8(0xc4579du);
            if(mask!=run->region_mask || occupied!=run->occupied_mask ||
               (mask!=occupied && (rd_u16(0xc458dau)&15)==3)) sample|=8;
            run->region_mask=mask;run->occupied_mask=occupied;
            for(unsigned slot=0;slot<16;++slot) {
                const gaddr record=CONTROL_RECORDS+slot*CONTROL_RECORD_BYTES;
                if((rd_u8(record+98)&0xf0)==0x10 && (rd_u16(record)&0xc0)==0xc0)
                    for(unsigned i=0;i<3;++i)
                        if(rd_u32(record+20+4*i)!=run->aircraft_positions[slot][i]) run->aircraft_moved|=1u<<slot;
                /* Sample actual manoeuvre/altitude-limit crossings, including
                 * the countdown arm that calls C06C02. No state is seeded. */
                if(run->flight==2 && (rd_u8(record+98)&0xf0)==0x10 &&
                   (rd_u16(record)&0xc0)==0xc0) {
                    const uint16_t guidance=rd_u8(record+5)|
                        ((uint16_t)(rd_s16(record+76)<0?0:rd_s16(record+76)<2?rd_s16(record+76)+1:3)<<8)|
                        ((uint16_t)(rd_s32(record+66)<0)<<10)|
                        ((uint16_t)((int32_t)rd_s16(record+108)*64>=rd_s32(record+24))<<11)|
                        ((uint16_t)(rd_s16(record+106)<0x320)<<12)|
                        ((uint16_t)(rd_s16(record+106)<0x3840)<<13)|
                        ((uint16_t)(rd_s16(record+102)<0x3840)<<14)|
                        ((uint16_t)(rd_s16(record+102)>=0x68b0)<<15);
                    unsigned found=0;
                    while(found<run->guidance_case_counts[slot] && run->guidance_cases[slot][found]!=guidance) ++found;
                    if(found==run->guidance_case_counts[slot]) {
                        if(found==512) abort();
                        run->guidance_cases[slot][run->guidance_case_counts[slot]++]=guidance;sample|=8;
                    }
                }
                const unsigned zone=rd_u8(record+93);
                const uint32_t state=(rd_u8(record+1)&0x41u)|((uint32_t)rd_u8(record+56)<<8)|
                    ((uint32_t)zone<<16)|((uint32_t)rd_u8(record+122)<<24);
                const uint32_t previous=run->record_states[slot];
                /* First occurrence of each activity/launch/zone/phase and
                 * signed target class; repeated boundary oscillation retains
                 * its actual gameplay updates without duplicating snapshots. */
                const uint32_t key=state&0xffff80ffu;
                unsigned found=0;
                while(found<run->record_case_counts[slot] && run->record_cases[slot][found]!=key) ++found;
                if(found==run->record_case_counts[slot]) {
                    if(found==32) abort();
                    run->record_cases[slot][run->record_case_counts[slot]++]=key;sample|=8;
                }
                if((slot==9 || slot==11 || slot==13) && (state&0x40) && rd_u8(record+98)<=1)
                    run->npc_missiles|=1u<<slot;
                if(game->ticks>=10000 && (state&0x40) && !(previous&0x40) && zone>=1 && zone<=8)
                    run->spawns|=1u<<slot;
                if((state>>24)==5 && ((previous>>24)==3 || (previous>>24)==4))
                    run->zone_exits|=1u<<slot;
                run->record_states[slot]=state;
                if((state&0x40) && zone>=1 && zone<=8 && (rd_u8(record+98)&0xf0)==0x10 &&
                   rd_u8(record+5)!=8 && !inside_region(rd_u32(0xc29720u+4*(zone-1)),record))
                    exits|=1u<<slot;
            }
            if(exits&~run->pending_exits) sample|=8;
            run->pending_exits|=exits;
        }
        if(run->weapon && stage==0xc10dae && !run->weapon_baseline) {
            run->weapon_baseline=1;
            run->stock=rd_u8(CONTROL_RECORDS+95);
            run->ammo=rd_u16(CONTROL_RECORDS+96);
            gaddr log=rd_u32(MODE_TABLE);
            run->counters[0]=rd_u16(log+58);
            run->counters[1]=rd_u16(log+62);
            run->counters[2]=rd_u16(log+66);
        }
        if(run->weapon && !run->hit_probe && run->weapon_baseline &&
           !run->weapon_checked && stage==0xc11788) {
            /* Observe consumption before the natural postflight reset
             * restores the player's stores. Keep the later reset exercised. */
            run->weapon_checked=1;
            run->weapon_stock=rd_u8(CONTROL_RECORDS+95);
            run->weapon_ammo=rd_u16(CONTROL_RECORDS+96);
            for(unsigned i=0;i<3;++i)
                run->weapon_counters[i]=rd_u16(rd_u32(MODE_TABLE)+58+4*i);
            printf("{\"weapon_checkpoint\":true,\"tick\":%u,\"stock\":%u,\"gun_ammo\":%u,\"initial_stock\":%u,\"initial_gun_ammo\":%u}\n",
                game->ticks,run->weapon_stock,run->weapon_ammo,run->stock,run->ammo);
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
            printf("{\"capture\":%u,\"stage\":\"%06X\",\"stream\":%u,\"body_serial\":%u,",run->captures,stage,stream,run->body_serial);
        }
    }
    if(run->capture.prefix && !run->capture.complete) {
        native_frame_capture(game,boundary,saved_tick,&run->capture);
        if(run->capture.complete) {
            printf("\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"owner_exit\":%s}\n",
                run->capture.before_tick,run->capture.after_tick,run->capture.saved_tick,
                run->capture.owner_exit?"true":"false");
            ++run->captures;
        }
    }
    if(run->hit_tracking && (boundary==NATIVE_FRAME_BODY_END || boundary==NATIVE_FRAME_OWNER_EXIT)) {
        run->hit_tracking=0;
        const uint16_t hits=rd_u16(run->hit_log+hit_counter_offset(run->weapon));
        if(hits!=run->hit_before_count) {
            if(run->kill_probe) {
                const unsigned index=rd_u16(0xc4fdd2u);
                if(index>=0x2000u || (index&511u)) abort();
                run->kill_record=CONTROL_RECORDS+index;
                const uint8_t *before=run->hit_before+0x80000+run->kill_record-0xc00000;
                const unsigned flags=before[0]*256u+before[1];
                if((flags&0x1048u)!=0x1040u || (before[98]&0xf0u)!=0x10u ||
                   (rd_u16(run->kill_record)&0x600u)!=0x400u || rd_s16(run->kill_record+76)!=15) {
                    fputs("Missile hit did not start enemy aircraft destruction\n",stderr);abort();
                }
                run->kill_started=1;
                run->kill_baseline=run->hit_before[0x80000+0x458ab];
                run->kill_count=run->kill_baseline;
            }
            /* C4FDD2 need not change on a first gun hit. */
            char impact_record[16];
            snprintf(impact_record,sizeof impact_record,"%u",rd_u16(0xc4fdd2u));
            write_hit_snapshot(run,"before",run->hit_before);
            printf("{\"capture\":\"hit\",\"hit_body\":true,\"stage\":\"%06X\",\"body_serial\":%u,\"before_tick\":%u,\"after_tick\":%u,\"saved_tick\":%u,\"owner_exit\":%s,\"pilot_log\":%u,\"weapon\":%u,\"hits_before\":%u,\"hits_after\":%u,\"impact_record\":%s,\"records\":[",
                run->hit_stage,run->hit_serial,run->hit_before_tick,game->ticks,run->hit_saved_tick,
                boundary==NATIVE_FRAME_OWNER_EXIT?"true":"false",run->hit_log,run->weapon,run->hit_before_count,hits,
                run->weapon==3?"null":impact_record);
            for(unsigned slot=0;slot<16;++slot) {
                const gaddr record=CONTROL_RECORDS+slot*CONTROL_RECORD_BYTES;
                const uint8_t *before=run->hit_before+0x80000+record-0xc00000;
                printf("%s{\"slot\":%u,\"flags_before\":%u,\"flags_after\":%u,\"kind_before\":%u,\"kind_after\":%u,\"damage_before\":%u,\"damage_after\":%u,\"lifetime_before\":%d,\"lifetime_after\":%d,\"phase_before\":%u,\"phase_after\":%u}",
                    slot?",":"",slot,(unsigned)(before[0]*256u+before[1]),rd_u16(record),
                    before[98],rd_u8(record+98),before[60],rd_u8(record+60),
                    (int16_t)(before[76]*256u+before[77]),rd_s16(record+76),before[122],rd_u8(record+122));
            }
            puts("]}");
            memcpy(run->hit_before,game->storage.buffers,0x80000);
            memcpy(run->hit_before+0x80000,game->storage.source,0x80000);
            write_hit_snapshot(run,"after",run->hit_before);
            ++run->hit_captured;
        }
    }
    if(run->kill_probe && run->kill_started && !run->kill_inactive &&
       (boundary==NATIVE_FRAME_BODY_END || boundary==NATIVE_FRAME_OWNER_EXIT)) {
        const uint16_t flags=rd_u16(run->kill_record);
        const int16_t lifetime=rd_s16(run->kill_record+76);
        const uint8_t count=rd_u8(SCENE_DISPATCH_AUX);
        if(flags!=run->kill_last_flags || lifetime!=run->kill_last_lifetime || count!=run->kill_count) {
            printf("{\"kill_transition\":true,\"body_serial\":%u,\"tick\":%u,\"record\":%u,\"flags\":%u,\"lifetime\":%d,\"enemy_expiries\":%u}\n",
                run->body_serial,game->ticks,run->kill_record,flags,lifetime,count);
            run->kill_last_flags=flags;run->kill_last_lifetime=lifetime;
        }
        ++run->kill_bodies;
        if(!(flags&0x400u) && (uint8_t)(count-run->kill_baseline)==1) run->kill_accounted=1;
        if(run->kill_accounted && !(flags&0x40u)) run->kill_inactive=1;
        run->kill_count=count;
    }
}
int main(int argc,char **argv) {
    if(argc<4 || argc>7) return 1;
    NativeFrontend *game=calloc(1,sizeof *game);char error[256];int result=1;
    ModeRun run={.prefix=argv[3],.mode=argc>=5?(unsigned)atoi(argv[4]):2};
    unsigned aircraft=argc>=6?(unsigned)atoi(argv[5]):1;
    if(argc==7) {
        if(!strcmp(argv[6],"callback") && run.mode==125) run.callback=1;
        else if(!strcmp(argv[6],"smoothing") && run.mode==4) run.smoothing_cancel=1;
        else if(!strcmp(argv[6],"combat") && (run.mode>=5 && run.mode<=8)) run.flight=2;
        else if(!strcmp(argv[6],"hit") && run.mode==8) {run.flight=2;run.weapon=2;run.hit_probe=1;}
        else if(!strcmp(argv[6],"kill") && run.mode==8) {run.flight=2;run.weapon=2;run.hit_probe=1;run.kill_probe=1;}
        else if(!strcmp(argv[6],"infrared-hit") && run.mode==8) {run.flight=2;run.weapon=1;run.hit_probe=1;}
        else if(!strcmp(argv[6],"infrared-kill") && run.mode==8) {run.flight=2;run.weapon=1;run.hit_probe=1;run.kill_probe=1;}
        else if(!strcmp(argv[6],"gun-hit") && run.mode==8) {run.flight=2;run.weapon=3;run.hit_probe=1;}
        else if(!strcmp(argv[6],"gun-approach") && run.mode==8) {run.flight=2;run.weapon=3;run.hit_probe=1;run.gun_approach=1;}
        else if(!strcmp(argv[6],"outcome") && run.mode==6) { run.flight=2;run.outcome=1; }
        else if(!strcmp(argv[6],"flight") && run.mode==4) run.flight=1;
        else if(!strcmp(argv[6],"eject") && run.mode==8) run.eject=1;
        else if(run.mode==8 && !strncmp(argv[6],"weapon",6) && strlen(argv[6])==7 && argv[6][6]>='1' && argv[6][6]<='3')
            run.weapon=(unsigned)(argv[6][6]-'0');
        else return 1;
    }
    if((run.mode!=2 && run.mode!=3 && run.mode!=4 && run.mode!=5 && run.mode!=6 && run.mode!=7 && run.mode!=8 && run.mode!=125) ||
       aircraft<1 || aircraft>2) return 1;
    if(!game) return 1;
    if(run.hit_probe) {
        run.hit_before=malloc(0x100000);
        if(!run.hit_before) goto done;
    }
    if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
    if(run.mode==7 || run.mode==8) {
        /* A saved-pilot fixture unlocks the original availability byte.
         * Reopen through the normal loader before any gameplay/input; only
         * validation creates this fixture, never the playable runtime. */
        native_frontend_enlist(game); /* C162E4 must establish file readiness before C1643A. */
        if(!rd_u16(MENU_FILE_READY) || rd_u16(MENU_TABLE_STATUS)) {
            fputs("eligible-pilot fixture has no writable source config file\n",stderr);goto done;
        }
        gaddr log=rd_u32(MODE_TABLE);
        if(!rd_u16(log)) goto done;
        wr_u8(log+0x12u+run.mode-1,1);
        native_frontend_save_log(game);
        native_frontend_close(game);
        if(!native_frontend_open(game,argv[1],argv[2],error,sizeof error)) {fprintf(stderr,"%s\n",error);goto done;}
        if(!rd_u8(rd_u32(MODE_TABLE)+0x12u+run.mode-1)) {
            fputs("eligible-pilot fixture did not persist its availability byte\n",stderr);goto done;
        }
    }
    game->observe_frame=observe;game->frame_context=&run;
    const unsigned times[]={1800,3000,5000,6500,11000,15000,16500};
    const int keys[]={32,run.mode==125?52:run.mode==6?55:51,13,13,27,13,13};
    const unsigned mission_times[]={1800,3000,4500,6500,8000,14500};
    const int mission_keys[]={32,54,282+(int)run.mode-3,13,13,48+(int)aircraft};
    const int mission=(run.mode>=3 && run.mode<=5) || run.mode==7 || run.mode==8;
    const unsigned *input_times=mission?mission_times:times;
    const int *input_keys=mission?mission_keys:keys;
    unsigned input_count=run.mode==3?6u:mission?5u:run.mode==125?7u:run.mode==2?5u:4u;
    while(game->ticks<(run.outcome?120000u:run.flight==2?60000u:run.flight?30000u:(mission || run.mode==125 || run.mode==2)?18000u:10000u)) {
        if(run.callback) {
            if(game->ticks==10000) {
                run.callback_events=game->input_events;
                native_frontend_event(game,127,1);run.callback_bodies=2;
            }
            if(game->ticks==10002) native_frontend_event(game,127,0);
            if(game->ticks==10040 && (!game->input_server_installed ||
               game->input_events!=run.callback_events+2)) {
                fputs("Delete callback reset did not consume both events and reinstall the PAL server\n",stderr);goto done;
            }
        }
        if(run.flight) {
            const unsigned times[]={10000,11500,11200,11220,13000,14000,16000,17000};
            const unsigned durations[]={10000,500,2,2,2,100,2,100};
            const int keys[]={61,274,13,13,116,32,116,32};
            /* Mode-eight's sustained combat probe needs a bounded pull-up;
             * holding it for 500 ticks now naturally ends this flight early.
             * Keep its long-flight guards and guide it with ordinary keys. */
            const unsigned pitch_duration=run.flight==2 && run.mode==8?100u:durations[1];
            for(unsigned i=0;i<8;++i) {
                if(run.hit_probe && (i==2 || i==3)) continue;
                if(game->ticks==times[i]) native_frontend_event(game,keys[i],1);
                if(game->ticks==times[i]+(i==1?pitch_duration:durations[i])) native_frontend_event(game,keys[i],0);
            }
        }
        if(run.outcome && game->ticks>=20000 && rd_u32(STAGE_CALLBACK)==0xc10dae &&
           rd_u8(POSTFLIGHT_RESET_REMAINING)<=3 &&
           !(run.outcome_controls&(1u<<rd_u8(POSTFLIGHT_RESET_REMAINING)))) {
            /* Ordinary keyboard controls drive the player into the ground.
             * Reset clears source input latches, so release and press again
             * after each reset instead of writing motion or control state. */
            run.outcome_controls|=1u<<rd_u8(POSTFLIGHT_RESET_REMAINING);
            native_frontend_event(game,61,0);
            native_frontend_event(game,273,0);
            native_frontend_event(game,61,1);
            native_frontend_event(game,273,1);
        }
        if(run.weapon) {
            for(unsigned i=0;i<run.weapon;++i) {
                if(game->ticks==11000+20*i) native_frontend_event(game,13,1);
                if(game->ticks==11002+20*i) native_frontend_event(game,13,0);
            }
            for(unsigned i=0;i<2 && !run.hit_probe;++i) {
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
        if(run.outcome && run.returned) break;
    }
    if(run.captures<8 || (run.mode==2 &&
       (!run.returned || game->scene_frames<30 || !run.stream_wraps || run.streams!=255)) ||
       (run.mode==125 && (!run.returned || game->scene_frames<2000 || (run.samples&7)!=7 ||
        rd_u32(STAGE_CALLBACK)!=0xc10dae)) ||
       (run.mode==6 && !run.flight && (game->scene_frames<384 || run.samples!=7 ||
        rd_u32(STAGE_CALLBACK)!=0xc10dae)) ||
       (run.mode==3 && (game->scene_frames<768 || run.samples!=7 ||
        rd_u8(RECORDER_MODE)!=0 || rd_u8(0xc45849)!=0x12u-aircraft ||
        rd_u32(STAGE_CALLBACK)!=0xc10dae)) ||
       (run.eject && (!run.returned || run.ejection!=0x1ff || game->scene_frames<512 ||
        rd_u8(MODE_SELECT)!=0 || rd_u32(STAGE_CALLBACK)!=0xc0fcb4)) ||
       ((!run.eject && !run.smoothing_cancel && run.flight!=2 && (run.mode==4 || run.mode==5 || run.mode==7 || run.mode==8)) && (game->scene_frames<2000 || (run.samples&7)!=7 ||
        rd_u8(RECORDER_MODE)!=0 || rd_u8(POSTFLIGHT_FAILURE_INPUT)!=0x11 ||
        rd_u32(STAGE_CALLBACK)!=0xc10dae))) {
        fprintf(stderr,"Mode %u failed: returned=%d captures=%u scene=%u postflight=%u\n",
            run.mode,run.returned,run.captures,game->scene_frames,game->postflight_callbacks);goto done;
    }
    if(run.mode==2)
        printf("{\"stream_wrap\":true,\"wraps\":%u,\"streams\":%u,\"returned\":true}\n",run.stream_wraps,run.streams);
    if(run.smoothing_cancel && (run.cancel_phase!=2 || run.cancelled!=3 ||
       !run.returned || game->scene_frames<30 || rd_u8(MODE_SELECT) || rd_u32(STAGE_CALLBACK)!=0xc0fcb4)) {
        fprintf(stderr,"Smoothing cancel failed: phase=%u continuation=%u returned=%d scene=%u\n",
                run.cancel_phase,run.cancelled,run.returned,game->scene_frames);goto done;
    }
    if(run.smoothing_cancel)
        printf("{\"smoothing_cancel\":true,\"scene_frames\":%u,\"continued\":true,\"returned\":true}\n",game->scene_frames);
    if(run.weapon && !run.hit_probe) {
        const unsigned consumed=(uint16_t)(run.ammo-run.weapon_ammo);
        const unsigned gun_shots=(uint16_t)(run.weapon_counters[0]-run.counters[0]);
        const unsigned first=(uint16_t)(run.weapon_counters[1]-run.counters[1]);
        const unsigned second=(uint16_t)(run.weapon_counters[2]-run.counters[2]);
        const unsigned stock=run.weapon_stock;
        if(!run.weapon_baseline || !run.weapon_checked || !game->postflight_callbacks ||
           rd_u8(CONTROL_RECORDS+95)!=run.stock || rd_u16(CONTROL_RECORDS+96)!=run.ammo ||
           (run.weapon==3?
           (!consumed || consumed!=gun_shots || first || second || stock!=run.stock):
           (!run.launched || !run.removed || consumed || gun_shots ||
            first!=(run.weapon==1?2u:0u) || second!=(run.weapon==2?2u:0u) ||
            stock!=(unsigned)(run.stock-(run.weapon==1?2u:32u))))) {
            fprintf(stderr,"Weapon %u failed: stock=%u ammo=%u shots=%u/%u/%u launched=%u removed=%u\n",
                run.weapon,stock,consumed,gun_shots,first,second,run.launched,run.removed);goto done;
        }
    }
    if(run.flight) {
        /* In mode five this input leaves the player stationary while the
         * other aircraft fly. Require observed aircraft motion in that case. */
        if(!run.flight_baseline || (run.mode==5?!(run.aircraft_moved&~1u):!run.moved) || game->scene_frames<5000 ||
           (run.flight==2 && (rd_u8(RECORDER_MODE)!=0 || (run.samples&7)!=7)) ||
           (run.flight==1 && (!run.spawns || !run.zone_exits || !(run.npc_missiles&(1u<<13))))) {
            fprintf(stderr,"Flight failed: baseline=%d moved=%d samples=%X returned=%d recorder=%u spawns=%X exits=%X missiles=%X scene=%u position=%u,%u,%u initial=%u,%u,%u\n",
                run.flight_baseline,run.moved,run.samples,run.returned,rd_u8(RECORDER_MODE),
                run.spawns,run.zone_exits,run.npc_missiles,game->scene_frames,
                rd_u32(CONTROL_RECORDS+20),rd_u32(CONTROL_RECORDS+24),rd_u32(CONTROL_RECORDS+28),
                run.initial_position[0],run.initial_position[1],run.initial_position[2]);goto done;
        }
        printf("{\"regions\":true,\"spawned_records\":%u,\"zone_exits\":%u,\"npc_missiles\":%u,\"aircraft_moved\":%u,\"scene_frames\":%u,\"gun_hits\":%u,\"infrared_hits\":%u,\"radar_hits\":%u}\n",
            run.spawns,run.zone_exits,run.npc_missiles,run.aircraft_moved,game->scene_frames,
            run.hit_counts[0],run.hit_counts[1],run.hit_counts[2]);
    }
    if(run.gun_approach) {
        printf("{\"gun_samples\":%u}\n",run.gun_bodies);
        if(run.gun_bodies!=32) {
            fputs("Gun approach did not capture 32 active-projectile bodies\n",stderr);goto done;
        }
    }
    if(run.hit_probe && !run.gun_approach && (!run.hit_counts[run.weapon==3?0:run.weapon] || run.hit_captured!=1)) {
        fprintf(stderr,"No normal-input selected weapon hit: gun=%u infrared=%u radar=%u target=%u launched=%u removed=%u\n",
            run.hit_counts[0],run.hit_counts[1],run.hit_counts[2],rd_u16(TARGET_RECORD),run.launched,run.removed);goto done;
    }
    if(run.kill_probe) {
        printf("{\"missile_kill\":true,\"weapon\":%u,\"started\":%s,\"accounted\":%s,\"inactive\":%s,\"record\":%u,\"expiry_bodies\":%u,\"enemy_expiries_before\":%u,\"enemy_expiries_after\":%u}\n",
            run.weapon,
            run.kill_started?"true":"false",run.kill_accounted?"true":"false",run.kill_inactive?"true":"false",
            run.kill_record,run.kill_bodies,run.kill_baseline,run.kill_count);
        if(!run.kill_accounted || !run.kill_inactive) {
            fputs("Missile destruction did not complete accounting/inactivation\n",stderr);goto done;
        }
    }
    if(run.outcome) {
        const unsigned losses=(uint16_t)(rd_u16(rd_u32(MODE_TABLE)+0x10)-run.initial_losses);
        if(!run.returned || !run.failure_seen || run.reset_states!=15 ||
           game->postflight_callbacks<3 || rd_u8(MODE_SELECT) ||
           rd_u32(STAGE_CALLBACK)!=0xc0fcb4 || losses!=3) {
            fprintf(stderr,"Natural outcome failed: returned=%d failure=%u resets=%X callbacks=%u stage=%06X mode=%u losses=%u\n",
                run.returned,run.failure_seen,run.reset_states,game->postflight_callbacks,
                rd_u32(STAGE_CALLBACK),rd_u8(MODE_SELECT),losses);goto done;
        }
        printf("{\"natural_outcome\":true,\"mode\":6,\"returned\":true,\"failure_seen\":true,\"reset_states\":%u,\"transitions\":%u,\"scene_frames\":%u,\"tick\":%u,\"aircraft_losses\":%u}\n",
            run.reset_states,run.outcome_transitions,game->scene_frames,game->ticks,losses);
        /* Continue through the returned menu using only its normal keys.
         * Keep the same runtime and log; reset only validation sampler state. */
        const unsigned return_tick=game->ticks,scene_frames=game->scene_frames;
        run.mode=125;run.outcome=0;run.flight=0;run.entered=0;
        run.stage_count=0;run.entry_count=0;run.streams=0;run.samples=0;
        const unsigned restart_times[]={1000,2500,4000};
        const int restart_keys[]={52,13,13};
        while(game->ticks<return_tick+5500) {
            for(unsigned i=0;i<3;++i) {
                if(game->ticks==return_tick+restart_times[i]) native_frontend_event(game,restart_keys[i],1);
                if(game->ticks==return_tick+restart_times[i]+2) native_frontend_event(game,restart_keys[i],0);
            }
            native_frontend_tick(game);
        }
        if(!run.entered || rd_u8(MODE_SELECT)!=125 || rd_u32(STAGE_CALLBACK)!=0xc10dae ||
           game->scene_frames<scene_frames+30) {
            fprintf(stderr,"Free Flight after natural failure did not start: mode=%u stage=%06X scene=%u/%u\n",
                rd_u8(MODE_SELECT),rd_u32(STAGE_CALLBACK),game->scene_frames,scene_frames);goto done;
        }
        printf("{\"outcome_restart\":true,\"mode\":125,\"scene_frames\":%u}\n",game->scene_frames-scene_frames);
    }
    result=0;
done:
    free(run.hit_before);
    if(game) {native_frontend_close(game);free(game);}return result;
}
