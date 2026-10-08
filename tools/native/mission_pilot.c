/* Validation pilot: all game-state accesses are read-only; only host keys
 * reach the existing runtime. Flight choices below are test input, not AI. */
#include "mission_pilot.h"
#include "globals.h"
#include <math.h>
#include <stdlib.h>

void mission_pilot_event(MissionPilot *pilot,NativeFrontend *game,int code,int down) {
    native_frontend_event(game,code,down);
    fprintf(pilot->keys,"F %u K %d 0 0 %d\n",game->ticks,code,down);
}
static void held(MissionPilot *pilot,NativeFrontend *game,int *current,int code) {
    if(*current==code) return;
    if(*current) mission_pilot_event(pilot,game,*current,0);
    if(code) mission_pilot_event(pilot,game,code,1);
    *current=code;
}
static double angle_delta(double a,double b) {
    double result=a-b;
    while(result>3.141592653589793) result-=6.283185307179586;
    while(result< -3.141592653589793) result+=6.283185307179586;
    return result;
}
static double clamp(double value,double low,double high) {
    return value<low?low:value>high?high:value;
}
static void command_throttle(MissionPilot *pilot,NativeFrontend *game,int throttle) {
    /* C1B35A and C13D84 can settle F10 at phase 120 before afterburner.
     * Reapply the normal plus key after the source releases throttle input. */
    if(throttle==291 && rd_s8(CONTROL_RECORDS+43)>=120 && !(rd_u16(CONTROL_RECORDS+2)&8))
        throttle=61;
    if(throttle==61 && pilot->throttle==61 && !(rd_u8(PLAYER_STICK)&3))
        mission_pilot_event(pilot,game,61,1);
    else held(pilot,game,&pilot->throttle,throttle);
}
/* Validation flight only: follow the stolen aircraft for C0A002's proximity
 * countdown. Formation input is separate from the subsequent combat/landing
 * pilot; every control remains an ordinary key. */
static int follow_stolen_aircraft(MissionPilot *pilot,NativeFrontend *game) {
    if(!pilot->force_return || pilot->formation_done || pilot->objective) return 0;
    pilot->target=4;
    if(rd_s16(SCENE_DISPATCH_GATE)<0) {
        pilot->formation_done=1;
        held(pilot,game,&pilot->throttle,61);
        pilot->previous_x=pilot->previous_y=0;
        printf("{\"formation_complete\":true,\"tick\":%u}\n",game->ticks);
        return 0;
    }
    const gaddr target=CONTROL_RECORDS+512*pilot->target;
    double delta[3],velocity[3];
    for(unsigned i=0;i<3;++i) {
        delta[i]=(rd_s32(target+20+4*i)-rd_s32(CONTROL_RECORDS+20+4*i))/256.0;
        velocity[i]=rd_s32(target+62+4*i)/256.0;
    }
    const double range=hypot(delta[0],delta[2]);
    const double speed=rd_s16(CONTROL_RECORDS+110)/64.0;
    const double target_speed=rd_s16(target+110)/64.0;
    const double horizontal=hypot(velocity[0],velocity[2]);
    const double time=clamp(range/fmax(speed,70),200,1000);
    const double along=horizontal? (delta[0]*velocity[0]+delta[2]*velocity[2])/horizontal:range;
    const double wanted_speed=target_speed+clamp((along-250)/80,-30,45);
    const double yaw=rd_u16(CONTROL_RECORDS+104)*6.283185307179586/28800;
    const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
    const double wanted_yaw=atan2(-(delta[0]+velocity[0]*time),delta[2]+velocity[2]*time);
    const double height=rd_s8(SCENE_DISPATCH_LIMIT)>=3?200:400;
    const double direction[3]={-sin(wanted_yaw),clamp((delta[1]+height)/6000,-0.08,0.08),cos(wanted_yaw)};
    double local[3]={0};
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
        local[i]+=direction[j]*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
    const double pitch=atan2(local[1],hypot(local[0],local[2]));
    if(!pilot->formation_started) {
        pilot->formation_started=1;
        pilot->previous_x=yaw;pilot->previous_y=pitch;
    }
    const double yaw_control=angle_delta(wanted_yaw,yaw)-10*angle_delta(yaw,pilot->previous_x);
    const double pitch_control=pitch+4*angle_delta(pitch,pilot->previous_y);
    pilot->previous_x=yaw;pilot->previous_y=pitch;
    int throttle=range>30000 || speed<wanted_speed-1?291:speed>wanted_speed+1?285:288;
    command_throttle(pilot,game,throttle);
    held(pilot,game,&pilot->roll,bank>0.02?275:bank< -0.02?276:0);
    held(pilot,game,&pilot->rudder,yaw_control>0.006?46:yaw_control< -0.006?44:0);
    held(pilot,game,&pilot->pitch,pitch_control>0.006?274:pitch_control< -0.006?273:0);
    held(pilot,game,&pilot->fire,0);
    return 1;
}
/* Validation input for the last patrol aircraft: aim the gun using the same
 * read-only position/velocity and bullet drop observations as gun_discovery. */
