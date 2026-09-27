#include "indexed_update_record_flag.h"

int fa18_route_indexed_update_record_flag(
    uint8_t record_offset_2,
    FA18IndexedUpdateRecordFlagRoute *route) {
    if (!route) return -1;
    *route = (record_offset_2 & 1u) ?
        FA18_INDEXED_UPDATE_RECORD_FLAG_SET_CONTINUATION :
        FA18_INDEXED_UPDATE_RECORD_FLAG_CLEAR_ROUTE;
    return 0;
}
