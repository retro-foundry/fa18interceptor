#ifndef FA18_GAME_VIEW_TRANSFORM_H
#define FA18_GAME_VIEW_TRANSFORM_H

/* Rotate a point into view space ($C1F2EE): the three words at `in`,
 * offset by (0, -$28, -$A3), shifted down by `shift`, multiplied by
 * CAMERA_MATRIX (8 fraction bits); three words stored at `out`. */

#include "memory.h"

void view_transform(gaddr in, int16_t shift, gaddr out);

#endif