static int final_patrol_gun(MissionPilot *pilot,NativeFrontend *game) {
    unsigned selected=0;
    double nearest=1e30;
    for(unsigned slot=4;slot<=14;slot+=2) {
        const gaddr record=CONTROL_RECORDS+512*slot;
        if((rd_u16(record)&0x1648u)!=0x1040u || (rd_u8(record+98)&0xf0u)!=0x10u ||
           (rd_u16(record+2)&0x1000u)) continue;
        double square=0;
        for(unsigned i=0;i<3;++i) {
            const double d=(rd_s32(record+20+4*i)-rd_s32(CONTROL_RECORDS+20+4*i))/256.0;
            square+=d*d;
        }
        if(square<nearest) {nearest=square;selected=slot;}
    }
    if(!selected) {held(pilot,game,&pilot->fire,0);return 1;}
    const gaddr target=CONTROL_RECORDS+512*selected;
    const double speed=198+(rd_s16(CONTROL_RECORDS+108)>>6);
    const double range=sqrt(nearest),time=fmin(range/speed,20);
    double local[3]={0};
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j) {
        const double point=(rd_s32(target+20+4*j)-rd_s32(CONTROL_RECORDS+20+4*j))/256.0+
            rd_s32(target+62+4*j)/256.0*time;
        local[i]+=point*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384.0;
    }
    for(unsigned i=0;i<3;++i)
        local[i]+=0.5*time*(time+1)*rd_s16(CONTROL_RECORDS+152+2*i)/16384.0;
    const double x=atan2(local[0],local[2]);
    const double y=atan2(local[1]-local[2]*67.0/1024,hypot(local[0],local[2]));
    if(pilot->target!=selected || !pilot->combat_started) {
        pilot->previous_x=x;pilot->previous_y=y;pilot->combat_started=1;
    }
    pilot->target=selected;
    const double rudder=x+4*angle_delta(x,pilot->previous_x),elevator=y+4*angle_delta(y,pilot->previous_y);
    pilot->previous_x=x;pilot->previous_y=y;
    const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
    const double target_speed=rd_s16(target+110)/64.0;
    const double wanted=target_speed+clamp((range-600)/100,-20,25);
    const double actual=rd_s16(CONTROL_RECORDS+110)/64.0;
    command_throttle(pilot,game,actual<wanted-1?291:actual>wanted+1?285:288);
    held(pilot,game,&pilot->roll,bank>0.02?275:bank< -0.02?276:0);
    held(pilot,game,&pilot->rudder,rudder>0.002?44:rudder< -0.002?46:0);
    held(pilot,game,&pilot->pitch,elevator>0.002?274:elevator< -0.002?273:0);
    const uint8_t weapon=rd_u8(CONTROL_RECORDS+99)&0xf0u;
    if(weapon==0x30u && range<5000 && local[2]>0 && fabs(x)<0.05 && fabs(y)<0.08 &&
       game->ticks>pilot->final_shot_tick+300) {
        pilot->final_shot_tick=game->ticks;
        printf("{\"patrol_finishing_shot\":true,\"tick\":%u,\"target\":%u,\"range\":%.3f}\n",
            game->ticks,selected,range);
    }
    const int gun=weapon==0x10u && (rd_u8(target+60)&15u)<2 && local[2]>0 &&
        local[2]<speed*20 && fabs(x)<0.03 && fabs(y)<0.03;
    held(pilot,game,&pilot->fire,gun || (weapon==0x30u && game->ticks==pilot->final_shot_tick)?32:0);
    return 1;
}
/* Mission validation only: pursue the remaining active enemies and launch
 * radar missiles with normal controls. No hit, collision or outcome state
 * is written. */
