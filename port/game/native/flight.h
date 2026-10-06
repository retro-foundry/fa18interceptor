#ifndef FA18_NATIVE_FLIGHT_H
#define FA18_NATIVE_FLIGHT_H
#include "frontend.h"
void native_flight_initialize(NativeFrontend *game);
void native_flight_tick(NativeFrontend *game);
void native_flight_refresh_cockpit(NativeFrontend *game);
#endif
