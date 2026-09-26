#include "polygon_submission.h"

int fa18_reduce_screen_pair_bounds(const FA18ScreenPairList *list,
                                   FA18ScreenPairBounds *bounds) {
    if (!list || !bounds || !list->count ||
        list->count > FA18_POLYGON_MAX_VERTICES) return -1;
    bounds->min_x = bounds->max_x = list->pair[0].x;
    bounds->min_y = bounds->max_y = list->pair[0].y;
    for (uint16_t index = 1; index < list->count; ++index) {
        const FA18ScreenPoint point = list->pair[index];
        if (point.x < bounds->min_x) bounds->min_x = point.x;
        if (point.x > bounds->max_x) bounds->max_x = point.x;
        if (point.y < bounds->min_y) bounds->min_y = point.y;
        if (point.y > bounds->max_y) bounds->max_y = point.y;
    }
    return 0;
}
