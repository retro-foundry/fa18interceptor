#ifndef FA18_GUN_DISCOVERY_H
#define FA18_GUN_DISCOVERY_H

#include "native/frontend.h"

/* Read-only validation telemetry; never supplied to game behavior. */
typedef struct {
    unsigned trace_tick;
} GunDiscovery;

void gun_discovery_target_local(gaddr record,int64_t local[3]);
void gun_discovery_trace(GunDiscovery *pilot,unsigned tick);

#endif
