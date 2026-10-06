#ifndef FA18_NATIVE_FLIGHT_H
#define FA18_NATIVE_FLIGHT_H
#include "frontend.h"
void native_flight_initialize(NativeFrontend *game);
/* Modes whose source startup/update composition is connected. */
int native_flight_enabled(const NativeFrontend *game);
/* Zero means C25312 is waiting for the next host clock sample. */
/* A menu selection already ran C0FCB4 in this update; its published flight
 * callback belongs to the following update. */
int native_flight_tick(NativeFrontend *game,int stage_already_ran);
/* C10B90 recorder/root refresh after aircraft selection. */
void native_flight_reset_aircraft(NativeFrontend *game);
#endif
