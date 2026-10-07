#ifndef FA18_NATIVE_FLIGHT_H
#define FA18_NATIVE_FLIGHT_H
#include "frontend.h"
void native_flight_initialize(NativeFrontend *game);
/* Modes whose source startup/update composition is connected. */
int native_flight_enabled(const NativeFrontend *game);
enum NativeFlightResult { NATIVE_FLIGHT_WAIT, NATIVE_FLIGHT_COMPLETE, NATIVE_FLIGHT_OWNER_EXIT };
/* WAIT suspends C25312. OWNER_EXIT skips C0EFD4's remaining children. */
/* A menu selection already ran C0FCB4 in this update; its published flight
 * callback belongs to the following update. */
int native_flight_tick(NativeFrontend *game,int stage_already_ran);
/* C10B90 recorder/root refresh after aircraft selection. */
void native_flight_reset_aircraft(NativeFrontend *game);
/* C10BAE shared cancel/reset, including C10A24's smoothing cancel child. */
void native_flight_cancel_context(NativeFrontend *game);
#endif
