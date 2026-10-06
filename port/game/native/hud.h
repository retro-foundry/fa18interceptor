#ifndef FA18_NATIVE_HUD_H
#define FA18_NATIVE_HUD_H
#include "../memory.h"
/* Direct instrument/panel slice of C0EFD4; full frame ordering is pending. */
void native_hud_draw(uint16_t saved_tick);
#endif
