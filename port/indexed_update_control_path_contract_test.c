#include "indexed_update_control_path.h"

#include <assert.h>

int main(void) {
    FA18IndexedUpdateControlPathRoute route;

    assert(fa18_route_indexed_update_control_path(0x0000, 0, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_MATRIX_DISPATCH_ROUTE);
    assert(fa18_route_indexed_update_control_path(0xffbf, 9, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_MATRIX_DISPATCH_ROUTE);
    assert(fa18_route_indexed_update_control_path(0x0040, 0, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_ZERO_INDEX_CONTROL_STAGE);
    assert(fa18_route_indexed_update_control_path(0x8040, 2, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_NONZERO_INDEX_CONTINUATION);
    assert(fa18_route_indexed_update_control_path(0, 0, 0) == -1);
    return 0;
}
