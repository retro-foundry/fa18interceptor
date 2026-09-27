#include "indexed_update_gate.h"

#include <assert.h>

int main(void) {
    FA18IndexedUpdateGateRoute route;
    assert(fa18_select_indexed_update_gate(0, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_ZERO_INDEX_CONTINUATION);
    assert(fa18_select_indexed_update_gate(1, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_NONZERO_INDEX_ROUTE);
    assert(fa18_select_indexed_update_gate(0, 0) == -1);
    assert(fa18_run_indexed_update_gate(0, 1, 0, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_ZERO_INDEX_RETURN);
    assert(fa18_run_indexed_update_gate(0, 0, 0, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_ZERO_INDEX_NORMAL);
    assert(fa18_run_indexed_update_gate(0, 0, 1, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_ZERO_INDEX_POSTFLIGHT);
    assert(fa18_run_indexed_update_gate(2, 1, 1, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_NONZERO_INDEX_ROUTE);
    return 0;
}
