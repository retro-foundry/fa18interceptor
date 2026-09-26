#include "polygon_submission.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    const FA18ScreenPairList list = {
        4, {{211, 60}, {216, 60}, {227, 67}, {223, 66}}
    };
    FA18ScreenPairBounds bounds;
    FA18ScreenPolygon polygon = {4, {{211, 60}, {216, 60}, {227, 67}, {223, 66}}};
    FA18ScreenPairList converted;
    assert(fa18_screen_polygon_to_pair_list(&polygon, &converted) == 0);
    assert(converted.count == 4 && converted.pair[2].x == 227);
    assert(fa18_reduce_screen_pair_bounds(&converted, &bounds) == 0);
    assert(bounds.min_x == 211 && bounds.max_x == 227 &&
           bounds.min_y == 60 && bounds.max_y == 67);
    const FA18ViewVertex vertices[] = {
        {51, 30, 40}, {56, 30, 40}, {67, 37, 40}, {63, 36, 40}
    };
    assert(fa18_prepare_projected_submission(vertices, 4, &converted, &bounds) == 0);
    assert(converted.count == 4 && bounds.min_x == 0 && bounds.max_x == 0);
    assert(fa18_reduce_screen_pair_bounds(NULL, &bounds) < 0);
    puts("screen pair bounds contract passed");
    return 0;
}
