#include "polygon_perspective.h"

#include <limits.h>

static int project_component(int16_t value, int16_t depth, int16_t scale,
                             int16_t centre, int16_t maximum, int16_t *result) {
    int32_t numerator;
    int32_t quotient;

    if (!result || depth <= 0) return -1;
    numerator = (int32_t)value * scale;
    quotient = numerator / depth; /* 68000 DIVS signed quotient truncates to zero. */
    if (quotient < INT16_MIN || quotient > INT16_MAX) return -1;
    quotient += centre;
    if (quotient < 0) quotient = 0;
    else if (quotient > maximum) quotient = maximum;
    *result = (int16_t)quotient;
    return 0;
}

int fa18_project_polygon_perspective(
    const FA18PolygonPerspectiveTriplet *triplets, uint16_t count,
    FA18PolygonPerspectiveOutput *output) {
    if (!triplets || !output || !output->points || count > output->capacity) return -1;
    output->count = 0;
    if (count <= 2) return 0;

    for (uint16_t index = 0; index < count; ++index) {
        FA18PolygonPerspectivePoint point;
        if (project_component(triplets[index].x, triplets[index].depth,
                              160, 160, 319, &point.x) != 0 ||
            project_component(triplets[index].y, triplets[index].depth,
                              90, 90, 179, &point.y) != 0)
            return 0;
        output->points[index] = point;
    }
    output->count = count;
    return 1;
}
