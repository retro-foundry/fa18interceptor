#ifndef FA18_NATIVE_FLIGHT_H
#define FA18_NATIVE_FLIGHT_H
#include "frontend.h"
void native_flight_initialize(NativeFrontend *game);
/* Zero means C25312 is waiting for the next host clock sample. */
int native_flight_tick(NativeFrontend *game);
/* C10B90 recorder/root refresh after aircraft selection. */
void native_flight_reset_aircraft(NativeFrontend *game);
#endif
