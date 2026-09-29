#ifndef FA18_GAME_TRACKING_H
#define FA18_GAME_TRACKING_H

/* Turning a pair of angles toward a direction. Angles are in 1/80 degree:
 * a turn is $7080. */

#include <stdint.h>

/* Turn `*elevation` and `*azimuth` toward the direction (x, y, z) ($C123FA):
 * the azimuth from x and z, the elevation from y against the horizontal
 * distance, both through ARCTAN_TABLE. Each moves a quarter of the way,
 * the short way round and at most `max_step`; it snaps to the direction
 * when `max_step` is negative or on the first call (TRACK_STARTED and
 * CONTEXT_STATE clear). The results' low words go to TRACKED_PITCH and
 * TRACKED_HEADING. */
void track_direction(int32_t *elevation, int32_t *azimuth, int32_t x, int32_t y, int32_t z, int32_t max_step);

#endif