static int combat_flight(MissionPilot *pilot,NativeFrontend *game) {
    if(!pilot->complete_flight || (pilot->mode==5 && !pilot->formation_done) || pilot->objective ||
       rd_u8(PLAYER_PHASE)==0xff || rd_u8(PLAYER_PHASE)==1) return 0;
    if(pilot->final_sequence && rd_u8(SCENE_DISPATCH_AUX)>=3) {
        if(!pilot->final_breakaway_tick) {
            pilot->final_breakaway_tick=game->ticks;
            pilot->final_breakaway_yaw=rd_u16(CONTROL_RECORDS+104)*6.283185307179586/28800;
            printf("{\"patrol_breakaway\":true,\"tick\":%u}\n",game->ticks);
        }
        if(game->ticks<pilot->final_breakaway_tick+1200) {
            /* Validation input: gain separation/height before the last pass.
             * Nothing here writes the aircraft's motion or mission state. */
            const double yaw=rd_u16(CONTROL_RECORDS+104)*6.283185307179586/28800;
            const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
            const double direction[3]={-sin(pilot->final_breakaway_yaw),0.15,cos(pilot->final_breakaway_yaw)};
            double local[3]={0};
            for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
                local[i]+=direction[j]*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
            const double pitch=atan2(local[1],hypot(local[0],local[2]));
            const double rudder=angle_delta(pilot->final_breakaway_yaw,yaw)-10*angle_delta(yaw,pilot->previous_x);
            const double elevator=pitch+4*angle_delta(pitch,pilot->previous_y);
            pilot->previous_x=yaw;pilot->previous_y=pitch;
            command_throttle(pilot,game,291);
            held(pilot,game,&pilot->roll,bank>0.02?275:bank< -0.02?276:0);
            held(pilot,game,&pilot->rudder,rudder>0.006?46:rudder< -0.006?44:0);
            held(pilot,game,&pilot->pitch,elevator>0.006?274:elevator< -0.006?273:0);
            held(pilot,game,&pilot->fire,0);
            return 1;
        }
        return final_patrol_gun(pilot,game);
    }
    unsigned selected=0;
    double target_distance_squared=0;
    for(unsigned slot=pilot->final_flight?4u:8u;slot<=(pilot->final_flight?14u:pilot->escort_flight?12u:10u);slot+=2) {
        const gaddr record=CONTROL_RECORDS+512*slot;
        if((rd_u16(record)&0x1648u)!=0x1040u) continue;
        if(pilot->final_flight && (rd_u8(record+98)&0xf0u)!=0x10u) continue;
        double square=0;
        for(unsigned i=0;i<3;++i) {
            const double d=(rd_s32(record+20+4*i)-rd_s32(CONTROL_RECORDS+20+4*i))/256.0;
            square+=d*d;
        }
        if(slot==pilot->target && (!pilot->final_flight || square<20000.0*20000.0)) {
            target_distance_squared=square;selected=slot;break;
        }
        if(!selected || ((pilot->mode==4 || pilot->final_flight) && square<target_distance_squared)) {
            target_distance_squared=square;selected=slot;
        }
    }
    if(!selected) {held(pilot,game,&pilot->fire,0);return 1;}
    const gaddr target=CONTROL_RECORDS+512*selected;
    const double range=sqrt(target_distance_squared),speed=rd_s16(CONTROL_RECORDS+110)/64.0;
    /* Input aim for the regional closing pass uses a shorter intercept lead;
     * the actual projectile continues to use original motion/tracking rules. */
    const int final_heat=pilot->final_flight && (rd_u8(CONTROL_RECORDS+99)&0xf0u)==0x30u;
    const double time=clamp(range/(final_heat || (pilot->escort_flight && selected==12)?198+speed:fmax(speed,70)),0,400);
    double delta[3],local[3]={0};
    for(unsigned i=0;i<3;++i)
        delta[i]=(rd_s32(target+20+4*i)-rd_s32(CONTROL_RECORDS+20+4*i))/256.0+
            rd_s32(target+62+4*i)/256.0*time;
    delta[1]+=400;
    const double wanted_yaw=atan2(-delta[0],delta[2]);
    const double yaw=rd_u16(CONTROL_RECORDS+104)*6.283185307179586/28800;
    const double yaw_error=angle_delta(wanted_yaw,yaw);
    const double horizontal=hypot(delta[0],delta[2]);
    /* Mode-four input keeps pitch relative to the current heading while
     * turning toward higher enemy aircraft; the accepted mode-five keys
     * retain their existing heading choice. */
    const double pitch_heading=pilot->mode==4 || pilot->final_flight?yaw:wanted_yaw;
    const double pitch_slope=pilot->mode==4 || pilot->final_flight?
        clamp(delta[1]/6000-rd_s32(CONTROL_RECORDS+66)/256.0/fmax(speed,30),-0.12,0.12):
        clamp(delta[1]/fmax(horizontal,100),-0.05,0.05);
    const double direction[3]={-sin(pitch_heading),fabs(yaw_error)>0.3?0:
        pitch_slope,cos(pitch_heading)};
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
        local[i]+=direction[j]*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
    const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
    const double x=atan2(local[0],local[2]),y=atan2(local[1],hypot(local[0],local[2]));
    if(!pilot->combat_started || pilot->target!=selected) {
        pilot->combat_started=1;pilot->previous_x=yaw;pilot->previous_y=y;
    }
    pilot->target=selected;
    const double yaw_control=angle_delta(wanted_yaw,yaw)-10*angle_delta(yaw,pilot->previous_x);
    const double pitch_control=y+4*angle_delta(y,pilot->previous_y);
    pilot->previous_x=yaw;pilot->previous_y=y;
    const double wanted_speed=rd_s16(target+110)/64.0+clamp((range-1800)/100,-25,25);
    command_throttle(pilot,game,speed<wanted_speed-1?291:speed>wanted_speed+1?285:288);
    held(pilot,game,&pilot->roll,bank>0.02?275:bank< -0.02?276:0);
    held(pilot,game,&pilot->rudder,yaw_control>0.006?46:yaw_control< -0.006?44:0);
    held(pilot,game,&pilot->pitch,pitch_control>0.006?274:pitch_control< -0.006?273:0);
    if((pilot->mode==4 || pilot->final_flight) && pilot->missile_target==selected && game->ticks>pilot->missile_tick+200) {
        int active=0;
        for(unsigned slot=1;slot<=3;++slot) {
            const gaddr projectile=CONTROL_RECORDS+512*slot;
            active|=(rd_u8(projectile+1)&0x48u)==0x48u && !rd_u8(projectile+94);
        }
        if(!active) pilot->missile_target=0;
    }
    /* Escort validation fires earlier on the closing pass. These are pilot
     * input choices; original launch/tracking/damage rules decide the result. */
    const int close_aim=pilot->mode==4 && !pilot->escort_flight;
    const double launch_range=close_aim?10000:20000;
    const uint8_t weapon=rd_u8(CONTROL_RECORDS+99)&0xf0u,stock=rd_u8(CONTROL_RECORDS+95);
    const int final_ready=!pilot->final_flight || (weapon==0x20 && (stock&0xf0u)) ||
        (weapon==0x30 && (stock&15u));
    const int launch=final_ready && rd_s16(SELECTED_RECORD)==(int)(selected*512) && range<launch_range &&
        (!(pilot->final_flight && weapon==0x30) || (rd_u8(SHOOT_CUE) && range<10000 && fabs(yaw_error)<0.1)) &&
        (!(pilot->escort_flight && selected==12) || rd_u8(SHOOT_CUE)) &&
        fabs(x)<(close_aim?0.2:0.6) && fabs(y)<(close_aim?0.15:0.3) && pilot->missile_target!=selected;
    if(launch) {
        pilot->missile_target=selected;pilot->missile_tick=game->ticks;
        if(getenv("FA18_MISSION_TRACE"))
            printf("{\"pilot_fire\":true,\"tick\":%u,\"target\":%u,\"range\":%.3f,\"yaw_error\":%.6f,\"stock\":%u,\"linked\":%u}\n",
                game->ticks,selected,range,yaw_error,rd_u8(CONTROL_RECORDS+95),rd_u8(CONTROL_RECORDS+56));
    }
    held(pilot,game,&pilot->fire,pilot->missile_target==selected &&
        game->ticks-pilot->missile_tick<(pilot->final_flight && weapon==0x30?1u:
            pilot->escort_flight || pilot->final_flight?12u:4u)?32:0);
    return 1;
}
/* Return flight input for the mode-five validation pilot. Keep heading
 * feedback in world coordinates across the combat/return handoff. */
