#include "indexed_update_selected_record_gate.h"

int fa18_route_indexed_update_selected_record(
    uint16_t selected_record_index,
    uint16_t reference_record_index,
    uint8_t indexed_update_context,
    uint8_t record_offset_3,
    FA18IndexedUpdateSelectedRecordRoute *route) {
    if (!route) return -1;
    *route = selected_record_index != reference_record_index ||
             indexed_update_context != 0 || !(record_offset_3 & 1u) ?
        FA18_INDEXED_UPDATE_SELECTOR_GATE_ROUTE :
        FA18_INDEXED_UPDATE_SELECTED_RECORD_CONTINUATION;
    return 0;
}
