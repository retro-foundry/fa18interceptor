#ifndef FA18_GAME_ATTITUDE_H
#define FA18_GAME_ATTITUDE_H

/* Attitude flags from two angles ($C122A2). */

#include "memory.h"

/* The two angles in tenths of a degree: the view pan while a context runs
 * (CONTEXT_SELECT), otherwise ATTITUDE_A and ATTITUDE_B. */
void attitude_angles(int16_t *a, int16_t *b);

/* Latch, band and near flags from angle A; STATUS_CA bit 1 when exactly
 * one of the two angles is past 90 degrees from level. */
void update_attitude_flags(void);

#endif
