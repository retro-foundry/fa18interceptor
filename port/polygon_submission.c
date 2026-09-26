#include "polygon_submission.h"

static int absolute_difference(int16_t first, int16_t second) {
    int difference = (int)second - first;
    return difference < 0 ? -difference : difference;
}

int fa18_screen_polygon_to_pair_list(const FA18ScreenPolygon *polygon,
                                     FA18ScreenPairList *list) {
    if (!polygon || !list || polygon->count > FA18_POLYGON_MAX_VERTICES) return -1;
    list->count = polygon->count;
    for (uint16_t index = 0; index < polygon->count; ++index) {
        list->pair[index] = polygon->point[index];
    }
    return 0;
}

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

int fa18_choose_submission_route(const FA18ScreenPairBounds *bounds,
                                 int16_t display_bound_y,
                                 FA18SubmissionDecision *decision) {
    if (!bounds || !decision) return -1;
    decision->line = (FA18LineSegment){bounds->min_x, bounds->min_y,
                                       bounds->max_x, bounds->max_y};
    if (bounds->min_y > display_bound_y) {
        decision->route = FA18_SUBMISSION_SUCCESS;
        return 0;
    }
    const int vertical = absolute_difference(bounds->min_y, bounds->max_y);
    if (vertical > 2) {
        decision->route = FA18_SUBMISSION_FAR_VERTICAL;
        return 0;
    }
    const int horizontal = absolute_difference(bounds->min_x, bounds->max_x);
    if (vertical > 1) {
        if (horizontal > 2) decision->route = FA18_SUBMISSION_FAR_HORIZONTAL;
        else {
            decision->route = FA18_SUBMISSION_NEAR_LINE;
            decision->line.y0++;
        }
    } else if (horizontal <= 1) {
        decision->route = FA18_SUBMISSION_OUTSIDE_SLICE;
    } else {
        decision->route = FA18_SUBMISSION_NEAR_LINE;
    }
    if (decision->route == FA18_SUBMISSION_NEAR_LINE &&
        decision->line.y0 > display_bound_y) {
        decision->route = FA18_SUBMISSION_SUCCESS;
    }
    return 0;
}

int fa18_submit_near_line(const FA18SubmissionDecision *decision,
                          FA18IndexedFrameBuffer *framebuffer,
                          const FA18LineStyle *style,
                          int16_t row_limit) {
    if (!decision || !framebuffer || !style) return -1;
    if (decision->route != FA18_SUBMISSION_NEAR_LINE) return 1;
    return fa18_draw_line(framebuffer, style, decision->line, row_limit);
}

int fa18_prepare_projected_submission(const FA18ViewVertex *vertices,
                                      uint16_t count,
                                      FA18ScreenPairList *list,
                                      FA18ScreenPairBounds *bounds) {
    FA18ScreenPolygon polygon;
    if (!list || !bounds || fa18_project_polygon(vertices, count, &polygon) <= 0) {
        return -1;
    }
    if (fa18_screen_polygon_to_pair_list(&polygon, list) != 0 ||
        fa18_reduce_screen_pair_bounds(list, bounds) != 0) return -1;
    return 0;
}
