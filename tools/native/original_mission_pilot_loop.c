/* External original-runner diagnostic only. Ordinary keyboard inputs feed the
 * original IRQ queue. Shared MissionPilot reads original RAM; never writes it.
 * Build by replacing loop_input.c, not by linking this into fa18_native. */
#define fa18_loop_iteration original_loop_iteration
#define fa18_loop_finish original_loop_finish
#define held original_held
#include "../../port/recomp/loop_input.c"
#undef held
#undef fa18_loop_finish
#undef fa18_loop_iteration
#include "../../port/amiga/host_keys.h"
#include "mission_pilot.h"

static FILE *pilot_record;
static NativeFrontend observation;
static MissionPilot pilot;
static unsigned started_menu, completed, escape_tick, menu_phase;
static unsigned releases[5];
static unsigned location_tick;
static unsigned airborne,gear_raised,gear_lowered;
static uint32_t previous_stage;
static unsigned controller_ticks_per_update=4;
static unsigned mission_mode=3,weapon_initial_pressed,weapon_initial_released;
static unsigned repeat_steering,repeated_steering_events;
static long prefix_end=8038;

void native_frontend_event(NativeFrontend *game,int code,int down) {
    (void)game;
    const int raw=amiga_host_legacy_raw_key(code);
    if(raw<0 || !pilot_record) {fprintf(stderr,"Original pilot unsupported key %d\n",code);exit(2);}
    /* Same physical keyboard API as the original runner's replay delivery. */
    fa18_machine_key(fa18_machine,raw,down);
    fprintf(pilot_record,"%ld %ld K %d %d\n",iteration+1,frame,raw,down);
    fflush(pilot_record);
}
#include "mission_pilot.c"

