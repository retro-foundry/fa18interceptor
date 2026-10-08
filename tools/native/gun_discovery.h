#ifndef FA18_GUN_DISCOVERY_H
#define FA18_GUN_DISCOVERY_H

#include "native/frontend.h"

/* Validation-only telemetry and pilot state; pilot emits host key events. */
typedef struct {
    unsigned trace_tick,scene,tick;
    int pitch,rudder,fire;
    double x,y;
} GunDiscovery;

void gun_discovery_target_local(gaddr record,int64_t local[3]);
void gun_discovery_trace(GunDiscovery *pilot,unsigned tick);
void gun_discovery_tick(GunDiscovery *pilot,NativeFrontend *game,int hit);

#endif
