#ifndef FA18_INDEXED_UPDATE_RECORD_FLAG_H
#define FA18_INDEXED_UPDATE_RECORD_FLAG_H

#include <stdint.h>

typedef enum {
    FA18_INDEXED_UPDATE_RECORD_FLAG_CLEAR_ROUTE,
    FA18_INDEXED_UPDATE_RECORD_FLAG_SET_CONTINUATION
} FA18IndexedUpdateRecordFlagRoute;

/* `$C25C3E-$C25C45`: test bit 0 of the selected record byte at +2. */
int fa18_route_indexed_update_record_flag(
    uint8_t record_offset_2,
    FA18IndexedUpdateRecordFlagRoute *route);

#endif
