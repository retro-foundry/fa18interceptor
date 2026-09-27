#ifndef FA18_PLANAR_LANE_PAGE_H
#define FA18_PLANAR_LANE_PAGE_H

#include <stddef.h>
#include <stdint.h>

#include "dimensions.h"

enum {
    FA18_PLANAR_LANE_COUNT = 4,
    FA18_PLANAR_LANE_ROW_BYTES = FA18_WIDTH / 8,
    FA18_PLANAR_LANE_PAGE_BYTES = FA18_PLANAR_LANE_ROW_BYTES * FA18_HEIGHT
};

/* The four lower Copper colour lanes owned by the renderer primitives. */
typedef struct {
    uint8_t *lanes[FA18_PLANAR_LANE_COUNT];
    size_t bytes_per_lane;
} FA18PlanarLanePage;

#endif
