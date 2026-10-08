/* Validation-only gun telemetry and ordinary-keyboard test pilot.
 * Never linked into fa18_native or used as game behavior. */
#include "gun_discovery.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Read-only discovery telemetry, never supplied to the game or its oracle. */
void gun_discovery_target_local(gaddr record,int64_t local[3]) {
    int64_t delta[3];
    for(unsigned i=0;i<3;++i) {
        /* +20/+24/+28 are complete world positions, as C2436A uses them.
         * The separate +6/+8 cells must not be added a second time. */
        delta[i]=(int64_t)rd_s32(record+20+4*i)-rd_s32(CONTROL_RECORDS+20+4*i);
    }
    for(unsigned i=0;i<3;++i) {
        local[i]=0;
        for(unsigned j=0;j<3;++j)
            local[i]+=(delta[j]/256)*rd_s16(CONTROL_RECORDS+146+6*j+2*i);
        local[i]/=16384;
    }
}
void gun_discovery_trace(GunDiscovery *pilot,unsigned tick) {
    if(!getenv("FA18_GUN_TRACE") || tick<12000 || tick>=22000 ||
       tick/125==pilot->trace_tick) return;
    pilot->trace_tick=tick/125;
    printf("{\"gun_approach\":true,\"tick\":%u,\"player_angles\":[%u,%u,%u],\"controls\":%u,\"steering\":[%d,%d,%d],\"position\":[%d,%d,%d],\"target_velocity\":[%d,%d,%d],\"records\":[",
        tick,rd_u16(CONTROL_RECORDS+102),rd_u16(CONTROL_RECORDS+104),rd_u16(CONTROL_RECORDS+106),
        rd_u8(PLAYER_STICK),
        rd_s16(CONTROL_RECORDS+86),rd_s16(CONTROL_RECORDS+88),rd_s16(CONTROL_RECORDS+90),
        rd_s32(CONTROL_RECORDS+20)/256,rd_s32(CONTROL_RECORDS+24)/256,rd_s32(CONTROL_RECORDS+28)/256,
        rd_s32(CONTROL_RECORDS+5120+62),rd_s32(CONTROL_RECORDS+5120+66),rd_s32(CONTROL_RECORDS+5120+70));
    for(unsigned slot=8;slot<=12;slot+=2) {
        const gaddr record=CONTROL_RECORDS+512*slot;
        int64_t local[3];gun_discovery_target_local(record,local);
        printf("%s{\"slot\":%u,\"flags\":%u,\"local\":[%lld,%lld,%lld],\"damage\":%u}",
            slot==8?"":",",slot,rd_u16(record),local[0],local[1],local[2],rd_u8(record+60));
    }
    printf("],\"bullet\":[");
    for(unsigned slot=0;slot<20;++slot) {
        const gaddr bullet=0xc45c72u+64*slot;
        if(rd_u16(bullet+38)&1u) {
            printf("%d,%d,%d,%d",rd_s32(bullet+12),rd_s32(bullet+16),rd_s32(bullet+20),rd_s16(bullet+40));
            break;
        }
    }
    puts("]}");
}
static void discovery_key(NativeFrontend *game,int *held,int key) {
    if(*held==key) return;
    if(*held) native_frontend_event(game,*held,0);
    if(key) native_frontend_event(game,key,1);
    *held=key;
}
/* Test pilot: only host events. No control, motion or outcome RAM writes. */
void gun_discovery_tick(GunDiscovery *pilot,NativeFrontend *game,int hit) {
    if(game->ticks<12000 ||
       game->flight_timer_pending || game->scene_frames==pilot->scene) return;
    pilot->scene=game->scene_frames;
    if(hit) {
        discovery_key(game,&pilot->rudder,0);
        discovery_key(game,&pilot->fire,0);
        discovery_key(game,&pilot->pitch,0);
        return;
    }
    const gaddr target=CONTROL_RECORDS+512*10;
    int64_t local[3];gun_discovery_target_local(target,local);
    const double speed=198+(rd_s16(CONTROL_RECORDS+108)>>6);
    double time=hypot(hypot((double)local[0],(double)local[1]),(double)local[2])/speed;
    if(time>20) time=20;
    for(unsigned i=0;i<3;++i) {
        double velocity=0;
        for(unsigned j=0;j<3;++j)
            velocity+=(double)rd_s32(target+62+4*j)*rd_s16(CONTROL_RECORDS+146+6*j+2*i)/4194304.0;
        local[i]+=(int64_t)(velocity*time+
            0.5*time*(time+1)*rd_s16(CONTROL_RECORDS+152+2*i)/16384.0);
    }
    const double x=atan2((double)local[0],(double)local[2]);
    const double y=atan2((double)local[1]-(double)local[2]*67.0/1024,
        hypot((double)local[0],(double)local[2]));
    double dx=x-pilot->x;
    if(dx>3.141592653589793) dx-=6.283185307179586;
    if(dx< -3.141592653589793) dx+=6.283185307179586;
    const double px=x+(pilot->tick?4*dx:0);
    const double py=y+(pilot->tick?4*(y-pilot->y):0);
    pilot->x=x;pilot->y=y;pilot->tick=game->ticks;
    const int active=game->ticks<19000 && rd_u32(STAGE_CALLBACK)==0xc10dae &&
        (rd_u16(target)&0x640)==0x40;
    discovery_key(game,&pilot->rudder,active?(px>0.002?44:px< -0.002?46:0):0);
    discovery_key(game,&pilot->pitch,active?(py>0.002?274:py< -0.002?273:0):0);
    discovery_key(game,&pilot->fire,active && local[2]>0 && local[2]<speed*20 &&
        fabs(x)<0.03 && fabs(y)<0.03?32:0);
}
