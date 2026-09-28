#ifndef FA18_COORDINATE_UPDATE_POSITIVE_PAIR_H
#define FA18_COORDINATE_UPDATE_POSITIVE_PAIR_H

#include "coordinate_update_negative_pair.h"
#include "scene_dispatch_coordinate_pose.h"

/* `$C123FA-$C1294E`, observed `$C2889E` route: the six source arguments are
 * `(0,0,positive,0,positive,-1)`.  It follows the measured max-component,
 * shift-14, two table-ratio, magnitude, sign, clamp, and output-publish path.
 * Other `$C123FA` branches are not represented by this bounded adapter. */
int fa18_update_coordinate_positive_pair(
    const FA18CoordinateAngleTable *table,
    const FA18SceneCoordinateUpdateInput *input, int16_t output[2]);

#endif
