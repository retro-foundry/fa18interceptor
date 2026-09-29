#ifndef FA18_GAME_PROJECTION_H
#define FA18_GAME_PROJECTION_H

#include <stdint.h>

/* Project a signed view-space point to the reflected screen pair
 * ($C2EC90); return whether it was accepted. */
int project_view_point(int16_t x, int16_t y, int16_t depth);

#endif