static void release_once(unsigned stamp,unsigned *released,int key) {
    if(stamp && stamp!=*released && observation.ticks>=stamp+2) {
        mission_pilot_event(&pilot,&observation,key,0);*released=stamp;
    }
}
static void menu_keys(unsigned relative,unsigned at,int key) {
    /* One ordinary press/release, even if an expensive original update skips
     * the exact PAL number. These are validation pilot choices, not game timers. */
    static unsigned pressed[5],released[5];
    unsigned slot=at==100?0:at==1600?1:at==3600?2:at==5100?3:4;
    if(relative>=at && !pressed[slot]) {
        mission_pilot_event(&pilot,&observation,key,1);pressed[slot]=observation.ticks+1;
    }
    if(pressed[slot] && observation.ticks+1>=pressed[slot]+2 && !released[slot]) {
        mission_pilot_event(&pilot,&observation,key,0);released[slot]=1;
    }
}
void fa18_loop_iteration(void) {
    if(!pilot_record) {
        const char *path=getenv("FA18_ORIGINAL_PILOT_INPUT");
        const char *log=getenv("FA18_ORIGINAL_PILOT_KEYS");
        const char *mode_text=getenv("FA18_ORIGINAL_PILOT_MODE");
        repeat_steering=getenv("FA18_ORIGINAL_PILOT_REPEAT_STEERING")!=NULL;
        if(mode_text) {
            char *end;unsigned long value=strtoul(mode_text,&end,10);
            if(*end || (value!=3 && value!=4)) {fputs("Original pilot mode requires 3 or 4\n",stderr);exit(2);}
            mission_mode=(unsigned)value;
        }
        if(mission_mode==4) {
            const char *prefix=getenv("FA18_ORIGINAL_PILOT_PREFIX_END");char *end;
            if(!prefix) {fputs("Escort requires the independently verified mission-three prefix\n",stderr);exit(2);}
            prefix_end=strtol(prefix,&end,10);
            if(*end || prefix_end<=8038 || prefix_end>=65000) {fputs("Invalid original escort prefix end\n",stderr);exit(2);}
        }
        if(!path || !log || !events || recorded_end!=prefix_end) {
            fputs("Original pilot requires its verified input prefix and two output paths\n",stderr);exit(2);
        }
        const char *scale=getenv("FA18_ORIGINAL_PILOT_TICKS_PER_UPDATE");
        if(scale) {
            char *end;
            unsigned long value=strtoul(scale,&end,10);
            if(*end || value<1 || value>16) {
                fputs("Original pilot input timing requires 1..16 ticks per update\n",stderr);exit(2);
            }
            controller_ticks_per_update=(unsigned)value;
        }
        fprintf(stderr,"Original validation controller ticks per update: %u\n",controller_ticks_per_update);
        pilot_record=fopen(path,"w");pilot.keys=fopen(log,"w");
        if(!pilot_record || !pilot.keys) {perror("original pilot output");exit(2);}
        fputs("FA18_LOOP_INPUT_V1\n",pilot_record);fputs("E9K_INPUT_V1\n",pilot.keys);
        for(int i=0;i<event_count;++i) {
            if(events[i].kind!='K') {fputs("Qualification has unexpected non-key input\n",stderr);exit(2);}
            fprintf(pilot_record,"%ld 0 K %d %d\n",events[i].iteration,events[i].a,events[i].b);
        }
        pilot.mode=mission_mode;pilot.manage_gear=1;
        if(mission_mode==4) pilot.escort_flight=pilot.complete_flight=pilot.campaign_flight=1;
        if(getenv("FA18_ORIGINAL_PILOT_PATROL")) {
            /* Test input: use the existing level-turn/return controller to
             * approach the normally spawned aircraft without firing. */
            pilot.patrol_flight=pilot.complete_flight=pilot.campaign_flight=1;
        }
        /* Diagnostic end bound only; the source game state is untouched. */
        recorded_end=mission_mode==4?65000:40000;
    }
    observation.ticks=(unsigned)frame;
    observation.scene_frames=rd_u16(UPDATE_TICK);
    const uint32_t stage=rd_u32(STAGE_CALLBACK);
    if(stage!=previous_stage) {
        fprintf(stderr,"Original pilot loop %ld PAL %ld stage %06X mode %u phase %u\n",
            iteration+1,frame,stage,rd_u8(MODE_SELECT),rd_u8(PLAYER_PHASE));
        fflush(stderr);previous_stage=stage;
    }
    if(pilot.started && stage==0xc11788 && menu_phase==4) {
        fprintf(stderr,"Original crash/reset outcome at loop %ld PAL %ld XYZ %d/%d/%d; airborne %u gear up/down %u/%u\n",
            iteration+1,frame,rd_s32(CONTROL_RECORDS+20)/256,rd_s32(CONTROL_RECORDS+24)/256,
            rd_s32(CONTROL_RECORDS+28)/256,airborne,gear_raised,gear_lowered);
        recorded_end=iteration+1;menu_phase=10;
    }
    if(pilot.started && mission_mode==4 && menu_phase==4 &&
       (rd_u8(PLAYER_PHASE)==0xfe || rd_u8(PLAYER_PHASE)==2)) {
        fprintf(stderr,"Original mission failure outcome at loop %ld PAL %ld phase %u\n",
            iteration+1,frame,rd_u8(PLAYER_PHASE));
        recorded_end=iteration+1;menu_phase=10;
    }
    if(iteration>=prefix_end && !menu_phase) {
        if(mission_mode==4) {
            const gaddr log=rd_u32(MODE_TABLE);
            if(rd_u8(MODE_SELECT) || stage!=0xc0fcb4 || !rd_u16(log) ||
               !rd_u8(log+21) || rd_u16(log+56)!=1) {
                fputs("Original escort prefix did not earn the actual mission-three result/menu\n",stderr);exit(2);
            }
            started_menu=observation.ticks;menu_phase=4;
            fprintf(stderr,"Original earned escort prefix: PAL %u loop %ld grade %u count %u\n",
                started_menu,iteration,rd_u8(log+21),rd_u16(log+56));
        } else {
            mission_pilot_event(&pilot,&observation,304,1);escape_tick=observation.ticks;
            menu_phase=1;
        }
    }
    if(menu_phase==1 && observation.ticks>=escape_tick+2) {
        mission_pilot_event(&pilot,&observation,27,1);escape_tick=observation.ticks;menu_phase=2;
    }
    if(menu_phase==2 && observation.ticks>=escape_tick+2) {
        mission_pilot_event(&pilot,&observation,27,0);mission_pilot_event(&pilot,&observation,304,0);menu_phase=3;
    }
    if(menu_phase==3 && !rd_u8(MODE_SELECT) && stage==0xc0fcb4) {
        started_menu=observation.ticks;menu_phase=4;
        fprintf(stderr,"Original pilot qualified word %u; entering mission menu\n",rd_u16(rd_u32(MODE_TABLE)));
    }
    if(menu_phase==4) {
        const unsigned relative=observation.ticks-started_menu;
        menu_keys(relative,100,54);menu_keys(relative,1600,282+(int)mission_mode-3);
        menu_keys(relative,3600,13);menu_keys(relative,5100,13);
        if(stage==0xc10ae6 && !location_tick) {
            mission_pilot_event(&pilot,&observation,50,1);location_tick=observation.ticks+1;
        }
        if(location_tick && observation.ticks+1>=location_tick+2 && !releases[2]) {
            mission_pilot_event(&pilot,&observation,50,0);releases[2]=1;
        }
        if(stage==0xc10dae || pilot.started) {
            /* The validation controller's takeoff choices use nominal
             * host ticks per physics update. Actual PAL time differs in the
             * original renderer; no clock is written to the source game. */
            observation.ticks=40000u+controller_ticks_per_update*rd_u16(UPDATE_TICK);
            release_once(pilot.target_press,&releases[0],116);
            release_once(pilot.gear_key_tick,&releases[1],103);
            if(mission_mode==4) {
                release_once(pilot.weapon_press,&releases[3],13);
                release_once(pilot.defense_tick,&releases[4],pilot.defense_key);
                if(pilot.started) for(unsigned i=0;i<2;++i) {
                    const unsigned bit=1u<<i,when=pilot.started+700+20*i;
                    if(observation.ticks>=when && !(weapon_initial_pressed&bit)) {
                        mission_pilot_event(&pilot,&observation,13,1);weapon_initial_pressed|=bit;
                    }
                    if(observation.ticks>=when+2 && (weapon_initial_pressed&bit) && !(weapon_initial_released&bit)) {
                        mission_pilot_event(&pilot,&observation,13,0);weapon_initial_released|=bit;
                    }
                }
            }
            const int previous_steering[]={pilot.rudder,pilot.pitch,pilot.roll};
            mission_pilot_tick(&pilot,&observation);
            if(repeat_steering && stage==0xc10dae) {
                /* Diagnostic keyboard input only. A retained make event is
                 * repeated through the real IRQ queue, not applied to RAM.
                 * Do not duplicate new choices or repeat toggle commands. */
                const int steering[]={pilot.rudder,pilot.pitch,pilot.roll};
                for(unsigned i=0;i<3;++i) if(steering[i] && steering[i]==previous_steering[i]) {
                    mission_pilot_event(&pilot,&observation,steering[i],1);
                    ++repeated_steering_events;
                }
            }
            observation.ticks=(unsigned)frame;
        }
        if(pilot.started && !(rd_u16(CONTROL_RECORDS+2)&0x80)) {
            airborne=1;
            if(rd_u8(COMMAND_BLOCK_FLAGS)&0x80) gear_raised=1;
            else if(gear_raised) gear_lowered=1;
        }
        if(pilot.started && (rd_u8(CONTROL_RECORDS+32)&2)) {
            fprintf(stderr,"Original player destroyed at loop %ld PAL %ld XYZ %d/%d/%d; airborne %u gear up/down %u/%u\n",
                iteration+1,frame,rd_s32(CONTROL_RECORDS+20)/256,rd_s32(CONTROL_RECORDS+24)/256,
                rd_s32(CONTROL_RECORDS+28)/256,airborne,gear_raised,gear_lowered);
            recorded_end=iteration+1;menu_phase=10;
        }
        if(pilot.started && rd_u16(rd_u32(MODE_TABLE)+56)>pilot.completions) {
            const int held_keys[]={pilot.rudder,pilot.pitch,pilot.roll,pilot.throttle,pilot.fire,pilot.hook};
            for(unsigned i=0;i<sizeof held_keys/sizeof held_keys[0];++i)
                if(held_keys[i]) mission_pilot_event(&pilot,&observation,held_keys[i],0);
            completed=observation.ticks;menu_phase=5;
            fprintf(stderr,"Original mission %u result: PAL %u phase %u grade %u count %u gear %u speed %u\n",
                mission_mode,completed,rd_u8(PLAYER_PHASE),rd_u8(rd_u32(MODE_TABLE)+18+mission_mode),
                rd_u16(rd_u32(MODE_TABLE)+56),rd_u8(COMMAND_BLOCK_FLAGS),rd_u16(CONTROL_RECORDS+110));
        }
    }
    if(menu_phase==5 && rd_u8(PLAYER_PHASE)==4 && rd_s8(MESSAGE_STATE_C)<0) {
        mission_pilot_event(&pilot,&observation,304,1);escape_tick=observation.ticks;menu_phase=6;
    }
    if(menu_phase==6 && observation.ticks>=escape_tick+2) {
        mission_pilot_event(&pilot,&observation,27,1);escape_tick=observation.ticks;menu_phase=7;
    }
    if(menu_phase==7 && observation.ticks>=escape_tick+2) {
        mission_pilot_event(&pilot,&observation,27,0);mission_pilot_event(&pilot,&observation,304,0);menu_phase=8;
    }
    if(menu_phase==8 && !rd_u8(MODE_SELECT) && stage==0xc0fcb4) {
        fprintf(stderr,"Original mission %u complete and menu returned at PAL %ld loop %ld\n",mission_mode,frame,iteration+1);
        recorded_end=iteration+1;menu_phase=9;
    }
    original_loop_iteration();
}
int fa18_loop_finish(void) {
    int ok=original_loop_finish();
    if(pilot_record) {
        fprintf(stderr,"Original validation repeated steering events: %u\n",repeated_steering_events);
        fprintf(pilot_record,"end %ld %ld\n",iteration,frame);
        if(fclose(pilot_record)) ok=0;
        if(fclose(pilot.keys)) ok=0;
    }
    if(menu_phase!=9) {
        fprintf(stderr,"Original mission pilot incomplete: phase %u menu-step %u objective %u completed PAL %u\n",
            rd_u8(PLAYER_PHASE),menu_phase,pilot.objective,completed);
        ok=0;
    }
    return ok;
}
