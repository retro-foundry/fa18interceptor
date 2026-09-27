#include "indexed_update_selected_record_gate.h"

#include <assert.h>

int main(void) {
    FA18IndexedUpdateSelectedRecordRoute route;

    assert(fa18_route_indexed_update_selected_record(2, 3, 0, 1, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_SELECTOR_GATE_ROUTE);
    assert(fa18_route_indexed_update_selected_record(2, 2, 1, 1, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_SELECTOR_GATE_ROUTE);
    assert(fa18_route_indexed_update_selected_record(2, 2, 0, 0, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_SELECTOR_GATE_ROUTE);
    assert(fa18_route_indexed_update_selected_record(2, 2, 0, 3, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_SELECTED_RECORD_CONTINUATION);
    assert(fa18_route_indexed_update_selected_record(0, 0, 0, 1, 0) == -1);
    return 0;
}
