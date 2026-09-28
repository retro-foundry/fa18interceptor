#ifndef FA18_POLYGON_PERSPECTIVE_H
#define FA18_POLYGON_PERSPECTIVE_H

#include <stdint.h>

/* Typed representation of the `$C24D00-$C24D70` final polygon projection
 * tail.  The preceding clipper owns the triplets; this routine only performs
 * the observed perspective conversion and bounded screen clamp. */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t depth;
} FA18PolygonPerspectiveTriplet;

typedef struct {
    int16_t x;
    int16_t y;
} FA18PolygonPerspectivePoint;

/* Source workspace `$C4B390` permits the caller to retain the count and its
 * screen pairs without importing that address into native state. */
typedef struct {
    uint16_t count;
    FA18PolygonPerspectivePoint *points;
    uint16_t capacity;
} FA18PolygonPerspectiveOutput;

/* `$C24D00-$C24D70`: returns zero when the source rejects fewer than three
 * points or any non-positive depth, one after it submits the converted list,
 * and minus one for invalid native bounds / unrepresentable DIVS input. */
int fa18_project_polygon_perspective(
    const FA18PolygonPerspectiveTriplet *triplets, uint16_t count,
    FA18PolygonPerspectiveOutput *output);

#endif
