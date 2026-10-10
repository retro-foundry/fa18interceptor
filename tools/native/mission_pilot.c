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
static void command_throttle(MissionPilot *pilot,NativeFrontend *game,int throttle);
/* Rescue validation input: fly to the normally spawned site and deploy with
 * Shift+F. Position, velocity and pod state are observed, never written. */
static int rescue_flight(MissionPilot *pilot,NativeFrontend *game) {
    if(!pilot->rescue_flight || pilot->objective || rd_u8(PLAYER_PHASE)==0xff || rd_u8(PLAYER_PHASE)==1) return 0;
    const gaddr site=CONTROL_RECORDS+0x1600u;
    pilot->target=11;
    double delta[3],velocity[3],local[3]={0};
    for(unsigned i=0;i<3;++i) {
        delta[i]=((double)rd_s32(site+20+4*i)-rd_s32(CONTROL_RECORDS+20+4*i))/256.0;
        velocity[i]=rd_s32(CONTROL_RECORDS+62+4*i)/256.0;
    }
    const double range=hypot(delta[0],delta[2]);
    const double yaw=rd_u16(CONTROL_RECORDS+104)*6.283185307179586/28800;
    const double wanted=pilot->rescue_drop_tick?pilot->rescue_drop_yaw:atan2(-delta[0],delta[2]);
    const double error=angle_delta(wanted,yaw);
    const double speed=rd_s16(CONTROL_RECORDS+110)/64.0;
    const double direction[3]={-sin(yaw),fabs(error)>0.3?0:
        clamp((delta[1]+400)/3000-velocity[1]/fmax(speed,30),-0.12,0.12),cos(yaw)};
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
        local[i]+=direction[j]*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
    const double pitch=atan2(local[1],hypot(local[0],local[2]));
    if(!pilot->combat_started) {pilot->combat_started=1;pilot->previous_x=yaw;pilot->previous_y=pitch;}
    const double rudder=error-10*angle_delta(yaw,pilot->previous_x),elevator=pitch+4*angle_delta(pitch,pilot->previous_y);
    pilot->previous_x=yaw;pilot->previous_y=pitch;
    const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
    command_throttle(pilot,game,pilot->rescue_drop_tick?291:range>5000?288:286);
    held(pilot,game,&pilot->roll,bank>0.02?275:bank< -0.02?276:0);
    held(pilot,game,&pilot->rudder,rudder>0.006?46:rudder< -0.006?44:0);
    held(pilot,game,&pilot->pitch,elevator>0.006?274:elevator< -0.006?273:0);
    held(pilot,game,&pilot->fire,0);
    /* Input aiming estimate for a slowing pod; original C241A6/C25B66 decide
     * its actual motion/contact, and C0A15C alone decides near/far outcome. */
    if(!pilot->rescue_drop_tick && fabs(delta[0]-22*velocity[0])<150 &&
       fabs(delta[2]-22*velocity[2])<150 && fabs(error)<0.3 && delta[1]>-700) {
        pilot->rescue_drop_tick=game->ticks;
        pilot->rescue_drop_yaw=yaw;
        printf("{\"rescue_drop_input\":true,\"tick\":%u,\"range\":%.3f,\"height_above_site\":%.3f}\n",
            game->ticks,range,-delta[1]);
        /* Let the last steering release finish before this one-shot modifier.
         * The original C1C23C publication clears Shift after any command. */
    }
    return 1;
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
    /* New-pilot validation input: retain flying speed during the closing
     * turn, and damp height feedback with observed vertical velocity. */
    const double pitch_heading=pilot->tour_flight?yaw:wanted_yaw;
    const double slope=pilot->tour_flight?
        clamp((delta[1]+height)/3000-velocity[1]/fmax(speed,30),-0.12,0.12):
        clamp((delta[1]+height)/6000,-0.08,0.08);
    const double direction[3]={-sin(pitch_heading),
        pilot->tour_flight && fabs(angle_delta(wanted_yaw,yaw))>0.3?0:slope,cos(pitch_heading)};
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
    if(pilot->tour_flight) {
        const double flying_speed=fmax(wanted_speed,80);
        throttle=range>30000 || speed<flying_speed-1?291:speed>flying_speed+1?286:288;
    }
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
    for(unsigned slot=pilot->cruise_flight?4u:pilot->campaign_flight && !pilot->final_flight?8u:4u;
        slot<=(pilot->cruise_flight?4u:pilot->campaign_flight && !pilot->final_flight?12u:14u);slot+=2) {
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
    const double range=sqrt(nearest);
    double time=fmin(range/speed,20);
    if(pilot->campaign_flight && pilot->cruise_flight) {
        /* A receding low target needs an intercept estimate, rather than
         * range divided by absolute bullet speed. Source C2436A retains
         * the 198-plus-aircraft-speed projectile; only aim keys change. */
        double a=-speed*speed,b=0;
        for(unsigned i=0;i<3;++i) {
            const double velocity=rd_s32(target+62+4*i)/256.0;
            const double distance=(rd_s32(target+20+4*i)-rd_s32(CONTROL_RECORDS+20+4*i))/256.0;
            a+=velocity*velocity;b+=2*distance*velocity;
        }
        const double discriminant=b*b-4*a*nearest;
        if(a<0 && discriminant>=0) time=fmin((-b-sqrt(discriminant))/(2*a),20);
    }
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
    const int throttle=pilot->campaign_flight && pilot->cruise_flight?
        (range>850?291:range<500?285:288):actual<wanted-1?291:actual>wanted+1?285:288;
    command_throttle(pilot,game,throttle);
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
    if(pilot->campaign_flight && pilot->cruise_flight && getenv("FA18_MISSION_TRACE") && game->ticks%100<3)
        printf("{\"cruise_gun_aim\":true,\"tick\":%u,\"range\":%.3f,\"local_z\":%.3f,\"x\":%.6f,\"y\":%.6f,\"damage\":%u,\"fire\":%s,\"fire_state\":%u,\"weapon\":%u,\"command_word\":%u,\"gear_flags\":%u,\"gun_request\":%u,\"gun_budget\":%u}\n",
            game->ticks,range,local[2],x,y,rd_u8(target+60),gun?"true":"false",rd_u8(FIRE_STATE),weapon,
            rd_u16(COMMAND_WORD),rd_u8(COMMAND_BLOCK_FLAGS),rd_u8(CONTROL_RECORDS+125),rd_u16(CONTROL_RECORDS+96));
    held(pilot,game,&pilot->fire,gun || (weapon==0x30u && game->ticks==pilot->final_shot_tick)?32:0);
    return 1;
}
/* Mission validation only: pursue the remaining active enemies and launch
 * radar missiles with normal controls. No hit, collision or outcome state
 * is written. */
static int combat_flight(MissionPilot *pilot,NativeFrontend *game) {
    if(!pilot->complete_flight || (pilot->mode==5 && !pilot->formation_done) || pilot->objective ||
       rd_u8(PLAYER_PHASE)==0xff || rd_u8(PLAYER_PHASE)==1) return 0;
    if(pilot->campaign_flight && (!rd_u8(CONTROL_RECORDS+95) || pilot->cruise_flight) &&
       (rd_u8(CONTROL_RECORDS+99)&0xf0u)==0x10u)
        return final_patrol_gun(pilot,game);
    if(pilot->final_sequence && !pilot->new_final_flight && rd_u8(SCENE_DISPATCH_AUX)>=3) {
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
    for(unsigned slot=pilot->final_flight || pilot->cruise_flight?4u:8u;
        slot<=(pilot->cruise_flight?4u:pilot->final_flight?14u:pilot->escort_flight || pilot->tour_flight?12u:10u);slot+=2) {
        const gaddr record=CONTROL_RECORDS+512*slot;
        if((rd_u16(record)&0x1648u)!=0x1040u) continue;
        if(pilot->final_flight && (rd_u8(record+98)&0xf0u)!=0x10u) continue;
        if(pilot->new_final_flight && rd_u8(record+98)==0x15u) continue;
        if(pilot->new_final_flight && ((rd_u16(record+2)&0x1000u) || (rd_u16(record)&0x400u))) continue;
        if(pilot->campaign_flight && pilot->mode==5 &&
           ((rd_u16(record+2)&0x1000u) || (rd_u16(record)&0x400u))) continue;
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
    const int patrol_wait=pilot->new_final_flight && rd_u8(SCENE_DISPATCH_CREATED)==3 &&
        !(rd_u8(CONTROL_RECORDS+95)&0xf0u) && pilot->missile_tick && game->ticks<pilot->missile_tick+400;
    if(!selected || patrol_wait) {
        if(pilot->campaign_flight && pilot->mode==5) {
            /* Wait for the source's falling-wreck expiry instead of chasing
             * its already destroyed aircraft down to the surface. */
            const double attitude=angle_delta(rd_u16(CONTROL_RECORDS+102)*6.283185307179586/28800,0);
            const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
            const double speed=rd_s16(CONTROL_RECORDS+110)/64.0;
            const double slope=clamp((pilot->home[1]+2000-rd_s32(CONTROL_RECORDS+24)/256.0)/6000-
                rd_s32(CONTROL_RECORDS+66)/256.0/fmax(speed,30),-0.12,0.12);
            const double elevator=attitude+atan(slope)+10*angle_delta(attitude,pilot->previous_y);
            pilot->previous_y=attitude;
            held(pilot,game,&pilot->pitch,elevator>0.006?274:elevator< -0.006?273:0);
            held(pilot,game,&pilot->rudder,0);
            held(pilot,game,&pilot->roll,bank>0.02?275:bank< -0.02?276:0);
            command_throttle(pilot,game,288);
        }
        if(pilot->new_final_flight) {
            /* Stop pursuing a falling wreck while the original regional
             * scheduler prepares another aircraft. Loiter just outside
             * the cruise-missile box in C29720's original region table,
             * leaving the admission slot available for another patrol. */
            const gaddr cruise_region=rd_u32(0xc29720u+12);
            const double waiting_x=patrol_wait || rd_u8(SCENE_DISPATCH_AUX)>=3?
                (rd_s16(cruise_region)-0.5)*16384:rd_s32(CONTROL_RECORDS+512*12+20)/256.0;
            const double waiting_z=patrol_wait || rd_u8(SCENE_DISPATCH_AUX)>=3?
                (rd_s16(cruise_region+4)-0.5)*16384:rd_s32(CONTROL_RECORDS+512*12+28)/256.0;
            const double yaw=rd_u16(CONTROL_RECORDS+104)*6.283185307179586/28800;
            const double wanted=patrol_wait?3.141592653589793:
                atan2(-(waiting_x-rd_s32(CONTROL_RECORDS+20)/256.0),
                waiting_z-rd_s32(CONTROL_RECORDS+28)/256.0);
            const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
            const double attitude=angle_delta(rd_u16(CONTROL_RECORDS+102)*6.283185307179586/28800,0);
            const double rudder=angle_delta(wanted,yaw)-10*angle_delta(yaw,pilot->previous_x);
            const double elevator=attitude+10*angle_delta(attitude,pilot->previous_y);
            pilot->previous_x=yaw;pilot->previous_y=attitude;
            held(pilot,game,&pilot->pitch,elevator>0.006?274:elevator< -0.006?273:0);
            held(pilot,game,&pilot->rudder,rudder>0.006?46:rudder< -0.006?44:0);
            held(pilot,game,&pilot->roll,bank>0.02?275:bank< -0.02?276:0);
            command_throttle(pilot,game,288);
        }
        held(pilot,game,&pilot->fire,0);return 1;
    }
    const gaddr target=CONTROL_RECORDS+512*selected;
    const double range=sqrt(target_distance_squared),speed=rd_s16(CONTROL_RECORDS+110)/64.0;
    /* Input aim for the regional closing pass uses a shorter intercept lead;
     * the actual projectile continues to use original motion/tracking rules. */
    const int final_heat=pilot->final_flight && (rd_u8(CONTROL_RECORDS+99)&0xf0u)==0x30u;
    const double time=clamp(range/(pilot->new_final_flight || final_heat || ((pilot->escort_flight || pilot->tour_flight) && selected==12)?198+speed:fmax(speed,70)),0,400);
    double delta[3],local[3]={0};
    for(unsigned i=0;i<3;++i)
        delta[i]=(rd_s32(target+20+4*i)-rd_s32(CONTROL_RECORDS+20+4*i))/256.0+
            rd_s32(target+62+4*i)/256.0*time;
    delta[1]+=400;
    const double yaw=rd_u16(CONTROL_RECORDS+104)*6.283185307179586/28800;
    double wanted_yaw=atan2(-delta[0],delta[2]);
    const int ground_attack=pilot->campaign_flight && pilot->mode==5 && (rd_u16(target+2)&0x80);
    if(ground_attack && pilot->ground_attack_target!=selected && range<8000) {
        pilot->ground_attack_target=selected;pilot->ground_attack_yaw=yaw;pilot->ground_breakaway=1;
        printf("{\"ground_attack_breakaway\":true,\"tick\":%u,\"target\":%u}\n",game->ticks,selected);
    }
    if(pilot->ground_breakaway) {
        if(range>=12000 || !ground_attack) pilot->ground_breakaway=0;
        else wanted_yaw=pilot->ground_attack_yaw;
    }
    const double yaw_error=angle_delta(wanted_yaw,yaw);
    const double horizontal=hypot(delta[0],delta[2]);
    /* Mode-four input keeps pitch relative to the current heading while
     * turning toward higher enemy aircraft; the accepted mode-five keys
     * retain their existing heading choice. */
    const double pitch_heading=pilot->mode==4 || pilot->final_flight || pilot->cruise_flight || pilot->tour_flight?yaw:wanted_yaw;
    const double pitch_slope=pilot->mode==4 || pilot->final_flight || pilot->cruise_flight || pilot->tour_flight?
        clamp(delta[1]/6000-rd_s32(CONTROL_RECORDS+66)/256.0/fmax(speed,30),-0.12,0.12):
        clamp(delta[1]/fmax(horizontal,100),-0.05,0.05);
    const double direction[3]={-sin(pitch_heading),fabs(yaw_error)>0.3?0:
        pitch_slope,cos(pitch_heading)};
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
        local[i]+=direction[j]*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
    const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
    const double x=atan2(local[0],local[2]),y=atan2(local[1],hypot(local[0],local[2]));
    const double attitude_pitch=angle_delta(rd_u16(CONTROL_RECORDS+102)*6.283185307179586/28800,0);
    if(!pilot->combat_started || pilot->target!=selected) {
        pilot->combat_started=1;pilot->previous_x=yaw;pilot->previous_y=pilot->new_final_flight || pilot->campaign_flight?attitude_pitch:y;
    }
    pilot->target=selected;
    const double yaw_control=angle_delta(wanted_yaw,yaw)-10*angle_delta(yaw,pilot->previous_x);
    double pitch_control=y+4*angle_delta(y,pilot->previous_y);
    if(pilot->new_final_flight || pilot->campaign_flight) {
        /* Validation input: keep a level turn and a safe selected height.
         * Use observed attitude feedback rather than chasing the target's
         * predicted vertical lead through its close-pass reversal. */
        /* In the continuous stolen-aircraft fight, surviving opponents may
         * land while retaining their active records. Descend to the normal
         * low attack height instead of orbiting above an unreachable aim. */
        const int landed_opponent=pilot->campaign_flight && pilot->mode==5 && (rd_u16(target+2)&0x80);
        const int cruise_heat=pilot->campaign_flight && pilot->cruise_flight && (rd_u8(CONTROL_RECORDS+99)&0xf0u)==0x30u;
        const double height=fmax(rd_s32(target+24)/256.0+400,pilot->home[1]+(landed_opponent || cruise_heat?400:2000));
        const double slope=fabs(yaw_error)>0.3 && !landed_opponent?0:
            clamp((height-rd_s32(CONTROL_RECORDS+24)/256.0)/6000-
                rd_s32(CONTROL_RECORDS+66)/256.0/fmax(speed,30),-0.12,0.12);
        const double desired_pitch=-atan(slope);
        pitch_control=attitude_pitch-desired_pitch+10*angle_delta(attitude_pitch,pilot->previous_y);
    }
    pilot->previous_x=yaw;pilot->previous_y=pilot->new_final_flight || pilot->campaign_flight?attitude_pitch:y;
    double wanted_speed=pilot->new_final_flight && final_heat?100:
        rd_s16(target+110)/64.0+clamp((range-1800)/100,-25,pilot->campaign_flight && pilot->cruise_flight?5:25);
    /* Continuous-campaign pilot input must retain flying speed while a
     * higher-level enemy slows or reverses. This only chooses throttle keys. */
    if(pilot->campaign_flight) wanted_speed=fmax(wanted_speed,90);
    const int throttle=speed<wanted_speed-1?291:speed>wanted_speed+1?285:288;
    command_throttle(pilot,game,throttle);
    held(pilot,game,&pilot->roll,bank>0.02?275:bank< -0.02?276:0);
    held(pilot,game,&pilot->rudder,yaw_control>0.006?46:yaw_control< -0.006?44:0);
    held(pilot,game,&pilot->pitch,pitch_control>0.006?274:pitch_control< -0.006?273:0);
    if((pilot->mode==4 || pilot->final_flight || pilot->cruise_flight || pilot->campaign_flight) && pilot->missile_target==selected && game->ticks>pilot->missile_tick+200) {
        int active=0;
        for(unsigned slot=1;slot<=3;++slot) {
            const gaddr projectile=CONTROL_RECORDS+512*slot;
            active|=(rd_u8(projectile+1)&0x48u)==0x48u && !rd_u8(projectile+94);
        }
        if(!active) {
            pilot->missile_target=0;
            if(ground_attack) pilot->ground_attack_target=0;
        }
    }
    /* Escort validation fires earlier on the closing pass. These are pilot
     * input choices; original launch/tracking/damage rules decide the result. */
    const int close_aim=pilot->mode==4 && !pilot->escort_flight;
    const double launch_range=pilot->new_final_flight && !final_heat?30000:close_aim || (pilot->campaign_flight && pilot->cruise_flight && (rd_u8(CONTROL_RECORDS+99)&0xf0u)==0x30u)?10000:20000;
    const uint8_t weapon=rd_u8(CONTROL_RECORDS+99)&0xf0u,stock=rd_u8(CONTROL_RECORDS+95);
    const int final_ready=(!pilot->final_flight && !pilot->campaign_flight) || (weapon==0x20 && (stock&0xf0u)) ||
        (weapon==0x30 && (stock&15u));
    int projectile_active=0;
    if(pilot->new_final_flight || pilot->campaign_flight) for(unsigned slot=1;slot<=3;++slot) {
        const gaddr projectile=CONTROL_RECORDS+512*slot;
        projectile_active|=(rd_u8(projectile+1)&0x48u)==0x48u && !rd_u8(projectile+94);
    }
    double weapon_aim[3]={0};
    if(pilot->new_final_flight || pilot->campaign_flight) for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
        weapon_aim[i]+=delta[j]*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
    const int weapon_aligned=(!pilot->new_final_flight && (!pilot->campaign_flight || pilot->mode==5 || pilot->cruise_flight)) || (weapon_aim[2]>0 &&
        fabs(atan2(weapon_aim[0],weapon_aim[2]))<(pilot->campaign_flight && pilot->cruise_flight?0.005:pilot->campaign_flight && pilot->mode==5?0.2:0.035) &&
        fabs(atan2(weapon_aim[1],hypot(weapon_aim[0],weapon_aim[2])))<(pilot->campaign_flight && pilot->mode==5?0.25:0.08));
    const int launch=!pilot->patrol_flight && final_ready && !projectile_active && !pilot->ground_breakaway && rd_s16(SELECTED_RECORD)==(int)(selected*512) && range<launch_range &&
        weapon_aligned &&
        (!(pilot->final_flight && weapon==0x30) ||
            ((pilot->new_final_flight || rd_u8(SHOOT_CUE)) && range<(pilot->new_final_flight?15000:10000) && fabs(yaw_error)<0.1)) &&
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
            pilot->escort_flight || pilot->final_flight || pilot->cruise_flight?12u:4u)?32:0);
    return 1;
}
static void carrier_wire_destination(MissionPilot *pilot) {
    gaddr carrier=0;
    unsigned slot=0,count=0;
    /* C26EBE's carrier class and its three original arrestor vertices.
     * Read the running game's geometry; no recorded coordinates are input. */
    for(unsigned i=0;i<16;++i) {
        const gaddr record=CONTROL_RECORDS+512*i;
        if((rd_u16(record)&0x40) && rd_u8(record+98)==0x20) {
            carrier=record;slot=i;++count;
        }
    }
    if(count!=1 || rd_u8(SCENE_POSE_ENTRY)!=3) {
        fprintf(stderr,"Carrier-wire validation requires one live carrier and carrier takeoff; found %u, pose %u\n",
            count,rd_u8(SCENE_POSE_ENTRY));exit(2);
    }
    const unsigned scale=rd_u8(carrier+125)&15;
    const double takeoff[3]={pilot->home[0],pilot->home[1],pilot->home[2]};
    double vertices[3][3],centroid[3]={0};
    for(unsigned vertex=0;vertex<3;++vertex) for(unsigned axis=0;axis<3;++axis) {
        vertices[vertex][axis]=rd_s32(carrier+20+4*axis)/256.0+
            (rd_s16(carrier+488+6*vertex+2*axis)>>scale);
        centroid[axis]+=vertices[vertex][axis]/3;
    }
    const double area=(vertices[1][0]-vertices[0][0])*(vertices[2][2]-vertices[0][2])-
        (vertices[1][2]-vertices[0][2])*(vertices[2][0]-vertices[0][0]);
    if(!area) {fputs("Carrier-wire validation found degenerate arrestor geometry\n",stderr);exit(2);}
    pilot->home[0]=centroid[0];pilot->home[2]=centroid[2];
    /* This route starts on that carrier. Retain its observed aircraft
     * touchdown height; the game owns gear clearance and ground contact. */
    for(unsigned axis=0;axis<3;++axis)
        pilot->forward[axis]=rd_s16(carrier+150+6*axis)/16384.0;
    printf("{\"carrier_wire_target\":true,\"slot\":%u,\"takeoff_home\":[%.6f,%.6f,%.6f],"
           "\"target\":[%.6f,%.6f,%.6f],\"wire_height\":%.6f,\"forward\":[%.6f,%.6f,%.6f]}\n",
        slot,takeoff[0],takeoff[1],takeoff[2],pilot->home[0],pilot->home[1],pilot->home[2],
        centroid[1],pilot->forward[0],pilot->forward[1],pilot->forward[2]);
}
/* Return flight input for the mode-five validation pilot. Keep heading
 * feedback in world coordinates across the combat/return handoff. */
static int return_flight(MissionPilot *pilot,NativeFrontend *game) {
    if(!pilot->complete_flight || (!pilot->objective &&
       rd_u8(PLAYER_PHASE)!=0xff && rd_u8(PLAYER_PHASE)!=1)) return 0;
    if((pilot->escort_flight || pilot->final_flight || pilot->rescue_flight || pilot->cruise_flight || pilot->tour_flight || pilot->campaign_flight) && pilot->return_input_phase!=rd_u8(PLAYER_PHASE)) {
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
    if(pilot->carrier_wire_return && !pilot->return_started) carrier_wire_destination(pilot);
    double point[3],position[3],local[3]={0};
    const int high_return=pilot->rescue_flight || pilot->cruise_flight;
    for(unsigned i=0;i<3;++i) {
        position[i]=rd_s32(CONTROL_RECORDS+20+4*i)/256.0;
        point[i]=pilot->home[i]-pilot->forward[i]*(high_return?24000:12000);
    }
    point[1]=pilot->home[1]+(high_return?2200:700);
    const double approach_distance=hypot(point[0]-position[0],point[2]-position[2]);
    if(pilot->touchdown_approach && pilot->phase==1 && approach_distance<1800)
        pilot->touchdown_descent_started=1;
    /* Validation input only: finish descent over the offshore standoff,
     * using this flight's observed carrier takeoff height. The original
     * game still owns the actual height, contact, arrest and result. */
    if(pilot->touchdown_descent_started) point[1]=pilot->home[1];
    const double approach_height_limit=pilot->approach_at_standoff_height?
        point[1]:pilot->home[1]+1400;
    if(pilot->phase==1 && approach_distance<1800 &&
       (!(pilot->final_sequence || pilot->wait_for_approach_height) ||
        position[1]<approach_height_limit)) {
        pilot->phase=2;
        if(pilot->wait_for_approach_height)
            printf("{\"approach_height_ready\":true,\"tick\":%u,\"height\":%.6f,\"home_height\":%.6f,\"height_limit\":%.6f,\"distance\":%.6f}\n",
                game->ticks,position[1],pilot->home[1],approach_height_limit,approach_distance);
    }
    if(pilot->phase>=2) {
        /* Pilot input: intercept the centerline and descend onto the wire
         * region before the starting pose, rather than flying past it. */
        const double before_home=(pilot->home[0]-position[0])*pilot->forward[0]+
            (pilot->home[2]-position[2])*pilot->forward[2];
        for(unsigned i=0;i<3;++i)
            point[i]=pilot->home[i]+pilot->forward[i]*(2000-before_home);
        point[1]=pilot->home[1]-(high_return?250:200)+fmax(before_home,0)*0.04;
        if(pilot->touchdown_approach) point[1]=pilot->home[1];
        if(pilot->patrol_flight) {
            /* Validation runway approach: aim beyond the starting position
             * and descend to its observed height, not the carrier wire. */
            for(unsigned i=0;i<3;++i) point[i]=pilot->home[i]+pilot->forward[i]*2500;
            point[1]=pilot->home[1];
        }
    }
    const double wanted_yaw=atan2(-(point[0]-position[0]),point[2]-position[2]);
    const double yaw=rd_u16(CONTROL_RECORDS+104)*6.283185307179586/28800;
    const double yaw_error=angle_delta(wanted_yaw,yaw);
    const double speed=rd_s16(CONTROL_RECORDS+110)/64.0;
    const double vertical_speed=rd_s32(CONTROL_RECORDS+66)/256.0;
    /* Pitch follows current heading so a return turn cannot reverse its
     * feedback; observed vertical speed damps the selected height approach. */
    const double descent=pilot->final_sequence && pilot->phase==1?0.25:0.12;
    double direction[3]={-sin(yaw),fabs(yaw_error)>0.3?0:
        clamp((point[1]-position[1])/3000-
              vertical_speed/fmax(speed,30),-descent,descent),cos(yaw)};
    const int carrier_turn_descent=pilot->mode==5 && pilot->carrier_wire_return &&
        pilot->wait_for_approach_height && pilot->phase==1 &&
        (approach_distance<1800 || pilot->touchdown_descent_started);
    if(pilot->patrol_flight || carrier_turn_descent) {
        /* The test pilot holds its approach height through the return turn.
         * The mode-five wire route must also descend during its standoff
         * orbit to reach the requested height. Keys remain the only output;
         * the original controls all motion. */
        direction[1]=clamp((point[1]-position[1])/6000-
                          vertical_speed/fmax(speed,30),-0.12,0.12);
    }
    for(unsigned i=0;i<3;++i) for(unsigned j=0;j<3;++j)
        local[i]+=direction[j]*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/16384;
    const double pitch=pilot->new_final_flight || pilot->patrol_flight?
        angle_delta(rd_u16(CONTROL_RECORDS+102)*6.283185307179586/28800,0):
        atan2(local[1],hypot(local[0],local[2]));
    const double bank=angle_delta(rd_u16(CONTROL_RECORDS+106)*6.283185307179586/28800,0);
    if(!pilot->return_started) {
        pilot->return_started=1;pilot->previous_x=yaw;pilot->previous_y=pitch;
        printf("{\"return_start\":true,\"tick\":%u,\"pose\":%u,\"home\":[%.3f,%.3f,%.3f],"
               "\"forward\":[%.3f,%.3f,%.3f]}\n",game->ticks,rd_u8(SCENE_POSE_ENTRY),
               pilot->home[0],pilot->home[1],pilot->home[2],
               pilot->forward[0],pilot->forward[1],pilot->forward[2]);
    }
    const double yaw_control=yaw_error-10*angle_delta(yaw,pilot->previous_x);
    const double pitch_control=pilot->new_final_flight || pilot->patrol_flight?
        pitch+atan(direction[1])+10*angle_delta(pitch,pilot->previous_y):
        pitch+4*angle_delta(pitch,pilot->previous_y);
    pilot->previous_x=yaw;pilot->previous_y=pitch;
    /* F9 on the escort's long return keeps separation from the regional
     * fighter; the established F5 approach remains the landing input. */
    command_throttle(pilot,game,pilot->phase>=2 || approach_distance<20000?286:
        pilot->cruise_flight?291:pilot->escort_flight?290:288);
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
    const unsigned trace_interval=(pilot->escort_flight || pilot->cruise_flight || pilot->campaign_flight) && getenv("FA18_MISSION_WEAPON_TRACE")?100u:500u;
    if(getenv("FA18_MISSION_TRACE") && game->ticks/trace_interval!=pilot->trace) {
        pilot->trace=game->ticks/trace_interval;
        printf("{\"trace\":true,\"tick\":%u,\"phase\":%u,\"pilot_phase\":%u,\"gate\":%d,"
               "\"function_level\":%u,\"controls\":%u,\"thrust\":%d,\"fuel\":%u,"
               "\"admitted\":%u,\"created\":%u,\"aux\":%u,\"gun_hits\":%u,\"radar_hits\":%u,\"weapon\":%u,\"stock\":%u,\"selected\":%d,\"pilot_target\":%u,\"cockpit_flags\":%u,\"message_state\":%u,\"view_record\":%d,\"context_select\":%u,\"stream_mode\":%d,\"input_queued\":%u,\"block_flags\":%u,\"records\":[",
               game->ticks,rd_u8(PLAYER_PHASE),pilot->phase,rd_s16(SCENE_DISPATCH_GATE),
               rd_u8(FUNCTION_KEY_LEVEL),rd_u8(PLAYER_STICK),rd_s8(CONTROL_RECORDS+43),rd_u32(CONTROL_RECORDS+114),
               rd_u8(SCENE_DISPATCH_ADMITTED),rd_u8(SCENE_DISPATCH_CREATED),rd_u8(SCENE_DISPATCH_AUX),
               rd_u16(rd_u32(MODE_TABLE)+60),rd_u16(rd_u32(MODE_TABLE)+68),rd_u8(CONTROL_RECORDS+99)&0xf0,
               rd_u8(CONTROL_RECORDS+95),rd_s16(SELECTED_RECORD),pilot->target,rd_u16(COCKPIT_FLAGS),rd_u16(MESSAGE_STATE),
               rd_s16(VIEW_RECORD),rd_u8(CONTEXT_SELECT),rd_s16(STREAM_MODE),game->input_count,rd_u8(COMMAND_BLOCK_FLAGS));
        for(unsigned slot=0;slot<=(pilot->new_final_flight?15u:pilot->rescue_flight || pilot->cruise_flight?14u:12u);slot+=trace_interval==100 || pilot->campaign_flight || pilot->rescue_flight || pilot->cruise_flight || pilot->new_final_flight?1u:2u) {
            gaddr record=CONTROL_RECORDS+512*slot;
            printf("%s{\"slot\":%u,\"flags\":%u,\"kind\":%u,\"contact\":%u,\"region\":%u,\"damage\":%u,\"motion_flags\":%u,"
                   "\"speed\":[%d,%d],\"position\":[%d,%d,%d],\"angles\":[%d,%d,%d],\"linked\":%u,\"lifetime\":%d}",
                   slot?",":"",slot,rd_u16(record),rd_u8(record+98),rd_u16(record+2),rd_u8(record+4),rd_u8(record+60),rd_u8(record+32),
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
    if(pilot->manage_gear && rd_u32(STAGE_CALLBACK)==0xc10dae &&
       !(rd_u16(CONTROL_RECORDS+2)&0x80) && game->ticks>pilot->started+575 &&
       game->ticks>pilot->gear_key_tick+80) {
        /* Original G/raw $24 toggles C46200 bit seven: set is retracted.
         * Raise after leaving the surface; lower before the return approach.
         * C1BC12 retains the original inhibition and damage-gate decisions. */
        const int raised=(rd_u8(COMMAND_BLOCK_FLAGS)&0x80)!=0;
        const int wanted=!pilot->objective && !pilot->phase;
        if(raised!=wanted) {
            mission_pilot_event(pilot,game,103,1);pilot->gear_key_tick=game->ticks;
            printf("{\"gear_input\":true,\"tick\":%u,\"raise\":%s}\n",game->ticks,wanted?"true":"false");
        }
    }
    if(((pilot->new_final_flight && !pilot->objective) || pilot->campaign_flight) && game->ticks>pilot->defense_tick+80) {
        /* Validation-only F/C keys exercise flight_commands.c's original
         * countermeasure owner; the game decides stock use and diversion. */
        for(unsigned slot=4;slot<16;++slot) {
            const gaddr incoming=CONTROL_RECORDS+512*slot;
            const unsigned kind=rd_u8(incoming+98);
            if((rd_u16(incoming)&0x1648u)!=0x1040u || kind>1 || rd_u8(incoming+56)!=0x80u) continue;
            if(rd_u8(kind?MISSION_LEVEL_B:MISSION_LEVEL_A)<=1) continue;
            pilot->defense_key=kind?102:99;
            pilot->defense_tick=game->ticks;
            mission_pilot_event(pilot,game,pilot->defense_key,1);
            if(getenv("FA18_MISSION_TRACE")) printf("{\"countermeasure_input\":true,\"tick\":%u,\"slot\":%u,\"kind\":%u}\n",game->ticks,slot,kind);
            break;
        }
    }
    if(pilot->rescue_drop_tick && game->ticks<pilot->rescue_drop_tick+20) return;
    if(!pilot->patrol_flight && (pilot->final_flight || (pilot->campaign_flight && pilot->mode!=6)) && !pilot->objective && game->ticks>pilot->started+750) {
        const uint8_t weapon=rd_u8(CONTROL_RECORDS+99)&0xf0u;
        const uint8_t stock=rd_u8(CONTROL_RECORDS+95);
        const gaddr target=CONTROL_RECORDS+512*pilot->target;
        int cruise_gun=0;
        if(pilot->campaign_flight && pilot->cruise_flight && !stock) {
            int active=0;
            for(unsigned slot=1;slot<=3;++slot) {
                const gaddr projectile=CONTROL_RECORDS+512*slot;
                active|=(rd_u8(projectile+1)&0x48u)==0x48u && !rd_u8(projectile+94);
            }
            /* Ordinary gun fallback after a missed missile, using the
             * existing observed-motion gun aiming input. */
            cruise_gun=weapon==0x10u || !active;
        }
        const uint8_t wanted=cruise_gun?0x10u:pilot->new_final_flight && !rd_u8(SCENE_DISPATCH_AUX) && (stock&15u)?0x30u:
            pilot->final_sequence && !pilot->new_final_flight && rd_u8(SCENE_DISPATCH_AUX)>=3?
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
       ((pilot->mode==4 || pilot->mode==5 || pilot->final_flight || pilot->cruise_flight) && rd_s16(SELECTED_RECORD)!=(int)(pilot->target*512))) &&
       game->ticks>pilot->target_press+200) {
        mission_pilot_event(pilot,game,116,1);pilot->target_press=game->ticks;
    }
    if(game->ticks<pilot->started+(pilot->force_return || pilot->complete_flight?575u:650u)) {
        held(pilot,game,&pilot->throttle,61);
        held(pilot,game,&pilot->pitch,game->ticks>=pilot->started+550?274:0);
        held(pilot,game,&pilot->rudder,0);
        return;
    }
    if(rescue_flight(pilot,game)) return;
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
