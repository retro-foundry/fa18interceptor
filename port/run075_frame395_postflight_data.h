#ifndef FA18_RUN075_FRAME395_POSTFLIGHT_DATA_H
#define FA18_RUN075_FRAME395_POSTFLIGHT_DATA_H

#include "postflight.h"

enum { FA18_RUN075_FRAME395_POSTFLIGHT_RECORDS = 32 };

/* Bounded $C2F688 entries collected from the run075 frame-395 postflight
 * submission path. Coordinates are the renderer entry coordinates; flags
 * preserve D7 bit zero and the observed adjacent-path selection. */
static const FA18PostflightRecord fa18_run075_frame395_postflight[] = {
    {156, 156, 1}, {158, 167, 0}, {157, 168, 0}, {159, 168, 0},
    {158, 168, 1}, {158, 167, 0}, {156, 156, 0}, {158, 167, 0},
    {156, 156, 1}, {158, 167, 0}, {160, 129, 0}, {159, 129, 0},
    {160, 130, 1}, {159, 130, 0}, {160, 131, 1}, {159, 131, 0},
    {159, 132, 1}, {161, 132, 1}, {159, 71, 0}, {159, 72, 1},
    {159, 74, 1}, {159, 75, 1}, {293, 156, 1}, {295, 156, 0},
    {298, 156, 0}, {300, 156, 0}, {298, 159, 0}, {300, 159, 0},
    {293, 159, 0}, {295, 159, 0}, {303, 159, 0}, {305, 159, 1}
};

#endif
