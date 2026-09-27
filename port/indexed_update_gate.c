#include "indexed_update_gate.h"

int fa18_select_indexed_update_gate(uint16_t selected_record_index,
                                    FA18IndexedUpdateGateRoute *route) {
    if (!route) return -1;
    *route = selected_record_index == 0 ? FA18_INDEXED_UPDATE_ZERO_INDEX_ROUTE :
        FA18_INDEXED_UPDATE_NONZERO_INDEX_ROUTE;
    return 0;
}