static int return_flight(MissionPilot *pilot,NativeFrontend *game) {
    if(!pilot->complete_flight || (!pilot->objective &&
       rd_u8(PLAYER_PHASE)!=0xff && rd_u8(PLAYER_PHASE)!=1)) return 0;
    if((pilot->escort_flight || pilot->final_flight) && pilot->return_input_phase!=rd_u8(PLAYER_PHASE)) {
        /* The escort's result camera clears input while changing FF to one.
         * Release/repress ordinary controls when the player regains the view. */
        held(pilot,game,&pilot->rudder,0);held(pilot,game,&pilot->pitch,0);
        held(pilot,game,&pilot->roll,0);held(pilot,game,&pilot->throttle,0);
        pilot->return_input_phase=rd_u8(PLAYER_PHASE);
    }
    if(!pilot->objective) {
        printf("{\"objective\":true,\"tick\":%u,\"phase\":%u}\n",game->ticks,rd_u8(PLAYER_PHASE));
        pilot->objective=1;pilot->phase=1;
    }
    double point[3],position[3],local[3]={0};
    for(unsigned i=0;i<3;++i) {
        position[i]=rd_s32(CONTROL_RECORDS+20+4*i)/256.0;
        point[i]=pilot->home[i]-pilot->forward[i]*12000;
    }
    point[1]=pilot->home[1]+700;
    const double approach_distance=hypot(point[0]-position[0],point[2]-position[2]);
    if(pilot->phase==1 && approach_distance<1800 &&
       (!pilot->final_sequence || position[1]<pilot->home[1]+1400)) pilot->phase=2;
    if(pilot->phase>=2) {
        /* Pilot input: intercept the centerline and descend onto the wire
         * region before the starting pose, rather than flying past it. */
        const double before_home=(pilot->home[0]-position[0])*pilot->forward[0]+
            (pilot->home[2]-position[2])*pilot->forward[2];
        for(unsigned i=0;i<3;++i)
            point[i]=pilot->home[i]+pilot->forward[i]*(2000-before_home);
        point[1]=pilot->home[1]-200+fmax(before_home,0)*0.04;
    }
    const double wanted_yaw=atan2(-(point[0]-position[0]),point[2]-position[2]);
    const double yaw=rd_u16(CONTROL_RECORDS+104)*6.283185307179586/28800;
    const double yaw_error=angle_delta(wanted_yaw,yaw);
    const double speed=rd_s16(CONTROL_RECORDS+110)/64.0;
    const double vertical_speed=rd_s32(CONTROL_RECORDS+66)/256.0;
    /* Pitch follows current heading so a return turn cannot reverse its
     * feedback; observed vertical speed damps the selected height approach. */
    const double descent=pilot->final_sequence && pilot->phase==1?0.25:0.12;
    const double direction[3]={-sin(yaw),fabs(yaw_error)>0.3?0:
        clamp((point[1]-position[1])/3000-
              vertical_speed/fmax(speed,30),-descent,descent),cos(yaw)};
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
        local[i]+=direction[j]*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
    const double pitch=atan2(local[1],hypot(local[0],local[2]));
    const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
    if(!pilot->return_started) {
        pilot->return_started=1;pilot->previous_x=yaw;pilot->previous_y=pitch;
        printf("{\"return_start\":true,\"tick\":%u,\"pose\":%u,\"home\":[%.3f,%.3f,%.3f],"
               "\"forward\":[%.3f,%.3f,%.3f]}\n",game->ticks,rd_u8(SCENE_POSE_ENTRY),
               pilot->home[0],pilot->home[1],pilot->home[2],
               pilot->forward[0],pilot->forward[1],pilot->forward[2]);
    }
    const double yaw_control=yaw_error-10*angle_delta(yaw,pilot->previous_x);
    const double pitch_control=pitch+4*angle_delta(pitch,pilot->previous_y);
    pilot->previous_x=yaw;pilot->previous_y=pitch;
    /* F9 on the escort's long return keeps separation from the regional
     * fighter; the established F5 approach remains the landing input. */
    command_throttle(pilot,game,pilot->phase>=2 || approach_distance<20000?286:pilot->escort_flight?290:288);
    /* Original A/raw $20 invokes COMMAND_HOOK for the F/A-18 arrestor. */
    held(pilot,game,&pilot->hook,pilot->phase>=2 && !(rd_u16(CONTROL_RECORDS+2)&0x8000)?97:0);
    held(pilot,game,&pilot->roll,bank>0.02?275:bank< -0.02?276:0);
    held(pilot,game,&pilot->rudder,yaw_control>0.006?46:yaw_control< -0.006?44:0);
    held(pilot,game,&pilot->pitch,pilot->phase==3?0:pitch_control>0.006?274:pitch_control< -0.006?273:0);
    held(pilot,game,&pilot->fire,0);
    return 1;
}
void mission_pilot_tick(MissionPilot *pilot,NativeFrontend *game) {
    if(game->flight_timer_pending || game->scene_frames==pilot->scene) return;
    pilot->scene=game->scene_frames;
    const unsigned trace_interval=pilot->escort_flight && getenv("FA18_MISSION_WEAPON_TRACE")?100u:500u;
    if(getenv("FA18_MISSION_TRACE") && game->ticks/trace_interval!=pilot->trace) {
        pilot->trace=game->ticks/trace_interval;
        printf("{\"trace\":true,\"tick\":%u,\"phase\":%u,\"pilot_phase\":%u,\"gate\":%d,"
               "\"function_level\":%u,\"controls\":%u,\"thrust\":%d,\"fuel\":%u,"
               "\"admitted\":%u,\"aux\":%u,\"gun_hits\":%u,\"radar_hits\":%u,\"weapon\":%u,\"records\":[",
               game->ticks,rd_u8(PLAYER_PHASE),pilot->phase,rd_s16(SCENE_DISPATCH_GATE),
               rd_u8(FUNCTION_KEY_LEVEL),rd_u8(PLAYER_STICK),rd_s8(CONTROL_RECORDS+43),rd_u32(CONTROL_RECORDS+114),
               rd_u8(SCENE_DISPATCH_ADMITTED),rd_u8(SCENE_DISPATCH_AUX),
               rd_u16(rd_u32(MODE_TABLE)+60),rd_u16(rd_u32(MODE_TABLE)+68),rd_u8(CONTROL_RECORDS+99)&0xf0);
        for(unsigned slot=0;slot<=12;slot+=trace_interval==100?1u:2u) {
            gaddr record=CONTROL_RECORDS+512*slot;
            printf("%s{\"slot\":%u,\"flags\":%u,\"kind\":%u,\"contact\":%u,\"region\":%u,\"damage\":%u,"
                   "\"speed\":[%d,%d],\"position\":[%d,%d,%d],\"angles\":[%d,%d,%d],\"linked\":%u,\"lifetime\":%d}",
                   slot?",":"",slot,rd_u16(record),rd_u8(record+98),rd_u16(record+2),rd_u8(record+4),rd_u8(record+60),
                   rd_s16(record+108),rd_s16(record+110),rd_s32(record+20)/256,
                   rd_s32(record+24)/256,rd_s32(record+28)/256,
                   rd_s16(record+102),rd_s16(record+104),rd_s16(record+106),rd_u8(record+56),rd_s16(record+76));
        }
        puts("]}");fflush(stdout);
    }
    if(!pilot->started && rd_u32(STAGE_CALLBACK)==0xc10dae) {
        pilot->started=game->ticks;
        pilot->completions=rd_u16(rd_u32(MODE_TABLE)+56);
        pilot->grade=rd_u8(rd_u32(MODE_TABLE)+18+pilot->mode);
        for(unsigned i=0;i<3;++i) {
            pilot->home[i]=(double)rd_s32(CONTROL_RECORDS+20+4*i)/256;
            pilot->forward[i]=(double)rd_s16(CONTROL_RECORDS+150+6*i)/16384;
        }
    }
    if(!pilot->started) return;
    if(pilot->final_flight && !pilot->objective && game->ticks>pilot->started+750) {
        const uint8_t weapon=rd_u8(CONTROL_RECORDS+99)&0xf0u;
        const uint8_t stock=rd_u8(CONTROL_RECORDS+95);
        const gaddr target=CONTROL_RECORDS+512*pilot->target;
        const uint8_t wanted=pilot->final_sequence && rd_u8(SCENE_DISPATCH_AUX)>=3?
            ((rd_u8(target+60)&15u)>=2 && (stock&15u)?0x30u:0x10u):
            (stock&0xf0u)?0x20u:(stock&15u)?0x30u:0x10u;
        if(weapon!=wanted && game->ticks>pilot->weapon_press+20) {
            held(pilot,game,&pilot->fire,0);
            mission_pilot_event(pilot,game,13,1);pilot->weapon_press=game->ticks;
            pilot->missile_target=0;
        }
    }
    if(pilot->phase==2 && (rd_u8(CONTROL_RECORDS+3)&0x80)) pilot->phase=3;
    if(pilot->phase==3) {
        held(pilot,game,&pilot->pitch,0);held(pilot,game,&pilot->roll,0);
        if(rd_u8(CONTROL_RECORDS+4)&(rd_u8(SCENE_POSE_ENTRY)==3?0xc0u:4u)) {
            held(pilot,game,&pilot->rudder,0);
            held(pilot,game,&pilot->throttle,rd_u16(CONTROL_RECORDS+110)?45:0);
            return;
        }
    }
    if(!pilot->objective && (rd_s16(SELECTED_RECORD)<0 ||
       ((pilot->mode==4 || pilot->mode==5 || pilot->final_flight) && rd_s16(SELECTED_RECORD)!=(int)(pilot->target*512))) &&
       game->ticks>pilot->target_press+200) {
        mission_pilot_event(pilot,game,116,1);pilot->target_press=game->ticks;
    }
    if(game->ticks<pilot->started+(pilot->force_return || pilot->complete_flight?575u:650u)) {
        held(pilot,game,&pilot->throttle,61);
        held(pilot,game,&pilot->pitch,game->ticks>=pilot->started+550?274:0);
        held(pilot,game,&pilot->rudder,0);
        return;
    }
    if(follow_stolen_aircraft(pilot,game)) return;
    if(combat_flight(pilot,game)) return;
    if(return_flight(pilot,game)) return;
    double point[3],position[3],local[3]={0};
    for(unsigned i=0;i<3;++i) position[i]=(double)rd_s32(CONTROL_RECORDS+20+4*i)/256;
    int gun_target=0,gun_aim=0;
    if(pilot->mode==5 && !pilot->objective) {
        pilot->target=4;
        for(unsigned slot=8;slot<=10;slot+=2) {
            const gaddr record=CONTROL_RECORDS+512*slot;
            if((rd_u16(record)&0x1648u)!=0x1040u) continue;
            const double range=hypot(hypot(rd_s32(record+20)/256.0-position[0],
                rd_s32(record+24)/256.0-position[1]),rd_s32(record+28)/256.0-position[2]);
            if(range<30000) {pilot->target=slot;gun_target=1;break;}
        }
    }
    if(!pilot->target) {
        double nearest=1e30;
        for(unsigned slot=1;slot<16;++slot) {
            const gaddr record=CONTROL_RECORDS+512*slot;
            if(!(rd_u8(record+1)&0x40) || (rd_u8(record+98)&0xf0)!=0x10) continue;
            double distance=0;
            for(unsigned i=0;i<3;++i) {
                double delta=(double)rd_s32(record+20+4*i)/256-position[i];distance+=delta*delta;
            }
            if(distance<nearest) {nearest=distance;pilot->target=slot;}
        }
    }
    if(rd_u8(PLAYER_PHASE)==0xff || rd_u8(PLAYER_PHASE)==1) {
        if(!pilot->objective) printf("{\"objective\":true,\"tick\":%u,\"phase\":%u}\n",game->ticks,rd_u8(PLAYER_PHASE));
        pilot->objective=1;if(!pilot->phase) pilot->phase=1;
    }
    if(!pilot->objective && pilot->target) {
        const gaddr target=CONTROL_RECORDS+512*pilot->target;
        for(unsigned i=0;i<3;++i) point[i]=(double)rd_s32(target+20+4*i)/256;
        if(pilot->mode==5) {
            /* Aim ahead using observed NPC motion; this is pilot input only. */
            double distance=hypot(hypot(point[0]-position[0],point[1]-position[1]),point[2]-position[2]);
            gun_aim=gun_target && distance<5000;
            double speed=gun_aim?198+(rd_s16(CONTROL_RECORDS+108)>>6):fmax(rd_s16(CONTROL_RECORDS+110)/64.0,70);
            double time=distance/speed;
            if(time>(gun_aim?20:2000)) time=gun_aim?20:2000;
            for(unsigned i=0;i<3;++i) if(gun_aim || i!=1)
                point[i]+=rd_s32(target+62+4*i)/256.0*time;
            point[1]+=gun_aim?0.5*time*(time+1):400;
        }
        /* Takeoff safety is pilot input, not a modification to game physics. */
        if(position[1]<pilot->home[1]+200) point[1]=position[1]+1000;
    } else {
        for(unsigned i=0;i<3;++i) point[i]=pilot->home[i]-pilot->forward[i]*12000;
        point[1]=pilot->home[1]+700;
        if(pilot->phase==1 && hypot(point[0]-position[0],point[2]-position[2])<1800) pilot->phase=2;
        if(pilot->phase>=2) {
            for(unsigned i=0;i<3;++i) point[i]=pilot->home[i]+pilot->forward[i]*2500;
            point[1]=pilot->home[1];
            held(pilot,game,&pilot->throttle,285); /* Source F4 level for approach. */
        }
    }
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
        local[i]+=(point[j]-position[j])*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
    if(gun_aim) local[1]-=local[2]*67.0/1024;
    const double x=atan2(local[0],local[2]);
    const double y=atan2(local[1],hypot(local[0],local[2]));
    const double px=x+4*angle_delta(x,pilot->previous_x),py=y+4*angle_delta(y,pilot->previous_y);
    pilot->previous_x=x;pilot->previous_y=y;
    double bank=rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800;
    if(bank>3.141592653589793) bank-=6.283185307179586;
    held(pilot,game,&pilot->roll,(pilot->phase==2 || pilot->mode==5) && fabs(x)<0.25?
        (bank>0.025?275:bank< -0.025?276:0):0);
    held(pilot,game,&pilot->rudder,px>0.006?44:px< -0.006?46:0);
    held(pilot,game,&pilot->pitch,pilot->phase==3?0:py>0.006?274:py< -0.006?273:0);
    if(pilot->mode==5) held(pilot,game,&pilot->fire,gun_aim && local[2]>0 &&
        fabs(x)<0.03 && fabs(y)<0.03?32:0);
}
