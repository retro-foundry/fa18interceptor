#ifndef FA18_PROJECTION_H
#define FA18_PROJECTION_H

#include <stdint.h>

enum { FA18_POLYGON_MAX_VERTICES = 16 };

typedef struct {
    int16_t x;
    int16_t y;
    int16_t depth;
} FA18ViewVertex;

typedef struct {
    int16_t x;
    int16_t y;
} FA18ScreenPoint;

typedef struct {
    uint16_t count;
    FA18ScreenPoint point[FA18_POLYGON_MAX_VERTICES];
} FA18ScreenPolygon;

/* `$C24CFE`'s proved 3+-tuple projection path. Returns 1 for a projected
 * polygon, 0 for source nonpositive-depth rejection, and -1 for native input
 * rejection. The original 1-2 tuple branch remains a separate contract. */
int fa18_project_polygon(const FA18ViewVertex *vertices, uint16_t count,
                         FA18ScreenPolygon *polygon);

#endif
