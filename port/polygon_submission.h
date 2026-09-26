#ifndef FA18_POLYGON_SUBMISSION_H
#define FA18_POLYGON_SUBMISSION_H

#include <stdint.h>

#include "projection.h"
#include "line.h"

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

/* Native form of the pair source consumed by `$C3019C`. */
typedef struct {
    uint16_t count;
    uint16_t x[FA18_POLYGON_MAX_VERTICES];
    uint16_t y[FA18_POLYGON_MAX_VERTICES];
} FA18PairSource;

/* Recovered run075 frame559 transition geometry after `$C2FF48` sorting. */
int fa18_build_run075_frame559_pair_source(FA18PairSource *source);

typedef enum {
    FA18_SUBMISSION_SUCCESS = 0,
    FA18_SUBMISSION_FAR_VERTICAL,
    FA18_SUBMISSION_FAR_HORIZONTAL,
    FA18_SUBMISSION_NEAR_LINE,
    FA18_SUBMISSION_OUTSIDE_SLICE
} FA18SubmissionRoute;

typedef struct {
    FA18SubmissionRoute route;
    FA18LineSegment line;
} FA18SubmissionDecision;

int fa18_screen_polygon_to_pair_list(const FA18ScreenPolygon *polygon,
                                     FA18ScreenPairList *list);

/* Convert source fixed point pairs using the observed $18/$1f scales. */
int fa18_build_renderer_pair_list(const FA18PairSource *source,
                                  int16_t horizontal_offset,
                                  int16_t vertical_offset,
                                  FA18ScreenPairList *list);

int fa18_prepare_projected_submission(const FA18ViewVertex *vertices,
                                      uint16_t count,
                                      FA18ScreenPairList *list,
                                      FA18ScreenPairBounds *bounds);

/* `$C301F6`'s proved bounds-reduction stage. */
int fa18_reduce_screen_pair_bounds(const FA18ScreenPairList *list,
                                   FA18ScreenPairBounds *bounds);

int fa18_choose_submission_route(const FA18ScreenPairBounds *bounds,
                                 int16_t display_bound_y,
                                 FA18SubmissionDecision *decision);

int fa18_submit_near_line(const FA18SubmissionDecision *decision,
                          FA18IndexedFrameBuffer *framebuffer,
                          const FA18LineStyle *style,
                          int16_t row_limit);

#endif
