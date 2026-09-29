#ifndef FA18_GAME_PROJECTION_H
#define FA18_GAME_PROJECTION_H

#include <stdint.h>

/* Project a signed view-space point to the reflected screen pair
 * ($C2EC90); return whether it was accepted. */
int project_view_point(int16_t x, int16_t y, int16_t depth);

/* The adjacent entries project and choose a pixel, pair, block, or filled
 * circle from their mode and size ($C2EC94/$C2EC9C/$C2ECA4). */
int project_view_point_mode(int16_t x, int16_t y, int16_t depth,
                            int16_t mode, int16_t size, int16_t radius);

#endif
