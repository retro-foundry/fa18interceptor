#include "indexed_update_gate.h"

int fa18_select_indexed_update_gate(uint16_t selected_record_index,
                                    FA18IndexedUpdateGateRoute *route) {
    if (!route) return -1;
    *route = selected_record_index == 0 ? FA18_INDEXED_UPDATE_ZERO_INDEX_CONTINUATION :
        FA18_INDEXED_UPDATE_NONZERO_INDEX_ROUTE;
    return 0;
}

int fa18_run_indexed_update_gate(uint16_t selected_record_index,
                                 uint8_t stride_state_flag,
                                 FA18IndexedUpdateGateRoute *route) {
    if (fa18_select_indexed_update_gate(selected_record_index, route) != 0) return -1;
    if (selected_record_index == 0 && stride_state_flag)
        *route = FA18_INDEXED_UPDATE_ZERO_INDEX_RETURN;
    return 0;
}
