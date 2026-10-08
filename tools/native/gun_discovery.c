/* Validation-only gun approach telemetry.
 * Never linked into fa18_native or used as game behavior. */
#include "gun_discovery.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>

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
    if(!getenv("FA18_GUN_TRACE") || tick<12500 || tick>=22000 ||
       tick/250==pilot->trace_tick) return;
    pilot->trace_tick=tick/250;
    printf("{\"gun_approach\":true,\"tick\":%u,\"player_angles\":[%u,%u,%u],\"controls\":%u,\"target_velocity\":[%d,%d,%d],\"records\":[",
        tick,rd_u16(CONTROL_RECORDS+102),rd_u16(CONTROL_RECORDS+104),rd_u16(CONTROL_RECORDS+106),
        rd_u8(PLAYER_STICK),
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
