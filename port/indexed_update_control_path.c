#include "indexed_update_control_path.h"

int fa18_route_indexed_update_control_path(
    uint16_t matrix_side_control_flags,
    uint16_t selected_record_index,
    FA18IndexedUpdateControlPathRoute *route) {
    if (!route) return -1;
    if (!(matrix_side_control_flags & 0x0040u))
        *route = FA18_INDEXED_UPDATE_MATRIX_DISPATCH_ROUTE;
    else if (selected_record_index == 0)
        *route = FA18_INDEXED_UPDATE_ZERO_INDEX_CONTROL_STAGE;
    else
        *route = FA18_INDEXED_UPDATE_NONZERO_INDEX_CONTINUATION;
    return 0;
}
