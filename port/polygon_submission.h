#ifndef FA18_POLYGON_SUBMISSION_H
#define FA18_POLYGON_SUBMISSION_H

#include <stdint.h>

#include "projection.h"

/* Semantic replacement for the `$C4B390` screen-pair workspace. */
typedef struct {
    uint16_t count;
    FA18ScreenPoint pair[FA18_POLYGON_MAX_VERTICES];
} FA18ScreenPairList;

typedef struct {
    int16_t min_x;
    int16_t max_x;
    int16_t min_y;
    int16_t max_y;
} FA18ScreenPairBounds;

int fa18_screen_polygon_to_pair_list(const FA18ScreenPolygon *polygon,
                                     FA18ScreenPairList *list);

int fa18_prepare_projected_submission(const FA18ViewVertex *vertices,
                                      uint16_t count,
                                      FA18ScreenPairList *list,
                                      FA18ScreenPairBounds *bounds);

/* `$C301F6`'s proved bounds-reduction stage. */
int fa18_reduce_screen_pair_bounds(const FA18ScreenPairList *list,
                                   FA18ScreenPairBounds *bounds);

#endif
