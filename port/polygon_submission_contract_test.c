#include "polygon_submission.h"

#include <assert.h>
#include <stdio.h>

int main(void) {
    const FA18ScreenPairList list = {
        4, {{211, 60}, {216, 60}, {227, 67}, {223, 66}}
    };
    FA18ScreenPairBounds bounds;
    assert(fa18_reduce_screen_pair_bounds(&list, &bounds) == 0);
    assert(bounds.min_x == 211 && bounds.max_x == 227 &&
           bounds.min_y == 60 && bounds.max_y == 67);
    assert(fa18_reduce_screen_pair_bounds(NULL, &bounds) < 0);
    puts("screen pair bounds contract passed");
    return 0;
}
