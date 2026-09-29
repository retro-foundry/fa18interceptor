#ifndef FA18_GAME_HUD_STORES_H
#define FA18_GAME_HUD_STORES_H

#include "memory.h"

/* $C30AE2: draw the vertical marks and bottom pixels from an (x,y) word
 * stream, terminated by a negative x. The caller supplies D4's full value:
 * its upper bits affect the colour at the last count. Returns the cursor
 * after the terminator and writes the final D4 value through `remaining`. */
gaddr draw_stores_icon_stream(gaddr points, uint32_t *remaining);

/* $C30A00: draw the selected record's centre mark and two store streams.
 * `count_state` is D4 after the centre pixel plot; `status` is D5.b at the
 * same point (which can come from the caller when the centre is offscreen). */
void draw_stores_icons(uint32_t count_state, uint8_t status);

#endif
