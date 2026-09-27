#include "indexed_update_gate.h"

#include <assert.h>

int main(void) {
    FA18IndexedUpdateGateRoute route;
    assert(fa18_select_indexed_update_gate(0, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_ZERO_INDEX_ROUTE);
    assert(fa18_select_indexed_update_gate(1, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_NONZERO_INDEX_ROUTE);
    assert(fa18_select_indexed_update_gate(0, 0) == -1);
    return 0;
}
