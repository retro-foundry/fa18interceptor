#include "indexed_update_record_flag.h"

#include <assert.h>

int main(void) {
    FA18IndexedUpdateRecordFlagRoute route;

    assert(fa18_route_indexed_update_record_flag(0x00, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_RECORD_FLAG_CLEAR_ROUTE);
    assert(fa18_route_indexed_update_record_flag(0xfe, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_RECORD_FLAG_CLEAR_ROUTE);
    assert(fa18_route_indexed_update_record_flag(0x01, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_RECORD_FLAG_SET_CONTINUATION);
    assert(fa18_route_indexed_update_record_flag(0xff, &route) == 0 &&
           route == FA18_INDEXED_UPDATE_RECORD_FLAG_SET_CONTINUATION);
    assert(fa18_route_indexed_update_record_flag(0, 0) == -1);
    return 0;
}
