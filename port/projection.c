#include "projection.h"

static int16_t clamp_i16(int32_t value, int16_t low, int16_t high) {
    if (value < low) return low;
    if (value > high) return high;
    return (int16_t)value;
}

int fa18_project_polygon(const FA18ViewVertex *vertices, uint16_t count,
                         FA18ScreenPolygon *polygon) {
    if (!vertices || !polygon || count > FA18_POLYGON_MAX_VERTICES) return -1;
    if (!count) {
        polygon->count = 0;
        return 0;
    }
    if (count < 3u) return -1;
    for (uint16_t index = 0; index < count; ++index) {
        const FA18ViewVertex vertex = vertices[index];
        if (vertex.depth <= 0) {
            polygon->count = 0;
            return 0;
        }
        const int16_t x = clamp_i16(160 + ((int32_t)vertex.x * 160) / vertex.depth,
                                    0, 319);
        const int16_t y = clamp_i16(90 + ((int32_t)vertex.y * 90) / vertex.depth,
                                    0, 179);
        polygon->point[index].x = (int16_t)(319 - x);
        polygon->point[index].y = (int16_t)(179 - y);
    }
    polygon->count = count;
    return 1;
}
